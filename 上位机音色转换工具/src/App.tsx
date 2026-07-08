import { useCallback, useEffect, useMemo, useRef, useState } from "react";
import { invoke } from "@tauri-apps/api/core";
import { listen } from "@tauri-apps/api/event";
import { getCurrentWindow } from "@tauri-apps/api/window";
import { open } from "@tauri-apps/plugin-dialog";
import { openPath } from "@tauri-apps/plugin-opener";
import {
  CheckCircle2,
  CircleAlert,
  FileAudio,
  FolderOpen,
  LoaderCircle,
  Play,
  Plus,
  RotateCcw,
  Trash2,
} from "lucide-react";
import "./App.css";

type FileStatus = "queued" | "working" | "done" | "error";
type Mode = "normal" | "pitch";

type AudioItem = {
  path: string;
  name: string;
  status: FileStatus;
  output?: string;
  message?: string;
};

type PitchSlot = AudioItem & {
  id: number;
  label: string;
  note: string;
  octave: number;
};

type FfmpegInfo = {
  available: boolean;
  path: string;
  version: string;
};

type ConversionStarted = {
  input: string;
  output: string;
};

type ConversionResult = {
  input: string;
  output?: string;
  ok: boolean;
  message: string;
};

type DropPosition = {
  x: number;
  y: number;
};

type PointerDrag = {
  sourceId: number;
  pointerId: number;
  startX: number;
  startY: number;
  active: boolean;
};

const audioExtensions = [
  "aac",
  "aiff",
  "alac",
  "ape",
  "flac",
  "m4a",
  "mka",
  "mp3",
  "ogg",
  "opus",
  "wav",
  "wma",
];

const notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
const octaves = [0, 1, 2, 3, 4, 5, 6, 7, 8];

function getDemoMode() {
  return new URLSearchParams(window.location.search).get("demo") ?? "";
}

function createPitchSlots(includeDemoFiles = false): PitchSlot[] {
  const demoFiles = new Map<number, string>([
    [48, "piano_C4.wav"],
    [51, "piano_D#4.wav"],
    [55, "pad_G4.flac"],
    [60, "lead_C5.mp3"],
    [63, "lead_D#5.wav"],
    [67, "bass_G5.aiff"],
  ]);

  return octaves.flatMap((octave) =>
    notes.map((note) => {
      const label = `${note}${octave}`;
      const id = octaves.indexOf(octave) * notes.length + notes.indexOf(note);
      const demoName = demoFiles.get(id);

      return {
        id,
        label,
        note,
        octave,
        path: includeDemoFiles && demoName ? `D:\\Samples\\${demoName}` : "",
        name: includeDemoFiles && demoName ? demoName : "",
        status: "queued" as FileStatus,
      };
    }),
  );
}

function basename(path: string) {
  return path.split(/[\\/]/).filter(Boolean).pop() ?? path;
}

function dirname(path: string) {
  const index = Math.max(path.lastIndexOf("\\"), path.lastIndexOf("/"));
  return index > 0 ? path.slice(0, index) : path;
}

function statusLabel(status: FileStatus) {
  if (status === "working") return "转换中";
  if (status === "done") return "完成";
  if (status === "error") return "失败";
  return "队列";
}

function getDefaultStartId() {
  return createPitchSlots().find((slot) => slot.label === "C4")?.id ?? 0;
}

function isTauriRuntime() {
  return "__TAURI_INTERNALS__" in window;
}

function findPitchSlotIdAtPosition(position: DropPosition | undefined, scaleFactor: number) {
  if (!position) return null;

  const element = document.elementFromPoint(position.x / scaleFactor, position.y / scaleFactor);
  const slotElement = element?.closest("[data-pitch-slot-id]");
  const slotId = slotElement?.getAttribute("data-pitch-slot-id");
  return slotId ? Number(slotId) : null;
}

function findPitchSlotIdAtClientPosition(x: number, y: number) {
  const element = document.elementFromPoint(x, y);
  const slotElement = element?.closest("[data-pitch-slot-id]");
  const slotId = slotElement?.getAttribute("data-pitch-slot-id");
  return slotId ? Number(slotId) : null;
}

function App() {
  const [mode, setMode] = useState<Mode>(() =>
    getDemoMode().startsWith("pitch") ? "pitch" : "normal",
  );
  const [items, setItems] = useState<AudioItem[]>([]);
  const [pitchSlots, setPitchSlots] = useState<PitchSlot[]>(() =>
    createPitchSlots(getDemoMode() === "pitch-mapped"),
  );
  const [batchStartId, setBatchStartId] = useState(getDefaultStartId);
  const [draggedSlotId, setDraggedSlotId] = useState<number | null>(null);
  const [hoverSlotId, setHoverSlotId] = useState<number | null>(null);
  const [scaleFactor, setScaleFactor] = useState(1);
  const pointerDragRef = useRef<PointerDrag | null>(null);
  const suppressNextClickRef = useRef(false);
  const [outputDir, setOutputDir] = useState<string>("");
  const [overwrite, setOverwrite] = useState(false);
  const [isConverting, setIsConverting] = useState(false);
  const [ffmpeg, setFfmpeg] = useState<FfmpegInfo | null>(null);
  const [appError, setAppError] = useState("");

  const filledPitchSlots = useMemo(() => pitchSlots.filter((slot) => slot.path), [pitchSlots]);
  const activeEntries = mode === "pitch" ? filledPitchSlots : items;
  const completedCount = useMemo(
    () => activeEntries.filter((item) => item.status === "done").length,
    [activeEntries],
  );
  const failedCount = useMemo(
    () => activeEntries.filter((item) => item.status === "error").length,
    [activeEntries],
  );

  const addNormalPaths = useCallback((paths: string[]) => {
    const uniquePaths = paths.filter(Boolean);
    if (!uniquePaths.length) return;

    setItems((current) => {
      const existing = new Set(current.map((item) => item.path));
      const next = uniquePaths
        .filter((path) => !existing.has(path))
        .map((path) => ({
          path,
          name: basename(path),
          status: "queued" as FileStatus,
        }));

      return [...current, ...next];
    });
  }, []);

  const fillPitchSlots = useCallback((paths: string[], startId: number) => {
    const nextPaths = paths.filter(Boolean);
    if (!nextPaths.length) return;

    setPitchSlots((current) => {
      const next = current.map((slot) => ({ ...slot }));
      let pathIndex = 0;

      for (const slot of next) {
        if (slot.id < startId || pathIndex >= nextPaths.length) continue;
        const path = nextPaths[pathIndex];
        slot.path = path;
        slot.name = basename(path);
        slot.status = "queued";
        slot.output = undefined;
        slot.message = "";
        pathIndex += 1;
      }

      return next;
    });
  }, []);

  const setPitchSlotFile = useCallback((slotId: number, path: string) => {
    setPitchSlots((current) =>
      current.map((slot) =>
        slot.id === slotId
          ? {
              ...slot,
              path,
              name: basename(path),
              status: "queued",
              output: undefined,
              message: "",
            }
          : slot,
      ),
    );
  }, []);

  const movePitchSlot = useCallback((sourceId: number, targetId: number) => {
    if (sourceId === targetId) return;

    setPitchSlots((current) => {
      const source = current.find((slot) => slot.id === sourceId);
      const target = current.find((slot) => slot.id === targetId);
      if (!source?.path || !target) return current;

      return current.map((slot) => {
        if (slot.id === sourceId) {
          return {
            ...slot,
            path: target.path,
            name: target.name,
            status: target.path ? "queued" : "queued",
            output: undefined,
            message: "",
          };
        }

        if (slot.id === targetId) {
          return {
            ...slot,
            path: source.path,
            name: source.name,
            status: "queued",
            output: undefined,
            message: "",
          };
        }

        return slot;
      });
    });
  }, []);

  const finishPointerDrag = useCallback(
    (clientX: number, clientY: number) => {
      const drag = pointerDragRef.current;

      if (drag?.active) {
        const targetSlotId = findPitchSlotIdAtClientPosition(clientX, clientY);
        if (targetSlotId !== null) {
          movePitchSlot(drag.sourceId, targetSlotId);
        }
        suppressNextClickRef.current = true;
      }

      pointerDragRef.current = null;
      setDraggedSlotId(null);
      setHoverSlotId(null);
    },
    [movePitchSlot],
  );

  const addPaths = useCallback(
    (paths: string[]) => {
      if (mode === "pitch") {
        fillPitchSlots(paths, batchStartId);
      } else {
        addNormalPaths(paths);
      }
    },
    [addNormalPaths, batchStartId, fillPitchSlots, mode],
  );

  useEffect(() => {
    if (!isTauriRuntime()) {
      setFfmpeg({
        available: true,
        path: "ffmpeg",
        version: "FFmpeg 已内置",
      });
      return;
    }

    invoke<FfmpegInfo>("check_ffmpeg")
      .then(setFfmpeg)
      .catch((error) =>
        setFfmpeg({
          available: false,
          path: "ffmpeg",
          version: String(error),
        }),
      );
  }, []);

  useEffect(() => {
    if (!isTauriRuntime()) return;

    getCurrentWindow()
      .scaleFactor()
      .then(setScaleFactor)
      .catch(() => setScaleFactor(1));
  }, []);

  useEffect(() => {
    if (!isTauriRuntime()) return;

    let cleanupDrop: (() => void) | undefined;

    getCurrentWindow()
      .onDragDropEvent((event) => {
        if (event.payload.type === "drop") {
          if (mode === "pitch") {
            const targetSlotId = findPitchSlotIdAtPosition(
              "position" in event.payload ? event.payload.position : undefined,
              scaleFactor,
            );
            if (targetSlotId !== null) {
              fillPitchSlots(event.payload.paths, targetSlotId);
              setHoverSlotId(null);
              return;
            }
          }

          addPaths(event.payload.paths);
          setHoverSlotId(null);
        }

        if (event.payload.type === "over" && mode === "pitch") {
          const targetSlotId = findPitchSlotIdAtPosition(event.payload.position, scaleFactor);
          setHoverSlotId(targetSlotId);
        }

        if (event.payload.type === "leave") {
          setHoverSlotId(null);
        }
      })
      .then((unlisten) => {
        cleanupDrop = unlisten;
      });

    return () => cleanupDrop?.();
  }, [addPaths, fillPitchSlots, mode, scaleFactor]);

  useEffect(() => {
    let cleanupStarted: (() => void) | undefined;
    let cleanupFinished: (() => void) | undefined;

    listen<ConversionStarted>("conversion-started", (event) => {
      setItems((current) =>
        current.map((item) =>
          item.path === event.payload.input
            ? {
                ...item,
                status: "working",
                output: event.payload.output,
                message: "",
              }
            : item,
        ),
      );

      setPitchSlots((current) =>
        current.map((slot) =>
          slot.path === event.payload.input
            ? {
                ...slot,
                status: "working",
                output: event.payload.output,
                message: "",
              }
            : slot,
        ),
      );
    }).then((unlisten) => {
      cleanupStarted = unlisten;
    });

    listen<ConversionResult>("conversion-finished", (event) => {
      setItems((current) =>
        current.map((item) =>
          item.path === event.payload.input
            ? {
                ...item,
                status: event.payload.ok ? "done" : "error",
                output: event.payload.output,
                message: event.payload.message,
              }
            : item,
        ),
      );

      setPitchSlots((current) =>
        current.map((slot) =>
          slot.path === event.payload.input
            ? {
                ...slot,
                status: event.payload.ok ? "done" : "error",
                output: event.payload.output,
                message: event.payload.message,
              }
            : slot,
        ),
      );
    }).then((unlisten) => {
      cleanupFinished = unlisten;
    });

    return () => {
      cleanupStarted?.();
      cleanupFinished?.();
    };
  }, []);

  async function chooseFiles() {
    if (!isTauriRuntime()) {
      addPaths([
        "D:\\Samples\\piano_C4.wav",
        "D:\\Samples\\piano_D4.wav",
        "D:\\Samples\\piano_E4.wav",
      ]);
      return;
    }

    const selected = await open({
      multiple: true,
      directory: false,
      filters: [{ name: "音频文件", extensions: audioExtensions }],
    });

    if (Array.isArray(selected)) addPaths(selected);
    if (typeof selected === "string") addPaths([selected]);
  }

  async function chooseSlotFile(slotId: number) {
    if (!isTauriRuntime()) {
      setPitchSlotFile(slotId, `D:\\Samples\\${pitchSlots[slotId]?.label ?? "sample"}.wav`);
      return;
    }

    const selected = await open({
      multiple: false,
      directory: false,
      filters: [{ name: "音频文件", extensions: audioExtensions }],
    });

    if (typeof selected !== "string") return;

    setPitchSlotFile(slotId, selected);
  }

  async function chooseOutputDir() {
    if (!isTauriRuntime()) {
      setOutputDir("D:\\SynthPCM");
      return;
    }

    const selected = await open({
      multiple: false,
      directory: true,
    });

    if (typeof selected === "string") {
      setOutputDir(selected);
    }
  }

  async function convertFiles() {
    if (isConverting) return;

    if (mode === "pitch") {
      await convertPitchFiles();
      return;
    }

    if (!items.length) return;

    setAppError("");
    setIsConverting(true);
    setItems((current) =>
      current.map((item) => ({
        ...item,
        status: "queued",
        message: "",
        output: undefined,
      })),
    );

    try {
      await invoke<ConversionResult[]>("convert_audio_files", {
        request: {
          inputs: items.map((item) => item.path),
          outputDir: outputDir || null,
          overwrite,
        },
      });
    } catch (error) {
      setAppError(String(error));
    } finally {
      setIsConverting(false);
    }
  }

  async function convertPitchFiles() {
    if (!filledPitchSlots.length) {
      setAppError("请先往音高表里放入音频文件");
      return;
    }

    setAppError("");
    setIsConverting(true);
    setPitchSlots((current) =>
      current.map((slot) => ({
        ...slot,
        status: slot.path ? "queued" : slot.status,
        message: "",
        output: undefined,
      })),
    );

    try {
      await invoke<ConversionResult[]>("convert_audio_files", {
        request: {
          jobs: filledPitchSlots.map((slot) => ({
            input: slot.path,
            outputName: `${slot.label}.pcm`,
          })),
          outputDir: outputDir || null,
          overwrite,
        },
      });
    } catch (error) {
      setAppError(String(error));
    } finally {
      setIsConverting(false);
    }
  }

  function removeItem(path: string) {
    setItems((current) => current.filter((item) => item.path !== path));
  }

  function clearSlot(slotId: number) {
    setPitchSlots((current) =>
      current.map((slot) =>
        slot.id === slotId
          ? {
              ...slot,
              path: "",
              name: "",
              status: "queued",
              output: undefined,
              message: "",
            }
          : slot,
      ),
    );
  }

  function resetQueue() {
    if (mode === "pitch") {
      setPitchSlots(createPitchSlots());
    } else {
      setItems([]);
    }
    setAppError("");
  }

  const convertDisabled =
    isConverting ||
    ffmpeg?.available === false ||
    (mode === "pitch" ? filledPitchSlots.length === 0 : items.length === 0);

  return (
    <main className="app-shell">
      <section className="toolbar">
        <div>
          <h1>合成器配套音色转换工具</h1>
          <p className={ffmpeg?.available ? "probe ok" : "probe"}>
            {ffmpeg?.available
              ? `输出裸 PCM：48 kHz / 16-bit / stereo · ${ffmpeg.version}`
              : ffmpeg
                ? ffmpeg.version
                : "检测 FFmpeg"}
          </p>
        </div>

        <div className="toolbar-actions">
          <button type="button" className="icon-button" onClick={chooseFiles} title="添加文件">
            <Plus size={18} />
          </button>
          <button
            type="button"
            className="icon-button"
            onClick={chooseOutputDir}
            title="输出目录"
          >
            <FolderOpen size={18} />
          </button>
          <button type="button" className="icon-button" onClick={resetQueue} title="清空">
            <Trash2 size={18} />
          </button>
        </div>
      </section>

      <section className="mode-row">
        <div className="segmented">
          <button
            type="button"
            className={mode === "normal" ? "active" : ""}
            onClick={() => {
              setMode("normal");
              setAppError("");
            }}
          >
            普通转换
          </button>
          <button
            type="button"
            className={mode === "pitch" ? "active" : ""}
            onClick={() => {
              setMode("pitch");
              setAppError("");
            }}
          >
            音高映射
          </button>
        </div>
        <p className="mode-hint">
          {mode === "pitch"
            ? `已映射 ${filledPitchSlots.length} 个音高`
            : "按源文件名批量导出 PCM"}
        </p>
      </section>

      <section className="settings-row">
        <button type="button" className="path-button" onClick={chooseOutputDir}>
          <FolderOpen size={16} />
          <span>{outputDir || "源文件目录"}</span>
        </button>

        <label className="toggle">
          <input
            type="checkbox"
            checked={overwrite}
            onChange={(event) => setOverwrite(event.currentTarget.checked)}
          />
          <span>覆盖同名文件</span>
        </label>
      </section>

      {mode === "pitch" ? (
        <section className="pitch-panel">
          <div className="pitch-actions">
            <div className="batch-start">
              <span>批量起点</span>
              <select
                value={batchStartId}
                onChange={(event) => setBatchStartId(Number(event.currentTarget.value))}
                disabled={isConverting}
              >
                {pitchSlots.map((slot) => (
                  <option value={slot.id} key={slot.id}>
                    {slot.label}
                  </option>
                ))}
              </select>
            </div>
            <button type="button" className="secondary-button" onClick={chooseFiles}>
              <Plus size={16} />
              批量导入
            </button>
          </div>

          <div className="pitch-table">
            <div className="pitch-corner" />
            {octaves.map((octave) => (
              <div className="pitch-head" key={octave}>
                {octave}
              </div>
            ))}
            {notes.map((note) => (
              <div className="pitch-row" key={note}>
                <div className="pitch-note">{note}</div>
                {octaves.map((octave) => {
                  const slot = pitchSlots.find(
                    (candidate) => candidate.note === note && candidate.octave === octave,
                  );
                  if (!slot) return null;

                  return (
                    <div
                      className={`pitch-cell ${slot.path ? "filled" : ""} ${
                        hoverSlotId === slot.id ? "drop-hover" : ""
                      } ${draggedSlotId === slot.id ? "dragging" : ""}`}
                      data-pitch-slot-id={slot.id}
                      key={slot.id}
                      onPointerDown={(event) => {
                        const target = event.target as Element;
                        if (
                          isConverting ||
                          !slot.path ||
                          event.button !== 0 ||
                          target.closest(".mini-button")
                        ) {
                          return;
                        }

                        pointerDragRef.current = {
                          sourceId: slot.id,
                          pointerId: event.pointerId,
                          startX: event.clientX,
                          startY: event.clientY,
                          active: false,
                        };
                        event.currentTarget.setPointerCapture(event.pointerId);
                      }}
                      onPointerMove={(event) => {
                        const drag = pointerDragRef.current;
                        if (!drag || drag.pointerId !== event.pointerId) return;

                        const moved =
                          Math.abs(event.clientX - drag.startX) > 5 ||
                          Math.abs(event.clientY - drag.startY) > 5;

                        if (!drag.active && moved) {
                          drag.active = true;
                          suppressNextClickRef.current = true;
                          setDraggedSlotId(drag.sourceId);
                        }

                        if (drag.active) {
                          event.preventDefault();
                          setHoverSlotId(
                            findPitchSlotIdAtClientPosition(event.clientX, event.clientY),
                          );
                        }
                      }}
                      onPointerUp={(event) => {
                        const drag = pointerDragRef.current;
                        if (!drag || drag.pointerId !== event.pointerId) return;

                        event.currentTarget.releasePointerCapture(event.pointerId);
                        finishPointerDrag(event.clientX, event.clientY);
                      }}
                      onPointerCancel={(event) => {
                        const drag = pointerDragRef.current;
                        if (!drag || drag.pointerId !== event.pointerId) return;

                        pointerDragRef.current = null;
                        setDraggedSlotId(null);
                        setHoverSlotId(null);
                      }}
                    >
                      <button
                        type="button"
                        className="pitch-cell-main"
                        onClick={(event) => {
                          if (suppressNextClickRef.current) {
                            suppressNextClickRef.current = false;
                            event.preventDefault();
                            return;
                          }
                          chooseSlotFile(slot.id);
                        }}
                        disabled={isConverting}
                        title={slot.name || slot.label}
                      >
                        <strong>{slot.label}</strong>
                        <span>{slot.name || "空"}</span>
                      </button>

                      <div className="pitch-cell-footer">
                        <small className={`dot ${slot.status}`} title={statusLabel(slot.status)} />
                        {slot.path ? (
                          slot.status === "done" ? (
                            <button
                              type="button"
                              className="mini-button"
                              onClick={() => slot.output && openPath(dirname(slot.output))}
                              onPointerDown={(event) => event.stopPropagation()}
                              title="打开位置"
                            >
                              <CheckCircle2 size={14} />
                            </button>
                          ) : slot.status === "working" ? (
                            <LoaderCircle className="spin mini-icon" size={14} />
                          ) : slot.status === "error" ? (
                            <CircleAlert className="mini-icon error-icon" size={14} />
                          ) : (
                            <button
                              type="button"
                              className="mini-button"
                              onClick={() => clearSlot(slot.id)}
                              onPointerDown={(event) => event.stopPropagation()}
                              title="清除此音高"
                            >
                              <Trash2 size={13} />
                            </button>
                          )
                        ) : null}
                      </div>
                    </div>
                  );
                })}
              </div>
            ))}
          </div>
        </section>
      ) : (
        <section className={`drop-zone ${items.length ? "has-items" : ""}`}>
          {items.length === 0 ? (
            <button type="button" className="empty-action" onClick={chooseFiles}>
              <FileAudio size={38} />
              <span>选择音频文件</span>
            </button>
          ) : (
            <div className="file-list">
              {items.map((item) => (
                <article className="file-row" key={item.path}>
                  <FileAudio className="file-icon" size={20} />
                  <div className="file-main">
                    <div className="file-title">
                      <span>{item.name}</span>
                      <strong className={`status ${item.status}`}>{statusLabel(item.status)}</strong>
                    </div>
                    <p>{item.output ?? item.path}</p>
                    {item.message ? <p className="message">{item.message}</p> : null}
                  </div>
                  {item.status === "working" ? (
                    <LoaderCircle className="spin status-icon" size={19} />
                  ) : item.status === "done" ? (
                    <button
                      type="button"
                      className="icon-button compact"
                      onClick={() => item.output && openPath(dirname(item.output))}
                      title="打开位置"
                    >
                      <CheckCircle2 size={19} />
                    </button>
                  ) : item.status === "error" ? (
                    <CircleAlert className="status-icon error-icon" size={19} />
                  ) : (
                    <button
                      type="button"
                      className="icon-button compact"
                      onClick={() => removeItem(item.path)}
                      title="移除"
                    >
                      <Trash2 size={17} />
                    </button>
                  )}
                </article>
              ))}
            </div>
          )}
        </section>
      )}

      {appError ? <p className="app-error">{appError}</p> : null}

      <footer className="footer-bar">
        <div className="summary">
          <span>{activeEntries.length} 个文件</span>
          <span>{completedCount} 完成</span>
          <span>{failedCount} 失败</span>
        </div>
        <div className="footer-actions">
          <button
            type="button"
            className="secondary-button"
            onClick={() => {
              if (mode === "pitch") {
                setPitchSlots((current) =>
                  current.map((slot) => ({
                    ...slot,
                    status: slot.path ? "queued" : slot.status,
                    message: "",
                    output: undefined,
                  })),
                );
              } else {
                setItems((current) =>
                  current.map((item) => ({
                    ...item,
                    status: "queued",
                    message: "",
                    output: undefined,
                  })),
                );
              }
            }}
            disabled={isConverting || activeEntries.length === 0}
          >
            <RotateCcw size={16} />
            重置
          </button>
          <button
            type="button"
            className="primary-button"
            onClick={convertFiles}
            disabled={convertDisabled}
          >
            {isConverting ? <LoaderCircle className="spin" size={17} /> : <Play size={17} />}
            导出 PCM
          </button>
        </div>
      </footer>
    </main>
  );
}

export default App;

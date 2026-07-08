use serde::{Deserialize, Serialize};
use std::{
    env, fs,
    path::{Path, PathBuf},
    process::{Command, Stdio},
};
use tauri::{path::BaseDirectory, Emitter, Manager};

#[derive(Debug, Deserialize)]
#[serde(rename_all = "camelCase")]
struct ConvertRequest {
    inputs: Option<Vec<String>>,
    jobs: Option<Vec<ConvertJob>>,
    output_dir: Option<String>,
    overwrite: bool,
}

#[derive(Debug, Deserialize)]
#[serde(rename_all = "camelCase")]
struct ConvertJob {
    input: String,
    output_name: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
struct ConversionResult {
    input: String,
    output: Option<String>,
    ok: bool,
    message: String,
}

#[derive(Debug, Serialize)]
#[serde(rename_all = "camelCase")]
struct FfmpegInfo {
    available: bool,
    path: String,
    version: String,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
struct ConversionStarted {
    input: String,
    output: String,
}

#[tauri::command]
fn check_ffmpeg(app: tauri::AppHandle) -> FfmpegInfo {
    match ffmpeg_version(&app) {
        Ok((path, version)) => FfmpegInfo {
            available: true,
            path,
            version,
        },
        Err(message) => FfmpegInfo {
            available: false,
            path: "ffmpeg".to_string(),
            version: message,
        },
    }
}

#[tauri::command]
async fn convert_audio_files(
    app: tauri::AppHandle,
    request: ConvertRequest,
) -> Result<Vec<ConversionResult>, String> {
    let jobs = normalize_jobs(request.inputs, request.jobs);

    if jobs.is_empty() {
        return Err("没有选择音频文件".to_string());
    }

    tauri::async_runtime::spawn_blocking(move || {
        let ffmpeg = find_ffmpeg(&app);
        let mut results = Vec::with_capacity(jobs.len());

        for job in jobs {
            let input_path = PathBuf::from(&job.input);
            let output_path = make_output_path(
                &input_path,
                request.output_dir.as_deref(),
                request.overwrite,
                job.output_name.as_deref(),
            );

            if let Err(message) = app.emit(
                "conversion-started",
                ConversionStarted {
                    input: job.input.clone(),
                    output: output_path.to_string_lossy().to_string(),
                },
            ) {
                eprintln!("failed to emit conversion-started: {message}");
            }

            let result = convert_one(&ffmpeg, &input_path, &output_path, request.overwrite);

            if let Err(message) = app.emit("conversion-finished", result.clone()) {
                eprintln!("failed to emit conversion-finished: {message}");
            }

            results.push(result);
        }

        results
    })
    .await
    .map_err(|error| error.to_string())
}

fn normalize_jobs(inputs: Option<Vec<String>>, jobs: Option<Vec<ConvertJob>>) -> Vec<ConvertJob> {
    if let Some(jobs) = jobs {
        return jobs
            .into_iter()
            .filter(|job| !job.input.trim().is_empty())
            .collect();
    }

    inputs
        .unwrap_or_default()
        .into_iter()
        .filter(|input| !input.trim().is_empty())
        .map(|input| ConvertJob {
            input,
            output_name: None,
        })
        .collect()
}

fn convert_one(
    ffmpeg: &Path,
    input_path: &Path,
    output_path: &Path,
    overwrite: bool,
) -> ConversionResult {
    if !input_path.is_file() {
        return ConversionResult {
            input: input_path.to_string_lossy().to_string(),
            output: None,
            ok: false,
            message: "输入文件不存在".to_string(),
        };
    }

    if let Some(parent) = output_path.parent() {
        if let Err(error) = fs::create_dir_all(parent) {
            return ConversionResult {
                input: input_path.to_string_lossy().to_string(),
                output: Some(output_path.to_string_lossy().to_string()),
                ok: false,
                message: format!("无法创建输出目录：{error}"),
            };
        }
    }

    let output = Command::new(ffmpeg)
        .arg("-hide_banner")
        .arg(if overwrite { "-y" } else { "-n" })
        .arg("-i")
        .arg(input_path)
        .arg("-vn")
        .arg("-map")
        .arg("0:a:0")
        .arg("-ar")
        .arg("48000")
        .arg("-ac")
        .arg("2")
        .arg("-c:a")
        .arg("pcm_s16le")
        .arg("-f")
        .arg("s16le")
        .arg(output_path)
        .stdout(Stdio::null())
        .stderr(Stdio::piped())
        .output();

    match output {
        Ok(output) if output.status.success() => ConversionResult {
            input: input_path.to_string_lossy().to_string(),
            output: Some(output_path.to_string_lossy().to_string()),
            ok: true,
            message: "转换完成".to_string(),
        },
        Ok(output) => ConversionResult {
            input: input_path.to_string_lossy().to_string(),
            output: Some(output_path.to_string_lossy().to_string()),
            ok: false,
            message: summarize_stderr(&output.stderr),
        },
        Err(error) => ConversionResult {
            input: input_path.to_string_lossy().to_string(),
            output: Some(output_path.to_string_lossy().to_string()),
            ok: false,
            message: format!("无法启动 FFmpeg：{error}"),
        },
    }
}

fn make_output_path(
    input_path: &Path,
    output_dir: Option<&str>,
    overwrite: bool,
    output_name: Option<&str>,
) -> PathBuf {
    let stem = input_path
        .file_stem()
        .and_then(|name| name.to_str())
        .unwrap_or("converted");
    let dir = output_dir
        .map(PathBuf::from)
        .or_else(|| input_path.parent().map(Path::to_path_buf))
        .unwrap_or_else(|| env::current_dir().unwrap_or_else(|_| PathBuf::from(".")));

    let file_name = output_name
        .map(sanitize_output_name)
        .filter(|name| !name.is_empty())
        .unwrap_or_else(|| format!("{stem}.pcm"));
    let mut output = dir.join(ensure_pcm_extension(&file_name));
    if overwrite {
        return output;
    }

    let mut index = 1;
    while output.exists() {
        let path = Path::new(&file_name);
        let name_stem = path
            .file_stem()
            .and_then(|name| name.to_str())
            .unwrap_or(stem);
        output = dir.join(format!("{name_stem}_{index}.pcm"));
        index += 1;
    }

    output
}

fn sanitize_output_name(name: &str) -> String {
    name.trim()
        .chars()
        .map(|ch| match ch {
            '\\' | '/' | ':' | '*' | '?' | '"' | '<' | '>' | '|' => '_',
            _ => ch,
        })
        .collect()
}

fn ensure_pcm_extension(name: &str) -> String {
    if name.to_ascii_lowercase().ends_with(".pcm") {
        name.to_string()
    } else {
        format!("{name}.pcm")
    }
}

fn ffmpeg_version(app: &tauri::AppHandle) -> Result<(String, String), String> {
    let ffmpeg = find_ffmpeg(app);
    let output = Command::new(&ffmpeg)
        .arg("-version")
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .output()
        .map_err(|error| format!("未找到 FFmpeg：{error}"))?;

    if !output.status.success() {
        return Err(summarize_stderr(&output.stderr));
    }

    let stdout = String::from_utf8_lossy(&output.stdout);
    let version = stdout.lines().next().unwrap_or("FFmpeg 可用").to_string();

    Ok((ffmpeg.to_string_lossy().to_string(), version))
}

fn find_ffmpeg(app: &tauri::AppHandle) -> PathBuf {
    let executable_name = if cfg!(windows) { "ffmpeg.exe" } else { "ffmpeg" };

    if let Ok(resource_path) = app
        .path()
        .resolve(format!("binaries/{executable_name}"), BaseDirectory::Resource)
    {
        if resource_path.is_file() {
            return resource_path;
        }
    }

    for candidate in bundled_ffmpeg_candidates(executable_name) {
        if candidate.is_file() {
            return candidate;
        }
    }

    PathBuf::from(executable_name)
}

fn bundled_ffmpeg_candidates(executable_name: &str) -> Vec<PathBuf> {
    let mut candidates = Vec::new();

    if let Ok(current_exe) = env::current_exe() {
        if let Some(exe_dir) = current_exe.parent() {
            candidates.push(exe_dir.join(executable_name));
            candidates.push(exe_dir.join("binaries").join(executable_name));
        }
    }

    if let Ok(current_dir) = env::current_dir() {
        candidates.push(current_dir.join(executable_name));
        candidates.push(current_dir.join("binaries").join(executable_name));
        candidates.push(current_dir.join("src-tauri").join("binaries").join(executable_name));
    }

    candidates
}

fn summarize_stderr(stderr: &[u8]) -> String {
    let text = String::from_utf8_lossy(stderr);
    let lines: Vec<&str> = text
        .lines()
        .filter(|line| !line.trim().is_empty())
        .rev()
        .take(5)
        .collect();

    if lines.is_empty() {
        "FFmpeg 转换失败".to_string()
    } else {
        lines.into_iter().rev().collect::<Vec<&str>>().join("\n")
    }
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_dialog::init())
        .invoke_handler(tauri::generate_handler![check_ffmpeg, convert_audio_files])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

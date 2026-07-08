# 合成器配套音色转换工具

Local Tauri desktop tool for converting audio files to raw PCM sample data for synthesizer sound assets:

- Headerless raw PCM
- 48 kHz sample rate
- 16-bit little-endian PCM
- Stereo

The app has two workflows:

- Normal conversion: export each selected audio file as `original_name.pcm`.
- Pitch mapping: put audio files into a C0-B8 note table and export note-named PCM files such as `C5.pcm` or `D#6.pcm`.
- In pitch mapping, drag audio files directly onto a note cell, or drag a filled cell onto another cell to move or swap mappings.

The app bundles `src-tauri/binaries/ffmpeg.exe` as a Tauri resource. At runtime it tries the bundled FFmpeg first, then falls back to `ffmpeg.exe` from `PATH`.

## Development

```powershell
npm install
npm run tauri -- dev
```

On Windows you can also double-click `启动开发版.cmd`.

## Build

```powershell
npm run tauri -- build
```

On Windows you can also double-click `重新打包.cmd`.

Current Windows bundle output:

- `src-tauri/target/release/bundle/nsis/合成器配套音色转换工具_0.1.0_x64-setup.exe`

## Conversion Command

The Rust backend runs FFmpeg with:

```powershell
ffmpeg -hide_banner -y -i input -vn -map 0:a:0 -ar 48000 -ac 2 -c:a pcm_s16le -f s16le output.pcm
```

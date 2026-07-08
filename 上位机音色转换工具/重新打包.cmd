@echo off
cd /d "%~dp0"
npm run tauri -- build
pause

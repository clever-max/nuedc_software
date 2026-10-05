@echo off
setlocal
cd /d "%~dp0.."
python tools\tmx_builder_gui.py
endlocal

set currentDir=%~dp0

REM ///------- Run the conversion tool.
cd %currentDir%\Meshes
start Obj2Headerv1.9d.exe

REM ///------- Timeout to allow the conversion tool time to work.
timeout /T 1 /NOBREAK

cd ..
copy %currentDir%\Meshes\*.h2b %currentDir%\LevelViewer
move %currentDir%\Meshes\*.h2b %currentDir%\LevelViewer\build
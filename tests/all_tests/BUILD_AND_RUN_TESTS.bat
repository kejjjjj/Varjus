
if exist "Varjus.exe" (
    del "Varjus.exe"
)

cmake -S . -B build
cmake --build build

:: Check if Varjus.exe exists
if exist "Varjus.exe" (
    start cmd /k "Varjus.exe"
) else (
    start cmd /K "pause"
    echo Varjus.exe not found.
)

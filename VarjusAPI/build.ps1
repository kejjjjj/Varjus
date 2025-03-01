$choice = Read-Host "Enter 1 for Release or 2 for Debug mode"
switch ($choice) {
    1 { $type = "Release" }
    2 { $type = "Debug" }
    default {
        Write-Host "Invalid input. Defaulting to Release."
        $type = "Release"
    }
}

if (Test-Path "build") {
    $confirm = Read-Host "Build folder exists. Delete it? (y/n)"
    if ($confirm -match "^[Yy]$") {
        Remove-Item -Recurse -Force "build"
    } else {
        Write-Host "Skipping deletion."
    }
}

$windows = $env:OS -eq "Windows_NT" -or $env:PROCESSOR_ARCHITECTURE -match "AMD64|x86"

if ($windows) {
    $archChoice = Read-Host "Enter 1 for x64 or 2 for x86"
    switch ($archChoice) {
        1 { $arch = "x64" }
        2 { $arch = "Win32" }
        default {
            Write-Host "Invalid input. Defaulting to x64."
            $arch = "x64"
        }
    }
    cmake -S . -B build -A "$arch" -DCMAKE_BUILD_TYPE="$type"
    cmake --build build --config "$type"
} else {
    cmake -S . -B build -DCMAKE_BUILD_TYPE="$type"
    cmake --build build
}

Write-Host "Build completed in $type mode."

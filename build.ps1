Write-Host "What do you want to build?"

# Select build target
$targetChoice = Read-Host "Enter 1 for Project, 2 for API, or 3 for Tests"
switch ($targetChoice) {
    1 { $targetPath = "." }
    2 { $targetPath = "VarjusAPI" }
    3 { $targetPath = "tests/all_tests" }
    default {
        Write-Host "Invalid input. Defaulting to Project."
        $targetPath = "."
    }
}

$buildDir = $targetPath + "/build";

# Handle existing build folder
if (Test-Path "$buildDir") {
    $confirm = Read-Host "Build folder exists. Delete it? (y/n)"
    if ($confirm -match "^[Yy]$") {
        Remove-Item -Recurse -Force "$buildDir"
    } else {
        Write-Host "Skipping deletion."
    }
}

# Select build type
$choice = Read-Host "Enter 1 for Unicode or 2 for UTF8"
switch ($choice) {
    1 { $_unicode = "ON" }
    2 { $_unicode = "OFF" }
    default {
        Write-Host "Invalid input. Defaulting to Unicode."
        $type = "ON"
    }
}

if ($_unicode -eq "ON" -and $targetPath -eq "tests/all_tests"){
    Write-Host "Make sure all test script files have been encoded with utf8/16 LE/BE with bom :)"
}

# Select build type
$choice = Read-Host "Enter 1 for Release or 2 for Debug mode"
switch ($choice) {
    1 { $type = "Release" }
    2 { $type = "Debug" }
    default {
        Write-Host "Invalid input. Defaulting to Release."
        $type = "Release"
    }
}



# Detect Windows
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
    cmake -S "$targetPath" -B "$buildDir" -A "$arch" -DCMAKE_BUILD_TYPE="$type" -DUSE_UNICODE="$_unicode"
    cmake --build "$buildDir" --config "$type"
} else {
    cmake -S "$targetPath" -B "$buildDir" -DCMAKE_BUILD_TYPE="$type" -DUSE_UNICODE="$_unicode"
    cmake --build "$buildDir"
}

Write-Host "Build completed for '$targetPath' in $type mode."

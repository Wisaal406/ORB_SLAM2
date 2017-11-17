@echo off

set "OrbSlamPlatform=x86"
set "OrbSlamToolset=v141"
set "OrbSlamBuildType=Debug"

if NOT "%~1"=="" set "OrbSlamPlatform=%~1"
if NOT "%~2"=="" set "OrbSlamToolset=%~2"
if NOT "%~3"=="" set "OrbSlamBuildType=%~3" 

set "VcPkgDir=%~d0\Software\vcpkg\vcpkg"
if defined VCPKG_ROOT_DIR if /i not "%VCPKG_ROOT_DIR%"=="" set "VcPkgDir=%VCPKG_ROOT_DIR%"
set "VcPkgTriplet=%OrbSlamPlatform%-windows-%OrbSlamToolset%"
if defined VCPKG_DEFAULT_TRIPLET if /i not "%VCPKG_DEFAULT_TRIPLET%"=="" set "VcpkgTriplet=%VCPKG_DEFAULT_TRIPLET%"
set "VcPkgPath=%VcPkgDir%\vcpkg.exe"

echo. & echo Bootstrapping dependencies for triplet: %VcPkgTriplet% & echo.

if not exist "%VcPkgDir%" echo vcpkg path is not set correctly, bailing out & exit /b 1

rem ==============================
rem Update and Install packages.
rem ==============================
call "%VcPkgPath%" update
call "%VcPkgPath%" install eigen3 opencv pangolin --triplet %VcPkgTriplet%

set "VcPkgTripletDir=%VcPkgDir%\installed\%VcPkgTriplet%"

if not exist "%VcPkgTripletDir%" echo %VcPkgTripletDir% does not exist, bailing out & exit /b 1

set "CMAKE_PREFIX_PATH=%VcPkgTripletDir%;%CMAKE_PREFIX_PATH%"

echo. & echo Bootstrapping successful for triplet: %VcPkgTriplet% & echo CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% & echo.

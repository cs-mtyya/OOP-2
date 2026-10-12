@echo off
setlocal EnableExtensions
cd /d "%~dp0"

set "MSYS_UCRT=C:\msys64\ucrt64"
set "QT_BIN=%MSYS_UCRT%\bin"
set "BUILD_DIR=%~dp0build"
set "DIST_DIR=%~dp0dist\PixelStudio"

if not exist "%QT_BIN%\windeployqt6.exe" (
    echo ERROR: windeployqt6.exe was not found under %QT_BIN%.
    echo Install the Qt6 ucrt64 package in MSYS2 and try again.
    exit /b 1
)

cmake -S "%~dp0." -B "%BUILD_DIR%" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%MSYS_UCRT%"
if errorlevel 1 exit /b %errorlevel%
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 exit /b %errorlevel%

if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"
"%QT_BIN%\windeployqt6.exe" --release --compiler-runtime --no-translations --dir "%DIST_DIR%" "%BUILD_DIR%\PixelStudio.exe"
if errorlevel 1 exit /b %errorlevel%
copy /y "%BUILD_DIR%\PixelStudio.exe" "%DIST_DIR%\PixelStudio.exe" >nul
if errorlevel 1 exit /b %errorlevel%

for %%D in (libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll libb2-1.dll libbrotlidec.dll libbrotlicommon.dll libdouble-conversion.dll libfreetype-6.dll libgio-2.0-0.dll libgobject-2.0-0.dll libglib-2.0-0.dll libgmodule-2.0-0.dll libffi-8.dll libharfbuzz-0.dll libgraphite2.dll libicuin78.dll libicuuc78.dll libicudt78.dll libjpeg-8.dll libmd4c.dll libpcre2-16-0.dll libpcre2-8-0.dll libpng16-16.dll libzstd.dll zlib1.dll libbz2-1.dll libintl-8.dll libiconv-2.dll) do (
    if not exist "%QT_BIN%\%%D" (
        echo ERROR: Required runtime DLL %%D was not found in %QT_BIN%.
        exit /b 1
    )
    copy /y "%QT_BIN%\%%D" "%DIST_DIR%\%%D" >nul
)

if exist "%~dp0dist\PixelStudio.zip" del /q "%~dp0dist\PixelStudio.zip"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%DIST_DIR%\*' -DestinationPath '%~dp0dist\PixelStudio.zip' -Force"
if errorlevel 1 exit /b %errorlevel%

echo.
echo Standalone app: %DIST_DIR%\PixelStudio.exe
echo ZIP package:    %~dp0dist\PixelStudio.zip
endlocal
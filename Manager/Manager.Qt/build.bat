@echo off
rem Builds Manager.Qt with qmake + a Qt make tool (Windows).
rem Usage: build.bat [release|debug]
setlocal
cd /d "%~dp0"

set MODE=%1
if "%MODE%"=="" set MODE=release

rem --- find qmake ---------------------------------------------------------
set QMAKE=
where qmake6 >nul 2>nul && set QMAKE=qmake6
if "%QMAKE%"=="" where qmake >nul 2>nul && set QMAKE=qmake
if "%QMAKE%"=="" (
    echo qmake6/qmake not found on PATH. Open a "Qt 6" command prompt first.
    exit /b 1
)

rem --- find a make tool ---------------------------------------------------
set MAKE=
for %%T in (jom nmake mingw32-make make) do (
    if "%MAKE%"=="" where %%T >nul 2>nul && set MAKE=%%T
)
if "%MAKE%"=="" (
    echo No make tool found (jom, nmake, mingw32-make). Open a Qt prompt.
    exit /b 1
)

set BUILD_DIR=build-%MODE%
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"

echo Configuring (%MODE%) with %QMAKE% ...
if /i "%MODE%"=="debug" (
    %QMAKE% ..\Manager.Qt.pro CONFIG+=debug CONFIG-=release
) else (
    %QMAKE% ..\Manager.Qt.pro CONFIG+=release CONFIG-=debug
)

echo Building with %MAKE% ...
%MAKE%
if errorlevel 1 (
    popd
    exit /b 1
)

popd
echo.
echo Built: %~dp0bin\ManagerQt.exe
endlocal

@echo off
echo building Lab Manager (Release)
cd Manager
dotnet publish Manager.Windows -c Release
dotnet publish Manager.Avalonia -c Release
dotnet publish Manager.Avalonia -c Release -r linux-x64 --self-contained true

rem --- Qt manager (optional: needs Qt's qmake on PATH) ---
where qmake6 >nul 2>nul
if errorlevel 1 (
    where qmake >nul 2>nul
    if errorlevel 1 (
        echo Qt ^(qmake^) not found - skipping Manager.Qt
        goto :done
    )
)
echo building Lab Manager Qt (Release)
call Manager.Qt\build.bat release

:done

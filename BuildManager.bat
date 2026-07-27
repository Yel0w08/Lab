@echo off
echo building Lab Manager (Release)
cd Manager
dotnet publish Manager.Windows -c Release
dotnet publish Manager.Avalonia -c Release
dotnet publish Manager.Avalonia -c Release -r linux-x64 --self-contained true

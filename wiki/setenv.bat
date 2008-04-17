@echo off

@echo Setting up Qt...
@set QTDIR=C:\Qt\4.4.1
@set PATH=%QTDIR%\bin;%PATH%
@set QMAKESPEC=win32-msvc2005

@echo Setting up CMake...
@set PATH=C:\Program Files\CMake 2.4\bin;%PATH%

rem See C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\SetEnv.Cmd
@echo Setting up Microsoft Platform SDK...
@set PLATFORMSDK=C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2
@set PATH=%PLATFORMSDK%\Bin;%PATH%
@set INCLUDE=%PLATFORMSDK%\Include;%INCLUDE%
@set LIB=%PLATFORMSDK%\Lib;%LIB%

rem See C:\Program Files\Microsoft DirectX SDK (November 2007)\Utilities\Bin\dx_setenv.cmd
@echo Setting up Microsoft DirectX SDK...
@set DXSDK_DIR=C:\Program Files\Microsoft DirectX SDK (November 2007)
@set PATH=%DXSDK_DIR%\Utilities\Bin\x86;%PATH%
@set INCLUDE=%DXSDK_DIR%\Include;%INCLUDE%
@set LIB=%DXSDK_DIR%\Lib\x86;%LIB%

rem Must be the last
@echo Setting up Microsoft Visual Studio...
"C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"
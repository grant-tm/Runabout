@echo off
setlocal EnableDelayedExpansion

REM ROOT is two levels above this script 
set "ROOT=%~dp0../.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"

set "ARCH=%~1"
if "%ARCH%"=="" set "ARCH=x64"

set "OUTDIR=%ROOT%\build\builds\win_%ARCH%"
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

set "RSP=%OUTDIR%\build.rsp"
> "%RSP%" (
	echo /nologo
	echo /W4
	echo /Zi
	echo /O2
	echo /std:c11
	echo /D_CRT_SECURE_NO_WARNINGS
	echo /Fe:"%OUTDIR%\app.exe"
	echo /Fo:"%OUTDIR%\\"
)

REM ------------------ Include directories ------------------

for /r "%ROOT%\source" %%D in (.) do (
	set "DIR=%%~fD\"
	set "DIR_NO_SLASH=!DIR:~0,-1!"
	>>"%RSP%" echo /I "!DIR_NO_SLASH!"
)

REM ------------------ Source files ------------------

set /a SRC_COUNT=0
for /r "%ROOT%\source" %%F in (*.c) do (
	set "FILE=%%~fF"
	>>"%RSP%" echo "!FILE!"
	set /a SRC_COUNT+=1
)

echo Building Windows %ARCH% (MSVC)...
echo	Out: %OUTDIR%
echo	Rsp: %RSP%
echo	Sources: %SRC_COUNT%
echo.

cl @"%RSP%"
if errorlevel 1 exit /b 1
exit /b 0

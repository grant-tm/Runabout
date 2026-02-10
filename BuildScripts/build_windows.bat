@echo off
setlocal EnableDelayedExpansion

REM Root = parent of this script folder
set "ROOT=%~dp0.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"

set "ARCH=%~1"
if "%ARCH%"=="" set "ARCH=x64"

set "OUTDIR=%ROOT%\Builds\win_%ARCH%"
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

REM Exclude markers (must match the style of your generated paths)
REM Use no quotes here; these are plain substrings we search for.
set "EX1=\GUI\Shaders\"
set "EX2=\Builds\"
set "EX3=\.git\"
set "EX4=\BuildScripts\"

set "RSP=%OUTDIR%\build.rsp"
> "%RSP%" (
	echo /nologo
	echo /W4
	echo /Zi
	echo /O2
	echo /std:c11
	echo /D_CRT_SECURE_NO_WARNINGS
	echo /Fe:"%OUTDIR%\app.exe"
	echo /Fo:"%OUTDIR%"
)

REM ------------------ Include directories ------------------

for /r "%ROOT%" %%D in (.) do (
	set "DIR=%%~fD\"
	set "SKIP=0"

	REM If DIR contains EX*, then substitution changes the string -> skip it
	if not "!DIR:%EX1%=!"=="!DIR!" set "SKIP=1"
	if not "!DIR:%EX2%=!"=="!DIR!" set "SKIP=1"
	if not "!DIR:%EX3%=!"=="!DIR!" set "SKIP=1"
	if not "!DIR:%EX4%=!"=="!DIR!" set "SKIP=1"

	if "!SKIP!"=="0" (
		set "DIR_NO_SLASH=!DIR:~0,-1!"
		>>"%RSP%" echo /I "!DIR_NO_SLASH!"
	)
)

REM ------------------ Source files ------------------

set /a SRC_COUNT=0
for /r "%ROOT%" %%F in (*.c) do (
	set "FILE=%%~fF"
	set "SKIP=0"

	if not "!FILE:%EX1%=!"=="!FILE!" set "SKIP=1"
	if not "!FILE:%EX2%=!"=="!FILE!" set "SKIP=1"
	if not "!FILE:%EX3%=!"=="!FILE!" set "SKIP=1"
	if not "!FILE:%EX4%=!"=="!FILE!" set "SKIP=1"

	if "!SKIP!"=="0" (
		>>"%RSP%" echo "!FILE!"
		set /a SRC_COUNT+=1
	)
)

echo Building Windows %ARCH% (MSVC)...
echo	Out: %OUTDIR%
echo	Rsp: %RSP%
echo	Sources: %SRC_COUNT%
echo.

cl @"%RSP%"
if errorlevel 1 exit /b 1
exit /b 0

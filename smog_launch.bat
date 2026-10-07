@echo off
setlocal
cd /d "%~dp0"
if not defined LOCALAPPDATA (
  echo Cannot locate the Windows user data directory. 1>&2
  exit /b 1
)
if not exist "LethalWorld\Binaries\Win64\LethalWorld-Win64-Shipping.exe" (
  echo The game installation is incomplete. Reinstall this build in SMOG. 1>&2
  exit /b 2
)
rem Invoke directly so SMOG tracks the complete game session.
rem Keep saves and settings outside SMOG's replaceable installation directory.
"%~dp0LethalWorld\Binaries\Win64\LethalWorld-Win64-Shipping.exe" -UserDir="%LOCALAPPDATA%\AllAmericanMeltdown" %*
exit /b %errorlevel%

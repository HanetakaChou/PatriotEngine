ECHO OFF
REM
REM Copyright (C) YuqiaoZhang(HanetakaYuminaga)
REM 
REM This program is free software: you can redistribute it and/or modify
REM it under the terms of the GNU Lesser General Public License as published
REM by the Free Software Foundation, either version 3 of the License, or
REM (at your option) any later version.
REM 
REM This program is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU Lesser General Public License for more details.
REM 
REM You should have received a copy of the GNU Lesser General Public License
REM along with this program.  If not, see <https://www.gnu.org/licenses/>.
REM

SET "MY_DIR=%~dp0"
CD /d "%MY_DIR%"

REM https://developer.android.com/studio/debug#debug-types
REM android-ndk-r14b/prebuilt/windows-x86_64/bin/ndk-gdb.py
REM android-ndk-r14b/python-packages/gdbrunner/__init__.py
REM gdbrunner.get_run_as_cmd

REM The windows batch is too ??? we use the octave script

REM CALL "%MY_DIR%/android-sdk/platform-tools/adb.exe" shell am start -a android.intent.action.MAIN -c android-intent.category.LAUNCH -n YuqiaoZhang.HanetakaYuminaga.PatriotEngine/android.app.NativeActivity

SET PACKAGE_NAME=YuqiaoZhang.HanetakaYuminaga.PatriotEngine
SET "ADB_CMD=%MY_DIR%/android-sdk/platform-tools/adb.exe"
SET "ECHO_CMD=%MY_DIR%/android-ndk-r14b/prebuilt/windows-x86_64/bin/echo.exe"
REM "%ADB_CMD%" shell "run-as "%PACKAGE_NAME%" sh -c 'pwd' 2>/dev/null"
REM FOR /f "tokens=* USEBACKQ" %%a in (`"%ADB_CMD%" shell "run-as "%PACKAGE_NAME%" sh -c 'pwd' 2>/dev/null"`) do ( SET APP_DATA_DIR=%%a )
REM ECHO "%ECHO_CMD%" %APP_DATA_DIR%1
FOR /f "delims=" %%a in (' "%MY_DIR%/android-sdk/platform-tools/adb.exe" shell "run-as %PACKAGE_NAME% /system/bin/sh -c pwd" ') DO ( SET APP_DATA_DIR=%%a )
 "%ECHO_CMD%" %APP_DATA_DIR%1
REM ECHO %APP_DATA_DIR%
REM awk to trim string

REM "%MY_DIR%/android-sdk/platform-tools/adb.exe" shell run-as %PACKAGE_NAME% "/system/bin/chmod" "a+x" "%APP_DATA_DIR%"
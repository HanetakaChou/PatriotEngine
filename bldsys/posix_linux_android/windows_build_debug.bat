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

REM ndk-build

CALL "%MY_DIR%/android-ndk-r14b/ndk-build.cmd" APP_DEBUG:=true NDK_PROJECT_PATH:=null NDK_OUT:=obj/debug NDK_LIBS_OUT:=libs/debug/lib NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk
IF %ERRORLEVEL% NEQ 0 ( 
   EXIT 1
)

REM Packaging

MKDIR "%MY_DIR%/bin"

REM help vscode find the symbol
SET "OUT_BINS=libpt_mcrt.so libpt_gfx.so libpt_wsi.so libpt_launcher.so"
FOR %%i in (%OUT_BINS%) do (
   COPY /y "%MY_DIR%\obj\debug\local\arm64-v8a\%%i" "%MY_DIR%/libs/debug/lib/arm64-v8a/%%i"
   COPY /y "%MY_DIR%\obj\debug\local\armeabi-v7a\%%i" "%MY_DIR%/libs/debug/lib/armeabi-v7a/%%i"
   COPY /y "%MY_DIR%\obj\debug\local\x86_64\%%i" "%MY_DIR%/libs/debug/lib/x86_64/%%i"
   COPY /y "%MY_DIR%\obj\debug\local\x86\%%i" "%MY_DIR%/libs/debug/lib/x86/%%i"
)

REM we upload the gdbserver manually in ndk-gdb.py
DEL /f /q "%MY_DIR%\libs\debug\lib\arm64-v8a\gdb.setup"

REM used by ndk_gdbserver_windows.m
MKDIR "%MY_DIR%/bin/arm64-v8a"
MOVE -y "%MY_DIR%\libs\debug\lib\arm64-v8a\gdbserver" "%MY_DIR%/bin/arm64-v8a/gdbserver" 

DEL /f /q "%MY_DIR%\libs\debug\lib\armeabi-v7a\gdb.setup"
DEL /f /q "%MY_DIR%\libs\debug\lib\armeabi-v7a\gdbserver"
DEL /f /q "%MY_DIR%\libs\debug\lib\x86_64\gdb.setup"
DEL /f /q "%MY_DIR%\libs\debug\lib\x86_64\gdbserver"
DEL /f /q "%MY_DIR%\libs\debug\lib\x86\gdb.setup"
DEL /f /q "%MY_DIR%\libs\debug\lib\x86\gdbserver"
RMDIR /s /q "%MY_DIR%\libs\debug\lib\arm64-v8a\gdb.setup" 2>NUL
RMDIR /s /q "%MY_DIR%\libs\debug\lib\arm64-v8a\gdbserver" 2>NUL 
RMDIR /s /q "%MY_DIR%\libs\debug\lib\armeabi-v7a\gdb.setup" 2>NUL
RMDIR /s /q "%MY_DIR%\libs\debug\lib\armeabi-v7a\gdbserver" 2>NUL
RMDIR /s /q "%MY_DIR%\libs\debug\lib\x86_64\gdb.setup" 2>NUL
RMDIR /s /q "%MY_DIR%\libs\debug\lib\x86_64\gdbserver" 2>NUL
RMDIR /s /q "%MY_DIR%\libs\debug\lib\x86\gdb.setup" 2>NUL
RMDIR /s /q "%MY_DIR%\libs\debug\lib\x86\gdbserver" 2>NUL

"%MY_DIR%/android-sdk/build-tools/29.0.3/aapt.exe" package -f --debug-mode -0 apk -M "%MY_DIR%/AndroidManifest.xml" -S "%MY_DIR%/res" -I "%MY_DIR%/android-sdk/platforms/android-24/android.jar" -F "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" "%MY_DIR%/libs/debug"    

"%MY_DIR%/android-sdk/build-tools/29.0.3/zipalign.exe" -f 4 "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" "%MY_DIR%/bin/Android.Packaging-debug.apk"

REM https://docs.microsoft.com/en-us/xamarin/android/deploy-test/signing/manually-signing-the-apk#sign-the-apk
java.exe -jar "%MY_DIR%/android-sdk/build-tools/29.0.3/lib/apksigner.jar" sign -v --ks "%MY_DIR%/debug.keystore" --ks-pass pass:android --ks-key-alias androiddebugkey "%MY_DIR%/bin/Android.Packaging-debug.apk"


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

CALL "%MY_DIR%/android-ndk-r14b/ndk-build.cmd" APP_DEBUG:=true NDK_PROJECT_PATH:=null NDK_OUT:=obj/debug NDK_LIBS_OUT:=libs/debug/lib NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=build.mk
IF %ERRORLEVEL% NEQ 0 ( 
   EXIT 1
)

ECHO ON

REM Packaging

MKDIR "%MY_DIR%/bin"

REM todo
REM help vscode find the symbol

"%MY_DIR%/android-sdk/build-tools/29.0.3/aapt.exe" package -f --debug-mode -0 apk -M "%MY_DIR%/AndroidManifest.xml" -S "%MY_DIR%/res" -I "%MY_DIR%/android-sdk/platforms/android-24/android.jar" -F "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" "%MY_DIR%/libs/debug"    

"%MY_DIR%/android-sdk/build-tools/29.0.3/zipalign.exe" -f 4 "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" "%MY_DIR%/bin/Android.Packaging-debug.apk"

REM https://docs.microsoft.com/en-us/xamarin/android/deploy-test/signing/manually-signing-the-apk#sign-the-apk
java.exe -jar "%MY_DIR%/android-sdk/build-tools/29.0.3/lib/apksigner.jar" sign -v --ks "%MY_DIR%/debug.keystore" --ks-pass pass:android --ks-key-alias androiddebugkey "%MY_DIR%/bin/Android.Packaging-debug.apk"


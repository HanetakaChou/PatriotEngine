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

call "%MY_DIR%/android-ndk-r14b/ndk-build.cmd" APP_DEBUG:=true NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=jni/libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=build.mk

ECHO ON

REM Packaging

MKDIR "%MY_DIR%/bin"

REM -crunch
REM "%MY_DIR%/android-10/aapt.exe" crunch -v -S "%MY_DIR%/res" -C "%MY_DIR%/bin/res"

REM -package-resources
REM "%MY_DIR%/android-10/aapt.exe" package --no-crunch -f --debug-mode -0 apk -M "%MY_DIR%/AndroidManifest.xml" -S "%MY_DIR%/bin/res" -I "%MY_DIR%/android-7.0/android.jar" -F "%MY_DIR%/bin/Android.Packaging.ap_" --generate-dependencies   

REM -apkbuilder
REM ANDROID_HOME/tools/ant/build.xml
REM ANDROID_HOME/tools/lib/ant-tasks.jar/ApkBuilderTask.class
REM java.exe -classpath ANDROID_HOME/tools/lib/sdklib.jar com.android.sdklib.build.ApkBuilderMain

"%MY_DIR%/android-10/aapt.exe" package -f --debug-mode -0 apk -M "%MY_DIR%/AndroidManifest.xml" -S "%MY_DIR%/res" -I "%MY_DIR%/android-7.0/android.jar" -F "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" --generate-dependencies "%MY_DIR%/jni"    

REM jarsigner.exe -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore "%MY_DIR%/debug.keystore" -storepass android "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" androiddebugkey

REM https://docs.microsoft.com/en-us/xamarin/android/deploy-test/signing/manually-signing-the-apk#sign-the-apk
java.exe -jar "%MY_DIR%/android-10/lib/apksigner.jar" sign -v --ks "%MY_DIR%/debug.keystore" --ks-pass pass:android --ks-key-alias androiddebugkey "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk"

REM -do-debug
"%MY_DIR%/android-10/zipalign.exe" -f 4 "%MY_DIR%/bin/Android.Packaging-debug-unaligned.apk" "%MY_DIR%/bin/Android.Packaging-debug.apk"
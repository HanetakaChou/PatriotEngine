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

REM -crunch
"%MY_DIR%/android-10/aapt.exe" crunch -v -S "%MY_DIR%/res" -C "%MY_DIR%/bin/res"

REM -package-resources
"%MY_DIR%/android-10/aapt.exe" package --no-crunch -f --debug-mode -0 apk -M "%MY_DIR%/AndroidManifest.xml" -S "%MY_DIR%/bin/res" -I "%MY_DIR%/android-7.0/android.jar" -F "%MY_DIR%/bin/Android.Packaging.ap_" --generate-dependencies   


REM apkbuilder
REM ANDROID_SDK_ROOT/tools/ant/build.xml
REM ANDROID_SDK_ROOT/tools/lib/ant-tasks.jar/ApkBuilderTask.class
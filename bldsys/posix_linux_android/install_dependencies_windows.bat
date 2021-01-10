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

DEL /f /q "%MY_DIR%/android-ndk-r14b-windows-x86_64.zip"
RMDIR /s /q "%MY_DIR%/android-ndk-r14b-windows-x86_64.zip"
"%MY_DIR%/../../third_party/libs/curl/bin/win32_x64/curl.exe" -L https://dl.google.com/android/repository/android-ndk-r14b-windows-x86_64.zip -o "%MY_DIR%/android-ndk-r14b-windows-x86_64.zip"

REM https://developer.android.com/studio/releases/build-tools
DEL /f /q "%MY_DIR%/build-tools_r29.0.3-windows.zip"
RMDIR /s /q "%MY_DIR%/build-tools_r29.0.3-windows.zip"
"%MY_DIR%/../../third_party/libs/curl/bin/win32_x64/curl.exe" -L https://dl.google.com/android/repository/build-tools_r29.0.3-windows.zip -o "%MY_DIR%/build-tools_r29.0.3-windows.zip"

DEL /f /q "%MY_DIR%/platform-24_r02.zip"
RMDIR /s /q "%MY_DIR%/platform-24_r02.zip"
"%MY_DIR%/../../third_party/libs/curl/bin/win32_x64/curl.exe" -L https://dl.google.com/android/repository/platform-24_r02.zip -o "%MY_DIR%/platform-24_r02.zip"

REM https://developer.android.com/studio/releases/platform-tools
DEL /f /q "%MY_DIR%/platform-tools_r29.0.6-windows.zip"
RMDIR /s /q "%MY_DIR%/platform-tools_r29.0.6-windows.zip"
"%MY_DIR%/../../third_party/libs/curl/bin/win32_x64/curl.exe" -L https://dl.google.com/android/repository/platform-tools_r29.0.6-windows.zip -o "%MY_DIR%/platform-tools_r29.0.6-windows.zip"

REM no unzip on windows
REM https://docs.oracle.com/javase/tutorial/deployment/jar/unpack.html

DEL /f /q "%MY_DIR%/android-ndk-r14b"
RMDIR /s /q "%MY_DIR%/android-ndk-r14b"
jar.exe xf "%MY_DIR%/android-ndk-r14b-windows-x86_64.zip"
DEL /f /q "%MY_DIR%/android-ndk-r14b-windows-x86_64.zip"

DEL /f /q "%MY_DIR%/android-10"
RMDIR /s /q "%MY_DIR%/android-10"
jar.exe xf "%MY_DIR%/build-tools_r29.0.3-windows.zip"
DEL /f /q "%MY_DIR%/build-tools_r29.0.3-windows.zip"

DEL /f /q "%MY_DIR%/android-sdk/build-tools/29.0.3"
RMDIR /s /q "%MY_DIR%/android-sdk/build-tools/29.0.3"
MKDIR "%MY_DIR%/android-sdk/build-tools/"
MOVE /y "%MY_DIR%/android-10" "%MY_DIR%/android-sdk/build-tools/29.0.3"

DEL /f /q "%MY_DIR%/android-7.0"
RMDIR /s /q "%MY_DIR%/android-7.0"
jar.exe xf "%MY_DIR%/platform-24_r02.zip"
DEL /f /q "%MY_DIR%/platform-24_r02.zip"

DEL /f /q "%MY_DIR%/android-sdk/platforms/android-24"
RMDIR /s /q "%MY_DIR%/android-sdk/platforms/android-24"
MKDIR "%MY_DIR%/android-sdk/platforms/"
MOVE /y "%MY_DIR%/android-7.0" "%MY_DIR%/android-sdk/platforms/android-24"

DEL /f /q "%MY_DIR%/platform-tools"
RMDIR /s /q "%MY_DIR%/platform-tools"
jar.exe xf "%MY_DIR%/platform-tools_r29.0.6-windows.zip"
DEL /f /q "%MY_DIR%/platform-tools_r29.0.6-windows.zip"

DEL /f /q "%MY_DIR%/android-sdk/platform-tools"
RMDIR /s /q "%MY_DIR%/android-sdk/platform-tools"
MKDIR "%MY_DIR%/android-sdk/"
MOVE /y "%MY_DIR%/platform-tools" "%MY_DIR%/android-sdk/platform-tools"

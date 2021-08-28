 #!/bin/bash

#
# Copyright (C) YuqiaoZhang(HanetakaYuminaga)
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

# Debugging with VS Code
# https://source.android.com/devices/tech/debug/gdb#vscode

MY_DIR="$(cd "$(dirname "$0")" 1>/dev/null 2>/dev/null && pwd)"  
cd ${MY_DIR}

# https://developer.android.com/studio/debug#debug-types
# android-ndk-r14b/prebuilt/windows-x86_64/bin/ndk-gdb.py
# android-ndk-r14b/python-packages/gdbrunner/__init__.py
# gdbrunner.get_run_as_cmd

PACKAGE_NAME=YuqiaoZhang.HanetakaYuminaga.PatriotEngine
LAUNCH_ACTIVITY_NAME=android.app.NativeActivity
ADB_CMD="${MY_DIR}/android-sdk/platform-tools/adb"
GDBSERVER_CMD="${MY_DIR}/bin/arm64-v8a/gdbserver"
OUT_DIR="${MY_DIR}/obj/debug/local/arm64-v8a"
ARCH=arm64
DELAY=0.25s
PORT=5039

APP_DATA_DIR="$("${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" sh -c 'pwd' 2>/dev/null" | xargs)"
if test -z ${APP_DATA_DIR}; then
    echo "Could not find application's data directory. Are you sure that the application is installed and debuggable?"
    exit 1
fi

# Applications with minSdkVersion >= 24 will have their data directories
# created with rwx------ permissions, preventing adbd from forwarding to
# the gdbserver socket. To be safe, if we're on a device >= 24, always
# chmod the directory.
if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" chmod 711 "${APP_DATA_DIR}""; then # chmod a+x
    echo "Found application data directory: ${APP_DATA_DIR}"
else
    echo "Failed to make application data directory world executable"
    exit 1
fi



#0x75dc5055d8 "/data/user/0/YuqiaoZhang.HanetakaYuminaga.PatriotEngine/files"
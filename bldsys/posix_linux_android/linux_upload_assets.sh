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
ADB_CMD="${MY_DIR}/android-sdk/platform-tools/adb"
ASSET_DIR="$(cd "${MY_DIR}/../../assets" 1>/dev/null 2>/dev/null && pwd)"

APP_PWD_DIR="$("${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" pwd")"
if test -z ${APP_PWD_DIR}; then
    echo "Could not find application's data directory. Are you sure that the application is installed and debuggable?"
    exit 1
fi

# Applications with minSdkVersion >= 24 will have their data directories
# created with rwx------ permissions, preventing adbd from forwarding to
# the gdbserver socket. To be safe, if we're on a device >= 24, always
# chmod the directory.
if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" chmod 711 "${APP_PWD_DIR}""; then # chmod a+x
    echo "Found application data directory: ${APP_PWD_DIR}"
else
    echo "Failed to make application data directory world executable"
    exit 1
fi

APP_INTERNAL_DATA_DIR="${APP_PWD_DIR}/files"

# We need to upload our asset
MY_ASSET_ARRAY=( $(find "${ASSET_DIR}" -type f -print0 | xargs -0) )
for ASSET_LOCAL_FULL_PATH in  "${MY_ASSET_ARRAY[@]}"
do
    ASSET_PATH="$(realpath --relative-to="${ASSET_DIR}" "${ASSET_LOCAL_FULL_PATH}")"

    COPY_DEST_PATH="${APP_INTERNAL_DATA_DIR}/${ASSET_PATH}"

    if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" ls "${COPY_DEST_PATH}" 1>/dev/null 2>/dev/null"; then
        echo "Found ${COPY_DEST_PATH} skip copy"
    else
        # Firstly we upload the local asset to "/data/local/tmp"
        PUSH_LOCAL_PATH="${ASSET_DIR}/${ASSET_PATH}"
        PUSH_REMOTE_PATH="/data/local/tmp/${APP_INTERNAL_DATA_DIR}/tmp_asset"
        if "${ADB_CMD}" push "${PUSH_LOCAL_PATH}" "${PUSH_REMOTE_PATH}"; then
            echo "Have uploaded ${PUSH_LOCAL_PATH} to ${PUSH_REMOTE_PATH}"
        else
            echo "Failed to upload ${PUSH_LOCAL_PATH} to ${PUSH_REMOTE_PATH}"
            exit 1
        fi

        # Secondly, we "copy" the asset from "/data/local/tmp" to app internal data path
        COPY_SRC_PATH="${PUSH_REMOTE_PATH}"
        COPY_DEST_DIR="$(dirname "${COPY_DEST_PATH}")"
        "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" mkdir -p "${COPY_DEST_DIR}""
        if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" cp -f "${COPY_SRC_PATH}" "${COPY_DEST_PATH}""; then
            echo "Have copied ${COPY_SRC_PATH} to ${COPY_DEST_PATH}"
        else
            echo "Failed to copy ${COPY_SRC_PATH} to ${COPY_DEST_PATH}"
            exit 1
        fi
    fi
done



#0x75dc5055d8 "/data/user/0/YuqiaoZhang.HanetakaYuminaga.PatriotEngine/files"
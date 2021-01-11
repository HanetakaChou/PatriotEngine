 
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

MY_DIR="$(readlink -f "$(dirname "$0")")"
cd ${MY_DIR}

# https://developer.android.com/studio/debug#debug-types
# android-ndk-r14b/prebuilt/windows-x86_64/bin/ndk-gdb.py
# android-ndk-r14b/python-packages/gdbrunner/__init__.py
# gdbrunner.get_run_as_cmd

ADB_CMD="${MY_DIR}/android-sdk/platform-tools/adb"
PACKAGE_NAME=YuqiaoZhang.HanetakaYuminaga.PatriotEngine
ARCH=arm64

DATA_DIR="$("${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" sh -c pwd '2>/dev/null'" | xargs)"
if test -z ${DATA_DIR}; then
    echo "Could not find application's data directory. Are you sure that the application is installed and debuggable?"
    exit 1
fi

# Applications with minSdkVersion >= 24 will have their data directories
# created with rwx------ permissions, preventing adbd from forwarding to
# the gdbserver socket. To be safe, if we're on a device >= 24, always
# chmod the directory.
if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" chmod 711 "${DATA_DIR}""; then # chmod a+x
    echo "Found application data directory: ${DATA_DIR}"
else
    echo "Failed to make application data directory world executable"
fi
APP_DATA_DIR=${DATA_DIR}

APP_GDBSERVER_PATH="${APP_DATA_DIR}/lib/gdbserver"
if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" ls "${APP_GDBSERVER_PATH}" '2>/dev/null'"; then
    echo "Found app gdbserver: ${APP_GDBSERVER_PATH}"
else
    # We need to upload our gdbserver
    echo "App gdbserver not found at ${APP_GDBSERVER_PATH}, uploading."
    LOCAL_PATH="${MY_DIR}/android-ndk-r14b/prebuilt/android-${ARCH}/gdbserver/gdbserver"
    REMOTE_PATH="/data/local/tmp/${ARCH}-gdbserver"
    "${ADB_CMD}" push "${LOCAL_PATH}" "${REMOTE_PATH}"

    # Copy gdbserver into the data directory on M+, because selinux prevents
    # execution of binaries directly from /data/local/tmp.
    DESTINATION="${APP_DATA_DIR}/${ARCH}-gdbserver"
    echo "Copying gdbserver to ${DESTINATION}."
    "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" sh -c 'cat '${REMOTE_PATH}' | cat > '${DESTINATION}''"
fi
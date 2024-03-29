 #!/bin/bash

#
# Copyright (C) YuqiaoZhang(HanetakaChou)
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

PACKAGE_NAME=YuqiaoZhang.HanetakaChou.PatriotEngine
ADB_CMD="${MY_DIR}/android-sdk/platform-tools/adb"

LAUNCH_ACTIVITY_NAME=android.app.NativeActivity
DELAY=0.25s

GDBSERVER_LOCAL_PATH="${MY_DIR}/bin/arm64-v8a/gdbserver"
ARCH=arm64
PORT=5039
OUT_DIR="${MY_DIR}/obj/debug/local/arm64-v8a"

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

APP_INTERNAL_LIB_DIR="${APP_PWD_DIR}/lib/${ARCH}"

# We need to upload our gdbserver
APP_GDBSERVER_PATH="${APP_INTERNAL_LIB_DIR}/gdbserver"
COPY_DEST_PATH="${APP_GDBSERVER_PATH}"
if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" ls "${COPY_DEST_PATH}" 1>/dev/null 2>/dev/null"; then
    echo "Found ${COPY_DEST_PATH} skip copy"
else
    # Firstly we upload the local gdbserver to "/data/local/tmp"
    PUSH_LOCAL_PATH="${GDBSERVER_LOCAL_PATH}"
    PUSH_REMOTE_PATH="/data/local/tmp/${APP_INTERNAL_LIB_DIR}/tmp_gdbserver"
    if "${ADB_CMD}" push "${PUSH_LOCAL_PATH}" "${PUSH_REMOTE_PATH}"; then
        echo "Have uploaded ${PUSH_LOCAL_PATH} to ${PUSH_REMOTE_PATH}"
    else
        echo "Failed to upload ${PUSH_LOCAL_PATH} to ${PUSH_REMOTE_PATH}"
        exit 1
    fi

    # Secondly, we copy the gdbserver from "/data/local/tmp" to app internal data path
    COPY_SRC_PATH="${PUSH_REMOTE_PATH}"
    COPY_DEST_DIR="$(dirname "${COPY_DEST_PATH}")"
    "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" mkdir -p "${COPY_DEST_DIR}""
    if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" cp -f "${COPY_SRC_PATH}" "${COPY_DEST_PATH}""; then
        echo "Have copied ${COPY_SRC_PATH} to ${COPY_DEST_PATH}"
    else
        echo "Failed to copy ${COPY_SRC_PATH} to ${COPY_DEST_PATH}"
        exit 1
    fi

    # Thirdly, we chmod the gdbserver
    if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" chmod 711 "${COPY_DEST_PATH}""; then
        echo "Have chmod ${COPY_DEST_PATH}"
    else
        echo "Failed to chmod ${DESTINATION}"
        exit 1
    fi
fi

# 
if "${ADB_CMD}" shell "ls /system/bin/readlink 1>/dev/null 2>/dev/null"; then
    if test "$("${ADB_CMD}" shell "readlink /system/bin/ps")" = "toolbox"; then
        PS_SCRIPT="ps"
    else
        PS_SCRIPT="ps -w"
    fi
else
    PS_SCRIPT="ps"
fi

# Kill the gdbserver if requested.
KILL_PIDS=$("${ADB_CMD}" shell ${PS_SCRIPT} | grep "${APP_GDBSERVER_PATH}" | awk '{print $2}' | xargs) 
if test '!' '(' '-z' "${KILL_PIDS}" ')'; then
    if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" kill ${KILL_PIDS}"; then # SIGKILL not support
        echo "Killed ${KILL_PIDS}"
    else
        echo "Failed to kill ${KILL_PIDS}"
        exit 1
    fi  
fi

# Kill the process if requested.
KILL_PIDS=$("${ADB_CMD}" shell ${PS_SCRIPT} | grep "${PACKAGE_NAME}" | awk '{print $2}' | xargs) 
if test '!' '(' '-z' "${KILL_PIDS}" ')'; then
    if "${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" kill ${KILL_PIDS}"; then # SIGKILL not support
        echo "Killed ${KILL_PIDS}"
    else
        echo "Failed to kill ${KILL_PIDS}"
        exit 1
    fi  
fi

# wait the kill since we don't have SIGKILL
sleep ${DELAY} 

PIDS=$("${ADB_CMD}" shell ${PS_SCRIPT} | grep "${APP_GDBSERVER_PATH}" | awk '{print $2}' | xargs) 
LEN_PIDS=0
for PID in ${PIDS}
do
   LEN_PIDS=$((LEN_PIDS+1))
done

if test '!' '(' 0 -eq ${LEN_PIDS} ')'; then
    echo "Failed to kill "${APP_GDBSERVER_PATH}"" # we may increase the delay
    exit 1
fi

# Launch the application if needed, and get its pid
COMPONENT_NAME="${PACKAGE_NAME}/${LAUNCH_ACTIVITY_NAME}"
if "${ADB_CMD}" shell "am start ${COMPONENT_NAME}"; then # -D # wait java 
    echo "Launching activity ${COMPONENT_NAME}..."
else
    echo "Failed to start ${COMPONENT_NAME}"
    exit 1
fi 

# we don't use "wait java" method 
# we connect to the gdbserver when we lock the screen of the phone
# and when we unlock the screen of the phone after the gdbserver is connected

# wait the activity to launch
sleep ${DELAY} 

PIDS=$("${ADB_CMD}" shell ${PS_SCRIPT} | grep "${PACKAGE_NAME}" | awk '{print $2}' | xargs) 
LEN_PIDS=0
for PID in ${PIDS}
do
   LEN_PIDS=$((LEN_PIDS+1))
done

if test 0 -eq ${LEN_PIDS}; then
    echo "Failed to find running process "${PACKAGE_NAME}"" # you may increase the delay
    exit 1
fi

if test 1 -lt ${LEN_PIDS}; then
    echo "Multiple running processes named "${PACKAGE_NAME}""
    exit 1
fi
PID=${PIDS}

# Pull the zygote
ZYGOTE_REMOTE_PATH="/system/bin/app_process64"
ZYGOTE_LOCAL_PATH="${OUT_DIR}/app_process64"
if "${ADB_CMD}" pull "${ZYGOTE_REMOTE_PATH}" "${ZYGOTE_LOCAL_PATH}"; then
    echo "Pulled "${ZYGOTE_REMOTE_PATH}" to "${ZYGOTE_LOCAL_PATH}""
else
    echo "Failed to pull "${ZYGOTE_REMOTE_PATH}" to "${ZYGOTE_LOCAL_PATH}""
    exit 1
fi

#
# Start gdbserver in the background and forward necessary ports.
#
# Args:
#   device: ADB device to start gdbserver on.
#   gdbserver_local_path: Host path to push gdbserver from, can be None.
#   gdbserver_remote_path: Device path to push gdbserver to.
#   target_pid: PID of device process to attach to.
#   run_cmd: Command to run on the device.
#   debug_socket: Device path to place gdbserver unix domain socket.
#   port: Host port to forward the debug_socket to.
#   user: Device user to run gdbserver as.
#
#   Returns:
#       Popen handle to the `adb shell` process gdbserver was started with.
#
DEBUG_SOCKET="${APP_PWD_DIR}/debug_socket"

# remove the old one
"${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" rm -rf ${DEBUG_SOCKET}"

if "${ADB_CMD}" forward "tcp:${PORT}" "localfilesystem:${DEBUG_SOCKET}"; then
    echo "Starting gdbserver..."
else
    echo "Failed to forward "tcp:${PORT}" "localfilesystem:${DEBUG_SOCKET}"."
    exit 1
fi
trap ""${ADB_CMD}" forward --remove "tcp:${PORT}"" EXIT

GDBSERVER_REMOTE_PATH=${APP_GDBSERVER_PATH}
TARGET_PID=${PID}
"${ADB_CMD}" shell "run-as "${PACKAGE_NAME}" ${GDBSERVER_REMOTE_PATH} --once +"${DEBUG_SOCKET}" --attach ${TARGET_PID}"

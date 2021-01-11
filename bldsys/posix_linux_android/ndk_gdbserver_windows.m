%
% Copyright (C) YuqiaoZhang(HanetakaYuminaga)
% 
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU Lesser General Public License as published
% by the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU Lesser General Public License for more details.
% 
% You should have received a copy of the GNU Lesser General Public License
% along with this program.  If not, see <https://www.gnu.org/licenses/>.
%

MY_DIR=fileparts(mfilename("fullpathext"))

% https://developer.android.com/studio/debug#debug-types
% android-ndk-r14b/prebuilt/windows-x86_64/bin/ndk-gdb.py
% android-ndk-r14b/python-packages/gdbrunner/__init__.py
% gdbrunner.get_run_as_cmd

PACKAGE_NAME="YuqiaoZhang.HanetakaYuminaga.PatriotEngine"
LAUNCH_ACTIVITY_NAME="android.app.NativeActivity"
ADB_CMD=cstrcat(MY_DIR,"/android-sdk/platform-tools/adb")
GDBSERVER_CMD=cstrcat(MY_DIR,"/bin/arm64-v8a/gdbserver")
ARCH="arm64"
DELAY=0.25

CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " sh -c \'pwd\' 2>/dev/null\"")
[CMD_STATUS, DATA_DIR]=system(CMD)
DATA_DIR=strtrim(DATA_DIR)
if 0 != CMD_STATUS || isempty(DATA_DIR)
  disp("Could not find application's data directory. Are you sure that the application is installed and debuggable?")
  exit 1
endif

% Applications with minSdkVersion >= 24 will have their data directories
% created with rwx------ permissions, preventing adbd from forwarding to
% the gdbserver socket. To be safe, if we're on a device >= 24, always
% chmod the directory.
CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " chmod 711 ", DATA_DIR,"\"")
CMD_STATUS=system(CMD)
if 0 != CMD_STATUS
  disp("Failed to make application data directory world executable")
  exit 1
endif
APP_DATA_DIR=DATA_DIR

APP_GDBSERVER_PATH=cstrcat(APP_DATA_DIR,"/lib/",ARCH,"/gdbserver")
CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " ls ", APP_GDBSERVER_PATH," 1>/dev/null 2>/dev/null\"")
CMD_STATUS=system(CMD)
if 0 == CMD_STATUS
  disp(cstrcat("Found app gdbserver: ", APP_GDBSERVER_PATH))
else
  % We need to upload our gdbserver
  LOCAL_PATH=GDBSERVER_CMD
  REMOTE_PATH=cstrcat("/data/local/tmp/", APP_GDBSERVER_PATH)
  CMD=cstrcat(ADB_CMD," push ", LOCAL_PATH, " ", REMOTE_PATH)
  CMD_STATUS=system(CMD)
  if 0 == CMD_STATUS
    disp(cstrcat("App gdbserver not found at ", APP_GDBSERVER_PATH," uploaded to ", REMOTE_PATH, "."))
  else
    disp(cstrcat("Failed to upload gdbserver to ", REMOTE_PATH,"."))
    exit 1
  endif
  
  % Copy gdbserver into the data directory on M+, because selinux prevents
  % execution of binaries directly from /data/local/tmp.
  DESTINATION=APP_GDBSERVER_PATH
  CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " mkdir -p ", fileparts(DESTINATION), "\"")
  CMD_STATUS=system(CMD)
  if 0 == CMD_STATUS
    disp(cstrcat("Mkdir ", fileparts(DESTINATION), "."))
  else
    disp(cstrcat("Failed to mkdir ", fileparts(DESTINATION), "."))
    exit 1
  endif
  
  CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " sh -c \'cat ", REMOTE_PATH, " | cat > ", DESTINATION,"\'\"")
  CMD_STATUS=system(CMD)
  if 0 == CMD_STATUS
    disp(cstrcat("Copied gdbserver to ", DESTINATION, "."))
  else
    disp(cstrcat("Failed to copy gdbserver to ", DESTINATION, "."))
    exit 1
  endif
  
  CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " chmod 711 ", DESTINATION,"\"")
  CMD_STATUS=system(CMD)
  if 0 == CMD_STATUS
    disp(cstrcat("Uploaded gdbserver to ", DESTINATION, "."))
  else
    disp(cstrcat("Failed to chmod gdbserver at ", DESTINATION, "."))
    exit 1
  endif
endif

CMD=cstrcat(ADB_CMD," shell \"ls /system/bin/readlink 1>/dev/null 2>/dev/null\"")
CMD_STATUS=system(CMD)
if 0 == CMD_STATUS
  CMD=cstrcat(ADB_CMD," shell \"readlink /system/bin/ps\"")
  [CMD_STATUS, CMD_OUTPUT]=system(CMD)
  if 0 == CMD_STATUS && 0 == strcmp(CMD_OUTPUT, "toolbox")
    PS_SCRIPT="ps"
  else
    PS_SCRIPT="ps -w"
  endif
else
  PS_SCRIPT="ps"
endif

% Kill the gdbserver if requested.
CMD=cstrcat(ADB_CMD," shell sh -c \'", PS_SCRIPT, " | grep ", APP_GDBSERVER_PATH," | awk \'\"\'\"\'{print $2}\'\"\'\"\' | xargs\'") 
[CMD_STATUS, KILL_PIDS]=system(CMD)
KILL_PIDS=strtrim(KILL_PIDS);
KILL_PIDS=strsplit(KILL_PIDS)
if !(1==numel(KILL_PIDS) && isempty(KILL_PIDS{1}))
  for k = 1:numel(KILL_PIDS)
    CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " kill ", KILL_PIDS{k},"\"") % SIGKILL not support
    CMD_STATUS=system(CMD)
    if 0 == CMD_STATUS
       disp(cstrcat("Killed ", KILL_PIDS{k}, "."))
    else
       disp(cstrcat("Failed to kill ", KILL_PIDS{k}, "."))
       exit 1
    endif
  endfor
endif

% Kill the process if requested.
CMD=cstrcat(ADB_CMD," shell sh -c \'", PS_SCRIPT, " | grep ", PACKAGE_NAME," | awk \'\"\'\"\'{print $2}\'\"\'\"\' | xargs\'") 
[CMD_STATUS, KILL_PIDS]=system(CMD)
KILL_PIDS=strtrim(KILL_PIDS);
KILL_PIDS=strsplit(KILL_PIDS)
if !(1==numel(KILL_PIDS) && isempty(KILL_PIDS{1}))
  for k = 1:numel(KILL_PIDS)
    CMD=cstrcat(ADB_CMD," shell \"run-as ", PACKAGE_NAME, " kill ", KILL_PIDS{k},"\"") % SIGKILL not support
    CMD_STATUS=system(CMD)
    if 0 == CMD_STATUS
       disp(cstrcat("Killed ", KILL_PIDS{k}, "."))
    else
       disp(cstrcat("Failed to kill ", KILL_PIDS{k}, "."))
       exit 1
    endif
  endfor
endif

% wait the kill since we don't have SIGKILL
pause(DELAY)

CMD=cstrcat(ADB_CMD," shell sh -c \'", PS_SCRIPT, " | grep ", APP_GDBSERVER_PATH," | awk \'\"\'\"\'{print $2}\'\"\'\"\' | xargs\'") 
[CMD_STATUS, KILL_PIDS]=system(CMD)
KILL_PIDS=strtrim(KILL_PIDS);
KILL_PIDS=strsplit(KILL_PIDS)
if !(1==numel(KILL_PIDS) && isempty(KILL_PIDS{1}))
  disp(cstrcat("Failed to kill running process ", APP_GDBSERVER_PATH, ".")) # we may increase the delay
  exit 1
endif

% Launch the application if needed, and get its pid
COMPONENT_NAME=cstrcat(PACKAGE_NAME,'/', LAUNCH_ACTIVITY_NAME)
CMD=cstrcat(ADB_CMD," shell \"am start ",COMPONENT_NAME, " 2>&1\"") % -D % wait java
[CMD_STATUS, CMD_OUTPUT]=system(CMD)
CMD_OUTPUT=strtrim(CMD_OUTPUT)
if 0 == CMD_STATUS && isempty(strfind(CMD_OUTPUT,"Error"))
  disp(cstrcat("Launching activity ", COMPONENT_NAME, "..."))
else
  disp(cstrcat("Failed to start ", COMPONENT_NAME))
  exit 1
endif 
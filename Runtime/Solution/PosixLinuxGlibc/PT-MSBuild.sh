#!/bin/bash

if [ $# -eq 2 ]; then
    MY_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" 
    #mono $MY_DIR/mono/msbuild/MSBuild.dll 
    dotnet build $MY_DIR/PTSystem_PosixAndroid.vcxproj -property:VCTargetsPath=$MY_DIR/Microsoft/VCTargets/ -property:Configuration=$1 -property:Platform=$2
else
    echo Usage: PT-MSBuild.sh Configuration\(Debug\|Release\) Platform\(x86\|x64\)
fi
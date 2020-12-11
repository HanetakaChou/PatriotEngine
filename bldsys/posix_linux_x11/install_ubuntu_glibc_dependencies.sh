#!/bin/bash  

# 16.04.7 LTS
# cat /etc/os-release 

if test \( $# -ne 1 \);
then
    echo "Usage: build.sh platform"
    echo ""
    echo "Platforms:"
    echo "  x86     -   build with the x86 configuration"
    echo "  x64     -   build with the x64 configuration"
    echo ""
    exit 1
fi

if test ! \( \( \( -n "$1" \) -a \( "$1" = "x86" \) \) -o \( \( -n "$1" \) -a \( "$1" = "x64" \) \) \);
then
    echo "The platform \"$1\" is not supported!"
    echo ""
    echo "Platforms:"
    echo "  x86     -   build with the x86 configuration"
    echo "  x64     -   build with the x64 configuration"
    echo ""
    exit 1
fi



if test \( \( -n "$1" \) -a \( "$1" = "x86" \) \);
then
    apt install gcc
    apt install g++
    apt install clang
    apt install gdbserver
    apt install chrpath

    dpkg --add-architecture i386
    apt-get update

    apt install libc6-dev:i386
    apt install libstdc++-5-dev:i386
elif test \( \( -n "$1" \) -a \( "$1" = "x64" \) \);
then
    apt install libc6-dev
    apt install libstdc++-5-dev
    apt install gcc
    apt install g++
    apt install clang
    apt install gdbserver
    apt install chrpath
fi




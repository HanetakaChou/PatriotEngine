#!/bin/bash  

# 16.04.7 LTS
# cat /etc/os-release 

apt install libc6-dev
apt install libstdc++-5-dev
apt install gcc
apt install g++
apt install clang

dpkg --add-architecture i386
apt-get update

apt install libc6-dev:i386
apt install libstdc++-5-dev:i386

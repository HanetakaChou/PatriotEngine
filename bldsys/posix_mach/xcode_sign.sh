#!/bin/zsh

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

# Sign Xcode applications
# https://docs.github.com/en/actions/guides/installing-an-apple-certificate-on-macos-runners-for-xcode-development

MY_DIR="$(cd "$(dirname "$0")" 1>/dev/null 2>/dev/null && pwd)"  
cd ${MY_DIR}

# github secrets
# P12_PASSWORD
KEYCHAIN_PASSWORD="YuqiaoZhang.HanetakaYuminaga.PatriotEngine"

# create variables
CERTIFICATE_PATH="${MY_DIR}/developer_profile/166480D6762EFCB19E95462DEFB451ED37755575.p12"
PP_PATH_1="${MY_DIR}/developer_profile/95bd8b63-02b8-484e-8739-ec844e5e7046.mobileprovision"
PP_PATH_2="${MY_DIR}/developer_profile/699c3cb8-3a60-4c84-a874-430de787ad97.mobileprovision"
PP_PATH_3="${MY_DIR}/developer_profile/47305c36-bf6b-4b15-ba8b-a464bb186101.mobileprovision"
PP_PATH_4="${MY_DIR}/developer_profile/de40ac22-2b60-4e08-8a31-f68447bc7c9a.mobileprovision"         
KEYCHAIN_PATH="${MY_DIR}/accounts.keychain"

# create temporary keychain
security create-keychain -p "${KEYCHAIN_PASSWORD}" ${KEYCHAIN_PATH}
security set-keychain-settings -lut 21600 ${KEYCHAIN_PATH}
security unlock-keychain -p "${KEYCHAIN_PASSWORD}" ${KEYCHAIN_PATH}

# import certificate to keychain
security import ${CERTIFICATE_PATH} -P "${P12_PASSWORD}" -A -t cert -f pkcs12 -k ${KEYCHAIN_PATH}
security list-keychain -d user -s ${KEYCHAIN_PATH}

# apply provisioning profile
mkdir -p "${HOME}/Library/MobileDevice/Provisioning Profiles"
cp -f ${PP_PATH_1} "${HOME}/Library/MobileDevice/Provisioning Profiles"
cp -f ${PP_PATH_2} "${HOME}/Library/MobileDevice/Provisioning Profiles"
cp -f ${PP_PATH_3} "${HOME}/Library/MobileDevice/Provisioning Profiles"
cp -f ${PP_PATH_4} "${HOME}/Library/MobileDevice/Provisioning Profiles"
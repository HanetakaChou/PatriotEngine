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

# create variables
CERTIFICATE_PATH="${RUNNER_TEMP}/build_certificate.p12"
PP_PATH_1="${RUNNER_TEMP}/build_pp_1.mobileprovision"
PP_PATH_2="${RUNNER_TEMP}/build_pp_2.mobileprovision"
PP_PATH_3="${RUNNER_TEMP}/build_pp_3.mobileprovision"
PP_PATH_4="${RUNNER_TEMP}/build_pp_4.mobileprovision"         
KEYCHAIN_PATH="${RUNNER_TEMP}/app-signing.keychain-db"

# import certificate and provisioning profile from secrets
echo -n "$BUILD_CERTIFICATE_BASE64" | base64 --decode --output ${CERTIFICATE_PATH}
echo -n "$BUILD_PROVISION_PROFILE_BASE64_1" | base64 --decode --output ${PP_PATH_1}
echo -n "$BUILD_PROVISION_PROFILE_BASE64_2" | base64 --decode --output ${PP_PATH_2}
echo -n "$BUILD_PROVISION_PROFILE_BASE64_3" | base64 --decode --output ${PP_PATH_3}
echo -n "$BUILD_PROVISION_PROFILE_BASE64_4" | base64 --decode --output ${PP_PATH_4}

# create temporary keychain
security create-keychain -p "${KEYCHAIN_PASSWORD}" ${KEYCHAIN_PATH}
security set-keychain-settings -lut 21600 ${KEYCHAIN_PATH}
security unlock-keychain -p "${KEYCHAIN_PASSWORD}" ${KEYCHAIN_PATH}

# import certificate to keychain
security import ${CERTIFICATE_PATH} -P "${P12_PASSWORD}" -A -t cert -f pkcs12 -k ${KEYCHAIN_PATH}
security list-keychain -d user -s ${KEYCHAIN_PATH}

# apply provisioning profile
mkdir -p "~/Library/MobileDevice/Provisioning Profiles"
cp ${PP_PATH_1} "~/Library/MobileDevice/Provisioning Profiles"
cp ${PP_PATH_2} "~/Library/MobileDevice/Provisioning Profiles"
cp ${PP_PATH_3} "~/Library/MobileDevice/Provisioning Profiles"
cp ${PP_PATH_4} "~/Library/MobileDevice/Provisioning Profiles"
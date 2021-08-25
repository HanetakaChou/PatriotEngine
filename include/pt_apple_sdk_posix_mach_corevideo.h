//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_APPLE_SDK_POSIX_MACH_COREVIDEO_H_
#define _PT_APPLE_SDK_POSIX_MACH_COREVIDEO_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_apple_sdk_common.h"

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreVideo.framework/Headers/CVDisplayLink.h */
typedef int32_t CVReturn;
enum : CVReturn
{
    kCVReturnSuccess = 0
};
typedef uint64_t CVOptionFlags;
typedef struct _CVDisplayLink_T_ *CVDisplayLinkRef;
typedef CVReturn (*CVDisplayLinkOutputCallback)(CVDisplayLinkRef displayLink, struct CVTimeStamp const *inNow, struct CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext);

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK CVReturn __attribute__((availability(macos, introduced = 10.4))) CVDisplayLink_CreateWithActiveCGDisplays(CVDisplayLinkRef *displayLinkOut);
    PT_ATTR_APPLE_SDK CVReturn __attribute__((availability(macos, introduced = 10.4))) CVDisplayLink_SetOutputCallback(CVDisplayLinkRef displayLink, CVDisplayLinkOutputCallback callback, void *userInfo);
    PT_ATTR_APPLE_SDK CVReturn __attribute__((availability(macos, introduced = 10.4))) CVDisplayLink_Start(CVDisplayLinkRef displayLink);
    PT_ATTR_APPLE_SDK CVReturn __attribute__((availability(macos, introduced = 10.4))) CVDisplayLink_Stop(CVDisplayLinkRef displayLink);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.4))) CVDisplayLink_Release(CVDisplayLinkRef displayLink);
#ifdef __cplusplus
}
#endif

#endif

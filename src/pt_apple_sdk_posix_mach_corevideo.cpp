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

#include <pt_apple_sdk_posix_mach_objc.h>
#include <pt_apple_sdk_posix_mach_corevideo.h>

extern "C" CVReturn CVDisplayLinkCreateWithActiveCGDisplays(CVDisplayLinkRef *displayLinkOut);

PT_ATTR_APPLE_SDK CVReturn CVDisplayLink_CreateWithActiveCGDisplays(CVDisplayLinkRef *displayLinkOut)
{
    return CVDisplayLinkCreateWithActiveCGDisplays(displayLinkOut);
}

extern "C" CVReturn CVDisplayLinkSetOutputCallback(CVDisplayLinkRef displayLink, CVDisplayLinkOutputCallback callback, void *userInfo);

PT_ATTR_APPLE_SDK CVReturn CVDisplayLink_SetOutputCallback(CVDisplayLinkRef displayLink, CVDisplayLinkOutputCallback callback, void *userInfo)
{
    return CVDisplayLinkSetOutputCallback(displayLink, callback, userInfo);
}

extern "C" CVReturn CVDisplayLinkStart(CVDisplayLinkRef displayLink);

PT_ATTR_APPLE_SDK CVReturn CVDisplayLink_Start(CVDisplayLinkRef displayLink)
{
    return CVDisplayLinkStart(displayLink);
}

extern "C" CVReturn CVDisplayLinkStop(CVDisplayLinkRef displayLink);

PT_ATTR_APPLE_SDK CVReturn CVDisplayLink_Stop(CVDisplayLinkRef displayLink)
{
    return CVDisplayLinkStop(displayLink);
}

extern "C" void CVDisplayLinkRelease(CVDisplayLinkRef displayLink);

PT_ATTR_APPLE_SDK void CVDisplayLink_Release(CVDisplayLinkRef displayLink)
{
    return CVDisplayLinkRelease(displayLink);
}

/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _PT_APPLE_SDK_POSIX_MACH_APPKIT_H_
#define _PT_APPLE_SDK_POSIX_MACH_APPKIT_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_apple_sdk_common.h"
#include "pt_apple_sdk_posix_mach_objc.h"

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSApplication.h */
typedef struct _Class_NSApplicationDelegate_T_ *Class_NSApplicationDelegate;
typedef struct _NSApplicationDelegate_T_ *NSApplicationDelegate;
typedef struct _NSApplicationDelegate_applicationDidFinishLaunching__T_ *NSApplicationDelegate_applicationDidFinishLaunching_;
typedef struct _NSApplicationDelegate_applicationWillTerminate__T_ *NSApplicationDelegate_applicationWillTerminate_;
typedef struct _NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed__T_ *NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_;
typedef struct _NSApplication_T_ *NSApplication;
PT_ATTR_APPLE_SDK Class_NSApplicationDelegate NSApplicationDelegate_allocateClass(
    char const *class_name,
    void (*_I_NSApplicationDelegate_applicationDidFinishLaunching_)(NSApplicationDelegate, NSApplicationDelegate_applicationDidFinishLaunching_, void *aNotification),
    void (*_I_NSApplicationDelegate_applicationWillTerminate_)(NSApplicationDelegate, NSApplicationDelegate_applicationWillTerminate_, void *aNotification),
    int8_t (*_I_NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_)(NSApplicationDelegate, NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_, NSApplication sender));
PT_ATTR_APPLE_SDK NSApplicationDelegate NSApplicationDelegate_alloc(Class_NSApplicationDelegate class_ns_application_delegate);
PT_ATTR_APPLE_SDK NSApplicationDelegate NSApplicationDelegate_init(NSApplicationDelegate ns_application_delegate);
PT_ATTR_APPLE_SDK NSApplication NSApplication_sharedApplication();
PT_ATTR_APPLE_SDK void NSApplication_setDelegate(NSApplication ns_application, NSApplicationDelegate ns_application_delegate);
PT_ATTR_APPLE_SDK int NSApplication_Main(int argc, char const *argv[]);

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreGraphics.framework/Headers/CGBase.h */
#if defined(__LP64__) && __LP64__
#define CGFLOAT_TYPE double
#else
#define CGFLOAT_TYPE float
#endif
typedef CGFLOAT_TYPE CGFloat;

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/NSGeometry.h */
typedef struct _NSPoint
{
    CGFloat x;
    CGFloat y;
} NSPoint;
typedef struct _NSSize
{
    CGFloat width;
    CGFloat height;
} NSSize;
typedef struct _NSRect
{
    NSPoint origin;
    NSSize size;
} NSRect;
static inline NSSize NSMakeSize(CGFloat w, CGFloat h)
{
    NSSize s;
    s.width = w;
    s.height = h;
    return s;
}
static inline NSRect NSMakeRect(CGFloat x, CGFloat y, CGFloat w, CGFloat h)
{
    NSRect r;
    r.origin.x = x;
    r.origin.y = y;
    r.size.width = w;
    r.size.height = h;
    return r;
}

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSScreen.h */
typedef struct _NSScreen_T_ *NSScreen;
PT_ATTR_APPLE_SDK NSScreen NSScreen_mainScreen();
PT_ATTR_APPLE_SDK NSRect NSScreen_frame(NSScreen ns_screen);

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSWindow.h */
typedef struct _Class_NSWindow_T_ *Class_NSWindow;
typedef struct _NSWindow_T_ *NSWindow;
typedef NSUInteger NSWindowStyleMask;
enum
{
    NSWindowStyleMaskTitled __attribute__((availability(macos, introduced = 10.12))) = 1 << 0,
    NSWindowStyleMaskClosable __attribute__((availability(macos, introduced = 10.12))) = 1 << 1,
    NSWindowStyleMaskMiniaturizable __attribute__((availability(macos, introduced = 10.12))) = 1 << 2,
    NSWindowStyleMaskResizable __attribute__((availability(macos, introduced = 10.12))) = 1 << 3
};
typedef NSUInteger NSBackingStoreType;
enum
{
    NSBackingStoreBuffered __attribute__((availability(macos, introduced = 10.0))) = 2
};
PT_ATTR_APPLE_SDK NSWindow NSWindow_alloc();
PT_ATTR_APPLE_SDK NSWindow __attribute__((availability(macos, introduced = 10.0))) NSWindow_initWithContentRect(NSWindow ns_window, NSRect ns_rect, NSWindowStyleMask ns_window_style_mask, NSBackingStoreType ns_backing_store_type, bool flag, NSScreen ns_screen);

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSViewController.h */
typedef struct _Class_NSViewController_T_ *Class_NSViewController;

#endif
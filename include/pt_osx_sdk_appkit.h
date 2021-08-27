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

#ifndef _PT_APPLE_SDK_POSIX_MACH_APPKIT_H_
#define _PT_APPLE_SDK_POSIX_MACH_APPKIT_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_apple_sdk_common.h"
#include "pt_osx_sdk_objc.h"
#include "pt_osx_sdk_foundation.h"

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSApplication.h */
typedef struct _NSApplication_T_ *NSApplication;
typedef struct _Class_NSApplicationDelegate_T_ *Class_NSApplicationDelegate;
typedef struct _NSApplicationDelegate_T_ *NSApplicationDelegate;
typedef struct _NSApplicationDelegate_applicationDidFinishLaunching__T_ *NSApplicationDelegate_applicationDidFinishLaunching_;
typedef struct _NSApplicationDelegate_applicationWillTerminate__T_ *NSApplicationDelegate_applicationWillTerminate_;
typedef struct _NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed__T_ *NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK NSApplication NSApplication_sharedApplication();
    PT_ATTR_APPLE_SDK void NSApplication_setDelegate(NSApplication ns_application, NSApplicationDelegate ns_application_delegate);
    PT_ATTR_APPLE_SDK int NSApplication_Main(int argc, char const *argv[]);
    PT_ATTR_APPLE_SDK Class_NSApplicationDelegate NSApplicationDelegate_allocateClass(
        char const *class_name,
        void (*_I_NSApplicationDelegate_applicationDidFinishLaunching_)(NSApplicationDelegate, NSApplicationDelegate_applicationDidFinishLaunching_, void *aNotification),
        void (*_I_NSApplicationDelegate_applicationWillTerminate_)(NSApplicationDelegate, NSApplicationDelegate_applicationWillTerminate_, void *aNotification),
        int8_t (*_I_NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_)(NSApplicationDelegate, NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_, NSApplication sender));
    PT_ATTR_APPLE_SDK void Class_NSApplicationDelegate_register(Class_NSApplicationDelegate class_ns_application_delegate);
    PT_ATTR_APPLE_SDK NSApplicationDelegate NSApplicationDelegate_alloc(Class_NSApplicationDelegate class_ns_application_delegate);
    PT_ATTR_APPLE_SDK NSApplicationDelegate NSApplicationDelegate_init(NSApplicationDelegate ns_application_delegate);
#ifdef __cplusplus
}
#endif

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSScreen.h */
typedef struct _NSScreen_T_ *NSScreen;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK NSScreen NSScreen_mainScreen();
    PT_ATTR_APPLE_SDK NSRect NSScreen_frame(NSScreen ns_screen);
#ifdef __cplusplus
}
#endif

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

typedef struct _NSViewController_T_ *NSViewController;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK NSWindow NSWindow_alloc();
    PT_ATTR_APPLE_SDK NSWindow __attribute__((availability(macos, introduced = 10.0))) NSWindow_initWithContentRect(NSWindow ns_window, NSRect ns_rect, NSWindowStyleMask ns_window_style_mask, NSBackingStoreType ns_backing_store_type, bool flag, NSScreen ns_screen);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.0))) NSWindow_setContentViewController(NSWindow ns_window, NSViewController ns_view_controller);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.0))) NSWindow_makeKeyAndOrderFront(NSWindow ns_window, void *sender);
#ifdef __cplusplus
}
#endif

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSView.h */
typedef struct _Class_NSView_T_ *Class_NSView;
typedef struct _NSView_wantsLayer_T_ *NSView_wantsLayer;
typedef struct _NSView_makeBackingLayer_T_ *NSView_makeBackingLayer;
typedef struct _NSView_wantsUpdateLayer_T_ *NSView_wantsUpdateLayer;
typedef struct _NSView_T_ *NSView;
typedef struct _CALayer_T_ *CALayer;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK Class_NSView NSView_allocateClass(
        char const *class_name,
        signed char (*_I_NSView_wantsLayer)(NSView ns_view, NSView_wantsLayer),
        void *(*_I_NSView_makeBackingLayer)(NSView ns_view, NSView_makeBackingLayer),
        signed char (*_I_NSView_wantsUpdateLayer)(NSView ns_view, NSView_wantsUpdateLayer));
    PT_ATTR_APPLE_SDK void Class_NSView_register(Class_NSView class_ns_view);
    PT_ATTR_APPLE_SDK NSView NSView_alloc(Class_NSView class_ns_view);
    PT_ATTR_APPLE_SDK NSView NSView_initWithFrame(NSView ns_view, NSRect frame_rect);
    PT_ATTR_APPLE_SDK CALayer NSView_layer(NSView ns_view);
#ifdef __cplusplus
}
#endif

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSViewController.h */
typedef struct _Class_NSViewController_T_ *Class_NSViewController;
typedef struct _NSViewController_loadView_T_ *NSViewController_loadView;
typedef struct _NSViewController_viewDidLoad_T_ *NSViewController_viewDidLoad;
typedef struct _NSViewController_setRepresentedObject__T_ *NSViewController_setRepresentedObject_;
#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK Class_NSViewController NSViewController_allocateClass(
        char const *class_name,
        void (*_I_NSViewController_loadView)(NSViewController ns_view_controller, NSViewController_loadView),
        void (*_I_NSViewController_viewDidLoad)(NSViewController ns_view_controller, NSViewController_viewDidLoad),
        void (*_I_NSViewController_setRepresentedObject_)(NSViewController ns_view_controller, NSViewController_setRepresentedObject_, void *represented_object));
    PT_ATTR_APPLE_SDK bool Class_NSViewController_addIvarVoidPointer(Class_NSViewController class_ns_view_controller, char const *ivarname);
    PT_ATTR_APPLE_SDK void Class_NSViewController_register(Class_NSViewController class_ns_view_controller);
    PT_ATTR_APPLE_SDK NSViewController NSViewController_alloc(Class_NSViewController class_ns_view_controller);
    PT_ATTR_APPLE_SDK NSViewController __attribute__((availability(macos, introduced = 10.5))) NSViewController_initWithNibName(NSViewController ns_view_controller, void *nibNameOrNil, void *nibBundleOrNil);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.5))) NSViewController_setView(NSViewController ns_view_controller, NSView ns_view);
    PT_ATTR_APPLE_SDK void NSViewController_super_viewDidLoad(NSViewController ns_view_controller, NSViewController_viewDidLoad cmd);
    PT_ATTR_APPLE_SDK void NSViewController_super_setRepresentedObject_(NSViewController ns_view_controller, NSViewController_setRepresentedObject_ cmd, void *represented_object);
    PT_ATTR_APPLE_SDK void NSViewController_setIvarVoidPointer(NSViewController ns_view_controller, char const *ivarname, void *pVoid);
    PT_ATTR_APPLE_SDK void *NSViewController_getIvarVoidPointer(NSViewController ns_view_controller, char const *ivarname);
#ifdef __cplusplus
}
#endif

#endif

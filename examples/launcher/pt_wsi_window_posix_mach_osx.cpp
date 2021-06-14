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

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_thread.h>
#include <assert.h>

#include <pt_apple_sdk_posix_mach_objc.h>
#include <pt_apple_sdk_posix_mach_foundation.h>
#include <pt_apple_sdk_posix_mach_appkit.h>
#include <pt_apple_sdk_posix_mach_metalkit.h>
#include <pt_apple_sdk_posix_mach_metal.h>

class ns_application_delegate
{
public:
    static void application_did_finish_launching(NSApplicationDelegate, NSApplicationDelegate_applicationDidFinishLaunching_, void *aNotification);
    static void application_will_terminate(NSApplicationDelegate, NSApplicationDelegate_applicationWillTerminate_, void *aNotification);
    static int8_t application_should_terminate_after_last_window_closed(NSApplicationDelegate, NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_, NSApplication sender);
};

class ns_view_controller
{
public:
    static void load_view(NSViewController ns_view_controller, NSViewController_loadView);
    static void view_did_load(NSViewController ns_view_controller, NSViewController_viewDidLoad);
    static void set_represented_object(NSViewController ns_view_controller, NSViewController_setRepresentedObject_, void *represented_object);
};

class mtk_view_delegate
{
public:
    static void drawable_size_will_change(MTKViewDelegate mtk_view_delegate, MTKViewDelegate_drawSizeWillChange_, MTKView mtk_view, CGSize size);
    static void draw_in_mtk_view(MTKViewDelegate mtk_view_delegate, MTKViewDelegate_drawInMTKView_, MTKView mtk_view);
};

int main(int argc, char const *argv[])
{
    //Enable MultiThreaded
    {
        // Using Autorelease Pool Blocks
        // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html

        void *__here_auto_release_pool_object = AutoReleasePool_Push();

        bool __here_ns_thread_detach_target_has_finished = false;

        Class_NSThreadDetachTarget class_ns_thread_detach_target = NSThreadDetachTarget_allocateClass(
            "NSThreadDetachTarget_pt_wsi_window_posix_mach_osx",
            "NSMain_pt_wsi_window_posix_mach_osx",
            [](NSThreadDetachTarget, NSThreadDetachSelector_, void *__here_ns_thread_detach_target_has_finished) -> void
            {
                (*static_cast<bool *>(__here_ns_thread_detach_target_has_finished)) = true;
            });

        NSThreadDetachTarget ns_thread_detach_target = NSThreadDetachTarget_init(NSThreadDetachTarget_alloc(class_ns_thread_detach_target));

        NSThread_detachNewThreadSelector("NSMain_pt_wsi_window_posix_mach_osx", ns_thread_detach_target, &__here_ns_thread_detach_target_has_finished);

        while (!__here_ns_thread_detach_target_has_finished)
        {
            mcrt_os_yield();
        }

        NSThreadDetachTarget_release(ns_thread_detach_target);

        assert(NSThread_isMultiThreaded() != false);

        AutoReleasePool_Pop(__here_auto_release_pool_object);
    }

    //Register NSApplicationDelegate
    {
        void *__here_auto_release_pool_object = AutoReleasePool_Push();

        Class_NSApplicationDelegate class_ns_application_delegate = NSApplicationDelegate_allocateClass(
            "NSApplicationDelegate_pt_wsi_window_posix_mach_osx",
            ns_application_delegate::application_did_finish_launching,
            ns_application_delegate::application_will_terminate,
            ns_application_delegate::application_should_terminate_after_last_window_closed);

        NSApplicationDelegate ns_application_delegate = NSApplicationDelegate_init(NSApplicationDelegate_alloc(class_ns_application_delegate));

        NSApplication ns_application = NSApplication_sharedApplication();

        NSApplication_setDelegate(ns_application, ns_application_delegate);

        AutoReleasePool_Pop(__here_auto_release_pool_object);
    }

    return NSApplication_Main(argc, argv);
}

void ns_application_delegate::application_did_finish_launching(NSApplicationDelegate, NSApplicationDelegate_applicationDidFinishLaunching_, void *aNotification)
{
    NSSize ns_size_window = NSMakeSize(1280, 720);

    NSScreen ns_screen = NSScreen_mainScreen();

    NSSize ns_size_screen = NSScreen_frame(ns_screen).size;

    NSRect ns_rect = NSMakeRect((ns_size_screen.width - ns_size_window.width) / 2,
                                (ns_size_screen.height - ns_size_window.height) / 2,
                                ns_size_window.width,
                                ns_size_window.height);

    NSWindow ns_window = NSWindow_initWithContentRect(
        NSWindow_alloc(),
        ns_rect,
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable,
        NSBackingStoreBuffered,
        false,
        ns_screen);

    Class_NSViewController class_ns_view_controller = NSViewController_allocateClass(
        "NSViewController_pt_wsi_window_posix_mach_osx",
        ns_view_controller::load_view,
        ns_view_controller::view_did_load,
        ns_view_controller::set_represented_object);

    Class_NSViewController_addIvarVoidPointer(class_ns_view_controller, "pUserData");

    NSViewController ns_view_controller = NSViewController_initWithNibName(NSViewController_alloc(class_ns_view_controller), NULL, NULL);

    NSWindow_setContentViewController(ns_window, ns_view_controller);

    NSWindow_makeKeyAndOrderFront(ns_window, NULL);
}

void ns_application_delegate::application_will_terminate(NSApplicationDelegate, NSApplicationDelegate_applicationWillTerminate_, void *aNotification)
{
}

int8_t ns_application_delegate::application_should_terminate_after_last_window_closed(NSApplicationDelegate, NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_, NSApplication sender)
{
    return 1;
}

void ns_view_controller::load_view(NSViewController ns_view_controller, NSViewController_loadView)
{
    MTLDevice device = NULL;
    {
        NSArray devices = MTLDevice_CopyAllDevices();
        NSUInteger count = NSArray_count(devices);
        for (NSUInteger idx = 0U; idx < count; ++idx)
        {
            MTLDevice current_device = NSObject_To_MTLDevice(NSArray_objectAtIndexedSubscript(devices, idx));
            bool hasUnifiedMemory = MTLDevice_hasUnifiedMemory(current_device);
            if (!hasUnifiedMemory)
            {
                device = current_device;
                break;
            }
        }
        NSArray_release(devices);
    }

    CGRect frame = {{0, 0}, {800, 600}};
    MTKView mtk_view = MTKView_initWithFrame(MTKView_alloc(), frame, device);

    Class_MTKViewDelegate class_mtk_view_delegate = MTKViewDelegate_allocClass(
        "MTKViewDelegate_pt_wsi_window_posix_mach_osx",
        mtk_view_delegate::drawable_size_will_change,
        mtk_view_delegate::draw_in_mtk_view);

    Class_MTKViewDelegate_addIvarVoidPointer(class_mtk_view_delegate, "pUserData");

    MTKViewDelegate mtk_view_delegate = MTKViewDelegate_init(MTKViewDelegate_alloc(class_mtk_view_delegate));
    //MTKViewDelegate_setIvarVoidPointer

    MTKView_setDelegate(mtk_view, mtk_view_delegate);

    NSViewController_setView(ns_view_controller, MTKView_To_NSView(mtk_view));
}

void ns_view_controller::view_did_load(NSViewController ns_view_controller, NSViewController_viewDidLoad cmd)
{
    NSViewController_super_viewDidLoad(ns_view_controller, cmd);
}

void ns_view_controller::set_represented_object(NSViewController ns_view_controller, NSViewController_setRepresentedObject_ cmd, void *represented_object)
{
    NSViewController_super_setRepresentedObject_(ns_view_controller, cmd, represented_object);
}

void mtk_view_delegate::drawable_size_will_change(MTKViewDelegate mtk_view_delegate, MTKViewDelegate_drawSizeWillChange_, MTKView mtk_view, CGSize size)
{
}

void mtk_view_delegate::draw_in_mtk_view(MTKViewDelegate mtk_view_delegate, MTKViewDelegate_drawInMTKView_, MTKView mtk_view)
{
}

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
#include <pt_apple_sdk_posix_mach_corevideo.h>
#include <pt_apple_sdk_posix_mach_dispatch.h>

#include <pt_gfx_connection.h>

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

        Class_NSApplicationDelegate_register(class_ns_application_delegate);

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

    Class_NSViewController_addIvarVoidPointer(class_ns_view_controller, "ns_view");
    Class_NSViewController_addIvarVoidPointer(class_ns_view_controller, "dispatch_source");

    Class_NSViewController_register(class_ns_view_controller);

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
    Class_NSView class_ns_view = NSView_allocateClass("CAMetalLayer_View", NULL, NULL, NULL);

    // The class seems incomplete without being registered and the MoltenVK would crash
    Class_NSView_register(class_ns_view);

    NSRect frame_rect = {{0, 0}, {800, 600}};
    NSView ns_view = NSView_initWithFrame(NSView_alloc(class_ns_view), frame_rect);

    NSViewController_setIvarVoidPointer(ns_view_controller, "ns_view", ns_view);

    NSViewController_setView(ns_view_controller, ns_view);
}

static CVReturn CVDisplayLink_Output_Callback(
    CVDisplayLinkRef displayLink,
    struct CVTimeStamp const *inNow, struct CVTimeStamp const *inOutputTime,
    CVOptionFlags flagsIn, CVOptionFlags *flagsOut,
    void *displayLinkContext);

static void MainThread_EventHandler(void *pUserData);

void ns_view_controller::view_did_load(NSViewController ns_view_controller, NSViewController_viewDidLoad cmd)
{
    NSViewController_super_viewDidLoad(ns_view_controller, cmd);

    // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
    
    dispatch_source_t dispatch_source = dispatch_create_source(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue());

    dispatch_set_source_event_handler(dispatch_source, MainThread_EventHandler, ns_view_controller);

    dispatch_resume_source(dispatch_source);

    NSViewController_setIvarVoidPointer(ns_view_controller, "dispatch_source", dispatch_source);

    CVDisplayLinkRef display_link;

    CVReturn cv_ret_create = CVDisplayLink_CreateWithActiveCGDisplays(&display_link);
    assert(kCVReturnSuccess == cv_ret_create);

    CVReturn cv_ret_set = CVDisplayLink_SetOutputCallback(display_link, CVDisplayLink_Output_Callback, ns_view_controller);
    assert(kCVReturnSuccess == cv_ret_set);

    CVReturn cv_ret_start = CVDisplayLink_Start(display_link);
    assert(kCVReturnSuccess == cv_ret_start);
}

void ns_view_controller::set_represented_object(NSViewController ns_view_controller, NSViewController_setRepresentedObject_ cmd, void *represented_object)
{
    NSViewController_super_setRepresentedObject_(ns_view_controller, cmd, represented_object);
}

static CVReturn CVDisplayLink_Output_Callback(
    CVDisplayLinkRef displayLink,
    struct CVTimeStamp const *inNow, struct CVTimeStamp const *inOutputTime,
    CVOptionFlags flagsIn, CVOptionFlags *flagsOut,
    void *displayLinkContext)
{
    NSViewController ns_view_controller = static_cast<NSViewController>(displayLinkContext);
    dispatch_source_t dispatch_source = static_cast<dispatch_source_t>(NSViewController_getIvarVoidPointer(ns_view_controller, "dispatch_source"));
    dispatch_merge_source_data(dispatch_source, 1);
    return kCVReturnSuccess;
}

static void MainThread_EventHandler(void *pUserData)
{
    NSViewController ns_view_controller = static_cast<NSViewController>(pUserData);

    NSView ns_view = static_cast<NSView>(NSViewController_getIvarVoidPointer(ns_view_controller, "ns_view"));
    CALayer ca_layer = NSView_layer(ns_view);
    
    return;
}

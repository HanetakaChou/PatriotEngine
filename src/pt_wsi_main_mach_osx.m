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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <dispatch/dispatch.h>
#include <CoreVideo/CoreVideo.h>
#include <QuartzCore/QuartzCore.h>
#include <pt_wsi_main.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_thread.h>
#include <pt_gfx_connection.h>

extern bool wsi_mach_osx_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg);
extern void wsi_mach_osx_os_yield(void);
extern bool wsi_mach_osx_atomic_load(bool volatile *src);
extern void wsi_mach_osx_atomic_store(bool volatile *dst, bool val);

// TODO GPU Capture doesn't work now
// Before macOS 10.15 and iOS 13.0, captureDesc will just be nil

@interface wsi_mach_osx_application_delegate : NSObject <NSApplicationDelegate>
- (void)applicationDidFinishLaunching:(NSNotification *)notification;
- (void)applicationWillTerminate:(NSNotification *)notification;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;
@end

@interface wsi_mach_osx_view_controller : NSViewController
- (void)loadView;
@end

@interface wsi_mach_osx_view : NSView
- (BOOL)wantsLayer;
- (CALayer *)makeBackingLayer;
- (BOOL)wantsUpdateLayer;
- (void)viewDidMoveToWindow;
- (void)draw_on_main_thread;
@end

@implementation wsi_mach_osx_application_delegate
- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    @autoreleasepool
    {
        CGFloat window_width = 1280.0f;
        CGFloat window_height = 720.0f;

        NSRect main_screen_rect = [[NSScreen mainScreen] frame];

        NSRect window_rect = NSMakeRect((main_screen_rect.size.width - window_width) * 0.5f + main_screen_rect.origin.x,
                                        (main_screen_rect.size.height - window_height) * 0.5f + main_screen_rect.origin.y,
                                        window_width,
                                        window_height);

        NSWindow *window = [[NSWindow alloc] initWithContentRect:window_rect styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable backing:NSBackingStoreBuffered defer:FALSE];

        NSViewController *view_controller = [[wsi_mach_osx_view_controller alloc] initWithNibName:nil bundle:nil];

        [window setContentViewController:view_controller];

        [window makeKeyAndOrderFront:nil];
    }
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    return;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return TRUE;
}
@end

@implementation wsi_mach_osx_view_controller
- (void)loadView
{
    @autoreleasepool
    {
        NSRect view_rect = {{0, 0}, {1280, 720}};

        NSView *view = [[wsi_mach_osx_view alloc] initWithFrame:view_rect];

        [self setView:view];
    }
}
@end

static CVReturn wsi_mach_osx_display_link_output_callback(CVDisplayLinkRef displayLink, CVTimeStamp const *inNow, CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext);

static pt_wsi_app_ref(PT_PTR *g_wsi_app_init_callback)(pt_gfx_connection_ref);
static int(PT_PTR *g_wsi_app_main_callback)(pt_wsi_app_ref);
struct app_main_argument_t
{
    pt_gfx_connection_ref m_gfx_connection;
    pt_wsi_app_ref m_wsi_app;
    bool m_running;
};
static void *app_main(void *argument);

@implementation wsi_mach_osx_view
{
    mcrt_native_thread_id m_app_main_thread_id;
    dispatch_source_t m_dispatch_source;
    pt_gfx_connection_ref m_gfx_connection;
    bool m_has_create_window;
    CVDisplayLinkRef m_display_link;
    pt_wsi_app_ref m_wsi_app;
}
- (BOOL)wantsLayer
{
    return YES;
}

- (CALayer *)makeBackingLayer
{
    @autoreleasepool
    {
        CALayer *layer = [CAMetalLayer layer];
        CGSize viewScale = [self convertSizeToBacking:CGSizeMake(1.0, 1.0)];
        layer.contentsScale = MIN(viewScale.width, viewScale.height);
        return layer;
    }
}

- (BOOL)wantsUpdateLayer
{
    return YES;
}

- (void)viewDidMoveToWindow
{
    [super viewDidMoveToWindow];

    @autoreleasepool
    {
        // Locating Items in the Standard Directories
        // https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/AccessingFilesandDirectories/AccessingFilesandDirectories.html
        char const *library_directory = [[[NSFileManager defaultManager] URLsForDirectory:NSLibraryDirectory inDomains:NSUserDomainMask][0] fileSystemRepresentation];

        //
        self->m_gfx_connection = pt_gfx_connection_init(NULL, NULL, library_directory);

        // nil layer
        self->m_has_create_window = false;

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        self->m_dispatch_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue());

        // Block Implementation Specification
        // https://clang.llvm.org/docs/Block-ABI-Apple.html
        void *const view_void = ((__bridge void *)self);
        dispatch_source_set_event_handler(self->m_dispatch_source, ^{
          [((wsi_mach_osx_view *)((__bridge id)view_void)) draw_on_main_thread];
        });

        dispatch_resume(self->m_dispatch_source);

        CVReturn res_display_link_create_with_active_cg_displays = CVDisplayLinkCreateWithActiveCGDisplays(&self->m_display_link);
        assert(kCVReturnSuccess == res_display_link_create_with_active_cg_displays);

        CVReturn res_display_link_set_output_callback = CVDisplayLinkSetOutputCallback(self->m_display_link, wsi_mach_osx_display_link_output_callback, ((__bridge void *)self->m_dispatch_source));
        assert(kCVReturnSuccess == res_display_link_set_output_callback);

        CVReturn ret_display_link_start = CVDisplayLinkStart(self->m_display_link);
        assert(kCVReturnSuccess == ret_display_link_start);

        // the "app_main"
        struct app_main_argument_t app_main_argument;
        app_main_argument.m_gfx_connection = self->m_gfx_connection;
        app_main_argument.m_wsi_app = NULL;
        wsi_mach_osx_atomic_store(&app_main_argument.m_running, false);

        PT_MAYBE_UNUSED bool res_native_thread_create = wsi_mach_osx_native_thread_create(&self->m_app_main_thread_id, app_main, &app_main_argument);
        assert(res_native_thread_create);

        while (!wsi_mach_osx_atomic_load(&app_main_argument.m_running))
        {
            wsi_mach_osx_os_yield();
        }

        assert(NULL != app_main_argument.m_wsi_app);
        self->m_wsi_app = app_main_argument.m_wsi_app;
    }
}

- (void)draw_on_main_thread
{
    @autoreleasepool
    {
        if (PT_LIKELY(self->m_has_create_window))
        {
            pt_gfx_connection_draw_acquire(self->m_gfx_connection);

            pt_gfx_connection_draw_release(self->m_gfx_connection);
        }
        else
        {
            CAMetalLayer *layer = ((CAMetalLayer *)[self layer]);
            if (nil != layer)
            {
                CGSize drawable_size = [layer drawableSize];
                pt_gfx_connection_on_wsi_window_created(self->m_gfx_connection, NULL, ((__bridge void *)layer), drawable_size.width, drawable_size.height);
                self->m_has_create_window = true;
            }
        }
    }
}
@end

static CVReturn wsi_mach_osx_display_link_output_callback(CVDisplayLinkRef displayLink, CVTimeStamp const *inNow, CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    @autoreleasepool
    {
        dispatch_source_t dispatch_source = ((__bridge dispatch_source_t)displayLinkContext);
        dispatch_source_merge_data(dispatch_source, 1);
        return kCVReturnSuccess;
    }
}

void *app_main(void *argument_void)
{
    pt_wsi_app_ref wsi_app;
    // app_init
    {
        struct app_main_argument_t *argument = ((struct app_main_argument_t *)argument_void);
        wsi_app = g_wsi_app_init_callback(argument->m_gfx_connection);
        argument->m_wsi_app = wsi_app;
        wsi_mach_osx_atomic_store(&argument->m_running, true);
    }

    // app_main
    int res_app_main_callback = g_wsi_app_main_callback(wsi_app);

    // mcrt_atomic_store(&self->m_loop, false);

    // wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

    return ((void *)((intptr_t)res_app_main_callback));
}

PT_ATTR_WSI int PT_CALL pt_wsi_main(int argc, char const *argv[], pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
    static __strong wsi_mach_osx_application_delegate *s_application_delegate = nil;
    @autoreleasepool
    {
        g_wsi_app_init_callback = wsi_app_init_callback;
        g_wsi_app_main_callback = wsi_app_main_callback;
        s_application_delegate = [[wsi_mach_osx_application_delegate alloc] init];
        ;
        [[NSApplication sharedApplication] setDelegate:s_application_delegate];
        return NSApplicationMain(argc, argv);
    }
}

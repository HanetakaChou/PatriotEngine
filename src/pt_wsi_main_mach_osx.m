//
// Copyright (C) YuqiaoZhang(HanetakaChou)
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
#include <pt_mcrt_thread.h>
#include <pt_gfx_connection.h>

extern bool mcrt_atomic_loadb(bool volatile *src);
extern void mcrt_atomic_storeb(bool volatile *dst, bool val);
static void draw_on_main_thread(void *argument);
static CVReturn display_link_output_callback(CVDisplayLinkRef displayLink, CVTimeStamp const *inNow, CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext);

static pt_gfx_connection_ref g_gfx_connection = NULL;

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
- (void)onWindowCreated;
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

@implementation wsi_mach_osx_view
{
    dispatch_source_t m_dispatch_source;
    CVDisplayLinkRef m_display_link;
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
    @autoreleasepool
    {
        [super viewDidMoveToWindow];

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        self->m_dispatch_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue());

        dispatch_resume(self->m_dispatch_source);

        // Block Implementation Specification
        // https://clang.llvm.org/docs/Block-ABI-Apple.html
        void *const view_void = ((__bridge void *)self);
        dispatch_source_set_event_handler(self->m_dispatch_source, ^{
          @autoreleasepool
          {
              [((wsi_mach_osx_view *)((__bridge id)view_void)) onWindowCreated];
          }
        });

        CGDirectDisplayID display_id =
            ((CGDirectDisplayID)[self.window.screen.deviceDescription[@"NSScreenNumber"] unsignedIntegerValue]);

        CVReturn res_display_link_create_with_cg_display = CVDisplayLinkCreateWithCGDisplay(display_id, &self->m_display_link);
        assert(kCVReturnSuccess == res_display_link_create_with_cg_display);

        CVReturn res_display_link_set_output_callback = CVDisplayLinkSetOutputCallback(self->m_display_link, display_link_output_callback, ((__bridge void *)self->m_dispatch_source));
        assert(kCVReturnSuccess == res_display_link_set_output_callback);

        CVReturn ret_display_link_start = CVDisplayLinkStart(self->m_display_link);
        assert(kCVReturnSuccess == ret_display_link_start);
    }
}

- (void)onWindowCreated
{
    @autoreleasepool
    {
        CAMetalLayer *layer = ((CAMetalLayer *)[self layer]);
        if (nil != layer)
        {
            CGSize drawable_size = [layer drawableSize];
            pt_gfx_connection_on_wsi_window_created(g_gfx_connection, NULL, ((__bridge void *)layer), drawable_size.width, drawable_size.height);

            void *const connection_void = g_gfx_connection;
            dispatch_source_set_event_handler(self->m_dispatch_source, ^{
              @autoreleasepool
              {
                  draw_on_main_thread(connection_void);
              }
            });
        }
    }
}
@end

struct app_main_argument_t
{
    pt_gfx_connection_ref m_gfx_connection;
    pt_wsi_app_ref m_wsi_app;
    bool m_app_main_running;
    int m_argc;
    char const **m_argv;
    pt_wsi_app_init_callback m_app_init_callback;
    pt_wsi_app_main_callback m_app_main_callback;
};
static void *app_main(void *argument);

PT_ATTR_WSI int PT_CALL pt_wsi_main(
    int argc, char const *argv[],
    pt_input_stream_init_callback cache_input_stream_init_callback, pt_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_input_stream_read_callback cache_input_stream_read_callback, pt_input_stream_destroy_callback cache_input_stream_destroy_callback,
    pt_output_stream_init_callback cache_output_stream_init_callback, pt_output_stream_write_callback cache_output_stream_write_callback, pt_output_stream_destroy_callback cache_output_stream_destroy_callback,
    pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback)
{
    static __strong wsi_mach_osx_application_delegate *application_delegate = nil;
    @autoreleasepool
    {
        // init
        {
            g_gfx_connection = pt_gfx_connection_init(
                NULL, NULL,
                cache_input_stream_init_callback, cache_input_stream_stat_size_callback, cache_input_stream_read_callback, cache_input_stream_destroy_callback,
                cache_output_stream_init_callback, cache_output_stream_write_callback, cache_output_stream_destroy_callback);

            // the "app_main"
            mcrt_native_thread_id app_main_thread_id;

            struct app_main_argument_t app_main_argument;
            app_main_argument.m_gfx_connection = g_gfx_connection;
            app_main_argument.m_wsi_app = NULL;
            app_main_argument.m_argc = argc;
            app_main_argument.m_argv = argv;
            app_main_argument.m_app_init_callback = app_init_callback;
            app_main_argument.m_app_main_callback = app_main_callback;
            mcrt_atomic_storeb(&app_main_argument.m_app_main_running, false);

            PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&app_main_thread_id, app_main, &app_main_argument);
            assert(res_native_thread_create);

            while (!mcrt_atomic_loadb(&app_main_argument.m_app_main_running))
            {
                mcrt_os_yield();
            }

            assert(NULL != app_main_argument.m_wsi_app);

            //
            application_delegate = [[wsi_mach_osx_application_delegate alloc] init];
        }

        // main
        [[NSApplication sharedApplication] setDelegate:application_delegate];
        return NSApplicationMain(argc, argv);
    }
}

static CVReturn display_link_output_callback(CVDisplayLinkRef displayLink, CVTimeStamp const *inNow, CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    @autoreleasepool
    {
        dispatch_source_t dispatch_source = ((__bridge dispatch_source_t)displayLinkContext);
        dispatch_source_merge_data(dispatch_source, 1);
        return kCVReturnSuccess;
    }
}

static void draw_on_main_thread(void *argument_void)
{
    pt_gfx_connection_ref gfx_connection = ((pt_gfx_connection_ref)argument_void);

    pt_gfx_connection_draw_acquire(gfx_connection);

    pt_gfx_connection_draw_release(gfx_connection);
}

static void *app_main(void *argument_void)
{
    pt_wsi_app_ref wsi_app = NULL;
    pt_wsi_app_main_callback app_main_callback = NULL;
    // app_init
    {
        struct app_main_argument_t *argument = ((struct app_main_argument_t *)argument_void);
        argument->m_wsi_app = argument->m_app_init_callback(argument->m_argc, argument->m_argv, argument->m_gfx_connection);

        wsi_app = argument->m_wsi_app;
        app_main_callback = argument->m_app_main_callback;

        mcrt_atomic_storeb(&argument->m_app_main_running, true);
    }

    // app_main
    int res_app_main_callback = app_main_callback(wsi_app);

    // mcrt_atomic_store(&self->m_loop, false);

    // wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

    return ((void *)((intptr_t)res_app_main_callback));
}

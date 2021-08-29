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

void get_mainbundle_resource_path(char *path, size_t *length)
{
    @autoreleasepool
    {
        char const *mainbundle_resource_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
        size_t mainbundle_resource_path_length = strlen(mainbundle_resource_path);
        (*length) = mainbundle_resource_path_length;
        if (NULL != path)
        {
            memcpy(path, mainbundle_resource_path, sizeof(char) * mainbundle_resource_path_length);
        }
    }
}

void get_library_directory(char *path, size_t *length)
{
    @autoreleasepool
    {
        // Locating Items in the Standard Directories
        // https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/AccessingFilesandDirectories/AccessingFilesandDirectories.html

        char const *standard_library_directory = [[[NSFileManager defaultManager] URLsForDirectory:NSLibraryDirectory inDomains:NSUserDomainMask][0] fileSystemRepresentation];
        size_t standard_library_directory_length = strlen(standard_library_directory);
        (*length) = standard_library_directory_length;
        if (NULL != path)
        {
            memcpy(path, standard_library_directory, sizeof(char) * standard_library_directory_length);
        }
    }
}

@interface pt_wsi_mach_osx_nsthread_detach_target : NSObject
- (void)pt_wsi_mach_osx_nsmain:(void *)__here_ns_thread_detach_target_has_finished;
@end

@implementation pt_wsi_mach_osx_nsthread_detach_target
- (void)pt_wsi_mach_osx_nsmain:(void *)__here_ns_thread_detach_target_has_finished
{
    (*((bool volatile *)__here_ns_thread_detach_target_has_finished)) = true;
}
@end

void cocoa_set_multithreaded(void)
{
    @autoreleasepool
    {
        bool volatile __here_ns_thread_detach_target_has_finished = false;
        id ns_thread_detach_target = [[pt_wsi_mach_osx_nsthread_detach_target alloc] init];
        [NSThread detachNewThreadSelector:@selector(pt_wsi_mach_osx_nsmain:)
                                 toTarget:ns_thread_detach_target
                               withObject:((__bridge id)((void *)&__here_ns_thread_detach_target_has_finished))];
        while (!__here_ns_thread_detach_target_has_finished)
        {
            pthread_yield_np();
        }
    }
}

bool cocoa_is_multithreaded(void)
{
    @autoreleasepool
    {
        return [NSThread isMultiThreaded];
    }
}

@interface pt_wsi_mach_osx_application_delegate : NSObject <NSApplicationDelegate>
- (void)applicationDidFinishLaunching:(NSNotification *)notification;
- (void)applicationWillTerminate:(NSNotification *)notification;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;
@end

@interface pt_wsi_mach_osx_view_controller : NSViewController
- (void)loadView;
- (void)viewDidLoad;
- (void)setRepresentedObject:representedObject;
- (void)pt_wsi_mach_osx_main_queue_dispatch_source_event_handler;
- (CVReturn)pt_wsi_mach_osx_display_link_output_callback;
@end

@interface pt_wsi_mach_osx_view : NSView
- (BOOL)wantsLayer;
- (CALayer *)makeBackingLayer;
- (BOOL)wantsUpdateLayer;
@end

static CVReturn pt_wsi_mach_osx_display_link_output_callback(CVDisplayLinkRef displayLink, CVTimeStamp const *inNow, CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext);

extern void *gfx_connection_init_callback(void *layer, float width, float height, void **void_instance);

extern void gfx_connection_redraw_callback(void *gfx_connection);

@implementation pt_wsi_mach_osx_application_delegate
- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    //@autoreleasepool
    {
        CGFloat ns_window_width = 1280.0f;
        CGFloat ns_window_height = 720.0f;

        NSRect ns_main_screen_rect = [[NSScreen mainScreen] frame];

        NSRect ns_window_rect = NSMakeRect((ns_main_screen_rect.size.width - ns_window_width) * 0.5f + ns_main_screen_rect.origin.x,
                                           (ns_main_screen_rect.size.height - ns_window_height) * 0.5f + ns_main_screen_rect.origin.y,
                                           ns_window_width,
                                           ns_window_height);

        id ns_window = [[NSWindow alloc] initWithContentRect:ns_window_rect styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable backing:NSBackingStoreBuffered defer:FALSE];

        id ns_view_controller = [[pt_wsi_mach_osx_view_controller alloc] initWithNibName:nil bundle:nil];

        [ns_window setContentViewController:ns_view_controller];

        [ns_window makeKeyAndOrderFront:nil];
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

@implementation pt_wsi_mach_osx_view_controller
{
    void *m_gfx_connection;
    void *m_void_instance;
    dispatch_source_t m_dispatch_source;
    CVDisplayLinkRef m_display_link;
}

- (void)loadView
{
    //@autoreleasepool
    {
        NSRect ns_view_rect = {{0, 0}, {1280, 720}};
        id ns_view = [[pt_wsi_mach_osx_view alloc] initWithFrame:ns_view_rect];
        [self setView:ns_view];
    }
}

- (void)viewDidLoad
{
    ////@autoreleasepool
    {
        [super viewDidLoad];

        //
        self->m_gfx_connection = NULL;

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        self->m_dispatch_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue());

        // Block Implementation Specification
        // https://clang.llvm.org/docs/Block-ABI-Apple.html
        void *const view_controller_void = ((__bridge void *)self);
        dispatch_source_set_event_handler(self->m_dispatch_source, ^{
          [((pt_wsi_mach_osx_view_controller *)((__bridge id)view_controller_void)) pt_wsi_mach_osx_main_queue_dispatch_source_event_handler];
        });

        dispatch_resume(self->m_dispatch_source);

        CVReturn res_display_link_create_with_active_cg_displays = CVDisplayLinkCreateWithActiveCGDisplays(&self->m_display_link);
        assert(kCVReturnSuccess == res_display_link_create_with_active_cg_displays);

        CVReturn res_display_link_set_output_callback = CVDisplayLinkSetOutputCallback(self->m_display_link, pt_wsi_mach_osx_display_link_output_callback, ((__bridge void *)self));
        assert(kCVReturnSuccess == res_display_link_set_output_callback);

        CVReturn ret_display_link_start = CVDisplayLinkStart(self->m_display_link);
        assert(kCVReturnSuccess == ret_display_link_start);
    }
}

- (void)setRepresentedObject:representedObject
{
    [super setRepresentedObject:representedObject];
}

- (void)pt_wsi_mach_osx_main_queue_dispatch_source_event_handler
{
    // main thread // serial
    if (NULL != self->m_gfx_connection)
    {
        gfx_connection_redraw_callback(self->m_gfx_connection);
    }
    else
    {
        ////@autoreleasepool
        {
            void *layer = ((__bridge void *)[[self view] layer]);
            if (NULL != layer)
            {
                CGSize drawable_size = [((CAMetalLayer *)((__bridge id)layer)) drawableSize];
                self->m_gfx_connection = gfx_connection_init_callback(layer, drawable_size.width, drawable_size.height, &self->m_void_instance);
            }
        }
    }
}

- (CVReturn)pt_wsi_mach_osx_display_link_output_callback
{
    dispatch_source_merge_data(self->m_dispatch_source, 1);
    return kCVReturnSuccess;
}

@end

@implementation pt_wsi_mach_osx_view : NSView
- (BOOL)wantsLayer
{
    return YES;
}

- (CALayer *)makeBackingLayer
{
    CALayer* layer = [CAMetalLayer layer];
    CGSize viewScale = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];
    layer.contentsScale = MIN(viewScale.width, viewScale.height);
    return layer;
}

- (BOOL)wantsUpdateLayer
{
    return YES;
}
@end

static CVReturn pt_wsi_mach_osx_display_link_output_callback(CVDisplayLinkRef displayLink, CVTimeStamp const *inNow, CVTimeStamp const *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    return [((pt_wsi_mach_osx_view_controller *)((__bridge id)displayLinkContext)) pt_wsi_mach_osx_display_link_output_callback];
}

// We need this to hold the refcount of the "pt_wsi_mach_osx_application_delegate" since the "NSApplication::delegate" is weak.
static __strong id application_delegate = nil;
void application_set_delegate(void)
{
    //@autoreleasepool
    {
        application_delegate = [[pt_wsi_mach_osx_application_delegate alloc] init];
        [[NSApplication sharedApplication] setDelegate:application_delegate];
    }
}

int application_main(int argc, char const *argv[])
{
    //@autoreleasepool
    {
        return NSApplicationMain(argc, argv);
    }
}

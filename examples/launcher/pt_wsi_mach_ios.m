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
#include <UIKit/UIKit.h>
#include <dispatch/dispatch.h>
#include <CoreVideo/CoreVideo.h>
#include <QuartzCore/QuartzCore.h>

void get_library_directory(char *path, size_t *length)
{
    @autoreleasepool
    {
        // Locating Items in the Standard Directories
        // https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/AccessingFilesandDirectories/AccessingFilesandDirectories.html

        char const *library_directory = [[[NSFileManager defaultManager] URLsForDirectory:NSLibraryDirectory inDomains:NSUserDomainMask][0] fileSystemRepresentation];
        size_t library_directory_length = strlen(library_directory);
        (*length) = library_directory_length;
        if (NULL != path)
        {
            memcpy(path, library_directory, sizeof(char) * library_directory_length);
        }
    }
}

@interface pt_wsi_mach_ios_thread_detach_target : NSObject
- (instancetype)init;
- (void)pt_wsi_mach_ios_main:(void *)argument;
- (bool)has_inited;
@end

@implementation pt_wsi_mach_ios_thread_detach_target
{
    bool volatile m_has_inited;
}
- (instancetype)init
{
    self = [super init];
    if (self)
    {
        self->m_has_inited = false;
    }
    return self;
}

- (void)pt_wsi_mach_ios_main:(void *)argument
{
    self->m_has_inited = true;
}

- (bool)has_inited
{
    return self->m_has_inited;
}

@end

void cocoa_set_multithreaded(void)
{
    @autoreleasepool
    {
        pt_wsi_mach_ios_thread_detach_target *thread_detach_target = [[pt_wsi_mach_ios_thread_detach_target alloc] init];
        [NSThread detachNewThreadSelector:@selector(pt_wsi_mach_ios_main:)
                                 toTarget:thread_detach_target
                               withObject:nil];

        while (![thread_detach_target has_inited])
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

@interface pt_wsi_mach_ios_application_delegate : NSObject <UIApplicationDelegate>
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions;
@end

@interface pt_wsi_mach_ios_view_controller : UIViewController
- (void)loadView;
- (void)viewDidLoad;
- (void)display_link_callback;
@end

@interface pt_wsi_mach_ios_view : UIView
+ (Class)layerClass;
@end

extern void *gfx_connection_init_callback(void *layer, float width, float height, void **void_instance);

extern void gfx_connection_redraw_callback(void *gfx_connection);

@implementation pt_wsi_mach_ios_application_delegate
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
    @autoreleasepool
    {
        CGRect main_screen_bounds = [[UIScreen mainScreen] bounds];

        UIWindow *window = [[UIWindow alloc] initWithFrame:main_screen_bounds];

        UIViewController *view_controller = [[pt_wsi_mach_ios_view_controller alloc] initWithNibName:nil bundle:nil];

        [window setRootViewController:view_controller];

        [window makeKeyAndVisible];

        return TRUE;
    }
}
@end

@implementation pt_wsi_mach_ios_view_controller
{
    void *m_gfx_connection;
    void *m_void_instance;
    CADisplayLink *m_display_link;
}

- (void)loadView
{
    @autoreleasepool
    {
        CGRect main_screen_bounds = [[UIScreen mainScreen] bounds];

        id view = [[pt_wsi_mach_ios_view alloc] initWithFrame:main_screen_bounds];

        [self setView:view];
    }
}
- (void)viewDidLoad
{
    @autoreleasepool
    {
        [super viewDidLoad];

        //
        self->m_gfx_connection = NULL;

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        self->m_display_link = [CADisplayLink displayLinkWithTarget:self
                                                           selector:@selector(display_link_callback)];

        [self->m_display_link setPreferredFramesPerSecond:60];

        // CADisplayLink callbacks are associated with an 'NSRunLoop'. The currentRunLoop is the
        // the main run loop (since 'viewDidLoad' is always executed from the main thread.
        [self->m_display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (void)display_link_callback
{
    @autoreleasepool
    {
        // main thread // serial
        if (NULL != self->m_gfx_connection)
        {
            gfx_connection_redraw_callback(self->m_gfx_connection);
        }
        else
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
@end

@implementation pt_wsi_mach_ios_view
+ (Class)layerClass
{
    @autoreleasepool
    {
        return [CAMetalLayer class];
    }
}
@end

int application_main(int argc, char *argv[])
{
    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([pt_wsi_mach_ios_application_delegate class]));
    }
}

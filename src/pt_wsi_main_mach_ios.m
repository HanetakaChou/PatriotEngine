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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <string.h>
#include <pthread.h>
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <dispatch/dispatch.h>
#include <CoreVideo/CoreVideo.h>
#include <QuartzCore/QuartzCore.h>
#include <pt_wsi_main.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>
#include <pt_gfx_connection.h>

extern bool wsi_mach_ios_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg);
extern void wsi_mach_ios_os_yield(void);
extern bool wsi_mach_ios_atomic_load(bool volatile *src);
extern void wsi_mach_ios_atomic_store(bool volatile *dst, bool val);

@interface wsi_mach_ios_thread_detach_target : NSObject
- (instancetype)init;
- (void)main:(void *)argument;
@end

@implementation wsi_mach_ios_thread_detach_target
{
    bool volatile m_running;
}
- (instancetype)init
{
    self = [super init];
    if (self)
    {
        self->m_running = false;
    }
    return self;
}

- (void)main:(void *)argument
{
    self->m_running = true;
}

- (bool)running
{
    return self->m_running;
}

@end

void wsi_mach_ios_cocoa_set_multithreaded(void)
{
    @autoreleasepool
    {
        // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html
        wsi_mach_ios_thread_detach_target *thread_detach_target = [[wsi_mach_ios_thread_detach_target alloc] init];

        [NSThread detachNewThreadSelector:@selector(main:) toTarget:thread_detach_target withObject:nil];

        while (![thread_detach_target running])
        {
            pthread_yield_np();
        }
    }
}

bool wsi_mach_ios_cocoa_is_multithreaded(void)
{
    @autoreleasepool
    {
        return [NSThread isMultiThreaded];
    }
}

@interface wsi_mach_ios_application_delegate : NSObject <UIApplicationDelegate>
- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options;
@end

@interface wsi_mach_ios_scene_delegate : NSObject <UIWindowSceneDelegate>
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions;
@property(strong, nonatomic) UIWindow *window;
@end

@interface wsi_mach_ios_view_controller : UIViewController
- (void)loadView;
@end

@interface wsi_mach_ios_view : UIView
+ (Class)layerClass;
- (void)didMoveToWindow;
- (void)display_link_callback;
- (void)draw_on_main_thread;
@end

@implementation wsi_mach_ios_application_delegate
- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options
{
    UISceneConfiguration *scene_configuration = [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
    scene_configuration.delegateClass = [wsi_mach_ios_scene_delegate class];
    return scene_configuration;
}
@end

@implementation wsi_mach_ios_scene_delegate
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions
{
    @autoreleasepool
    {
        assert([scene isKindOfClass:[UIWindowScene class]]);

        CGRect main_screen_bounds = [[UIScreen mainScreen] bounds];

        UIWindow *window = [[UIWindow alloc] initWithFrame:main_screen_bounds];
        [window setWindowScene:(UIWindowScene *)(scene)];
        [self setWindow:window];

        wsi_mach_ios_view_controller *view_controller = [[wsi_mach_ios_view_controller alloc] initWithNibName:nil bundle:nil];

        [window setRootViewController:view_controller];

        [window setBackgroundColor:[UIColor whiteColor]];

        [window makeKeyAndVisible];
    }
}
@end

@implementation wsi_mach_ios_view_controller
- (void)loadView
{
    @autoreleasepool
    {
        CGRect main_screen_bounds = [[UIScreen mainScreen] bounds];

        UIView *view = [[wsi_mach_ios_view alloc] initWithFrame:main_screen_bounds];

        [self setView:view];
    }
}
@end

static pt_wsi_app_ref(PT_PTR *g_wsi_app_init_callback)(pt_gfx_connection_ref);
static int(PT_PTR *g_wsi_app_main_callback)(pt_wsi_app_ref);
struct app_main_argument_t
{
    pt_gfx_connection_ref m_gfx_connection;
    pt_wsi_app_ref m_wsi_app;
    bool m_running;
};
static void *app_main(void *argument);

@implementation wsi_mach_ios_view
{
    mcrt_native_thread_id m_app_main_thread_id;
    CADisplayLink *m_display_link;
    pt_gfx_connection_ref m_gfx_connection;
    pt_wsi_app_ref m_wsi_app;
}

+ (Class)layerClass
{
    @autoreleasepool
    {
        return [CAMetalLayer class];
    }
}

- (void)didMoveToWindow
{
    @autoreleasepool
    {
        [super didMoveToWindow];

        // Locating Items in the Standard Directories
        // https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/AccessingFilesandDirectories/AccessingFilesandDirectories.html
        char const *library_directory = [[[NSFileManager defaultManager] URLsForDirectory:NSLibraryDirectory inDomains:NSUserDomainMask][0] fileSystemRepresentation];
        self->m_gfx_connection = pt_gfx_connection_init(NULL, NULL, library_directory);
        assert(NULL != self->m_gfx_connection);

        //
        UIWindow *window = [self window];
        assert(nil != window);
        UIScreen *screen = [window screen];
        assert(nil != screen);
        CAMetalLayer *layer = ((CAMetalLayer *)[self layer]);
        assert(nil != layer);
        CGSize drawable_size = [layer drawableSize];
        pt_gfx_connection_on_wsi_window_created(self->m_gfx_connection, NULL, ((__bridge void *)layer), drawable_size.width, drawable_size.height);

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        self->m_display_link = [screen displayLinkWithTarget:self selector:@selector(display_link_callback)];

        [self->m_display_link setPreferredFramesPerSecond:60];

        // CADisplayLink callbacks are associated with an 'NSRunLoop'. The currentRunLoop is the
        // the main run loop (since 'viewDidLoad' is always executed from the main thread.
        [self->m_display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

        // the "app_main"
        struct app_main_argument_t app_main_argument;
        app_main_argument.m_gfx_connection = self->m_gfx_connection;
        app_main_argument.m_wsi_app = NULL;
        wsi_mach_ios_atomic_store(&app_main_argument.m_running, false);

        PT_MAYBE_UNUSED bool res_native_thread_create = wsi_mach_ios_native_thread_create(&self->m_app_main_thread_id, app_main, &app_main_argument);
        assert(res_native_thread_create);

        while (!wsi_mach_ios_atomic_load(&app_main_argument.m_running))
        {
            wsi_mach_ios_os_yield();
        }

        assert(NULL != app_main_argument.m_wsi_app);
        self->m_wsi_app=app_main_argument.m_wsi_app;
    }
}

- (void)display_link_callback
{
    @autoreleasepool
    {
        [self draw_on_main_thread];
    }
}

- (void)draw_on_main_thread
{
    @autoreleasepool
    {
        pt_gfx_connection_draw_acquire(self->m_gfx_connection);

        pt_gfx_connection_draw_release(self->m_gfx_connection);
    }
}
@end

void *app_main(void *argument_void)
{
    pt_wsi_app_ref wsi_app;
    // app_init
    {
        struct app_main_argument_t *argument = ((struct app_main_argument_t *)argument_void);
        wsi_app = g_wsi_app_init_callback(argument->m_gfx_connection);
        argument->m_wsi_app = wsi_app;
        wsi_mach_ios_atomic_store(&argument->m_running, true);
    }

    // app_main
    int res_app_main_callback = g_wsi_app_main_callback(wsi_app);

    // mcrt_atomic_store(&self->m_loop, false);

    // wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

    return ((void *)((intptr_t)res_app_main_callback));
}

PT_ATTR_WSI int PT_CALL pt_wsi_main(int argc, char *argv[], pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
    @autoreleasepool
    {
        g_wsi_app_init_callback = wsi_app_init_callback;
        g_wsi_app_main_callback = wsi_app_main_callback;
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([wsi_mach_ios_application_delegate class]));
    }
}

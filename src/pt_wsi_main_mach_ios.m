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
#include <pt_gfx_connection.h>

extern bool mcrt_atomic_loadb(bool volatile *src);
extern void mcrt_atomic_storeb(bool volatile *dst, bool val);

static pt_gfx_connection_ref g_gfx_connection = NULL;

@interface wsi_mach_ios_application_delegate : NSObject <UIApplicationDelegate>
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions;
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
@end

@implementation wsi_mach_ios_application_delegate
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
    return TRUE;
}

- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options
{
    @autoreleasepool
    {
        UISceneConfiguration *scene_configuration = [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
        scene_configuration.delegateClass = [wsi_mach_ios_scene_delegate class];
        return scene_configuration;
    }
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

@implementation wsi_mach_ios_view
{
    CADisplayLink *m_display_link;
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

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        UIWindow *window = [self window];
        assert(nil != window);
        UIScreen *screen = [window screen];
        assert(nil != screen);

        self->m_display_link = [screen displayLinkWithTarget:self selector:@selector(display_link_callback)];

        [self->m_display_link setPreferredFramesPerSecond:60];
        // CADisplayLink callbacks are associated with an 'NSRunLoop'. The currentRunLoop is the
        // the main run loop (since 'viewDidLoad' is always executed from the main thread.
        [self->m_display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

        CAMetalLayer *layer = ((CAMetalLayer *)[self layer]);
        assert(nil != screen);

        CGSize drawable_size = [layer drawableSize];
        pt_gfx_connection_on_wsi_window_created(g_gfx_connection, NULL, ((__bridge void *)layer), drawable_size.width, drawable_size.height);
    }
}

- (void)display_link_callback
{
    @autoreleasepool
    {
        pt_gfx_connection_draw_acquire(g_gfx_connection);

        pt_gfx_connection_draw_release(g_gfx_connection);
    }
}
@end

struct app_main_argument_t
{
    pt_gfx_connection_ref m_gfx_connection;
    pt_wsi_app_ref m_wsi_app;
    bool m_app_main_running;
    int m_argc;
    char **m_argv;
    pt_wsi_app_init_callback m_app_init_callback;
    pt_wsi_app_main_callback m_app_main_callback;
};
static void *app_main(void *argument);

PT_ATTR_WSI int PT_CALL pt_wsi_main(
    int argc, char *argv[],
    pt_input_stream_init_callback cache_input_stream_init_callback, pt_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_input_stream_read_callback cache_input_stream_read_callback, pt_input_stream_destroy_callback cache_input_stream_destroy_callback,
    pt_output_stream_init_callback cache_output_stream_init_callback, pt_output_stream_write_callback cache_output_stream_write_callback, pt_output_stream_destroy_callback cache_output_stream_destroy_callback,
    pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback)
{
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
        }

        return UIApplicationMain(argc, argv, nil, NSStringFromClass([wsi_mach_ios_application_delegate class]));
    }
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

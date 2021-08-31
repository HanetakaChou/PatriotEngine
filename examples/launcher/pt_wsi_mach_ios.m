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
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

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
- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options;
@end

@interface wsi_mach_ios_scene_delegate : NSObject <UIWindowSceneDelegate>
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions;
@property(strong, nonatomic) UIWindow *window;
@end

@interface pt_wsi_mach_ios_view_controller : UIViewController
- (void)loadView;
- (void)viewDidLoad;
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator;
@end

@interface pt_wsi_mach_ios_view : UIView
+ (Class)layerClass;
- (void)didMoveToWindow;
- (void)display_link_callback;
@end

extern void *gfx_connection_init_callback(void *layer, float width, float height, void **void_instance);

extern void gfx_connection_redraw_callback(void *gfx_connection);

@implementation pt_wsi_mach_ios_application_delegate
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
    //@autoreleasepool
    {
        //CGRect main_screen_bounds = [[UIScreen mainScreen] bounds];

        //UIWindow *window = [[UIWindow alloc] initWithFrame:main_screen_bounds];

        //pt_wsi_mach_ios_view_controller *view_controller = [[pt_wsi_mach_ios_view_controller alloc] initWithNibName:nil bundle:nil];

        //pt_wsi_mach_ios_view_controller *view_controller = [[pt_wsi_mach_ios_view_controller alloc] initWithWindow:window];

        //[window setRootViewController:view_controller];

        //[window setBackgroundColor:[UIColor whiteColor]];

        //[window makeKeyAndVisible];

        return TRUE;
    }
}

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

        pt_wsi_mach_ios_view_controller *view_controller = [[pt_wsi_mach_ios_view_controller alloc] initWithNibName:nil bundle:nil];

        [window setRootViewController:view_controller];

        [window setBackgroundColor:[UIColor whiteColor]];

        [window makeKeyAndVisible];
    }
}
@end

@implementation pt_wsi_mach_ios_view_controller
- (void)loadView
{
    @autoreleasepool
    {
        CGRect main_screen_bounds = [[UIScreen mainScreen] bounds];

        UIView *view = [[pt_wsi_mach_ios_view alloc] initWithFrame:main_screen_bounds];

        [self setView:view];
    }
}
- (void)viewDidLoad
{
    @autoreleasepool
    {
        [super viewDidLoad];
    }
}

// Allow device rotation to resize the swapchain
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{

    @autoreleasepool
    {
        [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    }
}

@end

@implementation pt_wsi_mach_ios_view
{
    void *m_gfx_connection;
    void *m_void_instance;
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

        UIWindow *window = [self window];
        assert(nil != window);

        UIScreen *screen = [window screen];
        assert(nil != screen);

        CAMetalLayer *layer = ((CAMetalLayer *)[self layer]);
        assert(nil != layer);

        CGSize drawable_size = [layer drawableSize];
        self->m_gfx_connection = gfx_connection_init_callback((__bridge void *)layer, drawable_size.width, drawable_size.height, &self->m_void_instance);

        // [Creating a Custom Metal View](https://developer.apple.com/documentation/metal/drawable_objects/creating_a_custom_metal_view)
        // RENDER_ON_MAIN_THREAD
        self->m_display_link = [screen displayLinkWithTarget:self selector:@selector(display_link_callback)];

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
        gfx_connection_redraw_callback(self->m_gfx_connection);
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

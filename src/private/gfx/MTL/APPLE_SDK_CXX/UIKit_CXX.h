#ifndef _UIKIT_CXX_H_
#define _UIKIT_CXX_H_ 1

#include "CoreGraphics_CXX.h"

#include "NSRuntime_CXX.h"

#include <stdint.h>

struct UIScreen *UIScreen_mainScreen();

CGRect UIScreen_bounds(struct UIScreen *self);

struct UIWindow *UIWindow_alloc();

struct UIWindow *UIWindow_initWithFrame(struct UIWindow *self, CGRect frame);

void UIWindow_setRootViewController(struct UIWindow *self, struct UIViewController *rootViewController);

void UIWindow_makeKeyAndVisible(struct UIWindow *self);

struct UIView *UIView_alloc();

struct UIView *UIView_initWithFrame(UIView *self, CGRect frame);

struct UIViewController_Class *UIViewController_allocClass(
    char const *classname,
    void (*_I_UIViewController_loadView)(struct UIViewController *, struct UIViewController_loadView *),
    void (*_I_UIViewController_viewDidLoad)(struct UIViewController *, struct UIViewController_viewDidLoad *));

bool UIViewController_Class_addIvarVoidPointer(struct UIViewController_Class *self, char const *ivarname);

struct UIViewController *UIViewController_alloc(struct UIViewController_Class *);

struct UIViewController *UIViewController_init(struct UIViewController *self);

void UIViewController_setIvarVoidPointer(struct UIViewController *self, char const *ivarname, void *pVoid);

void *UIViewController_getIvarVoidPointer(struct UIViewController *self, char const *ivarname);

void UIViewController_setView(struct UIViewController *self, struct UIView *view);

void UIViewController_super_viewDidLoad(struct UIViewController *self, struct UIViewController_viewDidLoad *);

struct UIApplicationDelegate_Class *UIApplicationDelegate_allocClass(
    char const *classname,
    int8_t (*_I_UIApplicationDelegate_didFinishLaunchingWithOptions_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_didFinishLaunchingWithOptions_ *, struct UIApplication *application, void *launchOptions),
    void (*_I_UIApplicationDelegate_applicationWillResignActive_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillResignActive_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationDidEnterBackground_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationDidEnterBackground_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationWillEnterForeground_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillEnterForeground_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationDidBecomeActive_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillEnterForeground_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationWillTerminate_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillTerminate_ *, struct UIApplication *application));

void UIApplicationDelegate_registerClass(struct UIApplicationDelegate_Class *class_UIApplicationDelegate_CXX);

extern "C" int UIApplicationMain(int argc, char *argv[], struct NSString *principalClassName, struct NSString *delegateClassName) __attribute__((availability(ios, introduced = 2.0), availability(macos, unavailable)));

#endif
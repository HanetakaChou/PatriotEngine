#include "UIKit_CXX.h"
#include "UIKit_CXX_IMPL.h"

#include <objc/objc.h>

#if __is_target_os(ios)
#include <objc/message.h>
#include <objc/runtime.h>
#elif __is_target_os(macos)
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

#include <assert.h>

struct UIScreen *UIScreen_mainScreen()
{
    struct objc_object *screen = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("UIScreen"),
        sel_registerName("mainScreen"));

    return static_cast<struct UIScreen *>(screen);
}

CGRect UIScreen_bounds(struct UIScreen *self)
{
#if defined(__x86_64__) || defined(__i386__) || defined(__arm__)
    assert(sizeof(CGRect) != 1 && sizeof(CGRect) != 2 && sizeof(CGRect) != 4 && sizeof(CGRect) != 8);
    CGRect _bounds = reinterpret_cast<CGRect (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend_stret)(
        self,
        sel_registerName("bounds"));
    return _bounds;
#elif defined(__aarch64__)
    CGRect _bounds = reinterpret_cast<CGRect (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("bounds"));
    return _bounds;
#else
#error Unknown Architecture
#endif
}

struct UIWindow *UIWindow_alloc()
{
    struct objc_object *window = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("UIWindow"),
        sel_registerName("alloc"));

    return static_cast<struct UIWindow *>(window);
}

struct UIWindow *UIWindow_initWithFrame(struct UIWindow *self, CGRect frame)
{
    struct objc_object *window = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, CGRect)>(objc_msgSend)(
        self,
        sel_registerName("initWithFrame:"),
        frame);
    return static_cast<struct UIWindow *>(window);
}

void UIWindow_setRootViewController(struct UIWindow *self, struct UIViewController *rootViewController)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setRootViewController:"),
        rootViewController);
}

void UIWindow_makeKeyAndVisible(struct UIWindow *self)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("makeKeyAndVisible"));
}

struct UIView *UIView_alloc()
{
    struct objc_object *view = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("UIView"),
        sel_registerName("alloc"));
    return static_cast<struct UIView *>(view);
}

struct UIView *UIView_initWithFrame(UIView *self, CGRect frame)
{
    struct objc_object *view = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, CGRect)>(objc_msgSend)(
        self,
        sel_registerName("initWithFrame:"),
        frame);
    return static_cast<struct UIView *>(view);
}

struct UIViewController_Class *UIViewController_allocClass(
    char const *classname,
    void (*_I_UIViewController_loadView)(struct UIViewController *, struct UIViewController_loadView *),
    void (*_I_UIViewController_viewDidLoad)(struct UIViewController *, struct UIViewController_viewDidLoad *))
{
    Class class_UIViewController_CXX = objc_allocateClassPair(
        objc_getClass("UIViewController"),
        classname,
        0);
    assert(class_UIViewController_CXX != NULL);

    BOOL result1 = class_addMethod(
        class_UIViewController_CXX,
        sel_registerName("loadView"),
        reinterpret_cast<IMP>(_I_UIViewController_loadView),
        "v@:");
    assert(result1 != NO);

    BOOL result2 = class_addMethod(
        class_UIViewController_CXX,
        sel_registerName("viewDidLoad"),
        reinterpret_cast<IMP>(_I_UIViewController_viewDidLoad),
        "v@:");
    assert(result2 != NO);

    return reinterpret_cast<struct UIViewController_Class *>(class_UIViewController_CXX);
}

bool UIViewController_Class_addIvarVoidPointer(struct UIViewController_Class *self, char const *ivarname)
{
    return OBJC_CLASS_addIvarVoidPointer(self, ivarname);
}

struct UIViewController *UIViewController_alloc(struct UIViewController_Class *class_UIViewController_CXX)
{
    struct objc_object *viewcontroller = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        reinterpret_cast<Class>(class_UIViewController_CXX),
        sel_registerName("alloc"));

    return static_cast<struct UIViewController *>(viewcontroller);
}

struct UIViewController *UIViewController_init(struct UIViewController *self)
{
    struct objc_object *viewcontroller = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("init"));

    return static_cast<struct UIViewController *>(viewcontroller);
}

void UIViewController_setIvarVoidPointer(struct UIViewController *self, char const *ivarname, void *pVoid)
{
    return OBJC_OBJECT_setIvarVoidPointer(self, ivarname, pVoid);
}

void *UIViewController_getIvarVoidPointer(struct UIViewController *self, char const *ivarname)
{
    return OBJC_OBJECT_getIvarVoidPointer(self, ivarname);
}

void UIViewController_setView(struct UIViewController *self, struct UIView *view)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setView:"),
        view);
}

void UIViewController_super_viewDidLoad(struct UIViewController *self, struct UIViewController_viewDidLoad *_cmd)
{
    struct objc_super super = {
        self,
        class_getSuperclass(object_getClass(self))};

    reinterpret_cast<void (*)(struct objc_super *, struct objc_selector *)>(objc_msgSendSuper)(
        &super,
        reinterpret_cast<struct objc_selector *>(_cmd));
}

struct UIApplicationDelegate_Class *UIApplicationDelegate_allocClass(
    char const *classname,
    int8_t (*_I_UIApplicationDelegate_didFinishLaunchingWithOptions_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_didFinishLaunchingWithOptions_ *, struct UIApplication *application, void *launchOptions),
    void (*_I_UIApplicationDelegate_applicationWillResignActive_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillResignActive_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationDidEnterBackground_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationDidEnterBackground_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationWillEnterForeground_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillEnterForeground_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationDidBecomeActive_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillEnterForeground_ *, struct UIApplication *application),
    void (*_I_UIApplicationDelegate_applicationWillTerminate_)(struct UIApplicationDelegate *, struct UIApplicationDelegate_applicationWillTerminate_ *, struct UIApplication *application))
{
    Class class_UIApplicationDelegate_CXX;

    class_UIApplicationDelegate_CXX = objc_allocateClassPair(
        objc_getClass("NSObject"),
        classname,
        0);
    assert(class_UIApplicationDelegate_CXX != NULL);

    BOOL result1 = class_addMethod(
        class_UIApplicationDelegate_CXX,
        sel_registerName("application:didFinishLaunchingWithOptions:"),
        reinterpret_cast<IMP>(_I_UIApplicationDelegate_didFinishLaunchingWithOptions_),
        "B@:@");
    assert(result1 != NO);

    BOOL result2 = class_addMethod(
        class_UIApplicationDelegate_CXX,
        sel_registerName("applicationWillResignActive:"),
        reinterpret_cast<IMP>(_I_UIApplicationDelegate_applicationWillResignActive_),
        "v@:@");
    assert(result2 != NO);

    BOOL result3 = class_addMethod(
        class_UIApplicationDelegate_CXX,
        sel_registerName("applicationDidEnterBackground:"),
        reinterpret_cast<IMP>(_I_UIApplicationDelegate_applicationDidEnterBackground_),
        "v@:@");
    assert(result3 != NO);

    BOOL result4 = class_addMethod(
        class_UIApplicationDelegate_CXX,
        sel_registerName("applicationWillEnterForeground:"),
        reinterpret_cast<IMP>(_I_UIApplicationDelegate_applicationWillEnterForeground_),
        "v@:@");
    assert(result4 != NO);

    BOOL result5 = class_addMethod(
        class_UIApplicationDelegate_CXX,
        sel_registerName("applicationDidBecomeActive:"),
        reinterpret_cast<IMP>(_I_UIApplicationDelegate_applicationDidBecomeActive_),
        "v@:@");
    assert(result5 != NO);

    BOOL result6 = class_addMethod(
        class_UIApplicationDelegate_CXX,
        sel_registerName("applicationWillTerminate:"),
        reinterpret_cast<IMP>(_I_UIApplicationDelegate_applicationWillTerminate_),
        "v@:@");
    assert(result6 != NO);

    return reinterpret_cast<struct UIApplicationDelegate_Class *>(class_UIApplicationDelegate_CXX);
}

void UIApplicationDelegate_registerClass(struct UIApplicationDelegate_Class *class_UIApplicationDelegate_CXX)
{
    objc_registerClassPair(reinterpret_cast<Class>(class_UIApplicationDelegate_CXX));
}

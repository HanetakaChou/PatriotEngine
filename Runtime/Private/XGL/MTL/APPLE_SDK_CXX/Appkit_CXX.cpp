#include "AppKit_CXX.h"
#include "AppKit_CXX_IMPL.h"

#if __is_target_os(ios)
#include <objc/message.h>
#include <objc/runtime.h>
#elif __is_target_os(macos)
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

#include <assert.h>

struct NSScreen *NSScreen_mainScreen()
{
    struct objc_object *screen = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSScreen"),
        sel_registerName("mainScreen"));

    return static_cast<struct NSScreen *>(screen);
}

NSRect NSScreen_frame(struct NSScreen *self)
{
    assert(sizeof(NSRect) != 1 && sizeof(NSRect) != 2 && sizeof(NSRect) != 4 && sizeof(NSRect) != 8);
    NSRect _frame = reinterpret_cast<NSRect (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend_stret)(
        self,
        sel_registerName("frame"));
    return _frame;
}

struct NSWindow *NSWindow_alloc()
{
    struct objc_object *window = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSWindow"),
        sel_registerName("alloc"));

    return static_cast<struct NSWindow *>(window);
}

struct NSWindow *NSWindow_initWithContentRect(struct NSWindow *self, NSRect rect, NSWindowStyleMask styleMask, NSBackingStoreType backing, bool defer)
{
    struct objc_object *window = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSRect, NSWindowStyleMask, NSBackingStoreType, BOOL)>(objc_msgSend)(
        self,
        sel_registerName("initWithContentRect:styleMask:backing:defer:"),
        rect,
        styleMask,
        backing,
        (defer != false) ? YES : NO);
    return static_cast<struct NSWindow *>(window);
}

void NSWindow_setContentViewController(struct NSWindow *self, struct NSViewController *contentViewController)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setContentViewController:"),
        contentViewController);
}

void NSWindow_makeKeyAndOrderFront(struct NSWindow *self, void *sender)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("makeKeyAndOrderFront:"),
        reinterpret_cast<struct objc_object *>(sender));
}

struct NSView *NSView_alloc()
{
    struct objc_object *view = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSView"),
        sel_registerName("alloc"));
    return static_cast<struct NSView *>(view);
}

struct NSView *NSView_initWithFrame(NSView *self, NSRect frameRect)
{
    struct objc_object *view = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSRect)>(objc_msgSend)(
        self,
        sel_registerName("initWithFrame:"),
        frameRect);
    return static_cast<struct NSView *>(view);
}

struct NSViewController_Class *NSViewController_allocClass(
    char const *classname,
    void (*_I_NSViewController_loadView)(struct NSViewController *, struct NSViewController_loadView *),
    void (*_I_NSViewController_viewDidLoad)(struct NSViewController *, struct NSViewController_viewDidLoad *),
    void (*_I_NSViewController_setRepresentedObject_)(struct NSViewController *, struct NSViewController_setRepresentedObject_ *, void *representedObject))
{
    Class class_NSViewController_CXX = objc_allocateClassPair(
        objc_getClass("NSViewController"),
        classname,
        0);
    assert(class_NSViewController_CXX != NULL);

    BOOL result1 = class_addMethod(
        class_NSViewController_CXX,
        sel_registerName("loadView"),
        reinterpret_cast<IMP>(_I_NSViewController_loadView),
        "v@:");
    assert(result1 != NO);

    BOOL result2 = class_addMethod(
        class_NSViewController_CXX,
        sel_registerName("viewDidLoad"),
        reinterpret_cast<IMP>(_I_NSViewController_viewDidLoad),
        "v@:");
    assert(result2 != NO);

    BOOL result3 = class_addMethod(
        class_NSViewController_CXX,
        sel_registerName("setRepresentedObject:"),
        reinterpret_cast<IMP>(_I_NSViewController_setRepresentedObject_),
        "v@:@");
    assert(result3 != NO);

    return reinterpret_cast<struct NSViewController_Class *>(class_NSViewController_CXX);
}

bool NSViewController_Class_addIvarVoidPointer(struct NSViewController_Class *self, char const *ivarname)
{
    return OBJC_CLASS_addIvarVoidPointer(self, ivarname);
}

struct NSViewController *NSViewController_alloc(struct NSViewController_Class *class_NSViewController_CXX)
{
    struct objc_object *viewcontroller = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        reinterpret_cast<Class>(class_NSViewController_CXX),
        sel_registerName("alloc"));

    return static_cast<struct NSViewController *>(viewcontroller);
}

struct NSViewController *NSViewController_initWithNibName(struct NSViewController *self, void *nibNameOrNil, void *nibBundleOrNil)
{
    struct objc_object *viewcontroller = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("initWithNibName:bundle:"),
        reinterpret_cast<struct objc_object *>(nibNameOrNil),
        reinterpret_cast<struct objc_object *>(nibBundleOrNil));

    return static_cast<struct NSViewController *>(viewcontroller);
}

void NSViewController_setIvarVoidPointer(struct NSViewController *self, char const *ivarname, void *pVoid)
{
    return OBJC_OBJECT_setIvarVoidPointer(self, ivarname, pVoid);
}

void *NSViewController_getIvarVoidPointer(struct NSViewController *self, char const *ivarname)
{
    return OBJC_OBJECT_getIvarVoidPointer(self, ivarname);
}

void NSViewController_setView(struct NSViewController *self, NSView *view)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setView:"),
        view);
}

void NSViewController_super_viewDidLoad(struct NSViewController *self, struct NSViewController_viewDidLoad *_cmd)
{
    struct objc_super super = {
        self,
        class_getSuperclass(object_getClass(self))};

    reinterpret_cast<void (*)(struct objc_super *, struct objc_selector *)>(objc_msgSendSuper)(
        &super,
        reinterpret_cast<struct objc_selector *>(_cmd));
}

void NSViewController_super_setRepresentedObject_(struct NSViewController *self, struct NSViewController_setRepresentedObject_ *_cmd, void *representedObject)
{
    struct objc_super super = {
        self,
        class_getSuperclass(object_getClass(self))};

    reinterpret_cast<void (*)(struct objc_super *, struct objc_selector *, struct objc_object *)>(objc_msgSendSuper)(
        &super,
        reinterpret_cast<struct objc_selector *>(_cmd),
        reinterpret_cast<struct objc_object *>(representedObject));
}

struct NSApplicationDelegate_Class *NSApplicationDelegate_allocClass(
    char const *classname,
    void (*_I_NSApplicationDelegate_applicationDidFinishLaunching_)(struct NSApplicationDelegate *, struct NSApplicationDelegate_applicationDidFinishLaunching_ *, void *aNotification),
    void (*_I_NSApplicationDelegate_applicationWillTerminate_)(struct NSApplicationDelegate *, struct NSApplicationDelegate_applicationWillTerminate_ *, void *aNotification),
    int8_t (*_I_NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_)(struct NSApplicationDelegate *, struct NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_ *, struct NSApplication *sender))

{
    Class class_NSApplicationDelegate_CXX;

    //objc_object *protocol_NSApplicationDelegate = objc_getProtocol("NSApplicationDelegate");
    //return NULL why???
    class_NSApplicationDelegate_CXX = objc_allocateClassPair(
        objc_getClass("NSObject"),
        classname,
        0);
    assert(class_NSApplicationDelegate_CXX != NULL);

    BOOL result1 = class_addMethod(
        class_NSApplicationDelegate_CXX,
        sel_registerName("applicationDidFinishLaunching:"),
        reinterpret_cast<IMP>(_I_NSApplicationDelegate_applicationDidFinishLaunching_),
        "v@:@");
    assert(result1 != NO);

    BOOL result2 = class_addMethod(
        class_NSApplicationDelegate_CXX,
        sel_registerName("applicationWillTerminate:"),
        reinterpret_cast<IMP>(_I_NSApplicationDelegate_applicationWillTerminate_),
        "v@:@");
    assert(result2 != NO);

   BOOL result3 = class_addMethod(
        class_NSApplicationDelegate_CXX,
        sel_registerName("applicationShouldTerminateAfterLastWindowClosed:"),
        reinterpret_cast<IMP>(_I_NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_),
        "v@:@");
    assert(result3 != NO);

    //res = class_addProtocol(
    //      class_MyDelegate,
    //      protocol_NSApplicationDelegate);

    //assert(res != NO);

    //objc_registerClassPair(class_NSApplicationDelegate_CXX);

    //res = class_conformsToProtocol(class_NSApplicationDelegate_CXX, protocol_NSApplicationDelegate);
    //assert(res != NO);

    return reinterpret_cast<struct NSApplicationDelegate_Class *>(class_NSApplicationDelegate_CXX);
}

struct NSApplicationDelegate *NSApplicationDelegate_alloc(struct NSApplicationDelegate_Class *class_NSApplicationDelegate_CXX)
{
    struct objc_object *delegate = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        reinterpret_cast<Class>(class_NSApplicationDelegate_CXX),
        sel_registerName("alloc"));

    return static_cast<struct NSApplicationDelegate *>(delegate);
}

struct NSApplicationDelegate *NSApplicationDelegate_init(struct NSApplicationDelegate *self)
{
    struct objc_object *delegate = NSObject_init(self);
    return static_cast<struct NSApplicationDelegate *>(delegate);
}

struct NSApplication *NSApplication_sharedApplication()
{
    struct objc_object *application = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSApplication"),
        sel_registerName("sharedApplication"));

    return static_cast<struct NSApplication *>(application);
}

void NSApplication_setDelegate(struct NSApplication *self, struct NSApplicationDelegate *delegate)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setDelegate:"),
        delegate);
}

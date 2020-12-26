#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_thread.h>

/** @file objc/obc-internal.h */
extern "C" void *objc_autoreleasePoolPush(void);
extern "C" void objc_autoreleasePoolPop(void *);

typedef struct _NSObject_T_ *NSObject;
static inline NSObject NSObject_init(NSObject);
static inline void NSObject_release(NSObject);

typedef struct _Class_NSThreadDetachTarget_T_ *Class_NSThreadDetachTarget;
typedef struct _NSThreadDetachTarget_T_ *NSThreadDetachTarget;
typedef struct _NSThreadDetachSelector__T_ *NSThreadDetachSelector_;
static inline Class_NSThreadDetachTarget NSThreadDetachTarget_allocateClass(char const *class_name, char const *selector_name, void (*_I_NSThreadDetachSelector_)(NSThreadDetachTarget, NSThreadDetachSelector_, void *argument));
static inline NSThreadDetachTarget NSThreadDetachTarget_alloc(Class_NSThreadDetachTarget class_ns_thread_detach_target);
static inline NSThreadDetachTarget NSThreadDetachTarget_init(NSThreadDetachTarget ns_thread_detach_target);
static inline void NSThreadDetachTarget_release(NSThreadDetachTarget ns_thread_detach_target);
/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/NSThread.h */
static inline void NSThread_detachNewThreadSelector(char const *selector_name, NSThreadDetachTarget target, void *argument);
static inline bool NSThread_isMultiThreaded();

/** @file System/Library/Frameworks/AppKit.framework/Headers/NSApplication.h */
typedef struct _NSApplication_T_ *NSApplication;
static inline NSApplication NSApplication_sharedApplication();

/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers/NSApplication.h */
extern "C" int NSApplicationMain(int argc, char const *argv[]);

int main(int argc, char const *argv[])
{
    //Enable MultiThreaded
    {
        // Using Autorelease Pool Blocks
        // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html

        void *__here_auto_release_pool_object = objc_autoreleasePoolPush();

        bool __here_ns_thread_detach_target_has_finished = false;

        Class_NSThreadDetachTarget class_ns_thread_detach_target = NSThreadDetachTarget_allocateClass(
            "NSThreadDetachTarget_pt_wsi_window_posix_mach_osx",
            "NSMain_pt_wsi_window_posix_mach_osx",
            [](NSThreadDetachTarget, NSThreadDetachSelector_, void *__here_ns_thread_detach_target_has_finished) -> void {
                (*static_cast<bool *>(__here_ns_thread_detach_target_has_finished)) = true;
            });

        NSThreadDetachTarget ns_thread_detach_target = NSThreadDetachTarget_init(NSThreadDetachTarget_alloc(class_ns_thread_detach_target));

        NSThread_detachNewThreadSelector("NSMain_pt_wsi_window_posix_mach_osx", ns_thread_detach_target, &__here_ns_thread_detach_target_has_finished);

        while (!__here_ns_thread_detach_target_has_finished)
        {
            mcrt_os_yield();
        }

        NSThreadDetachTarget_release(ns_thread_detach_target);

        assert(NSThread_isMultiThreaded() != false);

        objc_autoreleasePoolPop(__here_auto_release_pool_object);
    }

    NSApplication ns_application = NSApplication_sharedApplication();

    return NSApplicationMain(argc, argv);
}

// ---
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#include <objc/message.h>
#include <objc/runtime.h>
#elif TARGET_OS_OSX
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif
#include <assert.h>

static inline NSObject NSObject_Wrap(struct objc_object *ns_object)
{
    return reinterpret_cast<NSObject>(ns_object);
}

static inline struct objc_object *NSObject_Unwrap(NSObject ns_object)
{
    return reinterpret_cast<struct objc_object *>(ns_object);
}

static inline Class_NSThreadDetachTarget Class_NSThreadDetachTarget_Wrap(Class class_ns_thread_detach_target)
{
    return reinterpret_cast<Class_NSThreadDetachTarget>(class_ns_thread_detach_target);
}

static inline Class Class_NSThreadDetachTarget_Unwrap(Class_NSThreadDetachTarget class_ns_thread_detach_target)
{
    return reinterpret_cast<Class>(class_ns_thread_detach_target);
}

static inline NSThreadDetachTarget NSThreadDetachTarget_Wrap(struct objc_object *ns_thread_detach_target)
{
    return reinterpret_cast<NSThreadDetachTarget>(ns_thread_detach_target);
}

static inline struct objc_object *NSThreadDetachTarget_Unwrap(NSThreadDetachTarget ns_thread_detach_target)
{
    return reinterpret_cast<struct objc_object *>(ns_thread_detach_target);
}

static inline NSObject NSThreadDetachTarget_To_NSObject(NSThreadDetachTarget ns_thread_detach_target)
{
    return reinterpret_cast<NSObject>(ns_thread_detach_target);
}

static inline NSThreadDetachTarget NSObject_To_NSThreadDetachTarget(NSObject ns_thread_detach_target)
{
    return reinterpret_cast<NSThreadDetachTarget>(ns_thread_detach_target);
}

static inline NSApplication NSApplication_Wrap(struct objc_object *ns_application)
{
    return reinterpret_cast<NSApplication>(ns_application);
}

// ---
static inline NSObject NSObject_init(NSObject ns_object)
{
    struct objc_object *ret_ns_object = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSObject_Unwrap(ns_object),
        sel_registerName("init"));
    return NSObject_Wrap(ret_ns_object);
}

static inline void NSObject_release(NSObject ns_object)
{
    return reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSObject_Unwrap(ns_object),
        sel_registerName("release"));
}

static inline Class_NSThreadDetachTarget NSThreadDetachTarget_allocateClass(char const *class_name, char const *selector_name, void (*_I_NSThreadDetachSelector_)(NSThreadDetachTarget, NSThreadDetachSelector_, void *argument))
{
    Class class_ns_thread_detach_target = objc_allocateClassPair(
        objc_getClass("NSObject"),
        class_name,
        0);
    assert(class_ns_thread_detach_target != NULL);

    BOOL res = class_addMethod(
        class_ns_thread_detach_target,
        sel_registerName(selector_name),
        reinterpret_cast<IMP>(_I_NSThreadDetachSelector_),
        "v@:@");
    assert(res != NO);

    return Class_NSThreadDetachTarget_Wrap(class_ns_thread_detach_target);
}

static inline NSThreadDetachTarget NSThreadDetachTarget_alloc(Class_NSThreadDetachTarget class_ns_thread_detach_target)
{
    struct objc_object *ns_thread_detach_target = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        Class_NSThreadDetachTarget_Unwrap(class_ns_thread_detach_target),
        sel_registerName("alloc"));

    return NSThreadDetachTarget_Wrap(ns_thread_detach_target);
}

static inline NSThreadDetachTarget NSThreadDetachTarget_init(NSThreadDetachTarget ns_thread_detach_target)
{
    return NSObject_To_NSThreadDetachTarget(NSObject_init(NSThreadDetachTarget_To_NSObject(ns_thread_detach_target)));
}

static inline void NSThreadDetachTarget_release(NSThreadDetachTarget ns_thread_detach_target)
{
    return NSObject_release(NSThreadDetachTarget_To_NSObject(ns_thread_detach_target));
}

static inline void NSThread_detachNewThreadSelector(char const *selector_name, NSThreadDetachTarget target, void *argument)
{
    return reinterpret_cast<void (*)(Class, struct objc_selector *, struct objc_selector *, struct objc_object *, void * /* struct objc_object * */)>(objc_msgSend)(
        objc_getClass("NSThread"),
        sel_registerName("detachNewThreadSelector:toTarget:withObject:"),
        sel_registerName(selector_name),
        NSThreadDetachTarget_Unwrap(target),
        argument);
}

static inline bool NSThread_isMultiThreaded()
{
    BOOL is_multi_threaded = reinterpret_cast<BOOL (*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSThread"),
        sel_registerName("isMultiThreaded"));
    return (is_multi_threaded != NO) ? true : false;
}

static inline NSApplication NSApplication_sharedApplication()
{
    struct objc_object *ns_application = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSApplication"),
        sel_registerName("sharedApplication"));

    return NSApplication_Wrap(ns_application);
}
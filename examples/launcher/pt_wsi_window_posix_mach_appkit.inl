/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _PT_WSI_WINDOW_POSIX_MACH_APPKIT_INL_
#define _PT_WSI_WINDOW_POSIX_MACH_APPKIT_INL_ 1

#ifndef _PT_WSI_WINDOW_POSIX_MACH_APPKIT_H_
#error 1
#endif

#ifndef _PT_WSI_WINDOW_POSIX_MACH_OBJC_INL_
#error 1
#endif

static inline NSObject NSApplicationDelegate_To_NSObject(NSApplicationDelegate ns_application_delegate)
{
    return reinterpret_cast<NSObject>(ns_application_delegate);
}

static inline NSApplicationDelegate NSObject_ToNSApplicationDelegate(NSObject ns_application_delegate)
{
    return reinterpret_cast<NSApplicationDelegate>(ns_application_delegate);
}

static inline NSApplicationDelegate NSApplicationDelegate_Wrap(struct objc_object *ns_application_delegate)
{
    return reinterpret_cast<NSApplicationDelegate>(ns_application_delegate);
}

static inline struct objc_object *NSApplicationDelegate_Unwrap(NSApplicationDelegate ns_application_delegate)
{
    return reinterpret_cast<struct objc_object *>(ns_application_delegate);
}

static inline NSApplication NSApplication_Wrap(struct objc_object *ns_application)
{
    return reinterpret_cast<NSApplication>(ns_application);
}

static inline struct objc_object *NSApplication_Unwrap(NSApplication ns_application)
{
    return reinterpret_cast<struct objc_object *>(ns_application);
}

static inline NSScreen NSScreen_Wrap(struct objc_object *ns_screen)
{
    return reinterpret_cast<NSScreen>(ns_screen);
}

static inline struct objc_object *NSScreen_Unwrap(NSScreen ns_screen)
{
    return reinterpret_cast<struct objc_object *>(ns_screen);
}

static inline Class_NSObject Class_NSWindow_To_Class_NSObject(Class_NSWindow class_ns_window)
{
    return reinterpret_cast<Class_NSObject>(class_ns_window);
}

static inline NSWindow NSWindow_Wrap(struct objc_object *ns_window)
{
    return reinterpret_cast<NSWindow>(ns_window);
}

static inline struct objc_object *NSWindow_Unwrap(NSWindow ns_window)
{
    return reinterpret_cast<struct objc_object *>(ns_window);
}

static inline NSWindow NSObject_To_NSWindow(NSObject ns_window)
{
    return reinterpret_cast<NSWindow>(ns_window);
}

// ---
static inline bool Class_NSObject_addIvarVoidPointer(Class_NSObject class_ns_object, char const *ivarname)
{
    BOOL res = class_addIvar(
        Class_NSObject_Unwrap(class_ns_object),
        ivarname,
        sizeof(void *),
        alignof(void *),
        "^v");
    return (res != NO) ? true : false;
}

static inline NSObject NSObject_alloc(Class_NSObject class_ns_object)
{
    struct objc_object *ns_object = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        Class_NSObject_Unwrap(class_ns_object),
        sel_registerName("alloc"));
    return NSObject_Wrap(ns_object);
}

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
    return NSObject_To_NSThreadDetachTarget(NSObject_alloc(Class_NSThreadDetachTarget_To_Class_NSObject(class_ns_thread_detach_target)));
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

static inline Class_NSApplicationDelegate NSApplicationDelegate_allocateClass(
    char const *class_name,
    void (*_I_NSApplicationDelegate_applicationDidFinishLaunching_)(NSApplicationDelegate, NSApplicationDelegate_applicationDidFinishLaunching_, void *aNotification),
    void (*_I_NSApplicationDelegate_applicationWillTerminate_)(NSApplicationDelegate, NSApplicationDelegate_applicationWillTerminate_, void *aNotification),
    int8_t (*_I_NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_)(NSApplicationDelegate, NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_, NSApplication sender))
{
    //objc_object *protocol_NSApplicationDelegate = objc_getProtocol("NSApplicationDelegate");
    //return NULL why???

    Class class_ns_application_delegate = objc_allocateClassPair(
        objc_getClass("NSObject"),
        class_name,
        0);
    assert(class_ns_application_delegate != NULL);

    BOOL result_did_finish_launching = class_addMethod(
        class_ns_application_delegate,
        sel_registerName("applicationDidFinishLaunching:"),
        reinterpret_cast<IMP>(_I_NSApplicationDelegate_applicationDidFinishLaunching_),
        "v@:@");
    assert(result_did_finish_launching != NO);

    BOOL result_will_terminate = class_addMethod(
        class_ns_application_delegate,
        sel_registerName("applicationWillTerminate:"),
        reinterpret_cast<IMP>(_I_NSApplicationDelegate_applicationWillTerminate_),
        "v@:@");
    assert(result_will_terminate != NO);

    BOOL result_should_terminate_after_last_window_closed = class_addMethod(
        class_ns_application_delegate,
        sel_registerName("applicationShouldTerminateAfterLastWindowClosed:"),
        reinterpret_cast<IMP>(_I_NSApplicationDelegate_applicationShouldTerminateAfterLastWindowClosed_),
        "v@:@");
    assert(result_should_terminate_after_last_window_closed != NO);

    //res = class_addProtocol(
    //      class_MyDelegate,
    //      protocol_NSApplicationDelegate);

    //assert(res != NO);

    //objc_registerClassPair(class_NSApplicationDelegate_CXX);

    //res = class_conformsToProtocol(class_NSApplicationDelegate_CXX, protocol_NSApplicationDelegate);
    //assert(res != NO);

    return Class_NSApplicationDelegate_Wrap(class_ns_application_delegate);
}

static inline NSApplicationDelegate NSApplicationDelegate_alloc(Class_NSApplicationDelegate class_ns_application_delegate)
{
    struct objc_object *ns_application_delegate = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        Class_NSThreadDetachTarget_Unwrap(class_ns_application_delegate),
        sel_registerName("alloc"));
    return NSApplicationDelegate_Wrap(ns_application_delegate);
}

static inline NSApplicationDelegate NSApplicationDelegate_init(NSApplicationDelegate ns_application_delegate)
{
    return NSObject_ToNSApplicationDelegate(NSObject_init(NSApplicationDelegate_To_NSObject(ns_application_delegate)));
}

static inline NSApplication NSApplication_sharedApplication()
{
    struct objc_object *ns_application = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSApplication"),
        sel_registerName("sharedApplication"));

    return NSApplication_Wrap(ns_application);
}

static inline void NSApplication_setDelegate(NSApplication ns_application, NSApplicationDelegate ns_application_delegate)
{
    return reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        NSApplication_Unwrap(ns_application),
        sel_registerName("setDelegate:"),
        NSApplicationDelegate_Unwrap(ns_application_delegate));
}

static inline NSScreen NSScreen_mainScreen()
{
    struct objc_object *ns_screen = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSScreen"),
        sel_registerName("mainScreen"));

    return NSScreen_Wrap(ns_screen);
}

static inline NSRect NSScreen_frame(NSScreen ns_screen)
{
#if defined(__x86_64__) || defined(__i386__) || defined(__arm__)
    assert(sizeof(NSRect) != 1 && sizeof(NSRect) != 2 && sizeof(NSRect) != 4 && sizeof(NSRect) != 8);
    NSRect ns_rect = reinterpret_cast<NSRect (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend_stret)(
        NSScreen_Unwrap(ns_screen),
        sel_registerName("frame"));
#elif defined(__aarch64__)
    assert(sizeof(NSRect) != 1 && sizeof(NSRect) != 2 && sizeof(NSRect) != 4 && sizeof(NSRect) != 8);
    NSRect ns_rect = reinterpret_cast<NSRect (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSScreen_Unwrap(ns_screen),
        sel_registerName("frame"));
#else
#error Unknown Architecture
#endif
    return ns_rect;
}

static inline NSWindow NSWindow_alloc()
{
    Class_NSWindow class_ns_window = reinterpret_cast<Class_NSWindow>(objc_getClass("NSWindow"));
    return NSObject_To_NSWindow(NSObject_alloc(Class_NSWindow_To_Class_NSObject(class_ns_window)));
}

static inline NSWindow NSWindow_initWithContentRect(NSWindow ns_window, NSRect ns_rect, NSWindowStyleMask ns_window_style_mask, NSBackingStoreType ns_backing_store_type, bool flag, NSScreen ns_screen)
{
    struct objc_object *ret_ns_window = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSRect, NSWindowStyleMask, NSBackingStoreType, BOOL, struct objc_object *)>(objc_msgSend)(
        NSWindow_Unwrap(ns_window),
        sel_registerName("initWithContentRect:styleMask:backing:defer:screen:"),
        ns_rect,
        ns_window_style_mask,
        ns_backing_store_type,
        (flag != false) ? YES : NO,
        NSScreen_Unwrap(ns_screen));
    return NSWindow_Wrap(ret_ns_window);
}

#else
#error 1
#endif
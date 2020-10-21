#include "NSRuntime_CXX.h"
#include "NSRuntime_CXX_IMPL.h"

#if _VSCODE_INTELLISENCE_
#include <objc/objc-runtime.h>
#else
#if __is_target_os(ios)
#include <objc/message.h>
#include <objc/runtime.h>
#elif __is_target_os(macos)
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif
#endif

#include <assert.h>
#include <string.h>

bool OBJC_CLASS_addIvarVoidPointer(struct OBJC_CLASS *self, char const *ivarname)
{
    BOOL result = class_addIvar(
        reinterpret_cast<Class>(self),
        ivarname,
        sizeof(void *),
        alignof(void *),
        "^v");
    return (result != NO) ? true : false;
}

void OBJC_OBJECT_setIvarVoidPointer(struct OBJC_OBJECT *self, char const *ivarname, void *pVoid)
{
    Ivar ivar_pUserData = class_getInstanceVariable(object_getClass(self), ivarname);
    assert(ivar_pUserData != NULL);
    assert(strcmp(ivar_getTypeEncoding(ivar_pUserData), "^v") == 0);
    (*reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(self) + ivar_getOffset(ivar_pUserData))) = pVoid;
}

void *OBJC_OBJECT_getIvarVoidPointer(struct OBJC_OBJECT *self, char const *ivarname)
{
    Ivar ivar_pUserData = class_getInstanceVariable(object_getClass(self), ivarname);
    assert(ivar_pUserData != NULL);
    assert(strcmp(ivar_getTypeEncoding(ivar_pUserData), "^v") == 0);
    return (*reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(self) + ivar_getOffset(ivar_pUserData)));
}

struct NSObject *NSObject_init(struct NSObject *self)
{
    struct objc_object *object = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("init"));
    return static_cast<struct NSObject *>(object);
}

void NSObject_release(struct NSObject *self)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("release"));
}

NSUInteger NSObject_retainCount(struct NSObject *self)
{
    NSUInteger retaincount = reinterpret_cast<NSUInteger (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("retainCount"));
    return retaincount;
}

struct NSString *NSString_alloc()
{
    struct objc_object *string = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSString"),
        sel_registerName("alloc"));
    return reinterpret_cast<struct NSString *>(string);
}

struct NSString *NSString_initWithUTF8String(struct NSString *self, char const *nullTerminatedCString)
{
    struct objc_object *string = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, char const *)>(objc_msgSend)(
        self,
        sel_registerName("initWithUTF8String:"),
        nullTerminatedCString);
    return static_cast<struct NSString *>(string);
}

void NSString_release(struct NSString *self)
{
    return NSObject_release(self);
}

NSUInteger NSString_retainCount(struct NSString *self)
{
    return NSObject_retainCount(self);
}

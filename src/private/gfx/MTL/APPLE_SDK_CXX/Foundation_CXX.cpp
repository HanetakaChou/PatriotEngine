#include "Foundation_CXX.h"
#include "Foundation_CXX_IMPL.h"

#if __is_target_os(ios)
#include <objc/message.h>
#include <objc/runtime.h>
#elif __is_target_os(macos)
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

#include <assert.h>

struct NSObject *NSArrayNSObject_objectAtIndexedSubscript(struct NSArrayNSObject *self, NSUInteger idx)
{
    struct objc_object *object = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("objectAtIndexedSubscript:"),
        idx);
    return static_cast<struct NSObject *>(object);
}

struct NSFileManager *NSFileManager_defaultManager()
{
    struct objc_object *fileManager = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSFileManager"),
        sel_registerName("defaultManager"));
    return static_cast<struct NSFileManager *>(fileManager);
}

struct NSArrayNSURL *NSFileManager_URLsForDirectory(struct NSFileManager *self, NSSearchPathDirectory inDomains, NSSearchPathDomainMask domainMask)
{
    struct objc_object *arrayurl = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSSearchPathDirectory, NSSearchPathDomainMask)>(objc_msgSend)(
        self,
        sel_registerName("URLsForDirectory:inDomains:"),
        inDomains,
        domainMask);

    return static_cast<struct NSArrayNSURL *>(arrayurl);
}

struct NSURL *NSArrayNSURL_objectAtIndexedSubscript(struct NSArrayNSURL *self, NSUInteger idx)
{
    NSObject *url = NSArrayNSObject_objectAtIndexedSubscript(self, idx);
    return static_cast<struct NSURL *>(url);
}

char const *NSURL_fileSystemRepresentation(struct NSURL *self)
{
    char const *filename = reinterpret_cast<char const *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("fileSystemRepresentation"));
    return filename;
}

struct NSThreadDetachTarget_Class *NSThreadDetachTarget_allocClass(
    char const *classname,
    char const *selectorname,
    void (*_I_NSThreadDetachSelector_)(struct NSThreadDetachTarget *, struct NSThreadDetachSelector_ *, void *argument))
{
    Class class_NSThreadDetachTarget_CXX;
    {
        class_NSThreadDetachTarget_CXX = objc_allocateClassPair(
            objc_getClass("NSObject"),
            classname,
            0);
        assert(class_NSThreadDetachTarget_CXX != NULL);

        BOOL result_1 = class_addMethod(
            class_NSThreadDetachTarget_CXX,
            sel_registerName(selectorname),
            reinterpret_cast<IMP>(_I_NSThreadDetachSelector_),
            "v@:@");
        assert(result_1 != NO);
    }

    return reinterpret_cast<struct NSThreadDetachTarget_Class *>(class_NSThreadDetachTarget_CXX);
}

struct NSThreadDetachTarget *NSThreadDetachTarget_alloc(struct NSThreadDetachTarget_Class *class_NSThreadDetachTarget_CXX)
{
    struct objc_object *target = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        reinterpret_cast<Class>(class_NSThreadDetachTarget_CXX),
        sel_registerName("alloc"));

    return static_cast<struct NSThreadDetachTarget *>(target);
}

struct NSThreadDetachTarget *NSThreadDetachTarget_init(struct NSThreadDetachTarget *self)
{
    struct objc_object *target = NSObject_init(self);
    return static_cast<struct NSThreadDetachTarget *>(target);
}

void NSThreadDetachTarget_release(struct NSThreadDetachTarget *self)
{
    return NSObject_release(self);
}

void NSThread_detachNewThreadSelector(char const *selectorname, struct NSThreadDetachTarget *toTarget, void *argument)
{
    reinterpret_cast<void (*)(Class, struct objc_selector *, struct objc_selector *, struct objc_object *, struct objc_object *)>(objc_msgSend)(
        objc_getClass("NSThread"),
        sel_registerName("detachNewThreadSelector:toTarget:withObject:"),
        sel_registerName(selectorname),
        toTarget,
        static_cast<struct objc_object *>(argument));
}

bool NSThread_isMultiThreaded()
{
    BOOL isMultiThreaded = reinterpret_cast<BOOL (*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSThread"),
        sel_registerName("isMultiThreaded"));
    return (isMultiThreaded != NO) ? true : false;
}
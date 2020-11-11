#ifndef _NSRUNTIME_CXX_H_
#define _NSRUNTIME_CXX_H_ 1

//#include <objc/NSObjCRuntime.h>
#if __LP64__ || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

bool OBJC_CLASS_addIvarVoidPointer(struct OBJC_CLASS *self, char const *ivarname);

void OBJC_OBJECT_setIvarVoidPointer(struct OBJC_OBJECT *self, char const *ivarname, void *pVoid);
void *OBJC_OBJECT_getIvarVoidPointer(struct OBJC_OBJECT *self, char const *ivarname);

struct NSObject *NSObject_init(struct NSObject *self);
void NSObject_release(struct NSObject *self);
NSUInteger NSObject_retainCount(struct NSObject *self);

struct NSString *NSString_alloc();
struct NSString *NSString_initWithUTF8String(struct NSString *self, char const *nullTerminatedCString);
void NSString_release(struct NSString *self);
NSUInteger NSString_retainCount(struct NSString *self);

//#include <objc/obc-internal.h>
extern "C" void *objc_autoreleasePoolPush(void);
extern "C" void objc_autoreleasePoolPop(void *);

#endif
#ifndef _FOUNDATION_CXX_IMPL_H_
#define _FOUNDATION_CXX_IMPL_H_ 1

#include "NSRuntime_CXX_IMPL.h"

struct NSArrayNSObject : public NSObject
{
    NSArrayNSObject() = delete;
};

struct NSFileManager : public NSObject
{
    NSFileManager() = delete;
};

struct NSArrayNSURL : public NSArrayNSObject
{
    NSArrayNSURL() = delete;
};

struct NSURL : public NSObject
{
    NSURL() = delete;
};

struct NSThreadDetachTarget : public NSObject
{
    NSThreadDetachTarget() = delete;
};

#endif

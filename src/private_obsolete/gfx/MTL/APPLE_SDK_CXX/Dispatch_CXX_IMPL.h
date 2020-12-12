#ifndef _DISPATCH_CXX_IMPL_H_
#define _DISPATCH_CXX_IMPL_H_ 1

#include "NSRuntime_CXX_IMPL.h"

struct _opaque_dispatch_object_t: public NSObject
{
    _opaque_dispatch_object_t() = delete;
};

struct _opaque_dispatch_queue_t : public _opaque_dispatch_object_t
{
    _opaque_dispatch_queue_t() = delete;
};

struct _opaque_dispatch_queue_serial_t : public _opaque_dispatch_queue_t
{
    _opaque_dispatch_queue_serial_t() = delete;
};

struct _opaque_dispatch_queue_main_t : public _opaque_dispatch_queue_serial_t
{
    _opaque_dispatch_queue_main_t() = delete;
};

struct _opaque_dispatch_data_t : public _opaque_dispatch_object_t
{
    _opaque_dispatch_data_t() = delete;
};

#endif
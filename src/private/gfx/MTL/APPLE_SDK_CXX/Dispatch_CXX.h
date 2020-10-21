#ifndef _DISPATCH_CXX_H_
#define _DISPATCH_CXX_H_ 1

#include "NSRuntime_CXX.h"

#include <stddef.h>
#include <stdint.h>

typedef struct _opaque_dispatch_object_t *dispatch_object_t;
typedef struct _opaque_dispatch_queue_t *dispatch_queue_t;
typedef struct _opaque_dispatch_queue_serial_t *dispatch_queue_serial_t;
typedef struct _opaque_dispatch_queue_main_t *dispatch_queue_main_t;
typedef struct _opaque_dispatch_data_t *dispatch_data_t;

void dispatch_release(dispatch_object_t object);

dispatch_queue_main_t dispatch_get_main_queue(void);

dispatch_data_t dispatch_data_create(void *buffer, size_t size, dispatch_queue_t queue, void *pUserData, void (*pfnCallback)(void *pUserData, void *buffer));
dispatch_data_t dispatch_data_create(void *buffer, size_t size, dispatch_queue_main_t queue, void *pUserData, void (*pfnCallback)(void *pUserData, void *buffer));
void dispatch_release(dispatch_data_t object);

#endif
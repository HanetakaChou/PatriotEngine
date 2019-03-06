#ifndef __COREFOUNDATION_UUID__
#define __COREFOUNDATION_UUID__ 1

#include <stdint.h>

typedef uint8_t uuid_t[16];

int uuid_generate_time_safe(uuid_t out);

#endif
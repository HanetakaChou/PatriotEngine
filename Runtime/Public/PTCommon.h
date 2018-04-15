#ifndef PT_COMMON_H
#define PT_COMMON_H

#if defined(PTWIN32)
#define PTCALL __stdcall
#define PTPTR __stdcall
#define PTIMPORT __declspec(dllimport)
#define PTEXPORT __declspec(dllexport)
#elif defined(PTPOSIX)
#define PTCALL 
#define PTPTR 
#define PTIMPORT
#define PTEXPORT __attribute__((visibility("default")))
#else
#error 未知的平台
#endif

#include <stdint.h>

typedef uint32_t PTBOOL;
#define PTFALSE 0U
#define PTTRUE 1U

#endif
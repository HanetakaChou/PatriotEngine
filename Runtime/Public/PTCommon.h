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
    #if defined(PTPOSIXLINUX)

    #elif defined(PTPOSIXMACH) //Mach-O Support Universal Binary
        #if defined(__i386__)
            #define PTX86
        #elif defined(__x86_64__)
            #define PTX64
        #elif defined(__arm__)
            #define PTARM
        #elif defined(__aarch64__)
            #define PTARM64
        #else
            #error Unknown Architecture
        #endif
    #else
        #error 未知的平台
    #endif
#else
#error 未知的平台
#endif

#endif

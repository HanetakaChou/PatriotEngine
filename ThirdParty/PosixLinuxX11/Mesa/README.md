## -----------------------------------------------------------------------------------

[libpciaccess](https://gitlab.freedesktop.org/xorg/lib/libpciaccess/tree/master)   
[drm](https://gitlab.freedesktop.org/mesa/drm)  
[libxtrans](https://gitlab.freedesktop.org/xorg/lib/libxtrans)  
[xextproto](https://gitlab.freedesktop.org/xorg/proto/xextproto)  
[kbproto](https://gitlab.freedesktop.org/xorg/proto/kbproto)  
[inputproto](https://gitlab.freedesktop.org/xorg/proto/inputproto)    
[libX11](https://gitlab.freedesktop.org/xorg/lib/libX11)  
[libxshmfence](https://gitlab.freedesktop.org/xorg/lib/libxshmfence)  
[libXext](https://gitlab.freedesktop.org/xorg/lib/libXext)  
[renderproto](https://gitlab.freedesktop.org/xorg/proto/renderproto)  
[libXrender](https://gitlab.freedesktop.org/xorg/lib/libXrender)  
[randrproto](https://gitlab.freedesktop.org/xorg/proto/randrproto)  
[libXrandr](https://gitlab.freedesktop.org/xorg/lib/libXrandr)  
[mesa](https://gitlab.freedesktop.org/mesa/mesa)  

Patch for projects 
```
# Build & Install xproto
## in Xos_r.h
### remove the following
>#elif !defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(__APPLE__)
>...
>#else

# Build & Install libpciaccess

## #include <sys/io.h> file not found
### Create sys/io.h in toolchain  

# Build & Install libxshmfence

## In src/xshmfence_futex.h
### #include <value.h> -> #include <limits.h>
### sys_futex(... MAXINT ...) -> sys_futex(... INT_MAX ...)

## In configure.ac
### AC_CHECK_FUNCS([open_memstream] -> AC_DEFINE([HAVE_OPEN_MEMSTREAM], 0, [no open_memstream])

# Build & Install drm


# Build & Install llvm

## In llvm/llvm/lib/Support/Unix/Unix.h
### After #include <unistd.h>
### Add #include <sys/syscall.h>

## #include <sysexits.h> file not found
### Create sysexits.h in toolchain  

### Use build_machine host first to build a tablegen
####  make llvm-tblgen -j10
### Specify in cmake option
LLVM_TABLEGEN -> .../llvm-tblgen

LLVM_BUILD_LLVM_DYLIB -> ON
LLVM_LINK_LLVM_DYLIB -> ON

### llvm-config ### used by mesa
#### chrpath -r '$ORIGIN' build/bin/llvm-config
#### copy linker to /system/bin #### the program interpreter ### readelf -l build/bin/llvm-config

# Build & Install mesa

## my-llvm-config-dir
export PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/bin"${PATH:+:${PATH}} 

## In meson_options.txt
platforms -> ['x11'] ## no wayland ## no drm(full screen) ## no surfaceless

dri-drivers -> [''] ## use OpenGL drivers
gallium-drivers -> ['zink'] ## OpenGL on Vulkan instead
glx -> ['xlib'] ## use zlink no dri
### egl -> ['auto'] ## current not support zlink

vulkan-drivers -> ['amd', 'intel']

## _glapi_tls_Dispatch undefined  
### in meson.build
> # if .....
> # pre_args += '-DUSE_ELF_TLS' ### use pthread_getspecific instead of  USE_ELF_TLS   
> # endif

## patch headers for libc

### mkostemp not found
#### add mkostemp to stdlib.h in toolchain
extern int mkostemp64(char*, int); //in bionic/libc/include/stdlib.h
...

### pthread_barrier_init not found
#### add pthread_barrier_init to pthread.h in toolchain
int pthread_barrierattr_init(pthread_barrierattr_t* attr) __nonnull((1)); //in bionic/libc/include/stdlib.h

### 'linux/kcmp.h' file not found
#### create linux/kcmp.h in toolchain

### open_memstream not found
#### add open_memstream to stdio.h in toolchain
FILE* open_memstream(char**, size_t*); //in bionic/libc/include/stdio.h

### strchrnul not found
#### add strchrnul to strings.h in toolchain
char* strchrnul(const char*, int) __purefunc; //in bionic/libc/include/strings.h

### shmat not found
#### in sys/shm.h 
> #include <sys/syscall.h>
> 
> static __inline void* shmat(int __shm_id, const void* __addr, int __flags)
> {
>    return syscall(SYS_shmat, __shm_id, __addr, __flags);
> }
> 
> static __inline int shmdt(const void *__addr)
> {
>     return syscall(SYS_shmdt, __addr);
> }
> 
> static __inline int shmctl(int __shm_id, int __cmd, struct shmid_ds *__buf)
> {
>     return syscall(SYS_shmctl, __shm_id, __cmd, __buf);
> }
> 
> static int __inline shmget(key_t __key, size_t __size, int __flags)
> {
>     return syscall(SYS_shmget, __key, __size, __flags);
> }

## use the libc built from aosp to replace the fake in toolchain

## Elf64_Section not found
patch code in external/elfutils/libelf/elf.h to the elf.h in toolchain  

## linux/bpf.h not found
### use source from kernel-headers rpm
###  https://elixir.bootlin.com/linux/v3.18/source/include/uapi/linux/bpf.h
### https://elixir.bootlin.com/linux/v3.18/source/include/uapi/linux/bpf_common.h

## errors for clang  
### in src/amd/compiler/aco_insert_waitcnt.cpp
> //wait_ctx out_ctx[program->blocks.size()] //variable length array of non-POD element type
> -> 
> wait_ctx *out_ctx = new(alloca(sizeof(wait_ctx)*program->blocks.size()))wait_ctx[program->blocks.size()]

### in src/amd/compiler/aco_insert_NOPs.cpp
> NOP_ctx_gfx10 all_ctx[program->blocks.size()] //variable length array of non-POD element type
> ->
> NOP_ctx_gfx10 *all_ctx = new(alloca(sizeof(NOP_ctx_gfx10)*program->blocks.size()))NOP_ctx_gfx10[program->blocks.size()]

### in src/amd/compiler/aco_spill.cpp
#### no viable conversion from 'aco_ptr<aco::Pseudo_instruction>' to 'aco_ptr<aco::Instruction>'
> aco_ptr<Pseudo_instruction> reload{create_instruction<Pseudo_instruction>(aco_opcode::p_reload, Format::PSEUDO, 1, 1)};
> ->
> aco_ptr<Instruction> reload{static_cast<Instruction*>(create_instruction<Pseudo_instruction>(aco_opcode::p_reload, Format::PSEUDO, 1, 1))};

### in src/amd/vulkan/radv_llvm_helper.cpp
#### undefined reference to '__cxa_thread_atexit'
#### use the code from my-ndk-dir/sources/cxx-stl/llvm-libc++abi/libcxxabi/src/cxa_thread_atexit.cpp  
> namespace __cxxabiv1
> {
> extern "C"
> {
> 	int __cxa_thread_atexit(void (*dtor)(void *), void *obj, void *dso_symbol) throw()
> 	{
> 		extern int __cxa_thread_atexit_impl(void (*)(void *), void *, void *);
> 		return __cxa_thread_atexit_impl(dtor, obj, dso_symbol);
> 	}
> } // extern "C"
> } // namespace __cxxabiv1


```

## -----------------------------------------------------------------------------------

chrpath  
```
target_arch=x86_64 ##x86 ##arm64 ##arm

chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libpciaccess.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libkms.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm_amdgpu.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm_intel.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm_radeon.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libLLVM.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxcb-dri3.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libX11-xcb.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxcb-present.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxcb-sync.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxshmfence.so"
```

## -----------------------------------------------------------------------------------
sys/io.h  

```
static __inline int iopl(int level)
{
    return syscall(SYS_iopl, level);
}

static __inline int ioperm(unsigned long from, unsigned long num, int turn_on)
{
    return syscall(SYS_ioperm, from, num, turn_on);
}

static __inline unsigned char inb(unsigned short int __port)
{
    unsigned char _v;

    __asm__ __volatile__("inb %w1,%0"
                         : "=a"(_v)
                         : "Nd"(__port));
    return _v;
}

static __inline unsigned short int inw(unsigned short int __port)
{
    unsigned short _v;

    __asm__ __volatile__("inw %w1,%0"
                         : "=a"(_v)
                         : "Nd"(__port));
    return _v;
}

static __inline unsigned int inl(unsigned short int __port)
{
    unsigned int _v;

    __asm__ __volatile__("inl %w1,%0"
                         : "=a"(_v)
                         : "Nd"(__port));
    return _v;
}

static __inline void outb(unsigned char __value, unsigned short int __port)
{
    __asm__ __volatile__("outb %b0,%w1"
                         :
                         : "a"(__value), "Nd"(__port));
}

static __inline void outw(unsigned short int __value, unsigned short int __port)
{
    __asm__ __volatile__("outw %w0,%w1"
                         :
                         : "a"(__value), "Nd"(__port));
}

static __inline void outl(unsigned int __value, unsigned short int __port)
{
    __asm__ __volatile__("outl %0,%w1"
                         :
                         : "a"(__value), "Nd"(__port));
}
```

## -----------------------------------------------------------------------------------
sysexits.h   

```
#ifndef _SYSEXITS_H
#define _SYSEXITS_H 1

#define EX_OK 0

#define EX__BASE 64

#define EX_USAGE 64
#define EX_DATAERR 65
#define EX_NOINPUT 66
#define EX_NOUSER 67
#define EX_NOHOST 68
#define EX_UNAVAILABLE 69
#define EX_SOFTWARE 70
#define EX_OSERR 71
#define EX_OSFILE 72
#define EX_CANTCREAT 73
#define EX_IOERR 74
#define EX_TEMPFAIL 75
#define EX_PROTOCOL 76
#define EX_NOPERM 77
#define EX_CONFIG 78

#define EX__MAX 78

#endif
``` 

## -----------------------------------------------------------------------------------
linux/kcmp.h  

```
/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_KCMP_H
#define _UAPI_LINUX_KCMP_H

#include <linux/types.h>

/* Comparison type */
enum kcmp_type
{
    KCMP_FILE,
    KCMP_VM,
    KCMP_FILES,
    KCMP_FS,
    KCMP_SIGHAND,
    KCMP_IO,
    KCMP_SYSVSEM,
    KCMP_EPOLL_TFD,

    KCMP_TYPES,
};

/* Slot for KCMP_EPOLL_TFD */
struct kcmp_epoll_slot
{
    __u32 efd;  /* epoll file descriptor */
    __u32 tfd;  /* target file number */
    __u32 toff; /* target offset within same numbered sequence */
};

#endif /* _UAPI_LINUX_KCMP_H */
```

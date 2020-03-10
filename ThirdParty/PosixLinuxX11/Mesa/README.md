## -----------------------------------------------------------------------------------

[libpciaccess](https://gitlab.freedesktop.org/xorg/lib/libpciaccess/tree/master)   
[drm](https://gitlab.freedesktop.org/mesa/drm)  
[libxtrans](https://gitlab.freedesktop.org/xorg/lib/libxtrans)  
[xextproto](https://gitlab.freedesktop.org/xorg/proto/xextproto)  
[kbproto](https://gitlab.freedesktop.org/xorg/proto/kbproto)  
[inputproto](https://gitlab.freedesktop.org/xorg/proto/inputproto)    
[libX11](https://gitlab.freedesktop.org/xorg/lib/libX11)  
[libxshmfence](https://gitlab.freedesktop.org/xorg/lib/libxshmfence)  
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
platforms -> ['x11', 'drm', 'surfaceless'] ## no wayland

dri-drivers -> [''] ## OpenGL drivers
gallium-drivers -> [''] ## zink ## OpenGL on Vulkan
glx -> ['']

vulkan-drivers -> ['amd', 'intel']

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
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libX11.so"
chrpath -r '$ORIGIN' "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libX11-xcb.so"
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

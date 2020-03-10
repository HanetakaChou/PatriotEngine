## -----------------------------------------------------------------------------------

[libpciaccess](https://gitlab.freedesktop.org/xorg/lib/libpciaccess/tree/master)   
[drm](https://gitlab.freedesktop.org/mesa/drm)  
[mesa](https://gitlab.freedesktop.org/mesa/mesa)  

Patch for projects 
```
# Build & Install libpciaccess

## #include <sys/io.h> file not found
### Create sys/io.h in toolchain  

# Build & Install drm

## In configure.ac
### AC_CHECK_FUNCS([open_memstream] -> AC_DEFINE([HAVE_OPEN_MEMSTREAM], 0, [no open_memstream])

# Build & Install mesa

## In meson_options.txt
platforms -> ['x11', 'drm', 'surfaceless']

dri-drivers -> [''] ## OpenGL drivers
gallium-drivers -> ['zink'] ## OpenGL on Vulkan

# Build & Install llvm

## In llvm/llvm/lib/Support/Unix/Unix.h
### After #include <unistd.h>
### Add #include <sys/syscall.h>

## #include <sysexits.h> file not found
### Create sysexits.h in toolchain  

## LLVM_TABLEGEN
### Use build_machine host first to build a tablegen
####  make llvm-tblgen -j10
### Specify in cmake option

LLVM_BUILD_LLVM_DYLIB -> ON
LLVM_LINK_LLVM_DYLIB:BOOL -> ON

```

## -----------------------------------------------------------------------------------

chrpath  
```
target_arch=x86_64 ##x86 ##arm64 ##arm

chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libpciaccess.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libkms.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm_amdgpu.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm_intel.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libdrm_radeon.so"

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


[libpciaccess](https://gitlab.freedesktop.org/xorg/lib/libpciaccess/tree/master)   
[drm](https://gitlab.freedesktop.org/mesa/drm)  
[mesa](https://gitlab.freedesktop.org/mesa/mesa)  

Patch for projects 
```
# Build & Install libpciaccess

## #include <sys/io.h> not found
### Create sys/io.h in toolchain  

## In src/Makefile.am
### libpciaccess_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

# Build & Install drm

## In configure.ac
### AC_CHECK_FUNCS([open_memstream] -> AC_DEFINE([HAVE_OPEN_MEMSTREAM], 0, [no open_memstream])

## In Makefile.am
### libdrm_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

## In libkms/Makefile.am
### libkms_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX

## In amdgpu/Makefile.am
### libdrm_amdgpu_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX

## In intel/Makefile.am
### libdrm_intel_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX

## In radeon/Makefile.am
### libdrm_radeon_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX

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

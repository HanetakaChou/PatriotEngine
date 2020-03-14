# mesa-vulkan-drivers 
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
[mesa-modified](https://github.com/YuqiaoZhang/mesa)  

# build rules for specific projects 

## libpciaccess

create \<sys/io.h\> in toolchain 
```
#ifndef _SYS_IO_H
#define _SYS_IO_H 1

#include <sys/syscall.h>

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

#endif
```

## drm

in libdrm_macros.h, fix static assert  
```
#if defined(ANDROID) && !defined(__LP64__)
...
#else
... //do static_assert
#endif
->
#ifndef __LP64__
...
#else
...
#endif
```

in amdgpu/amdgpu_asic_id.c, fix lookup amdgpu.ids
```
use dladdr(amdgpu_parse_asic_ids, ... //related to current so
```

add open_memstream to \<stdio.h\> in toolchain  
```
FILE* open_memstream(char**, size_t*); //from aosp/bionic/libc/include/stdio.h
```

## libX11

in modules/im/ximcp/imCallbk.c, define mblen 
```
int mblen(const char *s, size_t n) //from my-ndk-dir/sources/android/support/src/musl-multibyte/mblen.c
{
	return mbtowc(0, s, n);
}
```

in src/GetDiflt.c, remove unreliable getpw***
```
if ((ptr = getenv("HOME"))) 
{
    ...
}
else 
{
    /*
    if ...
        use getpw***
    else
    */
        *dest ='\0'}
}
```

## libxshmfence

in src/xshmfence_futex.h
```
#include <value.h> -> #include <limits.h>
sys_futex(... MAXINT ...) -> sys_futex(... INT_MAX ...)
```

## llvm

remove -Wl,--no-undefined in CMAKE_MODULE_LINKER_FLAGS
```
cmake .. -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="$HOME/bionic-toolchain-$target_arch/sysroot/usr" -DCMAKE_C_FLAGS="-fPIE -fPIC -U__ANDROID__ -UANDROID" -DCMAKE_CXX_FLAGS="-fPIE -fPIC -U__ANDROID__ -UANDROID" -DCMAKE_EXE_LINKER_FLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX -Wl,--no-undefined -lc++_shared" -DCMAKE_MODULE_LINKER_FLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX -lc++_shared" -DCMAKE_SHARED_LINKER_FLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX -Wl,--no-undefined -lc++_shared" -DCMAKE_SKIP_INSTALL_RPATH=ON 

```

enable dylib in cmake options  
```
LLVM_BUILD_LLVM_DYLIB -> ON
LLVM_LINK_LLVM_DYLIB -> ON
``` 

in llvm/llvm/lib/Support/Unix/Unix.h, add the "#include \<sys/syscall.h\>" after "#include \<unistd.h\>"
```
...
#include <unistd.h>
#include <sys/syscall.h> //we add
```
create <sysexits.h> in toolchain
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

intall bionic and libc++ from https://github.com/YuqiaoZhang/Bionic-based-Linux/tree/rpms  
to make the llvm-config (run by mesa) executable on build_machine  
```
ln -s lib my-toolchain-dir/sysroot/usr/bin
chrpath -r '$ORIGIN' my-toolchain-dir/sysroot/usr/bin/llvm-config
```
  
use build_machine host first to build a tablegen  
\# we can skip this if we intall zlib from https://github.com/YuqiaoZhang/Bionic-based-Linux/tree/rpms  
```
make llvm-tblgen -j10
```
and then specify the path of llvm-tblgen in cmake options  
```
LLVM_TABLEGEN -> .../llvm-tblgen
```



## mesa

### 1\. make the llvm-config executable on build_machine  

add the llvm-config to \$PATH
```
export PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/bin"${PATH:+:${PATH}} 
```

### 2\. configure the meson options, we only build the vulkan drivers and the zlink-base GL drivers
in meson_options.txt
```
vulkan-drivers -> ['amd', 'intel'] ## we only build the vulkan drivers 

platforms -> ['x11','drm', surfaceless] ## no wayland 

dri-drivers -> ['']  ### use zlink no dri
gallium-drivers -> ['zlink']  ### gallium on vulkan

glx -> ['gallium-xlib'] ### GL on gallium
opengl -> 'true'

egl -> ['false']  ### need dri-based glx
gles1 -> 'true'
gles2 -> 'true'

glvnd -> false ### need dri-based glx
```

in meson.build, disable USE_ELF_TLS
```
# if .....
# pre_args += '-DUSE_ELF_TLS' ### use pthread_getspecific instead of  USE_ELF_TLS   
# endif
```

in meson.build, zlink-based gallium-xlib
```
elif not with_gallium_softpipe
    error ... softpipe or llvmpipe
->
elif not (with_gallium_zink)
    error ... zink
```
  
in src/gallium/targets/libgl-xlib/meson.build, zlink-based gallium-xlib and remove version suffix  
```
gallium_xlib_c_args = ... -DGALLIUM_SOFTPIPE ...
->
gallium_xlib_c_args = ... -DGALLIUM_ZINK ...

libgl = shared_library
    ...
    # version ... //We remove the soname suffix
    ...
```

in src/galliun/targets/libgl-xlib, export glX***
```
global:
    gl*;
    mgl*;
    glX*; //We add
...
```  

in src/mapi/shared-glapi, remove version suffix  
```
libglapi = shared_library
    ...
    # soversion ... //We remove the soname suffix
    # version ... //We remove the soname suffix
    ...
```

in src/mapi/es1api, , remove version suffix  
```
libglesv1_cm = shared_library
    ...
    # soversion ... //We remove the soname suffix
    # version ... //We remove the soname suffix
    ...
```  

in src/mapi/es2api, , remove version suffix  
```
libglesv2 = shared_library
    ...
    # soversion ... //We remove the soname suffix
    # version ... //We remove the soname suffix
    ...
```  

### 3\. patch headers for libc  

add pthread_barrier support to \<pthread.h> in toolchian  
```
int pthread_barrierattr_init(pthread_barrierattr_t* attr) __nonnull((1)); //from bionic/libc/include/stdlib.h
```

add mkostemp to \<stdlib.h\> in toolchain  
```
extern int mkostemp(char*, int); //from bionic/libc/include/stdlib.h
```

add strchrnul to \<string.h\> in toolchain  
```
char* strchrnul(const char*, int) __purefunc; //from bionic/libc/include/string.h
```

add shm support to \<sys/shm.h\> in toolchain    
```
#include <sys/syscall.h>

static __inline void* shmat(int __shm_id, const void* __addr, int __flags)
{
   return (void*)syscall(SYS_shmat, __shm_id, __addr, __flags);
}
 
static __inline int shmdt(const void *__addr)
{
   return syscall(SYS_shmdt, __addr);
}

static __inline int shmctl(int __shm_id, int __cmd, struct shmid_ds *__buf)
{
    return syscall(SYS_shmctl, __shm_id, __cmd, __buf);
}

static int __inline shmget(key_t __key, size_t __size, int __flags)
{
    return syscall(SYS_shmget, __key, __size, __flags);
}
```  

use the bionic built from aosp to replace the fake in toolchain
```
copy to the toolchain sysroot  
```

### 4\. patch headers for linux kernel   

add \<linux/kcmp.h\> in toolchain  

```
from https://elixir.bootlin.com/linux/v3.19/source/include/uapi/linux/kcmp.h
```

add \<linux/bpf.h\> in toolchain  
```
from https://elixir.bootlin.com/linux/v3.18/source/include/uapi/linux/bpf.h
```

add \<linux/bpf_common.h\> in toolchain  
```
from https://elixir.bootlin.com/linux/v3.18/source/include/uapi/linux/bpf_common.h
```

### 5\. install dependency packages manually

#### 5\.1\. intall libelf-devel in toolchain

patch code in aosp/external/elfutils/libelf/elf.h to \<elf.h\> in toolchain  
```
Elf64_Section not found
```
  
write the libelf.pc manually
```
prefix=/home/aduzha01/bionic-toolchain-x86_64/sysroot/usr #you should chang to your own path
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libelf
Description: elfutils libelf library to read and write ELF files
Version: 0.165
URL: http://elfutils.org/

Libs: -L${libdir} -lelf
Cflags: -I${includedir}

Requires.private: zlib
```

#### 5\.2\. intall zlib-devel in toolchain
  
write the zlib.pc manually
```
prefix=/home/aduzha01/bionic-toolchain-x86_64/sysroot/usr #you should chang to your own path
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: zlib
Description: zlib compression library
Version: 1.2.8

Requires:
Libs: -L${libdir} -L${sharedlibdir} -lz
Cflags: -I${includedir}
```
#### 5\.3\. install expat-devel in toolchain
  
write the expat.pc manually
```
prefix=/home/aduzha01/bionic-toolchain-x86_64/sysroot/usr #you should chang to your own path
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: expat
Version: 2.1.1
Description: expat XML parser
URL: http://www.libexpat.org
Libs: -L${libdir} -lexpat
Cflags: -I${includedir}
```

### 6\. fix compile errors for clang

in src/amd/compiler/aco_insert_waitcnt.cpp
```
wait_ctx out_ctx[program->blocks.size()] //variable length array of non-POD element type
-> 
wait_ctx *out_ctx = new(alloca(sizeof(wait_ctx)*program->blocks.size()))wait_ctx[program->blocks.size()]
```

in src/amd/compiler/aco_insert_NOPs.cpp
```
NOP_ctx_gfx10 all_ctx[program->blocks.size()] //variable length array of non-POD element type
->
NOP_ctx_gfx10 *all_ctx = new(alloca(sizeof(NOP_ctx_gfx10)*program->blocks.size()))NOP_ctx_gfx10[program->blocks.size()]
```

in src/amd/compiler/aco_spill.cpp
```
aco_ptr<Pseudo_instruction> reload{create_instruction<Pseudo_instruction>(aco_opcode::p_reload, Format::PSEUDO, 1, 1)}; //no viable conversion from 'aco_ptr<aco::Pseudo_instruction>' to 'aco_ptr<aco::Instruction>'
->
aco_ptr<Instruction> reload{static_cast<Instruction*>(create_instruction<Pseudo_instruction>(aco_opcode::p_reload, Format::PSEUDO, 1, 1))};
```
  
in src/gallium/auxliary/target-helpers/inline_sw_helper.h, add zlink headers  
```
#if defined(GALLIUM_ZLINK)
#include "zlink/zlink_public.h
#endif
```

### 7\. fix link errors for clang  

in src/amd/vulkan/radv_llvm_helper.cpp, add the following  
```
//undefined reference to '__cxa_thread_atexit'
//from my-ndk-dir/sources/cxx-stl/llvm-libc++abi/libcxxabi/src/cxa_thread_atexit.cpp 

namespace __cxxabiv1
{
extern "C"
{
	int __cxa_thread_atexit(void (*dtor)(void *), void *obj, void *dso_symbol) throw() 
    {
		extern int __cxa_thread_atexit_impl(void (*)(void *), void *, void *);
		return __cxa_thread_atexit_impl(dtor, obj, dso_symbol);
	}
} // extern "C"
} // namespace __cxxabiv1
```

### 8\. fix portable problems 

in src/vulkan/wsi/wsi_common_display.c, use pthread_detach to replace pthread_cancel and pthread_join
```
pthread_setcanceltype(...
->
// pthread_setcanceltype(... ### remove

pthread_cancel(wsi->wait_thread ... 
pthread_join(wsi->wait_thread ...
->
pthread_detach(wsi->wait_thread ...
```

in disk_cache.c radv_meta.c JitManager.cpp, replace getpw*** by getenv("HOME")
```

```

### 9\. fix ninja install problems

I don't know how to skip intall rpath in meson and I have to use the binaries under build/src  

use relative path in icd json file


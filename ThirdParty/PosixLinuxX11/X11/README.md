Linux Version: EL7  

[NDK r11c](https://developer.android.com/ndk/downloads/older_releases)  

[Building Open Source Projects Using Standalone Toolchains](https://developer.android.com/ndk/guides/standalone_toolchain#building_open_source_projects_using_standalone_toolchains)  

Create toolchain  
```
target_arch=x86_64 ##x86 ##arm64 ##arm

my-ndk-dir/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --arch="$target_arch" --platform=android-24 --install-dir="$HOME/bionic-toolchain-$target_arch"

## Patch for x86_64 
rm -rf "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib"
rm -rf "$HOME/bionic-toolchain-x86_64/sysroot/usr/libx32"
mv "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib"
```  

## --------------------------------------------------------
```
yum install libtool ## Used by autoconf

```

## --------------------------------------------------------

Build projects  
```  
target_arch=x86_64 ##x86 ##arm64 ##arm
target_host=x86_64-linux-android  ##i686-linux-android ##aarch64-linux-android ##arm-linux-androideabi

# Add the standalone toolchain to the search path.
export PATH="$HOME/bionic-toolchain-$target_arch/bin"${PATH:+:${PATH}}

# Tell configure what tools to use.
export AR=$target_host-ar
export AS=$target_host-clang
export CC=$target_host-clang
export CXX=$target_host-clang++
export LD=$target_host-ld
export STRIP=$target_host-strip

# Tell configure what flags Android requires.
export CFLAGS="-fPIE -fPIC"
export LDFLAGS="-pie"

# pkg-config
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/share/pkgconfig"${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/pkgconfig"${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}

# Autoconf
autoreconf -v --install --force -I"$HOME/bionic-toolchain-$target_arch/sysroot/usr/share/aclocal" # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-$target_arch/sysroot/usr" --host=$target_host
make install
```

## --------------------------------------------------------

[macros](https://gitlab.freedesktop.org/xorg/util/macros)  
[xproto](https://gitlab.freedesktop.org/xorg/proto/xproto)  
[libXau](https://gitlab.freedesktop.org/xorg/lib/libXau)  
[xcbproto](https://gitlab.freedesktop.org/xorg/proto/xcbproto)  
[pthread-stubs](https://gitlab.freedesktop.org/xorg/lib/pthread-stubs)  
[libxcb](https://gitlab.freedesktop.org/xorg/lib/libxcb)  
  
Patch for projects 
```
# Build & Install macros

# Build & Install xproto

# Build & Install libXau

## In Makefile.am
### libXau_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

# Build & Install xcbproto

# Build & Install pthread-stubs

# Build & Install libxcb

## In src/Makefile.am
### libxcb_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter
### libxcb_xkb_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX
### libxcb_xinput_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX
```  

## -----------------------------------------------------------------------------------

chrpath  
```
target_arch=x86_64 ##x86 ##arm64 ##arm

chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libXau.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxcb.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxcb-xinput.so"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libxcb-xkb.so"
```
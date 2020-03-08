Linux Version: EL7  

[NDK r11c](https://developer.android.com/ndk/downloads/older_releases)  

[Building Open Source Projects Using Standalone Toolchains](https://developer.android.com/ndk/guides/standalone_toolchain#building_open_source_projects_using_standalone_toolchains)  

Create x86_64 toolchain  
```  
$NDK/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --arch=x86_64 --platform=android-24 --install-dir="$HOME/bionic-toolchain-x86_64"
```  
  
Create x86 toolchain  
```
$NDK/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --arch=x86 --platform=android-24 --install-dir="$HOME/bionic-toolchain-x86"
```

Create arm64 toolchain
```
$NDK/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --arch=arm64 --platform=android-24 --install-dir="$HOME/bionic-toolchain-arm64"
```

[macros](https://gitlab.freedesktop.org/xorg/util/macros)  
[xproto](https://gitlab.freedesktop.org/xorg/proto/xproto)  
[libXau](https://gitlab.freedesktop.org/xorg/lib/libxau)  
[xcbproto](https://gitlab.freedesktop.org/xorg/proto/xcbproto)  
[pthread-stubs](https://gitlab.freedesktop.org/xorg/lib/pthread-stubs)  
[libxcb](https://gitlab.freedesktop.org/xorg/lib/libxcb)  
  
Build x86_64 binaries  
```  
# Add the standalone toolchain to the search path.
export PATH="$HOME/bionic-toolchain-x86_64/bin":$PATH 

# Tell configure what tools to use.
target_host=x86_64-linux-android
export AR=$target_host-ar
export AS=$target_host-clang
export CC=$target_host-clang
export CXX=$target_host-clang++
export LD=$target_host-ld
export STRIP=$target_host-strip

# Tell configure what flags Android requires.
export CFLAGS="-fPIE -fPIC"
export LDFLAGS="-pie"

# Build & Install macros
autoreconf -v --install --force # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-x86_64/sysroot/usr" --libdir="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" --host=$target_host
make install

export PKG_CONFIG_PATH="$HOME/bionic-toolchain-x86_64/sysroot/usr/share/pkgconfig":$PKG_CONFIG_PATH

# Build & Install xproto
autoreconf -v --install --force # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-x86_64/sysroot/usr" --libdir="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" --host=$target_host --host=$target_host
make install

export PKG_CONFIG_PATH="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64/pkgconfig":$PKG_CONFIG_PATH

# Build & Install libXau
autoreconf -v --install --force # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-x86_64/sysroot/usr" --libdir="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" --host=$target_host
make install

# We have exported the path above
# export PKG_CONFIG_PATH="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64/pkgconfig":$PKG_CONFIG_PATH

# Build & Install xcbproto
autoreconf -v --install # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-x86_64/sysroot/usr" --libdir="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" --host=$target_host
make install

# We have exported the path above
# export PKG_CONFIG_PATH="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64/pkgconfig":$PKG_CONFIG_PATH

# Build & Install pthread-stubs
autoreconf -v --install # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-x86_64/sysroot/usr" --libdir="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" --host=$target_host
make install

# We have exported the path above
# export PKG_CONFIG_PATH="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64/pkgconfig":$PKG_CONFIG_PATH

# Build & Install libxcb

# Since we will use "--prefix=..." in the following ./configure
# Comment out the "m4_ifndef([XORG_MACROS_..." in configure.ac

autoreconf -v --install # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-x86_64/sysroot/usr" --libdir="$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" --host=$target_host
make install

```  

chrpath  
```
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64/libXau.so.6"
chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64/libxcb.so.1"
```

Build arm64 binaries  
```
# Add the standalone toolchain to the search path.
export PATH="$HOME/bionic-toolchain-arm64/bin":$PATH

# Tell configure what tools to use.
target_host=aarch64-linux-android
export AR=$target_host-ar
export AS=$target_host-clang
export CC=$target_host-clang
export CXX=$target_host-clang++
export LD=$target_host-ld
export STRIP=$target_host-strip

# Tell configure what flags Android requires.
export CFLAGS="-fPIE -fPIC"
export LDFLAGS="-pie"

# Autoconf
autoreconf -v --install --force # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-arm64/sysroot/usr" --libdir="$HOME/bionic-toolchain-arm64/sysroot/usr/lib" --host=$target_host
make install

export PKG_CONFIG_PATH="$HOME/bionic-toolchain-arm64/sysroot/usr/share/pkgconfig":$PKG_CONFIG_PATH
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-arm64/sysroot/usr/lib/pkgconfig":$PKG_CONFIG_PATH

# libXau
AM_LDFLAGS = -Wl,-rpath,/enough_long_path/enough_long_path/enough_long_path/enough_long_path  

## chrpath can only make path shorter

```
## Bionic-based Linux  
[bionic](https://android.googlesource.com/platform/bionic)  

由于glibc和stdc++的版本在不同Linux发行版之间差异较大，在一个Linux发行版上编译的二进制文件很难在未经重新编译的情况下在另一个Linux发行版上稳定并且行为一致地运行（Linux上的软件都倾向于开源，开发者们普遍期望最终用户下载源代码后在本地编译安装(著名的make install命令)，而很少考虑提供预编译的二进制文件供最终用户使用）  

因此，我们考虑到使用Android的bionic(-android-7.0.0_r36)，从而可以确保在一次编译后可以在任何的Linux发行版上稳定并且行为一致地运行  

参考google提供的官方文档[Building Open Source Projects Using Standalone Toolchains](https://developer.android.com/ndk/guides/standalone_toolchain#building_open_source_projects_using_standalone_toolchains)，我们基于bionic(-ndk-r11c)重新编译了PatriotEngine所依赖的所有第三方库（[X11](X11/README.md)等)  

同时，使用Android的bionic还带来了一个潜在的好处：我们可以复用Android的工具链(ndk-build等)，理论上，Linux可以和Android共用同一套编译系统，大大减少了开发时间  

[NDK r11c](https://developer.android.com/ndk/downloads/older_releases)  
  
[Building Open Source Projects Using Standalone Toolchains](https://developer.android.com/ndk/guides/standalone_toolchain#building_open_source_projects_using_standalone_toolchains)  

## Create ndk toolchain  
  
```
target_arch=x86_64 ##x86 ##arm64 ##arm

my-ndk-dir/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --arch="$target_arch" --platform=android-24 --install-dir="$HOME/bionic-toolchain-$target_arch"

## Patch for x86_64 
rm -rf "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib"
rm -rf "$HOME/bionic-toolchain-x86_64/sysroot/usr/libx32"
mv "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib"
```  

## Build autoconf projects

Linux Version: EL7  
```
yum install libtool ## Used by autoconf

```

Build projects  
```  
target_arch=x86_64 ##x86 ##arm64 ##arm
target_host=x86_64-linux-android  ##i686-linux-android ##aarch64-linux-android ##arm-linux-androideabi

# Add the standalone toolchain to the search path.
export PATH="$HOME/bionic-toolchain-$target_arch/bin"${PATH:+:${PATH}}

# Tell configure what tools to use.
# https://www.gnu.org/software/autoconf/manual/autoconf-2.66/html_node/Environment-Variable-Index.html
export AR=$target_host-ar
export AS=$target_host-clang
export CC=$target_host-clang
export CXX=$target_host-clang++
export LD=$target_host-ld
export STRIP=$target_host-strip

# Tell configure what flags Android requires.
export CFLAGS="-fPIE -fPIC"
export CXXFLAGS="-fPIE -fPIC"
export LDFLAGS="-pie"

# pkg-config
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/share/pkgconfig"${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/pkgconfig"${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}

# Autoconf
autoreconf -v --install --force -I"$HOME/bionic-toolchain-$target_arch/sysroot/usr/share/aclocal" # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-$target_arch/sysroot/usr" --host=$target_host
make install
```  

Patch for projects  
```
## in Makefile.am
### lib***_la_LDFLAGS = ... -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter
```

chrpath  
```
target_arch=x86_64 ##x86 ##arm64 ##arm

chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/lib***.so"
```

## Build meson  

Linux Version: EL7  
```
yum install meson

delete python python2 python2.7 in "$HOME/bionic-toolchain-$target_arch/bin" ### meson use python3

yum install gcc gcc-c++
delete clang clang++ in "$HOME/bionic-toolchain-$target_arch/bin" ### meson will compile build-machine binaries when call **project** in **meson.build** even if it is cross build

```

Build projects  
```  
target_arch=x86_64 ##x86 ##arm64 ##arm
target_host=x86_64-linux-android  ##i686-linux-android ##aarch64-linux-android ##arm-linux-androideabi

# Add the standalone toolchain to the search path.
export PATH="$HOME/bionic-toolchain-$target_arch/bin"${PATH:+:${PATH}}

# pkg-config
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/share/pkgconfig"${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}
export PKG_CONFIG_PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/pkgconfig"${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}

# meson
## mkdir build
## cd build
meson .. --prefix="$HOME/bionic-toolchain-$target_arch/sysroot/usr" --buildtype=release  -Db_ndebug=true --cross-file="$HOME/bionic-toolchain-$target_arch/cross_file.txt" 
```  

### -----------------------------------------------------------------------------------  

#### x86_64 cross file
```
[binaries]
ar = 'x86_64-linux-android-gcc-ar'
as = 'x86_64-linux-android-clang'
c = 'x86_64-linux-android-clang'
cpp = 'x86_64-linux-android-clang'
ld = 'x86_64-linux-android-ld'
strip = 'x86_64-linux-android-strip'
pkgconfig = 'pkg-config'

[properties]
c_args = ['-fPIE', '-fPIC']
c_link_args = ['-pie']
cpp_args = ['-fPIE', '-fPIC']
cpp_link_args = ['-pie']

[host_machine]
system = 'linux'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'
```

#### arm64 cross file
```
[binaries]
ar = 'aarch64-linux-android-gcc-ar'
as = 'aarch64-linux-android-clang'
c = 'aarch64-linux-android-clang'
cpp = 'aarch64-linux-android-clang'
ld = 'aarch64-linux-android-ld'
strip = 'aarch64-linux-android-strip'

[properties]
c_args = ['-fPIE', '-fPIC']
c_link_args = ['-pie']
cpp_args = ['-fPIE', '-fPIC']
cpp_link_args = ['-pie']

[host_machine]
system = 'linux'
cpu_family = 'aarch64'
cpu = 'arm64'
endian = 'little'
```
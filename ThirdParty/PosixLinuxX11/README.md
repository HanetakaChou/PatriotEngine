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
target_host=x86_64-linux-android  ##i686-linux-android ##aarch64-linux-android ##arm-linux-androideabi

## Path for ndk ## default to c++shared
mv my-ndk-dir/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_static.a my-ndk-dir/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_static.a.bak
ln -s libc++_shared.so my-ndk-dir/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_static.a
mv my-ndk-dir/sources/cxx-stl/llvm-libc++/libs/x86/libc++_static.a my-ndk-dir/sources/cxx-stl/llvm-libc++/libs/x86/libc++_static.a.bak
ln -s libc++_shared.so my-ndk-dir/sources/cxx-stl/llvm-libc++/libs/x86/libc++_static.a

## Alternative
rm -rf "$HOME/bionic-toolchain-x86_64/${target_host}/lib/libstdc++.a"
ln -s libc++_shared.so "$HOME/bionic-toolchain-x86_64/${target_host}/lib/libstdc++.a"

my-ndk-dir/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --arch="$target_arch" --platform=android-24 --install-dir="$HOME/bionic-toolchain-$target_arch"

## Patch for x86_64 
rm -rf "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib"
rm -rf "$HOME/bionic-toolchain-x86_64/sysroot/usr/libx32"
mv "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib64" "$HOME/bionic-toolchain-x86_64/sysroot/usr/lib"

rm -rf "$HOME/bionic-toolchain-x86_64/x86_64-linux-android/lib"
rm -rf "$HOME/bionic-toolchain-x86_64/x86_64-linux-android/libx32"
mv "$HOME/bionic-toolchain-x86_64/x86_64-linux-android/lib64" "$HOME/bionic-toolchain-x86_64/x86_64-linux-android/lib"

```  

## Build autoconf projects

Linux Version: EL8  
```
install autoconf automake libtool make el8 version ### there are bugs in el7 version ### use rpmrebuild to remove gcc dependency in libtool
rpm -e --nodeps gcc gcc-c++ kernel-headers glibc-headers glibc-devel libstdc++-devel ### 避免对sysroot造成干扰
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
export LDFLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX"

# pkg-config ### https://autotools.io/pkgconfig/cross-compiling.html
target_sysroot="$HOME/bionic-toolchain-$target_arch/sysroot"
export PKG_CONFIG_PATH=
export PKG_CONFIG_LIBDIR=${target_sysroot}/usr/lib/pkgconfig:${target_sysroot}/usr/share/pkgconfig
export PKG_CONFIG_SYSROOT_DIR=${target_sysroot}

# Autoconf
make clean
autoreconf -v --install --force -I"${target_sysroot}/usr/share/aclocal"  # From ./autogen.sh
./configure --prefix="$HOME/bionic-toolchain-$target_arch/sysroot/usr" --host=$target_host --disable-static
make install
```  

## Build meson projects 

Linux Version: EL8  
```
yum install meson

delete python python2 python2.7 in "$HOME/bionic-toolchain-$target_arch/bin" ### meson use python3

delete clang clang++ in "$HOME/bionic-toolchain-$target_arch/bin"

rpm -e --nodeps gcc gcc-c++ kernel-headers glibc-headers glibc-devel libstdc++-devel ### 避免对sysroot造成干扰

### meson will compile build-machine binaries when call **project** in **meson.build** even if it is cross build

we must copy the bionic to the /system path to pass the meson sanitycheck #### we can retrieve the program interpreter path /system/bin/linker(64) by "readelf -l"


```

Build projects  
```  
target_arch=x86_64 ##x86 ##arm64 ##arm
target_host=x86_64-linux-android  ##i686-linux-android ##aarch64-linux-android ##arm-linux-androideabi

# Add the standalone toolchain to the search path.
export PATH="$HOME/bionic-toolchain-$target_arch/bin"${PATH:+:${PATH}}

# pkg-config ### https://autotools.io/pkgconfig/cross-compiling.html  
target_sysroot="$HOME/bionic-toolchain-$target_arch/sysroot"
export PKG_CONFIG_PATH=
export PKG_CONFIG_LIBDIR=${target_sysroot}/usr/lib/pkgconfig:${target_sysroot}/usr/share/pkgconfig
export PKG_CONFIG_SYSROOT_DIR=${target_sysroot}

# Add the standalone toolchain to the search path 
### meson will compile build-machine binaries when call **project** in **meson.build** even if it is cross build
export PATH="$HOME/meson-sanitycheck-$target_arch"${PATH:+:${PATH}}

rm -rf "$HOME/meson-sanitycheck-$target_arch"
mkdir -p "$HOME/meson-sanitycheck-$target_arch"
echo '#!/bin/bash' > "$HOME/meson-sanitycheck-$target_arch"/clang ### add #!/bin.bash to avoid "Exec format error" from meson
echo "$target_host-clang -fPIE -fPIC -pie \"\$@\"" >> "$HOME/meson-sanitycheck-$target_arch"/clang
echo '#!/bin/bash' > "$HOME/meson-sanitycheck-$target_arch"/clang++ ### add #!/bin.bash to avoid "Exec format error" from meson
echo "$target_host-clang++ -fPIE -fPIC -pie \"\$@\"" >> "$HOME/meson-sanitycheck-$target_arch"/clang++
chmod +x "$HOME/meson-sanitycheck-$target_arch"/clang
chmod +x "$HOME/meson-sanitycheck-$target_arch"/clang++

## my-llvm-config-dir
export PATH="$HOME/bionic-toolchain-$target_arch/sysroot/usr/bin"${PATH:+:${PATH}} 

## 
### delete EGL and GLES in toolchain sysroot ### may conflicts with mesa

# meson
## mkdir build
## cd build
## rm -rf *
meson .. --prefix="$HOME/bionic-toolchain-$target_arch/sysroot/usr" --buildtype=release  -Db_ndebug=true --cross-file="$HOME/bionic-toolchain-$target_arch/cross_file.txt" ### --reconfigure 

rm -rf "$HOME/meson-sanitycheck-$target_arch" ## the sanity check has finished 
ninja

```  

### -----------------------------------------------------------------------------------  

#### x86_64 cross file
```
[binaries]
ar = 'x86_64-linux-android-ar'
as = 'x86_64-linux-android-clang' 
c = 'x86_64-linux-android-clang'
cpp = 'x86_64-linux-android-clang++'
ld = 'x86_64-linux-android-ld'
strip = 'x86_64-linux-android-strip'
pkgconfig = 'pkg-config'

[properties]
c_args = ['-fPIE', '-fPIC']
c_link_args = ['-pie', '-Wl,--enable-new-dtags', '-Wl,-rpath,/XXXXXX'] ### -shared conflicts -pie ### results errors when use gcc
cpp_args = ['-fPIE', '-fPIC']
cpp_link_args = ['-pie', '-Wl,--enable-new-dtags', '-Wl,-rpath,/XXXXXX']

[host_machine]
system = 'linux'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'
```

#### arm64 cross file
```
[binaries]
ar = 'aarch64-linux-android-ar'
as = 'aarch64-linux-android-clang'
c = 'aarch64-linux-android-clang'
cpp = 'aarch64-linux-android-clang++'
ld = 'aarch64-linux-android-ld'
strip = 'aarch64-linux-android-strip'

[properties]
c_args = ['-fPIE', '-fPIC']
c_link_args = ['-pie', '-Wl,--enable-new-dtags', '-Wl,-rpath,/XXXXXX']
cpp_args = ['-fPIE', '-fPIC']
cpp_link_args = ['-pie', '-Wl,--enable-new-dtags', '-Wl,-rpath,/XXXXXX']

[host_machine]
system = 'linux'
cpu_family = 'aarch64'
cpu = 'arm64'
endian = 'little'
```

## Build cmake projects 


```  
target_arch=x86_64 ##x86 ##arm64 ##arm
target_host=x86_64-linux-android  ##i686-linux-android ##aarch64-linux-android ##arm-linux-androideabi

# Add the standalone toolchain to the search path.
rm -rf "$HOME/cmake-$target_arch"
mkdir -p "$HOME/cmake-$target_arch"
echo "$target_host-ar \"\$@\"" > "$HOME/cmake-$target_arch"/ar
echo "$target_host-clang \"\$@\"" > "$HOME/cmake-$target_arch"/as
echo "$target_host-clang \"\$@\"" > "$HOME/cmake-$target_arch"/cc
echo "$target_host-clang++ \"\$@\"" > "$HOME/cmake-$target_arch"/c++
echo "$target_host-ld \"\$@\"" > "$HOME/cmake-$target_arch"/ld
echo "$target_host-strip \"\$@\"" > "$HOME/cmake-$target_arch"/strip
echo "$target_host-nm \"\$@\"" > "$HOME/cmake-$target_arch"/nm
echo "$target_host-objcopy \"\$@\"" > "$HOME/cmake-$target_arch"/objcopy
echo "$target_host-objdump \"\$@\"" > "$HOME/cmake-$target_arch"/objdump
echo "$target_host-ranlib \"\$@\"" > "$HOME/cmake-$target_arch"/ranlib
chmod +x "$HOME/cmake-$target_arch"/ar
chmod +x "$HOME/cmake-$target_arch"/as
chmod +x "$HOME/cmake-$target_arch"/cc
chmod +x "$HOME/cmake-$target_arch"/c++
chmod +x "$HOME/cmake-$target_arch"/ld
chmod +x "$HOME/cmake-$target_arch"/strip
chmod +x "$HOME/cmake-$target_arch"/nm
chmod +x "$HOME/cmake-$target_arch"/objcopy
chmod +x "$HOME/cmake-$target_arch"/objdump
chmod +x "$HOME/cmake-$target_arch"/ranlib
export PATH="$HOME/bionic-toolchain-$target_arch/bin"${PATH:+:${PATH}}
export PATH="$HOME/cmake-$target_arch"${PATH:+:${PATH}}

cd build
cmake .. -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="$HOME/bionic-toolchain-$target_arch/sysroot/usr" -DCMAKE_C_FLAGS="-fPIE -fPIC" -DCMAKE_CXX_FLAGS="-fPIE -fPIC" -DCMAKE_EXE_LINKER_FLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX" -DCMAKE_MODULE_LINKER_FLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX" -DCMAKE_SHARED_LINKER_FLAGS="-pie -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX" -DCMAKE_SKIP_INSTALL_RPATH=ON

cmake-gui

```  
  
### chrpath  /XXXXXX -> \$ORIGIN
```
target_arch=x86_64 ##x86 ##arm64 ##arm

chrpath -r "\$ORIGIN" "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/lib***.so"
```  

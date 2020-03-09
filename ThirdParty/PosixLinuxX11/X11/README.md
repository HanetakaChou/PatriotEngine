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
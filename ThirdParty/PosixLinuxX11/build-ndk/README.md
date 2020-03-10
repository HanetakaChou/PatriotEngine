 
build  
  
```
rm -rf obj
rm -rf libs
ndk-build NDK_DEBUG=1 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk 
```

before execute change the rpath to \$ORIGIN  
  
```
chrpath -r libs/x86_64/a.out
```
  
copy the dep libs to out dir  
  
```
cp ../Bionic/lib64/libc.so libs/x86_64
cp ../Bionic/lib64/libdl.so libs/x86_64
cp ../Bionic/lib64/libm.so libs/x86_64
cp ../Bionic/lib64/libstdc++.so libs/x86_64
```  

add the linker to PATH
```
export PATH="$(pwd)/../Bionic/bin64"${PATH:+:${PATH}}
```
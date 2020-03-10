 
### build by ndk
  
```
rm -rf obj
rm -rf libs
ndk-build NDK_DEBUG=1 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk 
```

### before execute change the rpath to \$ORIGIN  
  
```
chrpath -r '$ORIGIN' libs/x86_64/a.out
```
  
### copy the dep libs to out dir  
  
```
cp -f ../Bionic/lib64/libc.so libs/x86_64
cp -f ../Bionic/lib64/libdl.so libs/x86_64
cp -f ../Bionic/lib64/libm.so libs/x86_64
cp -f ../Bionic/lib64/libstdc++.so libs/x86_64
```  

### copy the linker to cwd  


```
cp -f ../Bionic/bin64/linker .
```
  
the Linux kernel use openat(AT_FDCWD, ...) to open the linker  
\-\- load_elf_binary \#\#\# fs/binfmt_elf.c  
\-\-\-\- open_exec(elf_interpreter) \#\#\# fs/binfmt_elf.c  
\-\-\-\- open_exec(name) \#\#\# fs/exec.c  
\-\-\-\-\-\- do_open_execat(AT_FDCWD, filename, ...) \#\#\# fs/exec.c  
and thus we must copy the linker to cwd  

### execute the generated a.out  

```
libs/x86_64/a.out
```
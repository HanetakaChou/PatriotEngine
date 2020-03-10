 
### build by ndk
  
```
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=your-built-type APP_ABI:=your-target-arch NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk 
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
cp -f ../Bionic/bin64/linker libs/x86_64
cd libs/x86_64
```
  
the Linux kernel use openat(AT_FDCWD, ...) to open the linker  
\-\- load_elf_binary \#\#\# fs/binfmt_elf.c  
\-\-\-\- open_exec(elf_interpreter) \#\#\# fs/binfmt_elf.c  
\-\-\-\- open_exec(name) \#\#\# fs/exec.c  
\-\-\-\-\-\- do_open_execat(AT_FDCWD, filename, ...) \#\#\# fs/exec.c  
and thus we must copy the linker to cwd  

### execute the generated a.out  

```
./a.out
```

### debug by ndk  
./build-and-debug-x86-64.sh  
> set APP_DEBUG to true   

vscode [launch.json](.vscode/launch.json)  
> set program to a.out in obj instead of in libs  
> set miDebuggerArgs to --init-command=.../gdb.setup  

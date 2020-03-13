[glslang](git@github.com:KhronosGroup/glslang.git)  
[SPIRV-Headers](https://github.com/KhronosGroup/SPIRV-Headers)  
[SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools)  
[Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader/tree/sdk-1.0.68.0) \#\#\# tree/sdk-1.0.68.0 download from the website directly  
[Vulkan-Loader-modified](https://github.com/YuqiaoZhang/Vulkan-Loader)  

# build rules for specific projects 

## glslang

patch libpthread.so  
```  
ln -s libc.so "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libpthread.so"  
```  

## SPIRV-Tools

remove static libs (libSPIRV-Tools-link.a libSPIRV-Tools-opt.a libSPIRV-Tools-reduce.a libSPIRV-Tools.a)  
libSPIRV.a and libSPVRemapper.a are from glslang and should not be removed  

## Vulkan-Loader  
  
add -DBUILD_WSI_WAYLAND_SUPPORT=OFF to cmake options
  
patch librt.so  
```  
ln -s libc.so "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/librt.so"  
``` 

fix **spirv_tools_commit_id.h** issue  
```
#ifndef _SPIRV_TOOLS_COMMIT_ID_H_
#define _SPIRV_TOOLS_COMMIT_ID_H_ 1

#define SPIRV_TOOLS_COMMIT_ID "..." //git commit id

#endif
```

patch loader_get_manifest_files in loader.c
```
if(override == NULL)
{
    //patch code here
    //use dladdr to write the proper path to the **loc**
}
else
{
     loc = loader_stack_alloc(strlen(override) + 1);
     strcpy(loc, override);
}
```
patch loader_platform_open_library in vk_loader_platform.h  
```
dlopen(... RTLD_LAZY ...
->
dlopen(... RTLD_NOW ...
```

patch CMakeLists.txt to use shared SPIRV-Tools  
```
find_library(SPIRV_TOOLS_LIB NAMES SPIRV-Tools ... -> SPIRV-Tools-shared
find_library(SPIRV_TOOLS_LIB NAMES SPIRV-Tools-opt ... -> SPIRV-Tools-shared
```  

patch CMakeLists.txt to remove version in name of shared lib
```
set_target_properties(... PROPERTIES SOVERSION ...)
->
# set_target_properties(... PROPERTIES SOVERSION ...)
``` 

patch CMakeLists.txt to make smoketest load vulkan correctly
```
... -DUNINSTALLED_LOADER ...
->
# ... -DUNINSTALLED_LOADER ...
```

in demos/smoke/ShellWayland.cpp
```
... filename ... libvulkan.so.1 ...
->
... filename ... libvulkan.so ...
```

in demos/smoke/ShellWayland.cpp
```
... filename ... libvulkan.so.1 ...
->
... filename ... libvulkan.so ...
```
[SPIRV-Headers](https://github.com/KhronosGroup/SPIRV-Headers)  
[SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools)  
[Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader/tree/sdk-1.0.68.0) \#\#\# tree/sdk-1.0.68.0 download from the website directly  

# build rules for specific projects 

## Vulkan-Loader  
  
add -DBUILD_WSI_WAYLAND_SUPPORT=OFF to cmake options
  
patch librt.so  
```  
ln -s libc.so "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/librt.so"  
``` 

patch libpthread.so  
```  
ln -s libc.so "$HOME/bionic-toolchain-$target_arch/sysroot/usr/lib/libpthread.so"  
```  

fix **spirv_tools_commit_id.h** issue  
```
#ifndef _SPIRV_TOOLS_COMMIT_ID_H_
#define _SPIRV_TOOLS_COMMIT_ID_H_ 1

#define SPIRV_TOOLS_COMMIT_ID "..." //git commit id

#endif
```
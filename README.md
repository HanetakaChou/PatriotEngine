﻿### Project Description     
PatriotEngine aims to be a state-of-the-art real-time **image synthesis(rendering)** engine and I believe that more and more programmers unabatedly endeavor to develop their own engines.  

Although there is no official standard of the **rendering** engine at present, the functionality of the **rendering** engine has steadily stabilized due to the evolution of the **rendering** engine during the past few decades and thus has formed the de facto standard.   

It is widely establish that "program = data structure + algorithm" and this engine will elaborate the **rendering** from two aspects: scene (data structure) and rendering pipeline (algorithm). 

Here is the document of this engine: [https://hanetakachou.github.io/PatriotEngine/index.html](https://hanetakachou.github.io/PatriotEngine/index.html)  

### Continuous build status    
    
Build Type | Status  
:-: | :-:  
**Build Win32 Desktop** | [![Build Win32](https://github.com/HanetakaChou/PatriotEngine/actions/workflows/build_win32_desktop.yml/badge.svg)](https://github.com/HanetakaChou/PatriotEngine/actions/workflows/build_win32_desktop.yml)  
**Build Posix Linux Glibc** | [![Build Posix Linux Glibc](https://github.com/HanetakaChou/PatriotEngine/actions/workflows/build_posix_linux_gblic.yml/badge.svg)](https://github.com/HanetakaChou/PatriotEngine/actions/workflows/build_posix_linux_gblic.yml)  

### Design Philosophy  

We may treat the image synthesis engine as the 3D version X11 server.  
The geometry(e.g. mesh, hair, terrain) / material / texture / light(e.g. directional light, punctual light, area light, light probe) are analogous to the pixmap on X11 server      
and the hierarchy of the scenetree / scenegraph are analogous to the relationship of the "child-parent" window on X11 server.  
   
The asset streaming process is totally asynchronous. This means that the calling thread will not halt at all. Thus we can definitely call these functions in the gameplay logic and no performance penalty will be introduced.  

```  
    pt_gfx_connection_init
    pt_gfx_connection_destroy

    // scene related
    pt_gfx_connection_create_node // Top-Level-Structure // to reuse mesh etc 
    gfx_connection_create_selector //LOD etc  
    gfx_connection_create_...
    
    pt_gfx_node_set_mesh
    pt_gfx_node_set_material
    gfx_node_set ...

    // asset related
    pt_gfx_connection_create_mesh
    gfx_connection_create_hair
    pt_gfx_connection_create_texture
    gfx_connection_create_light_probe
    gfx_connection_create_...

    // asset streaming
    pt_gfx_mesh_read_input_stream
    pt_gfx_material_init_with_texture
    pt_gfx_mesh_destroy
    pt_gfx_material_destroy

    // wsi related
    gfx_connection_wsi_on_resized

    // usage
    // [current thread] app on_redraw_needed //drawInMTKView 
    // [arbitrary thread] app update info which doesn't depend on accurate time ( scenetree etc ) //app may update in other threads 
    // [current thread] app call gfx acquire //gfx sync ( from other threads ) and flatten scenetree //and then gfx frame throttling
    // [current thread] app update time-related info ( animation etc ) //frame throttling make the time here less latency //scenetree update here (include update from other threads) is ignored in current frame and to impact on the next frame
    // [current thread] app call gfx release //gfx draw and present //gfx not sync scenetree here
    gfx_connection_wsi_on_redraw_needed_acquire

    // app related update

    gfx_connection_wsi_on_redraw_needed_release
```   

### Platform Support  
- [ ] Vulkan  
  - [ ] MemoryAllocator  
    - [x] VMA  
  - [ ] TextureLoader  
    - [x] DDS  
    - [x] PVR  
    - [ ] KTX
  - [ ] ModelLoader
    - [x] PMX
    - [ ] GLTF
    - [ ] ALEMBIC
    - [ ] USD
    - [ ] FBX
  - [x] WSI  
    - [x] PosixLinuxX11  
    - [x] PosixLinuxAndroid  
    - [x] PosixMachOSX
    - [x] PosixMachIOS
    - [x] Win32Desktop  
- [ ] Metal  
  - [ ] TextureLoader  
    - [ ] DDS  
    - [ ] PVR  
    - [ ] KTX
  - [ ] ModelLoader
    - [ ] PMX
    - [ ] GLTF
    - [ ] ALEMBIC
    - [ ] USD
    - [ ] FBX
  - [ ] WSI  
    - [ ] PosixMachOSX  
    - [ ] PosixMachIOS  
- [ ] Direct3D12  
  - [ ] MemoryAllocator  
    - [ ] AMD VMA  
  - [ ] TextureLoader  
    - [ ] DDS  
    - [ ] PVR  
    - [ ] KTX
  - [ ] ModelLoader
    - [ ] PMX
    - [ ] GLTF
    - [ ] ALEMBIC
    - [ ] USD
    - [ ] FBX
  - [ ] WSI  
    - [ ] Win32Desktop  

### Feature List
- [ ] Scene  
  - [ ] SceneGraph / SceneTree   
  - [ ] Switch Algorithm //LOD ...   
  - [ ] Culling Algorithm //Frustum Culling, Occlusion Culling ...    
- [ ] Geometry  
  - [ ] Mesh   
  - [ ] Decal  
  - [ ] Hair 
  - [ ] Cloth    
  - [ ] Ocean   
  - [ ] Terrain    
  - [ ] Grass  
  - [ ] Tree  
- [ ] Participating Media   
  - [ ] Volumetric Lighting/Fog/Cloud ...  
- [ ] Color  
  - [ ] UHD Display    
- [ ] Material   
  - [ ] MDL Frontend   
  - [ ] OSL Frontend  
  - [ ] LLVM IR    
  - [ ] GLSL Backend    
  - [ ] MSL Backend       
  - [ ] HLSL Backend   
- [ ] Light //The first item of the Liouville–Neumann series of the Rendering Equation  
  - [ ] Illumination //Tiled Shading, Clustered Shading, OIT ...    
  - [ ] Shadow Map //CSM, PCF, VSM ...  
- [ ] Light Probe //The second item of the Liouville–Neumann series of the Rendering Equation  
  - [ ] Spherical Function //SH, LTC, Spherical Wavelets ...  
  - [ ] IBL //Importance Sampling ...  
  - [ ] SSR  
  - [ ] SSAO/HBAO/HBAO+/SSDO  
- [ ] Global Illumination //The Whole Rendering Equation  
  - [ ] Radiosity  
  - [ ] Ray Tracing //VXGI, RTX ...  
- [ ] PostProcess  
  - [ ] Color //UHD Display ...  
  - [ ] AA //FXAA, TAA ...  
  - [ ] Effect //DOF ...  

### Project Description     
PatriotEngine aims to be a state-of-the-art real-time **image synthesis(rendering)** engine and I believe that more and more Chinese programmers unabatedly endeavor to resist and develop their own engines.  

Although there is no official standard of the **rendering** engine at present, the functionality of the **rendering** engine has steadily stabilized due to the evolution of the **rendering** engine during the past few decades and thus has formed the de facto standard.   

It is widely establish that "program = data structure + algorithm" and this engine will elaborate the **rendering** from two aspects: scene (data structure) and rendering pipeline (algorithm). 

Here is the document of this engine: [https://yuqiaozhang.github.io/PatriotEngine/index.html](https://yuqiaozhang.github.io/PatriotEngine/index.html)  

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
 
### Continuous build status    
    
Build Type | Status  
:-: | :-:  
**Build Posix Linux X11 On Ubuntu** | [![Build Posix Linux X11 On Ubuntu](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_linux_x11_on_ubuntu.yml/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_linux_x11_on_ubuntu.yml)  
**Build Win32 Desktop** | [![Build Win32](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_win32_desktop.yml/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_win32_desktop.yml)  
**Build Posix Linux Android On Ubuntu** | [![Build Posix Linux Android On Ubuntu](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_linux_android_on_ubuntu.yml/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_linux_android_on_ubuntu.yml)  
**Build Posix Linux Android On MacOSX** | [![Build Posix Linux Android On MacOSX](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_linux_android_on_macosx.yml/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_linux_android_on_macosx.yml)  
**Build Posix Mach OSX** | [![Build Posix Mach OSX](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_mach_osx.yml/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_mach_osx.yml)  
**Build Posix Mach Catalyst** | [![Build Posix Mach Catalyst](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_mach_catalyst.yml/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions/workflows/build_posix_mach_catalyst.yml)  

### To Do  
* 1\.To learn the Differential Geometry and try to treat the Steradian as the 2-Manifold which reduces the dimensions from 3(R^3) to 2(S^2) and simplifies the integral over the sphere surface when one calculates the lighting.      
Try to understand the following papers from the perspective of the 2-Manifold:         
> * An Introduction to Manifolds / Example 23.11 (Integral over a sphere)     
\[Tu 2011\] Loring Tu. "An Introduction to Manifolds, Second Edition." Springer 2011.    
> * LTC  
\[Heitz 2016\] [Eric Heitz, Jonathan Dupuy, Stephen Hill and David Neubelt. "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines." SIGGRAPH 2016](https://eheitzresearch.wordpress.com/415-2/)     

* To my pleasure, someone seems to have the similar idea which I find on the Google Scholar:      
> * \[Herholz 2018\] Sebastian Herholz, Oskar Elek, Jens Schindel, Jaroslav krivanek, Hendrik Lensch. "A Unified Manifold Framework for Efficient BRDF Sampling based on Parametric Mixture Models." EGSR 2018.    
* The traditional Euclidean-Space method may be replaced by the efficient 2-Manifold method in the next few years.    
    

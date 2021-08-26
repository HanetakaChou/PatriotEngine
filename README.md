### Project Description     
PatriotEngine aims to be a state-of-the-art real-time image synthesis engine and I believe that more and more programmers tend to develop their own engines without being restricted to Unity or UE4. 

Although there is no official standard of the **Graphics Rendering** Engine at present, the functionality of the **Graphics Rendering** Engine has steadily stabilized with the evolution of the **Graphics Rendering** Engine in the past few decades and thus has formed the de facto standard.   

It's widely establish that "Program = Data Structure + Algorithm" and this engine will elaborate the **Graphics Rendering** Engine from two aspects: Scene (Data Structure) and Rendering Pipeline (Algorithm). 

The contents of this engine is likely to be as follows:   
>
> [PatriotEngine](https://yuqiaozhang.github.io/PatriotEngine/)  
>    
> [docs/malloc.md](docs/malloc.md)  
> [docs/malloc.zh_CN.md](docs/malloc.zh_CN.md)  
>
> --- 
>
> [docs/tbb.md](docs/tbb.md)  
> [docs/task.zh_CN.md](docs/task.zh_CN.md)
>       
> ---
> 
> [docs/parallel-rendering-overview.md](docs/parallel-rendering-overview.md)  
> [docs/OIT.md](docs/OIT.md)  
> [docs/Grass.md](docs/Grass.md)     
> [docs/Volumetric-Rendering.md](docs/Volumetric-Rendering.md)     
>
---  

### Core Interfaces   
   
#### gfx_connection_ref   
  
We may treat the image synthesis graphics engine as the 3D version X11 server.  
The geometry(e.g. mesh, hair, terrain) / material / texture / light(e.g. directional light, punctual light, area light, light probe) are analogous to the pixmap on X11 server      
and the hierarchy of the scenetree / scenegraph are analogous to the relationship of the "child-parent" window on X11 server.  
   
The user can't control whether the "content" of the mesh / hair / material / ... is resident on the GPU since the memory allocation in Vulkan / Direct3D12 / Metal may fail.    
However, the user may tweak the "create / destory" strategy of the mesh / hair / material / ... according to the "fail_count / request_count".

```  
    gfx_connection_init
    gfx_connection_destroy

    //scenetree related
    gfx_connection_create_node  
    gfx_connection_create_selector //LOD etc  
    gfx_connection_create_...

    // asset related
    gfx_connection_create_mesh
    gfx_connection_create_hair
    gfx_connection_create_texture
    gfx_connection_create_light_probe
    gfx_connection_create_...

    // memory budget related
    gfx_connection_fail_count  
    gfx_connection_request_count   

    // wsi related
    gfx_connection_wsi_on_resized

    // usage
    // [current thread] app on_redraw_needed //drawInMTKView //onNativeWindowRedrawNeeded 
    // [arbitrary thread] app update info which doesn't depend on accurate time ( scenetree etc ) //app may update in other threads 
    // [current thread] app call gfx acquire //gfx sync ( from other threads ) and flatten scenetree //and then gfx frame throttling
    // [current thread] app update time-related info ( animation etc ) //frame throttling make the time here less latency //scenetree update here (include update from other threads) is ignored in current frame and to impact on the next frame
    // [current thread] app call gfx release //gfx draw and present //gfx not sync scenetree here
    gfx_connection_wsi_on_redraw_needed_acquire
    gfx_connection_wsi_on_redraw_needed_release
```   
 
---    

### Continuous build status    
    
#### Posix Linux X11    
      
Build Type | Status     
:-: | :-:     
**Ubuntu LTS Glibc Release x86** | [![Ubuntu LTS Glibc Release x86](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Ubuntu%20LTS%20Glibc%20Release%20x86/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Ubuntu+LTS+Glibc+Release+x86%22)    
**Ubuntu LTS Glibc Debug x86** | [![Ubuntu LTS Glibc Debug x86](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Ubuntu%20LTS%20Glibc%20Debug%20x86/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Ubuntu+LTS+Glibc+Debug+x86%22)      
**Ubuntu LTS Glibc Release x64** | [![Ubuntu LTS Glibc Release x64](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Ubuntu%20LTS%20Glibc%20Release%20x64/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Ubuntu+LTS+Glibc+Release+x64%22)    
**Ubuntu LTS Glibc Debug x64** | [![Ubuntu LTS Glibc Debug x64](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Ubuntu%20LTS%20Glibc%20Debug%20x64/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Ubuntu+LTS+Glibc+Debug+x64%22)    
    
#### Posix Linux Android
  
Build Type | Status     
:-: | :-:     
**Android Debug (All APP_ABI) (Build On Linux)** | [![Android Debug (All APP_ABI) (Build On Ubuntu LTS)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Android%20Debug%20(All%20APP_ABI)%20(Build%20On%20Ubuntu%20LTS)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Android+Debug+%28All+APP_ABI%29+%28Build+On+Ubuntu+LTS%29%22)    
**Android Release (All APP_ABI) (Build On Linux)** | [![Android Release (All APP_ABI) (Build On Ubuntu LTS)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Android%20Release%20(All%20APP_ABI)%20(Build%20On%20Ubuntu%20LTS)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Android+Release+%28All+APP_ABI%29+%28Build+On+Ubuntu+LTS%29%22)    
**Android Debug (All APP_ABI) (Build On MacOSX)** | [![Android Debug (All APP_ABI) (Build On MacOSX)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Android%20Debug%20(All%20APP_ABI)%20(Build%20On%20MacOSX)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Android+Debug+%28All+APP_ABI%29+%28Build+On+MacOSX%29%22)    
**Android Release (All APP_ABI) (Build On MacOSX)** | [![Android Release (All APP_ABI) (Build On MacOSX)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Android%20Release%20(All%20APP_ABI)%20(Build%20On%20MacOSX)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Android+Release+%28All+APP_ABI%29+%28Build+On+MacOSX%29%22)           
**Android Debug (All APP_ABI) (Build On Windows)** | [![Android Debug (All APP_ABI) (Build On Windows)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Android%20Debug%20(All%20APP_ABI)%20(Build%20On%20Windows)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Android+Debug+%28All+APP_ABI%29+%28Build+On+Windows%29%22)    
**Android Release (All APP_ABI) (Build On Windows)** | [![Android Release (All APP_ABI) (Build On Windows)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/Android%20Release%20(All%20APP_ABI)%20(Build%20On%20Windows)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22Android+Release+%28All+APP_ABI%29+%28Build+On+Windows%29%22)           

#### Posix Mach        
     
Build Type | Status      
:-: | :-:      
**MacOSX Release (Universal Binary)** | [![MacOSX Release (Universal Binary)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/MacOSX%20Release%20(Universal%20Binary)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22MacOSX+Release+%28Universal+Binary%29%22)         
**MacOSX Debug (Universal Binary)** | [![MacOSX Debug (Universal Binary)](https://github.com/YuqiaoZhang/PatriotEngine/workflows/MacOSX%20Debug%20(Universal%20Binary)/badge.svg)](https://github.com/YuqiaoZhang/PatriotEngine/actions?query=workflow%3A%22MacOSX+Debug+%28Universal+Binary%29%22)     
     
---        

### In Progress    
* 1\.To learn the Differential Geometry and try to treat the Steradian as the 2-Manifold which reduces the dimensions from 3(R^3) to 2(S^2) and simplifies the integral over the sphere surface when one calculates the lighting.      
Try to understand the following papers from the perspective of the 2-Manifold:         
> * An Introduction to Manifolds / Example 23.11 (Integral over a sphere)     
\[Tu 2011\] Loring Tu. "An Introduction to Manifolds, Second Edition." Springer 2011.    
> * LTC  
\[Heitz 2016\] [Eric Heitz, Jonathan Dupuy, Stephen Hill and David Neubelt. "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines." SIGGRAPH 2016](https://eheitzresearch.wordpress.com/415-2/)     

* To my pleasure, someone seems to have the similar idea which I find on the Google Scholar:      
> * \[Herholz 2018\] Sebastian Herholz, Oskar Elek, Jens Schindel, Jaroslav krivanek, Hendrik Lensch. "A Unified Manifold Framework for Efficient BRDF Sampling based on Parametric Mixture Models." EGSR 2018.    
* The traditional Euclidean-Space method may be replaced by the efficient 2-Manifold method in the next few years.    
    
---   

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
    
---   

### Platform Support  
- [ ] Vulkan  
  - [ ] MemoryAllocator  
    - [x] SLOB  
    - [ ] RingBuffer  
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
    - [ ] PosixLinuxX11  
    - [ ] PosixLinuxAndroid  
    - [ ] Win32Desktop  
- [ ] Metal  
  - [ ] MemoryAllocator  
    - [x] SLOB  
    - [ ] RingBuffer  
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
    - [x] SLOB  
    - [ ] RingBuffer  
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

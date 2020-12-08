﻿### Project Description     
PatriotEngine is the source code of the book "Developing A Graphics Rendering Engine With Vulkan/Metal/Direct3D12" which is being written.   

Although there is no official standard of the **Graphics Rendering** Engine at present, the functionality of the **Graphics Rendering** Engine has steadily stabilized with the evolution of the **Graphics Rendering** Engine in the past few decades and thus has formed the de facto standard.   

It's widely establish that "Program = Data Structure + Algorithm" and this book will elaborate the **Graphics Rendering** Engine from two aspects: Scene (Data Structure) and Rendering Pipeline (Algorithm). 

The contents of this book is likely to be as follows:   
>    
> [docs/malloc.md](docs/malloc.md)  
> [docs/malloc.zh_CN.md](docs/malloc.zh_CN.md)  
>        
> ---    
>     
> [docs/rendering_pipeline.md](docs/rendering_pipeline.md)      
> [docs/gfx/RenderingPipeline.zh_CN.md](docs/gfx/RenderingPipeline.zh_CN.md)      
>        
> ---    
>     
> [docs/gfx/SceneTree.zh_CN.md](docs/gfx/SceneTree.zh_CN.md)  
> [docs/gfx/BVH.md](docs/gfx/BVH.md)     
> [docs/gfx/BVH.zh_CN.md](docs/gfx/BVH.zh_CN.md)     
>        
> ---    
>      
> [docs/McRT/Scalable-Allocator.zh_CN.md](docs/McRT/Scalable-Allocator.zh_CN.md)             
> [docs/McRT/Task-Scheduler.zh_CN.md](docs/McRT/Task-Scheduler.zh_CN.md)        
>     
   
### In Progress   
* 1\.To learn the Differential Geometry and try to treat the Steradian as the 2-Manifold which reduces the dimensions from 3(R^3) to 2(S^2) and simplifies the integral over the sphere surface when one calculates the lighting.    
Try to understand the following papers from the perspective of the 2-Manifold：    
> * An Introduction to Manifolds / Example 23.11 (Integral over a sphere)     
\[Tu 2011\] Loring Tu. "An Introduction to Manifolds, Second Edition." Springer 2011.    
> * LTC  
\[Heitz 2016\] [Eric Heitz, Jonathan Dupuy, Stephen Hill and David Neubelt. "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines." SIGGRAPH 2016](https://eheitzresearch.wordpress.com/415-2/)     
      
---     
        
### 项目简介 
本项目作为图书《开发一个图形渲染引擎》[Document/目录.md]的配套源代码发布  
  
虽然**图形渲染**（Graphics Rendering）引擎在目前并没有标准定义，但是，与软件工程中很多其它领域的情况一样——比如LLVM框架已经成为了编译器领域事实上的标准——随着图形渲染技术在几十年间的发展，图形渲染引擎的功能不断趋于稳定，目前正是对图形渲染引擎未成文的事实上的标准进行一次系统性总结的最佳时机。  

我们知道“程序=数据结构+算法”，图形渲染引擎可以认为由**场景**（Scene）和**渲染流程**（Rendering Pipeline)两部分组成。场景即程序中的数据结构部分，描述了我们输入到渲染流程的数据；而渲染流程即程序中的算法部分，对输入的场景进行处理，最终输出图像（Image）的过程（在一些文献中，**图形渲染**（Graphics Rendering）又被称为**图像合成**（Image Synthesis））。本书将会从场景和渲染流程两个方面对图形渲染引擎进行阐述。  

### 已解决：  
* 1\.并行编程框架——[PatriotTBB](https://github.com/YuqiaoZhang/PatriotTBB)  
* 2\.可移植分发——[Bionic-based Linux](https://github.com/YuqiaoZhang/Bionic-based-Linux)  //[Bionic-based RenderDoc](https://github.com/YuqiaoZhang/Bionic-based-RenderDoc)  
* 3\.C++跨平台——[CXX-based-Mac](https://github.com/YuqiaoZhang/CXX-based-Mac/)    

### 正在解决：  
* **1\.学习微分几何(Differential Geometry)，尝试将球面度(Steradian)看作二维流形(2-Manifold)，从而更深刻地理解渲染中相关的公式，并尝试从微分几何的角度理解以下论文： /\*重点目标\*/**    
> * An Introduction to Manifolds / Example 23.11 (Integral over a sphere)     
\[Tu 2011\] Loring Tu. "An Introduction to Manifolds, Second Edition." Springer 2011.    
> * LTC  
\[Heitz 2016\] [Eric Heitz, Jonathan Dupuy, Stephen Hill and David Neubelt. "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines." SIGGRAPH 2016](https://eheitzresearch.wordpress.com/415-2/)     

* 2\.场景管理/渲染框架：对Pixar Hydra、NVIDIA SceniX、NvPro-Pipeline、Sony Alembic的源码进行剖析，并设计符合Vulkan和Direct3D12用法的场景管理模块和渲染框架，尽可能在用法上统一Vulkan和Direct3D12     
参考文献：   
> * Pixar Hydra     
\[Elkoura 2019\] [George Elkoura, Sebastian Grassia, Sunya Boonyatera, Alex Mohr, Pol Jeremias-Vila, Matt Kuruc. "A deep dive into universal scene description and hydra." SIGGRAPH 2019.](http://graphics.pixar.com/usd/files/Siggraph2019_Hydra.pdf)     
> * FrameGraph    
\[O'Donnell 2017\] [Yuriy O'Donnell. "FrameGraph: Extensible Rendering Architecture in Frostbite." GDC 2017.](https://www.ea.com/frostbite/news/framegraph-extensible-rendering-architecture-in-frostbite)       
> * [Intel TBB](https://software.intel.com/content/www/us/en/develop/tools/threading-building-blocks.html)     
[Flow Graph Designer](https://software.intel.com/content/www/us/en/develop/videos/flow-graph-designer-introduction.html)        
     
* 3\.开发适用于Direct3D12/Vulkan/Metal的内存分配器  
参考文献：   
> * [Intel TBB](https://www.threadingbuildingblocks.org/)  
\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.  
\[Kukanov 2007\] Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks." Intel Technology Journal, Volume11, Issue 4 2007.  
> * [Vulkan Memory Allocator](https://gpuopen.com/vulkan-memory-allocator-2-2/)  
> * [D3D12 Memory Allocator](https://gpuopen.com/d3d12-memory-allocator-1-0-0/)  
> * [\[Gelado 2019\] Isaac Gelado, Michael Garland. "Throughput-Oriented GPU Memory Allocation." PPOPP 2019.](https://research.nvidia.com/publication/2019-02_Throughput-oriented-GPU-memory)  

* 4\.材质系统——[PatriotMaterialCompiler](https://github.com/YuqiaoZhang/PatriotMaterialCompiler)  

* 5\.设计一套比较完整的实时全局光照解决方案  
参考文献：  
> * 实时渲染  
[\[Moller 2018\] Tomas Akenine Moller, Eric Haines, Naty Hoffman, Angelo Pesce, Michal Iwanicki, Sebastien Hillaire. "Real-Time Rendering, Fourth Edition." A K Peters 2018.](http://www.realtimerendering.com) / 10 Local Illumination  
[\[Moller 2018\] Tomas Akenine Moller, Eric Haines, Naty Hoffman, Angelo Pesce, Michal Iwanicki, Sebastien Hillaire. "Real-Time Rendering, Fourth Edition." A K Peters 2018.](http://www.realtimerendering.com) / 11 Global Illumination  
> * 光线追踪  
[\[Pharr 2016\] Matt Pharr, Wenzel Jakob, Greg Humphreys. "Physically based rendering: From theory to implementation, Third Edition." Morgan Kaufmann 2016.](http://www.pbr-book.org)  
[\[Haines 2019\] Eric Haines, Tomas Akenine Moller. "Ray Tracing Gems." Apress 2019.](https://research.nvidia.com/publication/2019-03_Ray-Tracing-Gems)  
[Intel Embree](https://www.embree.org/)  
[AMD Radeon Rays](https://gpuopen.com/gaming-product/radeon-rays/)  
> * 辐射度  
\[Cohen 1993\] Michael Cohen, John Wallace. "Radiosity and Realistic Image Synthesis." Morgan Kaufmann 1993.   
[\[Coombe 2005\] Greg Coombe, Mark Harris. "Global Illumination Using Progressive Refinement Radiosity." Chapter 39, GPU Gems 2, 2005.](https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter39.html)   
> * 体素  
[\[Moller 2018\] Tomas Akenine Moller, Eric Haines, Naty Hoffman, Angelo Pesce, Michal Iwanicki, Sebastien Hillaire. "Real-Time Rendering, Fourth Edition." A K Peters 2018.](http://www.realtimerendering.com) / 13.10 Voxels  
[NVIDIA GVDB Voxels](https://developer.nvidia.com/gvdb-samples)  
[OpenVDB](https://www.openvdb.org/)  
> * VXGI  
[\[Crassin 2011\] Cyril Crassin, Fabrice Neyret, Miguel Sainz, Simon Green, Elmar Eisemann. "Interactive Indirect Illumination Using Voxel Cone Tracing." SIGGRAPH 2011](https://research.nvidia.com/publication/interactive-indirect-illumination-using-voxel-cone-tracing)  
[NVIDIA VXGI](https://developer.nvidia.com/vxgi)  
> * 材质  
[SONY OSL](https://github.com/imageworks/OpenShadingLanguage/)  
[NVIDIA MDL](https://developer.nvidia.com/mdl-sdk)    
  
### 待解决：  
* 1\.其它渲染相关的需求在[Document/目录.md]中列出    
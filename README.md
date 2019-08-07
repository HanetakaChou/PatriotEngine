本项目作为图书《图形引擎开发》[https://www.epubit.com/selfpublish/article/1069](https://www.epubit.com/selfpublish/article/1069 "https://www.epubit.com/selfpublish/article/1069")的配套源代码发布

相对于传统的图形引擎而言，PatriotEngine具有以下优势：  
1.基于结构化并行编程  
2.基于Vulkan/Direct3D12 后续可能考虑支持Metal  
3.基于COM设计接口——这意味着，任何第三方厂商基于PatriotEngine的接口开发的上层软件（包括但不限于编辑器）都可以在不需要重新编译的情况下和任意版本的PatriotEngine兼容——同时PatriotEngine的LGPL授权模式对第三方厂商为PatriotEngine开发闭源的上层软件（包括但不限于编辑器）并从中获益是非常友好的    

已解决：  
1.并行编程框架——PatriotTBB  

待解决：  
1.场景管理：对NVIDIA的场景管理引擎SceniX和NvPro-Pipeline的源码进行剖析，并设计符合Vulkan和Direct3D12用法的场景管理模块  
//设计符合Vulkan和Direct3D12用法的渲染框架，尽可能在用法上统一Vulkan和Direct3D12  
2.函数式编程：由于C++支持Lambda表达式，拟深入研究Haskell，并基于函数式编程的方式设计对并行编程友好的接口（该需求暂停）  

正在解决：  
1.开发适用于Direct3D12/Vulkan的内存分配器  

参考文献：  
1.Intel TBB  
[Hudson 2006] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.  
[Kukanov 2007] Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks." Intel Technology Journal, Volume11, Issue 4 2007.  
https://www.threadingbuildingblocks.org/  
  
2.Vulkan Memory Allocator  
https://gpuopen.com/vulkan-memory-allocator-2-2/  
  
3.[Gelado 2019] Isaac Gelado, Michael Garland. "Throughput-Oriented GPU Memory Allocation." PPOPP 2019.  
https://research.nvidia.com/publication/2019-02_Throughput-oriented-GPU-memory  




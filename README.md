本项目作为图书《图形引擎开发》[Document/目录.md]的配套源代码发布  
  
虽然图形渲染（Graphics Rendering）引擎在目前并没有标准定义，但是，与软件工程中很多其它领域的情况一样——比如LLVM框架已经成为了编译器领域事实上的标准——随着图形渲染技术在几十年间的发展，图形渲染引擎的功能不断趋于稳定，目前正是对图形渲染引擎未成文的事实上的标准进行一次系统性的总结的最佳时机。  
我们知道“程序=数据结构+算法”，图形渲染引擎可以认为由场景（Scene）和渲染流程（Rendering Pipeline)两部分组成。场景即程序中的数据结构部分，描述了我们输入到渲染流程的数据；而渲染流程即程序中的算法部分，对输入的场景进行处理，最终输出图像（Image）的过程，因此，在一些文献中，图形渲染（Graphics Rendering）又被叫做图像合成（Image Synthesis）。  

已解决：  
1.并行编程框架——PatriotTBB  

待解决：  
1.场景管理：对NVIDIA的场景管理引擎SceniX和NvPro-Pipeline的源码进行剖析，并设计符合Vulkan和Direct3D12用法的场景管理模块 //设计符合Vulkan和Direct3D12用法的渲染框架，尽可能在用法上统一Vulkan和Direct3D12   
2.其它图形相关的需求在[Document/目录.md]中列出  
  
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




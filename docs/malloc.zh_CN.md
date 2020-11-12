## License  
```  
Copyright (C) YuqiaoZhang

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>
```  

### Slab分配器（Slab Allocator）  
      
根据 \[Bonwick 1994\] / 3. Slab Allocator Implementation 中的说法，Slab的命名源于SunOS中的源代码，并无特别的含义。 // 注：起初，在编写SunOS的源代码时，仅仅是出于“Slab”相对于“Object”或“Cache”而言更具有辨识度，才作此命名。   
   
根据 \[Bonwick 1994\] / 4.4. Arena Management 中的说法，分配器（Allocator）对区域（Arena）的管理策略可以大致分为三类：顺序适配（Sequential-Fit）、伙伴（Buddy）和**分离存储（Segregated-Storage）**。 // 注：根据 Wikipedia / Region-based memory management 中的说明，此处的Arena与Region同义，译为“区域”再合适不过。  

显然，Slab分配器属于分离存储。在 \[Bonwick 1994\] / 3. Slab Allocator Implementation 和 \[Bonwick 1994\] / 4. Hardware Cache Effects 中对Slab分配器进行了详尽的介绍。接下来，本文也打算对Slab分配器进行介绍。为了方便读者参阅国际上的文献资料，同时为了提高辨识度，本文保留了表示Slab分配器中的数据结构的英文术语Cache、Slab和Buffer，而不译作中文。

![](./malloc_slab_allocator.svg)  

所有Slab的大小都相同，被设定为一个页（Page）的大小。在POSIX系统上，一个页的大小可以用sysconf(_SC_PAGESIZE)查询。Slab所占用的内存从后端（Backend）分配器中分配。// 注：所谓的后端分配器是一个比Slab分配器粒度更粗的分配器，所谓的粒度更粗是指所允许的内存请求的最小值更大。比如，直接向操作系统申请，此时，所允许的内存请求的最小值为一页（在x86上为4096b）大小，即在每次内存请求时至少分配一页内存。在POSIX系统上，可以用mmap/munmap直接向操作系统申请。        

一个Slab中含有若干个Buffer，同一Slab中的Buffer的大小都相同，Slab的控制块（Slab Data）被置于Slab的结束位置。 // 注：根据 \[Bonwick 1994\] / 3.2.2. Slab Layout for Small Objects 中的说法，经验表明，当应用程序尝试非法地修改已经被释放的内存时，Slab开始位置的数据被修改的可能性更高，将Slab的控制块置于结束位置，可以减少Slab的控制块被错误修改的可能性，更有利于调试。然而，经验性的法则多不可靠，读者大可不必理会。当下主流的TBB-Malloc（\[Kukanov 2007\]）就将Slab的控制块置于开始位置。    

一个Cache中含有若干个Slab，同一Cache中的Slab中的Buffer的大小都相同。同一Cache中的Slab的控制块构成双向链表（Doubly-Linked List），在Cache中存放着一个表头指针（Freelist Pointer），指向双向链表中某一个Slab，并确保该Slab及其之后（Next）的Slab都至少含有一个空闲的Buffer。    

当Buffer被释放时，可以检测Buffer所在的Slab中未被释放的Buffer的个数，当Slab中的所有Buffer都被释放时，可以将Slab插入到双向链表的尾部，从而即可确保Cache中的表头指针指向的Slab及其之后的Slab都至少含有一个空闲的Buffer。 // 注：当Buffer被释放时，只要将Buffer的地址对齐到页大小，即可定位到Buffer所在的Slab的地址。检测Buffer所在的Slab中未被释放的Buffer的个数只是一个平凡的操作，只需在Slab的控制块中维护一个变量计数即可。         

根据在上文中的说明，Slab所占用的内存从后端分配器中分配。既然如此，那么当Slab中的所有Buffer都被释放时，理应将Slab送往后端分配器回收。但是，根据 \[Bonwick 1994\] / 3.4. Reclaiming Memory 中的说法，当某个Slab中的所有Buffer都被释放时，如果Cache中Slab的个数低于一定阈值（被称为工作集（Working-Set）大小），那么该Slab就不会立即被后端分配器回收，而是插入到Cache的双向链表中。显然，这种做法可以避免从后端分配器中分配和初始化Slab的开销。

所谓的分离存储是指，Buffer大小的取值只可能是事先设定的若干种（比如：8b、16b、32b、48b ...），并且维护了个数与Buffer大小可能取值的个数相同的Slab双向链表。每个Slab双向链表对应于一个Cache，同时也对应于Buffer大小的某一个取值。也就是说，同一Cache中的Buffer大小一定相同，不同大小的Buffer被**分离存储**在不同的Cache中。     

在分配内存时，对于不大于半页（即半个Slab的大小）的内存请求，可以匹配到不小于该内存请求的最小的Buffer大小取值。显然，该匹配过程是一个平凡的操作。根据匹配到的Buffer大小的取值，即可索引到对应的Cache，找到Cache中含有空闲Buffer的Slab，既可完成分配。对于大于半页的内存请求（即Large Object），直接从后端分配器中分配。 // 注：关于后端分配器的含义，在上文中已经说明。此处，可以理解为直接向操作系统申请。   

根据 \[Bonwick 1994\] / 3.2.3. Slab Layout for Large Objects 中的说法，在释放内存时，Slab分配器需要借助哈希表来区分被释放的地址是Buffer还是Large Object，以便采取不同的方式释放。      

显然，这种借助哈希表的解决方案过于低效，让人很难接受。实际上，我们可以将Slab的控制块置于开始位置，从而确保了Slab中的Buffer一定不会对齐到页大小。然而，由于Large Object的内存直接向系统申请，Large Object一定对齐到页大小。因此，在释放内存时，我们可以根据被释放的地址是否对齐对页大小来区分Buffer和Large Object，避免了借助哈希表进行映射的过程，这也是TBB-Malloc（\[Kukanov 2007\]）的做法。

### McRT-Malloc  

显然，McRT-Malloc也属于分离存储，McRT-Malloc中的各数据结构与Slab分配器之间的关系如下：     

McRT-Malloc | Slab Allocator   
:-: | :-:   
Bin | Cache  
Block | Slab  
Object | Buffer  

1\.\[Bonwick 1994\] [Jeff Bonwick. "The Slab Allocator: An Object-Caching Kernel Memory Allocator." USENIX 1994](https://www.usenix.org/legacy/publications/library/proceedings/bos94/bonwick.html)  

2\.\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.   

3\.\[Kukanov 2007\] Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks." Intel Technology Journal, Volume11, Issue 4 2007.  

\[Hebert 2016\] [Chris Hebert. "Vulkan Memory Management" NVIDIA GameWorks Blog 2016](https://developer.nvidia.com/vulkan-memory-management)

\[Schott 2016\] [Mathias Schott. "What’s your Vulkan Memory Type?" NVIDIA GameWorks Blog 2016](https://developer.nvidia.com/what%E2%80%99s-your-vulkan-memory-type)

[nvvk](https://github.com/nvpro-samples/shared_sources/blob/master/nvvk/README.md)   

[vk_async_resources](https://github.com/nvpro-samples/vk_async_resources)

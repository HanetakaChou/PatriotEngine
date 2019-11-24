PatriotTBB is part of PatriotEngine for addressing the requirements for parallel programming in the next-generation engine and also released as an independent project, allowing end users to use independently.  

PatriotTBB consists of Scalable Allocator and Task Scheduler.  

1.Scalable Allocator  
The malloc and free in the traditional C runtime library are mutually exclusive when concurrently accessed, resulting in serialization, and ScalableMemoryAllocator is a memory allocator that can be concurrently accessed efficiently.  

2.Task Scheduler  
For the case where there is only one MasterThread, it has been tested in the Sample that creates a binary tree of 10 million nodes in parallel and sums it; it is preparing to test the situation where multiple MasterThreads exist simultaneously.  

Based on Task Scheduler, implement Map and Reduce parallel pattern with recursion, which fully supports Nested-Parallelism.  
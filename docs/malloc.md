

The name derives from one of the allocator’s main data structures, the slab. The name stuck within Sun because it was more distinctive than “object" or "cache". (\[Bonwick 1994\])     

Bucket Allocator  
A slab consists of one or more pages of virtually contiguous memory carved up into **equal-size** chunks, with a reference count indicating how many of those chunks have been allocated. (\[Bonwick 1994\])

\[Bonwick 1994\] [Jeff Bonwick. "The Slab Allocator: An Object-Caching Kernel Memory Allocator." USENIX 1994](https://www.usenix.org/legacy/publications/library/proceedings/bos94/bonwick.html)  
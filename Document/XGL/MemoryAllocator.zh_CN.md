McRT 由 Slab 发展而来   
   
McRT   \-\-\- Slab       
Bin    \-\-\- Cache       
Block  \-\-\- Slab        
Object \-\-\- Buffer      

\[Bonwick 1994\] / 3. Slab Allocator Implementation  
\[Hudson 2006\] / 3. McRT-MALLOC 



关于 Arena   

Region-based memory management - Wikipedia 

CppCon2017 Local ('Arena') Memory Allocator  

Global Allocator  
Local Allocator  


根据 \[Bonwick 1994\] / 4.4. Arena Management  

Sequential-Fit      
Buddy   
Segregated-storage  

Slab Allocator  

The name derives from one of the allocator’s main data structures, the slab. The name stuck within Sun because it was more distinctive than “object" or "cache". (\[Bonwick 1994\])     
    
A slab consists of one or more pages of virtually contiguous memory carved up into **equal-size** chunks, with a reference count indicating how many of those chunks have been allocated. (\[Bonwick 1994\])

\[Bonwick 1994\] [Jeff Bonwick. "The Slab Allocator: An Object-Caching Kernel Memory Allocator." USENIX 1994](https://www.usenix.org/legacy/publications/library/proceedings/bos94/bonwick.html)  

\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.
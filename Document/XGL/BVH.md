## BVH（包围体层次，Bounding Volume Hierarchy）   
  
直接Transform Bounding Box是可行的  
但是，如果计算Bounding Box的开销不大，可以先变换物体到World Space，再计算Bounding Box
  
在Ray Tracing中  
图元分割->BVH；空间分割->kdTree //2.\[Pharr 2016\]/4.2 Aggregates  
图元分割：每个图元只在层次性结构（Hierachy）中出现一次；空间分割：同一个图元可能与多个空间区域重叠 //2.\[Pharr 2016\]/4.3 Bounding Volume Hierarchies       

图元分割   
BVH //二叉树    
//Construct BVH //可以基于Fork-Join模式并行化（3.\[McCool 2012\]/8.9 QuickSort）  
//Flatten BVH Tree //2.\[Pharr 2016\]/4.3.4 Compact BVH For Traversal  

在Construct BVH中，分割（Split)的目标是使产生的两个子树的重叠（Overlap）尽可能地小 //从而降低在遍历时需要同时遍历两个子树的可能性 //The general goal in partitioning here is to select a partition of primitives that doesn’t have too much overlap of the bounding boxes of the two resulting primitive sets—if there is substantial overlap then it will more frequently be necessary to traverse both children subtrees when traversing the tree, requiring more computation than if it had been possible to more effectively prune away collections of primitives. //2.\[Pharr 2016\]/4.3.1 BVH Construction  

SAH(Surface Area Heuristic) //2.\[Pharr 2016\]/4.3.2 The Surface Area Heuristic    
  
## 视锥体剔除（Frustum Culling）   
  
## 光线投射（Ray Casting）  
  
在交互式应用中 用于实现 pick up object //UE4/Unity/CryEngine的例子  
  
在光线追踪（Ray Tracing）应用中 用于粗粒度检测  
  
### Ray AABB Intersection  
//（2.\[Pharr 2016\]/3.1.2 Ray–Bounds Intersections| 2.\[Pharr 2016\]/3.9.2 Conservative Ray–Bounds Intersections）  
  
## 参考文献  
[1.\[Moller 2018\] Tomas Akenine Moller, Eric Haines, Naty Hoffman, Angelo Pesce, Michal Iwanicki, Sebastien Hillaire. "Real-Time Rendering, Fourth Edition." A K Peters 2018.](http://www.realtimerendering.com)  
[2.\[Pharr 2016\] Matt Pharr, Wenzel Jakob, Greg Humphreys. "Physically based rendering: From theory to implementation, Third Edition." Morgan Kaufmann 2016.](http://www.pbr-book.org)  
[3.\[McCool 2012\] Michael McCool, James Reinders, Arch Robison. "Structured Parallel Programming: Patterns for Efficient Computation." Morgan Kaufmann 2012.](http://parallelbook.com/)   
\[Arvo 1990\] James Arvo. "Transforming Axis-Aligned Bounding Boxes." Graphics Gems X.8 1990.  
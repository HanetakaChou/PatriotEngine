## BVH（包围体层次，Bounding Volume Hierarchy）   
  
直接Transform Bounding Box是可行的  
但是，如果计算Bounding Box的开销不大，可以先变换物体到World Space，再计算Bounding Box
  
在Ray Tracing中  
图元分割->BVH；空间分割->kdTree //2\.\[Pharr 2016\]/4.2 Aggregates  
图元分割：每个图元只在层次性结构（Hierachy）中出现一次；空间分割：同一个图元可能与多个空间区域重叠 //2\.\[Pharr 2016\]/4.3 Bounding Volume Hierarchies       

图元分割   
BVH //二叉树  

//Construct BVH 

1\.SAH(Surface Area Heuristic) 

基于递归的方式不断分割（Split）/\*可以基于Fork-Join模式并行化（3\.\[McCool 2012\]/8.9 QuickSort）\*/  
分割的目标是使产生的两个子树的重叠（Overlap）尽可能地小 //从而降低在遍历时需要同时遍历两个子树的可能性 //2\.\[Pharr 2016\]/4.3.1 BVH Construction  

基于表面积评估概率计算开销 ~~/\*Middle和EqualCount的SpiltMethod可以认为是SAH的简化版，仅用于教学演示目的\*/~~ //2\.\[Pharr 2016\]/4.3.2 The Surface Area Heuristic    

//CPU原生支持Fork-Join模式，而GPU原生支持Map模式  
//越接近原生支持的模式，效率越高  
//SAH不适合GPU并行化，但是对CPU并行化非常友好  

//---------------------------------------  
2\.HLBVH  

LBVH（Linear BVH）    
使用莫顿码（Morton Codes）将高维数据降到一维 将BVH构造转换为排序问题 //2\.\[Pharr 2016\]/4.3.3 Linear Bounding Volume Hierarchies       
在设计GPU的纹理缓存时，会用到莫顿序列（Morton Sequence）以提高相干性 //有着异曲同工之妙 1\.\[Moller 2018\]/23.8 Texturing  

莫顿码的含义 每一位将空间分割 //从这个意义上，LBVH更倾向于空间分割而非图元分割  
  
HLBVH（Hierarchical Linear BVH）  
1\.基于LBVH构造若干个小树（Treelet）  

1-1\.基于包围体的中心得到莫顿码 //最主要缺点  
1-2\.基于莫顿码高位 将“空间”分割成 大小相同的网格（Grid）——即小树（Treelet） //一些网格可能是空的，不会有小树  
1-3\.每个小树的构造 可以基于Map模式并行化 //构造方法是基于莫顿码低位不断二分  
  
2\.将小树视为Primitive，基于SAH构造BVH  

LBVH缺点 构造过程只依赖于包围体的中心（/\*Compute Morton indices of primitives\*/） 并不考虑包围体的范围    
  
   
**Flatten BVH Tree** //2\.\[Pharr 2016\]/4.3.4 Compact BVH For Traversal  
//个人认为游戏引擎非常值得借鉴 //将内存布局线性化    
   
//Traversal BVH   
  
  
## 视锥体剔除（Frustum Culling）   
  
## 光线投射（Ray Casting）  
  
在交互式应用中 用于实现 pick up object //UE4/Unity/CryEngine的例子  
  
在光线追踪（Ray Tracing）应用中 用于粗粒度检测  
  
### Ray AABB Intersection  
//（2\.\[Pharr 2016\]/3.1.2 Ray–Bounds Intersections| 2\.\[Pharr 2016\]/3.9.2 Conservative Ray–Bounds Intersections）  
  
## 参考文献  
[1\.\[Moller 2018\] Tomas Akenine Moller, Eric Haines, Naty Hoffman, Angelo Pesce, Michal Iwanicki, Sebastien Hillaire. "Real-Time Rendering, Fourth Edition." A K Peters 2018.](http://www.realtimerendering.com)  
[2\.\[Pharr 2016\] Matt Pharr, Wenzel Jakob, Greg Humphreys. "Physically based rendering: From theory to implementation, Third Edition." Morgan Kaufmann 2016.](http://www.pbr-book.org)  
[3\.\[McCool 2012\] Michael McCool, James Reinders, Arch Robison. "Structured Parallel Programming: Patterns for Efficient Computation." Morgan Kaufmann 2012.](http://parallelbook.com/)   
\[Arvo 1990\] James Arvo. "Transforming Axis-Aligned Bounding Boxes." Graphics Gems X.8 1990.  
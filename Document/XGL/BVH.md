## BVH（包围体层次，Bounding Volume Hierarchy）   
  
直接Transform Bounding Box是可行的  
但是，如果计算Bounding Box的开销不大，可以先变换物体到World Space，再计算Bounding Box
  
  
## 视锥体剔除（Frustum Culling）   
  
## 光线投射（Ray Casting）  
  
在交互式应用中 用于实现 pick up object //UE4/Unity/CryEngine的例子  
  
在光线追踪（Ray Tracing）应用中 用于粗粒度检测
  
### Ray AABB Intersection  
//（2.\[Pharr 2016\]/3.1.2 Ray–Bounds Intersections| 2.\[Pharr 2016\]/3.9.2 Conservative Ray–Bounds Intersections）  
  
## 参考文献  
[1.\[Moller 2018\] Tomas Akenine Moller, Eric Haines, Naty Hoffman, Angelo Pesce, Michal Iwanicki, Sebastien Hillaire. "Real Time Rendering." CRC Press 2018.](http://www.realtimerendering.com)  
[2.\[Pharr 2016\] Matt Pharr, Wenzel Jakob, Greg Humphreys. "Physically based rendering: From theory to implementation." Morgan Kaufmann 2016.](http://www.pbr-book.org)  
\[Arvo 1990\] James Arvo. "Transforming Axis-Aligned Bounding Boxes." Graphics Gems X.8 1990.  
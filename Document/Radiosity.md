### M(Radiant Existence, 辐射出射度)/B(Radiosity, 辐射度) 

#### 颜色溢出（Color Bleeding）  
Diffuse Inter-reflection/Color Bleeding: Diffuse inter-reflection refers to indirect
light reflected off of diffuse surfaces onto nearby surfaces. In particular, if the diffuse
surfaces have strong coloration, this leads to color bleeding, wherein the color of one
surface appears to leak, or bleed, onto nearby surfaces. The Cornell box, originally
designed by early global illumination researchers at Cornell University, is a classic testing
environment for global illumination algorithms.（[Ramamoorthi 2009] / Lecture 3 "Global Illumination and the Rendering Equation" / 1 "Introduction to Global Illumination"、[Christensen 2008]）  
  


//$M_d = \int_{\Omega}{L_r \lparen\overrightarrow{\omega_i}\rparen \cos \theta_i d\omega}$  
  
//渲染方程（Rendering Equation）  
  
//w_i指入射光的相反方向  
//x指入射点的位置  
//x’指发出入射光的位置  
  
渲染方程是第二类弗雷德霍姆积分方程（Fredholm Integral
Equation），可以利用刘维尔-诺伊曼级数（Liouville–Neumann series
）求解（[Ramamoorthi 2009], Scribe of Lecture 3 "Global Illumination and the Rendering Equation", 3 "Deriving the Rendering Equation"）  
  
但在Radiosity中，我们使用有限元法求解  
  
有限元法仅用于离散化渲染方程

//误差估计  
//余量仅代表[全局]的误差，余量大不代表[局部]误差大 
  
//影响误差的特征
//积分Kernel //Form Factor在某些情况下局部变化较大 //相邻物体 //阴影边缘 //半影 
//解决方法是进一步分割

//Radiosity定义域 //表面 //几何体 //场景  
//在ImageSynthesis中 //可以考虑 基于Image的Pixel为定义域  进行误差度量  //An importancedriven
radiosity algorithm. SIGGRAPH 1992  
  
基于Perceptually //涉及色度学理论  


Mesh的各个特征对精确度的影响 //FEM中的Mesh含义与目前主流的含义不同

//Reference Image //以各个Pixel作为Element  
  
//计算开销在Radiosity定义域上是均匀的 //但是对最终Image的贡献是不均匀的  
  
//Mesh密度 //即Element的（相对）大小/数量 //理想状态下 不均匀分割 在误差较大的地方细分  
  
//元的阶 //连续性  
  
//元的形状  
  
//元的纵横比（Aspect Ratio） //内切和外切园的比 //最大为1  
  
//糟糕的纵横比可能会违背一些 计算Form Factor时所依赖的假定（比如 元被近似成Disk）  
  
//网格分级（Mesh Grading)  密度不同处平滑过渡    

//网格一致性（Mesh Conformance）  //边界连续性   //避免T-Vertices  //常量元往往无需考虑 //常量元往往用于求解Radiosity（即权重函数） 而高阶元往往用于渲染（即基函数） //查找之前的文献，权重函数并不一定是基函数    

//不连续性（Discontinuities）  //由V(x,x')可见性引入    
  
//值不连续性（Value Discontinuities)  //表面交界处 //不连续的值用同一个元表示  //Shadow Leak //Light Leak  //    

//导数不连续性（Derivative Discontinuities） //半影 或 本影边界

//Continuity at Geometric Boundaries  
往往并不会出现在不同几何体的表面交界处，因为不同几何体是 “Mesh化” Separately      
但用于表示曲面的Facet的边界可能会出现问题 应当作为一个整体网格化  
  

  
//光源的表示：用Albedo为0的Element来表示光源  //OSL(Open Shading Language)的思想，不区分光源和表面  
  
Form Factor  
  
Radiance  
$\int_{S'} L_r{\lparen x',-\overrightarrow{\omega_i} \rparen} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA'$

Radiosity/Radiant Existance  
$\int_{S'} B{\lparen x'\rparen} \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA'$

Weighted Residual  
$\int_S \, {\operatorname{N_i}\lparen x\rparen} \, {\lparen \int_{S'} {\lparen \sum_{j=1}^{n} B_j \operatorname{N_j}\lparen x'\rparen \rparen} \, \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA' \rparen} \, dA$  
=$\sum_{j=1}^{n} B_j \, \lparen \int_S \, {\operatorname{N_i}\lparen x\rparen} \, {\lparen \int_{S'} {\operatorname{N_j}\lparen x'\rparen} \, \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA' \rparen} \, dA \rparen$  
=$\sum_{j=1}^{n} B_j \, \lparen \int_{A_i} {\lparen \int_{A_j} \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA' \rparen} \, dA \rparen$  //常量基函数  

Projected Solid Angle  
$d{\omega}^{\perp}=\cos\theta \, d{\omega}$  //[Cohen 1993] 2.4.3、[Pharr 2017] 5.5.1  

Integrals over Area  
$d{\omega}={\frac{\cos\theta_o}{{\vert p'-p\vert}^2} \, dA}$  //[Cohen 1993] 2.4.2、[Pharr 2017] 5.5.3 


半球采样算法（Hemisphere Sampling Algorithm）  
一次计算一行（即A_i相同）  

Nusselt Analog  //另一个半球则为0  
${\int_{A} \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA'}$  
=${\frac {\int_{A} V{\lparen x'\rarr x \rparen} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} \, dA'}{\pi}}$  
=${\frac {\int_{A} V{\lparen x'\rarr x \rparen} \cos\theta_i \, d{\omega}}{\pi}}$ //Integrals over Area  
=${\frac {\int_{A} V{\lparen x'\rarr x \rparen} \, d{{\omega}'}^{\perp}}{\pi}}$ //Projected Solid Angle    

HemiCube（[Cohen 1985]、[Cohen 1993] 4.9.3）

//scan conversion //即光栅化  
//Z-buffer //即深度测试  

将所有A_j投影到以A_i为中心的HemiCube //类似于渲染到CubeMap的方式 //也可以参考抛物线映射，直接投影到半球  
每个离散的表面（CubeMap中的每个像素）记录最近的A_j（可以通过深度测试实现），从而得到V(x'→x)  //Z-Buffer是高效的，同时也会产生走样（Alias）

//每个像素大小必须相同，并不高效 //从重要抽样法的角度，应当按照DeltaFormFactor相同的原则划分像素 //直接投影到半球可以解决这个问题
    
DeltaFormFactor（//即“每个离散的表面”）预计算并存放在LookUpTable中 //定积分对区间的可加性

计算full area-hemisphere //选择Hemi-Cube

Monte-Carlo积分//本质上可能并不是Ray-Tracing，而是用Monte-Carlo方法求解Nusselt Analog   //（[Malley 1988]、[Cohen 1993] 4.9.5）  
重要抽样法 //在底上的投影是均匀的，自变量为投影立体角时，以均匀分布作为概率密度函数  
  
------------------------------------------------------      
  
面采样算法（Area Sampling Algorithm）  
  
Analytic Point-To-Disk Form Factor  
$\int_{\Delta A_j}{\frac{\cos\theta_o}{{\vert p'-p\vert}^2} \, dA}={\frac{\cos\theta_o}{\pi {\vert x'-x\vert}^2+\Delta A_j} \Delta A_j}$  //（[Cohen 1993] Chapter 4.6 Figure 4.6）  
http://www.thermalradiation.net/sectionb/B-12.html   
http://heattransfer.asmedigitalcollection.asme.org/article.aspx?articleid=1434825  
http://heattransfer.asmedigitalcollection.asme.org/article.aspx?articleid=1435395  
  
$\cos\theta_i{\lparen \frac{\cos\theta_o}{\pi {\vert x'-x\vert}^2+\Delta A_j} \Delta A_j \rparen}$  //微分变成$\Delta A_j$后，适用该公式 //对区间的可加性 A_j分割后 蒙特卡洛方法
  
------------------------------------------------------      
求解线性方程组

$F_{ii}=0$ //因为$\Delta_i$和$\Delta_j$背对，即$\cos\theta_i$=0  
K=M-PF  //$K_{ii}=1-\rho F_{ii}=1$ 

//雅可比迭代 和 高斯-赛德尔迭代 不需要详细介绍
//重点介绍 Southwell迭代 和 渐进细分 //优势：在最开始，E较大往往余量较大，因此往往选中Light Source，收敛较快

雅可比（Jacobi）迭代（[Khoo 2003], Lecture 6 "Solution Methods: Iterative Techniques"）  
  
Perhaps the simplest iterative scheme is to update each element Bi(k) of the solution vector by solving for that variable using the current guess Bi
(k).  
余量$r_i = E_i - \sum_{j=1}^{n}K_{ij}B_j$  
$r_i^{\lparen k+1\rparen} + K_{ii}B_i^{\lparen k+1\rparen} = r_i^{\lparen k\rparen} + K_{ii}B_i^{\lparen k\rparen}$  
//由于$r_i^{\lparen k+1\rparen}$为0 //$K_{ii}B_i^{\lparen k+1\rparen} = r_i^{\lparen k\rparen} + K_{ii}B_i^{\lparen k\rparen}$  
//$B_i^{\lparen k+1\rparen} = \frac{B_i^{\lparen k\rparen} + r_i^{\lparen k\rparen}}{K_{ii}}$  //其中$K_{ii}=1-\rho F_{ii}=1$  //是否收敛与谱半径有关

高斯-赛德尔（Gauss-Seidel）迭代  
  
$B_i^{k+1} = \frac{E_i - \sum_{j=1}^{i-1}K_{ij}B_j^{k+1} - \sum_{j=i+1}^{n}K_{ij}B_j^k}{k_{ii}}$  //收敛与对角优势有关
  
在计算$B_i^{k+1}$时，使用了在本次迭代（Iteration）中的先前步骤（Step）中计算得到的$B_1^{k+1}$,$B_2^{k+1}$,$B_3^{k+1}$,...,$B_{i-1}^{k+1}$  
  
//norm（范数）  
  
Southwell 迭代  
不再有Iteration，只有Step  

在每个Step中  
  
选取余量$r_i = E_i - \sum_{j=1}^{n}K_{ij}B_j$ 最大的行$r_i=\operatorname{Max}\lparen r\rparen$  
由于$r_i^{\lparen p+1\rparen} + K_{ii}B^{\lparen p+1\rparen} = r_i^{\lparen p\rparen} + K_{ii}B^{\lparen p\rparen}$且$r_i^{\lparen p+i1\rparen}$为0  //即Relaxation Method  //还有Overrelaxation Method  
计算$B_i^{\lparen p+1\rparen} = \frac{B_i^{\lparen p\rparen} + r_i^{\lparen p\rparen}}{K_{ii}}$  //其中$K_{ii}=1-\rho F_{ii}=1$
  
目前余量$r_i=0$，重新计算“其余”的各个余量 //即$j \ne i$  
由于$r_j^{\lparen p+1\rparen} + K_{ji}B_i^{\lparen p+1\rparen} = \sum_{j \ne i} K_{ji}B_j^{\lparen p+1\rparen} = \sum_{j \ne i} K_{ji}B_j^{\lparen p\rparen} = r_j^{\lparen p\rparen} + K_{ji}B_i^{\lparen p\rparen}$  //只有$B_i$发生改变，其余$B_j$并没有发生改变   
$r_j^{\lparen p+1\rparen} = r_j^{\lparen p\rparen} + K_{ji}B_i^{\lparen p\rparen} - K_{ji}B_i^{\lparen p+1\rparen} = r_j^{\lparen p\rparen} - K_{ji}{\lparen B_i^{\lparen p+1\rparen} - B_i^{\lparen p\rparen} \rparen} = r_j^{\lparen p\rparen} - K_{ji}r_i^{\lparen p\rparen} = r_j^{\lparen p\rparen} + \rho_jF_{ji}r_i^{\lparen p\rparen} = r_j^{\lparen p\rparen} + \rho_jF_{ij}r_i^{\lparen p\rparen}$ //根据互反律$F_{ij} = F_{ji}$

由于$\rho_j$小于1 且 所有(j!=i)的$F_{ij}$的和小于1（可以从HemiCube角度理解），所有余量的和在减小（//单调递减）  
  
初始化 B=0 r=E-KB=E

物理解释  
余量的 //Unshot radiosity  
//received as illumination by the elements //Ei或Bj  
//but has not yet been “reflected” or shot back out to contribute further to the illumination of the environment. //Bi  
//r = Ei - (Bi - \sumBj/\*Indirect\*/) //B_i代表shot //从当前元发出reflect/shot //从而会被其它元接收contribute further //即Step中重新计算“其余”各个余量的过程（第二部分）  
//选取最大的余量计算Bi，实际上即其它元shot的radiosity（Ei或Bj）被当前元（Bi）//即Step中receive的过程（第一部分） 
  
//每个Step中receive后，立即reflect  
  
再次进行下一个Step  
  
## Progressive Refinement //渐进细化    
 （[Cohen 1993] 5.3.3、[Coombe 2005]） 
   
//The requirement is thus not only to provide an algorithm that converges quickly, but one that makes as much progress at the beginning of the algorithm as possible.  
  
不区分光源和表面 //用$\rho$=0表示光源 //OSL(Open Shading Language)中的定义  
  
//与 Southwell迭代 等价  
  
B即Bi+r //Received Total  
//Bi含义Received And Shot  
/deltaB即r //Received Not Shot  
  
//初始化/deltaBi为E 即 r=E  
  
每次Step  
选取/deltaB*Ai(常量基函数Ai为1)最大的行  //在最开始的时候，E较大往往余量较大，因此往往选中Light Source，收敛较快
Bi=Bi+r r=0 //但Bi+r不变  //Not shot-> Shot
其余各行的余量r改变//即Receive B Bi+r 和 /deltaB r 都改变   //但是随着迭代的进行，Unshot的能量被均匀地传播，收敛速度逐渐变慢
  
Overrelexation可以被认为是OverShooting  //或super-shoot-gather  
  
动态环境
和PRT有一定联系 //预计算传输  
  
光源改变  
KB=E中的E发生改变 
在光源变换不大的情况下，可以将之前的解作为迭代法（Iterative Method的初始值（Starting Guess）

也可以利用线性的特性  
比如  
     [E_1] //会改变    
     [E_2] //会改变   
     [E_3] //不变  
     [E_4] //不变  

预先解出  
[1]  
[0]  
[0]  
[0]  
_______
[0]  
[1]  
[0]  
[0]  
________  
[0]  
[0]  
[E_3]  
[E_4]  
________   
对于任意的  
[E_1]  
[E_2]  
[E_3]  
[E_4]  
__________
可以表示成
E_1[1 0 0 0] + E_2[0 1 0 0] + [0 0 E_3 E_4]  
对应的解即为之前解出的解的线性组合  

Albedo改变  
不影响Form Factor，但需要重新计算K  
可以将之前的解作为迭代法（Iterative Method的初始值（Starting Guess）  
  
//几何改变//并行  

  
## 参考文献  
[Willcox 2014] Karen Willcox, Qiqi Wang. "Computational Methods in Aerospace Engineering." MITOpenCourseWare 2014.  
[https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-90-computational-methods-in-aerospace-engineering-spring-2014/](https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-90-computational-methods-in-aerospace-engineering-spring-2014/)  
[Cohen 1993] Michael F. Cohen, John R. Wallace. "Radiosity and Realistic Image Synthesis." Academic Press Professional 1993.  
[https://dl.acm.org/citation.cfm?id=154731](https://dl.acm.org/citation.cfm?id=154731)  
[Ramamoorthi 2009] Ravi Ramamoorthi, James O'Brien. "Advanced Computer Graphics." University Of California at Berkeley, CS 294-13, Fall 2009.  
[http://inst.eecs.berkeley.edu/~cs294-13/fa09/](http://inst.eecs.berkeley.edu/~cs294-13/fa09/)   
[Pharr 2017] Matt Pharr, Wenzel Jakob, and Greg Humphreys. "Physically Based Rendering:From Theory To Implementation Third Edition." Morgan Kaufmann Publishers 2017.  
[http://www.pbr-book.org](http://www.pbr-book.org)  
[Cohen 1985] Michael F. Cohen, Donald P. Greenberg. "The hemi-cube: a radiosity solution for complex environments." SIGGRAPH 1998.  
[https://dl.acm.org/citation.cfm?id=325171](https://dl.acm.org/citation.cfm?id=325171)  
[Malley 1988] Thomas J. V. Malley. "A Shading Method for Computer Generated Images." University of Utah Computer Science Technical Report 1988.  
[https://collections.lib.utah.edu/details?id=101866](https://collections.lib.utah.edu/details?id=101866)  
[Khoo 2003] Boo Cheong Khoo, Jacob White, Jaime Peraire, Anthony T. Patera. "Numerical Methods for Partial Differential Equations (SMA 5212)."  MITOpenCourseWare 2003.  
[https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-920j-numerical-methods-for-partial-differential-equations-sma-5212-spring-2003/](https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-920j-numerical-methods-for-partial-differential-equations-sma-5212-spring-2003/)  
[Coombe 2005] Greg Coombe, Mark Harris. "Global Illumination Using Progressive Refinement Radiosity." GPU Gems 2, Chapter 39, 2005.  
[https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter39.html](https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter39.html)   
[Christensen 2008] Per H. Christensen. "Point-Based Approximate Color Bleeding." Pixar Technical Memo 2008.  
[http://graphics.pixar.com/library/](http://graphics.pixar.com/library/)  
[Christensen 2010] Per H. Christensen. "Point-Based Global Illumination for Movie Production." SIGGRAPH 2010.  
[http://graphics.pixar.com/library/](http://graphics.pixar.com/library/)  




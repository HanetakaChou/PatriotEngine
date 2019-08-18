### M(Radiant Existence, 辐射出射度)/B(Radiosity, 辐射度) 

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

//光源的表示：用Albedo为0的Element来表示光源  

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
$F_{ii}=0$ //因为$\Delta_i$和$\Delta_j$背对，即$\cos\theta_i$=0  
K=M-PF  //$K_{ii}=1-\rho F_{ii}=1$ 
  
雅可比（Jacobi）迭代（[Khoo 2003], Lecture 6 "Solution Methods: Iterative Techniques"）  
  
Perhaps the simplest iterative scheme is to update each element Bi(k) of the solution vector by solving for that variable using the current guess Bi
(k).  
余量$r_i = E_i - \sum_{j=1}^{n}K_{ij}B_j$  
$r^{\lparen k+i\rparen} + K_{ii}B^{\lparen k+i\rparen} = r^{\lparen k\rparen} + K_{ii}B^{\lparen k\rparen}$  
//由于$r^{\lparen k+i\rparen}$为0 //$K_{ii}B^{\lparen k+i\rparen} = r^{\lparen k\rparen} + K_{ii}B^{\lparen k\rparen}$  
//$B^{\lparen k+i\rparen} = \frac{B^{\lparen k\rparen} + r^{\lparen k\rparen}}{K_{ii}}$  //$K_{ii}=1-\rho F_{ii}=1$  //是否收敛与谱半径有关

高斯-赛德尔（Gauss-Seidel）迭代  
  
$B_i^{k+1} = \frac{E_i - \sum_{j=1}^{i-1}K_{ij}B_j^{k+1} - \sum_{j=i+1}^{n}K_{ij}B_j^k}{k_{ii}}$  //收敛与对角优势有关
  
在计算$B_i^{k+1}$时，使用了在本次迭代（Iteration）中的先前步骤（Step）中计算得到的$B_1^{k+1}$,$B_2^{k+1}$,$B_3^{k+1}$,...,$B_{i-1}^{k+1}$  
  
//norm（范数）


## 参考文献  
[Ramamoorthi 2009] Ravi Ramamoorthi, James O'Brien. "Advanced Computer Graphics." University Of California at Berkeley, CS 294-13, Fall 2009.  
[http://inst.eecs.berkeley.edu/~cs294-13/fa09/](http://inst.eecs.berkeley.edu/~cs294-13/fa09/)   
[Willcox 2014] Karen Willcox, Qiqi Wang. "Computational Methods in Aerospace Engineering." MITOpenCourseWare 2014.  
[https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-90-computational-methods-in-aerospace-engineering-spring-2014/](https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-90-computational-methods-in-aerospace-engineering-spring-2014/)  
[Cohen 1993] Michael F. Cohen, John R. Wallace. "Radiosity and Realistic Image Synthesis." Academic Press Professional 1993.  
[https://dl.acm.org/citation.cfm?id=154731](https://dl.acm.org/citation.cfm?id=154731)  
[Pharr 2017] Matt Pharr, Wenzel Jakob, and Greg Humphreys. "Physically Based Rendering:From Theory To Implementation Third Edition." Morgan Kaufmann Publishers 2017.  
[http://www.pbr-book.org](http://www.pbr-book.org)  
[Cohen 1985] Michael F. Cohen, Donald P. Greenberg. "The hemi-cube: a radiosity solution for complex environments." SIGGRAPH 1998.  
[https://dl.acm.org/citation.cfm?id=325171](https://dl.acm.org/citation.cfm?id=325171)  
[Malley 1988] Thomas J. V. Malley. "A Shading Method for Computer Generated Images." University of Utah Computer Science Technical Report 1988.  
[https://collections.lib.utah.edu/details?id=101866](https://collections.lib.utah.edu/details?id=101866)  
[Khoo 2003] Boo Cheong Khoo, Jacob White, Jaime Peraire, Anthony T. Patera. "Numerical Methods for Partial Differential Equations (SMA 5212)."  MITOpenCourseWare 2003.  
[https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-920j-numerical-methods-for-partial-differential-equations-sma-5212-spring-2003/](https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-920j-numerical-methods-for-partial-differential-equations-sma-5212-spring-2003/)  
[Coombe 2005] Greg Coombe, Mark Harris "Global Illumination Using Progressive Refinement Radiosity." GPU Gems 2, Chapter 39, 2005.  
[https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter39.html](https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter39.html)  



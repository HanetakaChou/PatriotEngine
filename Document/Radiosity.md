### M(Radiant Existence, 辐射出射度)/B(Radiosity, 辐射度) 

//$M_d = \int_{\Omega}{L_r \lparen\overrightarrow{\omega_i}\rparen \cos \theta_i d\omega}$

//渲染方程（Rendering Equation）

//w_i指入射光的相反方向 
//x指入射点的位置
//x’指发出入射光的位置 

有限元法仅用于离散化渲染方程

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

Nusselt Analog  
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


## 参考文献  
[Cohen 1993] Michael F. Cohen, John R. Wallace. "Radiosity and Realistic Image Synthesis." Academic Press Professional 1993.  
[https://dl.acm.org/citation.cfm?id=154731](https://dl.acm.org/citation.cfm?id=154731)  
[Pharr 2017] Matt Pharr, Wenzel Jakob, and Greg Humphreys. "Physically Based Rendering:From Theory To Implementation Third Edition." Morgan Kaufmann Publishers 2017.  
[http://www.pbr-book.org](http://www.pbr-book.org)  
[Cohen 1985] Michael F. Cohen, Donald P. Greenberg. "The hemi-cube: a radiosity solution for complex environments." SIGGRAPH 1998.  
[https://dl.acm.org/citation.cfm?id=325171](https://dl.acm.org/citation.cfm?id=325171)  
[Malley 1988] Thomas J. V. Malley. "A Shading Method for Computer Generated Images." University of Utah Computer Science Technical Report 1988.  
[https://collections.lib.utah.edu/details?id=101866](https://collections.lib.utah.edu/details?id=101866)  


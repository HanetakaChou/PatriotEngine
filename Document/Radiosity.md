### M(Radiant Existence, 辐射出射度)/B(Radiosity, 辐射度) 

//$M_d = \int_{\Omega}{L_r \lparen\overrightarrow{\omega_i}\rparen \cos \theta_i d\omega}$

//渲染方程（Rendering Equation）

//w_i指入射光的相反方向 
//x指入射点的位置
//x’指发出入射光的位置 

Form Factor

Radiance $\int_{S'} L_r{\lparen x',-\overrightarrow{\omega_i} \rparen} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} dA'$

Radiosity/Radiant Existance $\int_{S'} B{\lparen x'\rparen} \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} dA'$

Weighted Residual  
$\int_S \, {\operatorname{N_i}\lparen x\rparen} \, {\lparen \int_{S'} {\lparen \sum_{j=1}^{n} B_j \operatorname{N_j}\lparen x'\rparen \rparen} \, \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA' \rparen} \, dA$  
=$\sum_{j=1}^{n} B_j \, \lparen \int_S \, {\operatorname{N_i}\lparen x\rparen} \, {\lparen \int_{S'} {\operatorname{N_j}\lparen x'\rparen} \, \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA' \rparen} \, dA \rparen$  
=$\sum_{j=1}^{n} B_j \, \lparen \int_{A_i} {\lparen \int_{A_j} \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA' \rparen} \, dA \rparen$  //常量基函数  

Projected Solid Angle  
$d{\omega}^{\perp}=\cos\theta \, d{\omega}$  //[Cohen 1993] 2.4.3、[Pharr 2017] 5.5.1  

Integrals over Area  
$d{\omega}={\frac{\cos\theta_o}{{\vert p'-p\vert}^2} \, dA}$  //[Cohen 1993] 2.4.2、[Pharr 2017] 5.5.3 


Nusselt Analog  
${\int_{A} \frac{1}{\pi} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} V{\lparen x'\rarr x \rparen} \, dA'}$  
=${\frac {\int_{A} V{\lparen x'\rarr x \rparen} \frac{\cos\theta_i\cos\theta_o}{{\vert x'-x\vert}^2} \, dA'}{\pi}}$  
=${\frac {\int_{A} V{\lparen x'\rarr x \rparen} \cos\theta_i \, d{\omega}}{\pi}}$ //Integrals over Area  
=${\frac {\int_{A} V{\lparen x'\rarr x \rparen} \, d{{\omega}'}^{\perp}}{\pi}}$ //Projected Solid Angle  


## 参考文献  
[Cohen 1993] Michael F. Cohen, John R. Wallace. "Radiosity and Realistic Image Synthesis." Academic Press Professional 1993.  
[https://dl.acm.org/citation.cfm?id=154731](https://dl.acm.org/citation.cfm?id=154731)  
[Pharr 2017] Matt Pharr, Wenzel Jakob, and Greg Humphreys. "Physically Based Rendering:From Theory To Implementation Third Edition." Morgan Kaufmann Publishers 2017.  
[http://www.pbr-book.org](http://www.pbr-book.org)  


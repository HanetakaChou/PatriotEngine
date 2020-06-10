## License  
```  
Copyright (C) 2019 YuqiaoZhang

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>
```  
&nbsp;  
## 纹理  

&nbsp;  
## 球小波（Spherical Wavelets）  

&nbsp;  
## Ambient Cube  

&nbsp;  
## 球高斯（Spherical Gaussian）  

LAGARDE, S., AND DE ROUSIERS. Physically based shading in theory and practice: Moving Frostbite to PBR. In ACM SIGGRAPH Courses 2014.  

HILL, S., MCAULEY, S., BURLEY, B., CHAN, D., FASCIONE,
L., IWANICKI, M., HOFFMAN, N., JAKOB, W., NEUBELT, D.,
PESCE, A., AND PETTINEO, M. 2015. Physically based shading
in theory and practice. In ACM SIGGRAPH Courses 2015.

&nbsp;  
## LTC（Linearly Transformed Cosine）  

约定：
非面光源（Punctual Light）
  点光源/泛光灯（Point Light）
  聚光灯（Spot Light）

为了避免混淆，本文统一使用非面光源（Punctual Light）和泛光灯（Point Light），而避免使用容易引起二义性的“点光源”，但在一些文献中，可能用点光源（Point Light）表示非面光源的含义，需要读者在阅读相关文献时仔细甄别。

面光源（Area Light）   
$L_r{\lparen x, \overrightarrow{\omega_r} \rparen} = \int_{H} f_r{\lparen x, \overrightarrow{\omega_r}, \overrightarrow{\omega_i}  \rparen} L_i{\lparen x,\overrightarrow{\omega_i} \rparen} \cos\theta_i \, d{\omega_i}$  
  
非面光源（Punctual Light）  
在实时渲染中，非面光源的$f_r{\lparen x, \overrightarrow{\omega_r}, \overrightarrow{\omega_i}  \rparen}$为狄拉克δ函数(Dirac delta function)，即只在某一个方向（设$\overrightarrow{\omega_p}$）存在，渲染方程被简化为 $L_r{\lparen x, \overrightarrow{\omega_r} \rparen} = f_r{\lparen x, \overrightarrow{\omega_r}, \overrightarrow{\omega_p} \rparen} L_i{\lparen x,\overrightarrow{\omega_p} \rparen} \cos\theta_p$  


## 参考文献  
[Schroder 1995] Peter Schroder, Wim Sweldens. "Spherical Wavelets: Efficiently Representing Functions on the Sphere." SIGGRAPH 1995.  
[https://dl.acm.org/citation.cfm?id=218439](https://dl.acm.org/citation.cfm?id=218439)  
[Ng 2004] Ren Ng, Ravi Ramamoorthi, Pat Hanrahan. "Triple product wavelet integrals for all-frequency relighting." SIGGRAPH 2004.  
[https://graphics.stanford.edu/papers/allfreqmat/](https://graphics.stanford.edu/papers/allfreqmat/)  
[Wang 2006] Rui Wang, Ren Ng, David Luebke, Greg Humphreys. "Efficient Wavelet Rotation for Environment Map Rendering." EGSR 2006.  
[https://dl.acm.org/citation.cfm?id=2383916](https://dl.acm.org/citation.cfm?id=2383916)  
[McTaggart 2004] Gary McTaggart. "Half-Life 2/Valve Source Shading." GDC 2004.  
[https://steamcdn-a.akamaihd.net/apps/valve/2004/GDC2004_Half-Life2_Shading.pdf](https://steamcdn-a.akamaihd.net/apps/valve/2004/GDC2004_Half-Life2_Shading.pdf)  


[Heitz 2016] Eric Heitz, Jonathan Dupuy, Stephen Hill, David Neubelt. "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines." SIGGRAPH 2016.  
[https://eheitzresearch.wordpress.com/415-2/](https://eheitzresearch.wordpress.com/415-2/)    

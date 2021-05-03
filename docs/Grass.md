## NVIDIA Turf Effects   
The solutions provided by Makarov(1\.\[Makarov 2015\]) and Jahrmann(2\.\[Jahrmann 2017\]) all suggest using the tessellation shader to generate the vertices by the bezier control points.   
However, the tessellation shader is inefficient on the mobile GPU since the output vertices data must be written back into the main memory while the output framebuffer data may be kept in tile memory and discarded when renderpass ends(3\.\[Harris 2019\]).  
In fact, the tessellation shader is not supported by the hardware and is simulated by the computer shader on the Mali GPU.  
Evidently, it's senseless to use the tessellation shader on mobile GPU and thus we intend to use the computer shader mannually and optimize the usage of the computer shader finely.  
As suggested by "Best Practices"(3\.\[Harris 2019\]), we intend to generate two seperated vertex buffers: positions and varyings.  

## TressFX  
4\.\[Han 2015\]  

## Reference  
1\.\[Makarov 2015\] [Evgeny Makarov. "NVIDIA Turf Effects: Massive Grass Rendering With Dynamic Simulation." GTC 2015.](http://on-demand.gputechconf.com/gtc/2015/presentation/S5748-Evgeny-Makarov.pdf)  
Appendix: [GameWorks VisualFX](https://developer.nvidia.com/turfeffects)  
2\.\[Jahrmann 2017\] [Klemens Jahrmann, Michael Wimmer. "Responsive Real-Time Grass Rendering for General 3D Scenes." I3D 2017.](https://www.cg.tuwien.ac.at/research/publications/2017/JAHRMANN-2017-RRTG/JAHRMANN-2017-RRTG-draft.pdf)  
Appendix: [GitHub-Klejah](https://github.com/klejah/ResponsiveGrassDemo) [GitHub-YuqiaoZhang](https://github.com/YuqiaoZhang/ResponsiveGrassDemo)  
3\.\[Harris 2019\] [Pete Harris. "Arm Mali GPUs Best Practices Developer Guide." ARM Developer 2019.](https://developer.arm.com/solutions/graphics/developer-guides/mali-gpu-best-practices)  
4\.\[Han 2015\] Dongsoo Han, Hongwei Li. "Grass Rendering and Simulation with LOD." GPU Pro 6 Chapter II-2 2015.  
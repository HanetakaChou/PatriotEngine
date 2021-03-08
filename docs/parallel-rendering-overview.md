# Parallel Rendering Overview

## Introduction  
In general, the rendering-module reads from the scenegraph(\[1\]) while the logic-module writes into the scenegraph. It's the scenegraph where the race condition occurs.  

## Our parallel strategy

Since there occurs the race condition, the synchronisation should be used to avoid the race condition.  

In Pixar-Hydra, the phase "1. Sync RenderIndex"(\[2\]) handles the synchronisation.  

Some crafts such as COW(Copy-on-write) can be used when the logic-module writes into the scenegraph.

## The drawback of the UE4 strategy  

We assume that "the renderer ran in the Render Thread, and commands for that thread were enqueued by the Game Thread to be run later in the frame"(\[3\]) in UE4.  

The UE4 strategy is esstentially the pipeline parallel pattern(\[4\], \[5\]).  

The "APP/CULL stage" is analogous to the "Game Thread" while the "DRAW stage" is analogous to the "Render Thread".  

The race condition can be handled since the "APP stage" annexes the logic-module which writes into the scenegraph.

The pipeline parallel pattern can't reduce latency and may even increase latency(\[6\]). This means that the sum time of all stages in one frame remains the same and can‘t be reduced by the pipeline parallel pattern.  

We can comprehend from the fact that the "APP/CULL stage" competes for the CPU resources with the "DRAW stage". The pipeline parallel pattern is hostile to the "DRAW stage" which demands CPU resources for DRAW-CALL issuing, state binding and other operations.  

## Reference
\[1\] Real-Time Rendering Fourth Edition / Chapter 19 Acceleration Algorithms / 19.1 Spatial Data Structures / 19.1.5 Scene Graphs  
\[2\] [SIGGRAPH 2019 / A deep dive into universal scene description and hydra](http://graphics.pixar.com/usd/files/Siggraph2019_Hydra.pdf)  
\[3\] [Unreal Engine 4 Documentation / Parallel Rendering Overview / Threading Overview](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/Rendering/ParallelRendering/index.html#threadingoverview)  
\[4\] Real-Time Rendering Fourth Edition / Chapter 18 Pipeline Optimization / 18.5 Multiprocessing / 18.5.1 Multiprocessor Pipelining  
\[5\] Structured Parallel Programming: Patterns for Efficient Computation / CHAPTER 9 Pipeline  
\[6\] Structured Parallel Programming: Patterns for Efficient Computation / CHAPTER 2 Background / 2.5 Performance Theory / 2.5.1 Latency and Throughput 

   

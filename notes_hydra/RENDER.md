### Hydra Execute

**hdEngine::Execute** Main Entry Point
```cxx
--- Python ---  
StageView::_getRenderer  
  UsdImagingGL.Engine
StageView::renderSinglePass  
  Render  
  
wrapEngine  
  def UsdImagingGL.Engine.Render = UsdImagingGLEngine::Render  
     
--- USD ---  
UsdImagingGLEngine::Render
  UsdImagingGLEngine::PrepareBatch
    UsdImagingDelegate::SetTime
  UsdImagingGLEngine::RenderBatch
    HdxTaskController::GetRenderingTasks // HdxTaskController::_CreateRenderGraph // A Tiny Sample 2: Create your task graph
    UsdImagingGLEngine::_Execute  
   
--- Hydra ---  
HdEngine::Execute // **Main Entry Point**
  HdRenderIndex::SyncAll // Phase 1: Sync RenderIndex // Pulls data from the scene graph
    for task : tasks
      HdTask::Sync // UsdImagingDelegate allow multithreaded 
    WorkParallelForN   
      parallel_for // No Dependencies    

  for task : tasks 
    HdTask::Prepare // Phase 2: Prepare all tasks // Opportunity to resolve prim dependencies since sync has run for all prims // Dependencies -> Not parallel ? 
   
  HdStRenderDelegate::CommitResources // Phase 3: Commit resources // Opportunity to submit to the GPU for instance  

  for task : tasks 
    HdTask::Execute // Phase 4: Execute all tasks // Rendering    
```

VtArray // copy-on-write support  


#### UsdImagingDelegate(HdSceneDelegate)   
implement the following functions // can be called from multithreading code    
GetMeshTopology  
GetTransform  
Get  
GetPrimvarDescriptors  

//pxr/imaging/lib/hdSt/shaders/mesh.glslfx  
HdSt_CodeGen::_GenerateConstantPrimvar //pxr/imaging/lib/hdSt/shaders/mesh.glslfx  
  
#### Proxy

UsdImagingIndexProxy // Async insert remove 

Although HdRenderIndex also has "insert / remove" methods, only unit tests use these methods.   

HdRenderIndex->GetChangeTracker->HdChangeTracker  

HdChangeTracker::MarkRprimDirty //Mark Dirty manually  

#### HdTask(HdxRenderTask) 
// may use job instead of task to distinguished from the intel tbb task

```cxx
HdxRenderTask::Execute // Phase 2: Prepare all tasks


HdxRenderTask::Execute  
  HdRenderPass::Execute  
    HdSt_RenderPass::_Execute 
      HdSt_RenderPass::_Cull // CPU Culling
      HdStCommandBuffer::PrepareDraw // GPU Culling
      HdStCommandBuffer::ExecuteDraw // Draw
```
  
#### Animation HdSprim(HdStExtComputation)  
  
UsdSkelImagingSkeletonAdapter::Populate  
  InsertSprim // bind self with a SPRIM(HdStExtComputation) by id(SdfPath)    

HdSceneDelegate  
  |  
UsdImagingDelegate  
   
HdSprim    
  |   
HdExtComputation     
  |   
HdStExtComputation    
   
hdEngine::Execute    
  HdRenderIndex::SyncAll   
    SyncPrims //sprimIndex   
      HdStExtComputation::Sync
        UsdImagingDelegate::GetExtComputationInput    
          UsdImagingDelegate::_UpdateSingleValue  //HdExtComputation::DirtySceneInput  
            UsdSkelImagingSkeletonAdapter::UpdateForTime  
              UsdSkelImagingSkeletonAdapter::_UpdateSkinningComputationForTime  
              _ComputeSkinningTransforms //interpolate anime by time  
                            
_IsEnabledCPUComputations // skin by cpu (deprecated)  
_IsEnabledAggregatorComputation // skin by compute shader // not by vertex shader  

### Hydra Primitives    
   
HdBprim //B(uffer) Prim(itive)     
\[pxr/imaging/lib/hd/bprim.h\]       
\[pxr/imaging/lib/hdSt/renderDelegate.cpp\]  
```c++
HdStRenderDelegate::CreateBprim  
  HdStTexture HdStRenderBuffer HdStField  
```  
   
HdSprim //S(tate) Prim(itive)     
\[pxr/imaging/lib/hd/sprim.h\]    
\[pxr/imaging/lib/hdSt/renderDelegate.cpp\]  
```C++
HdStRenderDelegate::CreateSprim  
  HdCamera HdStLight HdStMaterial HdStDrawTarget HdStExtComputation  
```  

HdRprim //R(enderable) Prim(itive)  
\[pxr/imaging/lib/hd/rprim.h\]   
\[pxr/imaging/lib/hdSt/renderDelegate.cpp\]  
```c++
HdStRenderDelegate::CreateRprim
  HdStMesh HdStBasisCurves HdStPoints HdStVolume       
```
                   

Python  

--- 
USD  
UsdImagingGLEngine::Render
UsdImagingGLEngine::RenderBatch
UsdImagingGLEngine::_Execute  

---  
Hydra  
hdEngine::Execute  
HdxRenderTask::Execute  
HdRenderPass::Execute  
HdSt_RenderPass::_Execute 
  HdSt_RenderPass::_Cull //CPU Culling
  HdStCommandBuffer::PrepareDraw //GPU Culling
  HdStCommandBuffer::ExecuteDraw //Draw

     
---    
   
HdBprim     
//buffer prim     
//managing a blob of data that is used to communicate between the scene delegate and render    
//HdStTexture HdStRenderBuffer HdStField   
\[pxr/imaging/lib/hd/bprim.h\]     
   
HdSprim    
//state prim    
//managing state for non-drawable scene entity     
//HdCamera HdStLight HdStMaterial HdStDrawTarget HdStExtComputation      
\[pxr/imaging/lib/hd/sprim.h\]   
    
HdRprim   
//the render engine state for a given rprim from the scene graph       
//HdStMesh HdStBasisCurves HdStPoints HdStVolume       
\[pxr/imaging/lib/hd/rprim.h\]          
                
---   
HdChangeTracker


---  
Anim  
  
UsdSkelImagingSkeletonAdapter::Populate  
  InsertSprim // register self as a SPRIM   

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
_IsEnabledAggregatorComputation // skin by compute shaer // not by vertex shader  
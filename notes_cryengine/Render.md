
### RendererCVars  

r_GraphicsPipeline // CRendererCVars::InitCVars \[Code/CryEngine/RenderDll/Common/RendererCVars.cpp:1125\]  

### CRenderView  

Cry3DEngine // producer   
     ↓   
CRenderView  // contains all render items (in a camera view) \[Code/CryEngine/RenderDll/Common/RenderView.h:18\]    
     ↓  
CryRenderD3D11  // consumer   


RenderItems m_renderItems[EFSLIST_NUM] // the lists of render items


#### CryRenderD3D11  

```  
CSceneGBufferStage::Execute
  RenderSceneOpaque   
```  


### Portal Culling   
    
[CRYENGINE 3 Manual / Entities, Shapes and Tools / Area Objects / VisArea and Portals](https://docs.cryengine.com/display/SDKDOC2/VisArea+and+Portals)   
   
### Geometry Format

```  
CEntity::LoadGeometry // the logic layer   
  CRenderProxy::LoadGeometry 
//-------------------------------------------------------  
    C3DEngine::LoadStatObj
      CObjManager::LoadStatObj  // see the notes related to stream
        //Load Default Object

```  

```   
CEntity::LoadCharacter 
  CRenderProxy::LoadCharacter
    CharacterManager::CreateInstance
      CharacterManager::LoadCharacterDefinition //cdf 
        CharacterManager::GetOrLoadCDFId
          CharacterManager::LoadCDF // sync load // not stream
            CharacterManager::LoadCDFFromXML 
              CMatMan::LoadMaterial  
        CharacterManager::CreateInstance //chr  
          CharacterManager::CreateSKELInstance //chr
            CharacterManager::FetchModelSKEL //chr
            CDefaultSkeleton::LoadNewSKEL //chr
              C3DEngine::LoadChunkFileContent //chr
                CLoaderCGF::LoadCGF //chr
              CDefaultSkeleton::LoadCHRPARAMS//chrparams
```   

### Runtime Format   

#### 
//cdf CharacterDefinition //CharacterManager::LoadCDFFromXML  

CRenderMesh 
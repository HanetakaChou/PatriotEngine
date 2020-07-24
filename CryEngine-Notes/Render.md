
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

### 

```  
CEntity::LoadGeometry // the logic layer   
  CRenderProxy::LoadGeometry 
//-------------------------------------------------------  
    C3DEngine::LoadStatObj
      CObjManager::LoadStatObj  // see the notes related to stream
```  

```   
CEntity::LoadCharacter 
  CRenderProxy::LoadCharacter
    CharacterManager::CreateInstance
      CharacterManager::LoadCharacterDefinition  
        ...
          CharacterManager::LoadCDF // sync load // not stream
            CharacterManager::LoadCDFFromXML
```   

### Runtime Format   

#### 
CharacterDefinition //CharacterManager::LoadCDFFromXML  

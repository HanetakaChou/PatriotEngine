
### Application Layer - Request for Rendering     
   
//There is no SceneTree in CryEngine   
//CryEngine doesn't maintain the propagation of the transform from the parent to the child and leave this work to the application layer  

CryEngine provides three interfaces:  
1\.Static Object  
CEntity::LoadGeometry  
2\.Character  
CEntity::LoadCharacter    
3\.Light  
CEntity::LoadLight  

```  
CEntity::LoadGeometry // the logic/application layer   
  CRenderProxy::LoadGeometry 
//-------------------------------------------------------  
    C3DEngine::LoadStatObj
      CObjManager::LoadStatObj  // not actually load // create an empty static object instead  
    ...
    CRenderProxy::InvalidateBounds
      CRenderProxy::RegisterForRendering  //Add to m_deferredRenderProxyStreamingPriorityUpdates
```  

### Rendering Loop - Request for Streaming  
    
//Streaming + LOD  
  
//nRoundId  
//CObjManager::m_nUpdateStreamingPrioriryRoundId  
//CObjManager::m_nUpdateStreamingPrioriryRoundIdFast  

CObjManager: The Manager for Static Objects  
CharacterManager: The Manager for Characters  

\/\/Precache - Loading   
   
```    
CObjManager::UpdateRenderNodeStreamingPriority
  CObjManager::GetObjectLOD //the LOD system  
  ...
  //1.Static Object
  CObjManager::PrecacheStatObj //Add to m_arrStreamableObjects
    CStatObj::UpdateStreamableComponents
      CStatObj::UpdateStreamingPrioriryInternal
        IStreamable::UpdateStreamingPrioriryLowLevel //use nRoundId to avoid duplicated streaming 
        ...
        CObjManager::RegisterForStreaming //Add to m_arrStreamableObjects  
  //For CGA ???
  CObjManager::PrecacheCharacter
```      
    
---

//Request      
```     
C3DEngine::RenderWorld
  C3DEngine::UpdateRenderingCamera
    CObjManager::UpdateRenderNodeStreamingPriority //Remove from m_deferredRenderProxyStreamingPriorityUpdates //Add to m_arrStreamableObjects
  ...
  C3DEngine::RenderInternal
    C3DEngine::RenderScene
      ...
      //2.Charactor
      CharacterManager::UpdateStreaming
        CharacterManager::UpdateStreaming_SKEL
          CryCHRLoader::BeginLoadCHRRenderMesh
    ...   
    //1.Static Object
    CObjManager::ProcessObjectsStreaming
      CObjManager::UpdateStreamingPriority 
        CObjManager::UpdateObjectsStreamingPriority
          COctreeNode::UpdateStreamingPriority //Remove from m_arrStreamingNodeStack
            CObjManager::UpdateRenderNodeStreamingPriority //Iter the m_arrObjects //Add to m_arrStreamableObjects
      ...  
      CObjManager::ProcessObjectsStreaming_Finish //Remove from m_arrStreamableObjects
        CStatObj::StartStreaming 
          CStreamEngine::StartRead //Forward to the Streaming module          
```   


### Streaming Module 

Request  
//Wait //no wait  
Signal  

//Request
```
CStreamEngine::StartRead //Invoked from CStatObj::StartStreaming   
  CStreamEngine::StartFileRequest
      CStreamingIOThread::AddRequest
        //m_newFileRequests.push_back
```   

Streaming File IO HDD Thread //磁盘   
Streaming File IO Optical Thread //光驱   
Streaming File IO InMemory Thread //内存   

//Could these code be replaced by OS's async read?    
//Perhaps not? The OS's read may be replaced by gzread or etc sometimes and thus it is difficulty to interact with the OS's read.

//The number of CStreamingIOThread is fixed   
//Only one thread is permitted to access the same disk at the same time.  
//Thus, one thread would be enough when there is only one disk.  

//Could these code be replaced by the seiral stage of the pipeline pattern? //Intel TBB  

//Signal  
```
CStreamingIOThread  
  while
    ProcessNewRequests 
      //m_newFileRequest <-> temporaryArray
      //temporaryArray -> m_fileRequestQueue   
    while 
      m_fileRequestQueue //pop_back

      Handle OOM(Out Of **Budget* Memory) //OS Provides Virtual Memory
      //Staging Buffer 或许可以借鉴该机制

      CAsyncIOFileRequest::ReadFile //ReadFile //Alloc Memory //会反馈OOM(Out Of **Budget* Memory)
        CAsyncIOFileRequest::ReadFileInPages
          ZipDir::Cache::ReadFileStreaming
            ReadFile //Win32 API //同步IO //能否用异步IO取代？

      CAsyncIOFileRequest::JobFinalize_Read
        CAsyncIOFileRequest::JobFinalize_Transfer
          SStreamRequestQueue::TransferRequest
```         

```  
SStreamRequestQueue //General Stream Request  
    TransferRequest //Request
    TryPopRequest //Wait
```   

//Signal
```   
Streaming AsyncCallback Thread
    poll //TryPopRequest The SStreamRequestQueue
        CStreamEngine::ReportAsyncFileRequestComplete //Signal
          ...
            IStreamCallback::StreamAsyncOnComplete
            //Following Implementations
            //1.Static Object
            CStatObj::StreamAsyncOnComplete
              CStatObj::LoadStreamRenderMeshes
                ReadOnlyChunkFile::ReadFromMemory
                CLoaderCGF::LoadCGF
                  CLoaderCGF::LoadCGF_Int //Int(ernal)
                  CLoaderCGF::LoadChunks
                CStatObj::MakeRenderMesh
            //2.Charactor
            CryCHRLoader::StreamAsyncOnComplete
              CryCHRLoader::EndStreamSkel //chr
                CModelMesh::InitRenderMeshAsync
                  CModelMesh::CreateRenderMesh //sync in fact ???
                    CRenderMesh::SetMesh
                      CRenderMesh::SetMesh_Int
                        CRenderMesh::LockVB 
              CryCHRLoader::EndStreamSkinAsync //skin
                  CModelMesh::InitRenderMeshAsync
                    CModelMesh::CreateRenderMesh //sync in fact ???
```   

### TextureStream
s_StreamInTasks  

CSystem::RenderEnd  
  CTexture::Update //Static Method  
  CTexture::StartStreaming  

### Thread

CrySetThreadName
   

---   
   
### Geometry Format   
  
[CRYENGINE 3 Manual / Asset Creation / Art Asset File Types](https://docs.cryengine.com/display/SDKDOC2/Art+Asset+File+Types)  
[CRYENGINE V Manual / Asset Prep (External) / Asset Exporting Overview / Art Asset File Types](https://docs.cryengine.com/display/CEMANUAL/Art+Asset+File+Types)  

CGF \/\*Crytek Geometry Format\*\/  The Static Geometry  

//Shadow Proxy  
[CRYENGINE V Manual / Asset Prep (External) / Asset Exporting Overview / Geometry Creation Overview / Static Geometry / Shadow Proxies](https://docs.cryengine.com/display/CEMANUAL/Shadow+Proxies)   
//CryEngine introduce Shadow Proxy to boost performance  
The engine should provides functionality to support the No-Shadow option and it's the convention/Artist-Workflow to create the RenderMesh with the No-Shadow option checked and the Shadow Proxy with No-Shadow option unchecked.   
  
𓃤𓁴  
CGA \/\*Crytek Geometry Animation\*\/ //Rigid Body Animation   
The nodes are not merged and can be animated at runtime. //Diff from the CGF   
   
CHR \/\*Character\*\/   
The Skeleton   

SKIN \/\*Skinned Render Mesh\*\/   
The Skeletal Geometry  //contain vertex weights    
//Addition: morph targets are also stored in this format   
    
   
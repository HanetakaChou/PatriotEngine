Request  
//Wait //no wait  
Signal  

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
      CObjManager::LoadStatObj  // see the notes related to stream
        //Load Default Object
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


### Streaming Module //能否用OS的异步IO取代？
//Request
```
CStreamEngine::StartRead  
  CStreamEngine::StartFileRequest
      CStreamingIOThread::AddRequest
        //m_newFileRequests.push_back
```   
   
//The number of CStreamingIOThread is fixed  
Streaming File IO HDD Thread //磁盘   
Streaming File IO Optical Thread //光驱   
Streaming File IO InMemory Thread //内存   

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

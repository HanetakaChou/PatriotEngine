Request  
//Wait //no wait  
Signal  

//Streaming+LOD  
  
//nRoundId  
//CObjManager::m_nUpdateStreamingPrioriryRoundId  
//CObjManager::m_nUpdateStreamingPrioriryRoundIdFast  

CObjManager: The Manager for Static Objects  
CharacterManager: The Manager for Characters  

### Rendering Loop - Request
//Request
```   
C3DEngine::RenderWorld
  C3DEngine::UpdateRenderingCamera
    CObjManager::UpdateRenderNodeStreamingPriority
      CObjManager::GetObjectLOD //the LOD system  
      ...
      CObjManager::PrecacheStatObj
        CStatObj::UpdateStreamableComponents
          CStatObj::UpdateStreamingPrioriryInternal
            IStreamable::UpdateStreamingPrioriryLowLevel //use nRoundId to avoid duplicated streaming 
            CObjManager::RegisterForStreaming //add to m_arrStreamableObjects
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
      CObjManager::ProcessObjectsStreaming_Finish
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

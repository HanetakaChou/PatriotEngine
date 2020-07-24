### Stream

Request
Wait
Signal

IStreamEngine  //能否用OS的异步IO取代？
  StartRead //Request
    StartFileRequest
      CStreamingIOThread::AddRequest

```   
CStreamingIOThread   

  AddRequest //Request   
    // -> m_newFileRequest   

  Streaming File IO HDD Thread //磁盘   
  Streaming File IO Optical Thread //光驱   
  Streaming File IO InMemory Thread //内存   

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

```   
Streaming AsyncCallback Thread
    poll //TryPopRequest The SStreamRequestQueue
        CStreamEngine::ReportAsyncFileRequestComplete //Signal
          ...
            IStreamCallback::StreamAsyncOnComplete
```   

### TextureStream
s_StreamInTasks

CSystem::RenderEnd
  CTexture::Update //Static Method
  CTexture::StartStreaming


### Graphics Rendering   

```   
C3DEngine::RenderWorld
  C3DEngine::RenderInternal
    C3DEngine::RenderScene
//------------------------------------------       
    CObjManager::ProcessObjectsStreaming
      CStatObj::StartStreaming
```   



### Thread

CrySetThreadName


//Adam Sawicki. "Memory Management in Vulkan™ and DX12." GDC 2018.

//
//RENDERTARGET 

//Tile-Based GPU //MTLStorageModeMemoryless //

//All GPU //Hardware may compress

//D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //Only for depth stencil //Depth Stencil may support hirechal-z
//FrameBufferFetch can't apply to depth stencil //Metal write depth to color attachment //Turn on early-z manually?

//D3D12: Some adapter architectures experience increased bandwidth for depth stencil textures when shader resource views are precluded. If a texture is rarely used for shader resource, it may be worth having two textures around and copying between them. One texture would have this flag and the other wouldn't. Applications should set this flag when depth stencil textures will never be used from shader resource views.
//Application don't know whether the adapter //This strategy apply to all adapter

//D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
//meaningless? Compute Shader <=> Vertex Shader + Transform Feedback //Use Buffer instead of texture

//D3D12: Some adapter architectures must resort to less efficient texture layouts in order to provide this functionality.If a texture is rarely used for unordered access, it may be worth having two textures around and copying between them.One texture would have this flag, while the other wouldn't.
//Virtual Texture //Compress

//ColorAttachment
//ColorAttachment_Memoryless
//DepthAttachment_Memoryless //Always write to color attachment




//-------------------------------------------------------------------------------------------------

//ASSET

//BC DDS
//ASTC PVR

//ASSET_MAP //

//ASSET_VB
//ASSET_IB

//------------------------------------------------------------
//RingBuffer / StagingBuffer / UploadBuffer

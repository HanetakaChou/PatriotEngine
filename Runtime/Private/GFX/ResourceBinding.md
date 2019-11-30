https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/direct3d12/specifying-root-signatures-in-hlsl.md
RootSignatures <-> PipelineLayout
DescriptorTable + StaticSampler <-> DescriptorSetLayout
DescriptorRange <-> DescriptorSetLayoutBinding/binding+descriptorType+descriptorCount
StaticSampler	<-> DescriptorSetLayoutBinding/pImmutableSamplers
RootConstants <-> PushConstants
RootLevelCBV/SRV/UAV <-> ??? DynamicUniformBuffer/DynamicStorageBuffer

DescriptorHeap <-> DescriptorPool //Multi-Thread
               <-> DescriptorSet
ID3D12Device::Create*View <-> vkUpdateDescriptorSets

ID3D12GraphicsCommandList::Set*RootSignature <-> vkCmdBindPipeline //Pipeline/layout
ID3D12GraphicsCommandList::SetDescriptorHeap <->
ID3D12GraphicsCommandList::Set*RootDescriptorTable <-> vkCmdBindDescriptorSets

ID3D12GraphicsCommandList::Set*Root32BitConstant <-> vkCmdPushConstants
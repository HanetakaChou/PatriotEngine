#include "PTVDInstanceImpl.h"

#include "../../../Public/McRT/PTSThread.h"
#include "../../../Public/McRT/PTSMemoryAllocator.h"
#include <new>

IPTVDInstance * PTCALL PTVDInstance_ForProcess()
{
	static PTVDInstanceImpl *s_InstanceImpl_Singleton_Pointer = reinterpret_cast<PTVDInstanceImpl *>(uintptr_t(0X1U));//������Ϊ��Чֵ����Ϊ������Alignment
	static int32_t s_InstanceImpl_Initialize_RefCount = 0;

	PTVDInstanceImpl *pInstanceImpl;
	
	if (((pInstanceImpl = ::PTSAtomic_Get(&s_InstanceImpl_Singleton_Pointer)) == reinterpret_cast<PTVDInstanceImpl *>(uintptr_t(0X1U))) && (::PTSAtomic_GetAndAdd(&s_InstanceImpl_Initialize_RefCount, 1) == 0))
	{
		HRESULT hResult;

#ifndef NDEBUG
		ID3D12Debug *pDebug;
		hResult = ::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));
		assert(SUCCEEDED(hResult));
		assert(pDebug != NULL);
		pDebug->EnableDebugLayer();
		pDebug->Release();
#endif
		IDXGIFactory4 *pDXGIFactory;
		hResult = ::CreateDXGIFactory2(
#ifndef NDEBUG
			DXGI_CREATE_FACTORY_DEBUG,
#else
			0U,
#endif
			IID_PPV_ARGS(&pDXGIFactory)
		);
		assert(SUCCEEDED(hResult));
		assert(pDXGIFactory != NULL);

		//��DeviceNUMA And DeviceUMA -------------------------------------
		::PTS_CPP_Vector<ID3D12Device *> DeviceNUMAPointerS;
		::PTS_CPP_Vector<ID3D12Device *> DeviceUMAPointerS;

		IDXGIAdapter1 *pAdapter;
		for (UINT i = 0U; SUCCEEDED(pDXGIFactory->EnumAdapters1(i, &pAdapter)); ++i)
		{
			DXGI_ADAPTER_DESC1 AdapterDesc;
			pAdapter->GetDesc1(&AdapterDesc);
			assert((AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0U);
			if (!(AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
			{
				ID3D12Device *pDevice;
				if (SUCCEEDED(::D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
				{
					D3D12_FEATURE_DATA_ARCHITECTURE FeatureSupportData;
					FeatureSupportData.NodeIndex = 0U;//[In]
					hResult = pDevice->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &FeatureSupportData, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE));
					assert(SUCCEEDED(hResult));
					
					if (FeatureSupportData.UMA != TRUE)
					{
						DeviceNUMAPointerS.push_back(pDevice);
					}
					else
					{
						DeviceUMAPointerS.push_back(pDevice);
					}
				}
			}
		}

		//�������²���
		//1:ͬʱ���ڶ��Ժͼ��ԣ�һ��ΪDesktop��Ϸ�ʼǱ���Desktop̨ʽ������Ҫ��BIOS�����ú����Կ�����
		//Device3DΪ����
		//DeviceComputeΪ����
		//2:ֻ���ڶ��ԣ�һ��ΪDesktop̨ʽ����
		//Device3DΪ����
		//DeviceComputeΪNULL
		//3:ֻ���ڼ��ԣ�һ��ΪDesktop�ᱡ�ʼǱ���Mobile��
		//Device3DΪ����
		//DeviceComputeΪNULL

		if (DeviceNUMAPointerS.size() > 0U && DeviceUMAPointerS.size() > 0U)
		{
			//ͬʱ���ڶ��Ժͼ��ԣ�һ��ΪDesktop��Ϸ�ʼǱ���Desktop̨ʽ������Ҫ��BIOS�����ú����Կ�����

			void *pDevice3DImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDDevice3DImpl), alignof(PTVDDevice3DImpl));
			assert(pDevice3DImplVoid != NULL);

			PTVDDevice3DImpl *pDevice3DImpl = new(pDevice3DImplVoid)PTVDDevice3DImpl(
				DeviceNUMAPointerS[0],
				&pDXGIFactory
			);

			bool bResult = pDevice3DImpl->Initilize();
			assert(bResult != false);

			void *pInstanceImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDInstanceImpl), alignof(PTVDInstanceImpl));
			assert(pInstanceImplVoid != NULL);

			//DeviceUMAPointerS[0] For Compute

			pInstanceImpl = new(pInstanceImplVoid)PTVDInstanceImpl(
				pDevice3DImpl
			);

			for (size_t i = 1U; i < DeviceNUMAPointerS.size(); ++i)
			{
				DeviceNUMAPointerS[i]->Release();
			}

			for (size_t i = 1U; i < DeviceUMAPointerS.size(); ++i)
			{
				DeviceUMAPointerS[i]->Release();
			}

		}
		else if (DeviceNUMAPointerS.size() > 0U && DeviceUMAPointerS.size() == 0U)
		{
			//ֻ���ڶ��ԣ�һ��ΪDesktop̨ʽ����

			void *pDevice3DImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDDevice3DImpl), alignof(PTVDDevice3DImpl));
			assert(pDevice3DImplVoid != NULL);

			PTVDDevice3DImpl *pDevice3DImpl = new(pDevice3DImplVoid)PTVDDevice3DImpl(
				DeviceNUMAPointerS[0],
				&pDXGIFactory
			);

			bool bResult = pDevice3DImpl->Initilize();
			assert(bResult != false);

			void *pInstanceImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDInstanceImpl), alignof(PTVDInstanceImpl));
			assert(pInstanceImplVoid != NULL);

			pInstanceImpl = new(pInstanceImplVoid)PTVDInstanceImpl(
				pDevice3DImpl
			);

			for (size_t i = 1U; i < DeviceNUMAPointerS.size(); ++i)
			{
				DeviceNUMAPointerS[i]->Release();
			}
		}
		else if (DeviceNUMAPointerS.size() == 0U && DeviceUMAPointerS.size() > 0U)
		{
			//ֻ���ڼ��ԣ�һ��ΪDesktop�ᱡ�ʼǱ���Mobile��

			void *pDevice3DImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDDevice3DImpl), alignof(PTVDDevice3DImpl));
			assert(pDevice3DImplVoid != NULL);

			PTVDDevice3DImpl *pDevice3DImpl = new(pDevice3DImplVoid)PTVDDevice3DImpl(
				DeviceUMAPointerS[0],
				&pDXGIFactory
			);

			bool bResult = pDevice3DImpl->Initilize();
			assert(bResult != false);

			void *pInstanceImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDInstanceImpl), alignof(PTVDInstanceImpl));
			assert(pInstanceImplVoid != NULL);

			pInstanceImpl = new(pInstanceImplVoid)PTVDInstanceImpl(
				pDevice3DImpl
			);

			for (size_t i = 1U; i < DeviceUMAPointerS.size(); ++i)
			{
				DeviceUMAPointerS[i]->Release();
			}
		}
		else
		{
			assert(DeviceNUMAPointerS.size() == 0U && DeviceUMAPointerS.size() == 0U);

			pInstanceImpl = NULL;
		}
	}
	else
	{
		//Wait
		while (pInstanceImpl == reinterpret_cast<PTVDInstanceImpl *>(uintptr_t(0X1U)))
		{
			::PTS_Yield();
			pInstanceImpl = ::PTSAtomic_Get(&s_InstanceImpl_Singleton_Pointer);
		}
	}

	return pInstanceImpl;
}



static inline UINT64 PTG_Size_AlignUpFrom(UINT64 Value, UINT64 Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

#include "PTShader_GPGPU_Scan_RootSignature.h"
#include "PTShader_GPGPU_Scan_UpSweep_ComputeShader.h"
#include "PTShader_GPGPU_Scan_DownSweep_ComputeShader.h"

#include "../../../Public/Window/PTWWindow.h"

void PT3DInstanceImpl::WindowEventOutputCallback(
	void *pEventDataVoid
)
{
	IPTWWindow::EventOutput *pEventDataGeneric = static_cast<IPTWWindow::EventOutput *>(pEventDataVoid);

	switch (pEventDataGeneric->m_Type)
	{
	case IPTWWindow::EventOutput::Type_WindowCreated:
	{

	}
	break;
	case IPTWWindow::EventOutput::Type_WindowResized:
	{
		IPTWWindow::EventOutput_WindowResized *pEventDataSpecific = static_cast<IPTWWindow::EventOutput_WindowResized *>(pEventDataGeneric);

		HRESULT hResult;

		DXGI_SWAP_CHAIN_DESC1 Desc;
		Desc.Width = pEventDataSpecific->m_Width;
		Desc.Height = pEventDataSpecific->m_Height;
		Desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; //Rec.2020
		Desc.Stereo = FALSE;
		Desc.SampleDesc.Count = 1U;
		Desc.SampleDesc.Quality = 0U;
		Desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.BufferCount = 2U;
		Desc.Scaling = DXGI_SCALING_NONE;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		Desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		Desc.Flags = 0U;
		
		IDXGISwapChain1 *pSwapChain1;
		hResult = m_pDXGIFactory->CreateSwapChainForHwnd(m_pDeviceGraphics_QueueGraphics, pEventDataSpecific->m_hWindow, &Desc, NULL, NULL, &pSwapChain1);
		assert(SUCCEEDED(hResult));

		IDXGISwapChain3 *pSwapChain3;
		hResult = pSwapChain1->QueryInterface(IID_PPV_ARGS(&pSwapChain3));
		assert(SUCCEEDED(hResult));

		pSwapChain1->Release();

		IDXGISwapChain3 *pSwapChain_Previous = ::PTSAtomic_GetAndSet(&m_pDeviceGraphics_SwapChain, pSwapChain3);
		if (pSwapChain_Previous != NULL)
		{
			pSwapChain_Previous->Release();
		}
	}
	break;
	case IPTWWindow::EventOutput::Type_WindowDestroyed:
	{

	}
	break;
	default:
		assert(0);
	}

}

void PT3DInstanceImpl::Release()
{

}

void PT3DInstanceImpl::Render(
	PTGCamera *pCamera,
	IPTGSceneGraph *pSceneGraph,
	IPTGImage *pImageOut
)
{
	//BVH(Bounding Volume Hierarchy) Construction
	
	if (m_pDeviceCompute == NULL)
	{
		//CPU
		//Task Scheduler
	}
	else
	{
		//GPU
		//
	}

	//����Material����
	//uint32_t MeshNumber, IPTGMesh *MeshArray[],
	//uint32_t MeshSkinNumber, IPTGMeshSkin *MeshSkinArray[],
	//uint32_t TerrainNumber, IPTGTerrain *TerrainArray[],
	//uint32_t OceanNumber, IPTGOcean *OceanArray[],

	//DeviceComputeΪNULL����

	HRESULT hResult;

	ID3D12Heap *pHeapBuffer;
	{
		D3D12_HEAP_DESC Desc;
		Desc.SizeInBytes = 1024U * 1024U * 16U;
		Desc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM; //Upload��Readback������InitialState���Ӷ����ƶ�Map/Unmap���Ż� //��ʹ��������ΪUpload��Readback�Ķѣ�ҲӦ��ΪCustom
		Desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		Desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		Desc.Properties.CreationNodeMask = 0X1U;
		Desc.Properties.VisibleNodeMask = 0X1U;
		Desc.Alignment = 1024U * 64U;//D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
		Desc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
		hResult = m_pDeviceCompute->CreateHeap(&Desc, IID_PPV_ARGS(&pHeapBuffer));
		assert(SUCCEEDED(hResult));
	}

	UINT64 BumpPointer = 0U;

	ID3D12Resource *pBufferInput;
	{
		D3D12_RESOURCE_DESC Desc;
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Alignment = 1024U * 64U;//D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
		Desc.Width = sizeof(float) * 2048U;
		Desc.Height = 1U;
		Desc.DepthOrArraySize = 1U;
		Desc.MipLevels = 1U;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.SampleDesc.Count = 1U;
		Desc.SampleDesc.Quality = 0U;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		BumpPointer = ::PTG_Size_AlignUpFrom(BumpPointer, Desc.Alignment);
		hResult = m_pDeviceCompute->CreatePlacedResource(pHeapBuffer, BumpPointer, &Desc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&pBufferInput));
		assert(SUCCEEDED(hResult));

		BumpPointer += ::PTG_Size_AlignUpFrom(Desc.Width, Desc.Alignment);
		assert(m_pDeviceCompute->GetResourceAllocationInfo(0X1U, 1U, &Desc).SizeInBytes == ::PTG_Size_AlignUpFrom(Desc.Width, Desc.Alignment));
	}

	void *pBufferInputData;
	{
		D3D12_RANGE ReadRange;
		ReadRange.Begin = 0U;
		ReadRange.End = 0U;
		hResult = pBufferInput->Map(0U, &ReadRange, &pBufferInputData);
		assert(SUCCEEDED(hResult));
	}

	for (int i = 0; i < 2048; ++i)
	{
		static_cast<float*>(pBufferInputData)[i] = static_cast<float>(i);
	}

	ID3D12Resource *pBufferOutputPartialReduce;
	{
		D3D12_RESOURCE_DESC Desc;
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Alignment = 1024U * 64U;//D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
		Desc.Width = sizeof(float) * 512U;
		Desc.Height = 1U;
		Desc.DepthOrArraySize = 1U;
		Desc.MipLevels = 1U;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.SampleDesc.Count = 1U;
		Desc.SampleDesc.Quality = 0U;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		BumpPointer = ::PTG_Size_AlignUpFrom(BumpPointer, Desc.Alignment);
		hResult = m_pDeviceCompute->CreatePlacedResource(pHeapBuffer, BumpPointer, &Desc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&pBufferOutputPartialReduce));
		assert(SUCCEEDED(hResult));

		BumpPointer += ::PTG_Size_AlignUpFrom(Desc.Width, Desc.Alignment);
		assert(m_pDeviceCompute->GetResourceAllocationInfo(0X1U, 1U, &Desc).SizeInBytes == ::PTG_Size_AlignUpFrom(Desc.Width, Desc.Alignment));
	}

	ID3D12Resource *pBufferOutputGlobalScan;
	{
		D3D12_RESOURCE_DESC Desc;
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Alignment = 1024U * 64U;//D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
		Desc.Width = sizeof(float) * 2048U;
		Desc.Height = 1U;
		Desc.DepthOrArraySize = 1U;
		Desc.MipLevels = 1U;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.SampleDesc.Count = 1U;
		Desc.SampleDesc.Quality = 0U;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		BumpPointer = ::PTG_Size_AlignUpFrom(BumpPointer, Desc.Alignment);
		hResult = m_pDeviceCompute->CreatePlacedResource(pHeapBuffer, BumpPointer, &Desc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&pBufferOutputGlobalScan));
		assert(SUCCEEDED(hResult));

		BumpPointer += ::PTG_Size_AlignUpFrom(Desc.Width, Desc.Alignment);
		assert(m_pDeviceCompute->GetResourceAllocationInfo(0X1U, 1U, &Desc).SizeInBytes == ::PTG_Size_AlignUpFrom(Desc.Width, Desc.Alignment));
	}

	UINT Size_DescriptorHandleIncrement_CBVSRVUAV = m_pDeviceCompute->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	ID3D12DescriptorHeap *pGPGPU_Scan_DescriptorPool_CBVSRVUAV;
	{
		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
		DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		DescriptorHeapDesc.NumDescriptors = 5U;
		DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DescriptorHeapDesc.NodeMask = 0X1U;

		hResult = m_pDeviceCompute->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&pGPGPU_Scan_DescriptorPool_CBVSRVUAV));
		assert(SUCCEEDED(hResult));
	}

	//WriteDescriptor
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R32_FLOAT;
		Desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3
		);
		Desc.Buffer.FirstElement = 0U;
		Desc.Buffer.NumElements = 2048U;
		Desc.Buffer.StructureByteStride = 0U;
		Desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle;
		DestDescriptorHandle.ptr = pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetCPUDescriptorHandleForHeapStart().ptr;

		m_pDeviceCompute->CreateShaderResourceView(pBufferInput, &Desc, DestDescriptorHandle);
	}

	//WriteDescriptor
	{
		//Null descriptors
		//https://msdn.microsoft.com/en-us/library/windows/desktop/mt709127(v=vs.85).aspx#Null_descriptors

		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R32_FLOAT;
		Desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3
		);
		Desc.Buffer.FirstElement = 0U;
		Desc.Buffer.NumElements = 512U;
		Desc.Buffer.StructureByteStride = 0U;
		Desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle;
		DestDescriptorHandle.ptr = pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetCPUDescriptorHandleForHeapStart().ptr + Size_DescriptorHandleIncrement_CBVSRVUAV;

		m_pDeviceCompute->CreateShaderResourceView(NULL, &Desc, DestDescriptorHandle);
	}

	//WriteDescriptor
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R32_FLOAT;
		Desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3
		);
		Desc.Buffer.FirstElement = 0U;
		Desc.Buffer.NumElements = 512U;
		Desc.Buffer.StructureByteStride = 0U;
		Desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorHandle;
		DestDescriptorHandle.ptr = pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetCPUDescriptorHandleForHeapStart().ptr + Size_DescriptorHandleIncrement_CBVSRVUAV * 2U;

		m_pDeviceCompute->CreateShaderResourceView(pBufferOutputPartialReduce, &Desc, DestDescriptorHandle);
	}

	//WriteDescriptor
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R32_FLOAT;
		Desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		Desc.Buffer.FirstElement = 0U;
		Desc.Buffer.NumElements = 512U;
		Desc.Buffer.StructureByteStride = 0U;
		Desc.Buffer.CounterOffsetInBytes = 0U;
		Desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_pDeviceCompute->CreateUnorderedAccessView(
			pBufferOutputPartialReduce, 
			NULL, 
			&Desc, 
			D3D12_CPU_DESCRIPTOR_HANDLE{ 
				pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetCPUDescriptorHandleForHeapStart().ptr 
				+ Size_DescriptorHandleIncrement_CBVSRVUAV * 3U
			}
		);
	}

	//WriteDescriptor
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R32_FLOAT;
		Desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		Desc.Buffer.FirstElement = 0U;
		Desc.Buffer.NumElements = 2048U;
		Desc.Buffer.StructureByteStride = 0U;
		Desc.Buffer.CounterOffsetInBytes = 0U;
		Desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_pDeviceCompute->CreateUnorderedAccessView(
			pBufferOutputGlobalScan,
			NULL,
			&Desc,
			D3D12_CPU_DESCRIPTOR_HANDLE{
				pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetCPUDescriptorHandleForHeapStart().ptr
				+ Size_DescriptorHandleIncrement_CBVSRVUAV * 4U
			}
		);
	}

	ID3D12RootSignature *pGPGPU_Scan_RootSignature;
	hResult = m_pDeviceCompute->CreateRootSignature(0X1U, g_Shader_GPGPU_Scan_RootSignature, sizeof(g_Shader_GPGPU_Scan_RootSignature), IID_PPV_ARGS(&pGPGPU_Scan_RootSignature));
	assert(SUCCEEDED(hResult));

	ID3D12PipelineState *pGPGPU_Scan_UpSweep_PipelineState;
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC Desc;
		Desc.pRootSignature = pGPGPU_Scan_RootSignature;
		Desc.CS.pShaderBytecode = g_Shader_GPGPU_Scan_UpSweep_ComputeShader;
		Desc.CS.BytecodeLength = sizeof(g_Shader_GPGPU_Scan_UpSweep_ComputeShader);
		Desc.NodeMask = 0X1U;
		Desc.CachedPSO.pCachedBlob = NULL;
		Desc.CachedPSO.CachedBlobSizeInBytes = 0U;
		Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		hResult = m_pDeviceCompute->CreateComputePipelineState(&Desc, IID_PPV_ARGS(&pGPGPU_Scan_UpSweep_PipelineState));
		assert(SUCCEEDED(hResult));
	}

	ID3D12PipelineState *pGPGPU_Scan_DownSweep_PipelineState;
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC Desc;
		Desc.pRootSignature = pGPGPU_Scan_RootSignature;
		Desc.CS.pShaderBytecode = g_Shader_GPGPU_Scan_DownSweep_ComputeShader;
		Desc.CS.BytecodeLength = sizeof(g_Shader_GPGPU_Scan_DownSweep_ComputeShader);
		Desc.NodeMask = 0X1U;
		Desc.CachedPSO.pCachedBlob = NULL;
		Desc.CachedPSO.CachedBlobSizeInBytes = 0U;
		Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		hResult = m_pDeviceCompute->CreateComputePipelineState(&Desc, IID_PPV_ARGS(&pGPGPU_Scan_DownSweep_PipelineState));
		assert(SUCCEEDED(hResult));
	}
	
	ID3D12CommandAllocator *pCommandPool;
	{
		hResult = m_pDeviceCompute->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&pCommandPool));
		assert(SUCCEEDED(hResult));
	}

	ID3D12Fence *pFence;
	hResult = m_pDeviceCompute->CreateFence(0U, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
	assert(SUCCEEDED(hResult));

	//Setting and Populating Descriptor Heaps
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899210(v=vs.85).aspx

	//DescriptorTable state is Undefined 
	//at the Beginning of a CommandList
	//and after DescriptorHeaps are Changed on a CommandList. 
	//Redundantly setting the same DescriptorHeap 
	//does not cause DescriptorTable settings to be Undefined.

	//When DescriptorHeaps are set (using ID3D12GraphicsCommandList::SetDescriptorHeaps), 
	//All the Heaps being used are Set in a Single call (and all previously Set Heaps are Unset by the call). 
	//At most One heap of Each Type listed above can be set in the call.

	void *pBufferOutputPartialReduceData;
	{
		//CacheCoherentUMA
		D3D12_RANGE ReadRange;
		ReadRange.Begin = 0U;
		ReadRange.End = 0U;
		hResult = pBufferOutputPartialReduce->Map(0U, &ReadRange, &pBufferOutputPartialReduceData);
		assert(SUCCEEDED(hResult));
	}

	//Common state promotion
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#Common_state_promotion
	
	//Any Resource in the COMMON state can be accessed
	//as though ( == as if) it were in a single state with in a single state with 1 WRITE flag, or 1 or more READ flags.
	
	//Resources can be Promoted from the COMMON state based on the following table:
	//...

	//State decay to common
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#state_decay_to_common
	
	//Resources that meet certain requirements are considered to be stateless 
	//and effectively return to the COMMON state 
	//when the GPU finishes execution of an ExecuteCommandLists operation.
	//Decay does not occur between CommandLists executed together in the same ExecuteCommandLists call.
	
	//Resources will decay to the COMMON state 
	//regardless of whether they were Explicitly Transitioned using resource Barriers or Implicitly Promoted.

	//The following resources will decay when an ExecuteCommandLists operation is completed on the GPU:
	//Resources being accessed on a Copy queue, or
	//Buffer resources on any queue type, or
	//Texture resources on any queue type that have the D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS flag set, or
	//Any resource implicitly promoted to a read - only state.

	for (int i = 0; i < 1000; ++i)
	{
		ID3D12GraphicsCommandList *pCommandBufferCompute;
		m_pDeviceCompute->CreateCommandList(0X1U, D3D12_COMMAND_LIST_TYPE_COMPUTE, pCommandPool, NULL, IID_PPV_ARGS(&pCommandBufferCompute));
		
		pCommandBufferCompute->SetComputeRootSignature(pGPGPU_Scan_RootSignature);
		pCommandBufferCompute->SetDescriptorHeaps(1U, &pGPGPU_Scan_DescriptorPool_CBVSRVUAV);
		
		//For pBufferInput
		pCommandBufferCompute->SetComputeRootDescriptorTable(0U, pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetGPUDescriptorHandleForHeapStart());
		
		//UpSweep
		pCommandBufferCompute->SetPipelineState(pGPGPU_Scan_UpSweep_PipelineState);
		//Null descriptors
		pCommandBufferCompute->SetComputeRootDescriptorTable(1U, D3D12_GPU_DESCRIPTOR_HANDLE{ pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetGPUDescriptorHandleForHeapStart().ptr + Size_DescriptorHandleIncrement_CBVSRVUAV * 1U });
		//For pBufferOutputPartialReduce
		pCommandBufferCompute->SetComputeRootDescriptorTable(2U, D3D12_GPU_DESCRIPTOR_HANDLE{ pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetGPUDescriptorHandleForHeapStart().ptr + Size_DescriptorHandleIncrement_CBVSRVUAV * 3U });
	   
		//Promote: Buffer: COMMON->SRV For pBufferInput
		//Promote: Buffer: COMMON->UAV For pBufferOutputPartialReduce
		pCommandBufferCompute->Dispatch(4U, 1U, 1U);

		//Barrier
		//UAV->SRV For pBufferOutputPartialReduce
		{
			D3D12_RESOURCE_BARRIER Barriers[1];
			Barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barriers[0].Transition.pResource = pBufferOutputPartialReduce;
			Barriers[0].Transition.Subresource = 0U;
			Barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			Barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			pCommandBufferCompute->ResourceBarrier(1U, Barriers);
		}

		//DownSweep
		pCommandBufferCompute->SetPipelineState(pGPGPU_Scan_DownSweep_PipelineState);
		//For pBufferOutputPartialReduce
		pCommandBufferCompute->SetComputeRootDescriptorTable(1U, D3D12_GPU_DESCRIPTOR_HANDLE{ pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetGPUDescriptorHandleForHeapStart().ptr + Size_DescriptorHandleIncrement_CBVSRVUAV * 2U });
		//For pBufferOutputGlobalScan
		pCommandBufferCompute->SetComputeRootDescriptorTable(2U, D3D12_GPU_DESCRIPTOR_HANDLE{ pGPGPU_Scan_DescriptorPool_CBVSRVUAV->GetGPUDescriptorHandleForHeapStart().ptr + Size_DescriptorHandleIncrement_CBVSRVUAV * 4U });

		//Promote: Buffer: COMMON->UAV For pBufferOutputGlobalScan
		pCommandBufferCompute->Dispatch(4U, 1U, 1U);

		hResult = pCommandBufferCompute->Close();
		assert(SUCCEEDED(hResult));

		//Decay: Buffer: SRV->COMMON For pBufferInput
		//Decay: Buffer: SRV->COMMON For pBufferOutputPartialReduce
		//Decay: Buffer: UAV->COMMON For pBufferOutputGlobalScan
		ID3D12CommandList *const pCommandLists[] = { pCommandBufferCompute };
		m_pDeviceCompute_QueueCompute->ExecuteCommandLists(1U, pCommandLists);

		m_pDeviceCompute_QueueCompute->Signal(pFence, 2U);

		//Wait
		UINT64 iFence;
		while ((iFence = pFence->GetCompletedValue())< 2U)
		{
			::PTS_Yield();
		}

		//WriteHost
		hResult = pFence->Signal(1U);
		assert(SUCCEEDED(hResult));

		float *Test = static_cast<float *>(pBufferOutputPartialReduceData);

		//�ᵼ��ͼ�ε���������Bug
		//(*Test) = 0.0f;

		IDXGISwapChain3 *pSwapChain;
		
		//SpinLock
		while ((pSwapChain = ::PTSAtomic_Get(&m_pDeviceGraphics_SwapChain)) == NULL)
		{
			::PTS_Yield();
		}


		//Notes on the required resource initialization
		//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899180(v=vs.85).aspx

		//While the active / inactive abstraction can be ignored in the advanced model, certain resource types still require initialization.Resources with either the render target or depth stencil flags must be initialized with either a clear operation or a collection of full subresource copies.If an aliasing barrier was used to denote the transition between two aliased resources, the initialization must occur after the aliasing barrier.This initialization is still required whenever a resource would've been activated in the simple model.

		//Placed and reserved resources with either the render target or depth stencil flags must be initialized with one of the following operations before other operations are supported.

		//A Clear operation; for example ClearRenderTargetView or ClearDepthStencilView.
		//A DiscardResource operation.
		//A Copy operation; for example CopyBufferRegion, CopyTextureRegion, or CopyResource.
		//Applications should prefer the most explicit operation that results in the least amount of texels modified.Consider the following examples.

		//Using a depth buffer to solve pixel visibility typically requires each depth texel start out at 1.0 or 0. Therefore, a Clear operation should be the most efficient option for aliased depth buffer initialization.
		//An application may use an aliased render target as a destination for tone mapping.Since the application will render over every pixel during the tone mapping, DiscardResource should be the most efficient option for initialization.

		//Present
		{
			DXGI_PRESENT_PARAMETERS PresentParameters;
			PresentParameters.DirtyRectsCount = 0U;
			PresentParameters.pDirtyRects = NULL;
			PresentParameters.pScrollRect = NULL;
			PresentParameters.pScrollOffset = NULL;
			hResult = pSwapChain->Present1(1U, 0U, &PresentParameters);
			assert(SUCCEEDED(hResult));
		}
		pSwapChain->GetCurrentBackBufferIndex();
		for (int i0 = 0; i0 < 1000; ++i0)
		{
			::PTS_Pause();
		}
	}

	int huhu = 0;
}

static PT3DInstanceImpl *s_3DInstanceD3D12_Singleton_Pointer = reinterpret_cast<PT3DInstanceImpl *>(uintptr_t(0X1U));//������Ϊ��Чֵ����Ϊ������Alignment


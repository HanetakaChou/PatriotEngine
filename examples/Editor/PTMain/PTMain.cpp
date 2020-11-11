#include "../../Runtime/Public/Application/PTApplicationMain.h"
#include "../../Runtime/Public/System/PTMemorySystem.h"
#include "../../Runtime/Public/System/PTFileSystem.h"
#include "../../Runtime/Public/System/PTExecutionSystem.h"

#include "../../Runtime/Public/Math/PTMath.h"

#include <dxgi1_4.h>
#include <d3d11.h>
#include <d3d12.h>

#include "PTShader_ReliefMappingVS.h"
#include "PTShader_ReliefMappingPS.h"

#include <DirectXMath.h>

extern "C" PTAPPLICATIONAPI uint32_t PTCALL PTApplicationMain(IPTWindow *pWindow)
{
	//逻辑开发入口点


	//PTSceneEditor使用套接字传给Application.dll 再调用IPTWindow::Parent_Set


	PTFileSystemStorage Temp;
	IPTFileSystem *pFileSystem = ::PTFileSystem_Construct(&Temp);

	char PathName[0X10000];
	pFileSystem->RootPath_Get(PathName, 0X10000);

	PTFileStorage FileStorage;

	IPTFile *pFile = pFileSystem->PTFile_Construct(&FileStorage, "huhuhu.txt", IPTFileSystem::ReadWrite);

	char Text[20];

	uint32_t huhu = pFile->Read(Text, 4);

	huhu = pFile->Read(Text, 4);

	pFile->Seek(IPTFile::End, 10);

	char Text1[20] = "--!!!!";

	huhu = pFile->Write(Text1, 4);

	pFile->Destruct();

	return 0;

#if 0
	pWindow->InputCallbackSet(
		[](void *pInputData, void *pUserData)->void
	{
		
	}, 
		NULL
		);

	PTHandleDisplay hDisplay;
	PTHandleWindow hWnd;
	pWindow->HandleGet(&hDisplay, &hWnd);


	IDXGIFactory4 *pDXGIFactory;
	::CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&pDXGIFactory));

	ID3D12Debug *pDebug;
	::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));
	pDebug->EnableDebugLayer();

	ID3D12Device *pDevice = NULL;

	for (UINT i = 0U; ; ++i)
	{
		IDXGIAdapter1 *pAdapter;
		if (FAILED(pDXGIFactory->EnumAdapters1(i, &pAdapter)))
		{
			break;
		}

		bool bBreak = false;

		do {
			DXGI_ADAPTER_DESC1 AdapterDesc;
			pAdapter->GetDesc1(&AdapterDesc);
			if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				break;
			}

			ID3D12Device *pDeviceTemp = NULL;
			if (FAILED(::D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDeviceTemp))))
			{
				break;
			}

			do {
				D3D12_FEATURE_DATA_ARCHITECTURE FeatureSupportData;
				FeatureSupportData.NodeIndex = 0U;//[In]
				pDeviceTemp->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &FeatureSupportData, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE));

				//独立显卡
				if (FeatureSupportData.UMA != TRUE)
				{
					//独立显卡优先
					if (pDevice != NULL)
					{
						pDevice->Release();
					}
					//Steal
					pDevice = pDeviceTemp;
					pDeviceTemp = NULL;
					bBreak = true;//迭代结束
				}

				//集成显卡
				if (FeatureSupportData.UMA == TRUE)
				{
					//主显卡优先
					if (pDevice == NULL)
					{
						//Steal
						pDevice = pDeviceTemp;
						pDeviceTemp = NULL;
					}
				}

			} while (false);

			if (pDeviceTemp != NULL)
			{
				pDeviceTemp->Release();
			}

		} while (false);

		pAdapter->Release();

		if (bBreak)
		{
			break;
		}
	}

	if (pDevice == NULL)
	{
		return 1;
	}

	ID3D12CommandQueue *pCommandQueueGraphics;
	{
		D3D12_COMMAND_QUEUE_DESC Desc;
		Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Desc.NodeMask = 0X1U;
		pDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&pCommandQueueGraphics));
	}

	uint32_t Width;
	uint32_t Height;
	uint32_t Layer;
	pWindow->SizeGet(&Width, &Height, &Layer);

	IDXGISwapChain1 *pSwapChainOld = NULL;
	{
		DXGI_SWAP_CHAIN_DESC1 Desc;
		Desc.Width = Width;
		Desc.Height = Height;
		Desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		Desc.Stereo = (Layer > 1U) ? TRUE : FALSE;
		Desc.SampleDesc.Count = 1U;
		Desc.SampleDesc.Quality = 0U;
		Desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.BufferCount = 2U;
		Desc.Scaling = DXGI_SCALING_STRETCH;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		Desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		Desc.Flags = 0U;
		pDXGIFactory->CreateSwapChainForCoreWindow(pCommandQueueGraphics, hWnd, &Desc, NULL, &pSwapChainOld);
	}

	IDXGISwapChain3 *pSwapChain = NULL;
	pSwapChainOld->QueryInterface(IID_PPV_ARGS(&pSwapChain));
	
	pSwapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020);

	ID3D12Resource *pImageColor[2];
	pSwapChain->GetBuffer(0U, IID_PPV_ARGS(&pImageColor[0]));
	pSwapChain->GetBuffer(1U, IID_PPV_ARGS(&pImageColor[1]));

	ID3D12DescriptorHeap *pFrameBufferColor;
	{
		D3D12_DESCRIPTOR_HEAP_DESC Desc;
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		Desc.NumDescriptors = 2U;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Desc.NodeMask = 0X1U;
		pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pFrameBufferColor));
	}

	{
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MipSlice = 0U;
		Desc.Texture2D.PlaneSlice = 0U;

		D3D12_CPU_DESCRIPTOR_HANDLE ImageViewColor;

		ImageViewColor = pFrameBufferColor->GetCPUDescriptorHandleForHeapStart();
		pDevice->CreateRenderTargetView(pImageColor[0], &Desc, ImageViewColor);//Write
		ImageViewColor.ptr += pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		pDevice->CreateRenderTargetView(pImageColor[1], &Desc, ImageViewColor);
	}

	ID3D12DescriptorHeap *pFrameBufferDepth;
	{
		D3D12_DESCRIPTOR_HEAP_DESC Desc;
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		Desc.NumDescriptors = 2U;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Desc.NodeMask = 0X1U;
		pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pFrameBufferDepth));
	}
	

	ID3D12CommandAllocator *pCommandPoolGraphics;
	{
		pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandPoolGraphics));
	}

	ID3D12GraphicsCommandList *pCommandBufferGraphics;
	{
		pDevice->CreateCommandList(0X1U, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandPoolGraphics, NULL, IID_PPV_ARGS(&pCommandBufferGraphics));
	}

	//提升

	//衰退

	D3D12_RESOURCE_BARRIER ImageMemoryBarrier[1];
	ImageMemoryBarrier->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ImageMemoryBarrier->Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ImageMemoryBarrier->Transition.pResource = pImageColor[0];
	ImageMemoryBarrier->Transition.Subresource = 0U;
	ImageMemoryBarrier->Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	//pCommandBufferGraphics->ResourceBarrier()

	while (true)
	{
		pSwapChain->Present(0U, 0U);
	}
#endif

#if 0
	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pImmediateContext = NULL;

	D3D_FEATURE_LEVEL FeatureLevels[1] = { D3D_FEATURE_LEVEL_11_0 };
	::D3D11CreateDevice(pDXGIAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG, FeatureLevels, 1U, D3D11_SDK_VERSION, &pDevice, NULL, &pImmediateContext);

	uint32_t Width;
	uint32_t Height;
	uint32_t Layer;
	pWindow->SizeGet(&bChanged, &Width, &Height, &Layer);
	IDXGISwapChain1 *pSwapChain = NULL;
	{
		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
		SwapChainDesc.Width = Width;
		SwapChainDesc.Height = Height;
		SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.Stereo = FALSE;
		SwapChainDesc.SampleDesc.Count = 1U;
		SwapChainDesc.SampleDesc.Quality = 0U;
		SwapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2U;
		SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		SwapChainDesc.Flags = 0U;
		pDXGIFactory->CreateSwapChainForCoreWindow(pDevice, hWnd, &SwapChainDesc, NULL, &pSwapChain);

	}

	ID3D11Texture2D *pSwapChainBuffer;
	pSwapChain->GetBuffer(0U, IID_PPV_ARGS(&pSwapChainBuffer));

	ID3D11RenderTargetView *pSwapChainBufferRTV;
	{
		D3D11_RENDER_TARGET_VIEW_DESC Desc;
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MipSlice = 0U;
		pDevice->CreateRenderTargetView(pSwapChainBuffer, &Desc, &pSwapChainBufferRTV);
	}
	
	ID3D11Buffer *pBufferTransform;
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.ByteWidth = sizeof(float) * 4U * 4U * 3U;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0U;
		BufferDesc.StructureByteStride = 0U;
		pDevice->CreateBuffer(&BufferDesc, NULL, &pBufferTransform);
	}

	ID3D11Buffer *pReliefMappingVB;
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.ByteWidth = sizeof(float) * 4U * 8U;
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0U;
		BufferDesc.MiscFlags = 0U;
		BufferDesc.StructureByteStride = 0U;
		
		D3D11_SUBRESOURCE_DATA InitialData;
		float VertexBuffer[4U * 12U] = {
			256.0f,256.0f,0.0f,1.0f,
			256.0f,256.0f,512.0f,1.0f,
			256.0f,-256.0f,0.0f,1.0f,
			256.0f,-256.0f,512.0f,1.0f,
			-256.0f,-256.0f,0.0f,1.0f,
			-256.0f,-256.0f,512.0f,1.0f,
			-256.0f,256.0f,0.0f,1.0f,
			-256.0f,256.0f,512.0f,1.0f,

		};
		InitialData.pSysMem = VertexBuffer;

		pDevice->CreateBuffer(&BufferDesc, &InitialData, &pReliefMappingVB);
	}

	ID3D11Buffer *pReliefMappingIB;
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.ByteWidth = sizeof(UINT32) * 9U;
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0U;
		BufferDesc.MiscFlags = 0U;
		BufferDesc.StructureByteStride = 0U;

		D3D11_SUBRESOURCE_DATA InitialData;
		UINT32 IndexBuffer[16] = {
			0U,1U,2U,3U,4U,5U,6U,7U,
			0XFFFFFFFFU,
		};
		InitialData.pSysMem = IndexBuffer;

		pDevice->CreateBuffer(&BufferDesc, &InitialData, &pReliefMappingIB);
	}

	ID3D11InputLayout *pReliefMappingIL;
	ID3D11VertexShader *pReliefMappingVS;
	{
		D3D11_INPUT_ELEMENT_DESC InputElementDescS[1];
		InputElementDescS[0].SemanticName = "Position";
		InputElementDescS[0].SemanticIndex = 0U;
		InputElementDescS[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		InputElementDescS[0].InputSlot = 0U;
		InputElementDescS[0].AlignedByteOffset = 0U;
		InputElementDescS[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		InputElementDescS[0].InstanceDataStepRate = 0U;
		pDevice->CreateInputLayout(InputElementDescS, 1U, g_PTShader_ReliefMappingVS, sizeof(g_PTShader_ReliefMappingVS), &pReliefMappingIL);

		pDevice->CreateVertexShader(g_PTShader_ReliefMappingVS, sizeof(g_PTShader_ReliefMappingVS), NULL, &pReliefMappingVS);
	}

	ID3D11PixelShader *pReliefMappingPS;
	pDevice->CreatePixelShader(g_PTShader_ReliefMappingPS, sizeof(g_PTShader_ReliefMappingPS), NULL, &pReliefMappingPS);

	ID3D11RasterizerState *pReliefMappingRS;
	{
		D3D11_RASTERIZER_DESC RasterizerDesc;
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		RasterizerDesc.FrontCounterClockwise = TRUE;
		RasterizerDesc.DepthBias = 0U;
		RasterizerDesc.DepthBiasClamp = 0.0f;
		RasterizerDesc.SlopeScaledDepthBias = 0.0f;
		RasterizerDesc.DepthClipEnable = TRUE;
		RasterizerDesc.ScissorEnable = FALSE;
		RasterizerDesc.MultisampleEnable = FALSE;
		RasterizerDesc.AntialiasedLineEnable = FALSE;
		pDevice->CreateRasterizerState(&RasterizerDesc, &pReliefMappingRS);
	}


	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pImmediateContext->Map(pBufferTransform, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &MappedResource);
	PTM44F *pTransform = static_cast<PTM44F *>(MappedResource.pData);
	
	pTransform[0].r[0].x = 1.0f;
	pTransform[0].r[0].y = 0.0f;
	pTransform[0].r[0].z = 0.0f;
	pTransform[0].r[0].w = 0.0f;
	pTransform[0].r[1].x = 0.0f;
	pTransform[0].r[1].y = 1.0f;
	pTransform[0].r[1].z = 0.0f;
	pTransform[0].r[1].w = 0.0f;
	pTransform[0].r[2].x = 0.0f;
	pTransform[0].r[2].y = 0.0f;
	pTransform[0].r[2].z = 1.0f;
	pTransform[0].r[2].w = 0.0f;
	pTransform[0].r[3].x = 0.0f;
	pTransform[0].r[3].y = 0.0f;
	pTransform[0].r[3].z = 0.0f;
	pTransform[0].r[3].w = 1.0f;

	PTV3F EyePosition = { -500.0f,-1250.0f,250.0f };
	PTV3F EyeDirection = { 0.5f,1.0f,-0.2f };
	PTV3F UpDirection = { 0.0f,0.0f,1.0f };

	PTSIMDMatrix mV = ::PTSIMDM44LookToRH(::PTSIMDV3FLoad(&EyePosition), ::PTSIMDV3FLoad(&EyeDirection), ::PTSIMDV3FLoad(&UpDirection));
	::PTSIMDM44FStoreA(&pTransform[1], mV);

	//PTSIMDMatrix mP = ::PTSIMDM44PerspectiveFovRH(0.69f, 1.2f, 2.0f, 5000.0f);
	PTSIMDMatrix mP = ::PTSIMDM44OrthographicRH(1000.0f, 1000.0f, 2.0f, 5000.0f);
	::PTSIMDM44FStoreA(&pTransform[2], mP);

	PTSIMDMatrix mVP = ::PTSIMDM44Multiply(mP, mV);

	PTSIMDFrustum F = ::PTSIMDFrustumLoadRH(mVP);

	PTV3F Center = { 0.0f,0.0f,0.0f };
	PTSIMDSphere S = ::PTSIMDSphereLoad(&Center, 50.0f);

	::PTSIMDFrustumSphereIntersect(F, S);

	PTV3F Min = { 0.0f,0.0f,0.0f };
	PTV3F Max = { 1.0f,1.0f,1.0f };
	PTSIMDAAB AAB = ::PTSIMDAABBLoad(&Min, &Max);

	::PTSIMDFrustumAABIntersect(F, AAB);

	pImmediateContext->Unmap(pBufferTransform, 0U);

	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pImmediateContext->IASetInputLayout(pReliefMappingIL);
	UINT Strides[1] = { sizeof(float) * 4U };
	UINT Offsets[1] = { 0U };
	pImmediateContext->IASetVertexBuffers(0U, 1U, &pReliefMappingVB, Strides, Offsets);
	pImmediateContext->IASetIndexBuffer(pReliefMappingIB, DXGI_FORMAT_R32_UINT, 0U);
	pImmediateContext->VSSetConstantBuffers(0U, 1U, &pBufferTransform);
	pImmediateContext->VSSetShader(pReliefMappingVS, NULL, 0U);
	pImmediateContext->RSSetState(pReliefMappingRS);
	D3D11_VIEWPORT Viewports[1];
	Viewports[0].TopLeftX = 0.0f;
	Viewports[0].TopLeftY = 0.0f;
	Viewports[0].Width = static_cast<float>(Width);
	Viewports[0].Height = static_cast<float>(Height);
	Viewports[0].MinDepth = 0.0f;
	Viewports[0].MaxDepth = 1.0f;
	pImmediateContext->RSSetViewports(1U, Viewports);
	pImmediateContext->PSSetShader(pReliefMappingPS, NULL, 0U);

	while (true)
	{
		pImmediateContext->OMSetRenderTargets(1U, &pSwapChainBufferRTV, NULL);
		pImmediateContext->DrawIndexedInstanced(8U, 1U,0U, 0U, 0U);
		pSwapChain->Present(0U, 0U);
	}

#endif

	//IPTHeap *pHeap = ::PTMemorySysem_Get()->HeapCreate(64ULL * 1024ULL * 1024ULL);
	
	//IPTDebugSystem *pDebugSystem = ::PTDebugSystem_Get();
	//pDebugSystem->Initialize();
	//pDebugSystem->OutputString("sssssss");

	//uint64_t ProcessAffinityMask;
	//uint64_t SystemAffinityMask;
	//::GetProcessAffinityMask(::GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask);

	//pHeap->Destroy();
	

	//DirectX::XMMatrixRotationRollPitchYaw()

	//auto huhuhu = sizeof(PTFrustum);
#if 0
	PTV4F Point = { 7.33f,13.0f,-45.0f,1.0f };
	
	PTSIMDMatrix PerspectiveTransform = ::PTSIMDM44PerspectiveFovRH(0.69f, 1.2f, 2.0f, 150.0f);
	PTSIMDVector vPoint = ::PTSIMDV4FTransform(PerspectiveTransform, ::PTSIMDV4FLoad(&Point));

	

	//DirectX::XMMATRIX test2 = DirectX::XMMatrixPerspectiveFovRH(0.69f, 1.2f, 2.0f, 3.0f);
	//DirectX::XMVECTOR test4 = DirectX::XMVector4Transform(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)&Point), test2);

	::PTSIMDV4FStore(&Point, vPoint);
	Point.x /= Point.w;
	Point.y /= Point.w;
	Point.z /= Point.w;
	Point.w /= Point.w;

	//像素着色器中读取的SV_POSITION是在视口变换之后进行

	PTM44F mPerspective;
	::PTSIMDM44FStoreA(&mPerspective, PerspectiveTransform);
	
	float fZ = mPerspective.r[2].w / (-mPerspective.r[2].z - Point.z);
	float fX = -fZ*Point.x / mPerspective.r[0].x;
	float fY = -fZ*Point.y / mPerspective.r[1].y;

	//DirectX::XMStoreFloat3A

	//DirectX::XMMatrixMultiply

	//::PTSIMDV4FTransform(PerspectiveTransform,)

	//XMVECTOR NearPlane = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, Near);
	//NearPlane = DirectX::Internal::XMPlaneTransform(NearPlane, vOrientation, vOrigin);
	//NearPlane = XMPlaneNormalize(NearPlane);

	//DirectX::XMMatrixPerspectiveFovRH

	//DirectX::BoundingFrustum aa;
	//aa.Contains(aa);

	//int huhu = 0;
#endif
	return 0;
}
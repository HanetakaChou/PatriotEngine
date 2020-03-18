#include "PTVDDevice3DImpl.h"

#include "../../../Public/McRT/PTSThread.h"

bool PTVDDevice3DImpl::Initilize()
{
	HRESULT hResult;

	//m_pCommandQueue3D
	{
		D3D12_COMMAND_QUEUE_DESC Desc;
		Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Desc.NodeMask = 0X1U;
		hResult = m_pDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&m_pCommandQueue3D));
		if (!(SUCCEEDED(hResult)))
		{
			return false;
		}
	}

	//m_pFence3D
	{
		hResult = m_pDevice->CreateFence(0U, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence3D));
		if (!(SUCCEEDED(hResult)))
		{
			return false;
		}
	}

	//m_FrameIndexCurrent
	m_FrameIndexCurrent = 0;

	//m_pCommandAllocator3D_S
	for (uint32_t i = 0U; i < m_FrameCount; ++i)
	{
		hResult = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator3D_S[i]));
		if (!(SUCCEEDED(hResult)))
		{
			return false;
		}
	}

	//m_pCommandList3D_S
	for (uint32_t i = 0U; i < m_FrameCount; ++i)
	{		
		hResult = m_pDevice->CreateCommandList(0X1U, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pCommandAllocator3D_S[i], NULL, IID_PPV_ARGS(&m_pCommandList3D_S[i]));
		if (!(SUCCEEDED(hResult)))
		{
			return false;
		}
	}

	//m_iFence3D_Allow_WriteToImmediateMemory
	m_iFence3D_Allow_WriteToImmediateMemory = 0U;

	//m_iFence3D_Allow_ResetCommandList
	m_iFence3D_Allow_ResetCommandList = 0U;

	//m_iFence3D_Allow_FreeBackBuffer
	m_iFence3D_Allow_FreeBackBuffer = 0U;

	//m_bSwapChain_Locked
	m_bSwapChain_Locked = 0U;

	//m_pSwapChain
	m_pSwapChain = NULL;

	//m_pBackBuffer_S
	for (uint32_t i = 0U; i < m_FrameCount; ++i)
	{
		hResult = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pBackBuffer_S[i]));
		if (!(SUCCEEDED(hResult)))
		{
			return false;
		}
	}

	return true;
}

#include "../../../Public/Window/PTWWindow.h"

void PTVDDevice3DImpl::WindowEventOutputCallback(void *pEventDataVoid)
{
	PT_WSI_IWindow::EventOutput *pEventDataGeneric = static_cast<PT_WSI_IWindow::EventOutput *>(pEventDataVoid);

	switch (pEventDataGeneric->m_Type)
	{
	case PT_WSI_IWindow::EventOutput::Type_WindowCreated:
	{
		PT_WSI_IWindow::EventOutput_WindowCreated *pEventDataSpecific = static_cast<PT_WSI_IWindow::EventOutput_WindowCreated *>(pEventDataGeneric);

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
		hResult = (*m_ppSwapChain_Factory)->CreateSwapChainForHwnd(m_pCommandQueue3D, pEventDataSpecific->m_hWindow, &Desc, NULL, NULL, &pSwapChain1);
		assert(SUCCEEDED(hResult));

		IDXGISwapChain3 *pSwapChain3;
		hResult = pSwapChain1->QueryInterface(IID_PPV_ARGS(&pSwapChain3));
		assert(SUCCEEDED(hResult));

		ID3D12Resource *pBackBuffer_S[m_FrameCount];
		for (uint32_t i = 0U; i < m_FrameCount; ++i)
		{
			hResult = pSwapChain3->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer_S[i]));
			assert(SUCCEEDED(hResult));
		}

		//Lock-Begin
		while (::PTSAtomic_CompareAndSet(&m_bSwapChain_Locked, 0U, 1U) != 0U)
		{
			//Write-HighPriority
			::PTS_Pause();
		}

		m_pSwapChain = pSwapChain3;

		for (uint32_t i = 0U; i < m_FrameCount; ++i)
		{
			m_pBackBuffer_S[i] = pBackBuffer_S[i];
		}

		//Lock-End
		::PTSAtomic_Set(&m_bSwapChain_Locked, 0U);
	}
	break;
	case PT_WSI_IWindow::EventOutput::Type_WindowResized:
	{
		PT_WSI_IWindow::EventOutput_WindowResized *pEventDataSpecific = static_cast<PT_WSI_IWindow::EventOutput_WindowResized *>(pEventDataGeneric);

		HRESULT hResult;

		//Lock-Begin
		while (::PTSAtomic_CompareAndSet(&m_bSwapChain_Locked, 0U, 1U) != 0U)
		{
			//Write-HighPriority
			::PTS_Pause();
		}

		//Wait GPU
		//The Lock Block The Command Submit The Current Frame Which So We Just Need To Wait The Last Frame
		UINT64 iFence3D_Allow_FreeBackBuffer = ::PTSAtomic_Get(&m_iFence3D_Allow_FreeBackBuffer);
		while (m_pFence3D->GetCompletedValue() < iFence3D_Allow_FreeBackBuffer)
		{
			::PTS_Yield();

		}

		//We Can Release After CPU Finish Write To Them
		for (uint32_t i = 0U; i < m_FrameIndexCurrent; ++i)
		{
			m_pBackBuffer_S[i]->Release();
		}

		//We Can Resize After We Release
		hResult = m_pSwapChain->ResizeBuffers(m_FrameCount, pEventDataSpecific->m_Width, pEventDataSpecific->m_Height, DXGI_FORMAT_R10G10B10A2_UNORM /*Rec.2020*/, 0U);
		assert(SUCCEEDED(hResult));

		for (uint32_t i = 0U; i < m_FrameCount; ++i)
		{
			hResult = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pBackBuffer_S[i]));
			assert(SUCCEEDED(hResult));
		}

		//Lock-End
		::PTSAtomic_Set(&m_bSwapChain_Locked, 0U);
	}
	break;
	case PT_WSI_IWindow::EventOutput::Type_WindowDestroyed:
	{

	}
	break;
	default:
		assert(0);
	}
}
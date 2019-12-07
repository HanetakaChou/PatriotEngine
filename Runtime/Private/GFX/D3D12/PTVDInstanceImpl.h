#ifndef PT_VIDEO_D3D12_INSTANCEIMPL_H
#define PT_VIDEO_D3D12_INSTANCEIMPL_H

#include "../../../Public/Video/PTVDInstance.h"

#include "PTVDDevice3DImpl.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <dxgi1_4.h>
#include <d3d12.h>

#include "../../../Public/McRT/PTSMemoryAllocator.h"

class PTVDInstanceImpl : public IPTVDInstance
{
	PTVDDevice3DImpl *m_pDevice3D;

public:
	inline PTVDInstanceImpl(
		PTVDDevice3DImpl *pDevice3D
	)
		:
		m_pDevice3D(pDevice3D)
	{

	}

	void Release() override;

	void Initialize() override;

	IPTVDDevice3D *GetDevice3D() override;
};





#endif
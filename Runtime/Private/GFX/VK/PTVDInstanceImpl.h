#ifndef PT_VIDEO_VULKAN_INSTANCEIMPL_H
#define PT_VIDEO_VULKAN_INSTANCEIMPL_H

#include "../../../Public/GFX/PTVDInstance.h"

#include "PTVDVulkanHeader.h"

#include "PTVDDevice.h"

#include "../../../Public/System/PTSMemoryAllocator.h"

class PTVDInstanceImpl : public IPTVDInstance
{
	VkInstanceWrapper m_hInstanceWrapper;

#ifndef NDEBUG
	VkDebugReportCallbackEXT m_hDebugReportCallback;
#endif

	//SOA(Struct Of Array)
	::PTS_CPP_Vector<::PTS_CPP_String> m_NUMADeviceName;
	::PTS_CPP_Vector<::IPTVD_Device *> m_NUMADevicePointer;
	::PTS_CPP_Vector<::PTS_CPP_String> m_UMADeviceName;
	::PTS_CPP_Vector<::IPTVD_Device *> m_UMADevicePointer;

	VkSurfaceKHR m_hSurface;
	
	IPTVD_Device *m_pDevice3D;
	PTVD_Context3D m_Context3D;

	IPTVD_Device *m_pDeviceCompute;

public:
	inline PTVDInstanceImpl(
		VkInstanceWrapper &hInstanceWrapper,
#ifndef NDEBUG
		VkDebugReportCallbackEXT hDebugReportCallback,
#endif
		::PTS_CPP_Vector<::PTS_CPP_String> &rNUMADeviceNameS,
		::PTS_CPP_Vector<::IPTVD_Device *> &rNUMADevicePointerS,
		::PTS_CPP_Vector<::PTS_CPP_String> &rUMADeviceNameS,
		::PTS_CPP_Vector<::IPTVD_Device *> &rUMADevicePointerS
	)
		:
		m_hInstanceWrapper(hInstanceWrapper),
#ifndef NDEBUG
		m_hDebugReportCallback(hDebugReportCallback),
#endif
		m_NUMADeviceName(std::move(rNUMADeviceNameS)),
		m_NUMADevicePointer(std::move(rNUMADevicePointerS)),
		m_UMADeviceName(std::move(rUMADeviceNameS)),
		m_UMADevicePointer(std::move(rUMADevicePointerS)),
		m_hSurface{ VK_NULL_HANDLE },
		m_pDevice3D(NULL),
		m_Context3D
	{
		0U,
	{ VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE },
	{ VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE },
	{ VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE },
	{ VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE },
	VK_NULL_HANDLE,
	0U,
	0U,
	VK_NULL_HANDLE,
	NULL,
	NULL,
	0U
	},
		m_pDeviceCompute(NULL)
	{

	}

	void Release() override;

#if 0
	char const *EnumNUMA(uint32_t Index) override;
	char const *EnumUMA(uint32_t Index) override;

	void Initialize() override;
	void Initialize(bool XD_IsNUMA, uint32_t XD_Index, bool Compute_IsGPU, bool Compute_IsNUMA, uint32_t Compute_Index) override;

	//PTWindow
	void WindowEventOutputCallback(
		void *pEventData
	) override;

	void Render(
		PTGCamera *pCamera,
		IPTVDXBar *pSceneTree,
		IPTGImage *pImageOut
	) override;
#endif

};

#endif
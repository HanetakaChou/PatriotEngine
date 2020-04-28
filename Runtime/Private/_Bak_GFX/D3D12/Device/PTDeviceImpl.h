#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <dxgi1_4.h>
#include <d3d12.h>

#include "../../../"


class PTGraphicDeviceImpl : public IPTGDevice
{
	inline ~PTGraphicDeviceImpl();

	void Destruct() override;

	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	void MemoryInitialize(
		//Critical //Scaled/Optional //Streaming
		uint64_t SizeMaxAssetSRV,
		uint64_t SizeMaxAssetVBVIBV,
		//Re-used 
		uint64_t SizeMaxFrameRTVDSVSRV,
		uint64_t SizeMaxFrameUAVSRV,
		uint64_t SizeMaxFrameCBV,
		//Streaming 
		uint64_t SizeMaxStreamCPSC
	) override;

	IDXGIFactory4 *m_RenderMain_pDXGIFactory;
	ID3D12Device *m_RenderMain_pDevice;
	IPTHeap *m_StreamMain_pHeap;
	IPTDeviceMemory *m_StreamMain_pDeviceMemory;
	PTSocket m_RenderMain_hSocket;
	PTSocket m_StreamMain_hSocket;
	PTThread m_RenderMain_hThread;
	
	IPTGMesh *MeshCreate(
		char const *pFileName,
		void (PTPTR *pVertexCountDecoder)(
			IPTFile *pFile,
			uint32_t *pVertexCountOut
			),
		void (PTPTR *pVertexDataDecoder)(
			IPTFile *pFile,
			PTFORMAT_R32G32B32_FLOAT PositionArrayOut[],
			PTFORMAT_R8G8B8A8_SNORM NormalArrayOut[],
			PTFORMAT_R8G8B8A8_SNORM TangentArrayOut[],
			PTFORMAT_R8G8B8A8_SNORM BiTangentArrayOut[],
			//PBR(Physically Based Rendering)
			PTFORMAT_R8G8_UNORM UVArrayOut_Normal[],
			PTFORMAT_R8G8_UNORM UVArrayOut_Albedo[],
			PTFORMAT_R8G8_UNORM UVArrayOut_ColorSpecular[],
			PTFORMAT_R8G8_UNORM UVArrayOut_Glossiness[]
			),
		void (PTPTR *pIndexCountDecoder)(
			IPTFile *pFile,
			uint32_t *pIndexCountOut
			),
		void (PTPTR *pIndexDataDecoder)(
			IPTFile *pFile,
			uint32_t IndexArrayOut[]
			)
	) override;

	IPTGMeshSkin *MeshSkinCreate(
		char const *pFileName,
		void (PTPTR *pVertexCountDecoder)(
			IPTFile *pFile,
			uint32_t *pVertexCountOut
			),
		void (PTPTR *pVertexDataDecoder)(
			IPTFile *pFile,
			PTFORMAT_R32G32B32_FLOAT PositionArrayOut[],
			PTFORMAT_R8G8B8A8_SNORM NormalArrayOut[],
			PTFORMAT_R8G8B8A8_SNORM TangentArrayOut[],
			PTFORMAT_R8G8B8A8_SNORM BiTangentArrayOut[],
			//PBR(Physically Based Rendering)
			PTFORMAT_R8G8_UNORM UVArrayOut_Normal[],
			PTFORMAT_R8G8_UNORM UVArrayOut_Albedo[],
			PTFORMAT_R8G8_UNORM UVArrayOut_Specular[],
			PTFORMAT_R8G8_UNORM UVArrayOut_Glossiness[],
			//Skin <-> VertexBlend
			PTFORMAT_R8G8B8A8_UINT BlendIndexArrayOut[],
			PTFORMAT_R8G8B8A8_UNORM BlendWeightArrayOut[]
			),
		void (PTPTR *pIndexCountDecoder)(
			IPTFile *pFile,
			uint32_t *pIndexCountOut
			),
		void (PTPTR *pIndexDataDecoder)(
			IPTFile *pFile,
			uint32_t IndexArrayOut[]
			),
		void (PTPTR *pJointCountDecoder)(
			IPTFile *pFile,
			uint32_t *pJointCountOut
			),
		void (PTPTR *pJointDataDecoder)(
			IPTFile *pFile,
			PTM44F JointArrayOut_InverseBind[]
			)
	) override;

	IPTGRender *RenderConstruct(PTRenderStorage *pStorage) override;
	IPTGRenderVR *RenderVRConstruct(PTRenderVRStorage *pStorage) override;

	//DeviceExecutionImpl

	//DeviceMemoryImpl NUMA_NCC UMA_CC
public:
	inline PTGraphicDeviceImpl(
		IDXGIFactory4 *RenderMain_pDXGIFactory,
		ID3D12Device *RenderMain_pDevice)
		:
		m_RenderMain_pDXGIFactory(RenderMain_pDXGIFactory),
		m_RenderMain_pDevice(RenderMain_pDevice)
	{

	}

	inline void ExecutionInitialize();

	inline ID3D12Resource *AssertLoadDDS(const void *pSrcData);

};

static_assert(alignof(PTGraphicDeviceImpl) == alignof(PTGDeviceStorage), "alignof(PTGraphicDeviceImpl) != alignof(PTGraphicDeviceStorage)");
//static_assert(sizeof(PTGraphicDeviceImpl) == sizeof(PTGDeviceStorage), "sizeof(PTGraphicDeviceImpl) != sizeof(PTGraphicDeviceStorage)");
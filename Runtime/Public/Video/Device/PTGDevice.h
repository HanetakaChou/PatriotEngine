#ifndef PT_GRAPHICS_DEVICE_DEVICE_H
#define PT_GRAPHICS_DEVICE_DEVICE_H

#include "../../PTCommon.h"
#include "../PTGCommon.h"
#include "../Asset/PTAssetMesh.h"
#include "../../Math/PTMath.h"
#include "../Render/PTGRender.h"

#include "../../System/PTFileSystem.h"
#include "../../System/PTMemorySystem.h"
#include "../../Window/PTWWindow.h"

struct IPTGDevice;
struct IPTGraphicResource;
struct IPTGraphicTexture2DArray;

enum class PTGraphicDevice_MemoryArchitecture :uint32_t
{
	UNKNOWN,
	NUMA_NCC,
	NUMA_CC,
	UMA_NCC,
	UMA_CC
};

struct IPTGDevice
{
	virtual void Destruct() = 0;

	inline void Destory(IPTHeap *pHeap)
	{
		this->Destruct();
		pHeap->AlignedFree(this);
	}

	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	virtual void MemoryInitialize(
		//Critical //Scaled/Optional //Streaming
		uint64_t SizeMaxAssetSRV,
		uint64_t SizeMaxAssetVBVIBV,
		//Re-used 
		uint64_t SizeMaxFrameRTVDSVSRV,
		uint64_t SizeMaxFrameUAVSRV,
		uint64_t SizeMaxFrameCBV,
		//Streaming 
		uint64_t SizeMaxStreamCPSC
	) = 0;
	
	//可能在另一个线程中执行
	//传入函数指针以支持并行编程，在某种意义上涉及到函数式编程的思想
	virtual IPTGMesh *MeshCreate(
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
	) = 0;

	//可能在另一个线程中执行
	virtual IPTGMeshSkin *MeshSkinCreate(
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
	) = 0;

	virtual IPTGRender *RenderConstruct(PTRenderStorage *pStorage) = 0;

	inline IPTGRender *RenderCreate(IPTHeap *pHeap)
	{
		PTRenderStorage *pStorage = static_cast<PTRenderStorage *>(pHeap->AlignedAlloc(sizeof(PTRenderStorage), alignof(PTRenderStorage)));
		return (pStorage != NULL) ? this->RenderConstruct(pStorage) : NULL;
	}

	virtual IPTGRenderVR *RenderVRConstruct(PTRenderVRStorage *pStorage) = 0;

	inline IPTGRenderVR *RenderVRCreate(IPTHeap *pHeap)
	{
		PTRenderVRStorage *pStorage = static_cast<PTRenderVRStorage *>(pHeap->AlignedAlloc(sizeof(PTRenderVRStorage), alignof(PTRenderVRStorage)));
		return (pStorage != NULL) ? this->RenderVRConstruct(pStorage) : NULL;
	}
};

struct alignas(alignof(uintptr_t)) PTGDeviceStorage { char _Pad[sizeof(uintptr_t)]; };
inline PTGDeviceStorage PTCALL PTGDevice_Alloc() { PTGDeviceStorage Temp{}; return Temp; };
extern "C" PTGRAPHICSAPI IPTGDevice * PTCALL PTGDevice_Construct(PTGDeviceStorage *Storage);
inline IPTGDevice * PTCALL PTGDevice_Create(IPTHeap *pHeap)
{
	PTGDeviceStorage *pStorage = static_cast<PTGDeviceStorage *>(pHeap->AlignedAlloc(sizeof(PTGDeviceStorage), alignof(PTGDeviceStorage)));
	return (pStorage != NULL) ? ::PTGDevice_Construct(pStorage) : NULL;
};

#endif
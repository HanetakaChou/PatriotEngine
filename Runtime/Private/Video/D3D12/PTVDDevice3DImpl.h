#ifndef PT_VIDEO_D3D12_DEVICE3DIMPL_H
#define PT_VIDEO_D3D12_DEVICE3DIMPL_H

#include "../../../Public/Video/PTVDInstance.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <dxgi1_4.h>
#include <d3d12.h>

class PTVDDevice3DImpl :public IPTVDDevice3D
{
	ID3D12Device * m_pDevice; //

	//↓Context3D -----------------------------------------
	//↓We Use Tripple Buffer Semantic
	static constexpr uint32_t const m_FrameCount = 3U;
	//↓Queue -----------------------------------------
	ID3D12CommandQueue *m_pCommandQueue3D;
	ID3D12Fence *m_pFence3D; //Fence与CommandQueue对应
	//↓Command ----------------------------------------
	uint32_t m_FrameIndexCurrent;
	ID3D12CommandAllocator *m_pCommandAllocator3D_S[m_FrameCount];
	ID3D12GraphicsCommandList *m_pCommandList3D_S[m_FrameCount];
	//Record Last Frame Value
	UINT64 m_iFence3D_Allow_WriteToImmediateMemory; //For GPU To Wait //GPU Should Wait Because Immediate Image(For exmaple the G-Buffer) Share Between Frame
	UINT64 m_iFence3D_Allow_ResetCommandList; //For CPU To Wait 
	UINT64 m_iFence3D_Allow_FreeBackBuffer; //For CPU To Wait
	//↓SwapChain ----------------------------------------
	IDXGIFactory4 * const * const m_ppSwapChain_Factory;
	uint32_t m_bSwapChain_Locked;
	IDXGISwapChain3 *m_pSwapChain; //In D3D Present Must Succeed Even Though The Size Does Not Match
	ID3D12Resource *m_pBackBuffer_S[m_FrameCount];

	//↓ContextCopy -----------------------------------------
	//UMA //NUMA

	//NUMA
	ID3D12CommandQueue *m_pCommandQueueCopy;
	ID3D12Heap *m_pHeapRingBuffer;

	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	ID3D12Heap *m_pHeapImmediate; //Re-used Resources

	//↓MemoryAllocator -----------------------------------------
	ID3D12Heap *m_pHeapImage;


	//PTWindow
	void WindowEventOutputCallback(
		void *pEventData
	) override;

	IPTVDAssertImage2D UploadImage2D(
		/* void *pData */
	) override;

	IPTVDAssetGeometryMesh *UploadGeometryMesh(
		PTVector4F *pArray_vPosition,

		//The Skin
		//ToDo: What About Morph??? //Cloth??? //Hair???
		//Skin的本质是Vertex在Joint的LocalSpace中的位置保持不变
		//正常情况下，动画中间件会将AnimatedPose×ReferencePose-1（特别地，当AnimatedPose==ReferencePose时，即为单位矩阵）写入，而无需应用程序显式地乘以ReferencePose-1
		void *pPoseUpdater /*Optional And Can Be NULL*/, void(*pSkeletonUpdaterCallback)(void *pPoseUpdater, PTQuaternionF *pArray_Pose_dq0, PTQuaternionF *pArray_Pose_dq1, float fDeltaTime /*Optimized For Culling Only Update When Visible*/) /*Optional And Can Be NULL*/,
		PTVector4UI *pArray_vJointIndex /*Optional And Can Be NULL*/, PTVector4F *pArray_vJointWeight /*Optional And Can Be NULL*/,

		//The TangentSpace And UV
		//The PatriotEngine Support 4 UVSet While The Unity3D BuiltIn Shader Only Support 2 UVSet
		PTQuaternionF *pArray_qTangent0, PTQuaternionF *pArray_qTangent1, PTQuaternionF *pArray_qTangent2, PTQuaternionF *pArray_qTangent3,
		PTVector2F *pArray_vUV0, PTVector2F *pArray_vUV1, PTVector2F *pArray_vUV2, PTVector2F *pArray_vUV3,

		//The PBR Material
		IPTVDAssertImage2D *pNormalMap /* Optional And Can Be NULL Default To (0,0,1) */,
		IPTVDAssertImage2D *pAlbedoMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pColorSpecularMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pGlossinessMap /* Optional And Can Be NULL Default To 1 */

		//SubSurfaceScatter
		//FaceWorks ???
	) override;

	IPTVDAssetGeometryTerrian *UploadGeometryTerrian(
		//At Most Time It Is A Quad By Convention
		PTVector4F *pArray_vPosition,

		//The TangentSpace And UV
		//The PatriotEngine Support 4 UVSet While The Unity3D BuiltIn Shader Only Support 2 UVSet
		PTQuaternionF *pArray_qTangent0, PTQuaternionF *pArray_qTangent1, PTQuaternionF *pArray_qTangent2, PTQuaternionF *pArray_qTangent3,
		PTVector2F *pArray_vUV0, PTVector2F *pArray_vUV1, PTVector2F *pArray_vUV2, PTVector2F *pArray_vUV3,

		//The PBR Material
		IPTVDAssertImage2D *pHeightMap /* Optional And Can Be NULL Default To (0,0,1) */,
		IPTVDAssertImage2D *pAlbedoMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pColorSpecularMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pGlossinessMap /* Optional And Can Be NULL Default To 1 */
	) override;

	void Render(
		IPTVDSceneTreeNodeCamera *pCameraForCull,
		IPTVDSceneTreeNodeCamera *pCameraForRender,
		IPTVDXBar *pSceneTree,
		IPTVDAssertImage2D *pImageOut //The Immediate Image //Engine Create It //We Can ReadBack (For Example In EditMode) Or Present It
	) override;

	void Present(
		IPTVDAssertImage2D *pImageToPresent
	) override;

public:
	inline PTVDDevice3DImpl(
		ID3D12Device *pDevice,
		IDXGIFactory4 * const *ppFactory
	)
		:
		m_pDevice(pDevice),
		m_ppSwapChain_Factory(ppFactory)
	{
		assert(pDevice != NULL);
	}

	bool Initilize();
};

#endif
#ifndef PT_VIDEO_INSTANCE_H
#define PT_VIDEO_INSTANCE_H

#include "../PTCommon.h"
#include "PTVDCommon.h"

#include "PTVDSceneTree.h"
#include "PTVDAsset.h"

struct IPTVDDevice3D;

struct IPTVDInstance
{
	//3D---------------------------------------
	virtual void Release() = 0;

	//Return NULL When End
	//可提供UI由用户选择
	//virtual char const *EnumNUMA(uint32_t Index) = 0;
	//virtual char const *EnumUMA(uint32_t Index) = 0;

	//按照以下默认策略
	//1:同时存在独显和集显（一般为Desktop游戏笔记本或Desktop台式机（需要在BIOS中启用核心显卡））
	//Device3D为独显
	//DeviceCompute为集显
	//2:只存在独显（一般为Desktop台式机）
	//Device3D为独显
	//DeviceCompute为NULL
	//3:只存在集显（一般为Desktop轻薄笔记本或Mobile）
	//Device3D为集显
	//DeviceCompute为NULL
	virtual void Initialize() = 0;

	//不允许3DDevice为NULL
	//virtual void Initialize(bool _3D_IsNUMA, uint32_t _3D_Index, bool Compute_IsGPU, bool Compute_IsNUMA, uint32_t Compute_Index) = 0;
	
	virtual IPTVDDevice3D *GetDevice3D() = 0;
};

extern "C" PTVDAPI IPTVDInstance * PTCALL PTVDInstance_ForProcess();

struct PTVD_Image_SubResource_Data
{
	void const *pData;
	uint32_t RowPitch;
	uint32_t SlicePath;
};

struct IPTVDDevice3D
{
	//PTWindow
	virtual void WindowEventOutputCallback(
		void *pEventData
	) = 0;

	//For All Load*** Methods Below, The RawData Can Be Safely Freed After The Method Return
	
	//逻辑层负责将数据加载到SystemMemory
	//图形引擎层并不关心SystemMemory中数据的来源（并不一定是磁盘，比如可以来自网络）

	//We Upload Data From The SystemMemory To The VideoMemory(The VideoMemory 特定于 Device)

	virtual IPTVDAssertImage2D UploadImage2D(
		/* void *pData */
	) = 0;

	//动画引擎层与图形引擎层是分离的
	//逻辑层完全可以选择使用第三方的动画中间件，比如Havok Animation/Behavior、Morpheme等

	//struct PTGAnimation_Joint
	//{
	//	char m_Name[32];
	//};

	virtual IPTVDAssetGeometryMesh *UploadGeometryMesh(
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
		IPTVDAssertImage2D *pNormalMap /* Optional And Can Be NULL Default To (0,0,1) */ ,
		IPTVDAssertImage2D *pAlbedoMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pColorSpecularMap /* Optional And Can Be NULL Default To (1,1,1) */ ,
		IPTVDAssertImage2D *pGlossinessMap /* Optional And Can Be NULL Default To 1 */

		//SubSurfaceScatter
		//FaceWorks ???
	) = 0;

	virtual IPTVDAssetGeometryTerrian *UploadGeometryTerrian(
		//At Most Time It Is A Quad By Convention
		PTVector4F *pArray_vPosition,

		//The TangentSpace And UV
		//The PatriotEngine Support 4 UVSet While The Unity3D BuiltIn Shader Only Support 2 UVSet
		PTQuaternionF *pArray_qTangent0, PTQuaternionF *pArray_qTangent1, PTQuaternionF *pArray_qTangent2, PTQuaternionF *pArray_qTangent3,
		PTVector2F *pArray_vUV0, PTVector2F *pArray_vUV1, PTVector2F *pArray_vUV2, PTVector2F *pArray_vUV3,

		//The PBR Material
		IPTVDAssertImage2D *pHeightMap /* Optional And Can Be NULL Default To (0,0,1) */ ,
		IPTVDAssertImage2D *pAlbedoMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pColorSpecularMap /* Optional And Can Be NULL Default To (1,1,1) */,
		IPTVDAssertImage2D *pGlossinessMap /* Optional And Can Be NULL Default To 1 */
	) = 0;

	//We Download(/ReadBack) Data From The VideoMemory To The SystemMemory

	virtual void Render(
		IPTVDSceneTreeNodeCamera *pCameraForCull,
		IPTVDSceneTreeNodeCamera *pCameraForRender,
		IPTVDXBar *pSceneTree,
		IPTVDAssertImage2D *pImageOut //The Immediate Image //Engine Create It //We Can ReadBack (For Example In EditMode) Or Present It
	) = 0;

	virtual void Present(
		IPTVDAssertImage2D *pImageToPresent
	) = 0;

};

#endif
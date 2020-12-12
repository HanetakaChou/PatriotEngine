#ifndef PT_VIDEO_SCENETREE_H
#define PT_VIDEO_SCENETREE_H

#include "../PTCommon.h"
#include "PTVDCommon.h"

#include "../Math/PTMath.h"
#include "PTVDAsset.h"

//无缝大地图由逻辑层的流式加载/卸载模块负责
//而与图形引擎层无关

//SceneGraph只负责管理“已加载到内存中的”场景
//由于SceneGraph中环的存在，需要转换为SceneTree才能用于渲染（实际上，应用程序不一定需要SceneGraph所提供的复杂的语义）
//------------------------------------------
//SceneGraph由逻辑层负责（如果需要）
//而图形引擎层只负责SceneTree
//NVIDIA SceniX
//https://developer.nvidia.com/scenix-download
//Markus Tavenrath, Christoph Kubisch. "Advanced Scenegraph Rendering Pipeline." GTC 2013.
//http ://on-demand.gputechconf.com/gtc/2013/presentations/S3032-Advanced-Scenegraph-Rendering-Pipeline.pdf
//Markus Tavenrath. "NvPro-Pipeline A Research Rendering Pipeline." GTC 2015.
//http ://on-demand.gputechconf.com/gtc/2015/presentation/S5148-Markus-Tavenrath.pdf
//https ://github.com/nvpro-pipeline/pipeline

//BVH（包围体层次，BoundingVolumeHierarchy）显然应该由图形引擎层负责
//图形引擎会使用并行的方式快速处理BVH的相关操作（构造、插入、删除）
//Christian Lauterbach, Michael Garland, Shubhabrata Sengupta, David Luebke, Dinesh Manocha. "Fast BVH Construction on GPUs." Eurographics 2009.
//http://research.nvidia.com/publication/fast-bvh-construction-gpus
//Tero Karras. "Maximizing Parallelism in the Construction of BVHs, Octrees, and k-d Trees." HPC 2012.
//http ://research.nvidia.com/publication/maximizing-parallelism-construction-bvhs-octrees-and-k-d-trees
//Tero Karras, Timo Aila. "Fast Parallel Construction of High-Quality Bounding Volume Hierarchies." HPC 2013.
//http ://research.nvidia.com/publication/fast-parallel-construction-high-quality-bounding-volume-hierarchies

#if 0
1.使用对偶四元数(DualQuaternion)取代矩阵，实现次世代的场景树(SceneTree)，参考文献：
1 - 1.Markus Tavenrath, Christoph Kubisch. "Advanced Scenegraph Rendering Pipeline." GTC 2013.
1 - 2.Markus Tavenrath. "NvPro-Pipeline A Research Rendering Pipeline." GTC 2015.
1 - 3.Ladislav Kavan, Steven Collins, Jiri Zara, Carol O'Sullivan. "Skinning with Dual Quaternions." I3D 2007. 
1 - 4.Ladislav Kavan, Steven Collins, Jiri Zara, Carol O'Sullivan. "Geometric Skinning with Approximate Dual Quaternion Blending." SIGGRAPH 2008. 
1 - 5.NVIDIA Direct3D SDK 10.5 Code Samples / Skinning with Dual Quaternions

2.实现次世代的BVH Construction 和Frustum Culling，参考文献：
2 - 1.Christian Lauterbach, Michael Garland, Shubhabrata Sengupta, David Luebke, Dinesh Manocha. "Fast BVH Construction on GPUs." Eurographics 2009.
2 - 2.Tero Karras. "Maximizing Parallelism in the Construction of BVHs, Octrees, and k-d Trees." HPC 2012.
2 - 3.Tero Karras, Timo Aila. "Fast Parallel Construction of High-Quality Bounding Volume Hierarchies." HPC 2013.
#endif

struct IPTVDSceneTreeNode
{
	virtual void *Interface_Get(uint32_t InterfaceType) = 0;

	// Huffman Coding
	// For Lowest Bit To Highest Bit

	// 3                  | 2              | 1           | 0
	//                    | 0 TransformTR  | 0 Transform | 0 Internal
	//                    | 1 TransformS   |
	//----------------------------------------------------
	//                                     | 1 LOD 
	//----------------------------------------------------------------
	// 0 Mesh             | 00 Geometry                  | 1 Leaf
	// 1 Terrain          | 00 Geometry                  | 1 Leaf
	//----------------------------------------------------
	// 0 LightDirectional | 01 Light
	//----------------------------------------------------
	//                    | 10 Camera

	enum :uint32_t
	{
		ClassType_Internal = 0U,
		ClassType_Transform = 0U,
		ClassType_TransformTR = 0U,
		ClassType_TransformS = 4U,
		ClassType_LOD = 2U,
		ClassType_Leaf = 1U,
		ClassType_Geometry = 1U,
		ClassType_Mesh = 1U,
		ClassType_Light = 3U,
		ClassType_LightDirectional = 5U
	};
	virtual uint32_t ClassType_Get() = 0;
};

struct IPTVDSceneTreeNodeInternal :public IPTVDSceneTreeNode
{
	virtual IPTVDSceneTreeNode *Child_Get(uint32_t IndexChild) = 0;
};

struct IPTVDSceneTreeNodeTransform :public IPTVDSceneTreeNodeInternal
{
	
};

struct IPTVDSceneTreeNodeTransformTR :public IPTVDSceneTreeNodeTransform
{
	virtual void TranslateAndRotation_Get(/*Dual Quaternion For TAndR*/) = 0;
};

struct IPTVDSceneTreeNodeTransformS :public IPTVDSceneTreeNodeTransform
{
	virtual void Scale_Get(/*Vector3 For S*/) = 0;
};

struct IPTVDSceneTreeNodeSwitch :public IPTVDSceneTreeNodeInternal
{
	//To Implement The LOD
	virtual void IndexActive_Get() = 0;
};

struct IPTVDSceneTreeNodeLeaf :public IPTVDSceneTreeNode
{

};

struct IPTVDSceneTreeNodeGeometry :public IPTVDSceneTreeNodeLeaf
{

};

struct IPTVDSceneTreeNodeGeometryMesh :public IPTVDSceneTreeNodeGeometry
{
	virtual IPTVDAssetGeometryMesh *Asset_Get() const = 0;
};

struct IPTVDSceneTreeNodeGeometryTerrain :public IPTVDSceneTreeNodeGeometry
{

};

struct IPTVDSceneTreeNodeLight :public IPTVDSceneTreeNodeLeaf
{

};

struct IPTVDSceneTreeNodeLightDirectional :public IPTVDSceneTreeNodeLight
{

};

struct IPTVDSceneTreeNodeCamera :public IPTVDSceneTreeNodeLeaf
{

};

struct IPTVDSceneTreeNodeFactory
{
	virtual IPTVDSceneTreeNodeTransform * TransformTR_Allocate(/*Dual Quaternion For TAndR*/) = 0;
	//virtual void NodeChildAdd(IPTVDSceneTreeNodeInternal *pNodeParent, IPTVDSceneTreeNode *pNodeChild) = 0;
	//virtual void NodeChildRemove(IPTVDSceneTreeNodeInternal *pNodeParent, IPTVDSceneTreeNode *pNodeChild) = 0;
};

//XBar(CrossBar)
//Flat List Point To Nodes Of Various Types 

struct IPTVDXBar
{
	virtual IPTVDSceneTreeNodeInternal *SceneTree_Root() = 0;

	//Write Operation Below
	//Sync To The Flat List

	virtual void NodeInternal_ChildAdd(IPTVDSceneTreeNodeInternal *pNodeParent, IPTVDSceneTreeNode *pNodeChild) = 0;
	virtual void NodeInternal_ChildRemove(IPTVDSceneTreeNodeInternal *pNodeParent, IPTVDSceneTreeNode *pNodeChild) = 0;

	virtual void TransformTR_TranslateAndRotation_Set(IPTVDSceneTreeNodeTransformTR *pNode, PTDualQuaternionF *pDualQuaternionTR) = 0;
	virtual void TransformS_Scale_Set(IPTVDSceneTreeNodeTransformS *pNode, PTVector3F *pVectorScale) = 0;

	virtual void GeometryMesh_Asset_Set(IPTVDSceneTreeNodeGeometryMesh *pMesh, IPTVDAssetGeometryMesh *pAsset) = 0;

	virtual void Camera_Set(IPTVDSceneTreeNodeCamera *pCamera, float m_FovAngleY, float m_AspectHByW, float m_NearZ, float m_FarZ) = 0;
};

#endif
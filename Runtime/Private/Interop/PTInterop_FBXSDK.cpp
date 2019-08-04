#include <fbxsdk.h>

#include "../../Public/PTCommon.h"
#include "../../Public/System/PTSFile.h"
#include "../../Public/System/PTSThread.h"
#include "../../Public/System/PTSMemoryAllocator.h"
#include "../../Public/3D/PT3DSceneGraph.h"
#include "../../Public/Math/PTMath.h"
#include <assert.h>
#include <float.h>
#include <string.h>
#include <vector>
#include <map>
#include <functional>

static inline fbxsdk::FbxManager * PTI_FBXSDK_FbxManager_Singleton()
{
	static fbxsdk::FbxManager *s_FbxManager_Singleton_Pointer = NULL;
	static int32_t s_FbxManager_Initialize_RefCount = 0;

	fbxsdk::FbxManager *pFbxManager = ::PTSAtomic_Get(&s_FbxManager_Singleton_Pointer);

	if ((pFbxManager == NULL) && (::PTSAtomic_GetAndAdd(&s_FbxManager_Initialize_RefCount, 1) == 0))
	{
		::PTSMemoryAllocator_Initialize();

		fbxsdk::FbxSetMallocHandler([](size_t Size)->void* {
			void *pVoid = ::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(Size));
			assert(pVoid != NULL);
			return pVoid;
		});
		fbxsdk::FbxSetCallocHandler([](size_t NMemb, size_t Size)->void* {
			void *pVoid = ::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(Size*NMemb));
			::memset(pVoid, 0, Size*NMemb);
			assert(pVoid != NULL);
			return pVoid;
		});
		fbxsdk::FbxSetReallocHandler([](void *pVoidOld, size_t Size)->void* {
			void *pVoidNew = ::PTSMemoryAllocator_Realloc(pVoidOld, static_cast<uint32_t>(Size));
			assert(pVoidNew != NULL);
			return pVoidNew;
		});
		fbxsdk::FbxSetFreeHandler([](void *pVoid)->void {
			::PTSMemoryAllocator_Free(pVoid);
		});

		::PTSAtomic_Set(&s_FbxManager_Singleton_Pointer, fbxsdk::FbxManager::Create());
		assert(::PTSAtomic_Get(&s_FbxManager_Singleton_Pointer) != NULL);
	}

	//Wait
	do
	{
		pFbxManager = ::PTSAtomic_Get(&s_FbxManager_Singleton_Pointer);
	} while (pFbxManager == NULL);

	return pFbxManager;
};

class PTI_FBXSDK_FBXInputStream : public fbxsdk::FbxStream
{
	void *m_Buffer;
	long m_Size;
	int m_ReaderID;
	fbxsdk::FbxStream::EState m_State;
	int m_Error;
	mutable long m_Position;
public:
	inline PTI_FBXSDK_FBXInputStream(void *Buffer, long Size, int pFileFormat)
		:
		m_Buffer(Buffer),
		m_Size(Size),
		m_ReaderID(pFileFormat),
		m_State(fbxsdk::FbxStream::eEmpty),
		m_Error(0),
		m_Position(0)
	{

	}
	fbxsdk::FbxStream::EState GetState() override
	{
		return m_State;
	}
	bool Open(void *) override
	{
		m_State = fbxsdk::FbxStream::eOpen;		
		m_Position = 0;
		return true;
	}
	bool Close() override
	{
		m_State = fbxsdk::FbxStream::eClosed;	
		m_Position = 0;
		return true;
	}
	bool Flush() override
	{
		assert(0);
		return false;
	}
	int Write(const void *pData, int Size) override
	{
		assert(0);
		return -1;
	}
	int Read(void *pData, int Size) const override
	{
		if ((m_Position >= 0) && (m_Position <= m_Size))
		{
			int cbLeft = (m_Size - m_Position);
			int cbRead = (Size <= cbLeft) ? Size : cbLeft;
			::memcpy(pData, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(m_Buffer) + static_cast<uintptr_t>(m_Position)), cbRead);
			m_Position += cbRead;
			return cbRead;
		}
		else
		{
			assert(0);
			return -1;
		}
	}
	int GetReaderID() const override
	{
		return m_ReaderID;
	}
	int GetWriterID() const override
	{
		assert(0);
		return -1;
	}
	void Seek(const fbxsdk::FbxInt64 &pOffset_Int64, const fbxsdk::FbxFile::ESeekPos &pSeekPos) override
	{
		assert((pOffset_Int64 >= fbxsdk::FbxInt64{ -0X80000000LL }) && (pOffset_Int64 <= fbxsdk::FbxInt64{ 0X7FFFFFFF }));
		long pOffset = static_cast<long>(pOffset_Int64);

		switch (pSeekPos)
		{
		case fbxsdk::FbxFile::eBegin:
		{
			m_Position = pOffset;
		}
		break;
		case fbxsdk::FbxFile::eCurrent:
		{
			m_Position = m_Position + pOffset;
		}
		break;
		case fbxsdk::FbxFile::eEnd:
		{
			m_Position = m_Size + pOffset;
		}
		break;
		default:
			assert(0);
			m_Error = 22; //EINVAL
		}

		if ((m_Position < 0) || (m_Position > m_Size))
		{
			assert(0);
			m_Error = 22; //EINVAL
		}
	}
	long GetPosition() const override
	{
		return m_Position;
	}
	void SetPosition(long pPosition) override
	{
		m_Position = pPosition;
		if ((m_Position < 0) || (m_Position > m_Size))
		{
			assert(0);
			m_Error = 22; //EINVAL
		}
	}
	int GetError() const override
	{
		return m_Error;
	}
	void ClearError() override
	{
		m_Error = 0;
	}
};

struct PTGAnimation_Joint_Intermediate_FBX
{
	PTGAnimation_Joint m_Joint;
	uint32_t m_Index_Final;
};

struct PTGRender_MeshSkeletal_CPP
{
	std::vector<PTGRender_MeshSkeletalVertex, ::PTS_CPP_Allocator<PTGRender_MeshSkeletalVertex>> m_VertexArray;
	std::vector<uint32_t, ::PTS_CPP_Allocator<uint32_t>> m_IndexArray;
};

static inline void PTI_FBXSDK_NodeProcess_Skeleton(std::map<fbxsdk::FbxNode *, PTGAnimation_Joint_Intermediate_FBX, std::less<fbxsdk::FbxNode *>, ::PTS_CPP_Allocator<std::pair<fbxsdk::FbxNode const *, PTGAnimation_Joint_Intermediate_FBX>>> &rSkeloton_Intermediate_FBX, fbxsdk::FbxNode *pNodeToProcess)
{
	int CountAttribute = pNodeToProcess->GetNodeAttributeCount();
	if (CountAttribute != 0)
	{
		assert(CountAttribute == 1);
		assert(pNodeToProcess->GetDefaultNodeAttributeIndex() == 0);
		fbxsdk::FbxNodeAttribute *pNodeAttribute = pNodeToProcess->GetNodeAttributeByIndex(0);
		assert(pNodeAttribute != NULL);

		fbxsdk::FbxNodeAttribute::EType AttributeType = pNodeAttribute->GetAttributeType();
		switch (AttributeType)
		{
		case fbxsdk::FbxNodeAttribute::eSkeleton:
		{
			char const *pJointName = pNodeToProcess->GetName();
			assert(pNodeToProcess->GetNodeAttributeCount() == 1U);
			assert(pNodeToProcess->GetDefaultNodeAttributeIndex() == 0);
			assert(pNodeToProcess->GetNodeAttributeByIndex(0)->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton);
			//TheNameOfJoint��Node�϶���Skeleton��
			assert(::strlen(pNodeToProcess->GetSkeleton()->GetName()) == 0U);

			PTGAnimation_Joint_Intermediate_FBX JointToInsert;
			JointToInsert.m_Index_Final = static_cast<uint32_t>(rSkeloton_Intermediate_FBX.size());
			size_t JointNameLength = ::strlen(pJointName);
			JointNameLength = JointNameLength < 31U ? JointNameLength : 31U;
			assert(sizeof(char) == 1U);
			::PTS_MemoryCopy(JointToInsert.m_Joint.m_Name, pJointName, JointNameLength);
			JointToInsert.m_Joint.m_Name[JointNameLength] = '\0';

			rSkeloton_Intermediate_FBX.emplace(pNodeToProcess, JointToInsert);
		}
		break;
		case fbxsdk::FbxNodeAttribute::eMesh:
		case fbxsdk::FbxNodeAttribute::ePatch:
		case fbxsdk::FbxNodeAttribute::eNurbs:
		case fbxsdk::FbxNodeAttribute::eNurbsSurface:
		case fbxsdk::FbxNodeAttribute::eNull:
			break;
		default:
			assert(0);
		}
	}
};

static inline void PTI_FBXSDK_NodeProcess_Mesh(std::map<fbxsdk::FbxNode *, PTGAnimation_Joint_Intermediate_FBX, std::less<fbxsdk::FbxNode *>, ::PTS_CPP_Allocator<std::pair<fbxsdk::FbxNode const *, PTGAnimation_Joint_Intermediate_FBX>>> &rSkeleton_Intermediate_FBX, std::vector<PTGRender_MeshSkeletal_CPP, ::PTS_CPP_Allocator<PTGRender_MeshSkeletal_CPP>> &rMeshArray_Final, fbxsdk::FbxNode *pNodeToProcess)
{
	int CountAttribute = pNodeToProcess->GetNodeAttributeCount();
	if (CountAttribute != 0)
	{
		assert(CountAttribute == 1);
		assert(pNodeToProcess->GetDefaultNodeAttributeIndex() == 0);
		fbxsdk::FbxNodeAttribute *pNodeAttribute = pNodeToProcess->GetNodeAttributeByIndex(0);
		assert(pNodeAttribute != NULL);

		fbxsdk::FbxNodeAttribute::EType AttributeType = pNodeAttribute->GetAttributeType();
		switch (AttributeType)
		{
		case fbxsdk::FbxNodeAttribute::eMesh:
		case fbxsdk::FbxNodeAttribute::ePatch:
		case fbxsdk::FbxNodeAttribute::eNurbs:
		case fbxsdk::FbxNodeAttribute::eNurbsSurface:
		{
			fbxsdk::FbxMesh *pMesh;
			if (AttributeType == fbxsdk::FbxNodeAttribute::eMesh && static_cast<fbxsdk::FbxMesh *>(pNodeAttribute)->IsTriangleMesh())
			{
				pMesh = static_cast<fbxsdk::FbxMesh *>(pNodeAttribute);
			}
			else
			{
				//Warning
				fbxsdk::FbxGeometryConverter fbxGeometryConverter(::PTI_FBXSDK_FbxManager_Singleton());
				pMesh = static_cast<fbxsdk::FbxMesh *>(fbxGeometryConverter.Triangulate(pNodeAttribute, true, false));
				assert(pMesh != NULL);
				assert(pMesh->GetClassId().Is(fbxsdk::FbxMesh::ClassId));
				assert(pNodeToProcess->GetNodeAttributeCount() == 1);
			}

			//FBX //ControlPoint //SkinIndexAndWeight

			struct FBXControlPoint_SkinIndexAndWeight
			{
				PTVector4UI m_SkinIndex;
				PTVector4F m_SkinWeight;
			};

			std::vector<
				FBXControlPoint_SkinIndexAndWeight,
				::PTS_CPP_Allocator<FBXControlPoint_SkinIndexAndWeight>
			> FBXControlPointArray_SkinIndexAndWeight(pMesh->GetControlPointsCount());

			int CountDeformer = pMesh->GetDeformerCount();
			if (CountDeformer != 0)
			{
				assert(CountDeformer == 1);
				fbxsdk::FbxDeformer *pDeformer = pMesh->GetDeformer(0, NULL);
				assert(pDeformer != NULL);
				assert(pDeformer->GetDeformerType() == fbxsdk::FbxDeformer::eSkin);

				fbxsdk::FbxSkin *pSkin = static_cast<fbxsdk::FbxSkin *>(pDeformer);
				int CountJoint = pSkin->GetClusterCount();
				for (int IndexJoint = 0; IndexJoint < CountJoint; ++IndexJoint)
				{
					fbxsdk::FbxCluster *pCluster = pSkin->GetCluster(IndexJoint);

					//fbxsdk::FbxCluster::GetTransformLinkMatrix //GetBindPose

					fbxsdk::FbxNode *pNodeJoint = pCluster->GetLink();
					char const *pJointName = pNodeJoint->GetName();
					assert(pNodeJoint->GetNodeAttributeCount() == 1U);
					assert(pNodeJoint->GetDefaultNodeAttributeIndex() == 0);
					assert(pNodeJoint->GetNodeAttributeByIndex(0)->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton);
					//TheNameOfJoint��Node�϶���Skeleton��
					assert(::strlen(pNodeJoint->GetSkeleton()->GetName()) == 0U);

					uint32_t JointInSkeleton_Index;

					auto pJointInSkeletonPair = rSkeleton_Intermediate_FBX.find(pNodeJoint);
					if (pJointInSkeletonPair != rSkeleton_Intermediate_FBX.end())
					{
						JointInSkeleton_Index = pJointInSkeletonPair->second.m_Index_Final;
					}
					else
					{
						//Node Not Marked As Joint Worked As Joint
						assert(0);

						PTGAnimation_Joint_Intermediate_FBX JointToInsert;
						JointToInsert.m_Index_Final = static_cast<uint32_t>(rSkeleton_Intermediate_FBX.size());
						size_t JointNameLength = ::strlen(pJointName);
						JointNameLength = JointNameLength < 31U ? JointNameLength : 31U;
						assert(sizeof(char) == 1U);
						::PTS_MemoryCopy(JointToInsert.m_Joint.m_Name, pJointName, JointNameLength);
						JointToInsert.m_Joint.m_Name[JointNameLength] = '\0';

						rSkeleton_Intermediate_FBX.emplace(pNodeJoint, JointToInsert);

						JointInSkeleton_Index = JointToInsert.m_Index_Final;
					}

					int CountIndexControlPoint = pCluster->GetControlPointIndicesCount();
					for (int IndexIndexControlPoint = 0; IndexIndexControlPoint < CountIndexControlPoint; ++IndexIndexControlPoint)
					{
						int IndexControlPoint = pCluster->GetControlPointIndices()[IndexIndexControlPoint];
						float Weight = static_cast<float>(pCluster->GetControlPointWeights()[IndexIndexControlPoint]);

						assert(static_cast<size_t>(IndexControlPoint) < FBXControlPointArray_SkinIndexAndWeight.size());
						FBXControlPoint_SkinIndexAndWeight &rControlPoint_SkinIndexAndWeight = FBXControlPointArray_SkinIndexAndWeight[IndexControlPoint];

						int IndexVectorWeightMin = 0;
						for (int IndexVector = 1; IndexVector < 4; ++IndexVector)
						{
							if ((&rControlPoint_SkinIndexAndWeight.m_SkinWeight.x)[IndexVector] < (&rControlPoint_SkinIndexAndWeight.m_SkinWeight.x)[IndexVectorWeightMin])
							{
								IndexVectorWeightMin = IndexVector;
							}
						}

						if ((&rControlPoint_SkinIndexAndWeight.m_SkinWeight.x)[IndexVectorWeightMin] < Weight)
						{
							(&rControlPoint_SkinIndexAndWeight.m_SkinIndex.x)[IndexVectorWeightMin] = JointInSkeleton_Index;
							(&rControlPoint_SkinIndexAndWeight.m_SkinWeight.x)[IndexVectorWeightMin] = Weight;
						}
					}
				}
			}

			//VertexAttribute

			//����MaterialID�ָ�Mesh
			fbxsdk::FbxGeometryElementMaterial *pMaterialIDArray = NULL;
			int(*pMaterialIDArrayAccess)(fbxsdk::FbxGeometryElementMaterial *pMaterialIDArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint) = NULL;

			int CountMeterialID = pMesh->GetElementMaterialCount();
			if (CountMeterialID != 0)
			{
				assert(CountMeterialID == 1);
				pMaterialIDArray = pMesh->GetLayer(0)->GetMaterials();
				assert(pMesh->GetLayerCount() == 1 || pMesh->GetLayer(1)->GetMaterials() == NULL);
				assert(pMaterialIDArray->GetReferenceMode() == fbxsdk::FbxGeometryElementMaterial::eIndexToDirect);

				switch (pMaterialIDArray->GetMappingMode())
				{
				case fbxsdk::FbxGeometryElementMaterial::eByPolygon:
				{
					assert(pMaterialIDArrayAccess == NULL);
					pMaterialIDArrayAccess = [](fbxsdk::FbxGeometryElementMaterial *pMaterialIDArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->int
					{
						return pMaterialIDArray->GetIndexArray().GetAt(PolygonIndex);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementMaterial::eAllSame:
				{
					assert(pMaterialIDArrayAccess == NULL);
					pMaterialIDArrayAccess = [](fbxsdk::FbxGeometryElementMaterial *pMaterialIDArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->int
					{
						return pMaterialIDArray->GetIndexArray().GetAt(0);
					};
				}
				break;
				//��Maya��eAllSame
				//��3dsMax��eByPolygon
				//������MappingMode��ʵ������Ӧ������
				assert(0);
				case fbxsdk::FbxGeometryElementMaterial::eByControlPoint:
				{
					assert(pMaterialIDArrayAccess == NULL);
					pMaterialIDArrayAccess = [](fbxsdk::FbxGeometryElementMaterial *pMaterialIDArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->int
					{
						return pMaterialIDArray->GetIndexArray().GetAt(IndexControlPoint);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementMaterial::eByPolygonVertex:
				{
					assert(pMaterialIDArrayAccess == NULL);
					pMaterialIDArrayAccess = [](fbxsdk::FbxGeometryElementMaterial *pMaterialIDArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->int
					{
						return pMaterialIDArray->GetIndexArray().GetAt(IndexPolygonVertex);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementMaterial::eByEdge:
					//This is meant to be used with smoothing layer elements.
					break;
				case fbxsdk::FbxGeometryElementMaterial::eNone:
					break;
				}
			}
			else
			{
				pMaterialIDArrayAccess = [](fbxsdk::FbxGeometryElementMaterial *, int, int, int)->int
				{
					return 0;
				};
			}

			//Normal
			fbxsdk::FbxGeometryElementNormal *pNormalArray = NULL;
			fbxsdk::FbxVector4(*pNormalArrayAccess)(fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint) = NULL;

#if 0
			if (pMesh->GetElementNormalCount() == 0)
			{
				bool bResult = pMesh->GenerateNormals(false, true, false);
				assert(bResult);
			}
			else if (!pMesh->CheckIfVertexNormalsCCW())
			{
				bool bResult = pMesh->GenerateNormals(true, true, false);
				assert(bResult);
			}
#endif

			//Set pOverwrite To true Will Cause Problem
			//应当在GetNormals相关的方法中进行封装；当不为CW时，取相反方向
			#Error
			if (!tri_obj->CheckIfVertexNormalsCCW())
			{
				tri_obj->GenerateNormals(true, false, true);
			}
			else
			{
				tri_obj->GenerateNormals(false, false, false);
			}

			assert(pMesh->GetElementNormalCount() == 1);
			//assert(!pMesh->CheckIfVertexNormalsCCW()); 

			pNormalArray = pMesh->GetLayer(0)->GetNormals();
			assert(pMesh->GetLayerCount() == 1 || pMesh->GetLayer(1)->GetNormals() == NULL);

			//assert(pNormalArray->GetMappingMode() == fbxsdk::FbxGeometryElementMaterial::eByControlPoint);
			//��Ⱦ��Ӧ�����ö��㷨�߶����淨�ߣ�����������
			//����ByPolygonVertex����������Ϊ��ʵ���ض���Ч��

			switch (pNormalArray->GetReferenceMode())
			{
			case fbxsdk::FbxGeometryElementNormal::eDirect:
			{
				switch (pNormalArray->GetMappingMode())
				{
				case fbxsdk::FbxGeometryElementNormal::eByPolygonVertex:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						return pNormalArray->GetDirectArray().GetAt(IndexPolygonVertex);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementNormal::eByControlPoint:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						return pNormalArray->GetDirectArray().GetAt(IndexControlPoint);
					};
				}
				break;
				//������MappingMode��ʵ������Ӧ������
				assert(0);
				case fbxsdk::FbxGeometryElementNormal::eByPolygon:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						return pNormalArray->GetDirectArray().GetAt(PolygonIndex);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementNormal::eAllSame:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						return pNormalArray->GetDirectArray().GetAt(0);
					};
				}
				break;
				//eByEdge
				//This is meant to be used with smoothing layer elements.
				default:
					assert(0);
				}
			}
			break;
			case fbxsdk::FbxGeometryElementNormal::eIndexToDirect:
			{
				switch (pNormalArray->GetMappingMode())
				{
				case fbxsdk::FbxGeometryElementNormal::eByPolygonVertex:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						int Index = pNormalArray->GetIndexArray().GetAt(IndexPolygonVertex);
						return pNormalArray->GetDirectArray().GetAt(Index);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementNormal::eByControlPoint:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						int Index = pNormalArray->GetIndexArray().GetAt(IndexControlPoint);
						return pNormalArray->GetDirectArray().GetAt(Index);
					};
				}
				break;
				assert(0);
				//������MappingMode��ʵ������Ӧ������
				case fbxsdk::FbxGeometryElementNormal::eByPolygon:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						int Index = pNormalArray->GetIndexArray().GetAt(PolygonIndex);
						return pNormalArray->GetDirectArray().GetAt(Index);
					};
				}
				break;
				case fbxsdk::FbxGeometryElementNormal::eAllSame:
				{
					assert(pNormalArrayAccess == NULL);
					pNormalArrayAccess = [](fbxsdk::FbxGeometryElementNormal *pNormalArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
					{
						int Index = pNormalArray->GetIndexArray().GetAt(0);
						return pNormalArray->GetDirectArray().GetAt(Index);
					};
				}
				break;
				//eByEdge
				//This is meant to be used with smoothing layer elements.
				default:
					assert(0);
				}
			}
			break;
			default:
				assert(0);
			}

			//Split Vertex

			//UV + Tangent + BiTangent

			//Only Channel For NormalMap Need UV!!!

			//But The Engine Can't �ܲ� ������ʹ�õ�UVͨ��

			std::vector<fbxsdk::FbxLayerElementUV const *, ::PTS_CPP_Allocator<fbxsdk::FbxLayerElementUV const *>> pUVArrayGroup;
			std::vector<
				fbxsdk::FbxVector2(*)(fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint),
				::PTS_CPP_Allocator<fbxsdk::FbxVector2(*)(fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)>
			> pUVArrayAccessGroup;

			std::vector<fbxsdk::FbxGeometryElementTangent *, ::PTS_CPP_Allocator<fbxsdk::FbxGeometryElementTangent *>>pTangentArrayGroup;
			std::vector<
				fbxsdk::FbxVector4(*)(fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint),
				::PTS_CPP_Allocator<fbxsdk::FbxVector4(*)(fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)>
			> pTangentArrayAccessGroup;

			std::vector<fbxsdk::FbxGeometryElementBinormal *, ::PTS_CPP_Allocator<fbxsdk::FbxGeometryElementBinormal *>> pBiTangentArrayGroup;
			std::vector<
				fbxsdk::FbxVector4(*)(fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint),
				::PTS_CPP_Allocator<fbxsdk::FbxVector4(*)(fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)>
			> pBiTangentArrayAccessGroup;

			//fbxsdk::FbxStringList UVSetNameList;
			//pMesh->GetUVSetNames(UVSetNameList);

			int CountLayer = pMesh->GetLayerCount();
			for (int IndexLayer = 0; IndexLayer < CountLayer; ++IndexLayer)
			{
				//const char* UVSetName = UVSetNameList.GetStringAt(IndexLayer);

				//UV
				fbxsdk::FbxArray<fbxsdk::FbxLayerElementUV const*> pUVSets = pMesh->GetLayer(IndexLayer)->GetUVSets();
				assert(pUVSets.Size() == 1);

				fbxsdk::FbxLayerElementUV const *pUVArrayToInsert = pUVSets[0];
				fbxsdk::FbxVector2(*pUVArrayAccessToInsert)(fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint) = NULL;

				assert(pUVArrayToInsert->GetMappingMode() == fbxsdk::FbxLayerElementUV::eByPolygonVertex);
				switch (pUVArrayToInsert->GetReferenceMode())
				{
				case fbxsdk::FbxLayerElementUV::eDirect:
				{
					switch (pNormalArray->GetMappingMode())
					{
					case fbxsdk::FbxLayerElementUV::eByPolygonVertex:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							return pUVArray->GetDirectArray().GetAt(IndexPolygonVertex);
						};
					}
					break;
					//������MappingMode��ʵ������Ӧ������
					assert(0);
					case fbxsdk::FbxLayerElementUV::eByControlPoint:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							return pUVArray->GetDirectArray().GetAt(IndexControlPoint);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eByPolygon:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							return pUVArray->GetDirectArray().GetAt(PolygonIndex);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eAllSame:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							return pUVArray->GetDirectArray().GetAt(0);
						};
					}
					break;
					//eByEdge
					//This is meant to be used with smoothing layer elements.
					default:
						assert(0);
					}
				}
				break;
				case fbxsdk::FbxLayerElementUV::eIndexToDirect:
				{
					switch (pNormalArray->GetMappingMode())
					{
					case fbxsdk::FbxLayerElementUV::eByPolygonVertex:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							int Index = pUVArray->GetIndexArray().GetAt(IndexPolygonVertex);
							return pUVArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					assert(0);
					//������MappingMode��ʵ������Ӧ������
					case fbxsdk::FbxLayerElementUV::eByControlPoint:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							int Index = pUVArray->GetIndexArray().GetAt(IndexControlPoint);
							return pUVArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eByPolygon:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							int Index = pUVArray->GetIndexArray().GetAt(PolygonIndex);
							return pUVArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eAllSame:
					{
						assert(pUVArrayAccessToInsert == NULL);
						pUVArrayAccessToInsert = [](fbxsdk::FbxLayerElementUV const *pUVArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector2
						{
							int Index = pUVArray->GetIndexArray().GetAt(0);
							return pUVArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					//eByEdge
					//This is meant to be used with smoothing layer elements.
					default:
						assert(0);
					}
				}
				break;
				default:
					assert(0);
				}

				pUVArrayGroup.push_back(pUVArrayToInsert);
				pUVArrayAccessGroup.push_back(pUVArrayAccessToInsert);

				//Tangent
				fbxsdk::FbxGeometryElementTangent *pTangentArrayToInsert = NULL;
				fbxsdk::FbxVector4(*pTangentArrayAccessToInsert)(fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint) = NULL;

				pTangentArrayToInsert = pMesh->GetLayer(IndexLayer)->GetTangents();
				if (pTangentArrayToInsert == NULL)
				{
					//FBXSDK�������д���ȶ
					//��pOverwrite������£�ֻҪMesh��������һ��Layer����Tangent�����������ɣ���ʹMesh���ж��UV
					bool bResult = pMesh->GenerateTangentsData(IndexLayer, true);
					assert(bResult);
					pTangentArrayToInsert = pMesh->GetLayer(IndexLayer)->GetTangents();
				}
				assert(pTangentArrayToInsert != NULL);

				assert(pTangentArrayToInsert->GetMappingMode() == fbxsdk::FbxGeometryElementTangent::eByPolygonVertex);
				switch (pTangentArrayToInsert->GetReferenceMode())
				{
				case fbxsdk::FbxGeometryElementTangent::eDirect:
				{
					switch (pTangentArrayToInsert->GetMappingMode())
					{
					case fbxsdk::FbxGeometryElementTangent::eByPolygonVertex:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pTangentArray->GetDirectArray().GetAt(IndexPolygonVertex);
						};
					}
					break;
					//������MappingMode��ʵ������Ӧ������
					assert(0);
					case fbxsdk::FbxGeometryElementTangent::eByControlPoint:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pTangentArray->GetDirectArray().GetAt(IndexControlPoint);
						};
					}
					break;
					case fbxsdk::FbxGeometryElementTangent::eByPolygon:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pTangentArray->GetDirectArray().GetAt(PolygonIndex);
						};
					}
					break;
					case fbxsdk::FbxGeometryElementTangent::eAllSame:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pTangentArray->GetDirectArray().GetAt(0);
						};
					}
					break;
					//eByEdge
					//This is meant to be used with smoothing layer elements.
					default:
						assert(0);
					}
				}
				break;
				case fbxsdk::FbxGeometryElementTangent::eIndexToDirect:
				{
					switch (pNormalArray->GetMappingMode())
					{
					case fbxsdk::FbxGeometryElementTangent::eByPolygonVertex:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pTangentArray->GetIndexArray().GetAt(IndexPolygonVertex);
							return pTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					assert(0);
					//������MappingMode��ʵ������Ӧ������
					case fbxsdk::FbxGeometryElementTangent::eByControlPoint:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pTangentArray->GetIndexArray().GetAt(IndexControlPoint);
							return pTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					case fbxsdk::FbxGeometryElementTangent::eByPolygon:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pTangentArray->GetIndexArray().GetAt(PolygonIndex);
							return pTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					case fbxsdk::FbxGeometryElementTangent::eAllSame:
					{
						assert(pTangentArrayAccessToInsert == NULL);
						pTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementTangent *pTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pTangentArray->GetIndexArray().GetAt(0);
							return pTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					//eByEdge
					//This is meant to be used with smoothing layer elements.
					default:
						assert(0);
					}
				}
				break;
				default:
					assert(0);
				}

				pTangentArrayGroup.push_back(pTangentArrayToInsert);
				pTangentArrayAccessGroup.push_back(pTangentArrayAccessToInsert);

				//BiTangent
				fbxsdk::FbxGeometryElementBinormal *pBiTangentArrayToInsert = NULL;
				fbxsdk::FbxVector4(*pBiTangentArrayAccessToInsert)(fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint) = NULL;

				pBiTangentArrayToInsert = pMesh->GetLayer(IndexLayer)->GetBinormals();
				assert(pBiTangentArrayToInsert != NULL);

				assert(pBiTangentArrayToInsert->GetMappingMode() == fbxsdk::FbxGeometryElementBinormal::eByPolygonVertex);
				switch (pBiTangentArrayToInsert->GetReferenceMode())
				{
				case fbxsdk::FbxGeometryElementBinormal::eDirect:
				{
					switch (pNormalArray->GetMappingMode())
					{
					case fbxsdk::FbxGeometryElementBinormal::eByPolygonVertex:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pBiTangentArray->GetDirectArray().GetAt(IndexPolygonVertex);
						};
					}
					break;
					//������MappingMode��ʵ������Ӧ������
					assert(0);
					case fbxsdk::FbxLayerElementUV::eByControlPoint:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pBiTangentArray->GetDirectArray().GetAt(IndexControlPoint);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eByPolygon:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pBiTangentArray->GetDirectArray().GetAt(PolygonIndex);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eAllSame:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							return pBiTangentArray->GetDirectArray().GetAt(0);
						};
					}
					break;
					//eByEdge
					//This is meant to be used with smoothing layer elements.
					default:
						assert(0);
					}
				}
				break;
				case fbxsdk::FbxLayerElementUV::eIndexToDirect:
				{
					switch (pNormalArray->GetMappingMode())
					{
					case fbxsdk::FbxLayerElementUV::eByPolygonVertex:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pBiTangentArray->GetIndexArray().GetAt(IndexPolygonVertex);
							return pBiTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					assert(0);
					//������MappingMode��ʵ������Ӧ������
					case fbxsdk::FbxLayerElementUV::eByControlPoint:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pBiTangentArray->GetIndexArray().GetAt(IndexControlPoint);
							return pBiTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eByPolygon:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pBiTangentArray->GetIndexArray().GetAt(PolygonIndex);
							return pBiTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					case fbxsdk::FbxLayerElementUV::eAllSame:
					{
						assert(pBiTangentArrayAccessToInsert == NULL);
						pBiTangentArrayAccessToInsert = [](fbxsdk::FbxGeometryElementBinormal *pBiTangentArray, int PolygonIndex, int IndexPolygonVertex, int IndexControlPoint)->fbxsdk::FbxVector4
						{
							int Index = pBiTangentArray->GetIndexArray().GetAt(0);
							return pBiTangentArray->GetDirectArray().GetAt(Index);
						};
					}
					break;
					//eByEdge
					//This is meant to be used with smoothing layer elements.
					default:
						assert(0);
					}
				}
				break;
				default:
					assert(0);
				}

				pBiTangentArrayGroup.push_back(pBiTangentArrayToInsert);
				pBiTangentArrayAccessGroup.push_back(pBiTangentArrayAccessToInsert);
			}

			assert(pUVArrayGroup.size() == pTangentArrayGroup.size());
			assert(pUVArrayGroup.size() == pBiTangentArrayGroup.size());


			//Mesh Intermediate

			struct PTGRender_MeshSkeletalVertex_Intermediate_FBX
			{
				struct PTGRender_MeshSkeletalVertex m_Vertex;
				uint32_t m_Index_Final;
			};

			struct PTGRender_MeshSkeletal_Intermediate_FBX
			{
				std::map<
					int, //IndexControlPoint
					std::vector<PTGRender_MeshSkeletalVertex_Intermediate_FBX, ::PTS_CPP_Allocator<PTGRender_MeshSkeletalVertex_Intermediate_FBX>>,
					std::less<int>, //IndexControlPoint
					::PTS_CPP_Allocator<std::pair<int, std::vector<PTGRender_MeshSkeletalVertex_Intermediate_FBX, ::PTS_CPP_Allocator<PTGRender_MeshSkeletalVertex_Intermediate_FBX>>>>
				> m_VertexArray;

				std::vector<
					uint32_t,
					::PTS_CPP_Allocator<uint32_t>
				> m_IndexArray;

				uint32_t m_CountVertex;

				inline PTGRender_MeshSkeletal_Intermediate_FBX() :m_CountVertex(0)
				{

				}
			};

			std::map<
				int, //IndexMaterial
				PTGRender_MeshSkeletal_Intermediate_FBX,
				std::less<int>, //IndexMaterial
				::PTS_CPP_Allocator<std::pair<int, PTGRender_MeshSkeletal_Intermediate_FBX>>
			> MeshArray_Intermediate_FBX;

			//ObjectTM
			fbxsdk::FbxAMatrix objectTM;
			{
				// Get the geometry offset to a node. It is never inherited by the children.
				fbxsdk::FbxVector4 const lT = nodemesh_fbx->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
				fbxsdk::FbxVector4 const lR = nodemesh_fbx->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
				fbxsdk::FbxVector4 const lS = nodemesh_fbx->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);

				objectTM = std::move(fbxsdk::FbxAMatrix(lT, lR, lS));
			}

			//NodeTM
			fbxsdk::FbxAMatrix nodeTM = pNodeToProcess->GetAnimationEvaluator()->GetNodeGlobalTransform(pNodeToProcess, FBXSDK_TIME_INFINITE, fbxsdk::FbxNode::eSourcePivot, false, false);
			
			//In Max SDK
			//objectTM = objectOffsetTM * stretchTM * nodeTM

			//FBXSDK的[operator*]有BUG，禁止使用！
			//fbxsdk::FbxAMatrix TransformFBXCoord_Bug = objectTM * nodeTM;
			fbxsdk::FbxAMatrix TransformFBXCoord = PTSIMDMatrixMultiply(Load(nodeTM),Load(objectTM));
			
			PTMatrix4x4F TransformFCoord;
			TransformFCoord.r[0].x = static_cast<float>(TransformFBXCoord.mData[0][0]);
			TransformFCoord.r[0].y = static_cast<float>(TransformFBXCoord.mData[1][0]);
			TransformFCoord.r[0].z = static_cast<float>(TransformFBXCoord.mData[2][0]);
			TransformFCoord.r[0].w = static_cast<float>(TransformFBXCoord.mData[3][0]);
			TransformFCoord.r[1].x = static_cast<float>(TransformFBXCoord.mData[0][1]);
			TransformFCoord.r[1].y = static_cast<float>(TransformFBXCoord.mData[1][1]);
			TransformFCoord.r[1].z = static_cast<float>(TransformFBXCoord.mData[2][1]);
			TransformFCoord.r[1].w = static_cast<float>(TransformFBXCoord.mData[3][1]);
			TransformFCoord.r[2].x = static_cast<float>(TransformFBXCoord.mData[0][2]);
			TransformFCoord.r[2].y = static_cast<float>(TransformFBXCoord.mData[1][2]);
			TransformFCoord.r[2].z = static_cast<float>(TransformFBXCoord.mData[2][2]);
			TransformFCoord.r[2].w = static_cast<float>(TransformFBXCoord.mData[3][2]);
			TransformFCoord.r[3].x = static_cast<float>(TransformFBXCoord.mData[0][3]);
			TransformFCoord.r[3].y = static_cast<float>(TransformFBXCoord.mData[1][3]);
			TransformFCoord.r[3].z = static_cast<float>(TransformFBXCoord.mData[2][3]);
			TransformFCoord.r[3].w = static_cast<float>(TransformFBXCoord.mData[3][3]);

			PTMatrix4x4F TransformFTangent;
			TransformFTangent.r[0].x = TransformFCoord.r[0].x;
			TransformFTangent.r[0].y = TransformFCoord.r[0].y;
			TransformFTangent.r[0].z = TransformFCoord.r[0].z;
			TransformFTangent.r[0].w = 0.0f;
			TransformFTangent.r[1].x = TransformFCoord.r[1].x;
			TransformFTangent.r[1].y = TransformFCoord.r[1].y;
			TransformFTangent.r[1].z = TransformFCoord.r[1].z;
			TransformFTangent.r[1].w = 0.0f;
			TransformFTangent.r[2].x = TransformFCoord.r[2].x;
			TransformFTangent.r[2].y = TransformFCoord.r[2].y;
			TransformFTangent.r[2].z = TransformFCoord.r[2].z;
			TransformFTangent.r[2].w = 0.0f;
			TransformFTangent.r[3].x = TransformFCoord.r[3].x;
			TransformFTangent.r[3].y = TransformFCoord.r[3].y;
			TransformFTangent.r[3].z = TransformFCoord.r[3].z;
			TransformFTangent.r[3].w = TransformFCoord.r[3].w;

			fbxsdk::FbxAMatrix TransformFBXNormal = TransformFBXCoord.Inverse();//Transpose();
			PTMatrix4x4F TransformFNormal;
			TransformFNormal.r[0].x = static_cast<float>(TransformFBXNormal.mData[0][0]);
			TransformFNormal.r[0].y = static_cast<float>(TransformFBXNormal.mData[1][0]);
			TransformFNormal.r[0].z = static_cast<float>(TransformFBXNormal.mData[2][0]);
			TransformFNormal.r[0].w = 0.0f;
			TransformFNormal.r[1].x = static_cast<float>(TransformFBXNormal.mData[0][1]);
			TransformFNormal.r[1].y = static_cast<float>(TransformFBXNormal.mData[1][1]);
			TransformFNormal.r[1].z = static_cast<float>(TransformFBXNormal.mData[2][1]);
			TransformFNormal.r[1].w = 0.0f;
			TransformFNormal.r[2].x = static_cast<float>(TransformFBXNormal.mData[0][2]);
			TransformFNormal.r[2].y = static_cast<float>(TransformFBXNormal.mData[1][2]);
			TransformFNormal.r[2].z = static_cast<float>(TransformFBXNormal.mData[2][2]);
			TransformFNormal.r[2].w = 0.0f;
			TransformFNormal.r[3].x = static_cast<float>(TransformFBXNormal.mData[0][3]);
			TransformFNormal.r[3].y = static_cast<float>(TransformFBXNormal.mData[1][3]);
			TransformFNormal.r[3].z = static_cast<float>(TransformFBXNormal.mData[2][3]);
			TransformFNormal.r[3].w = static_cast<float>(TransformFBXNormal.mData[3][3]);

			PTMatrixSIMD TransformSIMDCoord = ::PTMatrix4x4FLoad(&TransformFCoord);
			PTMatrixSIMD TransformSIMDTangent = ::PTMatrix4x4FLoad(&TransformFTangent);
			PTMatrixSIMD TransformSIMDNormal = ::PTMatrix4x4FLoad(&TransformFNormal);

			int32_t CountNoSpiltForDebug = 0;

			//pMesh->GetControlPoints() //VertexArray / Position //�������ڳ�Position�����Attribute��һ�¶���ҪSpilt
			//pMesh->GetPolygonVertices() //IndexArray

			int CountPolygon = pMesh->GetPolygonCount();
			//MappingMode::ByPolygon->IndexPolygon
			for (int IndexPolygon = 0; IndexPolygon < CountPolygon; ++IndexPolygon)
			{
				assert(pMesh->GetPolygonSize(IndexPolygon) == 3);
				for (int PositionInPolygon = 0; PositionInPolygon < 3; ++PositionInPolygon)
				{
					assert(pMesh->GetPolygonVertexIndex(IndexPolygon) == 3 * IndexPolygon);
					//MappingMode::ByPolygonVertex->IndexPolygonVertex
					int IndexPolygonVertex = 3 * IndexPolygon + PositionInPolygon;
					//MappingMode::ByControlPoint->IndexControlPoint
					int IndexControlPoint = pMesh->GetPolygonVertices()[IndexPolygonVertex];

					assert(IndexControlPoint < pMesh->GetControlPointsCount());

					int MaterialID = pMaterialIDArrayAccess(pMaterialIDArray, IndexPolygon, IndexPolygonVertex, IndexControlPoint);
					PTGRender_MeshSkeletal_Intermediate_FBX &rMesh_Intermediate_FBX = MeshArray_Intermediate_FBX[MaterialID];

					auto &rVertexWithTheSameIndexControlPoint = rMesh_Intermediate_FBX.m_VertexArray[IndexControlPoint];

					PTVector4F Position;
					{
						fbxsdk::FbxVector4 FBXPositionLocal = pMesh->GetControlPointAt(IndexControlPoint);
						PTVector4F PositionLocal = { static_cast<float>(FBXPositionLocal.mData[0]),static_cast<float>(FBXPositionLocal.mData[1]),static_cast<float>(FBXPositionLocal.mData[2]),static_cast<float>(FBXPositionLocal.mData[3]) };
						::PTVector4FStore(&Position, ::PTVector3SIMDTransformCoord(TransformSIMDCoord, ::PTVector4FLoad(&PositionLocal)));
					}

					PTVector4F Normal;
					{
						fbxsdk::FbxVector4 FBXNormalLocal = pNormalArrayAccess(pNormalArray, IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F NormalLocal = { static_cast<float>(FBXNormalLocal.mData[0]),static_cast<float>(FBXNormalLocal.mData[1]),static_cast<float>(FBXNormalLocal.mData[2]),static_cast<float>(FBXNormalLocal.mData[3]) };
						::PTVector4FStore(&Normal, ::PTVector3SIMDTransformTangent(TransformSIMDNormal, ::PTVector4FLoad(&NormalLocal)));
					}

					//GenerateTangentsDataForAllUVSets
					//assert(Tangent.mData[3] == 1.0f); //3dsMax Right-Handed
					//assert(Tangent.mData[3] == -1.0f); //Maya Left-Handed
					PTVector4F Tangent0;
					if (pTangentArrayGroup.size() > 0)
					{
						fbxsdk::FbxVector4 FBXTangentLocal = pTangentArrayAccessGroup[0](pTangentArrayGroup[0], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F TangentLocal = { static_cast<float>(FBXTangentLocal.mData[0]),static_cast<float>(FBXTangentLocal.mData[1]),static_cast<float>(FBXTangentLocal.mData[2]),static_cast<float>(FBXTangentLocal.mData[3]) };
						::PTVector4FStore(&Tangent0, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&TangentLocal)));
					}

					PTVector4F Tangent1;
					if (pTangentArrayGroup.size() > 1)
					{
						fbxsdk::FbxVector4 FBXTangentLocal = pTangentArrayAccessGroup[1](pTangentArrayGroup[1], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F TangentLocal = { static_cast<float>(FBXTangentLocal.mData[0]),static_cast<float>(FBXTangentLocal.mData[1]),static_cast<float>(FBXTangentLocal.mData[2]),static_cast<float>(FBXTangentLocal.mData[3]) };
						::PTVector4FStore(&Tangent1, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&TangentLocal)));
					}

					PTVector4F Tangent2;
					if (pTangentArrayGroup.size() > 2)
					{
						fbxsdk::FbxVector4 FBXTangentLocal = pTangentArrayAccessGroup[2](pTangentArrayGroup[2], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F TangentLocal = { static_cast<float>(FBXTangentLocal.mData[0]),static_cast<float>(FBXTangentLocal.mData[1]),	static_cast<float>(FBXTangentLocal.mData[2]),static_cast<float>(FBXTangentLocal.mData[3]) };
						::PTVector4FStore(&Tangent2, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&TangentLocal)));
					}

					PTVector4F Tangent3;
					if (pTangentArrayGroup.size() > 3)
					{
						fbxsdk::FbxVector4 FBXTangentLocal = pTangentArrayAccessGroup[3](pTangentArrayGroup[3], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F TangentLocal = { static_cast<float>(FBXTangentLocal.mData[0]),static_cast<float>(FBXTangentLocal.mData[1]),static_cast<float>(FBXTangentLocal.mData[2]),static_cast<float>(FBXTangentLocal.mData[3]) };
						::PTVector4FStore(&Tangent3, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&TangentLocal)));
					}

					PTVector4F BiTangent0;
					if (pBiTangentArrayGroup.size() > 0)
					{
						fbxsdk::FbxVector4 FBXBiTangentLocal = pBiTangentArrayAccessGroup[0](pBiTangentArrayGroup[0], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F BiTangentLocal = { static_cast<float>(FBXBiTangentLocal.mData[0]),static_cast<float>(FBXBiTangentLocal.mData[1]),static_cast<float>(FBXBiTangentLocal.mData[2]),static_cast<float>(FBXBiTangentLocal.mData[3]) };
						::PTVector4FStore(&BiTangent0, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&BiTangentLocal)));
					}

					PTVector4F BiTangent1;
					if (pBiTangentArrayGroup.size() > 1)
					{
						fbxsdk::FbxVector4 FBXBiTangentLocal = pBiTangentArrayAccessGroup[1](pBiTangentArrayGroup[1], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F BiTangentLocal = { static_cast<float>(FBXBiTangentLocal.mData[0]),static_cast<float>(FBXBiTangentLocal.mData[1]),static_cast<float>(FBXBiTangentLocal.mData[2]),static_cast<float>(FBXBiTangentLocal.mData[3]) };
						::PTVector4FStore(&BiTangent1, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&BiTangentLocal)));
					}

					PTVector4F BiTangent2;
					if (pBiTangentArrayGroup.size() > 2)
					{
						fbxsdk::FbxVector4 FBXBiTangentLocal = pBiTangentArrayAccessGroup[2](pBiTangentArrayGroup[2], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F BiTangentLocal = { static_cast<float>(FBXBiTangentLocal.mData[0]),static_cast<float>(FBXBiTangentLocal.mData[1]),static_cast<float>(FBXBiTangentLocal.mData[2]),static_cast<float>(FBXBiTangentLocal.mData[3]) };
						::PTVector4FStore(&BiTangent2, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&BiTangentLocal)));
					}

					PTVector4F BiTangent3;
					if (pBiTangentArrayGroup.size() > 3)
					{
						fbxsdk::FbxVector4 FBXBiTangentLocal = pBiTangentArrayAccessGroup[3](pBiTangentArrayGroup[3], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						PTVector4F BiTangentLocal = { static_cast<float>(FBXBiTangentLocal.mData[0]),static_cast<float>(FBXBiTangentLocal.mData[1]),static_cast<float>(FBXBiTangentLocal.mData[2]),static_cast<float>(FBXBiTangentLocal.mData[3]) };
						::PTVector4FStore(&BiTangent3, ::PTVector3SIMDTransformTangent(TransformSIMDTangent, ::PTVector4FLoad(&BiTangentLocal)));
					}

					PTVector2F UV0;
					if (pUVArrayGroup.size() > 0)
					{
						fbxsdk::FbxVector2 FBXUV = pUVArrayAccessGroup[0](pUVArrayGroup[0], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						UV0.x = static_cast<float>(FBXUV.mData[0]);
						UV0.y = static_cast<float>(FBXUV.mData[1]);
					}


					PTVector2F UV1;
					if (pUVArrayGroup.size() > 1)
					{
						fbxsdk::FbxVector2 FBXUV = pUVArrayAccessGroup[1](pUVArrayGroup[1], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						UV1.x = static_cast<float>(FBXUV.mData[0]);
						UV1.y = static_cast<float>(FBXUV.mData[1]);
					}

					PTVector2F UV2;
					if (pUVArrayGroup.size() > 2)
					{
						fbxsdk::FbxVector2 FBXUV = pUVArrayAccessGroup[2](pUVArrayGroup[2], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						UV2.x = static_cast<float>(FBXUV.mData[0]);
						UV2.y = static_cast<float>(FBXUV.mData[1]);
					}

					PTVector2F UV3;
					if (pUVArrayGroup.size() > 3)
					{
						fbxsdk::FbxVector2 FBXUV = pUVArrayAccessGroup[3](pUVArrayGroup[3], IndexPolygon, IndexPolygonVertex, IndexControlPoint);
						UV3.x = static_cast<float>(FBXUV.mData[0]);
						UV3.y = static_cast<float>(FBXUV.mData[1]);
					}


					if (rVertexWithTheSameIndexControlPoint.size() == 0)
					{
						float fNotANumber;
						{
							int32_t Temp = 0X7FC00000;
							fNotANumber = *reinterpret_cast<float*>(&Temp);
						}

						PTGRender_MeshSkeletalVertex_Intermediate_FBX VertexToInsert;
						VertexToInsert.m_Index_Final = rMesh_Intermediate_FBX.m_CountVertex;

						//ͬʱ����AABB ???
						VertexToInsert.m_Vertex.m_Position = Position;
						VertexToInsert.m_Vertex.m_Normal = Normal;
						VertexToInsert.m_Vertex.m_SkinIndex = FBXControlPointArray_SkinIndexAndWeight[IndexControlPoint].m_SkinIndex;
						VertexToInsert.m_Vertex.m_SkinWeight = FBXControlPointArray_SkinIndexAndWeight[IndexControlPoint].m_SkinWeight;
						if (pTangentArrayGroup.size() > 0) { VertexToInsert.m_Vertex.m_Tangent0 = Tangent0; }
						else { VertexToInsert.m_Vertex.m_Tangent0 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pTangentArrayGroup.size() > 1) { VertexToInsert.m_Vertex.m_Tangent1 = Tangent1; }
						else { VertexToInsert.m_Vertex.m_Tangent1 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pTangentArrayGroup.size() > 2) { VertexToInsert.m_Vertex.m_Tangent2 = Tangent2; }
						else { VertexToInsert.m_Vertex.m_Tangent2 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pTangentArrayGroup.size() > 3) { VertexToInsert.m_Vertex.m_Tangent3 = Tangent3; }
						else { VertexToInsert.m_Vertex.m_Tangent3 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pBiTangentArrayGroup.size() > 0) { VertexToInsert.m_Vertex.m_BiTangent0 = BiTangent0; }
						else { VertexToInsert.m_Vertex.m_BiTangent0 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pBiTangentArrayGroup.size() > 1) { VertexToInsert.m_Vertex.m_BiTangent1 = BiTangent1; }
						else { VertexToInsert.m_Vertex.m_BiTangent1 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pBiTangentArrayGroup.size() > 2) { VertexToInsert.m_Vertex.m_BiTangent2 = BiTangent2; }
						else { VertexToInsert.m_Vertex.m_BiTangent2 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pBiTangentArrayGroup.size() > 3) { VertexToInsert.m_Vertex.m_BiTangent3 = BiTangent3; }
						else { VertexToInsert.m_Vertex.m_BiTangent3 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
						if (pUVArrayGroup.size() > 0) { VertexToInsert.m_Vertex.m_UV0 = UV0; }
						else { VertexToInsert.m_Vertex.m_UV0 = PTVector2F{ fNotANumber ,fNotANumber }; }
						if (pUVArrayGroup.size() > 1) { VertexToInsert.m_Vertex.m_UV1 = UV1; }
						else { VertexToInsert.m_Vertex.m_UV1 = PTVector2F{ fNotANumber ,fNotANumber }; }
						if (pUVArrayGroup.size() > 2) { VertexToInsert.m_Vertex.m_UV2 = UV2; }
						else { VertexToInsert.m_Vertex.m_UV2 = PTVector2F{ fNotANumber ,fNotANumber }; }
						if (pUVArrayGroup.size() > 3) { VertexToInsert.m_Vertex.m_UV3 = UV3; }
						else { VertexToInsert.m_Vertex.m_UV3 = PTVector2F{ fNotANumber ,fNotANumber }; }

						rVertexWithTheSameIndexControlPoint.push_back(VertexToInsert);
						rMesh_Intermediate_FBX.m_IndexArray.push_back(VertexToInsert.m_Index_Final);

						++rMesh_Intermediate_FBX.m_CountVertex;
					}
					else
					{
						bool NeedSpilt = true;

						for (PTGRender_MeshSkeletalVertex_Intermediate_FBX &rVertex : rVertexWithTheSameIndexControlPoint)
						{
							float ThresholdNormal;
							float ThresholdTangent0;
							float ThresholdTangent1;
							float ThresholdTangent2;
							float ThresholdTangent3;
							float ThresholdBiTangent0;
							float ThresholdBiTangent1;
							float ThresholdBiTangent2;
							float ThresholdBiTangent3;
							float ThresholdUV0;
							float ThresholdUV1;
							float ThresholdUV2;
							float ThresholdUV3;

							//DotForCos
							ThresholdNormal = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_Normal), ::PTVector4FLoad(&Normal)));
							if (pTangentArrayGroup.size() > 0) { ThresholdTangent0 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent0), ::PTVector4FLoad(&Tangent0))); }
							if (pTangentArrayGroup.size() > 1) { ThresholdTangent1 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent1), ::PTVector4FLoad(&Tangent1))); }
							if (pTangentArrayGroup.size() > 2) { ThresholdTangent2 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent2), ::PTVector4FLoad(&Tangent2))); }
							if (pTangentArrayGroup.size() > 3) { ThresholdTangent3 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent3), ::PTVector4FLoad(&Tangent3))); }
							if (pBiTangentArrayGroup.size() > 0) { ThresholdBiTangent0 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent0), ::PTVector4FLoad(&BiTangent0))); }
							if (pBiTangentArrayGroup.size() > 1) { ThresholdBiTangent1 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent1), ::PTVector4FLoad(&BiTangent1))); }
							if (pBiTangentArrayGroup.size() > 2) { ThresholdBiTangent2 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent2), ::PTVector4FLoad(&BiTangent2))); }
							if (pBiTangentArrayGroup.size() > 3) { ThresholdBiTangent3 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent3), ::PTVector4FLoad(&BiTangent3))); }
							//LengthSquare
							if (pUVArrayGroup.size() > 0)
							{
								PTVectorSIMD vUVOffset = ::PTVectorSIMDSubtract(::PTVector2FLoad(&rVertex.m_Vertex.m_UV0), ::PTVector2FLoad(&UV0));
								ThresholdUV0 = ::PTVectorSIMDGetX(::PTVector2SIMDDot(vUVOffset, vUVOffset));
							}
							if (pUVArrayGroup.size() > 1)
							{
								PTVectorSIMD vUVOffset = ::PTVectorSIMDSubtract(::PTVector2FLoad(&rVertex.m_Vertex.m_UV1), ::PTVector2FLoad(&UV1));
								ThresholdUV1 = ::PTVectorSIMDGetX(::PTVector2SIMDDot(vUVOffset, vUVOffset));
							}
							if (pUVArrayGroup.size() > 2)
							{
								PTVectorSIMD vUVOffset = ::PTVectorSIMDSubtract(::PTVector2FLoad(&rVertex.m_Vertex.m_UV2), ::PTVector2FLoad(&UV2));
								ThresholdUV2 = ::PTVectorSIMDGetX(::PTVector2SIMDDot(vUVOffset, vUVOffset));
							}
							if (pUVArrayGroup.size() > 3)
							{
								PTVectorSIMD vUVOffset = ::PTVectorSIMDSubtract(::PTVector2FLoad(&rVertex.m_Vertex.m_UV3), ::PTVector2FLoad(&UV3));
								ThresholdUV3 = ::PTVectorSIMDGetX(::PTVector2SIMDDot(vUVOffset, vUVOffset));
							}

							if (
								(ThresholdNormal > 0.99f)
								&& ((!(pTangentArrayGroup.size() > 0)) || (ThresholdTangent0 > 0.99f))
								&& ((!(pTangentArrayGroup.size() > 1)) || (ThresholdTangent1 > 0.99f))
								&& ((!(pTangentArrayGroup.size() > 2)) || (ThresholdTangent2 > 0.99f))
								&& ((!(pTangentArrayGroup.size() > 3)) || (ThresholdTangent3 > 0.99f))
								&& ((!(pBiTangentArrayGroup.size() > 0)) || (ThresholdBiTangent0 > 0.99f))
								&& ((!(pBiTangentArrayGroup.size() > 1)) || (ThresholdBiTangent1 > 0.99f))
								&& ((!(pBiTangentArrayGroup.size() > 2)) || (ThresholdBiTangent2 > 0.99f))
								&& ((!(pBiTangentArrayGroup.size() > 3)) || (ThresholdBiTangent3 > 0.99f))
								&& ((!(pUVArrayGroup.size() > 0)) || (ThresholdUV0 < 0.0001f))
								&& ((!(pUVArrayGroup.size() > 1)) || (ThresholdUV1 < 0.0001f))
								&& ((!(pUVArrayGroup.size() > 2)) || (ThresholdUV2 < 0.0001f))
								&& ((!(pUVArrayGroup.size() > 3)) || (ThresholdUV3 < 0.0001f))
								)
							{
								++CountNoSpiltForDebug;

								PTVectorSIMD vHalf = ::PTVectorSIMDReplicate(0.5f);

								::PTVector4FStore(
									&rVertex.m_Vertex.m_Normal,
									::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_Normal), ::PTVector4FLoad(&Normal)), vHalf)
								);

								if (pTangentArrayGroup.size() > 0)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_Tangent0,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent0), ::PTVector4FLoad(&Tangent0)), vHalf)
									);
								}

								if (pTangentArrayGroup.size() > 1)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_Tangent1,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent1), ::PTVector4FLoad(&Tangent1)), vHalf)
									);
								}

								if (pTangentArrayGroup.size() > 2)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_Tangent2,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent2), ::PTVector4FLoad(&Tangent2)), vHalf)
									);
								}

								if (pTangentArrayGroup.size() > 3)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_Tangent3,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_Tangent3), ::PTVector4FLoad(&Tangent3)), vHalf)
									);
								}

								if (pBiTangentArrayGroup.size() > 0)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_BiTangent0,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent0), ::PTVector4FLoad(&BiTangent0)), vHalf)
									);
								}

								if (pBiTangentArrayGroup.size() > 1)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_BiTangent1,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent1), ::PTVector4FLoad(&BiTangent1)), vHalf)
									);
								}

								if (pBiTangentArrayGroup.size() > 2)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_BiTangent2,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent2), ::PTVector4FLoad(&BiTangent2)), vHalf)
									);
								}

								if (pBiTangentArrayGroup.size() > 3)
								{
									::PTVector4FStore(
										&rVertex.m_Vertex.m_BiTangent3,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector4FLoad(&rVertex.m_Vertex.m_BiTangent3), ::PTVector4FLoad(&BiTangent3)), vHalf)
									);
								}

								if (pUVArrayGroup.size() > 0)
								{
									::PTVector2FStore(
										&rVertex.m_Vertex.m_UV0,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector2FLoad(&rVertex.m_Vertex.m_UV0), ::PTVector2FLoad(&UV0)), vHalf)
									);
								}

								if (pUVArrayGroup.size() > 1)
								{
									::PTVector2FStore(
										&rVertex.m_Vertex.m_UV1,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector2FLoad(&rVertex.m_Vertex.m_UV1), ::PTVector2FLoad(&UV1)), vHalf)
									);
								}

								if (pUVArrayGroup.size() > 2)
								{
									::PTVector2FStore(
										&rVertex.m_Vertex.m_UV2,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector2FLoad(&rVertex.m_Vertex.m_UV2), ::PTVector2FLoad(&UV2)), vHalf)
									);
								}

								if (pUVArrayGroup.size() > 3)
								{
									::PTVector2FStore(
										&rVertex.m_Vertex.m_UV3,
										::PTVectorSIMDMultiply(::PTVectorSIMDAdd(::PTVector2FLoad(&rVertex.m_Vertex.m_UV3), ::PTVector2FLoad(&UV3)), vHalf)
									);
								}

								rMesh_Intermediate_FBX.m_IndexArray.push_back(rVertex.m_Index_Final);
								NeedSpilt = false;
								break;
							}
						}

						if (NeedSpilt)
						{
							float fNotANumber;
							{
								int32_t Temp = 0X7FC00000;
								fNotANumber = *reinterpret_cast<float*>(&Temp);
							}

							PTGRender_MeshSkeletalVertex_Intermediate_FBX VertexToInsert;
							VertexToInsert.m_Index_Final = rMesh_Intermediate_FBX.m_CountVertex;

							//ͬʱ����AABB ???
							VertexToInsert.m_Vertex.m_Position = Position;
							VertexToInsert.m_Vertex.m_Normal = Normal;
							VertexToInsert.m_Vertex.m_SkinIndex = FBXControlPointArray_SkinIndexAndWeight[IndexControlPoint].m_SkinIndex;
							VertexToInsert.m_Vertex.m_SkinWeight = FBXControlPointArray_SkinIndexAndWeight[IndexControlPoint].m_SkinWeight;
							if (pTangentArrayGroup.size() > 0) { VertexToInsert.m_Vertex.m_Tangent0 = Tangent0; }
							else { VertexToInsert.m_Vertex.m_Tangent0 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pTangentArrayGroup.size() > 1) { VertexToInsert.m_Vertex.m_Tangent1 = Tangent1; }
							else { VertexToInsert.m_Vertex.m_Tangent1 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pTangentArrayGroup.size() > 2) { VertexToInsert.m_Vertex.m_Tangent2 = Tangent2; }
							else { VertexToInsert.m_Vertex.m_Tangent2 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pTangentArrayGroup.size() > 3) { VertexToInsert.m_Vertex.m_Tangent3 = Tangent3; }
							else { VertexToInsert.m_Vertex.m_Tangent3 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pBiTangentArrayGroup.size() > 0) { VertexToInsert.m_Vertex.m_BiTangent0 = BiTangent0; }
							else { VertexToInsert.m_Vertex.m_BiTangent0 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pBiTangentArrayGroup.size() > 1) { VertexToInsert.m_Vertex.m_BiTangent1 = BiTangent1; }
							else { VertexToInsert.m_Vertex.m_BiTangent1 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pBiTangentArrayGroup.size() > 2) { VertexToInsert.m_Vertex.m_BiTangent2 = BiTangent2; }
							else { VertexToInsert.m_Vertex.m_BiTangent2 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pBiTangentArrayGroup.size() > 3) { VertexToInsert.m_Vertex.m_BiTangent3 = BiTangent3; }
							else { VertexToInsert.m_Vertex.m_BiTangent3 = PTVector4F{ fNotANumber ,fNotANumber ,fNotANumber ,fNotANumber }; }
							if (pUVArrayGroup.size() > 0) { VertexToInsert.m_Vertex.m_UV0 = UV0; }
							else { VertexToInsert.m_Vertex.m_UV0 = PTVector2F{ fNotANumber ,fNotANumber }; }
							if (pUVArrayGroup.size() > 1) { VertexToInsert.m_Vertex.m_UV1 = UV1; }
							else { VertexToInsert.m_Vertex.m_UV1 = PTVector2F{ fNotANumber ,fNotANumber }; }
							if (pUVArrayGroup.size() > 2) { VertexToInsert.m_Vertex.m_UV2 = UV2; }
							else { VertexToInsert.m_Vertex.m_UV2 = PTVector2F{ fNotANumber ,fNotANumber }; }
							if (pUVArrayGroup.size() > 3) { VertexToInsert.m_Vertex.m_UV3 = UV3; }
							else { VertexToInsert.m_Vertex.m_UV3 = PTVector2F{ fNotANumber ,fNotANumber }; }

							rVertexWithTheSameIndexControlPoint.push_back(VertexToInsert);
							rMesh_Intermediate_FBX.m_IndexArray.push_back(VertexToInsert.m_Index_Final);

							++rMesh_Intermediate_FBX.m_CountVertex;
						}
					}

				}
			}

			//Export
			for (auto &rPair_Mesh_Intermediate_FBX : MeshArray_Intermediate_FBX)
			{
				PTGRender_MeshSkeletal_CPP MeshToInsert_Final;
				MeshToInsert_Final.m_VertexArray.resize(rPair_Mesh_Intermediate_FBX.second.m_CountVertex);
				for (auto &rPair_VertexWithTheSameIndexControlPoint : rPair_Mesh_Intermediate_FBX.second.m_VertexArray)
				{
					for (PTGRender_MeshSkeletalVertex_Intermediate_FBX &rVertex : rPair_VertexWithTheSameIndexControlPoint.second)
					{
						assert(static_cast<size_t>(rVertex.m_Index_Final) < MeshToInsert_Final.m_VertexArray.size());
						MeshToInsert_Final.m_VertexArray[rVertex.m_Index_Final] = rVertex.m_Vertex;
					}
				}
				MeshToInsert_Final.m_IndexArray.swap(rPair_Mesh_Intermediate_FBX.second.m_IndexArray);
				rMeshArray_Final.push_back(std::move(MeshToInsert_Final));
			}
		}
		break;
		case fbxsdk::FbxNodeAttribute::eSkeleton:
		case fbxsdk::FbxNodeAttribute::eNull:
			break;
		default:
			assert(0);
		}
	}
};

extern "C" PTEXPORT void PTCALL PTI_FBXSDK_FBXToPTTF(IPTSFile *pFileFBX, IPTSFile *pPTTF)
{
	std::vector<PTGRender_MeshSkeletal_CPP, ::PTS_CPP_Allocator<PTGRender_MeshSkeletal_CPP>> MeshArray_Final;
	std::vector<PTGAnimation_Joint, ::PTS_CPP_Allocator<PTGAnimation_Joint>> Skeleton_Final;
	
	//FBX To Memory
	{
		fbxsdk::FbxManager *pManager = PTI_FBXSDK_FbxManager_Singleton();

		fbxsdk::FbxScene *pScene = fbxsdk::FbxScene::Create(pManager, "");
		{
			fbxsdk::FbxImporter *pImporter = fbxsdk::FbxImporter::Create(pManager, "");

			//FbxImporter::Initialize根据传入的pFileName确定ReadID
			//在传入pStream时，pFileName的相关信息丢失，需要由应用程序显式地进行这个过程
			int pFileFormat; //ReaderID
			bool bResult = pManager->GetIOPluginRegistry()->DetectReaderFileFormat("PTEngine.fbx", pFileFormat);
			assert(bResult);

			//Read pFileFBX To fbxFileBuffer And fbxFileSize
			//...

			//FBXSDK可能会以size=1为参数调用FbxStream::Read
			//为避免读取时间过长，应当事先将整个FBX文件读取到内存
			PTI_FBXSDK_FBXInputStream InputStream(fbxFileBuffer, fbxFileSize, pFileFormat);

			bResult = pImporter->Initialize(&InputStream, NULL, -1, NULL);
			assert(bResult);

			bResult = pImporter->Import(pScene, false);
			assert(bResult);

			pImporter->Destroy(true);

			//��Caller�ͷš���˭����˭�ͷ�
			//pFileFBX->Release();
		}

		//FBX��ʽ��ʵ�ʶ�������3dsMax��Maya��FBX�����������FBXSDK

		//��3dsMax��Maya��FBX���������->��ת��Ĭ��->Y���� ��MayaYUp
		//MayaYUpͬOpenGL Up->Y Front->X Right->Z
		fbxsdk::FbxAxisSystem::MayaYUp.ConvertScene(pScene);

		//��3dsMax��Maya��FBX���������->��λת��Ĭ��->�Զ� ��������ת��
		//fbxsdk::FbxSystemUnit::cm.ConvertScene(pScene);

		fbxsdk::FbxNode *pNodeRoot = pScene->GetRootNode();


		//�����ض���
		//�����ϲ�
		//������������

		//Skeleton

		std::map<
			fbxsdk::FbxNode *,
			PTGAnimation_Joint_Intermediate_FBX,
			std::less<fbxsdk::FbxNode *>,
			::PTS_CPP_Allocator<std::pair<const fbxsdk::FbxNode *, PTGAnimation_Joint_Intermediate_FBX>>
		> Skeleton_Intermediate_FBX;

		std::function<void(fbxsdk::FbxNode *)> const Lambda_TreeTraverse_Skeleton = [&Lambda_TreeTraverse_Skeleton, &Skeleton_Intermediate_FBX](fbxsdk::FbxNode *pNodeRootOfSubtree)->void
		{
			::PTI_FBXSDK_NodeProcess_Skeleton(Skeleton_Intermediate_FBX, pNodeRootOfSubtree);
			for (int IndexChild = 0; IndexChild < pNodeRootOfSubtree->GetChildCount(); ++IndexChild)
			{
				Lambda_TreeTraverse_Skeleton(pNodeRootOfSubtree->GetChild(IndexChild));
			}
		};

		Lambda_TreeTraverse_Skeleton(pNodeRoot);

		//Mesh

		std::function<void(fbxsdk::FbxNode *)> const Lambda_TreeTraverse_Mesh = [&Lambda_TreeTraverse_Mesh, &Skeleton_Intermediate_FBX, &MeshArray_Final](fbxsdk::FbxNode *pNodeRootOfSubtree)->void
		{
			::PTI_FBXSDK_NodeProcess_Mesh(Skeleton_Intermediate_FBX, MeshArray_Final, pNodeRootOfSubtree);
			for (int IndexChild = 0; IndexChild < pNodeRootOfSubtree->GetChildCount(); ++IndexChild)
			{
				Lambda_TreeTraverse_Mesh(pNodeRootOfSubtree->GetChild(IndexChild));
			}
		};

		Lambda_TreeTraverse_Mesh(pNodeRoot);

		pScene->Destroy(true);

		//Export Skeleton
		Skeleton_Final.resize(Skeleton_Intermediate_FBX.size());
		for (auto &rPair_Joint : Skeleton_Intermediate_FBX)
		{
			assert(static_cast<size_t>(rPair_Joint.second.m_Index_Final) < Skeleton_Intermediate_FBX.size());
			Skeleton_Final[rPair_Joint.second.m_Index_Final] = rPair_Joint.second.m_Joint;
		}
	}

	//Memory To PTTF

	//PTAABSIMD TestSphere = ::PTAABSIMDLoadFromPoint(MeshArray[1].m_VertexArray.size(), &MeshArray[1].m_VertexArray[0].m_Position, sizeof(PTGR_Vertex));
	int huhu = 0;

}


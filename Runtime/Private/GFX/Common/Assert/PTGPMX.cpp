//MMD(Miku Miku Dance)
//PMD(Polygon Model Data)
//http://mikumikudance.wikia.com/wiki/MMD:Polygon_Model_Data
//PMX(Polygon Model eXtended)
//http://gist.github.com/felixjones

#include "../../../../Public/PTCommon.h"
#include "../../../../Public/System/PTSThread.h"
#include "../../../../Public/System/PTSMemoryAllocator.h"
#include <string.h>
#include "../../../../Public/System/PTSFile.h"
#include "../../../../Public/Math/PTMath.h"
#include <float.h>
#include <assert.h>
#include <functional>
#include <map>
#include <vector>
#include <stdint.h>
#include <string>
#include "../../../../Public/System/PTSConvUTF.h"

inline int16_t PTG_Assert_LEToH(int16_t Value);
inline uint16_t PTG_Assert_LEToH(uint16_t Value);
inline char16_t PTG_Assert_LEToH(char16_t Value);
inline int32_t PTG_Assert_LEToH(int32_t Value);
inline uint32_t PTG_Assert_LEToH(uint32_t Value);
inline float PTG_Assert_LEToH(float Value);
#include "PTGEndian.h"

inline void PMXTest_Internal()
{
	::PTSMemoryAllocator_Initialize();
	::PTSFileSystem_Initialize();

	struct PTGR_Vertex
	{
		PTVector4UI m_SkinIndex;
		PTVector4F m_SkinWeight;

		PTVector4F m_Position;
		PTVector4F m_Normal;
		//Q-Tangent
		PTVector4F m_Tangent0;
		PTVector4F m_Tangent1;
		PTVector4F m_Tangent2;
		PTVector4F m_Tangent3;
		PTVector4F m_BiTangent0;
		PTVector4F m_BiTangent1;
		PTVector4F m_BiTangent2;
		PTVector4F m_BiTangent3;
		//Unity3D BuiltIn Shader Only Support 2 UVSet
		PTVector2F m_UV0;
		PTVector2F m_UV1;
		PTVector2F m_UV2;
		PTVector2F m_UV3;
	};

	struct PTGR_Mesh_Intermediate_PMX
	{
		std::vector<PTGR_Vertex, ::PTS_CPP_Allocator<PTGR_Vertex>> m_VertexArray;
		std::vector<int32_t, ::PTS_CPP_Allocator<int32_t>> m_IndexArray;
		std::vector<int32_t, ::PTS_CPP_Allocator<int32_t>> m_IndexArraySpiltArray; //Material
	} Mesh_Intermediate_PMX;

	struct PTGA_Joint
	{
		int m_Index;
		char m_Name[32];
	};

	std::vector<PTGA_Joint, ::PTS_CPP_Allocator<PTGA_Joint>> Skeloton;

	float fNotANumber;
	{
		int32_t Temp = 0X7FC00000;
		fNotANumber = *reinterpret_cast<float*>(&Temp);
	}

	IPTSFile *pFilePMX = ::PTSFileSystem_ForProcess()->File_Create(IPTSFileSystem::FILE_OPEN_READONLY, "D:/Tda Helloween Stardust/readme.pmx");

	intptr_t CountDone;

	//Header
	//|Signature | byte[4] | "PMX "[0x50, 0x4D, 0x58, 0x20] | Notice the space at the end |
	//|Version | float | 2.0, 2.1 | Compare as floating point values |
	//|Globals count | byte | Fixed at 8 for PMX 2.0 | This could be variable |
	//|Globals | byte[N] | N = Globals count | See Globals below |
	//|Model name local | text | Name of model | Usually in Japanese |
	//|Model name universal | text | Name of model | Usually in English |
	//|Comments local | text | Additional information(license) | Usually in Japanese |
	//|Comments universal | text | Additional information(license) | Usually in English |

	uint8_t Header_Signature[4];
	pFilePMX->Read(Header_Signature, sizeof(uint8_t[4]));
	assert(Header_Signature[0] == 'P');
	assert(Header_Signature[1] == 'M');
	assert(Header_Signature[2] == 'X');
	assert(Header_Signature[3] == ' ');

	float Header_Version;
	pFilePMX->Read(&Header_Version, sizeof(float));
	Header_Version = ::PTG_Assert_LEToH(Header_Version);
	assert(Header_Version == 2.0f || Header_Version == 2.1f);

	uint8_t Header_GlobalsCount;
	pFilePMX->Read(&Header_GlobalsCount, sizeof(uint8_t));
	assert(Header_GlobalsCount == 8U);

	//Globals
	//|0 | Text encoding | 0, 1 | Byte encoding for the "text" type, 1 = UTF8, 0 = UTF16LE |
	//|1 | Additional vec4 count | 0..4 | Additional vec4 values are added to each vertex |
	//|2 | Vertex index size | 1, 2, 4 | The index type for vertices(1 = byte, 2 = short, 4 = int) |
	//|3 | Texture index size | 1, 2, 4 | The index type for textures(1 = byte, 2 = short, 4 = int) |
	//|4 | Material index size | 1, 2, 4 | The index type for materials(1 = byte, 2 = short, 4 = int) |
	//|5 | Bone index size | 1, 2, 4 | The index type for bones(1 = byte, 2 = short, 4 = int) |
	//|6 | Morph index size | 1, 2, 4 | The index type for morphs(1 = byte, 2 = short, 4 = int) |
	//|7 | Rigidbody index size | 1, 2, 4 | The index type for rigid bodies(1 = byte, 2 = short, 4 = int) |

	struct
	{
		uint8_t TextEncoding;
		uint8_t AdditionalVec4Count;
		uint8_t VertexIndexSize;
		uint8_t TextureIndexSize;
		uint8_t MaterialIndexSize;
		uint8_t BoneIndexSize;
		uint8_t MorphIndexSize;
		uint8_t RigidbodyIndexSize;
	}Header_Globals;
	pFilePMX->Read(&Header_Globals, 8U);

	void(*pFunReadAndIgnoreText)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.TextEncoding)
	{
	case 1U:
	case 0U:
	{
		assert(pFunReadAndIgnoreText == NULL);
		pFunReadAndIgnoreText = [](IPTSFile *pFile)->void
		{
			int32_t Length;
			intptr_t CountDone = pFile->Read(&Length, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			Length = ::PTG_Assert_LEToH(Length);

			for (int32_t IndexByte = 0; IndexByte < Length; ++IndexByte)
			{
				//Ignore UTF8 Or UTF16LE(LEToH)
				uint8_t Byte;
				CountDone = pFile->Read(&Byte, sizeof(uint8_t));
				assert(CountDone == sizeof(uint8_t));
			}
		};
	}
	break;
	default:
	assert(0);
	}

	char *(*pFunAllocAndReadText)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.TextEncoding)
	{
	case 1U:
	{
		assert(pFunAllocAndReadText == NULL);
		pFunAllocAndReadText = [](IPTSFile *pFile)->char *
		{
			int32_t Length;
			intptr_t CountDone = pFile->Read(&Length, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			assert(Length >= 0);
			assert(sizeof(uint8_t) == 1U);
			assert(sizeof(char) == 1U);

			char *pText = NULL;

			if (Length > 0)
			{
				pText = static_cast<char *>(::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(Length + 1)));
				CountDone = pFile->Read(pText, Length);
				assert(CountDone == Length);

				pText[Length] = '\0';
			}

			return pText;
		};
	}
	break;
	case 0U:
	{
		assert(pFunAllocAndReadText == NULL);
		pFunAllocAndReadText = [](IPTSFile *pFile)->char *
		{
			int32_t Length;
			intptr_t CountDone = pFile->Read(&Length, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			assert(Length >= 0);
			assert(sizeof(uint16_t) == 2U);
			assert(sizeof(char16_t) == 2U);
			assert((Length % 2) == 0U);
			
			char *pText = NULL;

			if (Length > 0)
			{
				Length = ((Length - 1) | (2 - 1)) + 1; //AlignUpTo2

				char16_t *pTextUTF16LE = static_cast<char16_t *>(::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(Length)));
				CountDone = pFile->Read(pTextUTF16LE, Length);
				assert(CountDone == Length);

				//ByteOrder
				for (int32_t IndexWChar = 0; IndexWChar < (Length / 2); ++IndexWChar)
				{
					pTextUTF16LE[IndexWChar] = ::PTG_Assert_LEToH(pTextUTF16LE[IndexWChar]);
				}

				//Conv
				uint32_t TextUTF8_Capacity = static_cast<uint32_t>(4 * (Length / 2));//1 UTF16 -> 1 UCS4 -> 4 UTF8 

				char *pTextUTF8 = static_cast<char *>(::PTSMemoryAllocator_Alloc(TextUTF8_Capacity));

				uint32_t InCharsLeft = static_cast<uint32_t>(Length / 2);
				uint32_t OutCharsLeft = TextUTF8_Capacity;

				bool bResult = ::PTSConv_UTF16ToUTF8(pTextUTF16LE, &InCharsLeft, pTextUTF8, &OutCharsLeft);
				assert(bResult);

				::PTSMemoryAllocator_Free(pTextUTF16LE);

				//MemCpy
				pText = static_cast<char *>(::PTSMemoryAllocator_Alloc(TextUTF8_Capacity - OutCharsLeft + 1U)); // + '\0'

				::memcpy(pText, pTextUTF8, TextUTF8_Capacity - OutCharsLeft);

				::PTSMemoryAllocator_Free(pTextUTF8);

				pText[TextUTF8_Capacity - OutCharsLeft] = '\0';
			}
			
			return pText;
		};
	}
	break;
	default:
	assert(0);
	}

	int32_t(*pFunReadBoneIndex)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.BoneIndexSize)
	{
	case 1U:
	{
		assert(pFunReadBoneIndex == NULL);
		pFunReadBoneIndex = [](IPTSFile *pFile)->int32_t
		{
			int8_t BoneIndex;
			intptr_t CountDone = pFile->Read(&BoneIndex, sizeof(int8_t));
			assert(CountDone == sizeof(int8_t));
			assert(BoneIndex >= 0 || BoneIndex == -1);
			return static_cast<int32_t>(BoneIndex);
		};
	}
	break;
	case 2U:
	{
		assert(pFunReadBoneIndex == NULL);
		pFunReadBoneIndex = [](IPTSFile *pFile)->int32_t
		{
			int16_t BoneIndex;
			intptr_t CountDone = pFile->Read(&BoneIndex, sizeof(int16_t));
			assert(CountDone == sizeof(int16_t));
			BoneIndex = ::PTG_Assert_LEToH(BoneIndex);
			assert(BoneIndex >= 0 || BoneIndex == -1);
			return static_cast<int32_t>(BoneIndex);
		};
	}
	break;
	case 4U:
	{
		assert(pFunReadBoneIndex == NULL);
		pFunReadBoneIndex = [](IPTSFile *pFile)->int32_t
		{
			int32_t BoneIndex;
			intptr_t CountDone = pFile->Read(&BoneIndex, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			BoneIndex = ::PTG_Assert_LEToH(BoneIndex);
			assert(BoneIndex >= 0 || BoneIndex == -1);
			return static_cast<int32_t>(BoneIndex);
		};
	}
	break;
	default:
		assert(0);
	}

	uint32_t(*pFunReadVertexIndex)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.VertexIndexSize)
	{
	case 1U:
	{
		assert(pFunReadVertexIndex == NULL);
		pFunReadVertexIndex = [](IPTSFile *pFile)->uint32_t
		{
			uint8_t VertexIndex;
			intptr_t CountDone = pFile->Read(&VertexIndex, sizeof(uint8_t));
			assert(CountDone == sizeof(uint8_t));
			return static_cast<uint32_t>(VertexIndex);
		};
	}
	break;
	case 2U:
	{
		assert(pFunReadVertexIndex == NULL);
		pFunReadVertexIndex = [](IPTSFile *pFile)->uint32_t
		{
			uint16_t VertexIndex;
			intptr_t CountDone = pFile->Read(&VertexIndex, sizeof(uint16_t));
			assert(CountDone == sizeof(uint16_t));
			VertexIndex = ::PTG_Assert_LEToH(VertexIndex);
			return static_cast<uint32_t>(VertexIndex);
		};
	}
	break;
	case 4U:
	{
		assert(pFunReadVertexIndex == NULL);
		pFunReadVertexIndex = [](IPTSFile *pFile)->uint32_t
		{
			int32_t VertexIndex;
			intptr_t CountDone = pFile->Read(&VertexIndex, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			VertexIndex = ::PTG_Assert_LEToH(VertexIndex);
			assert(VertexIndex >= 0);
			return VertexIndex;
		};
	}
	break;
	default:
		assert(0);
	}

	int32_t(*pFunReadTextureIndex)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.TextureIndexSize)
	{
	case 1U:
	{
		assert(pFunReadTextureIndex == NULL);
		pFunReadTextureIndex = [](IPTSFile *pFile)->int32_t
		{
			int8_t TextureIndex;
			intptr_t CountDone = pFile->Read(&TextureIndex, sizeof(int8_t));
			assert(CountDone == sizeof(int8_t));
			assert(TextureIndex >= 0 || TextureIndex == -1);
			return static_cast<int32_t>(TextureIndex);
		};
	}
	break;
	case 2U:
	{
		assert(pFunReadTextureIndex == NULL);
		pFunReadTextureIndex = [](IPTSFile *pFile)->int32_t
		{
			int16_t TextureIndex;
			intptr_t CountDone = pFile->Read(&TextureIndex, sizeof(int16_t));
			assert(CountDone == sizeof(int16_t));
			TextureIndex = ::PTG_Assert_LEToH(TextureIndex);
			assert(TextureIndex >= 0 || TextureIndex == -1);
			return static_cast<int32_t>(TextureIndex);
		};
	}
	break;
	case 4U:
	{
		assert(pFunReadTextureIndex == NULL);
		pFunReadTextureIndex = [](IPTSFile *pFile)->int32_t
		{
			int32_t TextureIndex;
			intptr_t CountDone = pFile->Read(&TextureIndex, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			TextureIndex = ::PTG_Assert_LEToH(TextureIndex);
			assert(TextureIndex >= 0 || TextureIndex == -1);
			return TextureIndex;
		};
	}
	break;
	default:
		assert(0);
	}

	int32_t(*pFunReadMorphIndex)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.MorphIndexSize)
	{
	case 1U:
	{
		assert(pFunReadMorphIndex == NULL);
		pFunReadMorphIndex = [](IPTSFile *pFile)->int32_t
		{
			int8_t MorphIndex;
			intptr_t CountDone = pFile->Read(&MorphIndex, sizeof(int8_t));
			assert(CountDone == sizeof(int8_t));
			assert(MorphIndex >= 0 || MorphIndex == -1);
			return static_cast<int32_t>(MorphIndex);
		};
	}
	break;
	case 2U:
	{
		assert(pFunReadMorphIndex == NULL);
		pFunReadMorphIndex = [](IPTSFile *pFile)->int32_t
		{
			int16_t MorphIndex;
			intptr_t CountDone = pFile->Read(&MorphIndex, sizeof(int16_t));
			assert(CountDone == sizeof(int16_t));
			MorphIndex = ::PTG_Assert_LEToH(MorphIndex);
			assert(MorphIndex >= 0 || MorphIndex == -1);
			return static_cast<int32_t>(MorphIndex);
		};
	}
	break;
	case 4U:
	{
		assert(pFunReadMorphIndex == NULL);
		pFunReadMorphIndex = [](IPTSFile *pFile)->int32_t
		{
			int32_t MorphIndex;
			intptr_t CountDone = pFile->Read(&MorphIndex, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			MorphIndex = ::PTG_Assert_LEToH(MorphIndex);
			assert(MorphIndex >= 0 || MorphIndex == -1);
			return MorphIndex;
		};
	}
	break;
	default:
		assert(0);
	}

	int32_t(*pFunReadMaterialIndex)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.MaterialIndexSize)
	{
	case 1U:
	{
		assert(pFunReadMaterialIndex == NULL);
		pFunReadMaterialIndex = [](IPTSFile *pFile)->int32_t
		{
			int8_t MaterialIndex;
			intptr_t CountDone = pFile->Read(&MaterialIndex, sizeof(int8_t));
			assert(CountDone == sizeof(int8_t));
			assert(MaterialIndex >= 0 || MaterialIndex == -1);
			return static_cast<int32_t>(MaterialIndex);
		};
	}
	break;
	case 2U:
	{
		assert(pFunReadMaterialIndex == NULL);
		pFunReadMaterialIndex = [](IPTSFile *pFile)->int32_t
		{
			int16_t MaterialIndex;
			intptr_t CountDone = pFile->Read(&MaterialIndex, sizeof(int16_t));
			assert(CountDone == sizeof(int16_t));
			MaterialIndex = ::PTG_Assert_LEToH(MaterialIndex);
			assert(MaterialIndex >= 0 || MaterialIndex == -1);
			return static_cast<int32_t>(MaterialIndex);
		};
	}
	break;
	case 4U:
	{
		assert(pFunReadMaterialIndex == NULL);
		pFunReadMaterialIndex = [](IPTSFile *pFile)->int32_t
		{
			int32_t MaterialIndex;
			intptr_t CountDone = pFile->Read(&MaterialIndex, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			MaterialIndex = ::PTG_Assert_LEToH(MaterialIndex);
			assert(MaterialIndex >= 0 || MaterialIndex == -1);
			return MaterialIndex;
		};
	}
	break;
	default:
		assert(0);
	}

	int32_t(*pFunReadRigidbodyIndex)(IPTSFile *pFile) = NULL;
	switch (Header_Globals.RigidbodyIndexSize)
	{
	case 1U:
	{
		assert(pFunReadRigidbodyIndex == NULL);
		pFunReadRigidbodyIndex = [](IPTSFile *pFile)->int32_t
		{
			int8_t RigidbodyIndex;
			intptr_t CountDone = pFile->Read(&RigidbodyIndex, sizeof(int8_t));
			assert(CountDone == sizeof(int8_t));
			assert(RigidbodyIndex >= 0 || RigidbodyIndex == -1);
			return static_cast<int32_t>(RigidbodyIndex);
		};
	}
	break;
	case 2U:
	{
		assert(pFunReadRigidbodyIndex == NULL);
		pFunReadRigidbodyIndex = [](IPTSFile *pFile)->int32_t
		{
			int16_t RigidbodyIndex;
			intptr_t CountDone = pFile->Read(&RigidbodyIndex, sizeof(int16_t));
			assert(CountDone == sizeof(int16_t));
			RigidbodyIndex = ::PTG_Assert_LEToH(RigidbodyIndex);
			assert(RigidbodyIndex >= 0 || RigidbodyIndex == -1);
			return static_cast<int32_t>(RigidbodyIndex);
		};
	}
	break;
	case 4U:
	{
		assert(pFunReadRigidbodyIndex == NULL);
		pFunReadRigidbodyIndex = [](IPTSFile *pFile)->int32_t
		{
			int32_t RigidbodyIndex;
			intptr_t CountDone = pFile->Read(&RigidbodyIndex, sizeof(int32_t));
			assert(CountDone == sizeof(int32_t));
			RigidbodyIndex = ::PTG_Assert_LEToH(RigidbodyIndex);
			assert(RigidbodyIndex >= 0 || RigidbodyIndex == -1);
			return RigidbodyIndex;
		};
	}
	break;
	default:
		assert(0);
	}

	//ModelNameLocal
	pFunReadAndIgnoreText(pFilePMX);

	//ModelNameUniversal
	pFunReadAndIgnoreText(pFilePMX);

	//CommentsLocal
	pFunReadAndIgnoreText(pFilePMX);

	//CommentsUniversal
	pFunReadAndIgnoreText(pFilePMX);


	//The vertex section starts with an int defining how many vertices there are(Note that it is a signed int).

	int32_t VertexData_VertexCount;
	pFilePMX->Read(&VertexData_VertexCount, sizeof(int32_t));
	VertexData_VertexCount = ::PTG_Assert_LEToH(VertexData_VertexCount);

	for (int32_t IndexVertex = 0; IndexVertex < VertexData_VertexCount; ++IndexVertex)
	{
		//Vertex Data

		//| Name | Type | Notes |
		//| ------------------ - | ------ - | -------------------------------------------------- - |
		//| Position | vec3 | XYZ |
		//| Normal | vec3 | XYZ |
		//| UV | vec2 | XY |
		//| Additional vec4 | vec4[N] | N is defined in the Globals(Can be 0) |
		//| Weight deform type | byte | 0 = BDEF1, 1 = BDEF2, 2 = BDEF4, 3 = SDEF, 4 = QDEF |
		//| Weight deform | ~| See Deform Types above |
		//| Edge scale | float | Pencil - outline scale(1.0 should be around 1 pixel) |

		PTGR_Vertex VertexToInsert;

		//Position
		pFilePMX->Read(&VertexToInsert.m_Position, sizeof(float) * 3U);
		VertexToInsert.m_Position.w = fNotANumber;
		//Normal
		pFilePMX->Read(&VertexToInsert.m_Normal, sizeof(float) * 3U);
		VertexToInsert.m_Normal.w = fNotANumber;
		//UV
		pFilePMX->Read(&VertexToInsert.m_UV0, sizeof(float) * 2U);
		VertexToInsert.m_UV1 = PTVector2F{ fNotANumber,fNotANumber };
		VertexToInsert.m_UV2 = PTVector2F{ fNotANumber,fNotANumber };
		VertexToInsert.m_UV3 = PTVector2F{ fNotANumber,fNotANumber };

		//AdditionalVec4
		if (Header_Globals.AdditionalVec4Count != 0U)
		{
			float AdditionalVec4[4U * 255U];
			pFilePMX->Read(AdditionalVec4, sizeof(float) * 4U * Header_Globals.AdditionalVec4Count);
		}

		//WeightDeformType
		//运行时蒙皮中插值的方式不应当依赖于对资产的加载
		//PatriotEngine始终采用DualQuaternion插值
		//所有类型都将转换为QDEF(Dual Quaternion Deform Blending)
		//没有实用价值的SDEF(Spherical Deform Blending)按照BDEF2处理
		
		uint8_t WeightDeformType;
		pFilePMX->Read(&WeightDeformType, sizeof(uint8_t));
		switch (WeightDeformType)
		{
		case 0U:
		{
			//BDEF1
			//| type | name | Notes |
			//|------ - | ---------- - | -------------- - |
			//| index | Bone index | Type specified in the Bone Index Size header field. Bone weight is 1.0. |

			VertexToInsert.m_SkinIndex.x = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.y = 0U;
			VertexToInsert.m_SkinIndex.z = 0U;
			VertexToInsert.m_SkinIndex.w = 0U;
			VertexToInsert.m_SkinWeight.x = 1.0f;
			VertexToInsert.m_SkinWeight.y = 0.0f;
			VertexToInsert.m_SkinWeight.z = 0.0f;
			VertexToInsert.m_SkinWeight.w = 0.0f;
		}
		break;
		case 1U:
		{
			//BDEF2
			//| type | name | Notes |
			//|------ - | ---------- - | -------------- - |
			//| index | Bone index 1 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 2 | Type specified in the Bone Index Size header field. |
			//| float | Bone 1 weight | Bone 2 weight = 1.0 - Bone 1 weight |

			VertexToInsert.m_SkinIndex.x = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.y = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.z = 0U;
			VertexToInsert.m_SkinIndex.w = 0U;
			pFilePMX->Read(&VertexToInsert.m_SkinWeight.x, sizeof(float));
			VertexToInsert.m_SkinWeight.x = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.x);
			VertexToInsert.m_SkinWeight.y = 1.0f - VertexToInsert.m_SkinWeight.x;
			VertexToInsert.m_SkinWeight.z = 0.0f;
			VertexToInsert.m_SkinWeight.w = 0.0f;
		}
		break;
		case 2U:
		{
			//BDEF4
			//| type | name | Notes |
			//|------ - | ---------- - | -------------- - |
			//| index | Bone index 1 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 2 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 3 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 4 | Type specified in the Bone Index Size header field. |
			//| float | Bone 1 weight | Weight total not guaranteed to be 1.0 |
			//| float | Bone 2 weight | Weight total not guaranteed to be 1.0 |
			//| float | Bone 3 weight | Weight total not guaranteed to be 1.0 |
			//| float | Bone 4 weight | Weight total not guaranteed to be 1.0 |

			VertexToInsert.m_SkinIndex.x = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.y = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.z = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.w = pFunReadBoneIndex(pFilePMX);
			pFilePMX->Read(&VertexToInsert.m_SkinWeight, sizeof(float) * 4U);
			VertexToInsert.m_SkinWeight.x = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.x);
			VertexToInsert.m_SkinWeight.y = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.y);
			VertexToInsert.m_SkinWeight.z = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.z);
			VertexToInsert.m_SkinWeight.w = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.w);
		}
		break;
		case 3U:
		{
			//SDEF
			//Spherical deform blending
			//| type | name | Notes								|
			//|------ - | -------------- - | ---------------------------------- - |
			//| index | Bone index 1 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 2 | Type specified in the Bone Index Size header field. |
			//| float | Bone 1 weight | Bone 2 weight = 1.0 - Bone 1 weight|
			//| vec3 | C | ? ? ? |
			//| vec3 | R0 | ? ? ? |
			//| vec3 | R1 | ? ? ? |

			VertexToInsert.m_SkinIndex.x = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.y = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.z = 0U;
			VertexToInsert.m_SkinIndex.w = 0U;
			pFilePMX->Read(&VertexToInsert.m_SkinWeight.x, sizeof(float));
			VertexToInsert.m_SkinWeight.x = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.x);
			VertexToInsert.m_SkinWeight.y = 1.0f - VertexToInsert.m_SkinWeight.x;
			VertexToInsert.m_SkinWeight.z = 0.0f;
			VertexToInsert.m_SkinWeight.w = 0.0f;
			float C[3];
			float R0[3];
			float R1[3];
			pFilePMX->Read(C, sizeof(float) * 3U);
			pFilePMX->Read(R0, sizeof(float) * 3U);
			pFilePMX->Read(R1, sizeof(float) * 3U);
		}
		break;
		case 4U:
		{
			//QDEF
			//Dual quaternion deform blending
			//| type | name | Notes |
			//|------ - | ---------- - | -------------- - |
			//| index | Bone index 1 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 2 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 3 | Type specified in the Bone Index Size header field. |
			//| index | Bone index 4 | Type specified in the Bone Index Size header field. |
			//| float | Bone 1 weight | Weight total not guaranteed to be 1.0 |
			//| float | Bone 2 weight | Weight total not guaranteed to be 1.0 |
			//| float | Bone 3 weight | Weight total not guaranteed to be 1.0 |
			//| float | Bone 4 weight | Weight total not guaranteed to be 1.0 |

			VertexToInsert.m_SkinIndex.x = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.y = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.z = pFunReadBoneIndex(pFilePMX);
			VertexToInsert.m_SkinIndex.w = pFunReadBoneIndex(pFilePMX);
			pFilePMX->Read(&VertexToInsert.m_SkinWeight.x, sizeof(float));
			pFilePMX->Read(&VertexToInsert.m_SkinWeight.y, sizeof(float));
			pFilePMX->Read(&VertexToInsert.m_SkinWeight.z, sizeof(float));
			pFilePMX->Read(&VertexToInsert.m_SkinWeight.w, sizeof(float));
			VertexToInsert.m_SkinWeight.x = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.x);
			VertexToInsert.m_SkinWeight.y = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.y);
			VertexToInsert.m_SkinWeight.z = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.z);
			VertexToInsert.m_SkinWeight.w = ::PTG_Assert_LEToH(VertexToInsert.m_SkinWeight.w);
		}
		break;
		default:
			assert(0);
		}
		//Tangent
		//不存在NormalMap
		VertexToInsert.m_Tangent0 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_Tangent1 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_Tangent2 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_Tangent3 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_BiTangent0 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_BiTangent1 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_BiTangent2 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };
		VertexToInsert.m_BiTangent3 = PTVector4F{ fNotANumber,fNotANumber,fNotANumber,fNotANumber };

		float EdgeScale;
		pFilePMX->Read(&EdgeScale, sizeof(float));

		Mesh_Intermediate_PMX.m_VertexArray.push_back(VertexToInsert);
	}

	//Surface
	//Surfaces are in groups of 3, they define a single triangle face based on the vertices that they are referencing.
	//[surface 0->A, surface 1->B, surface 2->C] = triangle[vertex A, vertex B, vertex C]
	//Triangle facing is defined in clockwise winding order(As with DirectX).
	
	//Surface Data
	//The surface section starts with an int defining how many surfaces there are(Note that it is a signed int).

	//| Name | Type | Notes					|
	//|------------------ - | ------ - | ---------------------- - |
	//|Vertex index | index | See Index Types above |

	int32_t SurfaceData_Surfacecount;
	pFilePMX->Read(&SurfaceData_Surfacecount, sizeof(int32_t));
	SurfaceData_Surfacecount = ::PTG_Assert_LEToH(SurfaceData_Surfacecount);
	assert((SurfaceData_Surfacecount % 3) == 0);

	for (int32_t IndexTriangle = 0; IndexTriangle < SurfaceData_Surfacecount; IndexTriangle += 3)
	{
		uint32_t IndexA;
		uint32_t IndexB;
		uint32_t IndexC;
		IndexA = pFunReadVertexIndex(pFilePMX);
		if ((IndexTriangle + 1) < SurfaceData_Surfacecount) { IndexB = pFunReadVertexIndex(pFilePMX); } else { IndexB = ~uint32_t(0U); }
		if ((IndexTriangle + 2) < SurfaceData_Surfacecount) { IndexC = pFunReadVertexIndex(pFilePMX); } else { IndexC = ~uint32_t(0U); }
		
		//PTEngine CCW
		//TriangleList
		Mesh_Intermediate_PMX.m_IndexArray.push_back(IndexC);
		Mesh_Intermediate_PMX.m_IndexArray.push_back(IndexB);
		Mesh_Intermediate_PMX.m_IndexArray.push_back(IndexA);
	}

	//Texture Data

	//The texture section starts with an int defining how many textures there are(Note that it is a signed int).

	//| Name | Type | Notes													|
	//|------ - | ------ - | ------------------------------------------------------ - |
	//| Path | text | File system path to a texture file(Usually relative) |

	int32_t TextureData_Texturecount;
	pFilePMX->Read(&TextureData_Texturecount, sizeof(int32_t));
	TextureData_Texturecount = ::PTG_Assert_LEToH(TextureData_Texturecount);
	
	int32_t IndexAccumulate = 0;

	for (int32_t IndexTexture = 0; IndexTexture < TextureData_Texturecount; ++IndexTexture)
	{
		char *Texture_Path = pFunAllocAndReadText(pFilePMX);
		::PTSMemoryAllocator_Free(Texture_Path);
	}
	
	//Material Data

	//The material section starts with an int defining how many materials there are(Note that it is a signed int).

	//| Name | Type | Notes																|
	//|---------------------- - | ---------- - | ------------------------------------------------------------------ - |
	//| Material name local | text | Handy name for the material(Usually Japanese) |
	//| Material name universal | text | Handy name for the material(Usually English) |
	//| Diffuse colour | vec4 | RGBA colour(Alpha will set a semi - transparent material) |
	//| Specular colour | vec3 | RGB colour of the reflected light |
	//| Specular strength | float | The "size" of the specular highlight |
	//| Ambient colour | vec3 | RGB colour of the material shadow(When out of light) |
	//| Drawing flags | flag | See Material Flags |
	//| Edge colour | vec4 | RGBA colour of the pencil - outline edge(Alpha for semi - transparent)|
	//| Edge scale | float | Pencil - outline scale(1.0 should be around 1 pixel) |
	//| Texture index | index | See Index Types, this is from the texture table by default |
	//| Environment index | index | Same as texture index, but for environment mapping |
	//| Environment blend mode | byte | 0 = disabled, 1 = multiply, 2 = additive, 3 = additional vec4* |
	//| Toon reference | byte | 0 = Texture reference, 1 = internal reference |
	//| Toon value | index / byte | If Toon reference is 0, then it has the type specified in the Texture Index Size header field. |
	//|            |              | If Toon reference is 1, the size is 1 byte with a value from 0 - 9. Most implementations will use "toon01.bmp" to "toon10.bmp" as the internal textures. |
	//| Meta data | text | This is used for scripting or additional data |
	//| Surface count | int |  How many faces this material effects. The starting offset is calculated by adding this value from all the previous materials.
	//                         Eg; mat3_first_face = [mat2.face_count + mat1.face_count + mat0.face_count]
	//                         mat3_last_face = mat3_first_face + mat3.face_count;

	Mesh_Intermediate_PMX.m_IndexArraySpiltArray.push_back(0);

	int32_t MaterialData_MaterialCount;
	pFilePMX->Read(&MaterialData_MaterialCount, sizeof(int32_t));
	MaterialData_MaterialCount = ::PTG_Assert_LEToH(MaterialData_MaterialCount);

	for (int32_t IndexMaterial = 0; IndexMaterial < MaterialData_MaterialCount; ++IndexMaterial)
	{
		//MaterialNameLocal
		pFunReadAndIgnoreText(pFilePMX);
		//MaterialNameUniversal
		pFunReadAndIgnoreText(pFilePMX);

		float DiffuseColor[4];
		pFilePMX->Read(DiffuseColor, sizeof(float) * 4);
		DiffuseColor[0] = ::PTG_Assert_LEToH(DiffuseColor[0]);
		DiffuseColor[1] = ::PTG_Assert_LEToH(DiffuseColor[1]);
		DiffuseColor[2] = ::PTG_Assert_LEToH(DiffuseColor[2]);
		DiffuseColor[3] = ::PTG_Assert_LEToH(DiffuseColor[3]);

		float SpecularColor[3];
		pFilePMX->Read(SpecularColor, sizeof(float) * 3);
		SpecularColor[0] = ::PTG_Assert_LEToH(SpecularColor[0]);
		SpecularColor[1] = ::PTG_Assert_LEToH(SpecularColor[1]);
		SpecularColor[2] = ::PTG_Assert_LEToH(SpecularColor[2]);

		float SpecularStrength;
		pFilePMX->Read(&SpecularStrength, sizeof(float));
		SpecularStrength = ::PTG_Assert_LEToH(SpecularStrength);

		float AmbientColor[3];
		pFilePMX->Read(AmbientColor, sizeof(float) * 3);
		AmbientColor[0] = ::PTG_Assert_LEToH(AmbientColor[0]);
		AmbientColor[1] = ::PTG_Assert_LEToH(AmbientColor[1]);
		AmbientColor[2] = ::PTG_Assert_LEToH(AmbientColor[2]);
		
		//Material Flags

		//Materials can have the following bit flags :

		//| Bit index | Name | Set effect | Version	|
		//| : -------- - : | -------------- - | ------------------------------------------ - | ---------- - |
		//| 0 | No-cull | Disables back - face culling | 2.0 | //个人认为可以等同于透明材质处理
		//| 1 | Ground shadow | Projects a shadow onto the geometry | 2.0 |
		//| 2 | Draw shadow | Renders to the shadow map | 2.0 |
		//| 3 | Receive shadow | Receives a shadow from the shadow map | 2.0 |
		//| 4 | Has edge | Has "pencil" outline | 2.0 |
		//| 5 | Vertex colour | Uses additional vec4 1 for vertex colour | 2.1 |
		//| 6 | Point drawing | Each of the 3 vertices are points | 2.1 |
		//| 7 | Line drawing | The triangle is rendered as lines | 2.1 |

		uint8_t DrawingFlags;
		pFilePMX->Read(&DrawingFlags, sizeof(uint8_t));
		
		float EdgeColor[4];
		pFilePMX->Read(EdgeColor, sizeof(float) * 4);
		EdgeColor[0] = ::PTG_Assert_LEToH(EdgeColor[0]);
		EdgeColor[1] = ::PTG_Assert_LEToH(EdgeColor[1]);
		EdgeColor[2] = ::PTG_Assert_LEToH(EdgeColor[2]);
		EdgeColor[3] = ::PTG_Assert_LEToH(EdgeColor[3]);

		float EdgeScale;
		pFilePMX->Read(&EdgeScale, sizeof(float));
		EdgeScale = ::PTG_Assert_LEToH(EdgeScale);

		int32_t TextureIndex = pFunReadTextureIndex(pFilePMX);
		int32_t EnvironmentIndex = pFunReadTextureIndex(pFilePMX);

		uint8_t EnvironmentBlendMode;
		pFilePMX->Read(&EnvironmentBlendMode, sizeof(uint8_t));

		uint8_t ToonReference;
		pFilePMX->Read(&ToonReference, sizeof(uint8_t));

		switch (ToonReference)
		{
		case 0:
		{
			int32_t ToonValue = pFunReadTextureIndex(pFilePMX);
		}
		break;
		case 1:
		{
			char const *InternalReference[] = {
				"toon01.bmp",
				"toon02.bmp",
				"toon03.bmp",
				"toon04.bmp",
				"toon05.bmp",
				"toon06.bmp",
				"toon07.bmp",
				"toon08.bmp",
				"toon09.bmp",
				"toon10.bmp",
			};
			uint8_t ToonValue;
			pFilePMX->Read(&ToonValue, sizeof(ToonValue));
			assert(ToonValue < 9U);
		}
		break;
		default:
			assert(0);
		}

		//Material_MetaData;
		pFunReadAndIgnoreText(pFilePMX);

		//根据MaterialID分割Mesh
		int32_t Material_SurfaceCount;
		pFilePMX->Read(&Material_SurfaceCount, sizeof(int32_t));
		Material_SurfaceCount = ::PTG_Assert_LEToH(Material_SurfaceCount);
		
		IndexAccumulate += Material_SurfaceCount;
		Mesh_Intermediate_PMX.m_IndexArraySpiltArray.push_back(IndexAccumulate);
	}
	assert(IndexAccumulate == SurfaceData_Surfacecount);

	//BoneData

	//The bone section starts with an int defining how many bones there are(Note that it is a signed int).

	//| Name | Type | Notes																	|
	//|---------------------- - | ---------- - | ---------------------------------------------------------------------- - |
	//| Bone name local | text | Handy name for the bone(Usually Japanese) |
	//| Bone name universal | text | Handy name for the bone(Usually English) |
	//| Position | vec3 | The local translation of the bone |
	//| Parent bone index | index | See Index Types |
	//| Layer | int | Deformation hierarchy |
	//| Flags | flag[2] | See Bone Flags |
	//| Tail position | vec3 / index | If indexed tail position flag is set then this is a bone index |
	//| Inherit bone | ~| Used if either of the inherit flags are set.See Inherit Bone |
	//| Fixed axis | ~| Used if fixed axis flag is set.See Bone Fixed Axis |
	//| Local co-ordinate | ~| Used if local co - ordinate flag is set.See Bone Local Co-ordinate |
	//| External parent | ~| Used if external parent deform flag is set.See Bone External Parent |
	//| IK | ~| Used if IK flag is set.See Bone IK |

	//动画重定向
	//交给第三方动画引擎
	
	int32_t BoneData_BoneCount;
	pFilePMX->Read(&BoneData_BoneCount, sizeof(int32_t));
	BoneData_BoneCount = ::PTG_Assert_LEToH(BoneData_BoneCount);

	for (int32_t IndexBone = 0; IndexBone < BoneData_BoneCount; ++IndexBone)
	{
		char *BoneNameLocal = pFunAllocAndReadText(pFilePMX);

		//-----------------------------------------------------------
		PTGA_Joint JointToInsert;

		size_t Len = ::strlen(BoneNameLocal);
		Len = Len < 31U ? Len : 31U;
		assert(sizeof(char) == 1U);
		::memcpy(JointToInsert.m_Name, BoneNameLocal, Len);
		JointToInsert.m_Name[Len] = '\0';

		Skeloton.push_back(JointToInsert);
		//-----------------------------------------------------------

		::PTSMemoryAllocator_Free(BoneNameLocal);

		char *BoneNameUniversal = pFunAllocAndReadText(pFilePMX);
		::PTSMemoryAllocator_Free(BoneNameUniversal);

		float Position[3];
		pFilePMX->Read(Position, sizeof(float) * 3U);
		Position[0] = ::PTG_Assert_LEToH(Position[0]);
		Position[1] = ::PTG_Assert_LEToH(Position[1]);
		Position[2] = ::PTG_Assert_LEToH(Position[2]);

		int32_t ParentBoneIndex = pFunReadBoneIndex(pFilePMX); //-1 -> Root ???
		
		int32_t Layer;
		pFilePMX->Read(&Layer, sizeof(int32_t));

		//Bone Flags

		//Bones can have the following bit flags :

		//| Bit index | Name | Set effect | Version	|
		//| : -------- - : | ---------------------- - | ------------------------------------------ - | ---------- - |
		//| 0 | Indexed tail position | Is the tail position a vec3 or bone index | 2.0		|
		//| 1 | Rotatable | Enables rotation | 2.0		|
		//| 2 | Translatable | Enables translation(shear) | 2.0		|
		//| 3 | Is visible | ? ? ? | 2.0		|
		//| 4 | Enabled | ? ? ? | 2.0		|
		//| 5 | IK | Use inverse kinematics(physics) | 2.0		|
		//| 8 | Inherit rotation | Rotation inherits from another bone | 2.0		|
		//| 9 | Inherit translation | Translation inherits from another bone | 2.0		|
		//| 10 | Fixed axis | The bone's shaft is fixed in a direction	|2.0		|
		//| 11 | Local co - ordinate | ? ? ? | 2.0		|
		//| 12 | Physics after deform | ? ? ? | 2.0		|
		//| 13 | External parent deform | ? ? ? | 2.0 |

		uint16_t BoneFlags;
		pFilePMX->Read(&BoneFlags, sizeof(uint16_t));
		BoneFlags = ::PTG_Assert_LEToH(BoneFlags);

		if (BoneFlags & 0X1U)
		{
			uint32_t TailPosition = pFunReadBoneIndex(pFilePMX);
		}
		else
		{
			float TailPosition[3]; //Tail Means Child ???
			pFilePMX->Read(TailPosition, sizeof(float) * 3);
			TailPosition[0] = ::PTG_Assert_LEToH(TailPosition[0]);
			TailPosition[1] = ::PTG_Assert_LEToH(TailPosition[1]);
			TailPosition[2] = ::PTG_Assert_LEToH(TailPosition[2]);
		}
		
		if (BoneFlags & 0X300U)
		{
			//Inherit Bone

			//| Name | Type | Notes														|
			//|------------------ - | ------ - | ---------------------------------------------------------- - |
			//|Parent index | index | See Index Types											|
			//|Parent influence | float | Weight of how much influence this parent has on this bone |

			int32_t ParentBoneIndex = pFunReadBoneIndex(pFilePMX);

			float ParentInfluence;
			pFilePMX->Read(&ParentInfluence, sizeof(float));
			ParentInfluence = ::PTG_Assert_LEToH(ParentInfluence);
		}

		if (BoneFlags & 0X400U)
		{
			//Bone Fixed Axis

			//| Name | Type | Notes						|
			//|-------------- - | ------ - | -------------------------- - |
			//|Axis direction | vec3 | Direction this bone points |

			float AxisDirection[3];
			pFilePMX->Read(AxisDirection, sizeof(float) * 3);
			AxisDirection[0] = ::PTG_Assert_LEToH(AxisDirection[0]);
			AxisDirection[1] = ::PTG_Assert_LEToH(AxisDirection[1]);
			AxisDirection[2] = ::PTG_Assert_LEToH(AxisDirection[2]);
		}

		if (BoneFlags & 0X800U)
		{
			//Bone Local Co-ordinate

			//| Name | Type | Notes	|
			//|---------- - | ------ - | ------ - |
			//|X vector | vec3 | ? ? ? |
			//|Z vector | vec3 | ? ? ? |

			float BoneLocalCoordinate_XVector[3];
			float BoneLocalCoordinate_ZVector[3];
			pFilePMX->Read(BoneLocalCoordinate_XVector, sizeof(float) * 3);
			pFilePMX->Read(BoneLocalCoordinate_ZVector, sizeof(float) * 3);
			BoneLocalCoordinate_XVector[0] = ::PTG_Assert_LEToH(BoneLocalCoordinate_XVector[0]);
			BoneLocalCoordinate_XVector[1] = ::PTG_Assert_LEToH(BoneLocalCoordinate_XVector[1]);
			BoneLocalCoordinate_XVector[2] = ::PTG_Assert_LEToH(BoneLocalCoordinate_XVector[2]);
			BoneLocalCoordinate_ZVector[0] = ::PTG_Assert_LEToH(BoneLocalCoordinate_ZVector[0]);
			BoneLocalCoordinate_ZVector[1] = ::PTG_Assert_LEToH(BoneLocalCoordinate_ZVector[1]);
			BoneLocalCoordinate_ZVector[2] = ::PTG_Assert_LEToH(BoneLocalCoordinate_ZVector[2]);
		}

		if (BoneFlags & 0X2000U)
		{
			//Bone External Parent

			//| Name | Type | Notes	|
			//|-------------- - | ------ - | ------ - |
			//| Parent index | index | ? ? ? |
			
			int32_t ParentIndex = pFunReadBoneIndex(pFilePMX);
		}

		if (BoneFlags & 0X20U)
		{
			//Bone IK

			//| Name | Type | Notes										|
			//|-------------- - | ------ - | ------------------------------------------ - |
			//|Target index | index | See Index Types							|
			//|Loop count | int | ? ? ? |
			//|Limit radian | float | ? ? ? |
			//|Link count | int | How many bones this IK links with			|
			//|IK links | ~[N] | N is link count; can be zero.See IK Links |

			int32_t TargetIndex = pFunReadBoneIndex(pFilePMX);
			
			int32_t LoopCount;
			pFilePMX->Read(&LoopCount, sizeof(int32_t));
			LoopCount = ::PTG_Assert_LEToH(LoopCount);

			float LimitRadian;
			pFilePMX->Read(&LimitRadian, sizeof(float));
			LimitRadian = ::PTG_Assert_LEToH(LimitRadian);

			int32_t LinkCount;
			pFilePMX->Read(&LinkCount, sizeof(int32_t));
			LinkCount = ::PTG_Assert_LEToH(LinkCount);

			for (int32_t IndexLink = 0; IndexLink < LinkCount; ++IndexLink)
			{
				//IK Links

				//| Name | Type | Notes											|
				//|------------------ - | ------ - | ---------------------------------------------- - |
				//| Bone index | index | See Index Types								|
				//| Has limits | byte | When equal to 1, use angle limits				|
				//| IK Angle limits | ~| Used if has limits is 1. See IK Angle Limit |

				int32_t IKLinks_BoneIndex = pFunReadBoneIndex(pFilePMX);

				uint8_t IKLinks_HasLimits;
				pFilePMX->Read(&IKLinks_HasLimits, sizeof(uint8_t));
				assert(IKLinks_HasLimits == 0U || IKLinks_HasLimits == 1U);

				if (IKLinks_HasLimits == 1U)
				{
					//IK Angle Limit

					//| Name | Type | Notes						|
					//|---------- - | ------ - | -------------------------- - |
					//| Limit min | vec3 | Minimum angle(radians)	|
					//| Limit max | vec3 | Maximum angle(radians) |
					float IKAngleLimit_LimitMin[3];
					float IKAngleLimit_LimitMax[3];
					pFilePMX->Read(IKAngleLimit_LimitMin, sizeof(float) * 3);
					pFilePMX->Read(IKAngleLimit_LimitMax, sizeof(float) * 3);
					IKAngleLimit_LimitMin[0] = ::PTG_Assert_LEToH(IKAngleLimit_LimitMin[0]);
					IKAngleLimit_LimitMin[1] = ::PTG_Assert_LEToH(IKAngleLimit_LimitMin[1]);
					IKAngleLimit_LimitMin[2] = ::PTG_Assert_LEToH(IKAngleLimit_LimitMin[2]);
					IKAngleLimit_LimitMax[0] = ::PTG_Assert_LEToH(IKAngleLimit_LimitMax[0]);
					IKAngleLimit_LimitMax[1] = ::PTG_Assert_LEToH(IKAngleLimit_LimitMax[1]);
					IKAngleLimit_LimitMax[2] = ::PTG_Assert_LEToH(IKAngleLimit_LimitMax[2]);
				}
			}	
		}
	}

	//Morph Data

	//The morph section starts with an int defining how many morphs there are(Note that it is a signed int).

	//| Name | Type | Notes												|
	//|---------------------- - | ------ - | -------------------------------------------------- - |
	//| Morph name local | text | Handy name for the morph(Usually Japanese)		|
	//| Morph name universal | text | Handy name for the morph(Usually English)			|
	//| Panel type | byte | See Morph Panel Types								|
	//| Morph type | byte | See Morph Types									|
	//| Offset size | int | Number of morph offsets							|
	//| Offset data | ~[N] | N is offset count; can be zero.See Offset Data |

	int32_t MorphData_MorphCount;
	pFilePMX->Read(&MorphData_MorphCount, sizeof(int32_t));
	MorphData_MorphCount = ::PTG_Assert_LEToH(MorphData_MorphCount);

	for (int32_t IndexMorph = 0; IndexMorph < MorphData_MorphCount; ++IndexMorph)
	{
		//MorphNameLocal
		pFunReadAndIgnoreText(pFilePMX);
		//MorphNameUniversal
		pFunReadAndIgnoreText(pFilePMX);

		uint8_t Morph_PanelType;
		pFilePMX->Read(&Morph_PanelType, sizeof(uint8_t));

		uint8_t Morph_MorphType;
		pFilePMX->Read(&Morph_MorphType, sizeof(uint8_t));

		int32_t OffsetSize;
		pFilePMX->Read(&OffsetSize, sizeof(int32_t));

		switch (Morph_MorphType)
		{
		case 0:
		{
			//Group

			//| Name | Type | Notes						|
			//|-------------- - | ------ - | -------------------------- - |
			//| Morph index | index | See Index Types			|
			//| Influence | float | Weight of indexed morph |

			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				int32_t MorphIndex = pFunReadMorphIndex(pFilePMX);
				
				float Influence;
				pFilePMX->Read(&Influence, sizeof(float));
				Influence = ::PTG_Assert_LEToH(Influence);
			}
		}
		break;
		case 1:
		{
			//目标变形动画

			//Vertex

			//| Name | Type | Notes							|
			//|-------------- - | ------ - | ------------------------------ - |
			//| Vertex index | index | See Index Types				|
			//| Translation | vec3 | Translation to apply to vertex |

			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				uint32_t VertexIndex = pFunReadVertexIndex(pFilePMX);

				float Translation[3];
				pFilePMX->Read(&Translation, sizeof(float) * 3U);
				Translation[0] = ::PTG_Assert_LEToH(Translation[0]);
				Translation[1] = ::PTG_Assert_LEToH(Translation[1]);
				Translation[2] = ::PTG_Assert_LEToH(Translation[2]);
			}
		}
		break;
		case 2:
		{
			//骨骼动画

			//Bone

			//| Name | Type | Notes							|
			//|-------------- - | ------ - | ------------------------------ - |
			//| Bone index | index | See Index Types				|
			//| Translation | vec3 | Translation to apply to bone	|
			//| Rotation | vec4 | Rotation to apply to bone |

			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				int32_t BoneIndex = pFunReadBoneIndex(pFilePMX);

				float Translation[3];
				pFilePMX->Read(&Translation, sizeof(float) * 3U);
				Translation[0] = ::PTG_Assert_LEToH(Translation[0]);
				Translation[1] = ::PTG_Assert_LEToH(Translation[1]);
				Translation[2] = ::PTG_Assert_LEToH(Translation[2]);

				float Rotation[4];
				pFilePMX->Read(&Rotation, sizeof(float) * 4U);
				Rotation[0] = ::PTG_Assert_LEToH(Rotation[0]);
				Rotation[1] = ::PTG_Assert_LEToH(Rotation[1]);
				Rotation[2] = ::PTG_Assert_LEToH(Rotation[2]);
				Rotation[3] = ::PTG_Assert_LEToH(Rotation[3]);
			}
		}
		break;
		case 3:
		case 4: //UV ext1
		case 5: //UV ext2
		case 6: //UV ext3
		case 7: //UV ext4
		{
			//UV

			//| Name | Type | Notes									|
			//|-------------- - | ------ - | -------------------------------------- - |
			//| Vertex index | index | See Index Types						|
			//| Floats | vec4 | What these do depends on the UV ext |


			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				uint32_t VertexIndex = pFunReadVertexIndex(pFilePMX);

				float Floats[4];
				pFilePMX->Read(&Floats, sizeof(float) * 4U);
				Floats[0] = ::PTG_Assert_LEToH(Floats[0]);
				Floats[1] = ::PTG_Assert_LEToH(Floats[1]);
				Floats[2] = ::PTG_Assert_LEToH(Floats[2]);
				Floats[3] = ::PTG_Assert_LEToH(Floats[3]);
			}
		}
		break;
		case 8:
		{
			//Material

			//| Name | Type | Notes									|
			//|------------------ - | ------ - | -------------------------------------- - |
			//| Material index | index | See Index Types. - 1 is all materials	|
			//|               | byte  |										|
			//| Diffuse | vec4	|										|
			//| Specular | vec3	|										|
			//| Specularity | float	|										|
			//| Ambient | vec3	|										|
			//| Edge colour | vec4	|										|
			//| Edge size | float	|										|
			//| Texture tint | vec4	|										|
			//| Environment tint | vec4	|										|
			//| Toon tint | vec4	|										|


			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				int32_t MaterialIndex = pFunReadMaterialIndex(pFilePMX);

				uint8_t Dummy;
				pFilePMX->Read(&Dummy, sizeof(uint8_t));

				float Diffuse[4];
				pFilePMX->Read(Diffuse, sizeof(float) * 4);
				Diffuse[0] = ::PTG_Assert_LEToH(Diffuse[0]);
				Diffuse[1] = ::PTG_Assert_LEToH(Diffuse[1]);
				Diffuse[2] = ::PTG_Assert_LEToH(Diffuse[2]);
				Diffuse[3] = ::PTG_Assert_LEToH(Diffuse[3]);

				float Specular[3];
				pFilePMX->Read(Specular, sizeof(float) * 3);
				Specular[0] = ::PTG_Assert_LEToH(Specular[0]);
				Specular[1] = ::PTG_Assert_LEToH(Specular[1]);
				Specular[2] = ::PTG_Assert_LEToH(Specular[2]);

				float Specularity;
				pFilePMX->Read(&Specularity, sizeof(float));
				Specularity = ::PTG_Assert_LEToH(Specularity);

				float Ambient[3];
				pFilePMX->Read(Ambient, sizeof(float) * 3);
				Ambient[0] = ::PTG_Assert_LEToH(Ambient[0]);
				Ambient[1] = ::PTG_Assert_LEToH(Ambient[1]);
				Ambient[2] = ::PTG_Assert_LEToH(Ambient[2]);

				float EdgeColor[4];
				pFilePMX->Read(EdgeColor, sizeof(float) * 4);
				EdgeColor[0] = ::PTG_Assert_LEToH(EdgeColor[0]);
				EdgeColor[1] = ::PTG_Assert_LEToH(EdgeColor[1]);
				EdgeColor[2] = ::PTG_Assert_LEToH(EdgeColor[2]);
				EdgeColor[3] = ::PTG_Assert_LEToH(EdgeColor[3]);

				float EdgeSize;
				pFilePMX->Read(&EdgeSize, sizeof(float));
				EdgeSize = ::PTG_Assert_LEToH(EdgeSize);

				float TextureTint[4];
				pFilePMX->Read(TextureTint, sizeof(float) * 4);
				TextureTint[0] = ::PTG_Assert_LEToH(TextureTint[0]);
				TextureTint[1] = ::PTG_Assert_LEToH(TextureTint[1]);
				TextureTint[2] = ::PTG_Assert_LEToH(TextureTint[2]);
				TextureTint[3] = ::PTG_Assert_LEToH(TextureTint[3]);

				float EnvironmentTint[4];
				pFilePMX->Read(EnvironmentTint, sizeof(float) * 4);
				EnvironmentTint[0] = ::PTG_Assert_LEToH(EnvironmentTint[0]);
				EnvironmentTint[1] = ::PTG_Assert_LEToH(EnvironmentTint[1]);
				EnvironmentTint[2] = ::PTG_Assert_LEToH(EnvironmentTint[2]);
				EnvironmentTint[3] = ::PTG_Assert_LEToH(EnvironmentTint[3]);

				float ToonTint[4];
				pFilePMX->Read(ToonTint, sizeof(float) * 4);
				ToonTint[0] = ::PTG_Assert_LEToH(ToonTint[0]);
				ToonTint[1] = ::PTG_Assert_LEToH(ToonTint[1]);
				ToonTint[2] = ::PTG_Assert_LEToH(ToonTint[2]);
				ToonTint[3] = ::PTG_Assert_LEToH(ToonTint[3]);
			}
		}
		break;
		case 9:
		{
			//Flip

			//| Name | Type | Notes						|
			//|-------------- - | ---------- - | -------------------------- - |
			//| Morph index | index | See Index Types			|
			//| Influence | float | Weight of indexed morph |


			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				int32_t MorphIndex = pFunReadMorphIndex(pFilePMX);

				float Influence;
				pFilePMX->Read(&Influence, sizeof(float));
				Influence = ::PTG_Assert_LEToH(Influence);
			}
		}
		break;
		case 10:
		{

			//Impulse

			//| Name | Type | Notes				|
			//|------------------ - | ------ - | ------------------ - |
			//| Rigid body index | index | See Index Types	|
			//| Local flag | byte	|					|
			//| Movement speed | vec3	|					|
			//| Rotation torque | vec3	|					|


			for (int32_t IndexOffset = 0; IndexOffset < OffsetSize; ++IndexOffset)
			{
				int32_t RigidbodyIndex = pFunReadRigidbodyIndex(pFilePMX);
				uint8_t LocalFlag;
				pFilePMX->Read(&LocalFlag, sizeof(uint8_t));

				float MovementSpeed[3];
				pFilePMX->Read(MovementSpeed, sizeof(float) * 3);
				MovementSpeed[0] = ::PTG_Assert_LEToH(MovementSpeed[0]);
				MovementSpeed[1] = ::PTG_Assert_LEToH(MovementSpeed[1]);
				MovementSpeed[2] = ::PTG_Assert_LEToH(MovementSpeed[2]);

				float RotationTorque[3];
				pFilePMX->Read(RotationTorque, sizeof(float) * 3);
				RotationTorque[0] = ::PTG_Assert_LEToH(RotationTorque[0]);
				RotationTorque[1] = ::PTG_Assert_LEToH(RotationTorque[1]);
				RotationTorque[2] = ::PTG_Assert_LEToH(RotationTorque[2]);
			}
		}
		break;
		}
		
	}
	
	//Display Data

	//The display frame section starts with an int defining how many frames there are(Note that it is a signed int).

	//| Name | Type | Notes											|
	//|---------------------- - | ------ - | ---------------------------------------------- - |
	//| Display name local | text | Handy name for the display(Usually Japanese) |
	//| Display name universal | text | Handy name for the display(Usually English) |
	//| Special flag | byte | 0 = normal frame, 1 = special frame |
	//| Frame count | int | How many frames are in this display |
	//| Frames | ~[N] | N is frame count.See Frame Data |

	int32_t DisplayData_DisplayCount;
	pFilePMX->Read(&DisplayData_DisplayCount, sizeof(int32_t));
	DisplayData_DisplayCount = ::PTG_Assert_LEToH(DisplayData_DisplayCount);

	for (int32_t IndexDisplay = 0; IndexDisplay < DisplayData_DisplayCount; ++IndexDisplay)
	{
		//DisplayNameLocal
		pFunReadAndIgnoreText(pFilePMX);
		//DisplayNameUniversal
		pFunReadAndIgnoreText(pFilePMX);

		uint8_t DisplayData_SpecialFlag;
		pFilePMX->Read(&DisplayData_SpecialFlag, sizeof(uint8_t));
		assert(DisplayData_SpecialFlag == 0U || DisplayData_SpecialFlag == 1U);

		int32_t DisplayData_FrameCount;
		pFilePMX->Read(&DisplayData_FrameCount, sizeof(int32_t));
		DisplayData_FrameCount = ::PTG_Assert_LEToH(DisplayData_FrameCount);

		for (int32_t IndexFrame = 0; IndexFrame < DisplayData_FrameCount; ++IndexFrame)
		{
			//Frame Data

			//| Name | Type | Notes				|
			//|---------- - | ------ - | ------------------ - |
			//| Frame type | byte | See Frame Type		|
			//| Frame data | ~| See Frame Datas |

			uint8_t FrameData_FrameType;
			pFilePMX->Read(&FrameData_FrameType, sizeof(uint8_t));
			assert(FrameData_FrameType == 0U || FrameData_FrameType == 1U);

			//Frame Type

			//| Index | Type | Notes | Version	|
			//|:---- - : | ------ - | ------ - | ---------- - |
			//| 0 | Bone	|		|2.0		|
			//| 1 | Morph	|		|2.0 |

			switch (FrameData_FrameType)
			{
			case 0:
			{
				//Bone Frame Data

				//| Name | Type | Notes				|
				//|---------- - | ------ - | ------------------ - |
				//| Bone index | index | See Index Types |

				int32_t BoneFrameData_BoneIndex = pFunReadBoneIndex(pFilePMX);

			}
			break;
			case 1:
			{
				//Morph Frame Data

				//| Name | Type | Notes				|
				//|-------------- - | ------ - | ------------------ - |
				//| Morph index | index | See Index Types |

				int32_t MorphFrameData_MorphIndex = pFunReadMorphIndex(pFilePMX);

			}
			break;
			default:
				assert(0);
			}

		}
	}
	
	//Rigid Body Data

	//The rigid body section starts with an int defining how many rigid bodies there are(Note that it is a signed int).

	//| Name | Type | Notes												|
	//| -------------------------- - | ------ - | -------------------------------------------------- - |
	//| Rigid body name local | text | Handy name for the rigid body(Usually Japanese) |
	//| Rigid body name universal | text | Handy name for the rigid body(Usually English) |
	//| Related bone index | index | See Index Types |
	//| Group id | byte | Group id |
	//| Non-collision group | short | Non collision mask |
	//| Shape | byte | See Shape Type |
	//| Shape size | vec3 | XYZ bounds of the shape |
	//| Shape position | vec3 | XYZ shape location |
	//| Shape rotation | vec3 | XYZ shape rotation angles in radians |
	//| Mass | float |													|
	//| Move attenuation | float |													|
	//| Rotation damping | float |													|
	//| Repulsion | float |													|
	//| Friction force | float |													|
	//| Physics mode | byte | See Physics Modes |

	int32_t RigidBodyData_RigidBodyCount;
	CountDone = pFilePMX->Read(&RigidBodyData_RigidBodyCount, sizeof(int32_t));
	assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
	RigidBodyData_RigidBodyCount = ::PTG_Assert_LEToH(RigidBodyData_RigidBodyCount);

	for (int32_t IndexRigidBody = 0; IndexRigidBody < RigidBodyData_RigidBodyCount; ++IndexRigidBody)
	{
		//RigidBodyNameLocal
		pFunReadAndIgnoreText(pFilePMX);
		//RigidBodyUniversal
		pFunReadAndIgnoreText(pFilePMX);

		int32_t RigidBodyData_RelatedBoneIndex = pFunReadBoneIndex(pFilePMX);
		
		uint8_t RigidBodyData_GroupId;
		CountDone = pFilePMX->Read(&RigidBodyData_GroupId, sizeof(uint8_t));
		assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

		int16_t RigidBodyData_NonCollisionGroup;
		CountDone = pFilePMX->Read(&RigidBodyData_NonCollisionGroup, sizeof(int16_t));
		assert(static_cast<size_t>(CountDone) == sizeof(int16_t));
		RigidBodyData_NonCollisionGroup = ::PTG_Assert_LEToH(RigidBodyData_NonCollisionGroup);

		uint8_t RigidBodyData_Shape;
		CountDone = pFilePMX->Read(&RigidBodyData_Shape, sizeof(uint8_t));
		assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

		//Shape Type

		//| Index | Type | Notes | Version	|
		//|:---- - : | ---------- - | ------ - | ---------- - |
		//| 0 | Sphere		|		|2.0		|
		//| 1 | Box		|		|2.0		|
		//| 2 | Capsule	|		|2.0 |

		assert(RigidBodyData_Shape == 0U || RigidBodyData_Shape == 1U || RigidBodyData_Shape == 2U);

		float RigidBodyData_ShapeSize[3];
		CountDone = pFilePMX->Read(RigidBodyData_ShapeSize, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		RigidBodyData_ShapeSize[0] = ::PTG_Assert_LEToH(RigidBodyData_ShapeSize[0]);
		RigidBodyData_ShapeSize[1] = ::PTG_Assert_LEToH(RigidBodyData_ShapeSize[1]);
		RigidBodyData_ShapeSize[2] = ::PTG_Assert_LEToH(RigidBodyData_ShapeSize[2]);

		float RigidBodyData_ShapePosition[3];
		CountDone = pFilePMX->Read(RigidBodyData_ShapePosition, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		RigidBodyData_ShapePosition[0] = ::PTG_Assert_LEToH(RigidBodyData_ShapePosition[0]);
		RigidBodyData_ShapePosition[1] = ::PTG_Assert_LEToH(RigidBodyData_ShapePosition[1]);
		RigidBodyData_ShapePosition[2] = ::PTG_Assert_LEToH(RigidBodyData_ShapePosition[2]);

		float RigidBodyData_ShapeRotation[3];
		CountDone = pFilePMX->Read(RigidBodyData_ShapeRotation, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		RigidBodyData_ShapeRotation[0] = ::PTG_Assert_LEToH(RigidBodyData_ShapeRotation[0]);
		RigidBodyData_ShapeRotation[1] = ::PTG_Assert_LEToH(RigidBodyData_ShapeRotation[1]);
		RigidBodyData_ShapeRotation[2] = ::PTG_Assert_LEToH(RigidBodyData_ShapeRotation[2]);

		float RigidBodyData_Mass;
		CountDone = pFilePMX->Read(&RigidBodyData_Mass, sizeof(float));
		assert(static_cast<size_t>(CountDone) == sizeof(float));

		float RigidBodyData_MoveAttenuation;
		CountDone = pFilePMX->Read(&RigidBodyData_MoveAttenuation, sizeof(float));
		assert(static_cast<size_t>(CountDone) == sizeof(float));

		float RigidBodyData_RotationDampin;
		CountDone = pFilePMX->Read(&RigidBodyData_RotationDampin, sizeof(float));
		assert(static_cast<size_t>(CountDone) == sizeof(float));

		float RigidBodyData_Repulsion;
		CountDone = pFilePMX->Read(&RigidBodyData_Repulsion, sizeof(float));
		assert(static_cast<size_t>(CountDone) == sizeof(float));

		float RigidBodyData_FrictionForce;
		CountDone = pFilePMX->Read(&RigidBodyData_FrictionForce, sizeof(float));
		assert(static_cast<size_t>(CountDone) == sizeof(float));

		uint8_t RigidBodyData_PhysicsMode;
		CountDone = pFilePMX->Read(&RigidBodyData_PhysicsMode, sizeof(uint8_t));
		assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

		//Physics Modes

		//| Index | Type | Notes | Version	|
		//| :---- - : | -------------- - | ------------------------------------------ - | ---------- - |
		//| 0 | Follow bone | Rigid body sticks to bone | 2.0 |
		//| 1 | Physics | Rigid body uses gravity | 2.0 |
		//| 2 | Physics + bone | Rigid body uses gravity pivoted to bone | 2.0 |

		assert(RigidBodyData_PhysicsMode == 0U || RigidBodyData_PhysicsMode == 1U || RigidBodyData_PhysicsMode == 2U);
	}
	
	//Joint Data

	//The joint section starts with an int defining how many joints there are(Note that it is a signed int).

	//| Name | Type | Notes												|
	//|---------------------- - | ------ - | -------------------------------------------------- - |
	//|Joint name local | text | Handy name for the rigid body(Usually Japanese)	|
	//|Joint name universal | text | Handy name for the rigid body(Usually English)	|
	//|Type | byte | See Joint Type										|
	//|Rigid body index A | index | See Index Types									|
	//|Rigid body index B | index	|													|
	//|Position | vec3	|													|
	//|Rotation | vec3 | Rotation angle radian								|
	//|Position minimum | vec3 | Minimum position value								|
	//|Position maximum | vec3 | Maximum position value								|
	//|Rotation minimum | vec3 | Minimum rotation value								|
	//|Rotation maximum | vec3 | Maximum rotation value								|
	//|Position spring | vec3	|													|
	//|Rotation spring | vec3	|													|
	
	int32_t JointData_SoftBodyCount;
	CountDone = pFilePMX->Read(&JointData_SoftBodyCount, sizeof(int32_t));
	assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
	JointData_SoftBodyCount = ::PTG_Assert_LEToH(JointData_SoftBodyCount);

	for (int32_t IndexSoftBody = 0; IndexSoftBody < JointData_SoftBodyCount; ++IndexSoftBody)
	{
		//JointNameLocal
		pFunReadAndIgnoreText(pFilePMX);
		//JointUniversal
		pFunReadAndIgnoreText(pFilePMX);

		uint8_t JointData_Type;
		CountDone = pFilePMX->Read(&JointData_Type, sizeof(uint8_t));
		assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

		//Joint Type

		//| Index | Type | Notes | Version	|
		//| :---- - : | -------------- - | ------ - | ---------- - |
		//| 0 | Spring 6DOF	|		|2.0		|
		//| 1 | 6DOF			|		|2.1		|
		//| 2 | P2P			|		|2.1		|
		//| 3 | ConeTwist		|		|2.1		|
		//| 4 | Slider			|		|2.1		|
		//| 5 | Hinge			|		|2.1 |

		assert(JointData_Type == 0U || JointData_Type == 1U || JointData_Type == 2U || JointData_Type == 3U || JointData_Type == 4U || JointData_Type == 5U);

		int32_t JointData_RigidBodyIndexA = pFunReadRigidbodyIndex(pFilePMX);
		int32_t JointData_RigidBodyIndexB = pFunReadRigidbodyIndex(pFilePMX);

		float JointData_Position[3];
		CountDone = pFilePMX->Read(JointData_Position, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_Position[0] = ::PTG_Assert_LEToH(JointData_Position[0]);
		JointData_Position[1] = ::PTG_Assert_LEToH(JointData_Position[1]);
		JointData_Position[2] = ::PTG_Assert_LEToH(JointData_Position[2]);

		float JointData_Rotation[3];
		CountDone = pFilePMX->Read(JointData_Rotation, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_Rotation[0] = ::PTG_Assert_LEToH(JointData_Rotation[0]);
		JointData_Rotation[1] = ::PTG_Assert_LEToH(JointData_Rotation[1]);
		JointData_Rotation[2] = ::PTG_Assert_LEToH(JointData_Rotation[2]);

		float JointData_PositionMinimum[3];
		CountDone = pFilePMX->Read(JointData_PositionMinimum, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_PositionMinimum[0] = ::PTG_Assert_LEToH(JointData_PositionMinimum[0]);
		JointData_PositionMinimum[1] = ::PTG_Assert_LEToH(JointData_PositionMinimum[1]);
		JointData_PositionMinimum[2] = ::PTG_Assert_LEToH(JointData_PositionMinimum[2]);

		float JointData_PositionMaximum[3];
		CountDone = pFilePMX->Read(JointData_PositionMaximum, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_PositionMaximum[0] = ::PTG_Assert_LEToH(JointData_PositionMaximum[0]);
		JointData_PositionMaximum[1] = ::PTG_Assert_LEToH(JointData_PositionMaximum[1]);
		JointData_PositionMaximum[2] = ::PTG_Assert_LEToH(JointData_PositionMaximum[2]);

		float JointData_RotationMinimum[3];
		CountDone = pFilePMX->Read(JointData_RotationMinimum, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_RotationMinimum[0] = ::PTG_Assert_LEToH(JointData_RotationMinimum[0]);
		JointData_RotationMinimum[1] = ::PTG_Assert_LEToH(JointData_RotationMinimum[1]);
		JointData_RotationMinimum[2] = ::PTG_Assert_LEToH(JointData_RotationMinimum[2]);

		float JointData_RotationMaximum[3];
		CountDone = pFilePMX->Read(JointData_PositionMaximum, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_RotationMaximum[0] = ::PTG_Assert_LEToH(JointData_RotationMaximum[0]);
		JointData_RotationMaximum[1] = ::PTG_Assert_LEToH(JointData_RotationMaximum[1]);
		JointData_RotationMaximum[2] = ::PTG_Assert_LEToH(JointData_RotationMaximum[2]);

		float JointData_PositionSpring[3];
		CountDone = pFilePMX->Read(JointData_PositionSpring, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_PositionSpring[0] = ::PTG_Assert_LEToH(JointData_PositionSpring[0]);
		JointData_PositionSpring[1] = ::PTG_Assert_LEToH(JointData_PositionSpring[1]);
		JointData_PositionSpring[2] = ::PTG_Assert_LEToH(JointData_PositionSpring[2]);

		float JointData_RotationSpring[3];
		CountDone = pFilePMX->Read(JointData_RotationSpring, sizeof(float) * 3U);
		assert(static_cast<size_t>(CountDone) == sizeof(float) * 3U);
		JointData_RotationSpring[0] = ::PTG_Assert_LEToH(JointData_RotationSpring[0]);
		JointData_RotationSpring[1] = ::PTG_Assert_LEToH(JointData_RotationSpring[1]);
		JointData_RotationSpring[2] = ::PTG_Assert_LEToH(JointData_RotationSpring[2]);
	}

	//Soft Body Data

	//The soft body section starts with an int defining how many soft bodies there are(Note that it is a signed int).

	//| Name | Type | Notes														|
	//|-------------------------- - | ------ - | ---------------------------------------------------------- - |
	//|Soft body name local | text | Handy name for the soft body(Usually Japanese)			|
	//|Soft body name universal | text | Handy name for the soft body(Usually English)				|
	//|Shape | byte | See Shape Types											|
	//|Material index | index | See Index Types											|
	//|Group | byte | Group id													|
	//|No collision mask | short | Non collision mask											|
	//|Flags | flag | See Flags													|
	//|B - link create distance | int | ? ? ? |
	//|Number of clusters | int | ? ? ? |
	//|Total mass | float	|															|
	//|Collision margin | float	|															|
	//|Aerodynamics model | int | See Aerodynamics Models									|
	//|Config VCF | float | Velocities correction factor(Baumgarte)					|
	//|Config DP | float | Damping coefficient										|
	//|Config DG | float | Drag coefficient											|
	//|Config LF | float | Lift coefficient											|
	//|Config PR | float | Pressure coefficient										|
	//|Config VC | float | Volume conversation coefficient							|
	//|Config DF | float | Dynamic friction coefficient								|
	//|Config MT | float | Pose matching coefficient									|
	//|Config CHR | float | Rigid contacts hardness									|
	//|Config KHR | float | Kinetic contacts hardness									|
	//|Config SHR | float | Soft contacts hardness										|
	//|Config AHR | float | Anchors hardness											|
	//|Cluster SRHR_CL | float | Soft vs rigid hardness										|
	//|Cluster SKHR_CL | float | Soft vs kinetic hardness									|
	//|Cluster SSHR_CL | float | Soft vs soft hardness										|
	//|Cluster SR_SPLT_CL | float | Soft vs rigid impulse split								|
	//|Cluster SK_SPLT_CL | float | Soft vs kinetic impulse split								|
	//|Cluster SS_SPLT_CL | float | Soft vs soft impulse split									|
	//|Interation V_IT | int | Velocities solver iterations								|
	//|Interation P_IT | int | Positions solver iterations								|
	//|Interation D_IT | int | Drift solver iterations									|
	//|Interation C_IT | int | Cluster solver iterations									|
	//|Material LST | int | Linear stiffness coefficient								|
	//|Material AST | int | Area / Angular stiffness coefficient						|
	//|Material VST | int | Volume stiffness coefficient								|
	//|Anchor rigid body count | int | How many anchoring rigid bodies there are					|
	//|Anchor rigid bodies | ~[N] | N is the anchor rigid body count.See Anchor Rigid Bodies	|
	//|Vertex pin count | int | How many vertex pins there are								|
	//|Vertex pins | ~[N] | N is the vertex pin count.See Vertex Pins |

	int32_t SoftBodyData_SoftBodyCount;
	CountDone = pFilePMX->Read(&SoftBodyData_SoftBodyCount, sizeof(int32_t));
	if (Header_Version == 2.1f)
	{
		assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
		SoftBodyData_SoftBodyCount = ::PTG_Assert_LEToH(SoftBodyData_SoftBodyCount);

		for (int32_t IndexSoftBody = 0; IndexSoftBody < SoftBodyData_SoftBodyCount; ++IndexSoftBody)
		{
			//SoftBodyNameLocal
			pFunReadAndIgnoreText(pFilePMX);
			//SoftBodyUniversal
			pFunReadAndIgnoreText(pFilePMX);

			uint8_t SoftBodyData_Shape;
			CountDone = pFilePMX->Read(&SoftBodyData_Shape, sizeof(uint8_t));
			assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

			//Shape Type

			//| Index | Type | Notes | Version	|
			//| :---- - : | ---------- - | ------ - | ---------- - |
			//| 0 | TriMesh	|		|2.1		|
			//| 1 | Rope		|		|2.1 |

			assert(SoftBodyData_Shape == 0U || SoftBodyData_Shape == 1U);

			int32_t SoftBodyData_MaterialIndex = pFunReadMaterialIndex(pFilePMX);

			uint8_t SoftBodyData_GroupId;
			CountDone = pFilePMX->Read(&SoftBodyData_GroupId, sizeof(uint8_t));
			assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

			int16_t oftBodyData_NonCollisionGroup;
			CountDone = pFilePMX->Read(&oftBodyData_NonCollisionGroup, sizeof(int16_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int16_t));
			oftBodyData_NonCollisionGroup = ::PTG_Assert_LEToH(oftBodyData_NonCollisionGroup);

			uint8_t SoftBodyData_Flags;
			CountDone = pFilePMX->Read(&SoftBodyData_Flags, sizeof(uint8_t));
			assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));

			//Flags

			//| Bit index | Name | Set effect | Version	|
			//| :-------- - : | ------------------ - | ---------- - | ---------- - |
			//| 0 | B-Link | ? ? ? | 2.1		|
			//| 1 | Cluster creation | ? ? ? | 2.1		|
			//| 2 | Link crossing | ? ? ? | 2.1 |

			assert(SoftBodyData_Flags == 0U || SoftBodyData_Flags == 1U || SoftBodyData_Flags == 2U);

			int32_t SoftBodyData_BlinkCreateDistance;
			CountDone = pFilePMX->Read(&SoftBodyData_BlinkCreateDistance, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_BlinkCreateDistance = ::PTG_Assert_LEToH(SoftBodyData_BlinkCreateDistance);

			int32_t SoftBodyData_NumberOfClusters;
			CountDone = pFilePMX->Read(&SoftBodyData_NumberOfClusters, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_NumberOfClusters = ::PTG_Assert_LEToH(SoftBodyData_NumberOfClusters);

			float SoftBodyData_TotalMass;
			CountDone = pFilePMX->Read(&SoftBodyData_TotalMass, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_TotalMass = ::PTG_Assert_LEToH(SoftBodyData_TotalMass);

			float SoftBodyData_CollisionMargin;
			CountDone = pFilePMX->Read(&SoftBodyData_CollisionMargin, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_CollisionMargin = ::PTG_Assert_LEToH(SoftBodyData_CollisionMargin);

			int32_t SoftBodyData_AerodynamicsModel;
			CountDone = pFilePMX->Read(&SoftBodyData_AerodynamicsModel, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_AerodynamicsModel = ::PTG_Assert_LEToH(SoftBodyData_AerodynamicsModel);

			//Aerodynamics Models

			//| Index | Type | Notes | Version	|
			//|:---- - : | ---------- - | ------ - | ---------- - |
			//|0 | V_Point | ? ? ? | 2.1		|
			//|1 | V_TwoSided | ? ? ? | 2.1		|
			//|2 | V_OneSided | ? ? ? | 2.1		|
			//|3 | F_TwoSided | ? ? ? | 2.1		|
			//|4 | F_OneSided | ? ? ? | 2.1 |

			assert(SoftBodyData_AerodynamicsModel == 0 || SoftBodyData_AerodynamicsModel == 1 || SoftBodyData_AerodynamicsModel == 2 || SoftBodyData_AerodynamicsModel == 3 || SoftBodyData_AerodynamicsModel == 4);

			float SoftBodyData_ConfigVCF;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigVCF, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigVCF = ::PTG_Assert_LEToH(SoftBodyData_ConfigVCF);

			float SoftBodyData_ConfigDP;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigDP, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigDP = ::PTG_Assert_LEToH(SoftBodyData_ConfigDP);

			float SoftBodyData_ConfigDG;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigDG, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigDG = ::PTG_Assert_LEToH(SoftBodyData_ConfigDG);

			float SoftBodyData_ConfigLF;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigLF, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigLF = ::PTG_Assert_LEToH(SoftBodyData_ConfigLF);

			float SoftBodyData_ConfigPR;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigPR, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigPR = ::PTG_Assert_LEToH(SoftBodyData_ConfigPR);

			float SoftBodyData_ConfigVC;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigVC, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigPR = ::PTG_Assert_LEToH(SoftBodyData_ConfigVC);

			float SoftBodyData_ConfigDF;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigDF, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigPR = ::PTG_Assert_LEToH(SoftBodyData_ConfigDF);

			float SoftBodyData_ConfigMT;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigMT, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigMT = ::PTG_Assert_LEToH(SoftBodyData_ConfigMT);

			float SoftBodyData_ConfigCHR;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigCHR, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigCHR = ::PTG_Assert_LEToH(SoftBodyData_ConfigCHR);

			float SoftBodyData_ConfigKHR;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigKHR, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigKHR = ::PTG_Assert_LEToH(SoftBodyData_ConfigKHR);

			float SoftBodyData_ConfigSHR;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigSHR, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigSHR = ::PTG_Assert_LEToH(SoftBodyData_ConfigSHR);

			float SoftBodyData_ConfigAHR;
			CountDone = pFilePMX->Read(&SoftBodyData_ConfigAHR, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ConfigAHR = ::PTG_Assert_LEToH(SoftBodyData_ConfigAHR);

			float SoftBodyData_ClusterSRHR_CL;
			CountDone = pFilePMX->Read(&SoftBodyData_ClusterSRHR_CL, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ClusterSRHR_CL = ::PTG_Assert_LEToH(SoftBodyData_ClusterSRHR_CL);

			float SoftBodyData_ClusterSKHR_CL;
			CountDone = pFilePMX->Read(&SoftBodyData_ClusterSKHR_CL, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ClusterSKHR_CL = ::PTG_Assert_LEToH(SoftBodyData_ClusterSKHR_CL);

			float SoftBodyData_ClusterSSHR_CL;
			CountDone = pFilePMX->Read(&SoftBodyData_ClusterSSHR_CL, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ClusterSSHR_CL = ::PTG_Assert_LEToH(SoftBodyData_ClusterSSHR_CL);

			float SoftBodyData_ClusterSR_SPLT_CL;
			CountDone = pFilePMX->Read(&SoftBodyData_ClusterSR_SPLT_CL, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ClusterSR_SPLT_CL = ::PTG_Assert_LEToH(SoftBodyData_ClusterSR_SPLT_CL);

			float SoftBodyData_ClusterSK_SPLT_CL;
			CountDone = pFilePMX->Read(&SoftBodyData_ClusterSK_SPLT_CL, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ClusterSK_SPLT_CL = ::PTG_Assert_LEToH(SoftBodyData_ClusterSK_SPLT_CL);

			float SoftBodyData_ClusterSS_SPLT_CL;
			CountDone = pFilePMX->Read(&SoftBodyData_ClusterSS_SPLT_CL, sizeof(float));
			assert(static_cast<size_t>(CountDone) == sizeof(float));
			SoftBodyData_ClusterSS_SPLT_CL = ::PTG_Assert_LEToH(SoftBodyData_ClusterSS_SPLT_CL);

			int32_t SoftBodyData_InterationV_IT;
			CountDone = pFilePMX->Read(&SoftBodyData_InterationV_IT, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_InterationV_IT = ::PTG_Assert_LEToH(SoftBodyData_InterationV_IT);

			int32_t SoftBodyData_InterationP_IT;
			CountDone = pFilePMX->Read(&SoftBodyData_InterationP_IT, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_InterationP_IT = ::PTG_Assert_LEToH(SoftBodyData_InterationP_IT);

			int32_t SoftBodyData_InterationD_IT;
			CountDone = pFilePMX->Read(&SoftBodyData_InterationD_IT, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_InterationD_IT = ::PTG_Assert_LEToH(SoftBodyData_InterationD_IT);

			int32_t SoftBodyData_InterationC_IT;
			CountDone = pFilePMX->Read(&SoftBodyData_InterationC_IT, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_InterationC_IT = ::PTG_Assert_LEToH(SoftBodyData_InterationC_IT);

			int32_t SoftBodyData_MaterialLST;
			CountDone = pFilePMX->Read(&SoftBodyData_MaterialLST, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_MaterialLST = ::PTG_Assert_LEToH(SoftBodyData_MaterialLST);

			int32_t SoftBodyData_MaterialAST;
			CountDone = pFilePMX->Read(&SoftBodyData_MaterialAST, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_MaterialAST = ::PTG_Assert_LEToH(SoftBodyData_MaterialAST);

			int32_t SoftBodyData_AnchorRigidBodyCount;
			CountDone = pFilePMX->Read(&SoftBodyData_AnchorRigidBodyCount, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_AnchorRigidBodyCount = ::PTG_Assert_LEToH(SoftBodyData_AnchorRigidBodyCount);

			for (int32_t IndexAnchorRigidBody = 0; IndexAnchorRigidBody < SoftBodyData_AnchorRigidBodyCount; ++IndexAnchorRigidBody)
			{
				//Anchor Rigid Bodies

				//| Name | Type | Notes				|
				//| ------------------ - | ------ - | ------------------ - |
				//| Rigid body index | index | See Index Types	|
				//| Vertex index | index | See Index Types	|
				//| Near mode | byte | ? ? ? |

				int32_t AnchorRigidBody_RigidBodyIndex = pFunReadRigidbodyIndex(pFilePMX);
				uint32_t AnchorRigidBody_VertexIndex = pFunReadVertexIndex(pFilePMX);

				uint8_t AnchorRigidBody_NearMode;
				CountDone = pFilePMX->Read(&AnchorRigidBody_NearMode, sizeof(uint8_t));
				assert(static_cast<size_t>(CountDone) == sizeof(uint8_t));
			}

			int32_t SoftBodyData_VertexPinCount;
			CountDone = pFilePMX->Read(&SoftBodyData_VertexPinCount, sizeof(int32_t));
			assert(static_cast<size_t>(CountDone) == sizeof(int32_t));
			SoftBodyData_VertexPinCount = ::PTG_Assert_LEToH(SoftBodyData_VertexPinCount);

			for (int32_t IndexAnchoVertexPin = 0; IndexAnchoVertexPin < SoftBodyData_VertexPinCount; ++IndexAnchoVertexPin)
			{
				//Vertex Pins

				//| Name | Type | Notes				|
				//| ------------------ - | ------ - | ------------------ - |
				//| Vertex index | index | See Index Types |

				uint32_t VertexPin_VertexIndex = pFunReadVertexIndex(pFilePMX);

			}
		}
	}
	else
	{
		assert(Header_Version == 2.0f);
		assert(CountDone == 0);
	}

	pFilePMX->Release();

	struct PTGR_Vertex_Intermediate
	{
		PTGR_Vertex m_Vertex;
		int32_t m_Index;
	};

	struct PTGR_Mesh_Intermediate_Split
	{
		std::map<
			int32_t,
			PTGR_Vertex_Intermediate,
			std::less<int32_t>,
			::PTS_CPP_Allocator<std::pair<int32_t, PTGR_Vertex_Intermediate>>
		> m_VertexArray;
		std::vector<int32_t, ::PTS_CPP_Allocator<int32_t>> m_IndexArray;
	};

	std::vector<PTGR_Mesh_Intermediate_Split, ::PTS_CPP_Allocator<PTGR_Mesh_Intermediate_Split>> Mesh_Intermediate_Split_Array;

	for (int32_t IndexSplit = 0; IndexSplit < Mesh_Intermediate_PMX.m_IndexArraySpiltArray.size() - 1; ++IndexSplit)
	{
		int32_t IndexIndexBegin = Mesh_Intermediate_PMX.m_IndexArraySpiltArray[IndexSplit];
		int32_t IndexIndexEnd = Mesh_Intermediate_PMX.m_IndexArraySpiltArray[IndexSplit + 1];
		
		PTGR_Mesh_Intermediate_Split Mesh_Intermediate_ToInsert;

		for (int32_t IndexIndex = IndexIndexBegin; IndexIndex < IndexIndexEnd; ++IndexIndex)
		{
			int32_t IndexVertex = Mesh_Intermediate_PMX.m_IndexArray[IndexIndex];

			auto pVertexPair = Mesh_Intermediate_ToInsert.m_VertexArray.find(IndexVertex);
			if (pVertexPair != Mesh_Intermediate_ToInsert.m_VertexArray.end())
			{
				Mesh_Intermediate_ToInsert.m_IndexArray.push_back(pVertexPair->second.m_Index);
			}
			else
			{
				PTGR_Vertex_Intermediate VertexToInsert;
				VertexToInsert.m_Vertex = Mesh_Intermediate_PMX.m_VertexArray[IndexVertex];
				VertexToInsert.m_Index = Mesh_Intermediate_ToInsert.m_VertexArray.size();

				Mesh_Intermediate_ToInsert.m_VertexArray[IndexVertex] = VertexToInsert;
				Mesh_Intermediate_ToInsert.m_IndexArray.push_back(VertexToInsert.m_Index);
			}
		}

		Mesh_Intermediate_Split_Array.push_back(std::move(Mesh_Intermediate_ToInsert));
	}

	struct PTGR_Mesh
	{
		std::vector<PTGR_Vertex, ::PTS_CPP_Allocator<PTGR_Vertex>> m_VertexArray;
		std::vector<int, ::PTS_CPP_Allocator<int>> m_IndexArray;
	};

	std::vector<PTGR_Mesh, ::PTS_CPP_Allocator<PTGR_Mesh>> MeshArray;

	for (auto &rMesh_Intermediate_Split : Mesh_Intermediate_Split_Array)
	{
		PTGR_Mesh MeshToInsert;
		MeshToInsert.m_VertexArray.resize(rMesh_Intermediate_Split.m_VertexArray.size());
		for (auto &rVertex : rMesh_Intermediate_Split.m_VertexArray)
		{
			assert(static_cast<size_t>(rVertex.second.m_Index) < MeshToInsert.m_VertexArray.size());
			MeshToInsert.m_VertexArray[rVertex.second.m_Index] = rVertex.second.m_Vertex;
		}
		MeshToInsert.m_IndexArray.swap(rMesh_Intermediate_Split.m_IndexArray);
		MeshArray.push_back(std::move(MeshToInsert));
	}

	int huhu = 0;
}
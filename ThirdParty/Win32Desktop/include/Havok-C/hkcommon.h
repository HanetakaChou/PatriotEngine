#ifndef _HAVOK_C_HKCOMMON_H_
#define _HAVOK_C_HKCOMMON_H_ 1

#ifndef HAVOK_C_API
#define HAVOK_C_API __declspec(dllimport)
#endif
#define HAVOK_C_CALL __stdcall
#define HAVOK_C_PTR

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct hkOpaqueMemoryAllocator *hkMemoryAllocatorRef;
	typedef struct hkOpaqueMemoryRouter *hkMemoryRouterRef;
	typedef struct hkOpaqueJobQueue *hkJobQueueRef;
	typedef struct hkOpaqueProcessContext *hkProcessContextRef;
	typedef struct hkOpaqueVisualDebugger *hkVisualDebuggerRef;
	typedef struct hkOpaqueObjectResource *hkObjectResourceRef;
	typedef struct hkOpaqueRootLevelContainer *hkRootLevelContainerRef;
	typedef struct hkxOpaqueScene *hkxSceneRef;
	typedef struct hkxOpaqueNode *hkxNodeRef;
	typedef struct hkxOpaqueMesh *hkxMeshRef;
	typedef struct hkxOpaqueMeshSection *hkxMeshSectionRef;
	typedef struct hkxOpaqueVertexDescriptionRef *hkxVertexDescriptionRef;
	typedef struct hkxOpaqueVertexBuffer *hkxVertexBufferRef;
	typedef struct hkxOpaqueIndexBuffer *hkxIndexBufferRef;
	typedef struct hkxOpaqueSkinBinding *hkxSkinBindingRef;
	typedef struct hkxOpaqueAttributeHolder *hkxAttributeHolderRef;
	typedef struct hkxOpaqueAttribute *hkxAttributeRef;
	typedef struct hkxOpaqueMaterial *hkxMaterialRef;
	typedef struct hkxOpaqueTextureFileRef *hkxTextureFileRef;
	typedef struct hkOpaqueOptionParser *hkOptionParserRef;
	typedef struct hkOpaqueError *hkErrorRef;

	HAVOK_C_API extern int8_t hkResultEnum_HK_SUCCESS;
	HAVOK_C_API extern int8_t hkResultEnum_HK_FAILURE;
	HAVOK_C_API extern int8_t hkJobQueue_JOB_INVALID;
	HAVOK_C_API extern int8_t hkJobQueue_GOT_NEXT_JOB;
	HAVOK_C_API extern int8_t hkJobQueue_NO_JOBS_AVAILABLE;
	HAVOK_C_API extern int8_t hkJobQueue_ALL_JOBS_FINISHED;
	HAVOK_C_API extern int8_t hkJobQueue_JOBS_AVAILABLE_BUT_NOT_FOR_CURRENT_ELF;
	HAVOK_C_API extern int8_t hkJobQueue_SPU_QUIT_ELF;

	HAVOK_C_API extern int8_t hkSeekableStreamReader_STREAM_SET;
	HAVOK_C_API extern int8_t hkSeekableStreamReader_STREAM_CUR;
	HAVOK_C_API extern int8_t hkSeekableStreamReader_STREAM_END;

	HAVOK_C_API extern int8_t hkStreamWriter_STREAM_SET;
	HAVOK_C_API extern int8_t hkStreamWriter_STREAM_CUR;
	HAVOK_C_API extern int8_t hkStreamWriter_STREAM_END;

	HAVOK_C_API extern int8_t hkSerializeUtil_SAVE_TEXT_FORMAT;

	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_NONE;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_POSITION;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_COLOR;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_NORMAL;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_TANGENT;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_BINORMAL;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_TEXCOORD;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_BLENDWEIGHTS;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_BLENDINDICES;
	HAVOK_C_API extern int16_t hkxVertexDescription_HKX_DU_USERDATA;

	HAVOK_C_API extern int8_t hkxVertexDescription_HKX_DT_NONE;
	HAVOK_C_API extern int8_t hkxVertexDescription_HKX_DT_UINT8;
	HAVOK_C_API extern int8_t hkxVertexDescription_HKX_DT_INT16;
	HAVOK_C_API extern int8_t hkxVertexDescription_HKX_DT_UINT32;
	HAVOK_C_API extern int8_t hkxVertexDescription_HKX_DT_FLOAT;

	HAVOK_C_API extern int8_t hkxIndexBuffer_INDEX_TYPE_TRI_LIST;

	HAVOK_C_API extern int8_t hkxMaterial_transp_none;
	HAVOK_C_API extern int8_t hkxMaterial_transp_alpha;
	HAVOK_C_API extern int8_t hkxMaterial_transp_additive;
	HAVOK_C_API extern int8_t hkxMaterial_transp_colorkey;
	HAVOK_C_API extern int8_t hkxMaterial_transp_subtractive;

	HAVOK_C_API extern int8_t hkxMaterial_UVMA_SRT;
	HAVOK_C_API extern int8_t hkxMaterial_UVMA_TRS;
	HAVOK_C_API extern int8_t hkxMaterial_UVMA_3DSMAX_STYLE;
	HAVOK_C_API extern int8_t hkxMaterial_UVMA_MAYA_STYLE;

	HAVOK_C_API extern int32_t hkxMaterial_TEX_UNKNOWN;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_DIFFUSE;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_REFLECTION;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_BUMP;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_NORMAL;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_DISPLACEMENT;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_SPECULAR;			// Specular Level map
	HAVOK_C_API extern int32_t hkxMaterial_TEX_SPECULARANDGLOSS; // Specular Level map with the Gloss (power) in the Alpha channel
	HAVOK_C_API extern int32_t hkxMaterial_TEX_OPACITY;			// Opacity (transparency) map. Normally not used, just use the alpha channel in one of the diffuse maps instead.
	HAVOK_C_API extern int32_t hkxMaterial_TEX_EMISSIVE;			// Emissive (self illumination) map
	HAVOK_C_API extern int32_t hkxMaterial_TEX_REFRACTION;
	HAVOK_C_API extern int32_t hkxMaterial_TEX_GLOSS;	 // Specular Power map, normally not used (alpha in specmap quicker)
	HAVOK_C_API extern int32_t hkxMaterial_TEX_DOMINANTS; // Dominant data for displacement mapping
	HAVOK_C_API extern int32_t hkxMaterial_TEX_NOTEXPORTED;

	HAVOK_C_API extern int32_t hkxMaterial_PROPERTY_MTL_UV_ID_STAGE0;
	HAVOK_C_API extern int32_t hkxMaterial_PROPERTY_MTL_UV_ID_STAGE_MAX;

	HAVOK_C_API void HAVOK_C_CALL hkPlatformInit();

	HAVOK_C_API hkMemoryAllocatorRef HAVOK_C_CALL hkMallocAllocator_defaultMallocAllocator();

	HAVOK_C_API hkMemoryRouterRef HAVOK_C_CALL hkMemoryInitUtil_initDefault(hkMemoryAllocatorRef memoryAllocator, int solverBufferSize);

	HAVOK_C_API hkMemoryRouterRef HAVOK_C_CALL hkMemoryInitUtil_initChecking(hkMemoryAllocatorRef memoryAllocator, int solverBufferSize);

	HAVOK_C_API hkMemoryRouterRef HAVOK_C_CALL hkMemoryInitUtil_initFreeListLargeBlock(hkMemoryAllocatorRef memoryAllocator, int solverBufferSize);

	HAVOK_C_API int8_t HAVOK_C_CALL hkMemoryInitUtil_quit();

	HAVOK_C_API int8_t HAVOK_C_CALL hkBaseSystem_init(hkMemoryRouterRef memoryRouter, void(HAVOK_C_PTR *errorReport)(char const *msg, void *errorReportObject), void *errorReportObject);

	HAVOK_C_API int8_t HAVOK_C_CALL hkBaseSystem_quit();

	HAVOK_C_API void HAVOK_C_CALL hkThreadNumber_set(int threadNumber);

	HAVOK_C_API hkJobQueueRef HAVOK_C_CALL hkJobQueue_new(int numCpuThreads);

	HAVOK_C_API int8_t HAVOK_C_CALL hkJobQueue_processAllJobs(hkJobQueueRef jobQueue);

	HAVOK_C_API void HAVOK_C_CALL hkJobQueue_delete(hkJobQueueRef jobQueue);

	HAVOK_C_API void HAVOK_C_CALL hkProcessContext_syncTimers(hkProcessContextRef context);

	HAVOK_C_API hkVisualDebuggerRef HAVOK_C_CALL hkVisualDebugger_new(hkProcessContextRef *contexts, int contextCount);

	HAVOK_C_API void HAVOK_C_CALL hkVisualDebugger_serve(hkVisualDebuggerRef vdb);

	HAVOK_C_API void HAVOK_C_CALL hkVisualDebugger_step(hkVisualDebuggerRef vdb);

	HAVOK_C_API void HAVOK_C_CALL hkVisualDebugger_removeReference(hkVisualDebuggerRef vdb);

	HAVOK_C_API void HAVOK_C_CALL hkProcessRegisterUtil_registerAllCommonProcesses();

	HAVOK_C_API hkObjectResourceRef HAVOK_C_CALL hkSerializeUtil_loadOnHeap(
		void *pStreamReaderRef,
		int(HAVOK_C_PTR *pFn_StreamReader_read)(void *pStreamReaderRef, void *buf, int nbytes),
		int(HAVOK_C_PTR *pFn_StreamReader_seek)(void *pStreamReaderRef, int offset, int whence));

	HAVOK_C_API hkRootLevelContainerRef HAVOK_C_CALL hkObjectResource_stealContents_hkOpaqueRootLevelContainer(hkObjectResourceRef res);

	HAVOK_C_API void HAVOK_C_CALL hkObjectResource_removeReference(hkObjectResourceRef res);

	HAVOK_C_API int8_t HAVOK_C_CALL hkSerializeUtil_save(
		hkRootLevelContainerRef container,
		void *pStreamWriterRef,
		int(HAVOK_C_PTR *pFn_StreamWriter_write)(void *pStreamWriterRef, void *buf, int nbytes),
		int(HAVOK_C_PTR *pFn_StreamWriter_seek)(void *pStreamWriterRef, int offset, int whence),
		int8_t options);

	HAVOK_C_API hkRootLevelContainerRef HAVOK_C_CALL hkRootLevelContainer_new();

	HAVOK_C_API void HAVOK_C_CALL hkRootLevelContainer_namedVariant_pushObject_hkxSence(hkRootLevelContainerRef container, char const *name, hkxSceneRef scene);

	HAVOK_C_API hkxSceneRef HAVOK_C_CALL hkRootLevelContainer_findObject_hkxScene(hkRootLevelContainerRef container);

	HAVOK_C_API void HAVOK_C_CALL hkRootLevelContainer_delete(hkRootLevelContainerRef container);

	HAVOK_C_API hkxSceneRef HAVOK_C_CALL hkxScene_new();

	HAVOK_C_API void HAVOK_C_CALL hkxScene_modeller_set(hkxSceneRef scene, char const *);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_asset_set(hkxSceneRef scene, char const *);

	HAVOK_C_API hkxNodeRef HAVOK_C_CALL hkxScene_rootNode(hkxSceneRef scene);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_rootNode_set(hkxSceneRef scene, hkxNodeRef);

	HAVOK_C_API uint32_t HAVOK_C_CALL hkxScene_numFrames(hkxSceneRef scene);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_numFrames_set(hkxSceneRef scene, uint32_t);

	HAVOK_C_API float HAVOK_C_CALL hkxScene_sceneLength(hkxSceneRef scene);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_sceneLength_set(hkxSceneRef scene, float);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_meshes_pushBack(hkxSceneRef scene, hkxMeshRef mesh);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_skinBindings_pushBack(hkxSceneRef scene, hkxSkinBindingRef skin);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_materials_pushBack(hkxSceneRef scene, hkxMaterialRef material);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_externalTextures_pushBack(hkxSceneRef scene, hkxTextureFileRef texture);

	HAVOK_C_API void HAVOK_C_CALL hkxScene_removeReference(hkxSceneRef scene);

	HAVOK_C_API hkxNodeRef HAVOK_C_CALL hkxNode_new();

	HAVOK_C_API char const *HAVOK_C_CALL hkxNode_name(hkxNodeRef node);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_name_set(hkxNodeRef node, char const *);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_keyFrames_setSize(hkxNodeRef node, int);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_keyFrames_setValue(hkxNodeRef node, int index, float[4][4]);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_keyFrames_setSizeValue(hkxNodeRef node, int, float[4][4]);

	HAVOK_C_API int HAVOK_C_CALL hkxNode_keyFrames_getSize(hkxNodeRef node);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_keyFrames_getValue(hkxNodeRef node, int index, float[4][4]);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_keyFrames_pushBack(hkxNodeRef node, float[4][4]);

	HAVOK_C_API int HAVOK_C_CALL hkxNode_linearKeyFrameHints_getSize(hkxNodeRef node);

	HAVOK_C_API float *HAVOK_C_CALL hkxNode_linearKeyFrameHints_begin(hkxNodeRef node);

	HAVOK_C_API float *HAVOK_C_CALL hkxNode_linearKeyFrameHints_end(hkxNodeRef node);

	HAVOK_C_API int HAVOK_C_CALL hkxNode_linearKeyFrameHints_indexOf(hkxNodeRef node, float);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_linearKeyFrameHints_pushBack(hkxNodeRef node, float);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_annotations_pushBack(hkxNodeRef node, float, char const *);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_children_pushBack(hkxNodeRef node, hkxNodeRef);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_selected_set(hkxNodeRef node, bool);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_bone_set(hkxNodeRef node, bool);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_selected_userProperties_set(hkxNodeRef node, char const *);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_object_setMesh(hkxNodeRef node, hkxMeshRef mesh);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_object_setSkinBinding(hkxNodeRef node, hkxSkinBindingRef skin);

	HAVOK_C_API void HAVOK_C_CALL hkxNode_removeReference(hkxNodeRef node);

	HAVOK_C_API hkxMeshRef HAVOK_C_CALL hkxMesh_new();

	HAVOK_C_API void HAVOK_C_CALL hkxMesh_sections_setSize(hkxMeshRef mesh, int size);

	HAVOK_C_API void HAVOK_C_CALL hkxMesh_sections_setValue(hkxMeshRef mesh, int index, hkxMeshSectionRef meshsection);

	HAVOK_C_API void HAVOK_C_CALL hkxMesh_removeReference(hkxMeshRef mesh);

	HAVOK_C_API void HAVOK_C_CALL hkxMeshSectionUtil_computeTangents(hkxMeshRef mesh, bool splitVertices, char const *nameHint);

	HAVOK_C_API hkxMeshSectionRef HAVOK_C_CALL hkxMeshSection_new();

	HAVOK_C_API void HAVOK_C_CALL hkxMeshSection_material_set(hkxMeshSectionRef meshsection, hkxMaterialRef material);

	HAVOK_C_API void HAVOK_C_CALL hkxMeshSection_vertexBuffer_set(hkxMeshSectionRef meshsection, hkxVertexBufferRef vb);

	HAVOK_C_API void HAVOK_C_CALL hkxMeshSection_indexBuffers_setSize(hkxMeshSectionRef meshsection, int size);

	HAVOK_C_API void HAVOK_C_CALL hkxMeshSection_indexBuffers_setValue(hkxMeshSectionRef meshsection, int index, hkxIndexBufferRef ib);

	HAVOK_C_API void HAVOK_C_CALL hkxMeshSection_removeReference(hkxMeshSectionRef meshsection);

	HAVOK_C_API hkxVertexDescriptionRef HAVOK_C_CALL hkxVertexDescription_new();

	HAVOK_C_API void HAVOK_C_CALL hkxVertexDescription_decls_pushBack(hkxVertexDescriptionRef format, int16_t dataUsage, int8_t dataType, int8_t numElements, char const *channelID);

	HAVOK_C_API void HAVOK_C_CALL hkxVertexDescription_delete(hkxVertexDescriptionRef format);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinUtils_quantizeWeights(float const inputWeights[4], uint8_t quantizedWeights[4]);

	HAVOK_C_API hkxVertexBufferRef HAVOK_C_CALL hkxVertexBuffer_new();

	HAVOK_C_API void HAVOK_C_CALL hkxVertexBuffer_setNumVertices(hkxVertexBufferRef vb, int n, hkxVertexDescriptionRef format);

	HAVOK_C_API uint32_t HAVOK_C_CALL hkxVertexBuffer_getElementDecl_byteStride(hkxVertexBufferRef vb, int16_t dataUsage, int index);

	HAVOK_C_API void *HAVOK_C_CALL hkxVertexBuffer_getElementDecl_VertexDataPtr(hkxVertexBufferRef vb, int16_t dataUsage, int index);

	HAVOK_C_API void HAVOK_C_CALL hkxVertexBuffer_removeReference(hkxVertexBufferRef vb);

	HAVOK_C_API void HAVOK_C_CALL hkxSceneUtils_flipWinding(hkxIndexBufferRef ib);

	HAVOK_C_API hkxIndexBufferRef HAVOK_C_CALL hkxIndexBuffer_new();

	HAVOK_C_API void HAVOK_C_CALL hkxIndexBuffer_indexType_set(hkxIndexBufferRef ib, int8_t indexType);

	HAVOK_C_API void HAVOK_C_CALL hkxIndexBuffer_indices32_setSize(hkxIndexBufferRef ib, int size);

	HAVOK_C_API uint32_t * HAVOK_C_CALL hkxIndexBuffer_indices32_begin(hkxIndexBufferRef ib);

	HAVOK_C_API void HAVOK_C_CALL hkxIndexBuffer_vertexBaseOffset_set(hkxIndexBufferRef ib, uint32_t vertexBaseOffset);

	HAVOK_C_API void HAVOK_C_CALL hkxIndexBuffer_length_set(hkxIndexBufferRef ib, uint32_t length);

	HAVOK_C_API void HAVOK_C_CALL hkxIndexBuffer_removeReference(hkxIndexBufferRef ib);

	HAVOK_C_API hkxSkinBindingRef HAVOK_C_CALL hkxSkinBinding_new();

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_mesh_set(hkxSkinBindingRef skin, hkxMeshRef mesh);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_initSkinTransform_set(hkxSkinBindingRef skin, float m[4][4]);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_bindPose_setSize(hkxSkinBindingRef skin, int size);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_bindPose_setValue(hkxSkinBindingRef skin, int index, float m[4][4]);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_nodeNames_setSize(hkxSkinBindingRef skin, int size);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_nodeNames_setValue(hkxSkinBindingRef skin, int index, char const *nodeName);

	HAVOK_C_API void HAVOK_C_CALL hkxSkinBinding_removeReference(hkxSkinBindingRef skin);

	HAVOK_C_API hkxMaterialRef HAVOK_C_CALL hkxMaterial_new();

	HAVOK_C_API char const *HAVOK_C_CALL hkxMaterial_name(hkxMaterialRef material);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_name_set(hkxMaterialRef material, char const *name);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_diffuseColor_set(hkxMaterialRef material, float diffuseColor[4]);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_ambientColor_set(hkxMaterialRef material, float ambientColor[4]);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_specularColor_set(hkxMaterialRef material, float specularColor[4]);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_emissiveColor_set(hkxMaterialRef material, float emissiveColor[4]);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_specularMultiplier_set(hkxMaterialRef material, float specularMultiplier);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_specularExponent_set(hkxMaterialRef material, float specularExponent);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_transparency_set(hkxMaterialRef material, int8_t transparency);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_uvMapScale_set(hkxMaterialRef material, float uvMapScale[2]);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_uvMapOffset_set(hkxMaterialRef material, float uvMapOffset[2]);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_uvMapRotation_set(hkxMaterialRef material, float uvMapRotation);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_uvMapAlgorithm_set(hkxMaterialRef material, int8_t uvMapAlgorithm);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_stages_pushBack(hkxMaterialRef material, hkxTextureFileRef texture, int32_t usageHint, int32_t tcoordChannel);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_addReference(hkxMaterialRef material);

	HAVOK_C_API void HAVOK_C_CALL hkxMaterial_removeReference(hkxMaterialRef material);

	HAVOK_C_API hkxTextureFileRef HAVOK_C_CALL hkxTextureFile_new();

	HAVOK_C_API void HAVOK_C_CALL hkxTextureFile_name_set(hkxTextureFileRef texture, char const *name);

	HAVOK_C_API void HAVOK_C_CALL hkxTextureFile_originalFilename_set(hkxTextureFileRef texture, char const *originalFilename);

	HAVOK_C_API void HAVOK_C_CALL hkxTextureFile_addReference(hkxTextureFileRef texture);

	HAVOK_C_API void HAVOK_C_CALL hkxTextureFile_removeReference(hkxTextureFileRef texture);

	HAVOK_C_API hkOptionParserRef HAVOK_C_CALL hkOptionParser_new(char const *title, char const *desc);

	HAVOK_C_API extern int8_t hkOptionParser_OPTION_BOOL;
	HAVOK_C_API extern int8_t hkOptionParser_OPTION_COUNTER;
	HAVOK_C_API extern int8_t hkOptionParser_OPTION_INT;
	HAVOK_C_API extern int8_t hkOptionParser_OPTION_STRING;
	struct hkOptionParser_Option
	{
		char const *shortFlag;
		char const *longFlag;
		char const *help;
		int8_t type;
		union {
			bool *b;
			unsigned int *u;
			int *i;
			const char **s;
		} value;
		union {
			bool b;
			unsigned int u;
			int i;
			const char *s;
		} defaultValue;
	};
	HAVOK_C_API bool HAVOK_C_CALL hkOptionParser_setOptions(hkOptionParserRef parser, hkOptionParser_Option *options, int count);

	HAVOK_C_API extern int8_t hkOptionParser_ARGUMENTS_ONE;
	HAVOK_C_API extern int8_t hkOptionParser_ARGUMENTS_ONE_OR_MORE;
	HAVOK_C_API extern int8_t hkOptionParser_ARGUMENTS_ZERO_OR_MORE;
	HAVOK_C_API void HAVOK_C_CALL hkOptionParser_setArguments(hkOptionParserRef parser, char const *name, char const *help, int8_t argumentsType, char const **arguments, int count);

	HAVOK_C_API extern int8_t hkOptionParser_PARSE_SUCCESS;
	HAVOK_C_API extern int8_t hkOptionParser_PARSE_FAILURE;
	HAVOK_C_API extern int8_t hkOptionParser_PARSE_HELP;
	HAVOK_C_API int8_t HAVOK_C_CALL hkOptionParser_parse(hkOptionParserRef parser, int argc, char *argv[]);

	HAVOK_C_API void HAVOK_C_CALL hkOptionParser_delete(hkOptionParserRef parser);

	HAVOK_C_API hkErrorRef HAVOK_C_CALL hkError_getInstance();

	HAVOK_C_API int HAVOK_C_CALL hkError_messageAssert(hkErrorRef error, int id, char const *description, char const *file, int line);

	HAVOK_C_API int HAVOK_C_CALL hkError_messageWarning(hkErrorRef error, int id, char const *description, char const *file, int line);

	HAVOK_C_API int HAVOK_C_CALL hkError_messageError(hkErrorRef error, int id, char const *description, char const *file, int line);

#ifdef __cplusplus
}
#endif

#endif
#ifndef _HAVOK_C_HKBEHAVIOR_H_
#define _HAVOK_C_HKBEHAVIOR_H_ 1

#include "hkcommon.h"
#include "hkanimation.h"

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct hkbOpaqueWorld *hkbWorldRef;
	typedef struct hkbOpaqueAssetLoader *hkbAssetLoaderRef;
	typedef struct hkbOpaqueProjectAssetManager *hkbProjectAssetManagerRef;
	typedef struct hkbOpaqueCharacter *hkbCharacterRef;

	HAVOK_C_API void HAVOK_C_CALL hkbBehaviorJobQueueUtils_registerWithJobQueue(hkJobQueueRef jobQueue);

	HAVOK_C_API hkbWorldRef HAVOK_C_CALL hkbWorld_new(
		float up[3],
		bool enableMultithreadedUpdate
	);

	HAVOK_C_API void HAVOK_C_CALL hkbWorld_addCharacter(hkbWorldRef behaviorWorld, hkbCharacterRef character);

	HAVOK_C_API void HAVOK_C_CALL hkbWorld_beginStepMultithreaded(hkbWorldRef behaviorWorld, float deltaTime);

	HAVOK_C_API bool HAVOK_C_CALL hkbWorld_getMoreJobsMultithreaded(hkbWorldRef behaviorWorld, hkJobQueueRef jobQueue);

	HAVOK_C_API void HAVOK_C_CALL hkbWorld_removeReference(hkbWorldRef behaviorWorld);

	HAVOK_C_API hkbAssetLoaderRef HAVOK_C_CALL hkbAssetLoader_new(
		void *(HAVOK_C_PTR *pFn_StreamReader_Create)(char const *pFileName),
		int(HAVOK_C_PTR *pFn_StreamReader_read)(void *pStreamReaderRef, void *buf, int nbytes),
		int(HAVOK_C_PTR *pFn_StreamReader_seek)(void *pStreamReaderRef, int offset, int whence),
		void (HAVOK_C_PTR *pFn_StreamReader_Dispose)(void * pStreamReaderRef));

	HAVOK_C_API void HAVOK_C_CALL hkbAssetLoader_removeReference(hkbAssetLoaderRef assetLoader);

	HAVOK_C_API hkbProjectAssetManagerRef HAVOK_C_CALL hkbProjectAssetManager_new(hkbAssetLoaderRef assetLoader);

	HAVOK_C_API bool HAVOK_C_CALL hkbProjectAssetManager_loadProject(hkbProjectAssetManagerRef assetManager, char const *filename);

	HAVOK_C_API void HAVOK_C_CALL hkbProjectAssetManager_linkAll(hkbProjectAssetManagerRef assetManager, hkbWorldRef world);

	HAVOK_C_API void HAVOK_C_CALL hkbProjectAssetManager_removeReference(hkbProjectAssetManagerRef assetManager);

	HAVOK_C_API hkbCharacterRef HAVOK_C_CALL hkbProjectAssetManager_createCharacter(hkbProjectAssetManagerRef assetManager, char const* projectName, char const* characterName, char const* behaviorName);

	HAVOK_C_API void HAVOK_C_CALL hkbCharacter_getWorldFromModel(
		hkbCharacterRef character,
		float position[3],
		float rotation[4],
		float scale[3]
	);

	HAVOK_C_API int HAVOK_C_CALL hkbCharacter_getNumBones(hkbCharacterRef character);

	HAVOK_C_API void HAVOK_C_CALL hkbCharacter_getSyncedPoseModelSpace(
		hkbCharacterRef character,
		float (*position)[3],
		float (*rotation)[4],
		float(*scale)[3],
		int numBones
	);

	HAVOK_C_API void HAVOK_C_CALL hkbCharacter_removeReference(hkbCharacterRef character);

	HAVOK_C_API void HAVOK_C_CALL hkbBehaviorContext_registerAllBehaviorViewers();

	HAVOK_C_API void HAVOK_C_CALL hkbBehaviorContext_addDefaultViewers(hkVisualDebuggerRef vdb);

	HAVOK_C_API hkProcessContextRef HAVOK_C_CALL hkbBehaviorContext_new(hkbAssetLoaderRef assetLoader);

	HAVOK_C_API void HAVOK_C_CALL hkbBehaviorContext_addWorld(hkProcessContextRef context, hkbWorldRef behaviorWorld);

	HAVOK_C_API void HAVOK_C_CALL hkbBehaviorContext_removeReference(hkProcessContextRef context);

#ifdef __cplusplus
}
#endif

#endif
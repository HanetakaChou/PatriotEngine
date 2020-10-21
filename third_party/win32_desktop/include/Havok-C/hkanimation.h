#ifndef _HAVOK_C_HKANIMATION_H_
#define _HAVOK_C_HKANIMATION_H_ 1

#include "hkcommon.h"

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct hkaOpaqueSkeleton *hkaSkeletonRef;
	typedef struct hkaOpaqueAnimationContainer *hkaAnimationContainerRef;

	HAVOK_C_API hkaAnimationContainerRef HAVOK_C_CALL hkRootLevelContainer_findObject_hkaAnimationContainer(hkRootLevelContainerRef container);

	HAVOK_C_API int HAVOK_C_CALL hkaAnimationContainer_skeletons_getSize(hkaAnimationContainerRef ac);

	HAVOK_C_API hkaSkeletonRef HAVOK_C_CALL hkaAnimationContainer_skeletons_index(hkaAnimationContainerRef ac, int i);

#ifdef __cplusplus
}
#endif

#endif
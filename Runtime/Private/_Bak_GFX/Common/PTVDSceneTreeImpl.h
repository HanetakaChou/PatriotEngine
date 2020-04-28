#ifndef PT_VIDEO_COMMON_SCENETREEIMPL_H
#define PT_VIDEO_COMMON_SCENETREEIMPL_H

#include "../../../Public/GFX/PTVDSceneTree.h"
#include "../../../Public/McRT/PTSMemoryAllocator.h"
#include "../../../Public/Math/PTMath.h"

class PTVDSceneTreeNodeTransformTR final :public IPTVDSceneTreeNodeTransformTR
{
	PTS_CPP_Vector<IPTVDSceneTreeNode *> m_NodeChild;
	PTDualQuaternionF m_Local_TR;
	PTDualQuaternionF m_Global_TR;
	bool m_Dirty_TR;
};

class PTVDSceneTreeNodeTransformTRS final :public IPTVDSceneTreeNodeTransformS
{
	PTS_CPP_Vector<IPTVDSceneTreeNode *> m_NodeChild;
	PTVector3F m_Local_S;
	PTVector3F m_Global_S;
	bool m_Dirty_S;
};

#endif
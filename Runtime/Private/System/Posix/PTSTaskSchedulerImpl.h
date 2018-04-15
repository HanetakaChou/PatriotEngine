
#include "../../../Public/System/PTSTaskScheduler.h"

class PTSTaskSchedulerImpl :public IPTSTaskScheduler
{
	inline ~PTSTaskSchedulerImpl();
	void Destruct() override;

	void SpawnRootAndWait(PTSTask *pFirst, PTSTask *pNext) override;
};

static_assert(alignof(PTSTaskSchedulerStorage) == alignof(PTSTaskSchedulerImpl), "alignof(PTSTaskSchedulerStorage) != alignof(PTSTaskSchedulerImpl)");
static_assert(sizeof(PTSTaskSchedulerStorage) == sizeof(PTSTaskSchedulerImpl), "sizeof(PTSTaskSchedulerStorage) != sizeof(PTSTaskSchedulerImpl)");
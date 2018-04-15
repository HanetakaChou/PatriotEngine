
#include "../../../Public/System/PTSTaskScheduler.h"

class PTSTaskSchedulerImpl :public IPTSTaskScheduler
{
	inline ~PTSTaskSchedulerImpl();
	void Destruct() override;

	//PTSTaskStorage
	//--------
	//PTSTaskImpl //IPTSTask *pThis
	//--------
	//Pad //ui_StorageCustom_Align
	//--------
	//StorageCustom //void *pVoid_StorageCustom
	//->ui_StorageCustom_Size
	//--------

	void Task_Allocate( //IPTTask *pParent, //SetParent 
		size_t ui_StorageCustom_Size,
		size_t ui_StorageCustom_Align,
		void *pVoid_StorageCustom_ConstructorArgument,
		void(*pFn_StorageCustom_Constructor)(
			void *pVoid_StorageCustom,
			void *pVoid_StorageCustom_ConstructorArgument
			),
		void(*pFn_Execute)(
			void *pVoid_StorageCustom,
			IPTSTask *pThis,
			IPTSTask **const ppNextToExecute //Scheduler Bypass
			)
	) override;

	void SpawnRootAndWait(IPTSTask *pFirst, IPTSTask *pNext) override;
};

static_assert(alignof(PTSTaskSchedulerStorage) == alignof(PTSTaskSchedulerImpl), "alignof(PTSTaskSchedulerStorage) != alignof(PTSTaskSchedulerImpl)");
static_assert(sizeof(PTSTaskSchedulerStorage) == sizeof(PTSTaskSchedulerImpl), "sizeof(PTSTaskSchedulerStorage) != sizeof(PTSTaskSchedulerImpl)");
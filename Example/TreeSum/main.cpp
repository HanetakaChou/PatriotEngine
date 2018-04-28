
#include "../../Runtime/Public/System/PTSMemoryAllocator.h"
#include "../../Runtime/Public/System/PTSTaskScheduler.h"
#include "../../Runtime/Public/System/PTSThread.h"

#include <new>
#include <assert.h>

#include <iostream>

static float const s_PI = 3.14159265358979f;

struct TreeNode 
{
	TreeNode* left;
	TreeNode* right;
	int32_t node_count;
	float value;
};

class Task_TreeCreation_Continuation : public IPTSTask
{
	TreeNode **m_ppRootOfSubTree;
	int32_t m_NodeCount;

	TreeNode *m_pRightOfSubTree;
	TreeNode *m_pLeftOfSubTree;
	friend class Task_TreeCreation;

public:
	inline Task_TreeCreation_Continuation(TreeNode **ppRootOfSubTree, int32_t NodeCount) :m_ppRootOfSubTree(ppRootOfSubTree), m_NodeCount(NodeCount)
	{

	}

	static IPTSTask * Execute(IPTSTask *pVoid);
};

class Task_TreeCreation : public IPTSTask
{
	TreeNode **m_ppRootOfSubTree;
	int32_t m_NodeCount;
public:
	inline Task_TreeCreation(TreeNode **ppRootOfSubTree, int32_t NodeCount) :m_ppRootOfSubTree(ppRootOfSubTree), m_NodeCount(NodeCount)
	{

	}

	static IPTSTask * Execute(IPTSTask *pVoid);
};

TreeNode *TreeCreation_Serial(int32_t NodeCount);

int main()
{
	::PTSMemoryAllocator_Initialize();
	::PTSTaskScheduler_Initialize();
	::PTSTaskScheduler_Local_Initialize();

	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_Local();

	TreeNode *pRoot;

	Task_TreeCreation *pTask = NULL;

	int64_t Frequency = PTSTick_Frequency();
	int64_t t1;
	int64_t t2;

	pTaskScheduler->Worker_Wake();

	t1 = ::PTSTick_Count();

	pTask = new(
		IPTSTaskPrefix::Allocate_Root<Task_TreeCreation>(pTask, sizeof(Task_TreeCreation), alignof(Task_TreeCreation), pTaskScheduler)
		)Task_TreeCreation(&pRoot, 10000000);

	pTaskScheduler->Task_Spawn_Root_And_Wait(pTask);

	t2 = ::PTSTick_Count();

	std::cout << (static_cast<float>(t2 - t1) / static_cast<float>(Frequency))*1000.0f << std::endl;

	t1 = ::PTSTick_Count();

	pTask = new(
		IPTSTaskPrefix::Allocate_Root<Task_TreeCreation>(pTask, sizeof(Task_TreeCreation), alignof(Task_TreeCreation), pTaskScheduler)
		)Task_TreeCreation(&pRoot, 10000000);

	pTaskScheduler->Task_Spawn_Root_And_Wait(pTask);

	t2 = ::PTSTick_Count();

	std::cout << (static_cast<float>(t2 - t1) / static_cast<float>(Frequency))*1000.0f << std::endl;

	pTaskScheduler->Worker_Sleep();

    return 0;
}

IPTSTask * Task_TreeCreation_Continuation::Execute(IPTSTask *pVoid)
{
	Task_TreeCreation_Continuation *pThis = static_cast<Task_TreeCreation_Continuation *>(pVoid);
	(*pThis->m_ppRootOfSubTree) = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(TreeNode), alignof(TreeNode)))TreeNode{};
	(*pThis->m_ppRootOfSubTree)->right = pThis->m_pRightOfSubTree;
	(*pThis->m_ppRootOfSubTree)->left = pThis->m_pLeftOfSubTree;
	(*pThis->m_ppRootOfSubTree)->node_count = pThis->m_NodeCount;
	(*pThis->m_ppRootOfSubTree)->value = s_PI * pThis->m_NodeCount;
	return NULL;
}

IPTSTask * Task_TreeCreation::Execute(IPTSTask *pVoid)
{
	Task_TreeCreation *pThis = static_cast<Task_TreeCreation *>(pVoid);
	IPTSTaskPrefix *pThisPrefix = ::PTSTaskScheduler_Task_Prefix(pThis);

	assert(pThis->m_ppRootOfSubTree != NULL);

	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_Local();

	if (pThis->m_NodeCount >= 1000) //Recursive
	{
		//Divide
		Task_TreeCreation_Continuation *pTaskContinuation = NULL;
		pTaskContinuation = new(
			pThisPrefix->Allocate_Continuation(pTaskContinuation, sizeof(Task_TreeCreation_Continuation), alignof(Task_TreeCreation_Continuation), pTaskScheduler)
			) Task_TreeCreation_Continuation(pThis->m_ppRootOfSubTree, pThis->m_NodeCount);

		IPTSTaskPrefix *pTaskContinuationPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskContinuation);

		--pThis->m_NodeCount;

		Task_TreeCreation *pTaskChildRight = NULL;
		pTaskChildRight = new(
			pTaskContinuationPrefix->Allocate_Child(pTaskChildRight, sizeof(Task_TreeCreation_Continuation), alignof(Task_TreeCreation_Continuation), pTaskScheduler)
			)Task_TreeCreation(&pTaskContinuation->m_pRightOfSubTree, pThis->m_NodeCount / 2);

		pThisPrefix->Recycle_AsChildOf(pTaskContinuationPrefix);
		pThis->m_ppRootOfSubTree = &pTaskContinuation->m_pLeftOfSubTree;
		pThis->m_NodeCount = pThis->m_NodeCount - pThis->m_NodeCount / 2;

		pTaskContinuationPrefix->RefCount_Set(2);

		pTaskScheduler->Task_Spawn(pTaskChildRight);

		return pThis;
	}
	else //Base Case
	{
		(*pThis->m_ppRootOfSubTree) = TreeCreation_Serial(pThis->m_NodeCount);
		return NULL;
	}
}

TreeNode *TreeCreation_Serial(int32_t NodeCount)
{
	if (NodeCount <= 0)
	{
		return NULL;
	}
	else
	{
		TreeNode* n = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(TreeNode), alignof(TreeNode)))TreeNode{};
		n->node_count = NodeCount;
		n->value = s_PI * NodeCount;
		--NodeCount;
		n->left = TreeCreation_Serial(NodeCount / 2);
		n->right = TreeCreation_Serial(NodeCount - NodeCount / 2);
		return n;
	}
}

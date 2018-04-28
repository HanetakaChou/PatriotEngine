
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
	TreeNode **const m_ppRootOfSubTree;
	int32_t m_NodeCount;

	TreeNode *m_pRightOfSubTree;
	TreeNode *m_pLeftOfSubTree;
	friend class Task_TreeCreation;

public:
	inline Task_TreeCreation_Continuation(TreeNode **ppRootOfSubTree, int32_t NodeCount)
		:
		m_ppRootOfSubTree(ppRootOfSubTree),
		m_NodeCount(NodeCount)
	{

	}

	static IPTSTask * Execute(IPTSTask *pVoid);
};

class Task_TreeCreation : public IPTSTask
{
	TreeNode **m_ppRootOfSubTree;
	int32_t m_NodeCount;
public:
	inline Task_TreeCreation(TreeNode **ppRootOfSubTree, int32_t NodeCount) :
		m_ppRootOfSubTree(ppRootOfSubTree),
		m_NodeCount(NodeCount)
	{

	}

	static IPTSTask * Execute(IPTSTask *pVoid);
};

TreeNode *TreeCreation_Serial(int32_t NodeCount);

class Task_TreeSum_Continuation : public IPTSTask
{
	float * const m_pSumofSubTree;
	float m_ValueOfMiddle;

	float m_SumOfLeft;
	float m_SumOfRight;
	friend class Task_TreeSum;

public:
	inline Task_TreeSum_Continuation(float *pSumofSubTree, float ValueOfMiddle)
		:
		m_pSumofSubTree(pSumofSubTree),
		m_ValueOfMiddle(ValueOfMiddle)
	{

	}

	static IPTSTask * Execute(IPTSTask *pVoid);
};

class Task_TreeSum : public IPTSTask
{
	float * m_pSumofSubTree;
	TreeNode *m_pRootOfSubTree;

public:
	inline Task_TreeSum(float *pSumofSubTree, TreeNode *pRootOfSubTree)
		:
		m_pSumofSubTree(pSumofSubTree),
		m_pRootOfSubTree(pRootOfSubTree)
	{

	}

	static IPTSTask * Execute(IPTSTask *pVoid);
};

float TreeSum_Serial(TreeNode* root);

int main()
{
	::PTSMemoryAllocator_Initialize();
	::PTSTaskScheduler_Initialize();
	::PTSTaskScheduler_Initialize_ForThread();

	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();

	TreeNode *pRoot;
	float Sum;

	Task_TreeCreation *pTaskCreation = NULL;
	Task_TreeSum *pTaskSum = NULL;

	int64_t TickPerSecond = PTSTick_Frequency();
	int64_t TickStart;
	int64_t TickEnd;

	TickStart = ::PTSTick_Count();

	pTaskScheduler->Worker_Wake();

	pTaskCreation = new(
		IPTSTaskPrefix::Allocate_Root(pTaskCreation, pTaskScheduler)
		)Task_TreeCreation(&pRoot, 100000000);

	pTaskScheduler->Task_Spawn_Root_And_Wait(pTaskCreation);

	pTaskScheduler->Worker_Sleep();

	TickEnd = ::PTSTick_Count();

	std::cout << (static_cast<float>(TickEnd - TickStart) / static_cast<float>(TickPerSecond))*1000.0f << std::endl;

	for (int i = 0; i < 5000000; ++i)
	{
		::PTS_Yield();
	}

	TickStart = ::PTSTick_Count();

	pTaskScheduler->Worker_Wake();

	pTaskSum = new(
		IPTSTaskPrefix::Allocate_Root(pTaskSum, pTaskScheduler)
		)Task_TreeSum(&Sum, pRoot);

	pTaskScheduler->Task_Spawn_Root_And_Wait(pTaskSum);

	pTaskScheduler->Worker_Sleep();

	TickEnd = ::PTSTick_Count();

	std::cout << (static_cast<float>(TickEnd - TickStart) / static_cast<float>(TickPerSecond))*1000.0f << std::endl;

	std::cout << Sum << std::endl;

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

	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();

	if (pThis->m_NodeCount >= 1000) //Recursive
	{
		//Divide
		Task_TreeCreation_Continuation *pTaskContinuation = NULL;
		pTaskContinuation = new(
			pThisPrefix->Allocate_Continuation(pTaskContinuation, pTaskScheduler)
			) Task_TreeCreation_Continuation(pThis->m_ppRootOfSubTree, pThis->m_NodeCount);

		IPTSTaskPrefix *pTaskContinuationPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskContinuation);

		--pThis->m_NodeCount;

		Task_TreeCreation *pTaskChildRight = NULL;
		pTaskChildRight = new(
			pTaskContinuationPrefix->Allocate_Child(pTaskChildRight, pTaskScheduler)
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


IPTSTask * Task_TreeSum_Continuation::Execute(IPTSTask *pVoid)
{
	Task_TreeSum_Continuation *pThis = static_cast<Task_TreeSum_Continuation *>(pVoid);
	(*pThis->m_pSumofSubTree) = pThis->m_ValueOfMiddle + pThis->m_SumOfLeft + pThis->m_SumOfRight;
	return NULL;
}

IPTSTask * Task_TreeSum::Execute(IPTSTask *pVoid)
{
	Task_TreeSum *pThis = static_cast<Task_TreeSum *>(pVoid);
	IPTSTaskPrefix *pThisPrefix = ::PTSTaskScheduler_Task_Prefix(pThis);

	assert(pThis->m_pRootOfSubTree != NULL);

	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();

	if (pThis->m_pRootOfSubTree->node_count >= 1000) //Recursive
	{
		//Divide
		Task_TreeSum_Continuation *pTaskContinuation = NULL;
		pTaskContinuation = new(
			pThisPrefix->Allocate_Continuation(pTaskContinuation, pTaskScheduler)
			) Task_TreeSum_Continuation(pThis->m_pSumofSubTree, pThis->m_pRootOfSubTree->value);
		
		IPTSTaskPrefix *pTaskContinuationPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskContinuation);

		//Maybe Modified By Recycle
		TreeNode *pRootOfSubTree = pThis->m_pRootOfSubTree;

		Task_TreeSum *pTaskChildNext = NULL;
		Task_TreeSum *pTaskChildSpawn = NULL;

		Task_TreeSum *pTaskChildLeft = NULL;
		if (pRootOfSubTree->left != NULL)
		{
			pThisPrefix->Recycle_AsChildOf(pTaskContinuationPrefix);
			pThis->m_pSumofSubTree = &pTaskContinuation->m_SumOfLeft;
			pThis->m_pRootOfSubTree = pRootOfSubTree->left;
			pTaskChildLeft = pThis;
			pTaskChildNext = pTaskChildLeft;
		}
		else
		{
			pTaskContinuation->m_SumOfLeft = 0.0f;
		}


		Task_TreeSum *pTaskChildRight = NULL;
		if (pRootOfSubTree->right != NULL)
		{
			if (pTaskChildNext != NULL)
			{
				pTaskChildRight = new(
					pTaskContinuationPrefix->Allocate_Child(pTaskChildRight, pTaskScheduler)
					)Task_TreeSum(&pTaskContinuation->m_SumOfRight, pRootOfSubTree->right);
				pTaskChildSpawn = pTaskChildRight;
			}
			else //!!!Still Can Recycle
			{
				pThisPrefix->Recycle_AsChildOf(pTaskContinuationPrefix);
				pThis->m_pSumofSubTree = &pTaskContinuation->m_SumOfRight;
				pThis->m_pRootOfSubTree = pRootOfSubTree->right;
				pTaskChildRight = pThis;
				pTaskChildNext = pTaskChildRight;
			}
		}
		else
		{
			pTaskContinuation->m_SumOfRight = 0.0f;
		}

		//Conquer
		pTaskContinuationPrefix->RefCount_Set(((pTaskChildRight != NULL) ? 1 : 0) + ((pTaskChildLeft != NULL) ? 1 : 0));
		if (pTaskChildSpawn != NULL)
		{
			pTaskScheduler->Task_Spawn(pTaskChildSpawn);
		}
		return pTaskChildNext;
	}
	else //Base Case
	{
		(*pThis->m_pSumofSubTree) = TreeSum_Serial(pThis->m_pRootOfSubTree);
		return NULL;
	}
}

float TreeSum_Serial(TreeNode* root)
{
	float result = root->value;
	if (root->left)
		result += TreeSum_Serial(root->left);
	if (root->right)
		result += TreeSum_Serial(root->right);
	return result;
}
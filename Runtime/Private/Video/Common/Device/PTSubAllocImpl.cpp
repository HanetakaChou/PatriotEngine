#include "PTSubAlloc.h"

#include <assert.h>
#include <new>

class PTTreeNodeInternal;

class PTTreeNode
{	
protected:
	PTTreeNodeInternal *m_Parent;
	uint32_t m_IsLeaf;

	inline PTTreeNode(PTTreeNodeInternal *Parent, uint32_t IsLeaf) :m_Parent(Parent), m_IsLeaf(IsLeaf)
	{
		assert((reinterpret_cast<uintptr_t>(this) % alignof(PTTreeNode)) == 0U);
	}

public:
	inline bool TreeIsLeaf()
	{
		return m_IsLeaf != 0U;
	}

	PTTreeNodeInternal *TreeGetParent()
	{
		return m_Parent;
	}
};

class PTTreeNodeInternal :public PTTreeNode
{
	PTTreeNode *m_ChildLeft;
	PTTreeNode *m_ChildRight;

	//��PTTreeNodeLeaf::TreeSeparate��PTTreeNodeLeaf::TreeCombine���죬���ᱻ����
	PTTreeNodeInternal(PTTreeNodeInternal *Parent, PTTreeNode *ChildLeft, PTTreeNode *ChildRight) :PTTreeNode(Parent, 0U), m_ChildLeft(ChildLeft), m_ChildRight(ChildRight)
	{

	}

	friend class PTTreeNodeLeaf;

public:
	PTTreeNode *TreeGetChildLeft()
	{
		return m_ChildLeft;
	}

	PTTreeNode *TreeGetChildRight()
	{
		return m_ChildRight;
	}
};

class PTTreeNodeLeaf;
class PTList;

class PTListNode
{
	PTTreeNodeLeaf *m_pNext;

	inline PTListNode()
	{
		assert((reinterpret_cast<uintptr_t>(this) % alignof(PTListNode)) == 0U);
	}

	friend class PTTreeNodeLeaf;
	friend class PTList;
};

class PTTreeNodeLeaf :public PTTreeNode
{
	//����
	PTListNode *m_pPrev;//Ϊ�����ڴ�����ͷŲ��������Ŀ�����æµ����Ҳ����������ص����ݳ�Ա
	PTListNode m_ListNode;
	//Payload
	uint64_t m_Offset;
	uint32_t m_IsBusy;

	friend class PTList;

public:
	PTTreeNodeLeaf(PTTreeNodeInternal *Parent, uint64_t Offset, uint32_t IsBusy) :PTTreeNode(Parent, 1U), m_Offset(Offset), m_IsBusy(IsBusy)
	{

	}

	inline PTTreeNodeLeaf *TreeSeparate(IPTHeap *pHeap, PTTreeNode **ppTreeRoot, uint32_t TreeLevelInverse)
	{
		PTTreeNode **ppParentRefence = NULL;
		{
			PTTreeNodeInternal *pParent = this->m_Parent;
			if (pParent != NULL)
			{
				ppParentRefence = (pParent->m_ChildLeft == static_cast<PTTreeNode *>(this)) ? (&pParent->m_ChildLeft) : (&pParent->m_ChildRight);
				assert((*ppParentRefence) == static_cast<PTTreeNode *>(this));
			}
			else
			{
				ppParentRefence = ppTreeRoot;
			}
		}

		void *pBlockParent = pHeap->AlignedAlloc(sizeof(PTTreeNodeInternal), alignof(PTTreeNodeInternal));
		assert(pBlockParent != NULL);

		void *pBlockChildRight = pHeap->AlignedAlloc(sizeof(PTTreeNodeLeaf), alignof(PTTreeNodeLeaf));
		assert(pBlockChildRight != NULL);

		//Parent
		PTTreeNodeInternal *pParent = ::new(pBlockParent)PTTreeNodeInternal(this->m_Parent, static_cast<PTTreeNode *>(this), static_cast<PTTreeNode *>(pBlockChildRight));
		(*ppParentRefence) = static_cast<PTTreeNode *>(pParent);
		//ChildLeft ����֮ǰ���ڴ�
		this->m_Parent = pParent;
		//ChildRight
		return ::new(pBlockChildRight)PTTreeNodeLeaf(pParent, this->m_Offset + (static_cast<uint64_t>(1U) << static_cast<uint64_t>(TreeLevelInverse)), 0U);
	}

	inline void TreeCombine(IPTHeap *pHeap, PTTreeNode **ppTreeRoot, PTTreeNodeLeaf *pTreeSibling)
	{		
		assert(this->m_Parent == pTreeSibling->m_Parent);
		PTTreeNodeInternal *pParent = this->m_Parent;
		assert(pParent != NULL);//PTSubAllocImpl::AlignedFree���Ѿ����ж�

		PTTreeNode **ppAncesorRefence = NULL;
		{
			PTTreeNodeInternal *pAncesor = pParent->m_Parent;

			if (pAncesor != NULL)
			{
				ppAncesorRefence = (pAncesor->m_ChildLeft == static_cast<PTTreeNode *>(pParent)) ? (&pAncesor->m_ChildLeft) : (&pAncesor->m_ChildRight);
				assert((*ppAncesorRefence) == static_cast<PTTreeNode *>(pParent));
			}
			else
			{
				ppAncesorRefence = ppTreeRoot;
			}
		}

		//����֮ǰ���ڴ�
		this->m_Offset = this->m_Offset < pTreeSibling->m_Offset ? this->m_Offset : pTreeSibling->m_Offset;
		this->m_Parent = pParent->m_Parent;
		pHeap->AlignedFree(pTreeSibling);
		pHeap->AlignedFree(pParent);
		(*ppAncesorRefence) = static_cast<PTTreeNode *>(this);
	}

	inline void ListRemove()
	{
		//������ֻ���ڿ��п�
		assert(m_IsBusy == 0U);
		
		this->m_pPrev->m_pNext = this->m_ListNode.m_pNext;
		if (this->m_ListNode.m_pNext != NULL)
		{
			this->m_ListNode.m_pNext->m_pPrev = this->m_pPrev;
		}
	}

	inline bool PayloadIsBusy()
	{
		return m_IsBusy != 0U;
	}

	inline void PayloadMarkBusy()
	{
		m_IsBusy = 1U;
	}

	inline void PayloadMarkFree()
	{
		m_IsBusy = 0U;
	}

	inline uint64_t PayloadGetOffset()
	{
		return m_Offset;
	}
};

class PTList
{
	PTListNode m_pHeader;

	friend class PTBlockFreeHeader;

public:
	inline PTList()
	{
		m_pHeader.m_pNext = NULL;
	}

	inline void ListInsert(PTTreeNodeLeaf *pNode)
	{
		pNode->m_pPrev = &this->m_pHeader;
		if (this->m_pHeader.m_pNext != NULL)
		{
			this->m_pHeader.m_pNext->m_pPrev = &pNode->m_ListNode;
		}
		pNode->m_ListNode.m_pNext = this->m_pHeader.m_pNext;
		this->m_pHeader.m_pNext = pNode;
	}

	inline PTTreeNodeLeaf *ListPop()
	{
		if (this->m_pHeader.m_pNext == NULL)
		{
			return NULL;
		}
		else
		{
			if (this->m_pHeader.m_pNext->m_ListNode.m_pNext != NULL)
			{
				this->m_pHeader.m_pNext->m_ListNode.m_pNext->m_pPrev = &this->m_pHeader;
			}
			PTTreeNodeLeaf *pNodeReturn = this->m_pHeader.m_pNext;
			this->m_pHeader.m_pNext = this->m_pHeader.m_pNext->m_ListNode.m_pNext;
			return pNodeReturn;
		}
	}
};

class PTSubAllocImpl:public IPTSubAlloc
{
	PTTreeNode *m_pTreeRoot;
	uint32_t m_BlockWholeSizeLevel;
	uint32_t m_BlockMinimumSizeLevel;
	PTList m_ListArray[1];//��������ʽ���ù��캯��

public:
	static size_t GetAdditionalSizeRequire(uint64_t BlockWholeSize, uint64_t BlockMinimumSize)
	{
		assert(!(BlockMinimumSize&(BlockMinimumSize - 1U)));
		assert(!(BlockWholeSize&(BlockWholeSize - 1U)));//����BlockWhole���ڴ����ⲿ���䣬��������ȡ����2���ݴη��������������ڴ����

		assert(BlockWholeSize >= BlockMinimumSize);

		uint32_t BlockWholeSizeLevel = 0U;
		BlockWholeSize >>= static_cast<size_t>(1U);
		while (BlockWholeSize)
		{
			++BlockWholeSizeLevel;
			BlockWholeSize >>= static_cast<size_t>(1U);
		}

		uint32_t BlockMinimumSizeLevel = 0U;
		BlockMinimumSize >>= static_cast<size_t>(1U);
		while (BlockMinimumSize)
		{
			++BlockMinimumSizeLevel;
			BlockMinimumSize >>= static_cast<size_t>(1U);
		}

		//m_ListArray[1]
		//m_ListArray[BlockWholeSizeLevel - BlockMinimumSizeLevel];
		return sizeof(PTList)*(BlockWholeSizeLevel - BlockMinimumSizeLevel);
	}

	inline PTSubAllocImpl(IPTHeap *pHeap, uint64_t BlockWholeSize, uint64_t BlockMinimumSize)
	{
		assert((reinterpret_cast<uintptr_t>(this) % alignof(PTSubAllocImpl)) == 0U);

		void *pBlockWholeFree = pHeap->AlignedAlloc(sizeof(PTTreeNodeLeaf), alignof(PTTreeNodeLeaf));
		assert(pBlockWholeFree != NULL);

		PTTreeNodeLeaf *pTreeNodeBlockWholeFree = ::new(pBlockWholeFree)PTTreeNodeLeaf(NULL,0U, 0U);

		this->m_pTreeRoot = static_cast<PTTreeNode *>(pTreeNodeBlockWholeFree);

		assert(!(BlockMinimumSize&(BlockMinimumSize - 1U)));
		assert(!(BlockWholeSize&(BlockWholeSize - 1U)));//����BlockWhole���ڴ����ⲿ���䣬��������ȡ����2���ݴη��������������ڴ����

		assert(BlockWholeSize >= BlockMinimumSize);

		this->m_BlockWholeSizeLevel = 0U;
		BlockWholeSize >>= static_cast<size_t>(1U);
		while (BlockWholeSize)
		{
			++this->m_BlockWholeSizeLevel;
			BlockWholeSize >>= static_cast<size_t>(1U);
		}

		this->m_BlockMinimumSizeLevel = 0U;
		BlockMinimumSize >>= static_cast<size_t>(1U);
		while (BlockMinimumSize)
		{
			++this->m_BlockMinimumSizeLevel;
			BlockMinimumSize >>= static_cast<size_t>(1U);
		}

		uint32_t ListSize = (this->m_BlockWholeSizeLevel - this->m_BlockMinimumSizeLevel) + 1U;
		for (uint32_t i = 1U; i < ListSize; ++i)//m_pBlockListDoubleHeaderListArray[0]�Ĺ��캯�����ɱ�������ʽ���õ�
		{
			::new(this->m_ListArray + i)PTList();
		}

		this->m_ListArray[this->m_BlockWholeSizeLevel - this->m_BlockMinimumSizeLevel].ListInsert(pTreeNodeBlockWholeFree);
	}

	inline uint64_t AlignedAllocInternal(IPTHeap *pHeap, uint32_t AllocSizeLevelRequest)
	{
		//�ҵ�һ�鲻С��pow(2,AllocSizeLevelRequest)���ڴ��
		PTTreeNodeLeaf *pBlockFreeFind = NULL;

		uint32_t AllocSizeLevelFind = AllocSizeLevelRequest;
		while (AllocSizeLevelFind <= this->m_BlockWholeSizeLevel)
		{
			PTTreeNodeLeaf *pBlock = this->m_ListArray[AllocSizeLevelFind - this->m_BlockMinimumSizeLevel].ListPop();
			if (pBlock != NULL)
			{
				assert(!pBlock->PayloadIsBusy());//������ֻ���ڿ��п�
				pBlockFreeFind = pBlock;
				break;
			}
			++AllocSizeLevelFind;
		}

		//�ڴ治�㣬������
		if (pBlockFreeFind == NULL)
			return PTSUBALLOC_INVIALD_OFFSET_VALUE;

		//�ָ��ڴ�飬�õ���С��pow(2,AllocSizeLevelRequest)����С�ڴ��
		//|-------Free-------|
		//|Busy|Free|--Free--|

		//�������п�
		while (AllocSizeLevelFind > AllocSizeLevelRequest)//--0U����һ���ܴ������������ѭ����������>=
		{
			this->m_ListArray[(AllocSizeLevelFind - this->m_BlockMinimumSizeLevel) - 1U].ListInsert(
				pBlockFreeFind->TreeSeparate(pHeap, &this->m_pTreeRoot, AllocSizeLevelFind - 1U)
			);
			--AllocSizeLevelFind;
		}

		//����æµ��
		pBlockFreeFind->PayloadMarkBusy();

		return pBlockFreeFind->PayloadGetOffset();
	}

	inline void AlignedFreeInternal(IPTHeap *pHeap, PTTreeNodeLeaf *pBlockBusyFind, uint32_t BlockSizeLevelFind)
	{
		//�ݹ�
		while (true)
		{
			PTTreeNodeInternal *pBlockParent = pBlockBusyFind->TreeGetParent();
			if (pBlockParent == NULL)//Root
			{
				pBlockBusyFind->PayloadMarkFree();
				this->m_ListArray[BlockSizeLevelFind - this->m_BlockMinimumSizeLevel].ListInsert(pBlockBusyFind);
				break;//�ݹ���ݣ�β�����Ż�
			}
			else
			{
				PTTreeNode *pBlockSibling = pBlockParent->TreeGetChildLeft();
				pBlockSibling = (pBlockSibling != pBlockBusyFind) ? (pBlockSibling) : (pBlockParent->TreeGetChildRight());
				assert(pBlockSibling != pBlockBusyFind);
				if (!pBlockSibling->TreeIsLeaf())
				{
					pBlockBusyFind->PayloadMarkFree();
					this->m_ListArray[BlockSizeLevelFind - this->m_BlockMinimumSizeLevel].ListInsert(pBlockBusyFind);
					break;//�ݹ���ݣ�β�����Ż�
				}
				else
				{
					PTTreeNodeLeaf *pBlockSiblingLeaf = static_cast<PTTreeNodeLeaf *>(pBlockSibling);
					if (pBlockSiblingLeaf->PayloadIsBusy())
					{
						pBlockBusyFind->PayloadMarkFree();
						this->m_ListArray[BlockSizeLevelFind - this->m_BlockMinimumSizeLevel].ListInsert(pBlockBusyFind);
						break;//�ݹ���ݣ�β�����Ż�
					}
					else
					{
						pBlockSiblingLeaf->ListRemove();//�ӿ����������Ƴ�
						pBlockBusyFind->TreeCombine(pHeap, &this->m_pTreeRoot, pBlockSiblingLeaf);//��ջ��β�����Ż�
						++BlockSizeLevelFind;
						continue;//�ݹ�����
					}
				}
			}	
		}
	}

private:
	~PTSubAllocImpl();

	void PTCALL Destroy(IPTHeap *pHeap) override
	{
		assert(this->m_pTreeRoot->TreeIsLeaf());//���ͷŷ�����ǰҪ���ͷ������ѷ���Ŀ飬���������ڴ�й¶
		pHeap->AlignedFree(static_cast<void *>(this->m_pTreeRoot));
		//this->~PTSubAllocImpl();
		pHeap->AlignedFree(this);
	}

	uint64_t PTCALL AlignedAlloc(IPTHeap *pHeap, uint64_t Size, uint64_t Alignment) override
	{
		//����ȡ����2���ݴη�
		uint32_t AllocSizeLevelRequest = 0U;
		Size -= 1U;
		while (Size)
		{
			++AllocSizeLevelRequest;
			Size >>= static_cast<size_t>(1U);
		}

		//Alignment
		assert(!(Alignment&(Alignment - 1U)));
		uint32_t AlignmentLevelRequest = 0U;
		Alignment >>= static_cast<size_t>(1U);
		while (Alignment)
		{
			++AlignmentLevelRequest;
			Alignment >>= static_cast<size_t>(1U);
		}

		//max(AllocSizeLevelRequest,m_BlockMinimumSizeLevel)
		AllocSizeLevelRequest = AllocSizeLevelRequest > this->m_BlockMinimumSizeLevel ? AllocSizeLevelRequest : this->m_BlockMinimumSizeLevel;

		//���뵽Alignment
		AllocSizeLevelRequest = AllocSizeLevelRequest > AlignmentLevelRequest ? AllocSizeLevelRequest : AlignmentLevelRequest;

		return this->AlignedAllocInternal(pHeap, AllocSizeLevelRequest);
	}

	uint64_t PTCALL AlignedRealloc(IPTHeap *pHeap, uint64_t OffsetOriginal, uint64_t SizeNew, uint64_t AlignmentNew) override
	{
		PTTreeNodeLeaf *pBlockBusyFind = NULL;
		uint32_t BlockSizeLevelFind = this->m_BlockWholeSizeLevel;
		//���������
		{
			PTTreeNode *pBlockIterate = this->m_pTreeRoot;
			uint64_t OffsetIterate = 0U;
			while (BlockSizeLevelFind > this->m_BlockMinimumSizeLevel)//--0U����һ���ܴ������������ѭ����������>=
			{
				if (pBlockIterate->TreeIsLeaf())
				{
					pBlockBusyFind = static_cast<PTTreeNodeLeaf *>(pBlockIterate);
					break;
				}
				else
				{
					PTTreeNodeInternal *pBlockIterateInternal = static_cast<PTTreeNodeInternal *>(pBlockIterate);
					if (OffsetOriginal < (OffsetIterate + (static_cast<uint64_t>(1U) << (BlockSizeLevelFind - 1U))))
					{
						pBlockIterate = pBlockIterateInternal->TreeGetChildLeft();
					}
					else//��������
					{
						OffsetIterate += (static_cast<uint64_t>(1U) << (BlockSizeLevelFind - 1U));
						pBlockIterate = pBlockIterateInternal->TreeGetChildRight();
					}
				}
				--BlockSizeLevelFind;
			}
			if (pBlockIterate->TreeIsLeaf())
			{
				pBlockBusyFind = static_cast<PTTreeNodeLeaf *>(pBlockIterate);
			}
			assert(pBlockBusyFind != NULL);
			assert(pBlockBusyFind->PayloadGetOffset() == OffsetOriginal);
			assert(pBlockBusyFind->PayloadIsBusy());
		}

		//����ȡ����2���ݴη�
		uint32_t AllocSizeLevelRequest = 0U;
		SizeNew -= 1U;
		while (SizeNew)
		{
			++AllocSizeLevelRequest;
			SizeNew >>= static_cast<size_t>(1U);
		}

		//Alignment
		assert(!(AlignmentNew&(AlignmentNew - 1U)));
		uint32_t AlignmentLevelRequest = 0U;
		AlignmentNew >>= static_cast<size_t>(1U);
		while (AlignmentNew)
		{
			++AlignmentLevelRequest;
			AlignmentNew >>= static_cast<size_t>(1U);
		}

		//max(AllocSizeLevelRequest,m_BlockMinimumSizeLevel)
		AllocSizeLevelRequest = AllocSizeLevelRequest > this->m_BlockMinimumSizeLevel ? AllocSizeLevelRequest : this->m_BlockMinimumSizeLevel;

		//���뵽Alignment
		AllocSizeLevelRequest = AllocSizeLevelRequest > AlignmentLevelRequest ? AllocSizeLevelRequest : AlignmentLevelRequest;

		if (BlockSizeLevelFind >= AllocSizeLevelRequest)
		{
			return OffsetOriginal;
		}
		else
		{
			this->AlignedFreeInternal(pHeap, pBlockBusyFind, BlockSizeLevelFind);
			return this->AlignedAllocInternal(pHeap, AllocSizeLevelRequest);
		}
	}
	
	void PTCALL AlignedFree(IPTHeap *pHeap, uint64_t Offset) override
	{

		PTTreeNodeLeaf *pBlockBusyFind = NULL;
		uint32_t BlockSizeLevelFind = this->m_BlockWholeSizeLevel;
		//���������
		{
			PTTreeNode *pBlockIterate = this->m_pTreeRoot;
			uint64_t OffsetIterate = 0U;
			while (BlockSizeLevelFind  > this->m_BlockMinimumSizeLevel)//--0U����һ���ܴ������������ѭ����������>=
			{
				if (pBlockIterate->TreeIsLeaf())
				{
					pBlockBusyFind = static_cast<PTTreeNodeLeaf *>(pBlockIterate);
					break;
				}
				else
				{
					PTTreeNodeInternal *pBlockIterateInternal = static_cast<PTTreeNodeInternal *>(pBlockIterate);
					if (Offset < (OffsetIterate + (static_cast<uint64_t>(1U) << (BlockSizeLevelFind - 1U))))
					{
						pBlockIterate = pBlockIterateInternal->TreeGetChildLeft();
					}
					else//��������
					{
						OffsetIterate += (static_cast<uint64_t>(1U) << (BlockSizeLevelFind - 1U));
						pBlockIterate = pBlockIterateInternal->TreeGetChildRight();
					}
				}
				--BlockSizeLevelFind;
			}
			if (pBlockIterate->TreeIsLeaf())
			{
				pBlockBusyFind = static_cast<PTTreeNodeLeaf *>(pBlockIterate);
			}
			assert(pBlockBusyFind != NULL);
			assert(pBlockBusyFind->PayloadGetOffset() == Offset);
			assert(pBlockBusyFind->PayloadIsBusy());
		}

		this->AlignedFreeInternal(pHeap, pBlockBusyFind, BlockSizeLevelFind);
	}

};

IPTSubAlloc * PTCALL PTSubAlloc_Create(IPTHeap *pHeap, uint64_t BlockWholeSize, uint64_t BlockMinimumAlignment)
{
	void *pMemorySubAlloc = pHeap->AlignedAlloc(sizeof(PTSubAllocImpl) + PTSubAllocImpl::GetAdditionalSizeRequire(BlockWholeSize, BlockMinimumAlignment), alignof(PTSubAllocImpl));
	assert(pMemorySubAlloc != NULL);
	return static_cast<IPTSubAlloc *>(::new(pMemorySubAlloc)PTSubAllocImpl(pHeap, BlockWholeSize, BlockMinimumAlignment));
}
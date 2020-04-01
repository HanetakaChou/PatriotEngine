#include "StagingBuffer.h"

template <typename T>
static inline T roundUp(const T value, const T alignment)
{
    auto temp = value + alignment - static_cast<T>(1);
    return temp - temp % alignment;
}

bool StagingBuffer::allocate(size_t sizeInBytes, size_t alignment, uint8_t **ptrOut, VkDeviceSize *offsetOut)
{
    assert(ptrOut != NULL);
    assert(offsetOut != NULL);

    size_t sizeToAllocate = roundUp(sizeInBytes, alignment);

    assert((mNextAllocationOffset + sizeToAllocate) < mSize);

    (*ptrOut) = m_StagingPointer + mNextAllocationOffset;
    (*offsetOut) = mNextAllocationOffset;

    mNextAllocationOffset += sizeToAllocate;

    return true;
}
#ifndef _VK_STAGINGBUFFER_H_
#define _VK_STAGINGBUFFER_H_ 1

#include <vulkan/vulkan.h>

#include <assert.h>

class StagingBuffer
{
    VkBuffer m_buffer;
    VkDeviceMemory m_mem;
    size_t mSize;
    size_t mNextAllocationOffset;
    size_t mLastFlushOrInvalidateOffset;
    uint8_t *m_StagingPointer;

public:
    inline StagingBuffer() : m_buffer(VK_NULL_HANDLE), m_mem(VK_NULL_HANDLE), m_StagingPointer(NULL)
    {
    }

    void init(VkBuffer buffer, VkDeviceMemory mem, size_t size, uint8_t *stagingPointer)
    {
        assert(VK_NULL_HANDLE == m_buffer);
        assert(VK_NULL_HANDLE == m_mem);
        assert(NULL == m_StagingPointer);

        m_buffer = buffer;
        m_mem = mem;
        mSize = size;
        mNextAllocationOffset = 0;
        mLastFlushOrInvalidateOffset = 0;
        m_StagingPointer = stagingPointer;
    }

    bool allocate(size_t sizeInBytes, size_t alignment, uint8_t **ptrOut, VkDeviceSize *offsetOut);

    VkBuffer buffer() { return m_buffer; }
};

#endif
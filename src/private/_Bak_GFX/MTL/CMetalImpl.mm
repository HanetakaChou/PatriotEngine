#include "CMetalImpl.h"
#include <new>

extern "C" ICMTLDevice * CMTLAPI_CALL CMTLCreateSystemDefaultDevice(void *(CMTLAPI_PTR *pfnAllocation)(size_t size,size_t alignment), void (CMTLAPI_PTR *pfnFree)(void *pMemory))
{
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    return new(pfnAllocation(sizeof(CMTLDeviceImpl),alignof(CMTLDeviceImpl)))CMTLDeviceImpl(device, pfnAllocation, pfnFree);
}

inline CMTLDeviceImpl::CMTLDeviceImpl(id<MTLDevice> device, void *(CMTLAPI_PTR *pfnAllocation)(size_t size,size_t alignment), void (CMTLAPI_PTR *pfnFree)(void *pMemory)):m_device(device),m_pfnAllocation(pfnAllocation),m_pfnFree(pfnFree)
{
    
}

inline CMTLDeviceImpl::~CMTLDeviceImpl()
{
    m_device = nil;
}

void CMTLAPI_CALL CMTLDeviceImpl::Release()
{
    this->~CMTLDeviceImpl();
    m_pfnFree(this);
}

CMTLBool CMTLDeviceImpl::IsHeadless()
{
	return (m_device.isHeadless) ? CMTLTrue : CMTLFalse;
}

CMTLBool CMTLDeviceImpl::IsLowPower()
{
	return (m_device.isLowPower) ? CMTLTrue : CMTLFalse;
}

CMTLBool CMTLDeviceImpl::IsRemovable()
{
	return (m_device.isRemovable) ? CMTLTrue : CMTLFalse;
}

CMTLUInt64 CMTLDeviceImpl::RegistryID()
{
	return m_device.registryID;
}

char const * CMTLDeviceImpl::Name()
{
	return [m_device.name UTF8String];
}


ICMTLEvent * CMTLAPI_CALL CMTLDeviceImpl::MakeEvent()
{
    return NULL;
}


ICMTLFence * CMTLAPI_CALL CMTLDeviceImpl::MakeFence()
{
    return NULL;
}

ICMTLHeap * CMTLAPI_CALL CMTLDeviceImpl::MakeHeap(CMTLHeapDescriptor const *pDescriptor)
{
    return NULL;
}

CMTLSizeAndAlign CMTLAPI_CALL CMTLDeviceImpl::HeapBufferSizeAndAlign(CMTLInt length, CMTLResourceOptions options)
{
    CMTLSizeAndAlign v={};
    return v;
}

CMTLSizeAndAlign CMTLAPI_CALL CMTLDeviceImpl::HeapTextureSizeAndAlign(CMTLTextureDescriptor const *pDescriptor)
{
    CMTLSizeAndAlign v={};
    return v;
}


void CMTLAPI_CALL CMTLHeapImpl::Label(char const *label)
{
	m_heap.label = [NSString stringWithUTF8String:label];
}

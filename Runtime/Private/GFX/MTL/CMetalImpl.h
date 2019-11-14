#include "CMetal.h"

#import <Metal/Metal.h>

class CMTLDeviceImpl: public ICMTLDevice
{
    id<MTLDevice> m_device;
    void *(CMTLAPI_PTR *m_pfnAllocation)(size_t size,size_t alignment);
    void (CMTLAPI_PTR *m_pfnFree)(void *pMemory);
    
    inline CMTLDeviceImpl(id<MTLDevice> device, void *(CMTLAPI_PTR *pfnAllocation)(size_t size,size_t alignment), void (CMTLAPI_PTR *pfnFree)(void *pMemory));
    inline ~CMTLDeviceImpl();
    
    friend ICMTLDevice * CMTLAPI_CALL ::CMTLCreateSystemDefaultDevice(void *(CMTLAPI_PTR *pfnAllocation)(size_t size,size_t alignment), void (CMTLAPI_PTR *pfnFree)(void *pMemory));
public:
    
    void CMTLAPI_CALL Release() override;
    
	//Querying Properties
	CMTLBool CMTLAPI_CALL IsHeadless() override;
	CMTLBool CMTLAPI_CALL IsLowPower() override;
	CMTLBool CMTLAPI_CALL IsRemovable() override;
	CMTLUInt64 CMTLAPI_CALL RegistryID()override;
	char const * CMTLAPI_CALL Name() override;

    //Synchronizing Commands
    ICMTLEvent * CMTLAPI_CALL MakeEvent() override;
    ICMTLFence * CMTLAPI_CALL MakeFence() override;
    
    ICMTLHeap * CMTLAPI_CALL MakeHeap(CMTLHeapDescriptor const *pDescriptor) override;
    CMTLSizeAndAlign CMTLAPI_CALL HeapBufferSizeAndAlign(CMTLInt length, CMTLResourceOptions options) override;
    CMTLSizeAndAlign CMTLAPI_CALL HeapTextureSizeAndAlign(CMTLTextureDescriptor const *pDescriptor) override;
};

class CMTLHeapImpl :public ICMTLHeap
{
	id<MTLHeap> m_heap;

public:
	void CMTLAPI_CALL Label(char const *label) override;
};
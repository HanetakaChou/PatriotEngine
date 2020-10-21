#ifndef _C_METAL_H_
#define _C_METAL_H_

#include <stddef.h>
#include <stdint.h>

#define CMTLAPI_ATTR __attribute__((visibility("default")))
#define CMTLAPI_CALL
#define CMTLAPI_PTR

typedef bool CMTLBool;
constexpr static CMTLBool const CMTLTrue = true;
constexpr static CMTLBool const CMTLFalse = false;
typedef intptr_t CMTLInt;
typedef uintptr_t CMTLUInt;
typedef uint64_t CMTLUInt64;

enum class MTLFeatureSet : CMTLUInt
{
	iOS_GPUFamily1_v1 = 0,
	iOS_GPUFamily1_v2 = 2,

	macOS_ReadWriteTextureTier2 = 10002
};

enum class CMTLStorageMode :CMTLUInt
{
    shared = 0,
    managed = 1,
    _private_ = 2,
    memoryless = 3
};

enum class CMTLCPUCacheMode :CMTLUInt
{
    defaultCache = 0,
    writeCombined = 1
};

enum class CMTLResourceOptionBits :CMTLUInt
{
    cpuCacheModeWriteCombined,
    storageModeShared,
    storageModeManaged,
    storageModePrivate,
    storageModeMemoryless,
    hazardTrackingModeUntracked,
    optionCPUCacheModeWriteCombined
};

typedef CMTLUInt CMTLResourceOptions;

enum class CMTLTextureType :CMTLUInt
{
    type1D = 0,
    type1DArray = 1,
    type2D = 2,
    type2DArray = 3,
    type2DMultisample = 4,
    typeCube = 5,
    typeCubeArray = 6,
    type3D = 7,
    type2DMultisampleArray = 8,
    typeTextureBuffer = 9
};

enum class CMTLPixelFormat : CMTLUInt
{
    a8Unorm = 1,
    r8Unorm = 10,
    astc_4x4_srgb = 186,
    bc1_rgba = 130
};

enum class CMTLTextureUsageBits :CMTLUInt
{
    unknown,
    shaderRead,
    shaderWrite,
    renderTarget,
    pixelFormatView
};

typedef CMTLUInt CMTLTextureUsage;

struct CMTLTextureDescriptor
{
    CMTLTextureType textureType;
    CMTLPixelFormat pixelFormat;
    CMTLInt width;
    CMTLInt height;
    CMTLInt depth;
    CMTLInt mipmapLevelCount;
    CMTLInt sampleCount;
    CMTLInt arrayLength;
    CMTLResourceOptions options;
    CMTLCPUCacheMode cpuCacheMode;
    CMTLStorageMode storageMode;
    CMTLBool allowGPUOptimizedContents;
    CMTLTextureUsage usage;
};

struct CMTLHeapDescriptor
{
    CMTLStorageMode storageMode;
    CMTLCPUCacheMode cpuCacheMode;
    CMTLInt size;
};

struct CMTLSizeAndAlign
{
    CMTLInt align;
    CMTLInt size;
};

enum class CMTLPurgeableState : CMTLUInt
{
    keepCurrent = 1,
    nonVolatile = 2,
    _volatile_ = 3,
    empty = 4
};

struct ICMTLDevice;
struct ICMTLHeap;
struct ICMTLBuffer;
struct ICMTLTexture;
struct ICMTLCommandBuffer;
struct ICMTLEvent;
struct ICMTLFence;

struct ICMTLDevice
{
    virtual void CMTLAPI_CALL Release() = 0;
    
	//Querying Properties
	virtual CMTLBool CMTLAPI_CALL IsHeadless() = 0;
	virtual CMTLBool CMTLAPI_CALL IsLowPower() = 0;
	virtual CMTLBool CMTLAPI_CALL IsRemovable() = 0;
	virtual CMTLUInt64 CMTLAPI_CALL RegistryID() = 0;
	virtual char const * CMTLAPI_CALL Name() = 0;

    //Synchronizing Commands
    virtual ICMTLEvent * CMTLAPI_CALL MakeEvent() = 0;
    virtual ICMTLFence * CMTLAPI_CALL MakeFence() = 0;
    
    virtual ICMTLHeap * CMTLAPI_CALL MakeHeap(CMTLHeapDescriptor const *pDescriptor) = 0;
    virtual CMTLSizeAndAlign CMTLAPI_CALL HeapBufferSizeAndAlign(CMTLInt length, CMTLResourceOptions options) = 0;
    virtual CMTLSizeAndAlign CMTLAPI_CALL HeapTextureSizeAndAlign(CMTLTextureDescriptor const *pDescriptor) = 0;
};

extern "C" CMTLAPI_ATTR ICMTLDevice * CMTLAPI_CALL CMTLCreateSystemDefaultDevice(void *(CMTLAPI_PTR *pfnAllocation)(size_t size,size_t alignment), void (CMTLAPI_PTR *pfnFree)(void *pMemory));

struct ICMTLHeap
{
    virtual ICMTLDevice * CMTLAPI_CALL Device() = 0;
    virtual char const * CMTLAPI_CALL Label() = 0;
    virtual void CMTLAPI_CALL Label(char const *label) = 0;
    
    virtual CMTLStorageMode CMTLAPI_CALL StorageMode() = 0;
    virtual CMTLCPUCacheMode CMTLAPI_CALL CpuCacheMode() = 0;
    virtual CMTLInt CMTLAPI_CALL Size() = 0;
    virtual CMTLInt CMTLAPI_CALL UsedSize() = 0;
    virtual CMTLInt CMTLAPI_CALL CurrentAllocatedSize() = 0;
    virtual CMTLInt CMTLAPI_CALL MaxAvailableSize(CMTLInt alignment) = 0;
    
    virtual CMTLPurgeableState CMTLAPI_CALL SetPurgeableState(CMTLPurgeableState _state) = 0;
    
    virtual ICMTLBuffer * CMTLAPI_CALL MakeBuffer(CMTLInt length, CMTLResourceOptions options) = 0;
    virtual ICMTLTexture * CMTLAPI_CALL MakeTexture(CMTLTextureDescriptor const *pDescriptor) = 0;
    
};

struct ICMTLCommandBuffer
{
    //Synchronizing a Nonshareable Event
    virtual void EncodeSignalEvent(ICMTLEvent *pEvent, CMTLUInt64 value) = 0;
    virtual void EncodeWaitForEvent(ICMTLEvent *pEvent, CMTLUInt64 value) = 0;
};

struct ICMTLEvent
{
    //Identifying the Event
    virtual ICMTLDevice * CMTLAPI_CALL Device() = 0;
    virtual char * CMTLAPI_CALL Label() = 0;
    virtual void CMTLAPI_CALL Label(char *string) = 0;
};

#endif

// concrtinternal.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//
// If this is defined to 1, memory logging of a series of things related to UMS will be preserved in contexts and virtual
// processors.  This *SERIOUSLY AFFECTS* performance.  It is a debugging only facility.
//
#define _UMSTRACE 0

//
// ifdefs that are not currently enabled
//

//
// If this is defined to 1, we will explicitly *NOT* work around a rare Win7 RTM issue.  This is for testing purposes only as the workaround
// is disabled on non-RTM builds.
//
#define _NO_WIN7_WORKAROUND 0

#include "targetver.h"

// Windows headers that we need

// We need to bring in data structures and function prototypes defined
// on Win7 for UMS.
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601

#include <windows.h>
#include <winnt.h>
#include <oaidl.h>

#undef Yield    // The windows headers #define Yield, a name we want to use

#include <wmistr.h>
#include <evntrace.h>

// C Runtime Header Files:

#include <sect_attribs.h>
#include <stdio.h>
#include <stddef.h>
#include <sal.h>
#include <new.h>
//#include <oacr.h>
#include <malloc.h>
#include <stdexcept>
#include <intsafe.h>
#include <internal.h>

//#pragma warning(disable :4127)

// Forward declarations
namespace Concurrency
{
namespace details
{
    // From runtime

    class SchedulerResourceManagement;
    class ScheduleGroupBase;
    class CacheLocalScheduleGroup;
    class FairScheduleGroup;
    class SchedulingRing;
    class SchedulingNode;
    class VirtualProcessor;
    class ThreadVirtualProcessor;
    class UMSThreadVirtualProcessor;
    class SchedulerBase;
    class ThreadScheduler;
    class UMSThreadScheduler;
    class ContextBase;
    class InternalContextBase;
    class ExternalContextBase;
    class ExternalStatistics;
    class UMSThreadInternalContext;
    class _UnrealizedChore;

    // From resource manager

    class ThreadProxy;

    class SchedulerProxy;
    class ResourceManager;
    class ExecutionResource;
    class VirtualProcessorRoot;
    class FreeVirtualProcessorRoot;
    class ThreadProxy;
    class FreeThreadProxy;
    struct IThreadProxyFactory;
    class FreeThreadProxyFactory;
    class SubAllocator;

    class UMSBackgroundPoller;
    class UMSSchedulerProxy;
    class UMSThreadProxy;
    class UMSFreeVirtualProcessorRoot;
    class UMSFreeThreadProxy;
    class TransmogrifiedPrimary;
    class Transmogrificator;
}

// From runtime


struct IExecutionContext;

// From resource manager

struct IScheduler;
struct IResourceManager;

} // namespace Concurrency

const int KB = 1024;
const size_t s_cacheLineSize = 64; 

// Public ConcRT Header Files:

#include <concrt.h>
#include <concrtrm.h>

#if defined(_DEBUG)
#define CTX_DEBUGBIT_ADDEDTORUNNABLES 0x00000001
#define CTX_DEBUGBIT_REMOVEDFROMRUNNABLES 0x00000002
#define CTX_DEBUGBIT_ADDEDTOLOCALRUNNABLECONTEXTS 0x00000004
#define CTX_DEBUGBIT_POPPEDFROMLOCALRUNNABLECONTEXTS 0x00000008
#define CTX_DEBUGBIT_STOLENFROMLOCALRUNNABLECONTEXTS 0x00000010
#define CTX_DEBUGBIT_PULLEDFROMCOMPLETIONLIST 0x00000020
#define CTX_DEBUGBIT_AFFINITIZED 0x00000040
#define CTX_DEBUGBIT_COOPERATIVEBLOCKED 0x00000080
#define CTX_DEBUGBIT_UMSBLOCKED 0x00000100
#define CTX_DEBUGBIT_CRITICALNOTIFY 0x00000200
#define CTX_DEBUGBIT_CHAINEDCRITICALBLOCK 0x00000400
#define CTX_DEBUGBIT_WAKEFROMCHAINEDCRITICALBLOCK 0x00000800
#define CTX_DEBUGBIT_LIKELYTOSTARTUPIDLEVPROCONOTHERCONTEXT 0x00001000
#define CTX_DEBUGBIT_STARTUPIDLEVPROCONADD 0x00002000
#define CTX_DEBUGBIT_ACTIVATEDAFTERRMAWAKEN 0x00004000
#define CTX_DEBUGBIT_RELEASED 0x00008000
#define CTX_DEBUGBIT_REINITIALIZED 0x00010000
#define CTX_DEBUGBIT_SWITCHTOWITHASSOCIATEDCHORE 0x00020000
#define CTX_DEBUGBIT_PRIMARYAFFINITIZEFROMSEARCH 0x00040000
#define CTX_DEBUGBIT_PRIMARYRESERVEDCONTEXT 0x00080000
#define CTX_DEBUGBIT_PRIMARYAFFINITIZEFROMCRITICAL 0x00100000
#define CTX_DEBUGBIT_PRIMARYSWITCHTOFAILED 0x00200000
#define CTX_DEBUGBIT_HOLDINGUMSBLOCKEDCONTEXT 0x00400000

namespace Concurrency
{
namespace details
{
    void SetContextDebugBits(Concurrency::details::InternalContextBase *pContext, DWORD bits);
} // namespace details
} // namespace Concurrency
#endif // _DEBUG


// Namespaces we use internally

using namespace Concurrency;
using namespace Concurrency::details;

// Internal Header Files (Both):
#include "utils.h"
#include "collections.h"
#include "Trace.h"

// Internal Header Files (Resource Manager):
#include "rminternal.h"
#include "ExecutionResource.h"
#include "VirtualProcessorRoot.h"
#include "FreeVirtualProcessorRoot.h"
#include "HillClimbing.h"

// Internal Header Files (Scheduler):
#include "WorkStealingQueue.h"
#include "StructuredWorkStealingQueue.h"
#include "workqueue.h"
#include "RealizedChore.h"

#include "SearchAlgorithms.h"

#include "SchedulingNode.h"
#include "VirtualProcessor.h"
#include "ThreadVirtualProcessor.h"
#include "UMSThreadVirtualProcessor.h"

#include "SubAllocator.h"
#include "ContextBase.h"
#include "SchedulerBase.h"
#include "InternalContextBase.h"
#include "SchedulingRing.h"
#include "ScheduleGroupBase.h"
#include "CacheLocalScheduleGroup.h"
#include "FairScheduleGroup.h"

#include "ExternalContextBase.h"
#include "ThreadInternalContext.h"
#include "SchedulerResourceManagement.h"
#include "ThreadScheduler.h"
#include "UMSThreadScheduler.h"
#include "align.h"
#include "TaskCollection.h"
#include "SchedulerProxy.h"

#include "UMSWrapper.h"

#include "UMSThreadInternalContext.h"
#include "UMSSchedulingContext.h"

#include "TransmogrifiedPrimary.h"
#include "Transmogrificator.h"
#include "ThreadProxy.h"
#include "FreeThreadProxy.h"
#include "Timer.h"

#include "UMSBackgroundPoller.h"
#include "UMSThreadProxy.h"
#include "UMSFreeThreadProxy.h"

#include "ThreadProxyFactory.h"
#include "ResourceManager.h"
#include "UMSSchedulerProxy.h"
#include "UMSFreeVirtualProcessorRoot.h"

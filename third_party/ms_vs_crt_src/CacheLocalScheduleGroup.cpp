// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// CacheLocalScheduleGroup.cpp
//
// Implementation file for CacheLocalScheduleGroup.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Puts a runnable context into the runnables collection in the schedule group.
    /// </summary>
    void CacheLocalScheduleGroup::AddToRunnablesCollection(InternalContextBase* pContext)
    {
        m_runnableContexts.Enqueue(pContext);
    }

} // namespace details
} // namespace Concurrency

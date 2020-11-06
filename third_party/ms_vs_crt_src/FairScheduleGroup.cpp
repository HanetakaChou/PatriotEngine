// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FairScheduleGroup.cpp
//
// Implementation file for FairScheduleGroup.
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
    void FairScheduleGroup::AddToRunnablesCollection(InternalContextBase* pContext)
    {
        m_runnableContexts.Enqueue(pContext);
    }

} // namespace details
} // namespace Concurrency

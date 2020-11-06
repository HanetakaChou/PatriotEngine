// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Exceptions.cpp
//
// Implementation for concurrency runtime exceptions.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
//
// scheduler_resource_allocation_error
//

/// <summary>
///     Construct a scheduler_resource_allocation_error exception with a message and an error code
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
/// <param name="hresult">
///     HRESULT of error that caused this exception
/// </param>
_CRTIMP scheduler_resource_allocation_error::scheduler_resource_allocation_error(const char* message, HRESULT hresult) throw()
    : exception(message), _Hresult(hresult)
{ }

/// <summary>
///     Construct a scheduler_resource_allocation_error exception with an error code
/// </summary>
/// <param name="hresult">
///     HRESULT of error that caused this exception
/// </param>
_CRTIMP scheduler_resource_allocation_error::scheduler_resource_allocation_error(HRESULT hresult) throw()
    : exception(""), _Hresult(hresult)
{
}

/// <summary>
///     Get the error code that caused this exception
/// </summary>
///<returns>HRESULT of error that caused the exception</returns>
_CRTIMP HRESULT scheduler_resource_allocation_error::get_error_code() const throw()
{
    return _Hresult;
}

//
// unsupported_os -- exception thrown whenever an unsupported OS is used
//

/// <summary>
///     Construct a unsupported_os exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP unsupported_os::unsupported_os(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a unsupported_os exception
/// </summary>
_CRTIMP unsupported_os::unsupported_os() throw()
    : exception("")
{
}

//
// scheduler_not_attached
//

/// <summary>
///     Construct a scheduler_not_attached exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP scheduler_not_attached::scheduler_not_attached(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a scheduler_not_attached exception
/// </summary>
_CRTIMP scheduler_not_attached::scheduler_not_attached() throw()
    : exception("")
{
}

//
// improper_scheduler_attach
//

/// <summary>
///     Construct a improper_scheduler_attach exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP improper_scheduler_attach::improper_scheduler_attach(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a improper_scheduler_attach exception
/// </summary>
_CRTIMP improper_scheduler_attach::improper_scheduler_attach() throw()
    : exception("")
{
}

//
// improper_scheduler_detach
//

/// <summary>
///     Construct a improper_scheduler_detach exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP improper_scheduler_detach::improper_scheduler_detach(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a improper_scheduler_detach exception
/// </summary>
_CRTIMP improper_scheduler_detach::improper_scheduler_detach() throw()
    : exception("")
{
}

//
// improper_scheduler_reference
//

/// <summary>
///     Construct a improper_scheduler_reference exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP improper_scheduler_reference::improper_scheduler_reference(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a improper_scheduler_reference exception
/// </summary>
_CRTIMP improper_scheduler_reference::improper_scheduler_reference() throw()
    : exception("")
{
}

//
// default_scheduler_exists
//

/// <summary>
///     Construct a default_scheduler_exists exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP default_scheduler_exists::default_scheduler_exists(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a default_scheduler_exists exception
/// </summary>
_CRTIMP default_scheduler_exists::default_scheduler_exists() throw()
    : exception("")
{
}

//
// context_unblock_unbalanced
//

/// <summary>
///     Construct a context_unblock_unbalanced exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP context_unblock_unbalanced::context_unblock_unbalanced(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a context_unblock_unbalanced exception
/// </summary>
_CRTIMP context_unblock_unbalanced::context_unblock_unbalanced() throw()
    : exception("")
{
}

//
// context_self_unblock
//

/// <summary>
///     Construct a context_self_unblock exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP context_self_unblock::context_self_unblock(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a context_unblock_unbalanced exception
/// </summary>
_CRTIMP context_self_unblock::context_self_unblock() throw()
    : exception("")
{
}

//
// missing_wait -- Exception thrown whenever a task collection is destructed without being waited upon and still contains work
//

/// <summary>
///     Construct a missing_wait exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP missing_wait::missing_wait(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a missing_wait exception
/// </summary>
_CRTIMP missing_wait::missing_wait() throw()
    : exception("")
{
}

//
// bad_target -- Exception thrown whenever a messaging block is given a bad target pointer
//

/// <summary>
///     Construct a bad_target exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP bad_target::bad_target(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a bad_target exception
/// </summary>
_CRTIMP bad_target::bad_target() throw()
    : exception("")
{
}

//
// message_not_found -- Exception thrown whenever a messaging block is unable to find a requested message
//

/// <summary>
///     Construct a message_not_found exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP message_not_found::message_not_found(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a message_not_found exception
/// </summary>
_CRTIMP message_not_found::message_not_found() throw()
    : exception("")
{
}

//
// invalid_link_target -- Exception thrown whenever a messaging block tries to link a target twice
// when it should only occur once
//

/// <summary>
///     Construct a invalid_link_target exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_link_target::invalid_link_target(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a message_not_found exception
/// </summary>
_CRTIMP invalid_link_target::invalid_link_target() throw()
    : exception("")
{
}

//
// invalid_scheduler_policy_key -- Exception thrown whenever a policy key is invalid
//

/// <summary>
///     Construct a invalid_scheduler_policy_key exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_scheduler_policy_key::invalid_scheduler_policy_key(const char* message) throw()
    : exception(message)
{ 
}

/// <summary>
///     Construct a invalid_scheduler_policy_key exception
/// </summary>
_CRTIMP invalid_scheduler_policy_key::invalid_scheduler_policy_key() throw()
    : exception("")
{
}

//
// invalid_scheduler_policy_value -- Exception thrown whenever a policy value is invalid
//

/// <summary>
///     Construct a invalid_scheduler_policy_value exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_scheduler_policy_value::invalid_scheduler_policy_value(const char* message) throw()
    : exception(message)
{ 
}

/// <summary>
///     Construct a invalid_scheduler_policy_value exception
/// </summary>
_CRTIMP invalid_scheduler_policy_value::invalid_scheduler_policy_value() throw()
    : exception("")
{
}

//
// invalid_scheduler_policy_thread_specification -- Exception thrown whenever a combination of thread specifications are invalid
//

/// <summary>
///     Construct a invalid_scheduler_policy_thread_specification exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_scheduler_policy_thread_specification::invalid_scheduler_policy_thread_specification(const char* message) throw()
    : exception(message)
{ 
}

/// <summary>
///     Construct a invalid_scheduler_policy_thread_specification exception
/// </summary>
_CRTIMP invalid_scheduler_policy_thread_specification::invalid_scheduler_policy_thread_specification() throw()
    : exception("")
{
}

//
// invalid_operation -- Exception thrown when an invalid operation is performed.
//

/// <summary>
///     Construct an invalid_operation exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_operation::invalid_operation(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct an invalid_operation exception
/// </summary>
_CRTIMP invalid_operation::invalid_operation() throw()
    : exception("")
{
}

//
// nested_scheduler_missing_detach -- Exception thrown when the runtime can detect that
// a Detach() was missing for a nested scheduler.
//

/// <summary>
///     Construct an nested_scheduler_missing_detach exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP nested_scheduler_missing_detach::nested_scheduler_missing_detach(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct an nested_scheduler_missing_detach exception
/// </summary>
_CRTIMP nested_scheduler_missing_detach::nested_scheduler_missing_detach() throw()
    : exception("")
{
}

//
// operation_timed_out -- An operation has timed out.
//

/// <summary>
///     Construct an operation_timed_out exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP operation_timed_out::operation_timed_out(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct an operation_timed_out exception
/// </summary>
_CRTIMP operation_timed_out::operation_timed_out() throw()
    : exception("")
{
}

//
// invalid_multiple_scheduling -- An exception thrown when a chore/task_handle is scheduled multiple
// times on one or more *TaskCollection/*task_group constructs before completing.
//

/// <summary>
///     Construct an invalid_multiple_scheduling exception with a message
/// </summary>
/// <param name="_Message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_multiple_scheduling::invalid_multiple_scheduling(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct an invalid_multiple_scheduling exception
/// </summary>
_CRTIMP invalid_multiple_scheduling::invalid_multiple_scheduling() throw()
    : exception("")
{
}

//
//
// invalid_oversubscribe_operation -- An exception thrown when Context::Oversubscribe(false)
// is called without calling Context::Oversubscribe(true) first.
//

/// <summary>
///     Construct an invalid_oversubscribe_operation exception with a message
/// </summary>
/// <param name="_Message">
///     Descriptive message of error
/// </param>
_CRTIMP invalid_oversubscribe_operation::invalid_oversubscribe_operation(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct an invalid_oversubscribe_operation exception
/// </summary>
_CRTIMP invalid_oversubscribe_operation::invalid_oversubscribe_operation() throw()
    : exception("")
{
}

//
// improper_lock
//

/// <summary>
///     Construct a improper_lock exception with a message
/// </summary>
/// <param name="message">
///     Descriptive message of error
/// </param>
_CRTIMP improper_lock::improper_lock(const char* message) throw()
    : exception(message)
{ }

/// <summary>
///     Construct a improper_lock exception
/// </summary>
_CRTIMP improper_lock::improper_lock() throw()
    : exception("")
{
}

namespace details
{
    //
    // task_canceled
    //

    /// <summary>
    ///     Construct a task_canceled exception with a message
    /// </summary>
    /// <param name="message">
    ///     Descriptive message of error
    /// </param>
    _CRTIMP task_canceled::task_canceled(const char* message) throw()
        : exception(message)
    { }

    /// <summary>
    ///     Construct a task_canceled exception
    /// </summary>
    _CRTIMP task_canceled::task_canceled() throw()
        : exception("")
    {
    }
} // namespace details
} // namespace Concurrency

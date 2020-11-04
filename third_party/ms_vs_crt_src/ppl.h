/***
* ==++==
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* ppl.h
*
* Parallel Patterns Library
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#pragma once

#include <crtdefs.h>
#include <concrt.h>
#include <stdexcept>
#include <iterator>
#include <functional>
#include <type_traits>

#pragma pack(push,_CRT_PACKING)

// Define the level of tracing to use

#define _TRACE_LEVEL_INFORMATION 4

/// <summary>
///     The <c>Concurrency</c> namespace provides classes and functions that give you access to the Concurrency Runtime,
///     a concurrent programming framework for C++. For more information, see <see cref="Concurrency Runtime"/>.
/// </summary>
/**/
namespace Concurrency
{
namespace details
{
    _CRTIMP2 size_t __cdecl _GetCombinableSize();

    _CRTIMP2 DWORD __cdecl _GetCurrentThreadId();
} // namespace details

class structured_task_group;
class task_group;

/// <summary>
///     The <c>task_handle</c> class represents an individual parallel work item. It encapsulates the instructions and the data required
///     to execute a piece of work.
/// </summary>
/// <typeparam name="_Function">
///     The type of the function object that will be invoked to execute the work represented by the <c>task_handle</c> object.
/// </typeparam>
/// <remarks>
///     <c>task_handle</c> objects can be used in conjunction with a <c>structured_task_group</c> or a more general <c>task_group</c> object,
///     to decompose work into parallel tasks. For more information, see <see cref="Task Parallelism"/>.
///     <para>Note that the creator of a <c>task_handle</c> object is responsible for maintaining the lifetime of the created
///     <c>task_handle</c> object until it is no longer required by the Concurrency Runtime.  Typically, this means that the <c>task_handle</c>
///     object must not destruct until either the <c>wait</c> or <c>run_and_wait</c> method of the <c>task_group</c> or
///     <c>structured_task_group</c> to which it is queued has been called.</para>
///     <para><c>task_handle</c> objects are typically used in conjunction with C++ lambdas.  Because you do not know the true type of the lambda,
///     the <see cref="make_task Function">make_task</see> function is typically used to create a <c>task_handle</c> object.</para>
/// </remarks>
/// <seealso cref="task_group Class"/>
/// <seealso cref="structured_task_group Class"/>
/// <seealso cref="make_task Function"/>
/// <seealso cref="task_group::run Method"/>
/// <seealso cref="task_group::wait Method"/>
/// <seealso cref="task_group::run_and_wait Method"/>
/// <seealso cref="structured_task_group::run Method"/>
/// <seealso cref="structured_task_group::wait Method"/>
/// <seealso cref="structured_task_group::run_and_wait Method"/>
/**/
template<typename _Function>
class task_handle : public ::Concurrency::details::_UnrealizedChore
{
public:
    /// <summary>
    ///     Constructs a new <c>task_handle</c> object.  The work of the task is performed by invoking the function specified as
    ///     a parameter to the constructor.
    /// </summary>
    /// <param name="_Func">
    ///     The function that will be invoked to execute the work represented by the <c>task_handle</c> object.  This may be a lambda functor,
    ///     a pointer to a function, or any object that supports a version of the function call operator with the signature <c>void operator()()</c>.
    /// </param>
    /**/
    task_handle(const _Function& _Func) : _M_function(_Func)
    {
        m_pFunction = reinterpret_cast <TaskProc> (&::Concurrency::details::_UnrealizedChore::_InvokeBridge<task_handle>);
    }

    /// <summary>
    ///     Destroys the <c>task_handle</c> object.
    /// </summary>
    /**/
    ~task_handle()
    {
        //
        // We only need to perform a liveness check if the client owns the lifetime of the handle.  Doing this for runtime owned handles
        // is not only unnecessary -- it is also dangerous.
        //
        if (_OwningCollection() != NULL && !_GetRuntimeOwnsLifetime())
        {
            _CheckTaskCollection();
        }
    }

    /// <summary>
    ///     The function call operator that the runtime invokes to perform the work of the task handle.
    /// </summary>
    /**/
    void operator()() const
    {
        _M_function();
    }

private:

    friend class task_group;
    friend class structured_task_group;

    // The function object invoked to perform the body of the task.
    _Function _M_function;

    task_handle const & operator=(task_handle const&);    // no assignment operator

};

/// <summary>
///     A factory method for creating a <c>task_handle</c> object.
/// </summary>
/// <typeparam name="_Function">
///     The type of the function object that will be invoked to execute the work represented by the <c>task_handle</c> object.
/// </typeparam>
/// <param name="_Func">
///     The function that will be invoked to execute the work represented by the <c>task_handle</c> object.  This may be a lambda functor,
///     a pointer to a function, or any object that supports a version of the function call operator with the signature <c>void operator()()</c>.
/// </param>
/// <returns>
///     A <c>task_handle</c> object.
/// </returns>
/// <remarks>
///     This function is useful when you need to create a <c>task_handle</c> object with a lambda expression, because it allows you to
///     create the object without knowing the true type of the lambda functor.
/// </remarks>
/// <seealso cref="task_handle Class"/>
/// <seealso cref="task_group Class"/>
/// <seealso cref="structured_task_group Class"/>
/**/
template <class _Function>
task_handle<_Function> make_task(const _Function& _Func)
{
    return task_handle<_Function>(_Func);
}

/// <summary>
///     Describes the execution status of a <c>task_group</c> or <c>structured_task_group</c> object.  A value of this type is returned
///     by numerous methods that wait on tasks scheduled to a task group to complete.
/// </summary>
/// <seealso cref="task_group Class"/>
/// <seealso cref="task_group::wait Method"/>
/// <seealso cref="task_group::run_and_wait Method"/>
/// <seealso cref="structured_task_group Class"/>
/// <seealso cref="structured_task_group::wait Method"/>
/// <seealso cref="structured_task_group::run_and_wait Method"/>
/**/
enum task_group_status
{
    /// <summary>
    ///     The tasks queued to the <c>task_group</c> object have not completed.  Note that this value is not presently returned by
    ///     the Concurrency Runtime.
    /// </summary>
    /**/
    not_complete,

    /// <summary>
    ///     The tasks queued to the <c>task_group</c> or <c>structured_task_group</c> object completed successfully.
    /// </summary>
    /**/
    completed,

    /// <summary>
    ///     The <c>task_group</c> or <c>structured_task_group</c> object was canceled.  One or more tasks may not have executed.
    /// </summary>
    /**/
    canceled
};

/// <summary>
///     The <c>structured_task_group</c> class represents a highly structured collection of parallel work. You can queue individual parallel tasks to
///     a <c>structured_task_group</c> using <c>task_handle</c> objects, and wait for them to complete, or cancel the task group before they have finished
///     executing, which will abort any tasks that have not begun execution.
/// </summary>
/// <remarks>
///     There are a number of severe restrictions placed on usage of a <c>structured_task_group</c> object in order to gain performance:
///     <list type="bullet">
///     <item>
///     <description>A single <c>structured_task_group</c> object cannot be used by multiple threads.  All operations on a <c>structured_task_group</c> object
///     must be performed by the thread that created the object.  The two exceptions to this rule are the member functions <c>cancel</c> and
///     <c>is_canceling</c>.  The object may not be in the capture list of a lambda expression and be used within a task, unless the task is using one
///     of the cancellation operations.</description>
///     </item>
///     <item>
///     <description>All tasks scheduled to a <c>structured_task_group</c> object are scheduled through the use of <c>task_handle</c> objects which
///     you must explicitly manage the lifetime of.</description>
///     </item>
///     <item>
///     <description>Multiple groups may only be used in absolutely nested order.  If two <c>structured_task_group</c> objects are declared, the second
///     one being declared (the inner one) must destruct before any method except <c>cancel</c> or <c>is_canceling</c> is called on the first one
///     (the outer one).  This condition holds true in both the case of simply declaring multiple <c>structured_task_group</c> objects within the same
///     or functionally nested scopes as well as the case of a task that was queued to the <c>structured_task_group</c> via the <c>run</c> or
///     <c>run_and_wait</c> methods.</description>
///     </item>
///     <item>
///     <description>Unlike the general <c>task_group</c> class, all states in the <c>structured_task_group</c> class are final.  Once you have queued tasks to the
///     group and waited for them to complete, you may not use the same group again.</description>
///     </item>
///     </list>
///     <para>For more information, see <see cref="Task Parallelism"/>.</para>
/// </remarks>
/// <seealso cref="task_group Class"/>
/// <seealso cref="task_handle Class"/>
/**/
class structured_task_group
{
public:

    /// <summary>
    ///     Constructs a new <c>structured_task_group</c> object.
    /// </summary>
    /// <seealso cref="Task Parallelism"/>
    /**/
    structured_task_group()
    {
    }

    /// <summary>
    ///     Destroys a <c>structured_task_group</c> object.  You are expected to call either the <c>wait</c> or <c>run_and_wait</c> method on the
    ///     object prior to the destructor executing, unless the destructor is executing as a result of stack unwinding due to an exception.
    /// </summary>
    /// <remarks>
    ///     If the destructor runs as the result of normal execution (e.g.: not stack unwinding due to an exception) and neither the <c>wait</c> nor
    ///     <c>run_and_wait</c> methods have been called, the destructor may throw a <see cref="missing_wait Class">missing_wait</see> exception.
    /// </remarks>
    /// <seealso cref="structured_task_group::wait Method"/>
    /// <seealso cref="structured_task_group::run_and_wait Method"/>
    /**/
    ~structured_task_group()
    {
    }

    /// <summary>
    ///     Schedules a task on the <c>structured_task_group</c> object.  The caller manages the lifetime of the <c>task_handle</c> object passed
    ///     in the <paramref name="_Task_handle"/> parameter.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the body of the task handle.
    /// </typeparam>
    /// <param name="_Task_handle">
    ///     A handle to the work being scheduled.  Note that the caller has responsibility for the lifetime of this object.  The runtime will
    ///     continue to expect it to live until either the <c>wait</c> or <c>run_and_wait</c> method has been called on this
    ///     <c>structured_task_group</c> object.
    /// </param>
    /// <remarks>
    ///     If the <c>structured_task_group</c> destructs as the result of stack unwinding from an exception, you do not need to guarantee
    ///     that a call has been made to either the <c>wait</c> or <c>run_and_wait</c> method.  In this case, the destructor will appropriately
    ///     cancel and wait for the task represented by the <paramref name="_Task_handle"/> parameter to complete.
    ///     <para>Throws an <see cref="invalid_multiple_scheduling Class">invalid_multiple_scheduling</see> exception if the task handle given by
    ///     the <paramref name="_Task_handle"/> parameter has already been scheduled onto a task group object via the <c>run</c> method and there has been
    ///     no intervening call to either the <c>wait</c> or <c>run_and_wait</c> method on that task group.</para>
    /// </remarks>
    /// <seealso cref="structured_task_group::wait Method"/>
    /// <seealso cref="structured_task_group::run_and_wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<class _Function>
    void run(task_handle<_Function>& _Task_handle)
    {
        _Task_handle._SetRuntimeOwnsLifetime(false);
        _M_task_collection._Schedule(&_Task_handle);
    }

    /// <summary>
    ///     Waits until all work on the <c>structured_task_group</c> has completed or is canceled.
    /// </summary>
    /// <returns>
    ///     An indication of whether the wait was satisfied or the task group was canceled, due to either an explicit cancel operation or an exception
    ///     being thrown from one of its tasks. For more information, see <see cref="task_group_status Enumeration">task_group_status</see>
    /// </returns>
    /// <remarks>
    ///     Note that one or more of the tasks scheduled to this <c>structured_task_group</c> object may execute inline on the calling context.
    ///     <para>If one or more of the tasks scheduled to this <c>structured_task_group</c> object throws an exception, the
    ///     runtime will select one such exception of its choosing and propagate it out of the call to the <c>wait</c> method.</para>
    ///     <para>Once this function returns, the <c>structured_task_group</c> object is considered in a final state and should not be used.  Note that
    ///     utilization after the <c>wait</c> method returns will result in undefined behavior.</para>
    ///     <para>In the non-exceptional path of execution, you have a mandate to call either this method or the <c>run_and_wait</c> method before
    ///     the destructor of the <c>structured_task_group</c> executes.</para>
    /// </remarks>
    /// <seealso cref="structured_task_group::wait Method"/>
    /// <seealso cref="structured_task_group::run_and_wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    task_group_status wait()
    {
        //
        // The underlying scheduler's definitions map exactly to the PPL's.  No translation beyond the cast is necessary.
        //
        return (task_group_status)_M_task_collection._Wait();
    }

    /// <summary>
    ///     Schedules a task to be run inline on the calling context with the assistance of the <c>structured_task_group</c> object for full
    ///     cancellation support.  If a <c>task_handle</c> object is passed as a parameter to <c>run_and_wait</c>, the caller is
    ///     responsible for managing the lifetime of the <c>task_handle</c> object.  The function then waits until all work on the
    ///     <c>structured_task_group</c> object has either completed or been canceled.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the body of the task handle.
    /// </typeparam>
    /// <param name="_Task_handle">
    ///     A handle to the task which will be run inline on the calling context.  Note that the caller has responsibility for the lifetime of this object.
    ///     The runtime will continue to expect it to live until the <c>run_and_wait</c> method finishes execution.
    /// </param>
    /// <returns>
    ///     An indication of whether the wait was satisfied or the task group was canceled, due to either an explicit cancel operation or an exception
    ///     being thrown from one of its tasks. For more information, see <see cref="task_group_status Enumeration">task_group_status</see>
    /// </returns>
    /// <remarks>
    ///     Note that one or more of the tasks scheduled to this <c>structured_task_group</c> object may execute inline on the calling context.
    ///     <para>If one or more of the tasks scheduled to this <c>structured_task_group</c> object throws an exception, the
    ///     runtime will select one such exception of its choosing and propagate it out of the call to the <c>run_and_wait</c> method.</para>
    ///     <para>Once this function returns, the <c>structured_task_group</c> object is considered in a final state and should not be used.
    ///     Note that utilization after the <c>run_and_wait</c> method returns will result in undefined behavior.</para>
    ///     <para>In the non-exceptional path of execution, you have a mandate to call either this method or the <c>wait</c> method before
    ///     the destructor of the <c>structured_task_group</c> executes.</para>
    /// </remarks>
    /// <seealso cref="structured_task_group::run Method"/>
    /// <seealso cref="structured_task_group::wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<class _Function>
    task_group_status run_and_wait(task_handle<_Function>& _Task_handle)
    {
        //
        // The underlying scheduler's definitions map exactly to the PPL's.  No translation beyond the cast is necessary.
        //
        return (task_group_status)_M_task_collection._RunAndWait(&_Task_handle);
    }

    /// <summary>
    ///     Schedules a task to be run inline on the calling context with the assistance of the <c>structured_task_group</c> object for full
    ///     cancellation support.  If a <c>task_handle</c> object is passed as a parameter to <c>run_and_wait</c>, the caller is
    ///     responsible for managing the lifetime of the <c>task_handle</c> object.  The function then waits until all work on the
    ///     <c>structured_task_group</c> object has either completed or been canceled.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the task.
    /// </typeparam>
    /// <param name="_Func">
    ///     A function which will be called to invoke the body of the work.  This may be a lambda or other object which supports
    ///     a version of the function call operator with the signature <c>void operator()()</c>.
    /// </param>
    /// <returns>
    ///     An indication of whether the wait was satisfied or the task group was canceled, due to either an explicit cancel operation or an exception
    ///     being thrown from one of its tasks. For more information, see <see cref="task_group_status Enumeration">task_group_status</see>
    /// </returns>
    /// <remarks>
    ///     Note that one or more of the tasks scheduled to this <c>structured_task_group</c> object may execute inline on the calling context.
    ///     <para>If one or more of the tasks scheduled to this <c>structured_task_group</c> object throws an exception, the
    ///     runtime will select one such exception of its choosing and propagate it out of the call to the <c>run_and_wait</c> method.</para>
    ///     <para>Once this function returns, the <c>structured_task_group</c> object is considered in a final state and should not be used.
    ///     Note that utilization after the <c>run_and_wait</c> method returns will result in undefined behavior.</para>
    ///     <para>In the non-exceptional path of execution, you have a mandate to call either this method or the <c>wait</c> method before
    ///     the destructor of the <c>structured_task_group</c> executes.</para>
    /// </remarks>
    /// <seealso cref="structured_task_group::run Method"/>
    /// <seealso cref="structured_task_group::wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<class _Function>
    task_group_status run_and_wait(const _Function& _Func)
    {
        //
        // The underlying scheduler's definitions map exactly to the PPL's.  No translation beyond the cast is necessary.
        //
        task_handle<_Function> _Task(_Func);
        return (task_group_status)_M_task_collection._RunAndWait(&_Task);
    }

    /// <summary>
    ///     Makes a best effort attempt to cancel the sub-tree of work rooted at this task group.  Every task scheduled on the task group
    ///     will get canceled transitively if possible.
    /// </summary>
    /// <remarks>
    ///     For more information, see <see cref="Cancellation in the PPL"/>.
    /// </remarks>
    /**/
    void cancel()
    {
        _M_task_collection._Cancel();
    }

    /// <summary>
    ///     Informs the caller whether or not the task group is currently in the midst of a cancellation.  This
    ///     does not necessarily indicate that the <c>cancel</c> method was called on the <c>structured_task_group</c> object
    ///     (although such certainly qualifies this method to return <c>true</c>).  It may be the case that the <c>structured_task_group</c> object
    ///     is executing inline and a task group further up in the work tree was canceled.  In cases such as these where the runtime can determine ahead
    ///     of time that cancellation will flow through this <c>structured_task_group</c> object, <c>true</c> will be returned as well.
    /// </summary>
    /// <returns>
    ///     An indication of whether the <c>structured_task_group</c> object is in the midst of a cancellation (or is guaranteed to be shortly).
    /// </returns>
    /// <remarks>
    ///     For more information, see <see cref="Cancellation in the PPL"/>.
    /// </remarks>
    /**/
    bool is_canceling()
    {
        return _M_task_collection._IsCanceling();
    }

private:

    // Disallow passing in an r-value for a task handle argument
    template<class _Function> void run(task_handle<_Function>&& _Task_handle);

    // The underlying group of tasks as known to the runtime.
    ::Concurrency::details::_StructuredTaskCollection _M_task_collection;
};

/// <summary>
///     The <c>task_group</c> class represents a collection of parallel work which can be waited on or canceled.
/// </summary>
/// <remarks>
///     Unlike the heavily restricted <c>structured_task_group</c> class, the <c>task_group</c> class is much more general construct.
///     It does not have any of the restrictions described by <see cref="structured_task_group Class">structured_task_group</see>.  <c>task_group</c>
///     objects may safely be used across threads and utilized in free-form ways.  The disadvantage of the <c>task_group</c> construct is that
///     it may not perform as well as the <c>structured_task_group</c> construct for tasks which perform small amounts of work.
///     <para>For more information, see <see cref="Task Parallelism"/>.</para>
/// </remarks>
/// <seealso cref="structured_task_group Class"/>
/// <seealso cref="task_handle Class"/>
/**/
class task_group
{
public:

    /// <summary>
    ///     Constructs a new <c>task_group</c> object.
    /// </summary>
    /// <seealso cref="Task Parallelism"/>
    /**/
    task_group()
    {
    }

    /// <summary>
    ///     Destroys a <c>task_group</c> object.  You are expected to call the either the <c>wait</c> or <c>run_and_wait</c> method on the object
    ///     prior to the destructor executing, unless the destructor is executing as the result of stack unwinding due to an exception.
    /// </summary>
    /// <remarks>
    ///     If the destructor runs as the result of normal execution (e.g.: not stack unwinding due to an exception) and neither the <c>wait</c> nor
    ///     <c>run_and_wait</c> methods have been called, the destructor may throw a <see cref="missing_wait Class">missing_wait</see> exception.
    /// </remarks>
    /// <seealso cref="task_group::wait Method"/>
    /// <seealso cref="task_group::run_and_wait Method"/>
    /**/
    ~task_group()
    {
    }

    /// <summary>
    ///     Schedules a task on the <c>task_group</c> object. If a <c>task_handle</c> object is passed as a parameter to <c>run</c>, the caller is
    ///     responsible for managing the lifetime of the <c>task_handle</c> object. The version of the method that takes a reference to a function
    ///     object as a parameter involves heap allocation inside the runtime which may be perform less well than using the version that takes a
    ///     reference to a <c>task_handle</c> object.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the body of the task handle.
    /// </typeparam>
    /// <param name="_Func">
    ///     A function which will be called to invoke the body of the task.  This may be a lambda expression or other object which supports
    ///     a version of the function call operator with the signature <c>void operator()()</c>.
    /// </param>
    /// <remarks>
    ///     If the <c>task_group</c> destructs as the result of stack unwinding from an exception, you do not need to guarantee
    ///     that a call has been made to either the <c>wait</c> or <c>run_and_wait</c> method.  In this case, the destructor will appropriately
    ///     cancel and wait for the task represented by the <paramref name="_Task_handle"/> parameter to complete.
    ///     <para>The method throws an <see cref="invalid_multiple_scheduling Class">invalid_multiple_scheduling</see> exception if the task
    ///     handle given by the <paramref name="_Task_handle"/> parameter has already been scheduled onto a task group object via the <c>run</c>
    ///     method and there has been no intervening call to either the <c>wait</c> or <c>run_and_wait</c> method on that task group.</para>
    /// </remarks>
    /// <seealso cref="task_group::wait Method"/>
    /// <seealso cref="task_group::run_and_wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<typename _Function>
    void run(const _Function& _Func)
    {
        _M_task_collection._Schedule(::Concurrency::details::_UnrealizedChore::_InternalAlloc<task_handle<_Function>, _Function>(_Func));
    }

    /// <summary>
    ///     Schedules a task on the <c>task_group</c> object. If a <c>task_handle</c> object is passed as a parameter to <c>run</c>, the caller is
    ///     responsible for managing the lifetime of the <c>task_handle</c> object. The version of the method that takes a reference to a function
    ///     object as a parameter involves heap allocation inside the runtime which may be perform less well than using the version that takes a
    ///     reference to a <c>task_handle</c> object.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the body of the task handle.
    /// </typeparam>
    /// <param name="_Task_handle">
    ///     A handle to the work being scheduled.  Note that the caller has responsibility for the lifetime of this object.  The runtime will
    ///     continue to expect it to live until either the <c>wait</c> or <c>run_and_wait</c> method has been called on this
    ///     <c>task_group</c> object.
    /// </param>
    /// <remarks>
    ///     If the <c>task_group</c> destructs as the result of stack unwinding from an exception, you do not need to guarantee
    ///     that a call has been made to either the <c>wait</c> or <c>run_and_wait</c> method.  In this case, the destructor will appropriately
    ///     cancel and wait for the task represented by the <paramref name="_Task_handle"/> parameter to complete.
    ///     <para>The method throws an <see cref="invalid_multiple_scheduling Class">invalid_multiple_scheduling</see> exception if the task
    ///     handle given by the <paramref name="_Task_handle"/> parameter has already been scheduled onto a task group object via the <c>run</c>
    ///     method and there has been no intervening call to either the <c>wait</c> or <c>run_and_wait</c> method on that task group.</para>
    /// </remarks>
    /// <seealso cref="task_group::wait Method"/>
    /// <seealso cref="task_group::run_and_wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<typename _Function>
    void run(task_handle<_Function>& _Task_handle)
    {
        _Task_handle._SetRuntimeOwnsLifetime(false);
        _M_task_collection._Schedule(&_Task_handle);
    }

    /// <summary>
    ///     Waits until all work on the <c>task_group</c> object has either completed or been canceled.
    /// </summary>
    /// <returns>
    ///     An indication of whether the wait was satisfied or the task group was canceled, due to either an explicit cancel operation or an exception
    ///     being thrown from one of its tasks. For more information, see <see cref="task_group_status Enumeration">task_group_status</see>.
    /// </returns>
    /// <remarks>
    ///     Note that one or more of the tasks scheduled to this <c>task_group</c> object may execute inline on the calling context.
    ///     <para>If one or more of the tasks scheduled to this <c>task_group</c> object throws an exception, the
    ///     runtime will select one such exception of its choosing and propagate it out of the call to the <c>wait</c> method.</para>
    ///     <para>Calling <c>wait</c> on a <c>task_group</c> object resets it to a clean state where it can be reused. This includes the case
    ///     where the <c>task_group</c> object was canceled.</para>
    ///     <para>In the non-exceptional path of execution, you have a mandate to call either this method or the <c>run_and_wait</c> method before
    ///     the destructor of the <c>task_group</c> executes.</para>
    /// </remarks>
    /**/
    task_group_status wait()
    {
        //
        // The underlying scheduler's definitions map exactly to the PPL's.  No translation beyond the cast is necessary.
        //
        return static_cast<task_group_status>(_M_task_collection._Wait());
    }

    /// <summary>
    ///     Schedules a task to be run inline on the calling context with the assistance of the <c>task_group</c> object for full cancellation support.
    ///     The function then waits until all work on the <c>task_group</c> object has either completed or been canceled.  If a <c>task_handle</c> object
    ///     is passed as a parameter to <c>run_and_wait</c>, the caller is responsible for managing the lifetime of the <c>task_handle</c> object.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the body of the task handle.
    /// </typeparam>
    /// <param name="_Task_handle">
    ///     A handle to the task which will be run inline on the calling context.  Note that the caller has responsibility for the lifetime of this object.
    ///     The runtime will continue to expect it to live until the <c>run_and_wait</c> method finishes execution.
    /// </param>
    /// <returns>
    ///     An indication of whether the wait was satisfied or the task group was canceled, due to either an explicit cancel operation or an exception
    ///     being thrown from one of its tasks. For more information, see <see cref="task_group_status Enumeration">task_group_status</see>.
    /// </returns>
    /// <remarks>
    ///     Note that one or more of the tasks scheduled to this <c>task_group</c> object may execute inline on the calling context.
    ///     <para>If one or more of the tasks scheduled to this <c>task_group</c> object throws an exception, the
    ///     runtime will select one such exception of its choosing and propagate it out of the call to the <c>run_and_wait</c> method.</para>
    ///     <para>Upon return from the <c>run_and_wait</c> method on a <c>task_group</c> object, the runtime resets the object to a clean state where it can be
    ///     reused. This includes the case where the <c>task_group</c> object was canceled.</para>
    ///     <para>In the non-exceptional path of execution, you have a mandate to call either this method or the <c>wait</c> method before
    ///     the destructor of the <c>task_group</c> executes.</para>
    /// </remarks>
    /// <seealso cref="task_group::run Method"/>
    /// <seealso cref="task_group::wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<class _Function>
    task_group_status run_and_wait(task_handle<_Function>& _Task_handle)
    {
        //
        // The underlying scheduler's definitions map exactly to the PPL's.  No translation beyond the cast is necessary.
        //
        _Task_handle._SetRuntimeOwnsLifetime(false);
        return (task_group_status)_M_task_collection._RunAndWait(&_Task_handle);
    }

    /// <summary>
    ///     Schedules a task to be run inline on the calling context with the assistance of the <c>task_group</c> object for full cancellation support.
    ///     The function then waits until all work on the <c>task_group</c> object has either completed or been canceled.  If a <c>task_handle</c> object
    ///     is passed as a parameter to <c>run_and_wait</c>, the caller is responsible for managing the lifetime of the <c>task_handle</c> object.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to execute the body of the task.
    /// </typeparam>
    /// <param name="_Func">
    ///     A function which will be called to invoke the body of the work.  This may be a lambda expression or other object which supports
    ///     a version of the function call operator with the signature <c>void operator()()</c>.
    /// </param>
    /// <returns>
    ///     An indication of whether the wait was satisfied or the task group was canceled, due to either an explicit cancel operation or an exception
    ///     being thrown from one of its tasks. For more information, see <see cref="task_group_status Enumeration">task_group_status</see>.
    /// </returns>
    /// <remarks>
    ///     Note that one or more of the tasks scheduled to this <c>task_group</c> object may execute inline on the calling context.
    ///     <para>If one or more of the tasks scheduled to this <c>task_group</c> object throws an exception, the
    ///     runtime will select one such exception of its choosing and propagate it out of the call to the <c>run_and_wait</c> method.</para>
    ///     <para>Upon return from the <c>run_and_wait</c> method on a <c>task_group</c> object, the runtime resets the object to a clean state where it can be
    ///     reused. This includes the case where the <c>task_group</c> object was canceled.</para>
    ///     <para>In the non-exceptional path of execution, you have a mandate to call either this method or the <c>wait</c> method before
    ///     the destructor of the <c>task_group</c> executes.</para>
    /// </remarks>
    /// <seealso cref="task_group::run Method"/>
    /// <seealso cref="task_group::wait Method"/>
    /// <seealso cref="Task Parallelism"/>
    /**/
    template<class _Function>
    task_group_status run_and_wait(const _Function& _Func)
    {
        //
        // The underlying scheduler's definitions map exactly to the PPL's.  No translation beyond the cast is necessary.
        //
        return (task_group_status)_M_task_collection._RunAndWait(::Concurrency::details::_UnrealizedChore::_InternalAlloc<task_handle<_Function>, _Function>(_Func));
    }

    /// <summary>
    ///     Makes a best effort attempt to cancel the sub-tree of work rooted at this task group.  Every task scheduled on the task group
    ///     will get canceled transitively if possible.
    /// </summary>
    /// <remarks>
    ///     For more information, see <see cref="Cancellation in the PPL"/>.
    /// </remarks>
    /**/
    void cancel()
    {
        _M_task_collection._Cancel();
    }

    /// <summary>
    ///     Informs the caller whether or not the task group is currently in the midst of a cancellation.  This
    ///     does not necessarily indicate that the <c>cancel</c> method was called on the <c>task_group</c> object
    ///     (although such certainly qualifies this method to return <c>true</c>).  It may be the case that the <c>task_group</c> object
    ///     is executing inline and a task group further up in the work tree was canceled.  In cases such as these where the runtime can determine ahead
    ///     of time that cancellation will flow through this <c>task_group</c> object, <c>true</c> will be returned as well.
    /// </summary>
    /// <returns>
    ///     An indication of whether the <c>task_group</c> object is in the midst of a cancellation (or is guaranteed to be shortly).
    /// </returns>
    /// <remarks>
    ///     For more information, see <see cref="Cancellation in the PPL"/>.
    /// </remarks>
    /**/
    bool is_canceling()
    {
        return _M_task_collection._IsCanceling();
    }

private:

    // Disallow passing in an r-value for a task handle argument
    template<class _Function> void run(task_handle<_Function>&& _Task_handle);

    // The underlying group of tasks as known to the runtime.
    ::Concurrency::details::_TaskCollection _M_task_collection;
};


/// <summary>
///     Returns an indication of whether the task group which is currently executing inline on the current context
///     is in the midst of an active cancellation (or will be shortly).  Note that if there is no task group currently
///     executing inline on the current context, <c>false</c> will be returned.
/// </summary>
/// <returns>
///     <c>true</c> if the task group which is currently executing is canceling, <c>false</c> otherwise.
/// </returns>
/// <remarks>
///     For more information, see <see cref="Cancellation in the PPL"/>.
/// </remarks>
/// <seealso cref="task_group Class"/>
/// <seealso cref="structured_task_group Class"/>
/**/
_CRTIMP2 bool __cdecl is_current_task_group_canceling();

// Parallel Algorithms and Patterns

// Helper function that implements parallel_invoke with two functions
// Used by parallel_for and parallel_foreach implementations

template <typename _Function1, typename _Function2>
void _Parallel_invoke_impl(const _Function1& _Func1, const _Function2& _Func2)
{
    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    // We inline the last item to prevent the unnecessary push/pop on the work queue.
    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run_and_wait(_Task_handle2);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    _Parallel_invoke_impl(_Func1, _Func2);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run_and_wait(_Task_handle3);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run_and_wait(_Task_handle4);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function5">
///     The type of the fifth function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <param name="_Func5">
///     The fifth function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4, typename _Function5>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4, const _Function5& _Func5)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run(_Task_handle4);

    task_handle<_Function5> _Task_handle5(_Func5);
    _Task_group.run_and_wait(_Task_handle5);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function5">
///     The type of the fifth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function6">
///     The type of the sixth function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <param name="_Func5">
///     The fifth function object to be executed in parallel.
/// </param>
/// <param name="_Func6">
///     The sixth function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4, typename _Function5,
    typename _Function6>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4, const _Function5& _Func5,
    const _Function6& _Func6)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run(_Task_handle4);

    task_handle<_Function5> _Task_handle5(_Func5);
    _Task_group.run(_Task_handle5);

    task_handle<_Function6> _Task_handle6(_Func6);
    _Task_group.run_and_wait(_Task_handle6);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function5">
///     The type of the fifth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function6">
///     The type of the sixth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function7">
///     The type of the seventh function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <param name="_Func5">
///     The fifth function object to be executed in parallel.
/// </param>
/// <param name="_Func6">
///     The sixth function object to be executed in parallel.
/// </param>
/// <param name="_Func7">
///     The seventh function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4, typename _Function5,
    typename _Function6, typename _Function7>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4, const _Function5& _Func5,
    const _Function6& _Func6, const _Function7& _Func7)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run(_Task_handle4);

    task_handle<_Function5> _Task_handle5(_Func5);
    _Task_group.run(_Task_handle5);

    task_handle<_Function6> _Task_handle6(_Func6);
    _Task_group.run(_Task_handle6);

    task_handle<_Function7> _Task_handle7(_Func7);
    _Task_group.run_and_wait(_Task_handle7);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function5">
///     The type of the fifth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function6">
///     The type of the sixth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function7">
///     The type of the seventh function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function8">
///     The type of the eighth function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <param name="_Func5">
///     The fifth function object to be executed in parallel.
/// </param>
/// <param name="_Func6">
///     The sixth function object to be executed in parallel.
/// </param>
/// <param name="_Func7">
///     The seventh function object to be executed in parallel.
/// </param>
/// <param name="_Func8">
///     The eighth function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4, typename _Function5,
    typename _Function6, typename _Function7, typename _Function8>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4, const _Function5& _Func5,
    const _Function6& _Func6, const _Function7& _Func7, const _Function8& _Func8)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run(_Task_handle4);

    task_handle<_Function5> _Task_handle5(_Func5);
    _Task_group.run(_Task_handle5);

    task_handle<_Function6> _Task_handle6(_Func6);
    _Task_group.run(_Task_handle6);

    task_handle<_Function7> _Task_handle7(_Func7);
    _Task_group.run(_Task_handle7);

    task_handle<_Function8> _Task_handle8(_Func8);
    _Task_group.run_and_wait(_Task_handle8);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function5">
///     The type of the fifth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function6">
///     The type of the sixth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function7">
///     The type of the seventh function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function8">
///     The type of the eighth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function9">
///     The type of the ninth function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <param name="_Func5">
///     The fifth function object to be executed in parallel.
/// </param>
/// <param name="_Func6">
///     The sixth function object to be executed in parallel.
/// </param>
/// <param name="_Func7">
///     The seventh function object to be executed in parallel.
/// </param>
/// <param name="_Func8">
///     The eighth function object to be executed in parallel.
/// </param>
/// <param name="_Func9">
///     The ninth function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4, typename _Function5,
    typename _Function6, typename _Function7, typename _Function8, typename _Function9>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4, const _Function5& _Func5,
    const _Function6& _Func6, const _Function7& _Func7, const _Function8& _Func8, const _Function9& _Func9)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run(_Task_handle4);

    task_handle<_Function5> _Task_handle5(_Func5);
    _Task_group.run(_Task_handle5);

    task_handle<_Function6> _Task_handle6(_Func6);
    _Task_group.run(_Task_handle6);

    task_handle<_Function7> _Task_handle7(_Func7);
    _Task_group.run(_Task_handle7);

    task_handle<_Function8> _Task_handle8(_Func8);
    _Task_group.run(_Task_handle8);

    task_handle<_Function9> _Task_handle9(_Func9);
    _Task_group.run_and_wait(_Task_handle9);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     Executes the function objects supplied as parameters in parallel, and blocks until they have finished executing.  Each function object
///     could be a lambda expression, a pointer to function, or any object that supports the function call operator with the signature
///     <c>void operator()()</c>.
/// </summary>
/// <typeparam name="_Function1">
///     The type of the first function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function2">
///     The type of the second function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function3">
///     The type of the third function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function4">
///     The type of the fourth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function5">
///     The type of the fifth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function6">
///     The type of the sixth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function7">
///     The type of the seventh function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function8">
///     The type of the eighth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function9">
///     The type of the ninth function object to be executed in parallel.
/// </typeparam>
/// <typeparam name="_Function10">
///     The type of the tenth function object to be executed in parallel.
/// </typeparam>
/// <param name="_Func1">
///     The first function object to be executed in parallel.
/// </param>
/// <param name="_Func2">
///     The second function object to be executed in parallel.
/// </param>
/// <param name="_Func3">
///     The third function object to be executed in parallel.
/// </param>
/// <param name="_Func4">
///     The fourth function object to be executed in parallel.
/// </param>
/// <param name="_Func5">
///     The fifth function object to be executed in parallel.
/// </param>
/// <param name="_Func6">
///     The sixth function object to be executed in parallel.
/// </param>
/// <param name="_Func7">
///     The seventh function object to be executed in parallel.
/// </param>
/// <param name="_Func8">
///     The eighth function object to be executed in parallel.
/// </param>
/// <param name="_Func9">
///     The ninth function object to be executed in parallel.
/// </param>
/// <param name="_Func10">
///     The tenth function object to be executed in parallel.
/// </param>
/// <remarks>
///     Note that one or more of the function objects supplied as parameters may execute inline on the calling context.
///     <para>If one or more of the function objects passed as parameters to this function throws an exception, the
///     runtime will select one such exception of its choosing and propagate it out of the call to <c>parallel_invoke</c>.</para>
///     <para>For more information, see <see cref="Parallel Algorithms"/>.</para>
/// </remarks>
/**/
template <typename _Function1, typename _Function2, typename _Function3, typename _Function4, typename _Function5,
    typename _Function6, typename _Function7, typename _Function8, typename _Function9, typename _Function10>
void parallel_invoke(const _Function1& _Func1, const _Function2& _Func2, const _Function3& _Func3, const _Function4& _Func4, const _Function5& _Func5,
    const _Function6& _Func6, const _Function7& _Func7, const _Function8& _Func8, const _Function9& _Func9, const _Function10& _Func10)
{
    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);

    structured_task_group _Task_group;

    task_handle<_Function1> _Task_handle1(_Func1);
    _Task_group.run(_Task_handle1);

    task_handle<_Function2> _Task_handle2(_Func2);
    _Task_group.run(_Task_handle2);

    task_handle<_Function3> _Task_handle3(_Func3);
    _Task_group.run(_Task_handle3);

    task_handle<_Function4> _Task_handle4(_Func4);
    _Task_group.run(_Task_handle4);

    task_handle<_Function5> _Task_handle5(_Func5);
    _Task_group.run(_Task_handle5);

    task_handle<_Function6> _Task_handle6(_Func6);
    _Task_group.run(_Task_handle6);

    task_handle<_Function7> _Task_handle7(_Func7);
    _Task_group.run(_Task_handle7);

    task_handle<_Function8> _Task_handle8(_Func8);
    _Task_group.run(_Task_handle8);

    task_handle<_Function9> _Task_handle9(_Func9);
    _Task_group.run(_Task_handle9);

    task_handle<_Function10> _Task_handle10(_Func10);
    _Task_group.run_and_wait(_Task_handle10);

    _Trace_ppl_function(PPLParallelInvokeEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

// Helper methods for scheduling and executing parallel tasks

// A helper method for determining how many parallel tasks to use based on
// the current number of available processors.

template <typename _Index_type>
_Index_type _Get_num_chunks()
{
    // Compute the best initial estimate on the number of workers, based on number
    // of available processors and the heuristic-based oversubscription factor.
    const DWORD _Oversubscribe_factor = 1;
    DWORD _Num_processors = CurrentScheduler::Get()->GetNumberOfVirtualProcessors();

    return _Index_type(_Num_processors * _Oversubscribe_factor);
}

// Disable C4180: qualifier applied to function type has no meaning; ignored
// Warning fires for passing Foo function pointer to parallel_for instead of &Foo.
#pragma warning(push)
#pragma warning(disable: 4180)

// Template class that invokes user function on a parallel_for_each

template <typename _Random_iterator, typename _Index_type, typename _Function, bool _Is_iterator>
class _Parallel_chunk_helper_invoke
{
public:
    static void __cdecl _Invoke(const _Random_iterator& _First, _Index_type& _Index, const _Function& _Func)
    {
        _Func(_First[_Index]);
    }
};

// Template specialized class that invokes user function on a parallel_for

template <typename _Random_iterator, typename _Index_type, typename _Function>
class _Parallel_chunk_helper_invoke<_Random_iterator, _Index_type, _Function, false>
{
public:
    static void __cdecl _Invoke(const _Random_iterator& _First, _Index_type& _Index, const _Function& _Func)
    {
        _Func(static_cast<_Random_iterator>(_First + _Index));
    }
};

// Represents a range of iteration

template<typename _Index_type>
class _Range
{
public:

    // Construct an object for the range [_Current_iteration, _Last_iteration)
    _Range(_Index_type _Current_iteration, _Index_type _Last_iteration) :
      _M_current(_Current_iteration), _M_last(_Last_iteration)
    {
        // On creation, the range shall have atleast 1 iteration.
        _ASSERTE(_Number_of_iterations() > 0);
    }

    // Send a portion of the range to the helper
    void _Send_range(_Range<_Index_type> * _Helper_range)
    {
        // If there are no iterations other than the current one left until finish, there is no help
        // needed. Set the pointer to a special value that helper will understand and continue
        // doing the work.
        _Index_type _Remaining_iterations = _Number_of_iterations();
        if (_Remaining_iterations > 1)
        {
            // Compute the two pieces of the work range: one for the worker and one for helper class.
            _M_last_iteration = _M_current_iteration + _Remaining_iterations / 2;

            // There needs to be atleast 1 iteration left since the current iteration cannot be sent.
            _ASSERTE(_Number_of_iterations() > 0);
        }

        // This is also a signal for the helper that a range has been sent to it.
        _Helper_range->_M_current_iteration = _M_last_iteration;
    }

    // Steal the entire range and give it to the helper
    void _Steal_range(_Range<_Index_type>  * _Helper_range)
    {
        // We allow stealing only from a range that has atleast 1 iteration
        _ASSERTE(_Number_of_iterations() > 0);

        _Index_type _Current_iter = _M_current_iteration;

        _Helper_range->_M_current_iteration = _Current_iter + 1;
        _Helper_range->_M_last_iteration = _M_last_iteration;

        _M_last_iteration = _Current_iter + 1;
    }

    // Returns the number of iterations in this range
    _Index_type _Number_of_iterations() const
    {
        return (_M_last_iteration - _M_current_iteration);
    }

    // Returns the current iteration in the range
    _Index_type _Get_current_iteration() const
    {
        return _M_current;
    }

    // Sets the current iteration in the range
    void _Set_current_iteration(const _Index_type _I)
    {
        _M_current = _I;
    }

    __declspec(property(get=_Get_current_iteration, put=_Set_current_iteration)) _Index_type _M_current_iteration;

    // Returns the last iteration in the range
    _Index_type _Get_last_iteration() const
    {
        return _M_last;
    }

    // Sets the last iteration in the range
    void _Set_last_iteration(const _Index_type _I)
    {
        _M_last = _I;
    }

    __declspec(property(get=_Get_last_iteration, put=_Set_last_iteration)) _Index_type _M_last_iteration;

private:

    // These members are volatile since they are updated by the helper
    // and used by the worker.
    volatile _Index_type _M_current;
    volatile _Index_type _M_last;
};

// A proxy for the worker responsible for maintaining communication with the helper

template<typename _Index_type>
class _Worker_proxy
{
public:
    _Worker_proxy() :
      _M_pHelper_range(NULL), _M_pWorker_range(NULL), _M_is_done(0), _M_stop_iterating(0)
    {
        _M_context = Context::CurrentContext();
    }

    ~_Worker_proxy()
    {
        // Make the check to avoid doing a fence in the non-exceptional cases
        if (_M_is_done == 0)
        {
            // On exception, we need to set _M_is_done to ensure that the
            // helper breaks out of its spin wait.
            _Set_done();
        }
    }

    // Obtain a range from the worker
    bool _Receive_range(_Range<_Index_type> * _Helper_range)
    {
        // If the worker already finished, then there is no work left for the helper
        if (_M_is_done)
        {
            return false;
        }

        // There are two special values for _M_current_iteration that are not valid: one is the
        // initial value of the working class which it will never share, and the other is
        // the last exclusive iteration of the working class, which has no work to be done.
        // We use the former value so that we can understand worker's response.
        _Index_type _Cached_first_iteration = _Helper_range->_M_current_iteration;

        // Following operation is not done via interlocked operation because it does not have to.
        // Helper lazily registers that it would like to help the worker, but it allows for some
        // time to elapse before that information has made it over to the worker. The idea
        // is not to disturb the worker if it is not necessary. It is possible to add interlocked
        // operation in the future if the time spent in the busy wait loop is too big.
        _ASSERTE(_M_pHelper_range == NULL);
        _M_pHelper_range = _Helper_range;

        ::Concurrency::details::_SpinWaitBackoffNone spinWait(Context::Yield);

        // If the worker is done, it will flush the store buffer and signal the helper by
        // changing _M_current_iteration in the helper's range.
        while ((_Helper_range->_M_current_iteration == _Cached_first_iteration) && !_M_is_done)
        {
            if ((_M_pWorker_range != NULL) && _M_context->IsSynchronouslyBlocked())
            {
                // Attempt to steal the entire range from the worker if it is synchronously blocked.

                // Make sure that worker makes no forward progress while helper is attempting to
                // steal its range. If worker does get unblocked, simply back off in the helper.
                // Note that there could be another helper running if a range has already been
                // sent to us.
                long _Stop_iterating = _InterlockedIncrement(&_M_stop_iterating);
                _ASSERTE(_Stop_iterating > 0);

                // We need to make a local copy as the pointer could be changed by the worker.
                _Range<_Index_type> * _Worker_range = _M_pWorker_range;

                // The order of comparison needs to be preserved. If the parent is blocked, then
                // it cannot send a range (since _M_stop_iterating is already set). If it sent a range
                // before being synchronously blocked, then we are no longer the helper. Refrain
                // from intrusively stealing the range.
                if ((_Worker_range != NULL) && _M_context->IsSynchronouslyBlocked()
                    && (_Helper_range->_M_current_iteration == _Cached_first_iteration))
                {
                    _ASSERTE(!_M_is_done);
                    _ASSERTE(_M_pHelper_range == _Helper_range);

                    _M_pHelper_range = NULL;
                    _Worker_range->_Steal_range(_Helper_range);

                    _ASSERTE(_Helper_range->_M_current_iteration != _Cached_first_iteration);
                }

                // At this point, worker is either:
                //
                // a) no longer blocked so range will come to the helper naturally, or
                // b) out of iterations because helper stole all of it
                _Stop_iterating = _InterlockedDecrement(&_M_stop_iterating);
                _ASSERTE(_Stop_iterating >= 0);
            }
            else
            {
                // If there is no work received in a full spin, then start yielding the context
                spinWait._SpinOnce();
            }
        }

        // If the initial iteration is the same as the original first iteration then the
        // worker class is sending the signal that it does not need any help.
        if (_Helper_range->_M_current_iteration == _Cached_first_iteration)
        {
            return false;
        }

        return (_Helper_range->_Number_of_iterations() > 0);
    }

    // Send a portion of our range and notify the helper.
    bool _Send_range(_Range<_Index_type> * _Worker_range)
    {
        // Worker range shall not be available for stealing at this time.
        _ASSERTE(_M_pWorker_range == NULL);

        // Helper shall be registered.
        _ASSERTE(_M_pHelper_range != NULL);

        // Send the range
        _Worker_range->_Send_range(_M_pHelper_range);

        // Notify the helper. The fence ensures that the prior updates are visible.
        _InterlockedExchangePointer(reinterpret_cast<void * volatile *>(&_M_pHelper_range), NULL);

        // The current iteration should still be left
        _ASSERTE(_Worker_range->_Number_of_iterations() >= 1);

        // Indicate if we need another helper
        return (_Worker_range->_Number_of_iterations() > 1);
    }

    // Let the helper know that it is ok to intrusively steal range from the worker by publishing the
    // remaining range.
    void _Enable_intrusive_steal(_Range<_Index_type> * _Worker_range)
    {
        _M_pWorker_range = _Worker_range;
    }

    // Prevent the helper from intrusively stealing range from the worker
    void _Disable_intrusive_steal()
    {
        _M_pWorker_range = NULL;
        _Wait_on_intrusive_steal();
    }

    bool _Is_helper_registered()
    {
        return (_M_pHelper_range != NULL);
    }

    void _Set_done()
    {
        // Let the helper know that this class is done with work and flush the store buffer. This operation
        // ensures that any buffered store to helper range in _Send_range is flushed and
        // available in _Receive_range (so there will be no lost ranges).
        _InterlockedExchange(&_M_is_done, 1);
    }
private:

    // Spin wait for any intrusive steal that is in progress.
    void _Wait_on_intrusive_steal()
    {
        // This code is used to synchronize with helper in case of worker cooperative blocking.
        if (_M_stop_iterating != 0)
        {
            ::Concurrency::details::_SpinWaitBackoffNone spinWait;

            while (_M_stop_iterating != 0)
            {
                spinWait._SpinOnce();
            }
        }
    }

    // Read in the loop
    _Range<_Index_type> * volatile  _M_pHelper_range;

    // Written rarely
    Context *                       _M_context;
    volatile long                   _M_is_done;

    // Written to in the loop
    _Range<_Index_type> * volatile  _M_pWorker_range;
    volatile long                   _M_stop_iterating;

    _Worker_proxy const & operator=(_Worker_proxy const&);    // no assignment operator
};

// parallel_for -- Performs parallel iteration over a range of indices from _First to _Last,
// excluding _Last. The order in which each iteration is executed is unspecified and non-deterministic.

// Closure (binding) classes for invoking parallel_for and parallel_for_each, with chunks

// A dynamically rebalancing closure class used for packaging parallel_for or parallel_for_each for invocation in chunks.
// If some tasks finish earlier than others, helper tasks get executed which ensures further distribution of work.

template <typename _Random_iterator, typename _Index_type, typename _Function, bool _Is_iterator>
class _Parallel_chunk_helper
{
public:
    _Parallel_chunk_helper(const _Random_iterator& _First, _Index_type _First_iteration, _Index_type _Last_iteration, const _Index_type& _Step,
        const _Function& _Func, _Worker_proxy<_Index_type> * const _Parent_data = NULL) :
        _M_first(_First), _M_first_iteration(_First_iteration), _M_last_iteration(_Last_iteration), _M_step(_Step), _M_function(_Func),
        _M_parent_worker(_Parent_data)
    {
        // Empty constructor since members are already assigned
    }

        // Constructor overload that accepts a range
    _Parallel_chunk_helper(const _Random_iterator& _First,  const _Index_type& _Step, const _Function& _Func,
        const _Range<_Index_type>& _Worker_range, _Worker_proxy<_Index_type> * const _Parent_data = NULL) :
        _M_first(_First), _M_first_iteration(_Worker_range._M_current_iteration), _M_last_iteration(_Worker_range._M_last_iteration), _M_step(_Step), _M_function(_Func),
        _M_parent_worker(_Parent_data)
    {
        // Empty constructor since members are already assigned
    }

    // The main helper function which iterates over the given collection and invokes user function on every iteration.
    // Function is marked as const even though it does mutate some of its members (those are declared as mutable). This is done
    // in order to easily communicate between a worker and a helper instance, without holding references to many local variables.
    // However, this function does not mutate any state that is visible to anyone outside of this class, nor would that be
    // possible due to the implicit copy of the functor that happens when a new task_handle is created.
    __declspec(safebuffers) void operator()() const
    {
        _Range<_Index_type> _Worker_range(_M_first_iteration, _M_last_iteration);

        // This class has two modes: worker and helper. The originally split chunk is always a
        // worker, while any subsequent class spawned from this class is in the helper
        // mode, which is signified using a link to the worker class through _M_pOwning_worker
        // handle. So, it will wait for work to be dished out by the working class while in helper mode.
        if (_M_parent_worker != NULL && !_M_parent_worker->_Receive_range(&_Worker_range))
        {
            // If the worker class rejected the help, simply return
            return;
        }

        // Keep the secondary, scaled, loop index for quick indexing into the data structure
        _Index_type _Current_iteration = _Worker_range._M_current_iteration;
        _Index_type _Scaled_index = _Current_iteration * _M_step;

        // If there is only one iteration to be executed there is no need to initialize any
        // helper classes (work is indivisible).
        if (_Worker_range._Number_of_iterations() == 1)
        {
            // Execute one iteration
            _Parallel_chunk_helper_invoke<_Random_iterator, _Index_type, _Function, _Is_iterator>::_Invoke(_M_first, _Scaled_index, _M_function);
            return;
        }

        // If the execution reaches this point it means that this class now has a chunk of work
        // that it needs to get done, so it has transitioned into the worker mode.
        structured_task_group _Helper_group;

        // Initialize fields that are needed in the helper
        _Worker_proxy<_Index_type> _Worker;

        // Instantiate a helper class for this working class and put it on the work queue.
        // If some thread is idle it will be able to steal the helper and help this class
        // finish its work by stealing a piece of the work range.
        task_handle<_Parallel_chunk_helper> _Helper_task(_Parallel_chunk_helper(_M_first, _M_step, _M_function, _Worker_range, &_Worker));

        _Helper_group.run(_Helper_task);

        for (_Index_type _I = _Current_iteration; _I < _Worker_range._M_last_iteration; (_I++, _Worker_range._M_current_iteration =_I, _Scaled_index += _M_step))
        {
            if (is_current_task_group_canceling())
            {
                // Either a parent task group is canceled or one of the other iterations
                // threw an exception. Abort the remaining iterations
                break;
            }

            if (_Worker._Is_helper_registered())
            {
                // The helper class (there can only be one) registered to help this class with the work.
                // Thus, figure out if this class needs help and split the range among the two classes.

                if (_Worker._Send_range(&_Worker_range))
                {
                    // Construct every new instance of a helper class on the stack because it is beneficial to use
                    // a structured task group where the class itself is responsible for task handle's lifetime.
                    task_handle<_Parallel_chunk_helper> * _Helper_subtask =
                        (task_handle<_Parallel_chunk_helper> *) _alloca(sizeof(task_handle<_Parallel_chunk_helper>));

                    new(_Helper_subtask) task_handle<_Parallel_chunk_helper>
                        (_Parallel_chunk_helper(_M_first, _M_step, _M_function, _Worker_range, &_Worker));

                    // If _Send_range returns true, that means that there is still some non-trivial
                    // work to be done, so this class will potentially need another helper.
                    _Helper_group.run(*_Helper_subtask);
                }
            }

            // Allow intrusive stealing by the helper
            _Worker._Enable_intrusive_steal(&_Worker_range);

            // Execute one iteration: the element is at scaled index away from the first element.
            _Parallel_chunk_helper_invoke<_Random_iterator, _Index_type, _Function, _Is_iterator>::_Invoke(_M_first, _Scaled_index, _M_function);

            // Helper shall not steal a range after this call
            _Worker._Disable_intrusive_steal();
        }

        // Indicate that the worker is done with its iterations.
        _Worker._Set_done();

        // Wait for all worker/helper iterations to finish
        _Helper_group.wait();
    }

private:

    const _Random_iterator&             _M_first;
    const _Index_type&                  _M_step;
    const _Function&                    _M_function;

    const _Index_type                   _M_first_iteration;
    const _Index_type                   _M_last_iteration;

    _Worker_proxy<_Index_type> * const   _M_parent_worker;

    _Parallel_chunk_helper const & operator=(_Parallel_chunk_helper const&);    // no assignment operator
};

#pragma warning(pop)

// Helper functions that implement parallel_for

template <typename _Random_iterator, typename _Index_type, typename _Worker_class, typename _Function>
void _Parallel_chunk_impl(const _Random_iterator& _First, _Index_type _Num_iterations, const _Index_type& _Step, const _Function& _Func,
    structured_task_group& _Task_group, task_handle<_Worker_class> * _Chunk_helpers, _Index_type _Num_chunks, bool _Do_inline_last_task_and_wait)
{
    _ASSERTE(_Num_chunks != 0);

    _Index_type _Iterations_per_chunk = _Num_iterations / _Num_chunks;
    _Index_type _Remaining_iterations = _Num_iterations % _Num_chunks;

    // If there is less iterations than desired chunks, then set the chunk number
    // to be the number of iterations.
    if (_Iterations_per_chunk == 0)
    {
        _Num_chunks = _Remaining_iterations;
    }

    _Index_type _Work_size = 0;
    _Index_type _Start_iteration = 0;
    _Index_type _I;

    // Split the available work in chunks
    for (_I = 0; _I < _Num_chunks - 1; _I++)
    {
        if (_Remaining_iterations > 0)
        {
            // Iterations are not divided evenly, so add 1 remainder iteration each time
            _Work_size = _Iterations_per_chunk + 1;
            _Remaining_iterations--;
        }
        else
        {
            _Work_size = _Iterations_per_chunk;
        }

        // Create task_handle "in-place", in the array preallocated on the stack
        new(&_Chunk_helpers[_I]) task_handle<_Worker_class>(_Worker_class(_First, _Start_iteration, _Start_iteration + _Work_size, _Step, _Func));

        // Run each of the chunk tasks in parallel
        _Task_group.run(_Chunk_helpers[_I]);

        // Prepare for the next iteration
        _Start_iteration += _Work_size;
    }

    // Since this is the last iteration, then work size might be different
    _ASSERTE((_Remaining_iterations == 0) || ((_Iterations_per_chunk == 0) && (_Remaining_iterations == 1)));
    _Work_size = _Num_iterations - _Start_iteration;

    // Create task_handle "in-place", in the array preallocated on the stack
    new(&_Chunk_helpers[_I]) task_handle<_Worker_class>(_Worker_class(_First, _Start_iteration, _Start_iteration + _Work_size, _Step, _Func));

    // Either run the last task inline and wait, or run it and defer to the caller to wait
    if (_Do_inline_last_task_and_wait)
    {
        _Task_group.run_and_wait(_Chunk_helpers[_I]);
    }
    else
    {
        _Task_group.run(_Chunk_helpers[_I]);
    }
}

template <typename _Index_type, typename _Function>
__declspec(safebuffers) void _Parallel_for_impl(_Index_type _First, _Index_type _Last, _Index_type _Step, const _Function& _Func)
{
    // The step argument must be 1 or greater; otherwise it is an invalid argument
    if (_Step < 1)
    {
        throw std::invalid_argument("_Step");
    }

    // If there are no elements in this range we just return
    if (_First >= _Last)
    {
        return;
    }

    // Compute the difference type based on the arguments and avoid signed overflow for int, long, and long long
    typedef typename std::tr1::conditional<std::tr1::is_same<_Index_type, int>::value, unsigned int,
        typename std::tr1::conditional<std::tr1::is_same<_Index_type, long>::value, unsigned long,
            typename std::tr1::conditional<std::tr1::is_same<_Index_type, long long>::value, unsigned long long, decltype(_Last - _First)
            >::type
        >::type
    >::type _Diff_type;
    typedef _Parallel_chunk_helper<_Index_type, _Diff_type, _Function, false> _Worker_class;

    _Diff_type _Range = _Diff_type(_Last) - _Diff_type(_First);
    _Diff_type _Diff_step = _Step;

    if (_Range <= _Diff_step)
    {
        _Func(_First);
    }
    else
    {
        // Get the number of chunks to divide the work
        _Diff_type _Num_chunks = _Get_num_chunks<_Diff_type>();
        _Diff_type _Num_iterations = _Range;
        _ASSERTE(_Num_chunks > 0);
        _ASSERTE(_Diff_step > 0);

        if (_Diff_step != 1)
        {
            _Num_iterations = ((_Range - 1) / _Diff_step) + 1;
        }

        _ASSERTE(_Num_iterations > 1);

        // Allocate memory on the stack for task_handles to ensure everything is properly structured.
        task_handle<_Worker_class> * _Chunk_helpers = (task_handle<_Worker_class> *) _malloca(sizeof(task_handle<_Worker_class>) * _Num_chunks);
        ::Concurrency::details::_MallocaHolder _Holder(_Chunk_helpers);

        bool _Do_inline_last_task_and_wait = true;
        structured_task_group _Task_group;

        _Parallel_chunk_impl(_First, _Num_iterations, _Diff_step, _Func, _Task_group, _Chunk_helpers, _Num_chunks, _Do_inline_last_task_and_wait);

        // If _Parallel_chunk_impl inlined the last task and waited, then there is no need to wait again
        if (!_Do_inline_last_task_and_wait)
        {
            _Task_group.wait();
        }
    }
}

/// <summary>
///     <c>parallel_for</c> iterates over a range of indices and executes a user-supplied function at each iteration, in parallel.
/// </summary>
/// <typeparam name="_Index_type">
///     The type of the index being used for the iteration. <paramref name="_Index_type"/> must be an integral type.
/// </typeparam>
/// <typeparam name="_Function">
///     The type of the function that will be executed at each iteration.
/// </typeparam>
/// <param name="_First">
///     The first index to be included in the iteration.
/// </param>
/// <param name="_Last">
///     The index one past the last index to be included in the iteration.
/// </param>
/// <param name="_Step">
///     The value by which to step when iterating from <paramref name="_First"/> to <paramref name="_Last"/>. The step must be positive.
///     <see cref="invalid_argument Class">invalid_argument</see> is thrown if the step is less than 1.
/// </param>
/// <param name="_Func">
///     The function to be executed at each iteration. This may be a lambda expression, a function pointer, or any object
///     that supports a version of the function call operator with the signature
///     <c>void operator()(</c><typeparamref name="_Index_type"/><c>)</c>.
/// </param>
/// <remarks>
///     For more information, see <see cref="Parallel Algorithms"/>.
/// </remarks>
/**/
template <typename _Index_type, typename _Function>
void parallel_for(_Index_type _First, _Index_type _Last, _Index_type _Step, const _Function& _Func)
{
    _Trace_ppl_function(PPLParallelForEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);
    _Parallel_for_impl(_First, _Last, _Step, _Func);
    _Trace_ppl_function(PPLParallelForEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     <c>parallel_for</c> iterates over a range of indices and executes a user-supplied function at each iteration, in parallel.
/// </summary>
/// <typeparam name="_Index_type">
///     The type of the index being used for the iteration.
/// </typeparam>
/// <typeparam name="_Function">
///     The type of the function that will be executed at each iteration.
/// </typeparam>
/// <param name="_First">
///     The first index to be included in the iteration.
/// </param>
/// <param name="_Last">
///     The index one past the last index to be included in the iteration.
/// </param>
/// <param name="_Func">
///     The function to be executed at each iteration. This may be a lambda expression, a function pointer, or any object
///     that supports a version of the function call operator with the signature
///     <c>void operator()(</c><typeparamref name="_Index_type"/><c>)</c>.
/// </param>
/// <remarks>
///     For more information, see <see cref="Parallel Algorithms"/>.
/// </remarks>
/**/
template <typename _Index_type, typename _Function>
void parallel_for(_Index_type _First, _Index_type _Last, const _Function& _Func)
{
    parallel_for(_First, _Last, _Index_type(1), _Func);
}

// parallel_for_each -- This function will iterate over all elements in the iterator’s range.

// Closure (binding) classes for invoking parallel_for_each recursively

// A closure class used for packaging chunk of elements in parallel_for_each for parallel invocation

// Forward iterator for_each using unstructured task group

// Disable C4180: qualifier applied to function type has no meaning; ignored
// Warning fires for passing Foo function pointer to parallel_for instead of &Foo.
#pragma warning(push)
#pragma warning(disable: 4180)

template <typename _Forward_iterator, typename _Function, unsigned int _Chunk_size>
class _Parallel_for_each_helper
{
public:
    typedef typename std::iterator_traits<_Forward_iterator>::value_type _Value_type;
    static const unsigned int _Size = _Chunk_size;

    _Parallel_for_each_helper(_Forward_iterator& _First, const _Forward_iterator& _Last, const _Function& _Func) :
        _M_function(_Func), _M_len(0)
    {
        // Add a batch of work items to this functor's array
        for (unsigned int _Index=0; (_Index < _Size) && (_First != _Last); _Index++)
        {
            _M_element[_M_len++] = &(*_First++);
        }
    }

    void operator()() const
    {
        // Invoke parallel_for on the batched up array of elements
        _Parallel_for_impl(0U, _M_len, 1U,
            [this] (unsigned int _Index)
            {
                _M_function(*(_M_element[_Index]));
            }
        );
    }

private:

    const _Function& _M_function;
    _Value_type *    _M_element[_Size];
    unsigned int     _M_len;

    _Parallel_for_each_helper const & operator=(_Parallel_for_each_helper const&);    // no assignment operator
};

#pragma warning(pop)

// Helper functions that implement parallel_for_each

template <typename _Forward_iterator, typename _Function>
void _Parallel_for_each_chunk(_Forward_iterator& _First, const _Forward_iterator& _Last, const _Function& _Func, task_group& _Task_group)
{
    // The chunk size selection depends more on the internal implementation of parallel_for than
    // on the actual input. Also, it does not have to be dynamically computed, but it helps
    // parallel_for if it is a power of 2 (easy to divide).
    const unsigned int _Chunk_size = 1024;

    // This functor will be copied on the heap and will execute the chunk in parallel
    _Parallel_for_each_helper<_Forward_iterator, _Function, _Chunk_size> _Functor(_First, _Last, _Func);

    // Since this is an unstructured task group, running the task will make a copy of the necessary data
    // on the heap, ensuring that it is available at the time of execution.
    _Task_group.run(_Functor);
}

template <typename _Forward_iterator, typename _Function>
void _Parallel_for_each_forward_impl(_Forward_iterator& _First, const _Forward_iterator& _Last, const _Function& _Func, task_group& _Task_group)
{
    _Parallel_for_each_chunk(_First, _Last, _Func, _Task_group);

    // If there is a tail, push the tail
    if (_First != _Last)
    {
        _Task_group.run(
            [&_First, &_Last, &_Func, &_Task_group]
            {
                Concurrency::_Parallel_for_each_forward_impl(_First, _Last, _Func, _Task_group);
            }
        );
    }
}

template <typename _Forward_iterator, typename _Function>
void _Parallel_for_each_impl(_Forward_iterator _First, const _Forward_iterator& _Last, const _Function& _Func, std::forward_iterator_tag)
{
    // Since this is a forward iterator, it is difficult to validate that _First comes before _Last, so
    // it is up to the user to provide valid range.
    if (_First != _Last)
    {
        task_group _Task_group;

        _Parallel_for_each_forward_impl(_First, _Last, _Func, _Task_group);

        _Task_group.wait();
    }
}

template <typename _Random_iterator, typename _Function>
void _Parallel_for_each_impl(const _Random_iterator& _First, const _Random_iterator& _Last, const _Function& _Func, std::random_access_iterator_tag)
{
    typedef std::iterator_traits<_Random_iterator>::difference_type _Index_type;
    typedef _Parallel_chunk_helper<_Random_iterator, _Index_type, _Function, true> _Worker_class;

    // Exit early if there is nothing in the collection
    if (_First >= _Last)
    {
        return;
    }

    _Index_type _Range = _Last - _First;

    if (_Range == 1)
    {
        _Func(*_First);
    }
    else
    {
        _Index_type _Step = 1;
        _Index_type _Num_chunks = _Get_num_chunks<_Index_type>();
        _ASSERTE(_Num_chunks > 0);

        // Allocate space on the stack for the task handles only if task group is structured
        task_handle<_Worker_class> * _Chunk_helpers = (task_handle<_Worker_class> *) _malloca(sizeof(task_handle<_Worker_class>) * _Num_chunks);
        ::Concurrency::details::_MallocaHolder _Holder(_Chunk_helpers);

        bool _Do_inline_last_task_and_wait = true;
        structured_task_group _Task_group;

        // Use the same function that schedules work for parallel for
        _Parallel_chunk_impl(_First, _Range, _Step, _Func, _Task_group, _Chunk_helpers, _Num_chunks, _Do_inline_last_task_and_wait);

        // If _Parallel_chunk_impl inlined the last task and waited, then there is no need to wait again
        if (!_Do_inline_last_task_and_wait)
        {
            _Task_group.wait();
        }
    }
}

/// <summary>
///     <c>parallel_for_each</c> applies a specified function to each element within a range, in parallel. It is semantically
///     equivalent to the <c>for_each</c> function in the <c>std</c> namespace, except that iteration over the elements is
///     performed in parallel, and the order of iteration is unspecified. The argument <paramref name="_Func"/> must support
///     a function call operator of the form <c>operator()(T)</c> where the parameter <paramref name="T"/> is the item type
///     of the container being iterated over.
/// </summary>
/// <typeparam name="_Iterator">
///     The type of the iterator being used to iterate over the container.
/// </typeparam>
/// <typeparam name="_Function">
///     The type of the function that will be applied to each element within the range.
/// </typeparam>
/// <param name="_First">
///     An iterator addressing the position of the first element to be included in parallel iteration.
/// </param>
/// <param name="_Last">
///     An iterator addressing the position one past the final element to be included in parallel iteration.
/// </param>
/// <param name="_Func">
///     A user-defined function object that is applied to each element in the range.
/// </param>
/// <remarks>
///     For more information, see <see cref="Parallel Algorithms"/>.
/// </remarks>
/**/
template <typename _Iterator, typename _Function>
void parallel_for_each(_Iterator _First, _Iterator _Last, const _Function& _Func)
{
    _Trace_ppl_function(PPLParallelForeachEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_START);
    _Parallel_for_each_impl(_First, _Last, _Func, std::_Iter_cat(_First));
    _Trace_ppl_function(PPLParallelForeachEventGuid, _TRACE_LEVEL_INFORMATION, CONCRT_EVENT_END);
}

/// <summary>
///     The <c>combinable&lt;T&gt;</c> object is intended to provide thread-private copies of data, to perform lock-free
///     thread-local sub-computations during parallel algorithms.  At the end of the parallel operation, the
///     thread-private sub-computations can then be merged into a final result.  This class can be used instead of
///     a shared variable, and can result in a performance improvement if there would otherwise be a lot of
///     contention on that shared variable.
/// </summary>
/// <typeparam name="_Ty">
///     The data type of the final merged result. The type must have a copy constructor and a default constructor.
/// </typeparam>
/// <remarks>
///     For more information, see <see cref="Parallel Containers and Objects"/>.
/// </remarks>
/**/
template<typename _Ty>
class combinable
{
private:

// Disable warning C4324: structure was padded due to __declspec(align())
// This padding is expected and necessary.
#pragma warning(push)
#pragma warning(disable: 4324)
    __declspec(align(64))
    struct _Node
    {
        unsigned long _M_key;
        _Ty _M_value;
        _Node* _M_chain;

        _Node(unsigned long _Key, _Ty _InitialValue)
            : _M_key(_Key), _M_value(_InitialValue), _M_chain(NULL)
        {
        }
    };
#pragma warning(pop)

    static _Ty _DefaultInit()
    {
        return _Ty();
    }

public:
    /// <summary>
    ///     Constructs a new <c>combinable</c> object.
    /// </summary>
    /// <remarks>
    ///     <para>The first constructor initializes new elements with the default constructor for the type <paramref name="_Ty"/>.</para>
    ///     <para>The second constructor initializes new elements using the initialization functor supplied as the
    ///           <paramref name="_FnInitialize"/> parameter.</para>
    ///     <para>The third constructor is the copy constructor.</para>
    /// </remarks>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    combinable()
        : _M_fnInitialize(_DefaultInit)
    {
        _InitNew();
    }

    /// <summary>
    ///     Constructs a new <c>combinable</c> object.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the initialization functor object.
    /// </typeparam>
    /// <param name="_FnInitialize">
    ///     A function which will be called to initialize each new thread-private value of the type <paramref name="_Ty"/>.
    ///     It must support a function call operator with the signature <c>_Ty ()</c>.
    /// </param>
    /// <remarks>
    ///     <para>The first constructor initializes new elements with the default constructor for the type <paramref name="_Ty"/>.</para>
    ///     <para>The second constructor initializes new elements using the initialization functor supplied as the
    ///           <paramref name="_FnInitialize"/> parameter.</para>
    ///     <para>The third constructor is the copy constructor.</para>
    /// </remarks>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    template <typename _Function>
    explicit combinable(_Function _FnInitialize)
        : _M_fnInitialize(_FnInitialize)
    {
        _InitNew();
    }

    /// <summary>
    ///     Constructs a new <c>combinable</c> object.
    /// </summary>
    /// <param name="_Copy">
    ///     An existing <c>combinable</c> object to be copied into this one.
    /// </param>
    /// <remarks>
    ///     <para>The first constructor initializes new elements with the default constructor for the type <paramref name="_Ty"/>.</para>
    ///     <para>The second constructor initializes new elements using the initialization functor supplied as the
    ///           <paramref name="_FnInitialize"/> parameter.</para>
    ///     <para>The third constructor is the copy constructor.</para>
    /// </remarks>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    combinable(const combinable& _Copy)
        : _M_size(_Copy._M_size), _M_fnInitialize(_Copy._M_fnInitialize)
    {
        _InitCopy(_Copy);
    }

    /// <summary>
    ///     Assigns to a <c>combinable</c> object from another <c>combinable</c> object.
    /// </summary>
    /// <param name="_Copy">
    ///     An existing <c>combinable</c> object to be copied into this one.
    /// </param>
    /// <returns>
    ///     A reference to this <c>combinable</c> object.
    /// </returns>
    /**/
    combinable& operator=(const combinable& _Copy)
    {
        clear();
        delete [] _M_buckets;
        _M_fnInitialize = _Copy._M_fnInitialize;
        _M_size = _Copy._M_size;
        _InitCopy(_Copy);

        return *this;
    }

    /// <summary>
    ///     Destroys a <c>combinable</c> object.
    /// </summary>
    /**/
    ~combinable()
    {
        clear();
        delete [] _M_buckets;
    }

    /// <summary>
    ///     Returns a reference to the thread-private sub-computation.
    /// </summary>
    /// <returns>
    ///     A reference to the thread-private sub-computation.
    /// </returns>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    _Ty& local()
    {
        unsigned long _Key = Concurrency::details::_GetCurrentThreadId();
        size_t _Index;
        _Node* _ExistingNode = _FindLocalItem(_Key, &_Index);
        if (_ExistingNode == NULL)
        {
            _ExistingNode = _AddLocalItem(_Key, _Index);
        }

        _ASSERTE(_ExistingNode != NULL);
        return _ExistingNode->_M_value;
    }

    /// <summary>
    ///     Returns a reference to the thread-private sub-computation.
    /// </summary>
    /// <param name="_Exists">
    ///     A reference to a boolean.  The boolean value referenced by this argument will be
    ///     set to <c>true</c> if the sub-computation already existed on this thread, and set to
    ///     <c>false</c> if this was the first sub-computation on this thread.
    /// </param>
    /// <returns>
    ///     A reference to the thread-private sub-computation.
    /// </returns>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    _Ty& local(bool& _Exists)
    {
        unsigned long _Key = Concurrency::details::_GetCurrentThreadId();
        size_t _Index;
        _Node* _ExistingNode = _FindLocalItem(_Key, &_Index);
        if (_ExistingNode == NULL)
        {
            _Exists = false;
            _ExistingNode = _AddLocalItem(_Key, _Index);
        }
        else
        {
            _Exists = true;
        }

        _ASSERTE(_ExistingNode != NULL);
        return _ExistingNode->_M_value;
    }

    /// <summary>
    ///     Clears any intermediate computational results from a previous usage.
    /// </summary>
    /**/
    void clear()
    {
        for (size_t _Index = 0; _Index < _M_size; ++_Index)
        {
            _Node* _CurrentNode = _M_buckets[_Index];
            while (_CurrentNode != NULL)
            {
                _Node* _NextNode = _CurrentNode->_M_chain;
                delete _CurrentNode;
                _CurrentNode = _NextNode;
            }
        }
        memset((void*)_M_buckets, 0, _M_size * sizeof _M_buckets[0]);
    }

    /// <summary>
    ///     Computes a final value from the set of thread-local sub-computations by calling the supplied combine functor.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to combine two thread-local sub-computations.
    /// </typeparam>
    /// <param name="_FnCombine">
    ///     The functor that is used to combine the sub-computations. Its signature is <c>T (T, T)</c> or
    ///     <c>T (const T&amp;, const T&amp;)</c>, and it must be associative and commutative.
    /// </param>
    /// <returns>
    ///     The final result of combining all the thread-private sub-computations.
    /// </returns>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    template<typename _Function>
    _Ty combine(_Function _FnCombine) const
    {
        _Node* _CurrentNode = NULL;
        size_t _Index;

        // Look for the first value in the set, and use (a copy of) that as the result.
        // This eliminates a single call (of unknown cost) to _M_fnInitialize.
        for (_Index = 0; _Index < _M_size; ++_Index)
        {
            _CurrentNode = _M_buckets[_Index];
            if (_CurrentNode != NULL)
            {
                 break;
            }
        }

        // No values... return the initializer value.
        if (_CurrentNode == NULL)
        {
            return _M_fnInitialize();
        }

        // Accumulate the rest of the items in the current bucket.
        _Ty _Result = _CurrentNode->_M_value;
        for (_CurrentNode = _CurrentNode->_M_chain; _CurrentNode != NULL; _CurrentNode = _CurrentNode->_M_chain)
        {
            _Result = _FnCombine(_Result, _CurrentNode->_M_value);
        }

        // Accumulate values from the rest of the buckets.
        _ASSERTE(_Index < _M_size);
        for (++_Index; _Index < _M_size; ++_Index)
        {
            for (_CurrentNode = _M_buckets[_Index]; _CurrentNode != NULL; _CurrentNode = _CurrentNode->_M_chain)
            {
                _Result = _FnCombine(_Result, _CurrentNode->_M_value);
            }
        }

        return _Result;
    }

    /// <summary>
    ///     Computes a final value from the set of thread-local sub-computations by calling the supplied combine functor
    ///     once per thread-local sub-computation.  The final result is accumulated by the function object.
    /// </summary>
    /// <typeparam name="_Function">
    ///     The type of the function object that will be invoked to combine a single thread-local sub-computation.
    /// </typeparam>
    /// <param name="_FnCombine">
    ///     The functor that is used to combine one sub-computation.  Its signature is <c>void (T)</c> or
    ///     <c>void (const T&amp;)</c>, and must be associative and commutative.
    /// </param>
    /// <seealso cref="Parallel Containers and Objects"/>
    /**/
    template<typename _Function>
    void combine_each(_Function _FnCombine) const
    {
        for (size_t _Index = 0; _Index < _M_size; ++_Index)
        {
            for (_Node* _CurrentNode = _M_buckets[_Index]; _CurrentNode != NULL; _CurrentNode = _CurrentNode->_M_chain)
            {
                _FnCombine(_CurrentNode->_M_value);
            }
        }
    }

private:
    void _InitNew()
    {
        _M_size = Concurrency::details::_GetCombinableSize();
        _M_buckets = new _Node*[_M_size];
        memset((void*)_M_buckets, 0, _M_size * sizeof _M_buckets[0]);
    }

    void _InitCopy(const combinable& _Copy)
    {
        _M_buckets = new _Node*[_M_size];
        for (size_t _Index = 0; _Index < _M_size; ++_Index)
        {
            _M_buckets[_Index] = NULL;
            for (_Node* _CurrentNode = _Copy._M_buckets[_Index]; _CurrentNode != NULL; _CurrentNode = _CurrentNode->_M_chain)
            {
                _Node* _NewNode = new _Node(_CurrentNode->_M_key, _CurrentNode->_M_value);
                _NewNode->_M_chain = _M_buckets[_Index];
                _M_buckets[_Index] = _NewNode;
            }
        }
    }

    _Node* _FindLocalItem(unsigned long _Key, size_t* _PIndex)
    {
        _ASSERTE(_PIndex != NULL);

        *_PIndex = _Key % _M_size;

        // Search at this index for an existing value.
        _Node* _CurrentNode = _M_buckets[*_PIndex];
        while (_CurrentNode != NULL)
        {
            if (_CurrentNode->_M_key == _Key)
            {
                return _CurrentNode;
            }

            _CurrentNode = _CurrentNode->_M_chain;
        }

        return NULL;
    }

    _Node* _AddLocalItem(unsigned long _Key, size_t _Index)
    {
        _Node* _NewNode = new _Node(_Key, _M_fnInitialize());
        _Node* _TopNode;
        do
        {
            _TopNode = _M_buckets[_Index];
            _NewNode->_M_chain = _TopNode;
        } while (_InterlockedCompareExchangePointer(reinterpret_cast<void * volatile *>(&_M_buckets[_Index]), _NewNode, _TopNode) != _TopNode);

        return _NewNode;
    }

private:
    _Node *volatile * _M_buckets;
    size_t _M_size;
    std::tr1::function<_Ty ()> _M_fnInitialize;
};
}

#pragma pack(pop)

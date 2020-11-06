/***
*ti_inst.cxx - One instance of class typeinfo.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This module insures that an instance of class type_info
*       will be present in msvcrt.lib, providing access to type_info's
*       vftable when compiling MD.
*
*
****/

#ifdef MRTDLL
#undef MRTDLL
#endif  /* MRTDLL */

#define _TICORE
#include <typeinfo.h>

/*
 * The purpose of defining these two dummy functions is to generate the
 * type_info::'vftable' ["const type_info::`vftable'" (??_7type_info@@6B@)].
 * This symbol is needed in our import lib, any class with virtual member
 * functions compiled with /GR (which is on by default) will depends on this
 * type_info::'vftable' symbol.
 *
 */

#if !defined (_M_CEE)
type_info::type_info(const type_info& rhs)
{
}

type_info& __CLR_OR_THIS_CALL type_info::operator=(const type_info& rhs)
{
        return *this;
}
#endif  /* !defined (_M_CEE) */


#if defined (_M_CEE_PURE)

#include <cruntime.h>
#include <internal.h>
#include <stdlib.h>
#include <typeinfo.h>
#include <mtdll.h>
#include <string.h>
#include <dbgint.h>

#define __RELIABILITY_CONTRACT \
    [System::Runtime::ConstrainedExecution::ReliabilityContract( \
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
        System::Runtime::ConstrainedExecution::Cer::Success)]

#define __PREPARE_CONSTRAINED_REGION \
    System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions();

#define __BEGIN_CONSTRAINED_REGION \
    __PREPARE_CONSTRAINED_REGION \
    __TRY __FINALLY

#define __END_CONSTRAINED_REGION \
    __END_TRY_FINALLY

__type_info_node __type_info_root_node;

SECURITYCRITICAL_ATTRIBUTE
const char* type_info::name(__type_info_node* __ptype_info_node) const //17.3.4.2.5
{
    return _Name_base(this, __ptype_info_node);
}

type_info::~type_info()
{
    type_info::_Type_info_dtor(this);
}

ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYSAFECRITICAL_ATTRIBUTE
bool type_info::operator==(const type_info& rhs) const
{
        return (strcmp((rhs._M_d_name)+1, (_M_d_name)+1)?0:1);
}

ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYSAFECRITICAL_ATTRIBUTE
bool type_info::operator!=(const type_info& rhs) const
{
        return (strcmp((rhs._M_d_name)+1, (_M_d_name)+1)?1:0);
}

ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYSAFECRITICAL_ATTRIBUTE
int type_info::before(const type_info& rhs) const
{
        return (strcmp((rhs._M_d_name)+1,(_M_d_name)+1) > 0);
}

SECURITYSAFECRITICAL_ATTRIBUTE
const char* type_info::raw_name() const
{
    return _M_d_name;
}


/*
 * type_info::~type_info() has been moved from typinfo.cpp to typname.cpp.
 * The reason being we need to clean the link list when destructing the
 * object.
 */
__RELIABILITY_CONTRACT
ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYCRITICAL_ATTRIBUTE
void type_info::_Type_info_dtor(type_info *_This)
{
    bool _MustReleaseLock = false;
    __PREPARE_CONSTRAINED_REGION
    __TRY
        __BEGIN_CONSTRAINED_REGION
            System::Threading::Thread::BeginThreadAffinity();
            _mlock(_TYPEINFO_LOCK);
            _MustReleaseLock = true;
        __END_CONSTRAINED_REGION
        if (_This->_M_data != NULL) {
            /*
             * We should first check the global link list before freeing _M_data.
             * Ideally we should always find _M_data in the linklist.
             */
            for(__type_info_node *pNode = __type_info_root_node._Next,*tmpNode = &__type_info_root_node;
                pNode!=NULL;
                pNode = tmpNode)
            {
                if(pNode->_MemPtr == _This->_M_data) {
                    /*
                     * Once the node is found, delete it from the list and
                     * free the memroy.
                     */
                    tmpNode->_Next = pNode->_Next;
                    _free_base(pNode);
                    break;
                }
                tmpNode=pNode;
                /*
                 * This should always be true. i.e. we should always find _M_data
                 * int the global linklist.
                 */
                _ASSERTE(pNode->_Next != NULL);
            }
            /*
             * Ideally we should be freeing this in the loop but just in case
             * something is wrong, we make sure we don't leak the memory.
             */
            _free_base(_This->_M_data);

            /*
             * Note that the same object can exist in different threads. This
             * means to be very sure, we must always set _M_data to NULL so that
             * we don't land in the _ASSERTE in the previous lines.
             */
            _This->_M_data = NULL;
        }
    __FINALLY
        if (_MustReleaseLock)
        {
            _munlock(_TYPEINFO_LOCK);
            System::Threading::Thread::EndThreadAffinity();
        }
    __END_TRY_FINALLY

}

extern "C" void* __cdecl __unDNameHelper(
        char * outputString,
        const char * name,
        int maxStringLength,
        unsigned short disableFlags);

__RELIABILITY_CONTRACT
ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYCRITICAL_ATTRIBUTE
const char * type_info::_Name_base(const type_info *_This,__type_info_node* __ptype_info_node)
{
        void *pTmpUndName;
        size_t len;

        if (_This->_M_data == NULL) {
            if ((pTmpUndName = __unDNameHelper(NULL,
                                               (_This->_M_d_name)+1,
                                               0,
                                               0)) == NULL)
                return NULL;

            /*
             * Pad all the trailing spaces with null. Note that len-- > 0 is used
             * at left side which depends on operator associativity. Also note
             * that len will be used later so don't trash.
             */
            for (len=strlen((char *)pTmpUndName); len-- > 0 && ((char *)pTmpUndName)[len] == ' ';) {
                ((char *)pTmpUndName)[len] = '\0';
            }

            bool _MustReleaseLock = false;
            __PREPARE_CONSTRAINED_REGION
            __TRY
                __BEGIN_CONSTRAINED_REGION
                    System::Threading::Thread::BeginThreadAffinity();
                    _mlock(_TYPEINFO_LOCK);
                    _MustReleaseLock = true;
                __END_CONSTRAINED_REGION
                /*
                 * We need to check if this->_M_data is still NULL, this will
                 * prevent the memory leak.
                 */
                if (_This->_M_data == NULL) {
                    /*
                     * allocate a node which will store the pointer to the memory
                     * allocated for this->_M_data. We need to store all this in
                     * linklist so that we can free them as process exit. Note
                     * that __clean_type_info_names is freeing this memory.
                     */
                    __type_info_node *pNode = (__type_info_node *)_malloc_base(sizeof(__type_info_node));
                    if (pNode != NULL) {

                        /*
                         * We should be doing only if we are sucessful in allocating
                         * node pointer. Note that we need to add 2 to len, this
                         * is because len = strlen(pTmpUndName)-1.
                         */
                        if ((((type_info *)_This)->_M_data = _malloc_base(len+2)) != NULL) {
                            _ERRCHECK(strcpy_s ((char *)((type_info *)_This)->_M_data, len+2, (char *)pTmpUndName));
                            pNode->_MemPtr = _This->_M_data;

                            /*
                             * Add this to global linklist. Note that we always
                             * add this as second element in linklist.
                             */
                            pNode->_Next = __ptype_info_node->_Next;
                            __ptype_info_node->_Next = pNode;
                        } else {
                            /*
                             * Free node pointer as there is no allocation for
                             * this->_M_data, this means that we don't really
                             * need this in the link list.
                             */
                            _free_base(pNode);
                        }
                    }
                }
                /*
                 * Free the temporary undecorated name.
                 */
                _free_base (pTmpUndName);
            __FINALLY
                if (_MustReleaseLock)
                {
                    _munlock(_TYPEINFO_LOCK);
                    System::Threading::Thread::EndThreadAffinity();
                }
            __END_TRY_FINALLY


        }

        return (char *) _This->_M_data;
}

__RELIABILITY_CONTRACT
ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYCRITICAL_ATTRIBUTE
void __clean_type_info_names_internal(__type_info_node * p_type_info_root_node)
{
    bool _MustReleaseLock = false;
    __PREPARE_CONSTRAINED_REGION
    __TRY
        __BEGIN_CONSTRAINED_REGION
            System::Threading::Thread::BeginThreadAffinity();
            _mlock(_TYPEINFO_LOCK);
            _MustReleaseLock = true;
        __END_CONSTRAINED_REGION
        /*
         * Loop through the link list and delete all the entries.
         */
        for (__type_info_node *pNode = p_type_info_root_node->_Next, *tmpNode=NULL;
             pNode!=NULL;
             pNode = tmpNode)
        {
            tmpNode = pNode->_Next;
            _free_base(pNode->_MemPtr);
            _free_base(pNode);
        }
    __FINALLY
        if (_MustReleaseLock)
        {
            _munlock(_TYPEINFO_LOCK);
            System::Threading::Thread::EndThreadAffinity();
        }
    __END_TRY_FINALLY
}

/*
 * __clean_type_info_names_internal is invoked by __clean_type_info_names at dll unload.
 */
SECURITYSAFECRITICAL_ATTRIBUTE
extern "C" void __clrcall __clean_type_info_names()
{
    __clean_type_info_names_internal(&__type_info_root_node);
}

#endif  /* defined (_M_CEE_PURE) */

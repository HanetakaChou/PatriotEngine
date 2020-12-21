
/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

/** @file lib/librte_eal/include/rte_common.h */

#ifndef _PT_MCRT_MEMCPY_DPDK_RTE_EAL_RTE_COMMON_H_
#define _PT_MCRT_MEMCPY_DPDK_RTE_EAL_RTE_COMMON_H_ 1

#if defined(__GNUC__)

#define __rte_always_inline inline __attribute__((always_inline))

#elif defined(_MSC_VER)

#define __rte_always_inline __forceinline

#else
#error Unknown Compiler
#endif

#endif
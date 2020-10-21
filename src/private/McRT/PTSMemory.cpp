#include "../../Public/McRT/PTSMemory.h"


// SPDX-License-Identifier: BSD-3-Clause
// Copyright(c) 2010-2014 Intel Corporation

//https://git.dpdk.org/dpdk-stable/tree/lib/librte_eal/common/include/arch/x86/rte_memcpy.h?h=v18.11
//https://git.dpdk.org/dpdk-stable/tree/lib/librte_eal/common/include/arch/arm/rte_memcpy_32.h?h=v18.11
//https://git.dpdk.org/dpdk-stable/tree/lib/librte_eal/common/include/arch/arm/rte_memcpy_64.h?h=v18.11

static inline void rte_memcpy(void *dst, const void *src, size_t n);

extern "C" void PTCALL PTS_MemoryCopy(void *pOutBuf, void const *pInBuf, size_t Count)
{
	rte_memcpy(pOutBuf, pInBuf, Count);
}

#if defined(PTX86) || defined(PTX64)

#include <xmmintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <assert.h>

static inline void rte_memcpy_align16(void *dst, void const*src, size_t n)
{
	assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == 0U);
	assert((reinterpret_cast<uintptr_t>(src) & 0XFU) == 0U);

	//The CPU CacheLine Size Is 64 Or 128
	//We Use 64 To Be Compatible With Both
	while(n >= 64U)
	{
		//mov64
		{
			__m128i const x0 = _mm_load_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 0U));
			__m128i const x1 = _mm_load_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 1U));
			__m128i const x2 = _mm_load_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 2U));
			__m128i const x3 = _mm_load_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 3U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), x0);
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), x1);
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 2U), x2);
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 3U), x3);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 64U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 64U);
		n -= 64U;
	}

	if(n & 0X20U)
	{
		assert(n >= 32U && n < 64U);
		//mov32
		{
			__m128i const x0 = _mm_load_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 0U));
			__m128i const x1 = _mm_load_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 1U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), x0);
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), x1);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 32U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 32U);
		n -= 32U;
	}

	if(n & 0X10U)
	{
		assert(n >= 16U && n < 32U);
		//mov16
		{
			__m128i const x0 = _mm_load_si128(reinterpret_cast<__m128i const *>(src));
			_mm_store_si128(reinterpret_cast<__m128i *>(dst), x0);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
		n -= 16U;
	}

	if (n & 0X8U)
	{
		assert(n >= 8U && n < 16U);
		reinterpret_cast<int64_t *>(dst)[0] = reinterpret_cast<int64_t const *>(src)[0];
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
		n -= 8U;
	}

	if (n & 0X4U)
	{
		assert(n >= 4U && n < 8U);
		reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
		n -= 4U;
	}

	if (n & 0X2U)
	{
		assert(n >= 2U && n < 4U);
		reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
		n -= 2U;
	}

	if (n & 0X1U)
	{
		assert(n >= 1U && n < 2U);
		reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
	}
}

static inline void rte_memcpy_congruent16(void *dst, void const *src, size_t n)
{
	assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == (reinterpret_cast<uintptr_t>(src) & 0XFU));

	if (n >= 16U)
	{
		size_t const offset16 = 16U - (reinterpret_cast<uintptr_t>(dst) & 0XFU);
		assert(offset16 == (16U - (reinterpret_cast<uintptr_t>(src) & 0XFU)));
		//mov16
		{
			__m128i const x0 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(src));
			_mm_storeu_si128(reinterpret_cast<__m128i *>(dst), x0);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + offset16);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + offset16);
		n -= offset16;		
		rte_memcpy_align16(dst, src, n);
	}
	else
	{
		if (((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 0X3U) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else
		{
			size_t const offset4 = 4U - (reinterpret_cast<uintptr_t>(dst) & 0X3U);
			assert(offset4 == (4U - (reinterpret_cast<uintptr_t>(src) & 0X3U)));

			if (n >= offset4)
			{
				size_t remainoffset4 = offset4;

				if (remainoffset4 & 0X2U)
				{
					assert(remainoffset4 >= 2U && remainoffset4 < 4U);
					//We Not Sure Whether The Data Is Align 16
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
					remainoffset4 -= 2U;
					n -= 2U;
				}

				if (remainoffset4 & 0X1U)
				{
					assert(remainoffset4 >= 1U && remainoffset4 < 2U);
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 1U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 1U);
					remainoffset4 -= 1U;
					n -= 1U;
				}

				//Align 4
				assert(((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 0X3U) == 0U);

				if (n & 0X8U)
				{
					assert(n >= 8U && n < 16U);
					//We Not Sure Whether The Data Is Align 8
					reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
					reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
					n -= 8U;
				}

				if (n & 0X4U)
				{
					assert(n >= 4U && n < 8U);
					reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
					n -= 4U;
				}

				if (n & 0X2U)
				{
					assert(n >= 2U && n < 4U);
					reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
					n -= 2U;
				}

				if (n & 0X1U)
				{
					assert(n >= 1U && n < 2U);
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				}
			}
			else
			{
				if (n & 0X2U)
				{
					assert(n >= 2U && n < 4U);
					//We Not Sure Whether The Data Is Align 2
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
					n -= 2U;
				}

				if (n & 0X1U)
				{
					assert(n >= 1U && n < 2U);
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 1U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 1U);
					n -= 1U;
				}
			}
		}
	}
}

template<size_t const srcoffset16>
static inline void rte_movleft31_unalign(void *(&dst), void const *(&src), size_t (&n))
{
	assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == 0U);
	assert(((reinterpret_cast<uintptr_t>(src) - srcoffset16) & 0XFU) == 0U);

	//The CPU CacheLine Size Is 64 Or 128
	//We Use 64 To Be Compatible With Both
	while (n >= ((/*AddressForLast*/(16U * 8U - srcoffset16)) + (/*Length*/16U)))
	{
		//mov128
		{
			__m128i const x0 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U));
			__m128i const x1 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U));
			__m128i const x2 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 2U));
			__m128i const x3 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 3U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), _mm_alignr_epi8(x1, x0, srcoffset16));
			__m128i const x4 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 4U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), _mm_alignr_epi8(x2, x1, srcoffset16));
			__m128i const x5 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 5U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 2U), _mm_alignr_epi8(x3, x2, srcoffset16));
			__m128i const x6 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 6U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 3U), _mm_alignr_epi8(x4, x3, srcoffset16));
			__m128i const x7 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 7U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 4U), _mm_alignr_epi8(x5, x4, srcoffset16));
			__m128i const x8 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 8U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 5U), _mm_alignr_epi8(x6, x5, srcoffset16));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 6U), _mm_alignr_epi8(x7, x6, srcoffset16));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 7U), _mm_alignr_epi8(x8, x7, srcoffset16));
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 128U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 128U);
		n -= 128U;
	}

	if (n >= ((/*AddressForLast*/(16U * 4U - srcoffset16)) + (/*Length*/16U)))
	{
		assert(n < ((/*AddressForLast*/(16U * 8U - srcoffset16)) + (/*Length*/16U)));
		//mov64
		{
			__m128i const xmm0 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U));
			__m128i const xmm1 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U));
			__m128i const xmm2 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 2U));
			__m128i const xmm3 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 3U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), _mm_alignr_epi8(xmm1, xmm0, srcoffset16));
			__m128i const xmm4 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 4U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), _mm_alignr_epi8(xmm2, xmm1, srcoffset16));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 2U), _mm_alignr_epi8(xmm3, xmm2, srcoffset16));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 3U), _mm_alignr_epi8(xmm4, xmm3, srcoffset16));
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 64U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 64U);
		n -= 64U;
	}

	if (n >= ((/*AddressForLast*/(16U * 2U - srcoffset16)) + (/*Length*/16U)))
	{
		assert(n < ((/*AddressForLast*/(16U * 4U - srcoffset16)) + (/*Length*/16U)));
		//mov32
		{
			__m128i const xmm0 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U));
			__m128i const xmm1 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U));
			__m128i const xmm2 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 2U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), _mm_alignr_epi8(xmm1, xmm0, srcoffset16));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), _mm_alignr_epi8(xmm2, xmm1, srcoffset16));
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 32U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 32U);
		n -= 32U;
	}

	if (n >= ((/*AddressForLast*/(16U * 1U - srcoffset16)) + (/*Length*/16U)))
	{
		assert(n < ((/*AddressForLast*/(16U * 2U - srcoffset16)) + (/*Length*/16U)));
		//mov16
		{
			__m128i const xmm0 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U));
			__m128i const xmm1 = _mm_load_si128(reinterpret_cast<__m128i const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U));
			_mm_store_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), _mm_alignr_epi8(xmm1, xmm0, srcoffset16));
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
		n -= 16U;
	}

	assert(n < 32U);
}

static inline void rte_memcpy_unalign(void *dst, void const*src, size_t n)
{
	if (n >= 32)
	{
		size_t const dstoffset16 = 16U - (reinterpret_cast<uintptr_t>(dst) & 0XFU);
		//mov32
		{
			__m128i const x0 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 0U));
			__m128i const x1 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(reinterpret_cast<uintptr_t>(src) + 16U * 1U));
			_mm_storeu_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), x0);
			_mm_storeu_si128(reinterpret_cast<__m128i *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), x1);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + (dstoffset16 + 16U));
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + (dstoffset16 + 16U));
		n -= (dstoffset16 + 16U);

		size_t const srcoffset16 = reinterpret_cast<uintptr_t>(src) & 0XFU;

		//We Must Pass A Constant Express To The "count" Parameter Of "_mm_alignr_epi8"
		switch (srcoffset16)
		{
		case 0X1U: rte_movleft31_unalign<0X1U>(dst, src, n); break;
		case 0X2U: rte_movleft31_unalign<0X2U>(dst, src, n); break;
		case 0X3U: rte_movleft31_unalign<0X3U>(dst, src, n); break;
		case 0X4U: rte_movleft31_unalign<0X4U>(dst, src, n); break;
		case 0X5U: rte_movleft31_unalign<0X5U>(dst, src, n); break;
		case 0X6U: rte_movleft31_unalign<0X6U>(dst, src, n); break;
		case 0X7U: rte_movleft31_unalign<0X7U>(dst, src, n); break;
		case 0X8U: rte_movleft31_unalign<0X8U>(dst, src, n); break;
		case 0X9U: rte_movleft31_unalign<0X9U>(dst, src, n); break;
		case 0XAU: rte_movleft31_unalign<0XAU>(dst, src, n); break;
		case 0XBU: rte_movleft31_unalign<0XBU>(dst, src, n); break;
		case 0XCU: rte_movleft31_unalign<0XCU>(dst, src, n); break;
		case 0XDU: rte_movleft31_unalign<0XDU>(dst, src, n); break;
		case 0XEU: rte_movleft31_unalign<0XEU>(dst, src, n); break;
		case 0XFU: rte_movleft31_unalign<0XFU>(dst, src, n); break;
		default:assert(0);
		}

		//dst Is Align 16
		assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == 0U);

		if (n & 0X10U)
		{
			assert(n >= 16U && n < 32U);
			//mov16
			{
				__m128i const x0 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(src));
				//dst Is Align 16
				_mm_store_si128(reinterpret_cast<__m128i *>(dst), x0);
			}
			dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
			src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
			n -= 16U;
		}

		if ((reinterpret_cast<uintptr_t>(src) & 0X3U) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else if ((reinterpret_cast<uintptr_t>(src) & 0X1U) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				reinterpret_cast<int16_t *>(dst)[2] = reinterpret_cast<int16_t const *>(src)[2];
				reinterpret_cast<int16_t *>(dst)[3] = reinterpret_cast<int16_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				reinterpret_cast<int8_t *>(dst)[4] = reinterpret_cast<int8_t const *>(src)[4];
				reinterpret_cast<int8_t *>(dst)[5] = reinterpret_cast<int8_t const *>(src)[5];
				reinterpret_cast<int8_t *>(dst)[6] = reinterpret_cast<int8_t const *>(src)[6];
				reinterpret_cast<int8_t *>(dst)[7] = reinterpret_cast<int8_t const *>(src)[7];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				//We Not Sure Whether The Data Is Align 2
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
	}
	else
	{
		if (n & 0X10U)
		{
			assert(n >= 16U && n < 32U);
			//mov16
			{
				__m128i const x0 = _mm_loadu_si128(reinterpret_cast<__m128i const *>(src));
				_mm_storeu_si128(reinterpret_cast<__m128i *>(dst), x0);
			}
			dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
			src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
			n -= 16U;
		}

		if (((reinterpret_cast<uintptr_t>(dst) & 0X3U) | (reinterpret_cast<uintptr_t>(src) & 0X3U)) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else if (((reinterpret_cast<uintptr_t>(dst) & 0X1U) | (reinterpret_cast<uintptr_t>(src) & 0X1U)) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				reinterpret_cast<int16_t *>(dst)[2] = reinterpret_cast<int16_t const *>(src)[2];
				reinterpret_cast<int16_t *>(dst)[3] = reinterpret_cast<int16_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				reinterpret_cast<int8_t *>(dst)[4] = reinterpret_cast<int8_t const *>(src)[4];
				reinterpret_cast<int8_t *>(dst)[5] = reinterpret_cast<int8_t const *>(src)[5];
				reinterpret_cast<int8_t *>(dst)[6] = reinterpret_cast<int8_t const *>(src)[6];
				reinterpret_cast<int8_t *>(dst)[7] = reinterpret_cast<int8_t const *>(src)[7];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				//We Not Sure Whether The Data Is Align 2
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
	}
}

static inline void rte_memcpy(void *dst, const void *src, size_t n)
{
	if (((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 0XFU) == 0U)
	{
		rte_memcpy_align16(dst, src, n);
	}
	else if((reinterpret_cast<uintptr_t>(dst) & 0XFU) == (reinterpret_cast<uintptr_t>(src) & 0XFU))
	{
		rte_memcpy_congruent16(dst, src, n);
	}
	else
	{
		rte_memcpy_unalign(dst, src, n);
	}
}

#elif defined(PTARM) || defined(PTARM64)

#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>

//To Be Compatible With MSVC

#ifndef vld1q_s8_ex
#define vld1q_s8_ex(pD, align) (vld1q_s8(pD))
#endif

#ifndef vst1q_s8_ex
#define vst1q_s8_ex(pD, Q, align) (vst1q_s8(pD, Q))
#endif

#ifndef vld2q_s8_ex
#define vld2q_s8_ex(pD, align) (vld2q_s8(pD))
#endif

#ifndef vst2q_s8_ex
#define vst2q_s8_ex(pD, Q, align) (vst2q_s8(pD, Q))
#endif

#ifndef vld3q_s8_ex
#define vld3q_s8_ex(pD, align) (vld3q_s8(pD))
#endif

#ifndef vst3q_s8_ex
#define vst3q_s8_ex(pD, Q, align) (vst3q_s8(pD, Q))
#endif

#ifndef vld4q_s8_ex
#define vld4q_s8_ex(pD, align) (vld4q_s8(pD))
#endif

#ifndef vst4q_s8_ex
#define vst4q_s8_ex(pD, Q, align) (vst4q_s8(pD, Q))
#endif

#ifndef vld1q_s32_ex
#define vld1q_s32_ex(pD, align) (vld1q_s32(pD))
#endif

#ifndef vst1q_s32_ex
#define vst1q_s32_ex(pD, Q, align) (vst1q_s32(pD, Q))
#endif

#ifndef vld2q_s32_ex
#define vld2q_s32_ex(pD, align) (vld2q_s32(pD))
#endif

#ifndef vst2q_s32_ex
#define vst2q_s32_ex(pD, Q, align) (vst2q_s32(pD, Q))
#endif

#ifndef vld4q_s32_ex
#define vld4q_s32_ex(pD, align) (vld4q_s32(pD))
#endif

#ifndef vst4q_s32_ex
#define vst4q_s32_ex(pD, Q, align) (vst4q_s32(pD, Q))
#endif

static inline void rte_memcpy_align16(void *dst, void const*src, size_t n)
{
	assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == 0U);
	assert((reinterpret_cast<uintptr_t>(src) & 0XFU) == 0U);

	//The CPU CacheLine Size Is 64 Or 128
	//We Use 64 To Be Compatible With Both
	while (n >= 64U)
	{
		//mov64
		{
			int32x4x4_t const x0x1x2x3 = vld4q_s32_ex(reinterpret_cast<int32_t const *>(src), 128);
			vst4q_s32_ex((reinterpret_cast<int32_t *>(dst)), x0x1x2x3, 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 64U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 64U);
		n -= 64U;
	}

	if (n & 0X20U)
	{
		assert(n >= 32U && n < 64U);
		//mov32
		{
			int32x4x2_t const x0x1 = vld2q_s32_ex(reinterpret_cast<int32_t const *>(src), 128);
			vst2q_s32_ex((reinterpret_cast<int32_t *>(dst)), x0x1, 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 32U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 32U);
		n -= 32U;
	}

	if (n & 0X10U)
	{
		assert(n >= 16U && n < 32U);
		//mov16
		{
			int32x4_t const x0 = vld1q_s32_ex(reinterpret_cast<int32_t const *>(src), 128);
			vst1q_s32_ex((reinterpret_cast<int32_t *>(dst)), x0, 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
		n -= 16U;
	}

	if (n & 0X8U)
	{
		assert(n >= 8U && n < 16U);
		reinterpret_cast<int64_t *>(dst)[0] = reinterpret_cast<int64_t const *>(src)[0];
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
		n -= 8U;
	}

	if (n & 0X4U)
	{
		assert(n >= 4U && n < 8U);
		reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
		n -= 4U;
	}

	if (n & 0X2U)
	{
		assert(n >= 2U && n < 4U);
		reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
		n -= 2U;
	}

	if (n & 0X1U)
	{
		assert(n >= 1U && n < 2U);
		reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
	}
}

static inline void rte_memcpy_congruent16(void *dst, void const *src, size_t n)
{
	assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == (reinterpret_cast<uintptr_t>(src) & 0XFU));

	if (n >= 16U)
	{
		size_t const offset16 = 16U - (reinterpret_cast<uintptr_t>(dst) & 0XFU);
		assert(offset16 == (16U - (reinterpret_cast<uintptr_t>(src) & 0XFU)));
		//mov16
		{
			int8x16_t const x0 = vld1q_s8(reinterpret_cast<int8_t const *>(src));
			vst1q_s8((reinterpret_cast<int8_t *>(dst)), x0);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + offset16);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + offset16);
		n -= offset16;
		rte_memcpy_align16(dst, src, n);
	}
	else
	{
		if (((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 0X3U) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else
		{
			size_t const offset4 = 4U - (reinterpret_cast<uintptr_t>(dst) & 0X3U);
			assert(offset4 == (4U - (reinterpret_cast<uintptr_t>(src) & 0X3U)));

			if (n >= offset4)
			{
				size_t remainoffset4 = offset4;

				if (remainoffset4 & 0X2U)
				{
					assert(remainoffset4 >= 2U && remainoffset4 < 4U);
					//We Not Sure Whether The Data Is Align 16
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
					remainoffset4 -= 2U;
					n -= 2U;
				}

				if (remainoffset4 & 0X1U)
				{
					assert(remainoffset4 >= 1U && remainoffset4 < 2U);
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 1U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 1U);
					remainoffset4 -= 1U;
					n -= 1U;
				}

				//Align 4
				assert(((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 0X3U) == 0U);

				if (n & 0X8U)
				{
					assert(n >= 8U && n < 16U);
					//We Not Sure Whether The Data Is Align 8
					reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
					reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
					n -= 8U;
				}

				if (n & 0X4U)
				{
					assert(n >= 4U && n < 8U);
					reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
					n -= 4U;
				}

				if (n & 0X2U)
				{
					assert(n >= 2U && n < 4U);
					reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
					n -= 2U;
				}

				if (n & 0X1U)
				{
					assert(n >= 1U && n < 2U);
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				}
			}
			else
			{
				if (n & 0X2U)
				{
					assert(n >= 2U && n < 4U);
					//We Not Sure Whether The Data Is Align 2
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
					n -= 2U;
				}

				if (n & 0X1U)
				{
					assert(n >= 1U && n < 2U);
					reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
					dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 1U);
					src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 1U);
					n -= 1U;
				}
			}
		}
	}
}

template<size_t const srcoffset16>
static inline void rte_movleft31_unalign(void *(&dst), void const *(&src), size_t(&n))
{
	assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == 0U);
	assert(((reinterpret_cast<uintptr_t>(src) - srcoffset16) & 0XFU) == 0U);

	//The CPU CacheLine Size Is 64 Or 128
	//We Use 64 To Be Compatible With Both
	while (n >= ((/*AddressForLast*/(16U * 8U - srcoffset16)) + (/*Length*/16U)))
	{
		//mov128
		{
			int8x16_t const x0 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U), 128);
			int8x16_t const x1 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U), 128);
			int8x16_t const x2 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 2U), 128);
			int8x16_t const x3 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 3U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), vextq_s8(x0, x1, srcoffset16), 128);
			int8x16_t const x4 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 4U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), vextq_s8(x1, x2, srcoffset16), 128);
			int8x16_t const x5 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 5U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 2U), vextq_s8(x2, x3, srcoffset16), 128);
			int8x16_t const x6 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 6U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 3U), vextq_s8(x3, x4, srcoffset16), 128);
			int8x16_t const x7 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 7U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 4U), vextq_s8(x4, x5, srcoffset16), 128);
			int8x16_t const x8 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 8U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 5U), vextq_s8(x5, x6, srcoffset16), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 6U), vextq_s8(x6, x7, srcoffset16), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 7U), vextq_s8(x7, x8, srcoffset16), 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 128U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 128U);
		n -= 128U;
	}

	if (n >= ((/*AddressForLast*/(16U * 4U - srcoffset16)) + (/*Length*/16U)))
	{
		assert(n < ((/*AddressForLast*/(16U * 8U - srcoffset16)) + (/*Length*/16U)));
		//mov64
		{
			int8x16_t const x0 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U), 128);
			int8x16_t const x1 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U), 128);
			int8x16_t const x2 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 2U), 128);
			int8x16_t const x3 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 3U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), vextq_s8(x0, x1, srcoffset16), 128);
			int8x16_t const x4 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 4U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), vextq_s8(x1, x2, srcoffset16), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 2U), vextq_s8(x2, x3, srcoffset16), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 3U), vextq_s8(x3, x4, srcoffset16), 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 64U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 64U);
		n -= 64U;
	}


	if (n >= ((/*AddressForLast*/(16U * 2U - srcoffset16)) + (/*Length*/16U)))
	{
		assert(n < ((/*AddressForLast*/(16U * 4U - srcoffset16)) + (/*Length*/16U)));
		//mov32
		{
			int8x16_t const x0 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U), 128);
			int8x16_t const x1 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U), 128);
			int8x16_t const x2 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 2U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), vextq_s8(x0, x1, srcoffset16), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 1U), vextq_s8(x1, x2, srcoffset16), 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 32U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 32U);
		n -= 32U;
	}

	if (n >= ((/*AddressForLast*/(16U * 1U - srcoffset16)) + (/*Length*/16U)))
	{
		assert(n < ((/*AddressForLast*/(16U * 2U - srcoffset16)) + (/*Length*/16U)));
		//mov16
		{
			int8x16_t const x0 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 0U), 128);
			int8x16_t const x1 = vld1q_s8_ex(reinterpret_cast<int8_t const *>((reinterpret_cast<uintptr_t>(src) - srcoffset16) + 16U * 1U), 128);
			vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), vextq_s8(x0, x1, srcoffset16), 128);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
		n -= 16U;
	}

	assert(n < 32U);
}

static inline void rte_memcpy_unalign(void *dst, void const*src, size_t n)
{
	if (n >= 32)
	{
		size_t const dstoffset16 = 16U - (reinterpret_cast<uintptr_t>(dst) & 0XFU);
		//mov32
		{
			int8x16x2_t const x0x1 = vld2q_s8(reinterpret_cast<int8_t const *>(reinterpret_cast<uintptr_t>(src) + 16U * 0U));
			vst2q_s8(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), x0x1);
		}
		dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + (dstoffset16 + 16U));
		src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + (dstoffset16 + 16U));
		n -= (dstoffset16 + 16U);

		size_t const srcoffset16 = reinterpret_cast<uintptr_t>(src) & 0XFU;

		//We Must Pass A Constant Express To The "count" Parameter Of "vextq_s8"
		switch (srcoffset16)
		{
		case 0X1U: rte_movleft31_unalign<0X1U>(dst, src, n); break;
		case 0X2U: rte_movleft31_unalign<0X2U>(dst, src, n); break;
		case 0X3U: rte_movleft31_unalign<0X3U>(dst, src, n); break;
		case 0X4U: rte_movleft31_unalign<0X4U>(dst, src, n); break;
		case 0X5U: rte_movleft31_unalign<0X5U>(dst, src, n); break;
		case 0X6U: rte_movleft31_unalign<0X6U>(dst, src, n); break;
		case 0X7U: rte_movleft31_unalign<0X7U>(dst, src, n); break;
		case 0X8U: rte_movleft31_unalign<0X8U>(dst, src, n); break;
		case 0X9U: rte_movleft31_unalign<0X9U>(dst, src, n); break;
		case 0XAU: rte_movleft31_unalign<0XAU>(dst, src, n); break;
		case 0XBU: rte_movleft31_unalign<0XBU>(dst, src, n); break;
		case 0XCU: rte_movleft31_unalign<0XCU>(dst, src, n); break;
		case 0XDU: rte_movleft31_unalign<0XDU>(dst, src, n); break;
		case 0XEU: rte_movleft31_unalign<0XEU>(dst, src, n); break;
		case 0XFU: rte_movleft31_unalign<0XFU>(dst, src, n); break;
		default:assert(0);
		}

		//dst Is Align 16
		assert((reinterpret_cast<uintptr_t>(dst) & 0XFU) == 0U);

		if (n & 0X10U)
		{
			assert(n >= 16U && n < 32U);
			//mov16
			{
				int8x16_t const x0 = vld1q_s8(reinterpret_cast<int8_t const *>(reinterpret_cast<uintptr_t>(src) + 16U * 0U));
				//dst Is Align 16
				vst1q_s8_ex(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), x0, 128);
			}
			dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
			src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
			n -= 16U;
		}

		if ((reinterpret_cast<uintptr_t>(src) & 0X3U) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else if ((reinterpret_cast<uintptr_t>(src) & 0X1U) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				reinterpret_cast<int16_t *>(dst)[2] = reinterpret_cast<int16_t const *>(src)[2];
				reinterpret_cast<int16_t *>(dst)[3] = reinterpret_cast<int16_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				reinterpret_cast<int8_t *>(dst)[4] = reinterpret_cast<int8_t const *>(src)[4];
				reinterpret_cast<int8_t *>(dst)[5] = reinterpret_cast<int8_t const *>(src)[5];
				reinterpret_cast<int8_t *>(dst)[6] = reinterpret_cast<int8_t const *>(src)[6];
				reinterpret_cast<int8_t *>(dst)[7] = reinterpret_cast<int8_t const *>(src)[7];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				//We Not Sure Whether The Data Is Align 2
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
	}
	else
	{
		if (n & 0X10U)
		{
			assert(n >= 16U && n < 32U);
			//mov16
			{
				int8x16_t const x0 = vld1q_s8(reinterpret_cast<int8_t const *>(reinterpret_cast<uintptr_t>(src) + 16U * 0U));
				vst1q_s8(reinterpret_cast<int8_t *>(reinterpret_cast<uintptr_t>(dst) + 16U * 0U), x0);
			}
			dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 16U);
			src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 16U);
			n -= 16U;
		}

		if (((reinterpret_cast<uintptr_t>(dst) & 0X3U) | (reinterpret_cast<uintptr_t>(src) & 0X3U)) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				reinterpret_cast<int32_t *>(dst)[1] = reinterpret_cast<int32_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				reinterpret_cast<int32_t *>(dst)[0] = reinterpret_cast<int32_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else if (((reinterpret_cast<uintptr_t>(dst) & 0X1U) | (reinterpret_cast<uintptr_t>(src) & 0X1U)) == 0U)
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				reinterpret_cast<int16_t *>(dst)[2] = reinterpret_cast<int16_t const *>(src)[2];
				reinterpret_cast<int16_t *>(dst)[3] = reinterpret_cast<int16_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				reinterpret_cast<int16_t *>(dst)[1] = reinterpret_cast<int16_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				reinterpret_cast<int16_t *>(dst)[0] = reinterpret_cast<int16_t const *>(src)[0];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
		else
		{
			if (n & 0X8U)
			{
				assert(n >= 8U && n < 16U);
				//We Not Sure Whether The Data Is Align 8
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				reinterpret_cast<int8_t *>(dst)[4] = reinterpret_cast<int8_t const *>(src)[4];
				reinterpret_cast<int8_t *>(dst)[5] = reinterpret_cast<int8_t const *>(src)[5];
				reinterpret_cast<int8_t *>(dst)[6] = reinterpret_cast<int8_t const *>(src)[6];
				reinterpret_cast<int8_t *>(dst)[7] = reinterpret_cast<int8_t const *>(src)[7];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 8U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 8U);
				n -= 8U;
			}

			if (n & 0X4U)
			{
				assert(n >= 4U && n < 8U);
				//We Not Sure Whether The Data Is Align 4
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				reinterpret_cast<int8_t *>(dst)[2] = reinterpret_cast<int8_t const *>(src)[2];
				reinterpret_cast<int8_t *>(dst)[3] = reinterpret_cast<int8_t const *>(src)[3];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 4U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 4U);
				n -= 4U;
			}

			if (n & 0X2U)
			{
				assert(n >= 2U && n < 4U);
				//We Not Sure Whether The Data Is Align 2
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
				reinterpret_cast<int8_t *>(dst)[1] = reinterpret_cast<int8_t const *>(src)[1];
				dst = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(dst) + 2U);
				src = reinterpret_cast<void const *>(reinterpret_cast<uintptr_t>(src) + 2U);
				n -= 2U;
			}

			if (n & 0X1U)
			{
				assert(n >= 1U && n < 2U);
				reinterpret_cast<int8_t *>(dst)[0] = reinterpret_cast<int8_t const *>(src)[0];
			}
		}
	}
}

static inline void rte_memcpy(void *dst, const void *src, size_t n)
{
	if (((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 0XFU) == 0U)
	{
		rte_memcpy_align16(dst, src, n);
	}
	else if ((reinterpret_cast<uintptr_t>(dst) & 0XFU) == (reinterpret_cast<uintptr_t>(src) & 0XFU))
	{
		rte_memcpy_congruent16(dst, src, n);
	}
	else
	{
		rte_memcpy_unalign(dst, src, n);
	}
}

#else
#error 未知的架构
#endif

#include <string.h>

extern "C" PTMCRTAPI void PTCALL PTS_MemoryZero(void *pBuf, size_t Count)
{
	::memset(pBuf, 0, Count);
}

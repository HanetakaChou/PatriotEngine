/***
*** Copyright (C) 1985-2008 Intel Corporation.  All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
***
****/

#pragma once
#ifndef __midl
#ifndef _INCLUDED_IMM
#define _INCLUDED_IMM

#if defined (_M_CEE_PURE)
        #error ERROR: EMM intrinsics not supported in the pure mode!
#else  /* defined (_M_CEE_PURE) */

#include <wmmintrin.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Intel(R) AVX compiler intrinsics.
 */

typedef union __declspec(intrin_type) _CRT_ALIGN(32) __m256 {
    float m256_f32[8];
} __m256;

typedef struct __declspec(intrin_type) _CRT_ALIGN(32) {
    double m256d_f64[4];
} __m256d;

typedef union  __declspec(intrin_type) _CRT_ALIGN(32) __m256i {
    __int8              m256i_i8[32];
    __int16             m256i_i16[16];
    __int32             m256i_i32[8];
    __int64             m256i_i64[4];
    unsigned __int8     m256i_u8[32];
    unsigned __int16    m256i_u16[16];
    unsigned __int32    m256i_u32[8];
    unsigned __int64    m256i_u64[4];
} __m256i;

/*
 * Compare predicates for scalar and packed compare intrinsics
 */
#define _CMP_EQ_OQ     0x00  /* Equal (ordered, nonsignaling)                       */
#define _CMP_LT_OS     0x01  /* Less-than (ordered, signaling)                      */
#define _CMP_LE_OS     0x02  /* Less-than-or-equal (ordered, signaling)             */
#define _CMP_UNORD_Q   0x03  /* Unordered (nonsignaling)                            */
#define _CMP_NEQ_UQ    0x04  /* Not-equal (unordered, nonsignaling)                 */
#define _CMP_NLT_US    0x05  /* Not-less-than (unordered, signaling)                */
#define _CMP_NLE_US    0x06  /* Not-less-than-or-equal (unordered, signaling)       */
#define _CMP_ORD_Q     0x07  /* Ordered (nonsignaling)                              */
#define _CMP_EQ_UQ     0x08  /* Equal (unordered, non-signaling)                    */
#define _CMP_NGE_US    0x09  /* Not-greater-than-or-equal (unordered, signaling)    */
#define _CMP_NGT_US    0x0A  /* Not-greater-than (unordered, signaling)             */
#define _CMP_FALSE_OQ  0x0B  /* False (ordered, nonsignaling)                       */
#define _CMP_NEQ_OQ    0x0C  /* Not-equal (ordered, non-signaling)                  */
#define _CMP_GE_OS     0x0D  /* Greater-than-or-equal (ordered, signaling)          */
#define _CMP_GT_OS     0x0E  /* Greater-than (ordered, signaling)                   */
#define _CMP_TRUE_UQ   0x0F  /* True (unordered, non-signaling)                     */
#define _CMP_EQ_OS     0x10  /* Equal (ordered, signaling)                          */
#define _CMP_LT_OQ     0x11  /* Less-than (ordered, nonsignaling)                   */
#define _CMP_LE_OQ     0x12  /* Less-than-or-equal (ordered, nonsignaling)          */
#define _CMP_UNORD_S   0x13  /* Unordered (signaling)                               */
#define _CMP_NEQ_US    0x14  /* Not-equal (unordered, signaling)                    */
#define _CMP_NLT_UQ    0x15  /* Not-less-than (unordered, nonsignaling)             */
#define _CMP_NLE_UQ    0x16  /* Not-less-than-or-equal (unordered, nonsignaling)    */
#define _CMP_ORD_S     0x17  /* Ordered (signaling)                                 */
#define _CMP_EQ_US     0x18  /* Equal (unordered, signaling)                        */
#define _CMP_NGE_UQ    0x19  /* Not-greater-than-or-equal (unordered, nonsignaling) */
#define _CMP_NGT_UQ    0x1A  /* Not-greater-than (unordered, nonsignaling)          */
#define _CMP_FALSE_OS  0x1B  /* False (ordered, signaling)                          */
#define _CMP_NEQ_OS    0x1C  /* Not-equal (ordered, signaling)                      */
#define _CMP_GE_OQ     0x1D  /* Greater-than-or-equal (ordered, nonsignaling)       */
#define _CMP_GT_OQ     0x1E  /* Greater-than (ordered, nonsignaling)                */
#define _CMP_TRUE_US   0x1F  /* True (unordered, signaling)                         */

/*
 * Add Packed Double Precision Floating-Point Values
 * **** VADDPD ymm1, ymm2, ymm3/m256
 * Performs an SIMD add of the four packed double-precision floating-point
 * values from the first source operand to the second source operand, and
 * stores the packed double-precision floating-point results in the
 * destination
 */
extern __m256d __cdecl _mm256_add_pd(__m256d m1, __m256d m2);

/*
 * Add Packed Single Precision Floating-Point Values
 * **** VADDPS ymm1, ymm2, ymm3/m256
 * Performs an SIMD add of the eight packed single-precision floating-point
 * values from the first source operand to the second source operand, and
 * stores the packed single-precision floating-point results in the
 * destination
 */
extern __m256 __cdecl _mm256_add_ps(__m256 m1, __m256 m2);

/*
 * Add/Subtract Double Precision Floating-Point Values
 * **** VADDSUBPD ymm1, ymm2, ymm3/m256
 * Adds odd-numbered double-precision floating-point values of the first
 * source operand with the corresponding double-precision floating-point
 * values from the second source operand; stores the result in the odd-numbered
 * values of the destination. Subtracts the even-numbered double-precision
 * floating-point values from the second source operand from the corresponding
 * double-precision floating values in the first source operand; stores the
 * result into the even-numbered values of the destination
 */
extern __m256d __cdecl _mm256_addsub_pd(__m256d m1, __m256d m2);

/*
 * Add/Subtract Packed Single Precision Floating-Point Values
 * **** VADDSUBPS ymm1, ymm2, ymm3/m256
 * Adds odd-numbered single-precision floating-point values of the first source
 * operand with the corresponding single-precision floating-point values from
 * the second source operand; stores the result in the odd-numbered values of
 * the destination. Subtracts the even-numbered single-precision floating-point
 * values from the second source operand from the corresponding
 * single-precision floating values in the first source operand; stores the
 * result into the even-numbered values of the destination
 */
extern __m256 __cdecl _mm256_addsub_ps(__m256 m1, __m256 m2);

/*
 * Bitwise Logical AND of Packed Double Precision Floating-Point Values
 * **** VANDPD ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical AND of the four packed double-precision
 * floating-point values from the first source operand and the second
 * source operand, and stores the result in the destination
 */
extern __m256d __cdecl _mm256_and_pd(__m256d m1, __m256d m2);

/*
 * Bitwise Logical AND of Packed Single Precision Floating-Point Values
 * **** VANDPS ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical AND of the eight packed single-precision
 * floating-point values from the first source operand and the second
 * source operand, and stores the result in the destination
 */
extern __m256 __cdecl _mm256_and_ps(__m256 m1, __m256 m2);

/*
 * Bitwise Logical AND NOT of Packed Double Precision Floating-Point Values
 * **** VANDNPD ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical AND NOT of the four packed double-precision
 * floating-point values from the first source operand and the second source
 * operand, and stores the result in the destination
 */
extern __m256d __cdecl _mm256_andnot_pd(__m256d m1, __m256d m2);

/*
 * Bitwise Logical AND NOT of Packed Single Precision Floating-Point Values
 * **** VANDNPS ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical AND NOT of the eight packed single-precision
 * floating-point values from the first source operand and the second source
 * operand, and stores the result in the destination
 */
extern __m256 __cdecl _mm256_andnot_ps(__m256 m1, __m256 m2);

/*
 * Blend Packed Double Precision Floating-Point Values
 * **** VBLENDPD ymm1, ymm2, ymm3/m256, imm8
 * Double-Precision Floating-Point values from the second source operand are
 * conditionally merged with values from the first source operand and written
 * to the destination. The immediate bits [3:0] determine whether the
 * corresponding Double-Precision Floating Point value in the destination is
 * copied from the second source or first source. If a bit in the mask,
 * orresponding to a word, is "1", then the Double-Precision Floating-Point
 * value in the second source operand is copied, else the value in the first
 * source operand is copied
 */
extern __m256d __cdecl _mm256_blend_pd(__m256d m1, __m256d m2, const int mask);

/*
 * Blend Packed Single Precision Floating-Point Values
 * **** VBLENDPS ymm1, ymm2, ymm3/m256, imm8
 * Single precision floating point values from the second source operand are
 * conditionally merged with values from the first source operand and written
 * to the destination. The immediate bits [7:0] determine whether the
 * corresponding single precision floating-point value in the destination is
 * copied from the second source or first source. If a bit in the mask,
 * corresponding to a word, is "1", then the single-precision floating-point
 * value in the second source operand is copied, else the value in the first
 * source operand is copied
 */
extern __m256 __cdecl _mm256_blend_ps(__m256 m1, __m256 m2, const int mask);

/*
 * Blend Packed Double Precision Floating-Point Values
 * **** VBLENDVPD ymm1, ymm2, ymm3/m256, ymm4
 * Conditionally copy each quadword data element of double-precision
 * floating-point value from the second source operand (third operand) and the
 * first source operand (second operand) depending on mask bits defined in the
 * mask register operand (fourth operand).
 */
extern __m256d __cdecl _mm256_blendv_pd(__m256d m1, __m256d m2, __m256d m3);

/*
 * Blend Packed Single Precision Floating-Point Values
 * **** VBLENDVPS ymm1, ymm2, ymm3/m256, ymm4
 * Conditionally copy each dword data element of single-precision
 * floating-point value from the second source operand (third operand) and the
 * first source operand (second operand) depending on mask bits defined in the
 * mask register operand (fourth operand).
 */
extern __m256 __cdecl _mm256_blendv_ps(__m256 m1, __m256 m2, __m256 mask);

/*
 * Divide Packed Double-Precision Floating-Point Values
 * **** VDIVPD ymm1, ymm2, ymm3/m256
 * Performs an SIMD divide of the four packed double-precision floating-point
 * values in the first source operand by the four packed double-precision
 * floating-point values in the second source operand
 */
extern __m256d __cdecl _mm256_div_pd(__m256d m1, __m256d m2);

/*
 * Divide Packed Single-Precision Floating-Point Values
 * **** VDIVPS ymm1, ymm2, ymm3/m256
 * Performs an SIMD divide of the eight packed single-precision
 * floating-point values in the first source operand by the eight packed
 * single-precision floating-point values in the second source operand
 */
extern __m256 __cdecl _mm256_div_ps(__m256 m1, __m256 m2);

/*
 * Dot Product of Packed Single-Precision Floating-Point Values
 * **** VDPPS ymm1, ymm2, ymm3/m256, imm8
 * Multiplies the packed single precision floating point values in the
 * first source operand with the packed single-precision floats in the
 * second source. Each of the four resulting single-precision values is
 * conditionally summed depending on a mask extracted from the high 4 bits
 * of the immediate operand. This sum is broadcast to each of 4 positions
 * in the destination if the corresponding bit of the mask selected from
 * the low 4 bits of the immediate operand is "1". If the corresponding
 * low bit 0-3 of the mask is zero, the destination is set to zero.
 * The process is replicated for the high elements of the destination.
 */
extern __m256 __cdecl _mm256_dp_ps(__m256 m1, __m256 m2, const int mask);

/*
 * Add Horizontal Double Precision Floating-Point Values
 * **** VHADDPD ymm1, ymm2, ymm3/m256
 * Adds pairs of adjacent double-precision floating-point values in the
 * first source operand and second source operand and stores results in
 * the destination
 */
extern __m256d __cdecl _mm256_hadd_pd(__m256d m1, __m256d m2);

/*
 * Add Horizontal Single Precision Floating-Point Values
 * **** VHADDPS ymm1, ymm2, ymm3/m256
 * Adds pairs of adjacent single-precision floating-point values in the
 * first source operand and second source operand and stores results in
 * the destination
 */
extern __m256 __cdecl _mm256_hadd_ps(__m256 m1, __m256 m2);

/*
 * Subtract Horizontal Double Precision Floating-Point Values
 * **** VHSUBPD ymm1, ymm2, ymm3/m256
 * Subtract pairs of adjacent double-precision floating-point values in
 * the first source operand and second source operand and stores results
 * in the destination
 */
extern __m256d __cdecl _mm256_hsub_pd(__m256d m1, __m256d m2);

/*
 * Subtract Horizontal Single Precision Floating-Point Values
 * **** VHSUBPS ymm1, ymm2, ymm3/m256
 * Subtract pairs of adjacent single-precision floating-point values in
 * the first source operand and second source operand and stores results
 * in the destination.
 */
extern __m256 __cdecl _mm256_hsub_ps(__m256 m1, __m256 m2);

/*
 * Maximum of Packed Double Precision Floating-Point Values
 * **** VMAXPD ymm1, ymm2, ymm3/m256
 * Performs an SIMD compare of the packed double-precision floating-point
 * values in the first source operand and the second source operand and
 * returns the maximum value for each pair of values to the destination
 */
extern __m256d __cdecl _mm256_max_pd(__m256d m1, __m256d m2);

/*
 * Maximum of Packed Single Precision Floating-Point Values
 * **** VMAXPS ymm1, ymm2, ymm3/m256
 * Performs an SIMD compare of the packed single-precision floating-point
 * values in the first source operand and the second source operand and
 * returns the maximum value for each pair of values to the destination
 */
extern __m256 __cdecl _mm256_max_ps(__m256 m1, __m256 m2);

/*
 * Minimum of Packed Double Precision Floating-Point Values
 * **** VMINPD ymm1, ymm2, ymm3/m256
 * Performs an SIMD compare of the packed double-precision floating-point
 * values in the first source operand and the second source operand and
 * returns the minimum value for each pair of values to the destination
 */
extern __m256d __cdecl _mm256_min_pd(__m256d m1, __m256d m2);

/*
 * Minimum of Packed Single Precision Floating-Point Values
 * **** VMINPS ymm1, ymm2, ymm3/m256
 * Performs an SIMD compare of the packed single-precision floating-point
 * values in the first source operand and the second source operand and
 * returns the minimum value for each pair of values to the destination
 */
extern __m256 __cdecl _mm256_min_ps(__m256 m1, __m256 m2);

/*
 * Multiply Packed Double Precision Floating-Point Values
 * **** VMULPD ymm1, ymm2, ymm3/m256
 * Performs a SIMD multiply of the four packed double-precision floating-point
 * values from the first Source operand to the Second Source operand, and
 * stores the packed double-precision floating-point results in the
 * destination
 */
extern __m256d __cdecl _mm256_mul_pd(__m256d m1, __m256d m2);

/*
 * Multiply Packed Single Precision Floating-Point Values
 * **** VMULPS ymm1, ymm2, ymm3/m256
 * Performs an SIMD multiply of the eight packed single-precision
 * floating-point values from the first source operand to the second source
 * operand, and stores the packed double-precision floating-point results in
 * the destination
 */
extern __m256 __cdecl _mm256_mul_ps(__m256 m1, __m256 m2);

/*
 * Bitwise Logical OR of Packed Double Precision Floating-Point Values
 * **** VORPD ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical OR of the four packed double-precision
 * floating-point values from the first source operand and the second
 * source operand, and stores the result in the destination
 */
extern __m256d __cdecl _mm256_or_pd(__m256d m1, __m256d m2);

/*
 * Bitwise Logical OR of Packed Single Precision Floating-Point Values
 * **** VORPS ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical OR of the eight packed single-precision
 * floating-point values from the first source operand and the second
 * source operand, and stores the result in the destination
 */
extern __m256 __cdecl _mm256_or_ps(__m256 m1, __m256 m2);

/*
 * Shuffle Packed Double Precision Floating-Point Values
 * **** VSHUFPD ymm1, ymm2, ymm3/m256, imm8
 * Moves either of the two packed double-precision floating-point values from
 * each double quadword in the first source operand into the low quadword
 * of each double quadword of the destination; moves either of the two packed
 * double-precision floating-point values from the second source operand into
 * the high quadword of each double quadword of the destination operand.
 * The selector operand determines which values are moved to the destination
 */
extern __m256d __cdecl _mm256_shuffle_pd(__m256d m1, __m256d m2, const int select);

/*
 * Shuffle Packed Single Precision Floating-Point Values
 * **** VSHUFPS ymm1, ymm2, ymm3/m256, imm8
 * Moves two of the four packed single-precision floating-point values
 * from each double qword of the first source operand into the low
 * quadword of each double qword of the destination; moves two of the four
 * packed single-precision floating-point values from each double qword of
 * the second source operand into to the high quadword of each double qword
 * of the destination. The selector operand determines which values are moved
 * to the destination.
 */
extern __m256 __cdecl _mm256_shuffle_ps(__m256 m1, __m256 m2, const int select);

/*
 * Subtract Packed Double Precision Floating-Point Values
 * **** VSUBPD ymm1, ymm2, ymm3/m256
 * Performs an SIMD subtract of the four packed double-precision floating-point
 * values of the second Source operand from the first Source operand, and
 * stores the packed double-precision floating-point results in the destination
 */
extern __m256d __cdecl _mm256_sub_pd(__m256d m1, __m256d m2);

/*
 * Subtract Packed Single Precision Floating-Point Values
 * **** VSUBPS ymm1, ymm2, ymm3/m256
 * Performs an SIMD subtract of the eight packed single-precision
 * floating-point values in the second Source operand from the First Source
 * operand, and stores the packed single-precision floating-point results in
 * the destination
 */
extern __m256 __cdecl _mm256_sub_ps(__m256 m1, __m256 m2);

/*
 * Bitwise Logical XOR of Packed Double Precision Floating-Point Values
 * **** VXORPD ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical XOR of the four packed double-precision
 * floating-point values from the first source operand and the second
 * source operand, and stores the result in the destination
 */
extern __m256d __cdecl _mm256_xor_pd(__m256d m1, __m256d m2);

/*
 * Bitwise Logical XOR of Packed Single Precision Floating-Point Values
 * **** VXORPS ymm1, ymm2, ymm3/m256
 * Performs a bitwise logical XOR of the eight packed single-precision
 * floating-point values from the first source operand and the second
 * source operand, and stores the result in the destination
 */
extern __m256 __cdecl _mm256_xor_ps(__m256 m1, __m256 m2);

/*
 * Compare Packed Double-Precision Floating-Point Values
 * **** VCMPPD xmm1, xmm2, xmm3/m128, imm8
 * **** VCMPPD ymm1, ymm2, ymm3/m256, imm8
 * Performs an SIMD compare of the four packed double-precision floating-point
 * values in the second source operand (third operand) and the first source
 * operand (second operand) and returns the results of the comparison to the
 * destination operand (first operand). The comparison predicate operand
 * (immediate) specifies the type of comparison performed on each of the pairs
 * of packed values.
 * For 128-bit intrinsic with compare predicate values in range 0-7 compiler
 * may generate SSE2 instructions if it is warranted for performance reasons.
 */
extern __m128d __cdecl _mm_cmp_pd(__m128d m1, __m128d m2, const int predicate);
extern __m256d __cdecl _mm256_cmp_pd(__m256d m1, __m256d m2, const int predicate);

/*
 * Compare Packed Single-Precision Floating-Point Values
 * **** VCMPPS xmm1, xmm2, xmm3/m256, imm8
 * **** VCMPPS ymm1, ymm2, ymm3/m256, imm8
 * Performs a SIMD compare of the packed single-precision floating-point values
 * in the second source operand (third operand) and the first source operand
 * (second operand) and returns the results of the comparison to the destination
 * operand (first operand). The comparison predicate operand (immediate)
 * specifies the type of comparison performed on each of the pairs of packed
 * values.
 * For 128-bit intrinsic with compare predicate values in range 0-7 compiler
 * may generate SSE2 instructions if it is warranted for performance reasons.
 */
extern __m128 __cdecl _mm_cmp_ps(__m128 m1, __m128 m2, const int predicate);
extern __m256 __cdecl _mm256_cmp_ps(__m256 m1, __m256 m2, const int predicate);

/*
 * Compare Scalar Double-Precision Floating-Point Values
 * **** VCMPSD xmm1, xmm2, xmm3/m64, imm8
 * Compares the low double-precision floating-point values in the second source
 * operand (third operand) and the first source operand (second operand) and
 * returns the results in of the comparison to the destination operand (first
 * operand). The comparison predicate operand (immediate operand) specifies the
 * type of comparison performed.
 * For compare predicate values in range 0-7 compiler may generate SSE2
 * instructions if it is warranted for performance reasons.
 */
extern __m128d __cdecl _mm_cmp_sd(__m128d m1, __m128d m2, const int predicate);

/*
 * Compare Scalar Single-Precision Floating-Point Values
 * **** VCMPSS xmm1, xmm2, xmm3/m64, imm8
 * Compares the low single-precision floating-point values in the second source
 * operand (third operand) and the first source operand (second operand) and
 * returns the results of the comparison to the destination operand (first
 * operand). The comparison predicate operand (immediate operand) specifies
 * the type of comparison performed.
 * For compare predicate values in range 0-7 compiler may generate SSE2
 * instructions if it is warranted for performance reasons.
 */
extern __m128 __cdecl _mm_cmp_ss(__m128 m1, __m128 m2, const int predicate);

/*
 * Convert Packed Doubleword Integers to
 * Packed Double-Precision Floating-Point Values
 * **** VCVTDQ2PD ymm1, xmm2/m128
 * Converts four packed signed doubleword integers in the source operand to
 * four packed double-precision floating-point values in the destination
 */
extern __m256d __cdecl _mm256_cvtepi32_pd(__m128i m1);

/*
 * Convert Packed Doubleword Integers to
 * Packed Single-Precision Floating-Point Values
 * **** VCVTDQ2PS ymm1, ymm2/m256
 * Converts eight packed signed doubleword integers in the source operand to
 * eight packed double-precision floating-point values in the destination
 */
extern __m256  __cdecl _mm256_cvtepi32_ps(__m256i m1);

/*
 * Convert Packed Double-Precision Floating-point values to
 * Packed Single-Precision Floating-Point Values
 * **** VCVTPD2PS xmm1, ymm2/m256
 * Converts four packed double-precision floating-point values in the source
 * operand to four packed single-precision floating-point values in the
 * destination
 */
extern __m128  __cdecl _mm256_cvtpd_ps(__m256d m1);

/*
 * Convert Packed Single Precision Floating-Point Values to
 * Packed Singed Doubleword Integer Values
 * **** VCVTPS2DQ ymm1, ymm2/m256
 * Converts eight packed single-precision floating-point values in the source
 * operand to eight signed doubleword integers in the destination
 */
extern __m256i __cdecl _mm256_cvtps_epi32(__m256 m1);

/*
 * Convert Packed Single Precision Floating-point values to
 * Packed Double Precision Floating-Point Values
 * **** VCVTPS2PD ymm1, xmm2/m128
 * Converts four packed single-precision floating-point values in the source
 * operand to four packed double-precision floating-point values in the
 * destination
 */
extern __m256d __cdecl _mm256_cvtps_pd(__m128 m1);

/*
 * Convert with Truncation Packed Double-Precision Floating-Point values to
 * Packed Doubleword Integers
 * **** VCVTTPD2DQ xmm1, ymm2/m256
 * Converts four packed double-precision floating-point values in the source
 * operand to four packed signed doubleword integers in the destination.
 * When a conversion is inexact, a truncated (round toward zero) value is
 * returned. If a converted result is larger than the maximum signed doubleword
 * integer, the floating-point invalid exception is raised, and if this
 * exception is masked, the indefinite integer value (80000000H) is returned
*/
extern __m128i __cdecl _mm256_cvttpd_epi32(__m256d m1);

/*
 * Convert Packed Double-Precision Floating-point values to
 * Packed Doubleword Integers
 * **** VCVTPD2DQ xmm1, ymm2/m256
 * Converts four packed double-precision floating-point values in the source
 * operand to four packed signed doubleword integers in the destination
 */
extern __m128i __cdecl _mm256_cvtpd_epi32(__m256d m1);

/*
 * Convert with Truncation Packed Single Precision Floating-Point Values to
 * Packed Singed Doubleword Integer Values
 * **** VCVTTPS2DQ ymm1, ymm2/m256
 * Converts eight packed single-precision floating-point values in the source
 * operand to eight signed doubleword integers in the destination.
 * When a conversion is inexact, a truncated (round toward zero) value is
 * returned. If a converted result is larger than the maximum signed doubleword
 * integer, the floating-point invalid exception is raised, and if this
 * exception is masked, the indefinite integer value (80000000H) is returned
 */
extern __m256i __cdecl _mm256_cvttps_epi32(__m256 m1);

/*
 * Extract packed floating-point values
 * **** VEXTRACTF128 xmm1/m128, ymm2, imm8
 * Extracts 128-bits of packed floating-point values from the source operand
 * at an 128-bit offset from imm8[0] into the destination
 */
extern __m128  __cdecl _mm256_extractf128_ps(__m256 m1, const int offset);
extern __m128d __cdecl _mm256_extractf128_pd(__m256d m1, const int offset);
extern __m128i __cdecl _mm256_extractf128_si256(__m256i m1, const int offset);

/*
 * Zero All YMM registers
 * **** VZEROALL
 * Zeros contents of all YMM registers
 */
extern void __cdecl _mm256_zeroall(void);

/*
 * Zero Upper bits of YMM registers
 * **** VZEROUPPER
 * Zeros the upper 128 bits of all YMM registers. The lower 128-bits of the
 * registers (the corresponding XMM registers) are unmodified
 */
extern void __cdecl _mm256_zeroupper(void);

/*
 * Permute Single-Precision Floating-Point Values
 * **** VPERMILPS ymm1, ymm2, ymm3/m256
 * **** VPERMILPS xmm1, xmm2, xmm3/m128
 * Permute Single-Precision Floating-Point values in the first source operand
 * using 8-bit control fields in the low bytes of corresponding elements the
 * shuffle control and store results in the destination
 */
extern __m256  __cdecl _mm256_permutevar_ps(__m256 m1, __m256i control);
extern __m128  __cdecl _mm_permutevar_ps(__m128 a, __m128i control);

/*
 * Permute Single-Precision Floating-Point Values
 * **** VPERMILPS ymm1, ymm2/m256, imm8
 * **** VPERMILPS xmm1, xmm2/m128, imm8
 * Permute Single-Precision Floating-Point values in the first source operand
 * using four 2-bit control fields in the 8-bit immediate and store results
 * in the destination
 */
extern __m256  __cdecl _mm256_permute_ps(__m256 m1, int control);
extern __m128  __cdecl _mm_permute_ps(__m128 a, int control);

/*
 * Permute Double-Precision Floating-Point Values
 * **** VPERMILPD ymm1, ymm2, ymm3/m256
 * **** VPERMILPD xmm1, xmm2, xmm3/m128
 * Permute Double-Precision Floating-Point values in the first source operand
 * using 8-bit control fields in the low bytes of the second source operand
 * and store results in the destination
 */
extern __m256d __cdecl _mm256_permutevar_pd(__m256d m1, __m256i control);
extern __m128d __cdecl _mm_permutevar_pd(__m128d a, __m128i control);

/*
 * Permute Double-Precision Floating-Point Values
 * **** VPERMILPD ymm1, ymm2/m256, imm8
 * **** VPERMILPD xmm1, xmm2/m128, imm8
 * Permute Double-Precision Floating-Point values in the first source operand
 * using two, 1-bit control fields in the low 2 bits of the 8-bit immediate
 * and store results in the destination
 */
extern __m256d __cdecl _mm256_permute_pd(__m256d m1, int control);
extern __m128d __cdecl _mm_permute_pd(__m128d a, int control);

/*
 * Permute Floating-Point Values
 * **** VPERM2F128 ymm1, ymm2, ymm3/m256, imm8
 * Permute 128 bit floating-point-containing fields from the first source
 * operand and second source operand using bits in the 8-bit immediate and
 * store results in the destination
 */
extern __m256  __cdecl _mm256_permute2f128_ps(__m256 m1, __m256 m2, int control);
extern __m256d __cdecl _mm256_permute2f128_pd(__m256d m1, __m256d m2, int control);
extern __m256i __cdecl _mm256_permute2f128_si256(__m256i m1, __m256i m2, int control);

/*
 * Load with Broadcast
 * **** VBROADCASTSS ymm1, m32
 * **** VBROADCASTSS xmm1, m32
 * Load floating point values from the source operand and broadcast to all
 * elements of the destination
 */
extern __m256  __cdecl _mm256_broadcast_ss(float const *a);
extern __m128  __cdecl _mm_broadcast_ss(float const *a);

/*
 * Load with Broadcast
 * **** VBROADCASTSD ymm1, m64
 * Load floating point values from the source operand and broadcast to all
 * elements of the destination
 */
extern __m256d __cdecl _mm256_broadcast_sd(double const *a);

/*
 * Load with Broadcast
 * **** VBROADCASTF128 ymm1, m128
 * Load floating point values from the source operand and broadcast to all
 * elements of the destination
 */
extern __m256  __cdecl _mm256_broadcast_ps(__m128 const *a);
extern __m256d __cdecl _mm256_broadcast_pd(__m128d const *a);

/*
 * Insert packed floating-point values
 * **** VINSERTF128 ymm1, ymm2, xmm3/m128, imm8
 * Performs an insertion of 128-bits of packed floating-point values from the
 * second source operand into an the destination at an 128-bit offset from
 * imm8[0]. The remaining portions of the destination are written by the
 * corresponding fields of the first source operand
 */
extern __m256  __cdecl _mm256_insertf128_ps(__m256, __m128 a, int offset);
extern __m256d __cdecl _mm256_insertf128_pd(__m256d, __m128d a, int offset);
extern __m256i __cdecl _mm256_insertf128_si256(__m256i, __m128i a, int offset);

/*
 * Move Aligned Packed Double-Precision Floating-Point Values
 * **** VMOVAPD ymm1, m256
 * **** VMOVAPD m256, ymm1
 * Moves 4 double-precision floating-point values from the source operand to
 * the destination
 */
extern __m256d __cdecl _mm256_load_pd(double const *a);
extern void    __cdecl _mm256_store_pd(double *a, __m256d b);

/*
 * Move Aligned Packed Single-Precision Floating-Point Values
 * **** VMOVAPS ymm1, m256
 * **** VMOVAPS m256, ymm1
 * Moves 8 single-precision floating-point values from the source operand to
 * the destination
 */
extern __m256  __cdecl _mm256_load_ps(float const *a);
extern void    __cdecl _mm256_store_ps(float *a, __m256 b);

/*
 * Move Unaligned Packed Double-Precision Floating-Point Values
 * **** VMOVUPD ymm1, m256
 * **** VMOVUPD m256, ymm1
 * Moves 256 bits of packed double-precision floating-point values from the
 * source operand to the destination
 */
extern __m256d __cdecl _mm256_loadu_pd(double const *a);
extern void    __cdecl _mm256_storeu_pd(double *a, __m256d b);

/*
 * Move Unaligned Packed Single-Precision Floating-Point Values
 * **** VMOVUPS ymm1, m256
 * **** VMOVUPS m256, ymm1
 * Moves 256 bits of packed single-precision floating-point values from the
 * source operand to the destination
 */
extern __m256  __cdecl _mm256_loadu_ps(float const *a);
extern void    __cdecl _mm256_storeu_ps(float *a, __m256 b);

/*
 * Move Aligned Packed Integer Values
 * **** VMOVDQA ymm1, m256
 * **** VMOVDQA m256, ymm1
 * Moves 256 bits of packed integer values from the source operand to the
 * destination
 */
extern __m256i __cdecl _mm256_load_si256(__m256i const *a);
extern void    __cdecl _mm256_store_si256(__m256i *a, __m256i b);

/*
 * Move Unaligned Packed Integer Values
 * **** VMOVDQU ymm1, m256
 * **** VMOVDQU m256, ymm1
 * Moves 256 bits of packed integer values from the source operand to the
 * destination
 */
extern __m256i __cdecl _mm256_loadu_si256(__m256i const *a);
extern void    __cdecl _mm256_storeu_si256(__m256i *a, __m256i b);

/*
 * Conditional SIMD Packed Loads and Stores
 * **** VMASKMOVPD xmm1, xmm2, m128
 * **** VMASKMOVPD ymm1, ymm2, m256
 * **** VMASKMOVPD m128, xmm1, xmm2
 * **** VMASKMOVPD m256, ymm1, ymm2
 *
 * Load forms:
 * Load packed values from the 128-bit (XMM forms) or 256-bit (YMM forms)
 * memory location (third operand) into the destination XMM or YMM register
 * (first operand) using a mask in the first source operand (second operand).
 *
 * Store forms:
 * Stores packed values from the XMM or YMM register in the second source
 * operand (third operand) into the 128-bit (XMM forms) or 256-bit (YMM forms)
 * memory location using a mask in first source operand (second operand).
 * Stores are atomic.
 */
extern __m256d __cdecl _mm256_maskload_pd(double const *a, __m256i mask);
extern void    __cdecl _mm256_maskstore_pd(double *a, __m256i mask, __m256d b);
extern __m128d __cdecl _mm_maskload_pd(double const *a, __m128i mask);
extern void    __cdecl _mm_maskstore_pd(double *a, __m128i mask, __m128d b);

/*
 * Conditional SIMD Packed Loads and Stores
 * **** VMASKMOVPS xmm1, xmm2, m128
 * **** VMASKMOVPS ymm1, ymm2, m256
 * **** VMASKMOVPS m128, xmm1, xmm2
 * **** VMASKMOVPS m256, ymm1, ymm2
 *
 * Load forms:
 * Load packed values from the 128-bit (XMM forms) or 256-bit (YMM forms)
 * memory location (third operand) into the destination XMM or YMM register
 * (first operand) using a mask in the first source operand (second operand).
 *
 * Store forms:
 * Stores packed values from the XMM or YMM register in the second source
 * operand (third operand) into the 128-bit (XMM forms) or 256-bit (YMM forms)
 * memory location using a mask in first source operand (second operand).
 * Stores are atomic.
 */
extern __m256  __cdecl _mm256_maskload_ps(float const *a, __m256i mask);
extern void    __cdecl _mm256_maskstore_ps(float *a, __m256i mask, __m256 b);
extern __m128  __cdecl _mm_maskload_ps(float const *a, __m128i mask);
extern void    __cdecl _mm_maskstore_ps(float *a, __m128i mask, __m128 b);

/*
 * Replicate Single-Precision Floating-Point Values
 * **** VMOVSHDUP ymm1, ymm2/m256
 * Duplicates odd-indexed single-precision floating-point values from the
 * source operand
 */
extern __m256  __cdecl _mm256_movehdup_ps(__m256 a);

/*
 * Replicate Single-Precision Floating-Point Values
 * **** VMOVSLDUP ymm1, ymm2/m256
 * Duplicates even-indexed single-precision floating-point values from the
 * source operand
 */
extern __m256  __cdecl _mm256_moveldup_ps(__m256 a);

/*
 * Replicate Double-Precision Floating-Point Values
 * **** VMOVDDUP ymm1, ymm2/m256
 * Duplicates even-indexed double-precision floating-point values from the
 * source operand
 */
extern __m256d __cdecl _mm256_movedup_pd(__m256d a);

/*
 * Move Unaligned Integer
 * **** VLDDQU ymm1, m256
 * The instruction is functionally similar to VMOVDQU YMM, m256 for loading
 * from memory. That is: 32 bytes of data starting at an address specified by
 * the source memory operand are fetched from memory and placed in a
 * destination
 */
extern __m256i __cdecl _mm256_lddqu_si256(__m256i const *a);

/*
 * Store Packed Integers Using Non-Temporal Hint
 * **** VMOVNTDQ m256, ymm1
 * Moves the packed integers in the source operand to the destination using a
 * non-temporal hint to prevent caching of the data during the write to memory
 */
extern void    __cdecl _mm256_stream_si256(__m256i *p, __m256i a);

/*
 * Store Packed Double-Precision Floating-Point Values Using Non-Temporal Hint
 * **** VMOVNTPD m256, ymm1
 * Moves the packed double-precision floating-point values in the source
 * operand to the destination operand using a non-temporal hint to prevent
 * caching of the data during the write to memory
 */
extern void    __cdecl _mm256_stream_pd(double *p, __m256d a);

/*
 * Store Packed Single-Precision Floating-Point Values Using Non-Temporal Hint
 * **** VMOVNTPS m256, ymm1
 * Moves the packed single-precision floating-point values in the source
 * operand to the destination operand using a non-temporal hint to prevent
 * caching of the data during the write to memory
 */
extern void    __cdecl _mm256_stream_ps(float *p, __m256 a);

/*
 * Compute Approximate Reciprocals of Packed Single-Precision Floating-Point Values
 * **** VRCPPS ymm1, ymm2/m256
 * Performs an SIMD computation of the approximate reciprocals of the eight
 * packed single precision floating-point values in the source operand and
 * stores the packed single-precision floating-point results in the destination
 */
extern __m256  __cdecl _mm256_rcp_ps(__m256 a);

/*
 * Compute Approximate Reciprocals of Square Roots of
 * Packed Single-Precision Floating-point Values
 * **** VRSQRTPS ymm1, ymm2/m256
 * Performs an SIMD computation of the approximate reciprocals of the square
 * roots of the eight packed single precision floating-point values in the
 * source operand and stores the packed single-precision floating-point results
 * in the destination
 */
extern __m256  __cdecl _mm256_rsqrt_ps(__m256 a);

/*
 * Square Root of Double-Precision Floating-Point Values
 * **** VSQRTPD ymm1, ymm2/m256
 * Performs an SIMD computation of the square roots of the two or four packed
 * double-precision floating-point values in the source operand and stores
 * the packed double-precision floating-point results in the destination
 */
extern __m256d __cdecl _mm256_sqrt_pd(__m256d a);

/*
 * Square Root of Single-Precision Floating-Point Values
 * **** VSQRTPS ymm1, ymm2/m256
 * Performs an SIMD computation of the square roots of the eight packed
 * single-precision floating-point values in the source operand stores the
 * packed double-precision floating-point results in the destination
 */
extern __m256  __cdecl _mm256_sqrt_ps(__m256 a);

/*
 * Round Packed Double-Precision Floating-Point Values
 * **** VROUNDPD ymm1,ymm2/m256,imm8
 * Round the four Double-Precision Floating-Point Values values in the source
 * operand by the rounding mode specified in the immediate operand and place
 * the result in the destination. The rounding process rounds the input to an
 * integral value and returns the result as a double-precision floating-point
 * value. The Precision Floating Point Exception is signaled according to the
 * immediate operand. If any source operand is an SNaN then it will be
 * converted to a QNaN.
 */
extern __m256d __cdecl _mm256_round_pd(__m256d a, int iRoundMode);
#define _mm256_ceil_pd(val)   _mm256_round_pd((val), 0x0A);
#define _mm256_floor_pd(val)  _mm256_round_pd((val), 0x09);

/*
 * Round Packed Single-Precision Floating-Point Values
 * **** VROUNDPS ymm1,ymm2/m256,imm8
 * Round the four single-precision floating-point values values in the source
 * operand by the rounding mode specified in the immediate operand and place
 * the result in the destination. The rounding process rounds the input to an
 * integral value and returns the result as a double-precision floating-point
 * value. The Precision Floating Point Exception is signaled according to the
 * immediate operand. If any source operand is an SNaN then it will be
 * converted to a QNaN.
 */
extern __m256  __cdecl _mm256_round_ps(__m256 a, int iRoundMode);
#define _mm256_ceil_ps(val)   _mm256_round_ps((val), 0x0A);
#define _mm256_floor_ps(val)  _mm256_round_ps((val), 0x09);

/*
 * Unpack and Interleave High Packed Double-Precision Floating-Point Values
 * **** VUNPCKHPD ymm1,ymm2,ymm3/m256
 * Performs an interleaved unpack of the high double-precision floating-point
 * values from the first source operand and the second source operand.
 */
extern __m256d __cdecl _mm256_unpackhi_pd(__m256d m1, __m256d m2);

/*
 * Unpack and Interleave High Packed Single-Precision Floating-Point Values
 * **** VUNPCKHPS ymm1,ymm2,ymm3
 * Performs an interleaved unpack of the high single-precision floating-point
 * values from the first source operand and the second source operand
 */
extern __m256  __cdecl _mm256_unpackhi_ps(__m256 m1, __m256 m2);

/*
 * Unpack and Interleave Low Packed Double-Precision Floating-Point Values
 * **** VUNPCKLPD ymm1,ymm2,ymm3/m256
 * Performs an interleaved unpack of the low double-precision floating-point
 * values from the first source operand and the second source operand
 */
extern __m256d __cdecl _mm256_unpacklo_pd(__m256d m1, __m256d m2);

/*
 * Unpack and Interleave Low Packed Single-Precision Floating-Point Values
 * **** VUNPCKLPS ymm1,ymm2,ymm3
 * Performs an interleaved unpack of the low single-precision floating-point
 * values from the first source operand and the second source operand
 */
extern __m256  __cdecl _mm256_unpacklo_ps(__m256 m1, __m256 m2);

/*
 * Packed Bit Test
 * **** VPTEST ymm1, ymm2/m256
 * VPTEST set the ZF flag if all bits in the result are 0 of the bitwise AND
 * of the first source operand and the second source operand. VPTEST sets the
 * CF flag if all bits in the result are 0 of the bitwise AND of the second
 * source operand and the logical NOT of the destination.
 */
extern int     __cdecl _mm256_testz_si256(__m256i s1, __m256i s2);
extern int     __cdecl _mm256_testc_si256(__m256i s1, __m256i s2);
extern int     __cdecl _mm256_testnzc_si256(__m256i s1, __m256i s2);

/*
 * Packed Bit Test
 * **** VTESTPD ymm1, ymm2/m256
 * **** VTESTPD xmm1, xmm2/m128
 * VTESTPD performs a bitwise comparison of all the sign bits of the
 * double-precision elements in the first source operation and corresponding
 * sign bits in the second source operand. If the AND of the two sets of bits
 * produces all zeros, the ZF is set else the ZF is clear. If the AND NOT of
 * the source sign bits with the dest sign bits produces all zeros the CF is
 * set else the CF is clear
 */
extern int     __cdecl _mm256_testz_pd(__m256d s1, __m256d s2);
extern int     __cdecl _mm256_testc_pd(__m256d s1, __m256d s2);
extern int     __cdecl _mm256_testnzc_pd(__m256d s1, __m256d s2);
extern int     __cdecl _mm_testz_pd(__m128d s1, __m128d s2);
extern int     __cdecl _mm_testc_pd(__m128d s1, __m128d s2);
extern int     __cdecl _mm_testnzc_pd(__m128d s1, __m128d s2);

/*
 * Packed Bit Test
 * **** VTESTPS ymm1, ymm2/m256
 * **** VTESTPS xmm1, xmm2/m128
 * VTESTPS performs a bitwise comparison of all the sign bits of the packed
 * single-precision elements in the first source operation and corresponding
 * sign bits in the second source operand. If the AND of the two sets of bits
 * produces all zeros, the ZF is set else the ZF is clear. If the AND NOT of
 * the source sign bits with the dest sign bits produces all zeros the CF is
 * set else the CF is clear
 */
extern int     __cdecl _mm256_testz_ps(__m256 s1, __m256 s2);
extern int     __cdecl _mm256_testc_ps(__m256 s1, __m256 s2);
extern int     __cdecl _mm256_testnzc_ps(__m256 s1, __m256 s2);
extern int     __cdecl _mm_testz_ps(__m128 s1, __m128 s2);
extern int     __cdecl _mm_testc_ps(__m128 s1, __m128 s2);
extern int     __cdecl _mm_testnzc_ps(__m128 s1, __m128 s2);

/*
 * Extract Double-Precision Floating-Point Sign mask
 * **** VMOVMSKPD r32, ymm2
 * Extracts the sign bits from the packed double-precision floating-point
 * values in the source operand, formats them into a 4-bit mask, and stores
 * the mask in the destination
 */
extern int     __cdecl _mm256_movemask_pd(__m256d a);

/*
 * Extract Single-Precision Floating-Point Sign mask
 * **** VMOVMSKPS r32, ymm2
 * Extracts the sign bits from the packed single-precision floating-point
 * values in the source operand, formats them into a 8-bit mask, and stores
 * the mask in the destination
 */
extern int     __cdecl _mm256_movemask_ps(__m256 a);

/*
 * Return 256-bit vector with all elements set to 0
 */
extern __m256d __cdecl _mm256_setzero_pd(void);
extern __m256  __cdecl _mm256_setzero_ps(void);
extern __m256i __cdecl _mm256_setzero_si256(void);

/*
 * Return 256-bit vector intialized to specified arguments
 */
extern __m256d __cdecl _mm256_set_pd(double, double, double, double);
extern __m256  __cdecl _mm256_set_ps(float, float, float, float, float, float, float, float);
extern __m256i __cdecl _mm256_set_epi8(char, char, char, char, char, char, char, char,
                                       char, char, char, char, char, char, char, char,
                                       char, char, char, char, char, char, char, char,
                                       char, char, char, char, char, char, char, char);
extern __m256i __cdecl _mm256_set_epi16(short, short, short, short, short, short, short, short,
                                        short, short, short, short, short, short, short, short);
extern __m256i __cdecl _mm256_set_epi32(int, int, int, int, int, int, int, int);
extern __m256i __cdecl _mm256_set_epi64x(long long, long long, long long, long long);

extern __m256d __cdecl _mm256_setr_pd(double, double, double, double);
extern __m256  __cdecl _mm256_setr_ps(float, float, float, float, float, float, float, float);
extern __m256i __cdecl _mm256_setr_epi8(char, char, char, char, char, char, char, char,
                                        char, char, char, char, char, char, char, char,
                                        char, char, char, char, char, char, char, char,
                                        char, char, char, char, char, char, char, char);
extern __m256i __cdecl _mm256_setr_epi16(short, short, short, short, short, short, short, short,
                                         short, short, short, short, short, short, short, short);
extern __m256i __cdecl _mm256_setr_epi32(int, int, int, int, int, int, int, int);
extern __m256i __cdecl _mm256_setr_epi64x(long long, long long, long long, long long);

/*
 * Return 256-bit vector with all elements intialized to specified scalar
 */
extern __m256d __cdecl _mm256_set1_pd(double);
extern __m256  __cdecl _mm256_set1_ps(float);
extern __m256i __cdecl _mm256_set1_epi8(char);
extern __m256i __cdecl _mm256_set1_epi16(short);
extern __m256i __cdecl _mm256_set1_epi32(int);
extern __m256i __cdecl _mm256_set1_epi64x(long long);

/*
 * Support intrinsics to do vector type casts. These intrinsics do not introduce
 * extra moves to generated code. When cast is done from a 128 to 256-bit type
 * the low 128 bits of the 256-bit result contain source parameter value; the
 * upper 128 bits of the result are undefined
 */
extern __m256  __cdecl _mm256_castpd_ps(__m256d a);
extern __m256d __cdecl _mm256_castps_pd(__m256 a);
extern __m256i __cdecl _mm256_castps_si256(__m256 a);
extern __m256i __cdecl _mm256_castpd_si256(__m256d a);
extern __m256  __cdecl _mm256_castsi256_ps(__m256i a);
extern __m256d __cdecl _mm256_castsi256_pd(__m256i a);
extern __m128  __cdecl _mm256_castps256_ps128(__m256 a);
extern __m128d __cdecl _mm256_castpd256_pd128(__m256d a);
extern __m128i __cdecl _mm256_castsi256_si128(__m256i a);
extern __m256  __cdecl _mm256_castps128_ps256(__m128 a);
extern __m256d __cdecl _mm256_castpd128_pd256(__m128d a);
extern __m256i __cdecl _mm256_castsi128_si256(__m128i a);

#if defined __cplusplus
}; /* End "C" */
#endif  /* defined __cplusplus */

#endif  /* defined (_M_CEE_PURE) */

#endif  /* _INCLUDED_IMM */
#endif  /* __midl */

// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef BVH_ToolsSIMD_AVX512_HeaderFile
#define BVH_ToolsSIMD_AVX512_HeaderFile

#include <BVH_SIMDDispatch.hxx>

// AVX-512 path: per-function target attribute on GCC/Clang only. MSVC needs
// a separate .cxx with /arch:AVX512 (a build wrapper not yet provided here).
#if defined(__x86_64__) || defined(_M_X64)
  #if defined(__GNUC__) || defined(__clang__)
    #define BVH_HAS_AVX512_KERNEL 1
    #include <immintrin.h>
  #endif
#endif

namespace BVH
{
namespace SIMD
{

#if defined(BVH_HAS_AVX512_KERNEL)

//! AVX-512 implementation of the 1-ray-vs-4-AABB slab test.
//!
//! Stays on __m128 lanes -- a true BVH16 would be needed to fill __m512.
//! The win over AVX2 at BVH4 fan-out is modest (~5-15%): AVX-512VL provides
//! native mask-register comparisons (__mmask8) so the final hit-mask comes
//! out as a register write rather than a movemask + shuffle dance, and the
//! AVX-512 EVEX encoding offers slightly better scheduling. For workloads
//! that actually scale (BVH8/BVH16, or wide ray-packets) the gain would be
//! 4-8x; here this kernel is mainly a placeholder so AVX-512 hosts do not
//! fall back to AVX2/SSE2 when the dispatch table includes it.
//!
//! Slab formula intentionally matches SSE2/AVX2 -- (box-origin)*invDir, not
//! the FMA form -- because parallel-ray NaN propagation differs between
//! formulations and the random-consistency tests pin the behaviour to the
//! scalar reference.
__attribute__((target("avx512f,avx512vl"))) inline int RayBox4_AVX512(const BVH_Ray4f_Splat& theRay,
                                                                      const BVH_Box4f_SoA& theBoxes,
                                                                      float* theOutTEnter,
                                                                      float* theOutTLeave) noexcept
{
  const __m128 anOx  = _mm_loadu_ps(theRay.ox);
  const __m128 anOy  = _mm_loadu_ps(theRay.oy);
  const __m128 anOz  = _mm_loadu_ps(theRay.oz);
  const __m128 anIdx = _mm_loadu_ps(theRay.idx);
  const __m128 anIdy = _mm_loadu_ps(theRay.idy);
  const __m128 anIdz = _mm_loadu_ps(theRay.idz);

  const __m128 aMinX = _mm_loadu_ps(theBoxes.minX);
  const __m128 aMaxX = _mm_loadu_ps(theBoxes.maxX);
  const __m128 aMinY = _mm_loadu_ps(theBoxes.minY);
  const __m128 aMaxY = _mm_loadu_ps(theBoxes.maxY);
  const __m128 aMinZ = _mm_loadu_ps(theBoxes.minZ);
  const __m128 aMaxZ = _mm_loadu_ps(theBoxes.maxZ);

  const __m128 aT1x = _mm_mul_ps(_mm_sub_ps(aMinX, anOx), anIdx);
  const __m128 aT2x = _mm_mul_ps(_mm_sub_ps(aMaxX, anOx), anIdx);
  const __m128 aT1y = _mm_mul_ps(_mm_sub_ps(aMinY, anOy), anIdy);
  const __m128 aT2y = _mm_mul_ps(_mm_sub_ps(aMaxY, anOy), anIdy);
  const __m128 aT1z = _mm_mul_ps(_mm_sub_ps(aMinZ, anOz), anIdz);
  const __m128 aT2z = _mm_mul_ps(_mm_sub_ps(aMaxZ, anOz), anIdz);

  const __m128 aTminX = _mm_min_ps(aT1x, aT2x);
  const __m128 aTmaxX = _mm_max_ps(aT1x, aT2x);
  const __m128 aTminY = _mm_min_ps(aT1y, aT2y);
  const __m128 aTmaxY = _mm_max_ps(aT1y, aT2y);
  const __m128 aTminZ = _mm_min_ps(aT1z, aT2z);
  const __m128 aTmaxZ = _mm_max_ps(aT1z, aT2z);

  const __m128 aTEnter = _mm_max_ps(_mm_max_ps(aTminY, aTminZ), aTminX);
  const __m128 aTLeave = _mm_min_ps(_mm_min_ps(aTmaxY, aTmaxZ), aTmaxX);

  // AVX-512VL: produce the hit predicate directly into a mask register.
  // Two CMPPS-into-mask ops + KAND give the 4-bit hit mask without the
  // movemask round-trip the SSE2/AVX2 paths need.
  const __mmask8 aLeaveOk  = _mm_cmp_ps_mask(aTLeave, _mm_setzero_ps(), _CMP_GE_OQ);
  const __mmask8 aIntervOk = _mm_cmp_ps_mask(aTEnter, aTLeave, _CMP_LE_OQ);
  // AND of mask registers via plain operator& (avoids the AVX-512DQ-only
  // _kand_mask8 intrinsic; __mmask8 is an integer alias).
  const __mmask8 aHitMask = static_cast<__mmask8>(aLeaveOk & aIntervOk);

  _mm_storeu_ps(theOutTEnter, aTEnter);
  _mm_storeu_ps(theOutTLeave, aTLeave);
  return static_cast<int>(aHitMask) & 0xF;
}

//! AVX-512 BVH16 kernel: 1 ray vs 16 AABBs, true 512-bit zmm pass.
//!
//! This is the kernel BVH16 was built for -- one __m512 holds all 16 lanes,
//! so a single slab pass exhausts the AVX-512 register width with no waste.
//! Unlike the (now-removed) BVH8 AVX-512 kernel that used __m256 + mask
//! tricks to save a movemask round-trip (and gained only "a few cycles"
//! per call), this kernel uses zmm intrinsics throughout so per-lane work
//! genuinely halves vs BVH8/AVX2.
//!
//! Same sign-based corner select as RayBoxN_AVX2_8 (tavianator), now with
//! _mm512_mask_blend_ps driven by a sign-extracted __mmask16. Final hit
//! predicate goes straight into __mmask16 via _mm512_cmp_ps_mask, no
//! movemask needed.
//!
//! Keep the same NaN-absorbing operand order as the AVX2 BVH8 path
//! (parallel-axis NaN dominated by finite results of the other two axes).
__attribute__((target("avx512f"))) inline int RayBoxN_AVX512_16(const BVH_RayNf_Splat<16>& theRay,
                                                                const BVH_BoxNf_SoA<16>&   theBoxes,
                                                                float* theOutTEnter,
                                                                float* theOutTLeave) noexcept
{
  const __m512 anOx  = _mm512_loadu_ps(theRay.ox);
  const __m512 anOy  = _mm512_loadu_ps(theRay.oy);
  const __m512 anOz  = _mm512_loadu_ps(theRay.oz);
  const __m512 anIdx = _mm512_loadu_ps(theRay.idx);
  const __m512 anIdy = _mm512_loadu_ps(theRay.idy);
  const __m512 anIdz = _mm512_loadu_ps(theRay.idz);

  const __m512 aMinX = _mm512_loadu_ps(theBoxes.minX);
  const __m512 aMaxX = _mm512_loadu_ps(theBoxes.maxX);
  const __m512 aMinY = _mm512_loadu_ps(theBoxes.minY);
  const __m512 aMaxY = _mm512_loadu_ps(theBoxes.maxY);
  const __m512 aMinZ = _mm512_loadu_ps(theBoxes.minZ);
  const __m512 aMaxZ = _mm512_loadu_ps(theBoxes.maxZ);

  // sign-extract via cmp-against-zero -> __mmask16; bits set on negative idx.
  // _mm512_mask_blend_ps(mask, A, B) returns B for set bits, A otherwise.
  const __mmask16 aNegX = _mm512_cmp_ps_mask(anIdx, _mm512_setzero_ps(), _CMP_LT_OQ);
  const __mmask16 aNegY = _mm512_cmp_ps_mask(anIdy, _mm512_setzero_ps(), _CMP_LT_OQ);
  const __mmask16 aNegZ = _mm512_cmp_ps_mask(anIdz, _mm512_setzero_ps(), _CMP_LT_OQ);

  // For positive idx: near = min, far = max. For negative idx: near = max, far = min.
  const __m512 aNearX = _mm512_mask_blend_ps(aNegX, aMinX, aMaxX);
  const __m512 aFarX  = _mm512_mask_blend_ps(aNegX, aMaxX, aMinX);
  const __m512 aNearY = _mm512_mask_blend_ps(aNegY, aMinY, aMaxY);
  const __m512 aFarY  = _mm512_mask_blend_ps(aNegY, aMaxY, aMinY);
  const __m512 aNearZ = _mm512_mask_blend_ps(aNegZ, aMinZ, aMaxZ);
  const __m512 aFarZ  = _mm512_mask_blend_ps(aNegZ, aMaxZ, aMinZ);

  const __m512 aTNearX = _mm512_mul_ps(_mm512_sub_ps(aNearX, anOx), anIdx);
  const __m512 aTFarX  = _mm512_mul_ps(_mm512_sub_ps(aFarX, anOx), anIdx);
  const __m512 aTNearY = _mm512_mul_ps(_mm512_sub_ps(aNearY, anOy), anIdy);
  const __m512 aTFarY  = _mm512_mul_ps(_mm512_sub_ps(aFarY, anOy), anIdy);
  const __m512 aTNearZ = _mm512_mul_ps(_mm512_sub_ps(aNearZ, anOz), anIdz);
  const __m512 aTFarZ  = _mm512_mul_ps(_mm512_sub_ps(aFarZ, anOz), anIdz);

  // Operand order matches the AVX2 BVH8 / SSE2 BVH4 kernels for identical
  // NaN propagation: parallel-axis NaN dominated by finite lanes from the
  // other two axes via "min/max returns the second operand on NaN".
  const __m512 aTEnter = _mm512_max_ps(_mm512_max_ps(aTNearY, aTNearZ), aTNearX);
  const __m512 aTLeave = _mm512_min_ps(_mm512_min_ps(aTFarY, aTFarZ), aTFarX);

  // Hit predicate straight into __mmask16, no movemask needed.
  const __mmask16 aLeaveOk  = _mm512_cmp_ps_mask(aTLeave, _mm512_setzero_ps(), _CMP_GE_OQ);
  const __mmask16 aIntervOk = _mm512_cmp_ps_mask(aTEnter, aTLeave, _CMP_LE_OQ);
  const __mmask16 aHitMask  = static_cast<__mmask16>(aLeaveOk & aIntervOk);

  _mm512_storeu_ps(theOutTEnter, aTEnter);
  _mm512_storeu_ps(theOutTLeave, aTLeave);
  return static_cast<int>(aHitMask);
}

#endif // BVH_HAS_AVX512_KERNEL

} // namespace SIMD
} // namespace BVH

#endif // BVH_ToolsSIMD_AVX512_HeaderFile

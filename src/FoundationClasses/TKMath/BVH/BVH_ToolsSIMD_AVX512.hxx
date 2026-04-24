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

#endif // BVH_HAS_AVX512_KERNEL

} // namespace SIMD
} // namespace BVH

#endif // BVH_ToolsSIMD_AVX512_HeaderFile

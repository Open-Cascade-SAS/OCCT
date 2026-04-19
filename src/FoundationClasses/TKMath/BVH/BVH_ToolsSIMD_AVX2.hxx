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

#ifndef BVH_ToolsSIMD_AVX2_HeaderFile
#define BVH_ToolsSIMD_AVX2_HeaderFile

#include <BVH_SIMDDispatch.hxx>

// AVX2+FMA path is available on GCC/Clang when the target architecture supports
// per-function attributes; on MSVC the same kernel must live in a .cxx compiled
// with /arch:AVX2 (a build wrapper not yet provided in this commit).
#if defined(__x86_64__) || defined(_M_X64)
  #if defined(__GNUC__) || defined(__clang__)
    #define BVH_HAS_AVX2_KERNEL 1
    #include <immintrin.h>
  #endif
#endif

namespace BVH
{
namespace SIMD
{

#if defined(BVH_HAS_AVX2_KERNEL)

//! AVX2 implementation of the 1-ray-vs-4-AABB slab test.
//!
//! Uses BVH4-sized __m128 lanes (the upper 128 bits of __m256 would be wasted
//! at this fan-out). The slab formula is the same as SSE2 -- t = (box-origin)
//! * invDir, NOT t = invDir*box - invDir*origin -- because FMA's
//! "multiply-then-subtract" order produces 0*inf = NaN for parallel rays
//! on lanes whose box face touches origin, which would diverge from the
//! scalar reference. The win over SSE2 comes from VEX 3-operand encoding
//! and the more flexible scheduling enabled by the AVX2 target attribute,
//! not from FMA.
//!
//! Function-level target attribute keeps the rest of the translation unit
//! buildable on hosts without AVX2; the dispatcher only routes to this
//! kernel after runtime CPU detection (BVH_SIMDDispatch::Detect) confirms
//! AVX2 + OS XSAVE support are present.
__attribute__((target("avx2,fma")))
inline int RayBox4_AVX2(const BVH_Ray4f_Splat& theRay,
                        const BVH_Box4f_SoA&   theBoxes,
                        float*                 theOutTEnter,
                        float*                 theOutTLeave) noexcept
{
  const __m128 anOx = _mm_loadu_ps(theRay.ox);
  const __m128 anOy = _mm_loadu_ps(theRay.oy);
  const __m128 anOz = _mm_loadu_ps(theRay.oz);
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

  // Same NaN-absorbing operand order as the SSE2 path.
  const __m128 aTEnter = _mm_max_ps(_mm_max_ps(aTminY, aTminZ), aTminX);
  const __m128 aTLeave = _mm_min_ps(_mm_min_ps(aTmaxY, aTmaxZ), aTmaxX);

  const __m128 aZero    = _mm_setzero_ps();
  const __m128 aHitMask = _mm_and_ps(_mm_cmple_ps(aTEnter, aTLeave),
                                     _mm_cmpge_ps(aTLeave, aZero));

  _mm_storeu_ps(theOutTEnter, aTEnter);
  _mm_storeu_ps(theOutTLeave, aTLeave);
  return _mm_movemask_ps(aHitMask);
}

#endif // BVH_HAS_AVX2_KERNEL

} // namespace SIMD
} // namespace BVH

#endif // BVH_ToolsSIMD_AVX2_HeaderFile

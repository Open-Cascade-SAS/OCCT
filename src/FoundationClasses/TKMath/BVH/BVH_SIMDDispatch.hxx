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

#ifndef BVH_SIMDDispatch_HeaderFile
#define BVH_SIMDDispatch_HeaderFile

#include <Standard_Macro.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

namespace BVH
{
namespace SIMD
{

//! 4 axis-aligned bounding boxes packed in Structure-of-Arrays layout.
//! Used as input to SIMD-accelerated quad-BVH (BVH4) traversal kernels.
//! Kept as a non-template legacy struct because BVH4 is intentionally outside
//! the BVH_WideTree<W> abstraction (SSE2 with __m128 is its natural fit).
struct alignas(16) BVH_Box4f_SoA
{
  float minX[4];
  float minY[4];
  float minZ[4];
  float maxX[4];
  float maxY[4];
  float maxZ[4];
};

//! Single ray with each component broadcast to 4 SIMD lanes.
//! Built once per ray and reused across all internal nodes during traversal.
struct alignas(16) BVH_Ray4f_Splat
{
  float ox[4];
  float oy[4];
  float oz[4];
  float idx[4];
  float idy[4];
  float idz[4];
};

//! W axis-aligned bounding boxes packed in Structure-of-Arrays layout.
//! Used as input to BVH_WideTree<W> SIMD kernels:
//!   - W=8  fills a single __m256 (AVX2)   in one slab pass
//!   - W=16 fills a single __m512 (AVX-512) in one slab pass
template <int W>
struct alignas(W * 4) BVH_BoxNf_SoA
{
  static_assert(W == 8 || W == 16, "Only W=8 (AVX2) and W=16 (AVX-512) are instantiated");
  float minX[W];
  float minY[W];
  float minZ[W];
  float maxX[W];
  float maxY[W];
  float maxZ[W];
};

//! Single ray with each component broadcast to W SIMD lanes.
template <int W>
struct alignas(W * 4) BVH_RayNf_Splat
{
  static_assert(W == 8 || W == 16, "Only W=8 (AVX2) and W=16 (AVX-512) are instantiated");
  float ox[W];
  float oy[W];
  float oz[W];
  float idx[W];
  float idy[W];
  float idz[W];
};

//! Detected SIMD instruction set level.
enum class Level
{
  Scalar = 0,
  SSE2   = 1,
  AVX2   = 2,
  AVX512 = 3
};

//! Detects the highest SIMD level supported by the current CPU and OS.
//! Performs cpuid plus xgetbv (for AVX-512) to ensure the OS preserves
//! the relevant register state across context switches.
//! Result is computed once per process and cached.
Standard_EXPORT Level Detect() noexcept;

//=============================================================================
// BVH4 (legacy, untouched by the BVH_WideTree<W> refactor)
//=============================================================================

//! Function pointer type for the 1-ray-vs-4-AABB kernel.
//! Returns a 4-bit hit mask (bit i set if child i is hit).
//! outTEnter[i] / outTLeave[i] receive the slab interval per lane;
//! values for non-hit lanes are unspecified.
using RayBox4_Fn = int (*)(const BVH_Ray4f_Splat& theRay,
                           const BVH_Box4f_SoA&   theBoxes,
                           float*                 theOutTEnter,
                           float*                 theOutTLeave) noexcept;

//! Scalar reference implementation. Always available, used as fallback
//! and as the equivalence baseline in tests.
Standard_EXPORT int RayBox4_Scalar(const BVH_Ray4f_Splat& theRay,
                                   const BVH_Box4f_SoA&   theBoxes,
                                   float*                 theOutTEnter,
                                   float*                 theOutTLeave) noexcept;

//! Returns a function pointer to the best available kernel for this CPU.
//! Thread-safe lazy initialization (C++17 magic statics).
//!
//! The dispatch can be overridden at process start by setting environment
//! variable OCCT_BVH_SIMD_FORCE to one of: scalar, sse2, avx2, avx512.
//! Useful for testing each kernel on a single machine.
Standard_EXPORT RayBox4_Fn GetRayBox4() noexcept;

//=============================================================================
// BVH_WideTree<W> kernels (W=8: AVX2 + scalar; W=16: AVX-512 + scalar)
//=============================================================================

//! Function pointer type for the 1-ray-vs-W-AABB kernel.
//! Returns a W-bit hit mask (bit i set if child i is hit).
template <int W>
using RayBoxN_Fn = int (*)(const BVH_RayNf_Splat<W>& theRay,
                           const BVH_BoxNf_SoA<W>&   theBoxes,
                           float*                    theOutTEnter,
                           float*                    theOutTLeave) noexcept;

//! Scalar reference implementation of the W-wide kernel. Defined inline as
//! a function template so any W can be exercised; the dispatcher uses it as
//! a fallback when no SIMD kernel is available for the host.
template <int W>
inline int RayBoxN_Scalar(const BVH_RayNf_Splat<W>& theRay,
                          const BVH_BoxNf_SoA<W>&   theBoxes,
                          float*                    theOutTEnter,
                          float*                    theOutTLeave) noexcept
{
  int aMask = 0;
  for (int i = 0; i < W; ++i)
  {
    float aTEnter = std::numeric_limits<float>::lowest();
    float aTLeave = (std::numeric_limits<float>::max)();
    bool  aHit    = true;

    if (std::isinf(theRay.idx[i]))
    {
      if (theRay.ox[i] < theBoxes.minX[i] || theRay.ox[i] > theBoxes.maxX[i])
        aHit = false;
    }
    else
    {
      const float aT1 = (theBoxes.minX[i] - theRay.ox[i]) * theRay.idx[i];
      const float aT2 = (theBoxes.maxX[i] - theRay.ox[i]) * theRay.idx[i];
      aTEnter         = (std::max)(aTEnter, (std::min)(aT1, aT2));
      aTLeave         = (std::min)(aTLeave, (std::max)(aT1, aT2));
    }

    if (aHit)
    {
      if (std::isinf(theRay.idy[i]))
      {
        if (theRay.oy[i] < theBoxes.minY[i] || theRay.oy[i] > theBoxes.maxY[i])
          aHit = false;
      }
      else
      {
        const float aT1 = (theBoxes.minY[i] - theRay.oy[i]) * theRay.idy[i];
        const float aT2 = (theBoxes.maxY[i] - theRay.oy[i]) * theRay.idy[i];
        aTEnter         = (std::max)(aTEnter, (std::min)(aT1, aT2));
        aTLeave         = (std::min)(aTLeave, (std::max)(aT1, aT2));
      }
    }

    if (aHit)
    {
      if (std::isinf(theRay.idz[i]))
      {
        if (theRay.oz[i] < theBoxes.minZ[i] || theRay.oz[i] > theBoxes.maxZ[i])
          aHit = false;
      }
      else
      {
        const float aT1 = (theBoxes.minZ[i] - theRay.oz[i]) * theRay.idz[i];
        const float aT2 = (theBoxes.maxZ[i] - theRay.oz[i]) * theRay.idz[i];
        aTEnter         = (std::max)(aTEnter, (std::min)(aT1, aT2));
        aTLeave         = (std::min)(aTLeave, (std::max)(aT1, aT2));
      }
    }

    if (aHit && aTEnter <= aTLeave && aTLeave >= 0.0f)
    {
      theOutTEnter[i] = aTEnter;
      theOutTLeave[i] = aTLeave;
      aMask |= (1 << i);
    }
    else
    {
      theOutTEnter[i] = std::numeric_limits<float>::quiet_NaN();
      theOutTLeave[i] = std::numeric_limits<float>::quiet_NaN();
    }
  }
  return aMask;
}

//! Returns a function pointer to the best available W-wide kernel for this
//! CPU. Defined as explicit specializations in the .cxx so the static
//! dispatch cache lives in one place per W.
//!
//! For W=8 the dispatch chooses AVX2 (the natural fit) or scalar fallback.
//! For W=16 the dispatch chooses AVX-512 (the natural fit) or scalar fallback.
//! Setting OCCT_BVH_SIMD_FORCE=scalar forces scalar regardless of CPU.
template <int W>
Standard_EXPORT RayBoxN_Fn<W> GetRayBoxN() noexcept;

// Explicit specialization declarations -- bodies live in BVH_SIMDDispatch.cxx.
template <>
Standard_EXPORT RayBoxN_Fn<8> GetRayBoxN<8>() noexcept;
template <>
Standard_EXPORT RayBoxN_Fn<16> GetRayBoxN<16>() noexcept;

} // namespace SIMD
} // namespace BVH

#endif // BVH_SIMDDispatch_HeaderFile

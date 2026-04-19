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

namespace BVH
{
namespace SIMD
{

//! 4 axis-aligned bounding boxes packed in Structure-of-Arrays layout.
//! Used as input to SIMD-accelerated quad-BVH traversal kernels.
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

} // namespace SIMD
} // namespace BVH

#endif // BVH_SIMDDispatch_HeaderFile

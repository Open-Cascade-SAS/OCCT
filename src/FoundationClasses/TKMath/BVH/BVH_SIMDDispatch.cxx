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

#include <BVH_SIMDDispatch.hxx>

#include <BVH_ToolsSIMD_SSE2.hxx>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
  #include <intrin.h>
  #include <immintrin.h>
#endif

namespace BVH
{
namespace SIMD
{

namespace
{

//! Reads OCCT_BVH_SIMD_FORCE env var and clamps to <= theDetected.
//! Returns Level::Scalar..AVX512, never above what the CPU actually supports.
Level applyForceOverride(Level theDetected) noexcept
{
  const char* anEnv = std::getenv("OCCT_BVH_SIMD_FORCE");
  if (anEnv == nullptr)
  {
    return theDetected;
  }
  Level aForced = theDetected;
  if (std::strcmp(anEnv, "scalar") == 0)
  {
    aForced = Level::Scalar;
  }
  else if (std::strcmp(anEnv, "sse2") == 0)
  {
    aForced = Level::SSE2;
  }
  else if (std::strcmp(anEnv, "avx2") == 0)
  {
    aForced = Level::AVX2;
  }
  else if (std::strcmp(anEnv, "avx512") == 0)
  {
    aForced = Level::AVX512;
  }
  // Never select a level the CPU does not support.
  return (static_cast<int>(aForced) <= static_cast<int>(theDetected)) ? aForced : theDetected;
}

Level detectImpl() noexcept
{
#if defined(__GNUC__) || defined(__clang__)
  __builtin_cpu_init();
  if (__builtin_cpu_supports("avx512f"))
  {
    return Level::AVX512;
  }
  if (__builtin_cpu_supports("avx2"))
  {
    return Level::AVX2;
  }
  if (__builtin_cpu_supports("sse2"))
  {
    return Level::SSE2;
  }
  return Level::Scalar;
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
  int aRegs[4] = {0, 0, 0, 0};
  __cpuid(aRegs, 1);
  const bool hasOSXSAVE = ((aRegs[2] >> 27) & 1) != 0;
  const bool hasSSE2    = ((aRegs[3] >> 26) & 1) != 0;
  __cpuidex(aRegs, 7, 0);
  const bool hasAVX2    = ((aRegs[1] >> 5) & 1) != 0;
  const bool hasAVX512F = ((aRegs[1] >> 16) & 1) != 0;
  // OS XSAVE bits: bit 1 = SSE, bit 2 = AVX (YMM), bits 5-7 = AVX-512 (opmask, ZMM_Hi256, Hi16_ZMM).
  // 0xE6 = 0b11100110 covers AVX-512 + AVX + SSE state preservation.
  unsigned long long anXcr0 = hasOSXSAVE ? _xgetbv(0) : 0ULL;
  if (hasOSXSAVE && hasAVX512F && (anXcr0 & 0xE6ULL) == 0xE6ULL)
  {
    return Level::AVX512;
  }
  if (hasOSXSAVE && hasAVX2 && (anXcr0 & 0x6ULL) == 0x6ULL)
  {
    return Level::AVX2;
  }
  if (hasSSE2)
  {
    return Level::SSE2;
  }
  return Level::Scalar;
#else
  return Level::Scalar;
#endif
}

} // namespace

Level Detect() noexcept
{
  static const Level sLevel = applyForceOverride(detectImpl());
  return sLevel;
}

//=================================================================================================

int RayBox4_Scalar(const BVH_Ray4f_Splat& theRay,
                   const BVH_Box4f_SoA&   theBoxes,
                   float*                 theOutTEnter,
                   float*                 theOutTLeave) noexcept
{
  int aMask = 0;
  for (int i = 0; i < 4; ++i)
  {
    float aTEnter = std::numeric_limits<float>::lowest();
    float aTLeave = (std::numeric_limits<float>::max)();
    bool  aHit    = true;

    // X axis slab.
    if (std::isinf(theRay.idx[i]))
    {
      if (theRay.ox[i] < theBoxes.minX[i] || theRay.ox[i] > theBoxes.maxX[i])
      {
        aHit = false;
      }
    }
    else
    {
      const float aT1 = (theBoxes.minX[i] - theRay.ox[i]) * theRay.idx[i];
      const float aT2 = (theBoxes.maxX[i] - theRay.ox[i]) * theRay.idx[i];
      aTEnter         = (std::max)(aTEnter, (std::min)(aT1, aT2));
      aTLeave         = (std::min)(aTLeave, (std::max)(aT1, aT2));
    }

    // Y axis slab.
    if (aHit)
    {
      if (std::isinf(theRay.idy[i]))
      {
        if (theRay.oy[i] < theBoxes.minY[i] || theRay.oy[i] > theBoxes.maxY[i])
        {
          aHit = false;
        }
      }
      else
      {
        const float aT1 = (theBoxes.minY[i] - theRay.oy[i]) * theRay.idy[i];
        const float aT2 = (theBoxes.maxY[i] - theRay.oy[i]) * theRay.idy[i];
        aTEnter         = (std::max)(aTEnter, (std::min)(aT1, aT2));
        aTLeave         = (std::min)(aTLeave, (std::max)(aT1, aT2));
      }
    }

    // Z axis slab.
    if (aHit)
    {
      if (std::isinf(theRay.idz[i]))
      {
        if (theRay.oz[i] < theBoxes.minZ[i] || theRay.oz[i] > theBoxes.maxZ[i])
        {
          aHit = false;
        }
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

//=================================================================================================

RayBox4_Fn GetRayBox4() noexcept
{
  static const RayBox4_Fn sFn = []() noexcept -> RayBox4_Fn {
    switch (Detect())
    {
      // case Level::AVX512: return &RayBox4_AVX512;  // wired in commit 5
      // case Level::AVX2:   return &RayBox4_AVX2;    // wired in commit 4
#if defined(BVH_HAS_SSE2_KERNEL)
      case Level::SSE2:
      case Level::AVX2:
      case Level::AVX512:
        return &RayBox4_SSE2;
#endif
      default:
        return &RayBox4_Scalar;
    }
  }();
  return sFn;
}

} // namespace SIMD
} // namespace BVH

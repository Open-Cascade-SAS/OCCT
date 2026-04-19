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

#include <gtest/gtest.h>

#include <BVH_SIMDDispatch.hxx>
#include <BVH_ToolsSIMD_AVX2.hxx>
#include <BVH_ToolsSIMD_SSE2.hxx>

#include <cmath>
#include <limits>
#include <random>

namespace
{

BVH::SIMD::BVH_Ray4f_Splat MakeRaySplat(float ox, float oy, float oz, float dx, float dy, float dz)
{
  BVH::SIMD::BVH_Ray4f_Splat aRay{};
  for (int i = 0; i < 4; ++i)
  {
    aRay.ox[i]  = ox;
    aRay.oy[i]  = oy;
    aRay.oz[i]  = oz;
    aRay.idx[i] = (dx != 0.0f) ? (1.0f / dx) : std::numeric_limits<float>::infinity();
    aRay.idy[i] = (dy != 0.0f) ? (1.0f / dy) : std::numeric_limits<float>::infinity();
    aRay.idz[i] = (dz != 0.0f) ? (1.0f / dz) : std::numeric_limits<float>::infinity();
  }
  return aRay;
}

void SetBox(BVH::SIMD::BVH_Box4f_SoA& theBoxes,
            int                       i,
            float                     minx,
            float                     miny,
            float                     minz,
            float                     maxx,
            float                     maxy,
            float                     maxz)
{
  theBoxes.minX[i] = minx;
  theBoxes.minY[i] = miny;
  theBoxes.minZ[i] = minz;
  theBoxes.maxX[i] = maxx;
  theBoxes.maxY[i] = maxy;
  theBoxes.maxZ[i] = maxz;
}

} // namespace

#if defined(BVH_HAS_SSE2_KERNEL)

TEST(BVH_ToolsSIMDTest, SSE2_AllHit)
{
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b1111);
}

TEST(BVH_ToolsSIMDTest, SSE2_AllMiss)
{
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 5.0f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0);
}

TEST(BVH_ToolsSIMDTest, SSE2_PartialHit_Mosaic)
{
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 1, 2.0f, 5.0f, 0.0f, 3.0f, 6.0f, 1.0f); // miss
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 3, 6.0f, 5.0f, 0.0f, 7.0f, 6.0f, 1.0f); // miss

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b0101);
}

TEST(BVH_ToolsSIMDTest, SSE2_RayBehind)
{
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0);
}

TEST(BVH_ToolsSIMDTest, SSE2_ParallelRayInsideSlab)
{
  // Ray with dy=dz=0; idx is finite, idy/idz are infinite. The SIMD path
  // must produce identical hit/miss to the scalar version.
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 1, 2.0f, 0.6f, 0.0f, 3.0f, 1.0f, 1.0f); // miss (Y origin outside)
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.6f, 7.0f, 1.0f, 1.0f); // miss (Z origin outside)

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b0101);
}

TEST(BVH_ToolsSIMDTest, SSE2_DegenerateBox)
{
  // Point boxes (min == max). Hit if ray passes exactly through the point;
  // since exact equality is fragile in float, we test the *behaviour matches scalar*.
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 1.0f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f); // degenerate point on ray
  SetBox(aBoxes, 1, 2.0f, 5.0f, 5.0f, 2.0f, 5.0f, 5.0f); // degenerate, off ray
  SetBox(aBoxes, 2, 3.0f, 0.5f, 0.5f, 3.0f, 0.5f, 0.5f); // degenerate, on ray
  SetBox(aBoxes, 3, 4.0f, 5.0f, 5.0f, 4.0f, 5.0f, 5.0f); // degenerate, off ray

  float aSEnter[4]{}, aSLeave[4]{};
  int   aSMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aSEnter, aSLeave);
  float aTEnter[4]{}, aTLeave[4]{};
  int   aTMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aSMask, aTMask);
}

TEST(BVH_ToolsSIMDTest, SSE2_RandomConsistencyVsScalar)
{
  // Random rays vs random AABBs; SSE2 and scalar must agree on the hit mask
  // and on tEnter/tLeave for hit lanes (within EPS).
  std::mt19937                          aGen(0xC0FFEEu);
  std::uniform_real_distribution<float> aPos(-10.0f, 10.0f);
  std::uniform_real_distribution<float> aSize(0.1f, 5.0f);
  std::uniform_real_distribution<float> aDir(-1.0f, 1.0f);

  constexpr int   kNumCases = 1000;
  constexpr float kEps      = 1.0e-3f;
  int             anAgree   = 0;

  for (int aCase = 0; aCase < kNumCases; ++aCase)
  {
    // Random ray; reject zero direction (degenerate ray).
    float dx = aDir(aGen), dy = aDir(aGen), dz = aDir(aGen);
    if (std::abs(dx) < 1.0e-3f && std::abs(dy) < 1.0e-3f && std::abs(dz) < 1.0e-3f)
    {
      dx = 1.0f;
    }
    BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(aPos(aGen), aPos(aGen), aPos(aGen), dx, dy, dz);

    BVH::SIMD::BVH_Box4f_SoA aBoxes{};
    for (int k = 0; k < 4; ++k)
    {
      const float cx = aPos(aGen), cy = aPos(aGen), cz = aPos(aGen);
      const float sx = aSize(aGen), sy = aSize(aGen), sz = aSize(aGen);
      SetBox(aBoxes, k, cx - sx, cy - sy, cz - sz, cx + sx, cy + sy, cz + sz);
    }

    float aSEnter[4]{}, aSLeave[4]{};
    int   aSMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aSEnter, aSLeave);
    float aTEnter[4]{}, aTLeave[4]{};
    int   aTMask = BVH::SIMD::RayBox4_SSE2(aRay, aBoxes, aTEnter, aTLeave);

    EXPECT_EQ(aSMask, aTMask) << "case " << aCase << " mask mismatch";

    // For hit lanes both sides should produce comparable t values.
    for (int i = 0; i < 4; ++i)
    {
      if (((aSMask >> i) & 1) && ((aTMask >> i) & 1))
      {
        EXPECT_NEAR(aSEnter[i], aTEnter[i], kEps) << "case " << aCase << " lane " << i << " tEnter";
        EXPECT_NEAR(aSLeave[i], aTLeave[i], kEps) << "case " << aCase << " lane " << i << " tLeave";
      }
    }
    if (aSMask == aTMask)
    {
      ++anAgree;
    }
  }
  EXPECT_EQ(anAgree, kNumCases);
}

TEST(BVH_ToolsSIMDTest, Dispatch_PrefersBestKernelOnX86)
{
  // On any x86_64 host SSE2 is mandatory; AVX2 may or may not be present.
  // The dispatcher must select something better than scalar.
  #if defined(__x86_64__) || defined(_M_X64)
  BVH::SIMD::RayBox4_Fn aFn = BVH::SIMD::GetRayBox4();
  EXPECT_NE(aFn, &BVH::SIMD::RayBox4_Scalar);
  #else
  GTEST_SKIP() << "Non-x86 architecture, dispatch test does not apply";
  #endif
}

#endif // BVH_HAS_SSE2_KERNEL

#if defined(BVH_HAS_AVX2_KERNEL)

TEST(BVH_ToolsSIMDTest, AVX2_AllHit)
{
  if (BVH::SIMD::Detect() < BVH::SIMD::Level::AVX2)
  {
    GTEST_SKIP() << "AVX2 not supported on this CPU";
  }
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_AVX2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b1111);
}

TEST(BVH_ToolsSIMDTest, AVX2_PartialHit_Mosaic)
{
  if (BVH::SIMD::Detect() < BVH::SIMD::Level::AVX2)
  {
    GTEST_SKIP() << "AVX2 not supported on this CPU";
  }
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 5.0f, 0.0f, 3.0f, 6.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 5.0f, 0.0f, 7.0f, 6.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_AVX2(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b0101);
}

TEST(BVH_ToolsSIMDTest, AVX2_RandomConsistencyVsScalar)
{
  if (BVH::SIMD::Detect() < BVH::SIMD::Level::AVX2)
  {
    GTEST_SKIP() << "AVX2 not supported on this CPU";
  }
  std::mt19937                          aGen(0xCAFEFEEDu);
  std::uniform_real_distribution<float> aPos(-10.0f, 10.0f);
  std::uniform_real_distribution<float> aSize(0.1f, 5.0f);
  std::uniform_real_distribution<float> aDir(-1.0f, 1.0f);

  constexpr int   kNumCases = 1000;
  constexpr float kEps      = 1.0e-3f;

  for (int aCase = 0; aCase < kNumCases; ++aCase)
  {
    float dx = aDir(aGen), dy = aDir(aGen), dz = aDir(aGen);
    if (std::abs(dx) < 1.0e-3f && std::abs(dy) < 1.0e-3f && std::abs(dz) < 1.0e-3f)
    {
      dx = 1.0f;
    }
    BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(aPos(aGen), aPos(aGen), aPos(aGen), dx, dy, dz);

    BVH::SIMD::BVH_Box4f_SoA aBoxes{};
    for (int k = 0; k < 4; ++k)
    {
      const float cx = aPos(aGen), cy = aPos(aGen), cz = aPos(aGen);
      const float sx = aSize(aGen), sy = aSize(aGen), sz = aSize(aGen);
      SetBox(aBoxes, k, cx - sx, cy - sy, cz - sz, cx + sx, cy + sy, cz + sz);
    }

    float aSEnter[4]{}, aSLeave[4]{};
    int   aSMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aSEnter, aSLeave);
    float aTEnter[4]{}, aTLeave[4]{};
    int   aTMask = BVH::SIMD::RayBox4_AVX2(aRay, aBoxes, aTEnter, aTLeave);

    EXPECT_EQ(aSMask, aTMask) << "AVX2 case " << aCase << " mask mismatch";
    for (int i = 0; i < 4; ++i)
    {
      if (((aSMask >> i) & 1) && ((aTMask >> i) & 1))
      {
        EXPECT_NEAR(aSEnter[i], aTEnter[i], kEps);
        EXPECT_NEAR(aSLeave[i], aTLeave[i], kEps);
      }
    }
  }
}

#endif // BVH_HAS_AVX2_KERNEL

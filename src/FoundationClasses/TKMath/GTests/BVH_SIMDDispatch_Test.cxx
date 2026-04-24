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

#include <limits>

namespace
{

//! Helper that fills a Ray4 splat from a single ray.
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

//! Helper that sets the i-th lane of a SoA box.
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

TEST(BVH_SIMDDispatchTest, DetectReturnsValidLevel)
{
  // Should never crash, should return one of the four enum values.
  BVH::SIMD::Level aLvl = BVH::SIMD::Detect();
  EXPECT_TRUE(aLvl == BVH::SIMD::Level::Scalar || aLvl == BVH::SIMD::Level::SSE2
              || aLvl == BVH::SIMD::Level::AVX2 || aLvl == BVH::SIMD::Level::AVX512);
}

TEST(BVH_SIMDDispatchTest, GetRayBox4ReturnsNonNull)
{
  BVH::SIMD::RayBox4_Fn aFn = BVH::SIMD::GetRayBox4();
  EXPECT_NE(aFn, nullptr);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBox4_AllHit)
{
  // Ray along +X starting at origin, 4 unit boxes along the X axis.
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b1111);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBox4_AllMiss)
{
  // Ray along +X but offset in Y so it misses every box.
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 5.0f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBox4_PartialHit_Mosaic)
{
  // Lanes 0 and 2 hit, lanes 1 and 3 miss (Y offset places them out of plane).
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 1, 2.0f, 5.0f, 0.0f, 3.0f, 6.0f, 1.0f); // miss (Y out of range)
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 3, 6.0f, 5.0f, 0.0f, 7.0f, 6.0f, 1.0f); // miss

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b0101);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBox4_RayBehind)
{
  // Boxes are behind the ray origin; tLeave < 0 must reject all.
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 1, 2.0f, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f);
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.0f, 7.0f, 1.0f, 1.0f);

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBox4_ParallelRayInsideSlab)
{
  // Ray along +X, dy = dz = 0 (parallel to YZ plane). Origin must be inside
  // each box's Y/Z slab to hit. Lane 1 has Y origin outside its slab => miss.
  BVH::SIMD::BVH_Ray4f_Splat aRay = MakeRaySplat(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_Box4f_SoA   aBoxes{};
  SetBox(aBoxes, 0, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 1, 2.0f, 0.6f, 0.0f, 3.0f, 1.0f, 1.0f); // miss (origin Y=0.5 < boxMinY=0.6)
  SetBox(aBoxes, 2, 4.0f, 0.0f, 0.0f, 5.0f, 1.0f, 1.0f); // hit
  SetBox(aBoxes, 3, 6.0f, 0.0f, 0.6f, 7.0f, 1.0f, 1.0f); // miss (origin Z=0.5 < boxMinZ=0.6)

  float aTEnter[4]{}, aTLeave[4]{};
  int   aMask = BVH::SIMD::RayBox4_Scalar(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b0101);
}

namespace
{

template <int W>
BVH::SIMD::BVH_RayNf_Splat<W> MakeRayNSplat(float ox,
                                            float oy,
                                            float oz,
                                            float dx,
                                            float dy,
                                            float dz)
{
  BVH::SIMD::BVH_RayNf_Splat<W> aRay{};
  for (int i = 0; i < W; ++i)
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

template <int W>
void SetBoxN(BVH::SIMD::BVH_BoxNf_SoA<W>& theBoxes,
             int                          i,
             float                        minx,
             float                        miny,
             float                        minz,
             float                        maxx,
             float                        maxy,
             float                        maxz)
{
  theBoxes.minX[i] = minx;
  theBoxes.minY[i] = miny;
  theBoxes.minZ[i] = minz;
  theBoxes.maxX[i] = maxx;
  theBoxes.maxY[i] = maxy;
  theBoxes.maxZ[i] = maxz;
}

} // namespace

TEST(BVH_SIMDDispatchTest, ScalarRayBoxN8_AllHit)
{
  auto                        aRay = MakeRayNSplat<8>(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_BoxNf_SoA<8> aBoxes{};
  for (int i = 0; i < 8; ++i)
  {
    SetBoxN<8>(aBoxes, i, i * 2.0f, 0.0f, 0.0f, i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  float aTEnter[8]{}, aTLeave[8]{};
  int   aMask = BVH::SIMD::RayBoxN_Scalar<8>(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0xFF);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBoxN8_AllMiss)
{
  auto                        aRay = MakeRayNSplat<8>(-1.0f, 5.0f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_BoxNf_SoA<8> aBoxes{};
  for (int i = 0; i < 8; ++i)
  {
    SetBoxN<8>(aBoxes, i, i * 2.0f, 0.0f, 0.0f, i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  float aTEnter[8]{}, aTLeave[8]{};
  int   aMask = BVH::SIMD::RayBoxN_Scalar<8>(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0);
}

TEST(BVH_SIMDDispatchTest, ScalarRayBoxN8_PartialHit_8BitMosaic)
{
  // Even-index lanes hit, odd lanes are offset out of plane.
  auto                        aRay = MakeRayNSplat<8>(-1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
  BVH::SIMD::BVH_BoxNf_SoA<8> aBoxes{};
  for (int i = 0; i < 8; ++i)
  {
    const float dy = (i & 1) ? 5.0f : 0.0f;
    SetBoxN<8>(aBoxes, i, i * 2.0f, dy, 0.0f, i * 2.0f + 1.0f, dy + 1.0f, 1.0f);
  }
  float aTEnter[8]{}, aTLeave[8]{};
  int   aMask = BVH::SIMD::RayBoxN_Scalar<8>(aRay, aBoxes, aTEnter, aTLeave);
  EXPECT_EQ(aMask, 0b01010101);
}

TEST(BVH_SIMDDispatchTest, GetRayBoxN8ReturnsNonNull)
{
  EXPECT_NE(BVH::SIMD::GetRayBoxN<8>(), nullptr);
}

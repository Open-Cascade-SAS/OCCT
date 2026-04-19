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
//
// Microbenchmark for the BVH_ToolsSIMD kernels.
//
// All tests in this file use the DISABLED_ prefix so they are skipped by
// default; CI does not measure timing reliably. Run them explicitly with
//   OpenCascadeGTest --gtest_also_run_disabled_tests
//                    --gtest_filter='BVH_ToolsSIMDBenchmark*'

#include <gtest/gtest.h>

#include <BVH_SIMDDispatch.hxx>
#include <BVH_ToolsSIMD_AVX2.hxx>
#include <BVH_ToolsSIMD_AVX512.hxx>
#include <BVH_ToolsSIMD_SSE2.hxx>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

namespace
{

constexpr int kNumCases   = 4096;     // distinct (ray, 4-box) pairs
constexpr int kNumRepeats = 4096;     // outer iteration count

//! Generates kNumCases random (ray, 4-AABB) pairs that mostly intersect.
struct BenchmarkData
{
  std::vector<BVH::SIMD::BVH_Ray4f_Splat> rays;
  std::vector<BVH::SIMD::BVH_Box4f_SoA>   boxes;

  BenchmarkData()
  {
    rays.reserve(kNumCases);
    boxes.reserve(kNumCases);

    std::mt19937                          aGen(0x12345);
    std::uniform_real_distribution<float> aPos(-10.0f, 10.0f);
    std::uniform_real_distribution<float> aSize(0.5f, 4.0f);
    std::uniform_real_distribution<float> aDir(-1.0f, 1.0f);

    for (int aCase = 0; aCase < kNumCases; ++aCase)
    {
      float dx = aDir(aGen), dy = aDir(aGen), dz = aDir(aGen);
      const float aLen = std::sqrt(dx * dx + dy * dy + dz * dz);
      if (aLen < 1.0e-3f)
      {
        dx = 1.0f;
        dy = 0.0f;
        dz = 0.0f;
      }
      else
      {
        dx /= aLen;
        dy /= aLen;
        dz /= aLen;
      }
      const float ox = aPos(aGen), oy = aPos(aGen), oz = aPos(aGen);

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
      rays.push_back(aRay);

      BVH::SIMD::BVH_Box4f_SoA aBoxes{};
      for (int k = 0; k < 4; ++k)
      {
        const float cx = aPos(aGen), cy = aPos(aGen), cz = aPos(aGen);
        const float sx = aSize(aGen), sy = aSize(aGen), sz = aSize(aGen);
        aBoxes.minX[k] = cx - sx;
        aBoxes.minY[k] = cy - sy;
        aBoxes.minZ[k] = cz - sz;
        aBoxes.maxX[k] = cx + sx;
        aBoxes.maxY[k] = cy + sy;
        aBoxes.maxZ[k] = cz + sz;
      }
      boxes.push_back(aBoxes);
    }
  }
};

//! Result of one kernel measurement.
struct KernelTime
{
  const char* name;
  double      nsPerCall;
  long long   sinkSum; // accumulated mask, prevents the optimizer from removing the loop
};

KernelTime MeasureKernel(const char*               theName,
                         BVH::SIMD::RayBox4_Fn     theFn,
                         const BenchmarkData&      theData)
{
  // Warm-up: 1 pass over all data.
  long long aWarmupSink = 0;
  for (const auto& aRay : theData.rays)
  {
    float aTEnter[4]{}, aTLeave[4]{};
    aWarmupSink += theFn(aRay, theData.boxes[0], aTEnter, aTLeave);
  }

  long long aSink = aWarmupSink;
  auto      t0    = std::chrono::high_resolution_clock::now();
  for (int aRep = 0; aRep < kNumRepeats; ++aRep)
  {
    const auto& aRay = theData.rays[aRep & (kNumCases - 1)];
    for (int aCase = 0; aCase < kNumCases; ++aCase)
    {
      float aTEnter[4]{}, aTLeave[4]{};
      aSink += theFn(aRay, theData.boxes[aCase], aTEnter, aTLeave);
    }
  }
  auto t1 = std::chrono::high_resolution_clock::now();

  const double aTotalNs   = std::chrono::duration<double, std::nano>(t1 - t0).count();
  const double aPerCallNs = aTotalNs / (static_cast<double>(kNumRepeats) * kNumCases);
  return {theName, aPerCallNs, aSink};
}

void PrintRow(const KernelTime& theTime, double theBaselineNs)
{
  std::cout << "  " << std::left << std::setw(14) << theTime.name
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(8) << theTime.nsPerCall << " ns/call"
            << "  " << std::setw(6) << std::setprecision(2)
            << (theBaselineNs / theTime.nsPerCall) << "x"
            << "  (sink=" << theTime.sinkSum << ")"
            << std::endl;
}

} // namespace

TEST(BVH_ToolsSIMDBenchmark, DISABLED_RayBox4_KernelComparison)
{
  BenchmarkData aData;

  std::cout << "\n=== RayBox4 microbenchmark ===" << std::endl;
  std::cout << "  cases   : " << kNumCases << " (random ray + 4 AABB)" << std::endl;
  std::cout << "  repeats : " << kNumRepeats << std::endl;
  std::cout << "  total   : " << (static_cast<long long>(kNumCases) * kNumRepeats)
            << " kernel invocations per kernel" << std::endl;
  std::cout << "  detected: ";
  switch (BVH::SIMD::Detect())
  {
    case BVH::SIMD::Level::AVX512: std::cout << "AVX-512"; break;
    case BVH::SIMD::Level::AVX2:   std::cout << "AVX2";    break;
    case BVH::SIMD::Level::SSE2:   std::cout << "SSE2";    break;
    default:                       std::cout << "Scalar";  break;
  }
  std::cout << std::endl << std::endl;

  KernelTime aScalar = MeasureKernel("Scalar", &BVH::SIMD::RayBox4_Scalar, aData);
  PrintRow(aScalar, aScalar.nsPerCall);

#if defined(BVH_HAS_SSE2_KERNEL)
  KernelTime aSSE2 = MeasureKernel("SSE2", &BVH::SIMD::RayBox4_SSE2, aData);
  PrintRow(aSSE2, aScalar.nsPerCall);
  EXPECT_LT(aSSE2.nsPerCall, aScalar.nsPerCall)
    << "SSE2 should be at least as fast as scalar";
#endif

#if defined(BVH_HAS_AVX2_KERNEL)
  if (BVH::SIMD::Detect() >= BVH::SIMD::Level::AVX2)
  {
    KernelTime aAVX2 = MeasureKernel("AVX2", &BVH::SIMD::RayBox4_AVX2, aData);
    PrintRow(aAVX2, aScalar.nsPerCall);
  }
  else
  {
    std::cout << "  AVX2          (skipped: CPU support absent)" << std::endl;
  }
#endif

#if defined(BVH_HAS_AVX512_KERNEL)
  if (BVH::SIMD::Detect() >= BVH::SIMD::Level::AVX512)
  {
    KernelTime aAVX512 = MeasureKernel("AVX-512", &BVH::SIMD::RayBox4_AVX512, aData);
    PrintRow(aAVX512, aScalar.nsPerCall);
  }
  else
  {
    std::cout << "  AVX-512       (skipped: CPU support absent)" << std::endl;
  }
#endif

  // Dispatched kernel (whatever the host CPU selects).
  KernelTime aDispatched = MeasureKernel("Dispatched", BVH::SIMD::GetRayBox4(), aData);
  PrintRow(aDispatched, aScalar.nsPerCall);

  std::cout << std::endl;
}

namespace
{

//! 8-wide variant of BenchmarkData / MeasureKernel for the BVH8 kernels.
struct BenchmarkData8
{
  std::vector<BVH::SIMD::BVH_Ray8f_Splat> rays;
  std::vector<BVH::SIMD::BVH_Box8f_SoA>   boxes;

  BenchmarkData8()
  {
    rays.reserve(kNumCases);
    boxes.reserve(kNumCases);

    std::mt19937                          aGen(0x12345);
    std::uniform_real_distribution<float> aPos(-10.0f, 10.0f);
    std::uniform_real_distribution<float> aSize(0.5f, 4.0f);
    std::uniform_real_distribution<float> aDir(-1.0f, 1.0f);

    for (int aCase = 0; aCase < kNumCases; ++aCase)
    {
      float dx = aDir(aGen), dy = aDir(aGen), dz = aDir(aGen);
      const float aLen = std::sqrt(dx * dx + dy * dy + dz * dz);
      if (aLen < 1.0e-3f) { dx = 1.0f; dy = 0.0f; dz = 0.0f; }
      else { dx /= aLen; dy /= aLen; dz /= aLen; }
      const float ox = aPos(aGen), oy = aPos(aGen), oz = aPos(aGen);

      BVH::SIMD::BVH_Ray8f_Splat aRay{};
      for (int i = 0; i < 8; ++i)
      {
        aRay.ox[i]  = ox; aRay.oy[i] = oy; aRay.oz[i] = oz;
        aRay.idx[i] = (dx != 0.0f) ? (1.0f / dx) : std::numeric_limits<float>::infinity();
        aRay.idy[i] = (dy != 0.0f) ? (1.0f / dy) : std::numeric_limits<float>::infinity();
        aRay.idz[i] = (dz != 0.0f) ? (1.0f / dz) : std::numeric_limits<float>::infinity();
      }
      rays.push_back(aRay);

      BVH::SIMD::BVH_Box8f_SoA aBoxes{};
      for (int k = 0; k < 8; ++k)
      {
        const float cx = aPos(aGen), cy = aPos(aGen), cz = aPos(aGen);
        const float sx = aSize(aGen), sy = aSize(aGen), sz = aSize(aGen);
        aBoxes.minX[k] = cx - sx; aBoxes.minY[k] = cy - sy; aBoxes.minZ[k] = cz - sz;
        aBoxes.maxX[k] = cx + sx; aBoxes.maxY[k] = cy + sy; aBoxes.maxZ[k] = cz + sz;
      }
      boxes.push_back(aBoxes);
    }
  }
};

KernelTime MeasureKernel8(const char*               theName,
                          BVH::SIMD::RayBox8_Fn     theFn,
                          const BenchmarkData8&     theData)
{
  long long aWarmupSink = 0;
  for (const auto& aRay : theData.rays)
  {
    float aTEnter[8]{}, aTLeave[8]{};
    aWarmupSink += theFn(aRay, theData.boxes[0], aTEnter, aTLeave);
  }
  long long aSink = aWarmupSink;
  auto      t0    = std::chrono::high_resolution_clock::now();
  for (int aRep = 0; aRep < kNumRepeats; ++aRep)
  {
    const auto& aRay = theData.rays[aRep & (kNumCases - 1)];
    for (int aCase = 0; aCase < kNumCases; ++aCase)
    {
      float aTEnter[8]{}, aTLeave[8]{};
      aSink += theFn(aRay, theData.boxes[aCase], aTEnter, aTLeave);
    }
  }
  auto         t1         = std::chrono::high_resolution_clock::now();
  const double aTotalNs   = std::chrono::duration<double, std::nano>(t1 - t0).count();
  const double aPerCallNs = aTotalNs / (static_cast<double>(kNumRepeats) * kNumCases);
  return {theName, aPerCallNs, aSink};
}

} // namespace

TEST(BVH_ToolsSIMDBenchmark, DISABLED_RayBox8_KernelComparison)
{
  BenchmarkData8 aData;

  std::cout << "\n=== RayBox8 microbenchmark (BVH8 fan-out) ===" << std::endl;
  std::cout << "  cases   : " << kNumCases << " (random ray + 8 AABB)" << std::endl;
  std::cout << "  repeats : " << kNumRepeats << std::endl;
  std::cout << "  per kernel: " << (static_cast<long long>(kNumCases) * kNumRepeats)
            << " invocations" << std::endl << std::endl;

  KernelTime aScalar = MeasureKernel8("Scalar", &BVH::SIMD::RayBox8_Scalar, aData);
  PrintRow(aScalar, aScalar.nsPerCall);

  // SIMD variants (SSE2/AVX2/AVX-512) wired in subsequent commits will
  // appear here as their kernels are added to the dispatcher.
  KernelTime aDispatched = MeasureKernel8("Dispatched", BVH::SIMD::GetRayBox8(), aData);
  PrintRow(aDispatched, aScalar.nsPerCall);

  std::cout << std::endl;
}

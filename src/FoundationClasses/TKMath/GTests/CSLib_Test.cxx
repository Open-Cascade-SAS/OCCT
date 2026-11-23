// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <CSLib.hxx>
#include <CSLib_Class2d.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <CSLib_NormalPolyDef.hxx>
#include <CSLib_NormalStatus.hxx>

#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Sequence.hxx>

#include <array>
#include <atomic>
#include <cmath>
#include <limits>
#include <thread>

namespace
{
// Helper function for comparing directions with tolerance
void checkDirEqual(const gp_Dir& theDir1, const gp_Dir& theDir2, const double theTolerance = 1e-10)
{
  EXPECT_NEAR(theDir1.X(), theDir2.X(), theTolerance) << "X components differ";
  EXPECT_NEAR(theDir1.Y(), theDir2.Y(), theTolerance) << "Y components differ";
  EXPECT_NEAR(theDir1.Z(), theDir2.Z(), theTolerance) << "Z components differ";
}

// Helper function for comparing vectors with tolerance
void checkVecEqual(const gp_Vec& theVec1, const gp_Vec& theVec2, const double theTolerance = 1e-10)
{
  EXPECT_NEAR(theVec1.X(), theVec2.X(), theTolerance) << "X components differ";
  EXPECT_NEAR(theVec1.Y(), theVec2.Y(), theTolerance) << "Y components differ";
  EXPECT_NEAR(theVec1.Z(), theVec2.Z(), theTolerance) << "Z components differ";
}
} // namespace

//=================================================================================================
// CSLib Normal Calculation Tests
//=================================================================================================

class CSLibNormalTest : public ::testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Test Normal calculation from D1U and D1V (first overload)
TEST_F(CSLibNormalTest, Normal_FromD1U_D1V_OrthogonalVectors)
{
  // D1U along X, D1V along Y -> Normal should be along Z
  const gp_Vec aD1U(1.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 1.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_Done);
  checkDirEqual(aNormal, gp_Dir(0.0, 0.0, 1.0));
}

TEST_F(CSLibNormalTest, Normal_FromD1U_D1V_ScaledVectors)
{
  // Non-unit vectors should still produce correct normal
  const gp_Vec aD1U(3.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 5.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_Done);
  checkDirEqual(aNormal, gp_Dir(0.0, 0.0, 1.0));
}

TEST_F(CSLibNormalTest, Normal_FromD1U_D1V_ParallelVectors)
{
  // Parallel vectors should return D1uIsParallelD1v status
  const gp_Vec aD1U(1.0, 0.0, 0.0);
  const gp_Vec aD1V(2.0, 0.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-6, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_D1uIsParallelD1v);
}

TEST_F(CSLibNormalTest, Normal_FromD1U_D1V_D1UIsNull)
{
  const gp_Vec aD1U(0.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 1.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_D1uIsNull);
}

TEST_F(CSLibNormalTest, Normal_FromD1U_D1V_D1VIsNull)
{
  const gp_Vec aD1U(1.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 0.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_D1vIsNull);
}

TEST_F(CSLibNormalTest, Normal_FromD1U_D1V_BothNull)
{
  const gp_Vec aD1U(0.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 0.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_D1IsNull);
}

// Test Normal with MagTol (third overload)
TEST_F(CSLibNormalTest, Normal_WithMagTol_Defined)
{
  const gp_Vec aD1U(1.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 1.0, 0.0);

  CSLib_NormalStatus aStatus;
  gp_Dir             aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_Defined);
  checkDirEqual(aNormal, gp_Dir(0.0, 0.0, 1.0));
}

TEST_F(CSLibNormalTest, Normal_WithMagTol_Singular)
{
  // Very small vectors below tolerance
  const gp_Vec aD1U(1e-12, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 1e-12, 0.0);

  CSLib_NormalStatus aStatus;
  gp_Dir             aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_Singular);
}

// Test DNNUV function
TEST_F(CSLibNormalTest, DNNUV_ZeroDerivative)
{
  // Create a simple surface derivative array
  NCollection_Array2<gp_Vec> aDerSurf(0, 2, 0, 2);

  // Set D1U and D1V at index (1,0) and (0,1)
  aDerSurf.SetValue(1, 0, gp_Vec(1.0, 0.0, 0.0)); // D1U
  aDerSurf.SetValue(0, 1, gp_Vec(0.0, 1.0, 0.0)); // D1V
  aDerSurf.SetValue(0, 0, gp_Vec(0.0, 0.0, 0.0)); // Position (not used)
  aDerSurf.SetValue(1, 1, gp_Vec(0.0, 0.0, 0.0)); // D2UV
  aDerSurf.SetValue(2, 0, gp_Vec(0.0, 0.0, 0.0)); // D2U
  aDerSurf.SetValue(0, 2, gp_Vec(0.0, 0.0, 0.0)); // D2V

  // DNNUV(0,0) should give D1U x D1V
  gp_Vec aResult = CSLib::DNNUV(0, 0, aDerSurf);

  // D1U x D1V = (1,0,0) x (0,1,0) = (0,0,1)
  checkVecEqual(aResult, gp_Vec(0.0, 0.0, 1.0));
}

//=================================================================================================
// CSLib_Class2d Tests
//=================================================================================================

class CSLibClass2dTest : public ::testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Test point inside a simple square polygon
TEST_F(CSLibClass2dTest, SiDans_PointInsideSquare)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 1.0);
  aPnts(4) = gp_Pnt2d(0.0, 1.0);

  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 0.0, 0.0, 1.0, 1.0);

  // Point clearly inside
  const gp_Pnt2d aPointInside(0.5, 0.5);
  EXPECT_EQ(aClassifier.SiDans(aPointInside), 1);
}

TEST_F(CSLibClass2dTest, SiDans_PointOutsideSquare)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 1.0);
  aPnts(4) = gp_Pnt2d(0.0, 1.0);

  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 0.0, 0.0, 1.0, 1.0);

  // Point clearly outside
  const gp_Pnt2d aPointOutside(2.0, 2.0);
  EXPECT_EQ(aClassifier.SiDans(aPointOutside), -1);
}

TEST_F(CSLibClass2dTest, SiDans_PointOnBoundary)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 1.0);
  aPnts(4) = gp_Pnt2d(0.0, 1.0);

  CSLib_Class2d aClassifier(aPnts, 0.01, 0.01, 0.0, 0.0, 1.0, 1.0);

  // Point on boundary (with tolerance)
  const gp_Pnt2d aPointOnEdge(0.5, 0.0);
  // Point on boundary should return 0 (uncertain)
  EXPECT_EQ(aClassifier.SiDans(aPointOnEdge), 0);
}

TEST_F(CSLibClass2dTest, DeepCopyPreservesClassification)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 1.0);
  aPnts(4) = gp_Pnt2d(0.0, 1.0);

  CSLib_Class2d       aSource(aPnts, 0.01, 0.01, 0.0, 0.0, 1.0, 1.0);
  const CSLib_Class2d aCopy(aSource);
  CSLib_Class2d       anAssigned;
  anAssigned = aSource;

  const gp_Pnt2d aSamples[] = {gp_Pnt2d(0.5, 0.5), gp_Pnt2d(2.0, 2.0), gp_Pnt2d(0.5, 0.0)};
  for (const gp_Pnt2d& aPoint : aSamples)
  {
    const CSLib_Class2d::Result aState = aSource.SiDans(aPoint);
    EXPECT_EQ(aCopy.SiDans(aPoint), aState);
    EXPECT_EQ(anAssigned.SiDans(aPoint), aState);
  }
}

TEST_F(CSLibClass2dTest, SiDans_TriangularPolygon)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 3);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(2.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 2.0);

  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 0.0, 0.0, 2.0, 2.0);

  // Point inside triangle (centroid)
  const gp_Pnt2d aPointInside(1.0, 0.5);
  EXPECT_EQ(aClassifier.SiDans(aPointInside), 1);

  // Point outside triangle
  const gp_Pnt2d aPointOutside(0.0, 1.0);
  EXPECT_EQ(aClassifier.SiDans(aPointOutside), -1);
}

// Test with Sequence constructor
TEST_F(CSLibClass2dTest, SequenceConstructor_PointInsideSquare)
{
  NCollection_Sequence<gp_Pnt2d> aPnts;
  aPnts.Append(gp_Pnt2d(0.0, 0.0));
  aPnts.Append(gp_Pnt2d(1.0, 0.0));
  aPnts.Append(gp_Pnt2d(1.0, 1.0));
  aPnts.Append(gp_Pnt2d(0.0, 1.0));

  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 0.0, 0.0, 1.0, 1.0);

  const gp_Pnt2d aPointInside(0.5, 0.5);
  EXPECT_EQ(aClassifier.SiDans(aPointInside), 1);
}

// Test InternalSiDans with normalized coordinates
TEST_F(CSLibClass2dTest, InternalSiDans_NormalizedCoordinates)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(10.0, 0.0);
  aPnts(3) = gp_Pnt2d(10.0, 10.0);
  aPnts(4) = gp_Pnt2d(0.0, 10.0);

  CSLib_Class2d aClassifier(aPnts, 0.1, 0.1, 0.0, 0.0, 10.0, 10.0);

  // Test classification with actual coordinates
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(5.0, 5.0)), CSLib_Class2d::Result_Inside);
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(15.0, 15.0)), CSLib_Class2d::Result_Outside);
}

TEST_F(CSLibClass2dTest, LazyGridRemainsExactAwayFromBoundary)
{
  constexpr int                THE_POINT_COUNT = 64;
  NCollection_Array1<gp_Pnt2d> aPnts(1, THE_POINT_COUNT);
  for (int anIdx = 0; anIdx < THE_POINT_COUNT; ++anIdx)
  {
    const double anAngle =
      2.0 * M_PI * static_cast<double>(anIdx) / static_cast<double>(THE_POINT_COUNT);
    aPnts(anIdx + 1) = gp_Pnt2d(0.5 + 0.4 * std::cos(anAngle), 0.5 + 0.4 * std::sin(anAngle));
  }

  CSLib_Class2d aClassifier(aPnts, 1.0e-8, 1.0e-8, 0.0, 0.0, 1.0, 1.0);

  // Cross the lazy-build threshold with exact interior queries.
  for (int aQueryIdx = 0; aQueryIdx < 64; ++aQueryIdx)
  {
    EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  }

  // Exercise cached cells, excluding an annulus around the polygon boundary.
  for (int aY = 0; aY < 16; ++aY)
  {
    for (int anX = 0; anX < 16; ++anX)
    {
      const gp_Pnt2d aPoint((static_cast<double>(anX) + 0.5) / 16.0,
                            (static_cast<double>(aY) + 0.5) / 16.0);
      const double   aDx     = aPoint.X() - 0.5;
      const double   aDy     = aPoint.Y() - 0.5;
      const double   aRadius = std::sqrt(aDx * aDx + aDy * aDy);
      if (aRadius < 0.35)
      {
        EXPECT_EQ(aClassifier.SiDans(aPoint), CSLib_Class2d::Result_Inside);
      }
      else if (aRadius > 0.45)
      {
        EXPECT_EQ(aClassifier.SiDans(aPoint), CSLib_Class2d::Result_Outside);
      }
    }
  }

  // Boundary candidates must stay on the exact/tolerance path after caching.
  EXPECT_EQ(aClassifier.SiDans(aPnts(1)), CSLib_Class2d::Result_Uncertain);

  const CSLib_Class2d aCopy(aClassifier);
  CSLib_Class2d       anAssigned;
  anAssigned = aClassifier;
  EXPECT_EQ(aCopy.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  EXPECT_EQ(anAssigned.SiDans(gp_Pnt2d(0.99, 0.99)), CSLib_Class2d::Result_Outside);
  EXPECT_EQ(aCopy.SiDans(aPnts(1)), CSLib_Class2d::Result_Uncertain);
}

TEST_F(CSLibClass2dTest, LazyGridMatchesExactPathForConcaveDiagonalPolygon)
{
  constexpr int                THE_POINT_COUNT = 32;
  NCollection_Array1<gp_Pnt2d> aPnts(1, THE_POINT_COUNT);
  for (int anIdx = 0; anIdx < THE_POINT_COUNT; ++anIdx)
  {
    const double anAngle =
      2.0 * M_PI * static_cast<double>(anIdx) / static_cast<double>(THE_POINT_COUNT);
    const double aRadius = (anIdx % 4 == 1) ? 0.22 : ((anIdx % 2 == 0) ? 0.46 : 0.34);
    aPnts(anIdx + 1) =
      gp_Pnt2d(0.5 + aRadius * std::cos(anAngle), 0.5 + aRadius * std::sin(anAngle));
  }

  CSLib_Class2d aCached(aPnts, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0);
  CSLib_Class2d anExact(aPnts, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0);
  for (size_t aQueryIdx = 0; aQueryIdx < 64; ++aQueryIdx)
  {
    ASSERT_EQ(aCached.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  }

  for (int aY = 0; aY < 47; ++aY)
  {
    for (int anX = 0; anX < 47; ++anX)
    {
      const gp_Pnt2d aPoint((static_cast<double>(anX) + 0.37) / 47.0,
                            (static_cast<double>(aY) + 0.61) / 47.0);
      EXPECT_EQ(aCached.SiDans(aPoint), anExact.SiDans_OnMode(aPoint, 0.0))
        << "sample (" << anX << ", " << aY << ")";
    }
  }
}

TEST_F(CSLibClass2dTest, LazyGridConcurrentActivationQueriesAreThreadSafe)
{
  // Release concurrent queries together at the lazy-grid activation threshold.
  constexpr int                THE_POINT_COUNT = 32768;
  NCollection_Array1<gp_Pnt2d> aPnts(1, THE_POINT_COUNT);
  for (int anIdx = 0; anIdx < THE_POINT_COUNT; ++anIdx)
  {
    const double anAngle =
      2.0 * M_PI * static_cast<double>(anIdx) / static_cast<double>(THE_POINT_COUNT);
    aPnts(anIdx + 1) = gp_Pnt2d(0.5 + 0.4 * std::cos(anAngle), 0.5 + 0.4 * std::sin(anAngle));
  }

  CSLib_Class2d aClassifier(aPnts, 1.0e-8, 1.0e-8, 0.0, 0.0, 1.0, 1.0);
  for (size_t aQueryIdx = 0; aQueryIdx < 63; ++aQueryIdx)
  {
    ASSERT_EQ(aClassifier.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  }

  std::atomic<bool>          hasFailure{false};
  std::atomic<bool>          canStart{false};
  std::atomic<size_t>        aReadyCount{0};
  std::array<std::thread, 8> aQueryThreads;
  for (size_t aThreadIdx = 0; aThreadIdx < aQueryThreads.size(); ++aThreadIdx)
  {
    aQueryThreads[aThreadIdx] = std::thread([&]() {
      aReadyCount.fetch_add(1, std::memory_order_release);
      while (!canStart.load(std::memory_order_acquire))
      {
        std::this_thread::yield();
      }
      for (size_t aQueryIdx = 0; aQueryIdx < 8; ++aQueryIdx)
      {
        const bool                  isInside = (aQueryIdx & 1u) == 0u;
        const gp_Pnt2d              aPoint   = isInside ? gp_Pnt2d(0.5, 0.5) : gp_Pnt2d(0.99, 0.99);
        const CSLib_Class2d::Result anExpected =
          isInside ? CSLib_Class2d::Result_Inside : CSLib_Class2d::Result_Outside;
        if (aClassifier.SiDans(aPoint) != anExpected)
        {
          hasFailure.store(true, std::memory_order_relaxed);
          return;
        }
      }
    });
  }
  while (aReadyCount.load(std::memory_order_acquire) != aQueryThreads.size())
  {
    std::this_thread::yield();
  }
  canStart.store(true, std::memory_order_release);
  for (std::thread& aThread : aQueryThreads)
  {
    aThread.join();
  }
  EXPECT_FALSE(hasFailure.load(std::memory_order_relaxed));
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(0.99, 0.99)), CSLib_Class2d::Result_Outside);
}

TEST_F(CSLibClass2dTest, SiDansHandlesNegativeNonFiniteAndExtremeInputs)
{
  constexpr int                THE_POINT_COUNT     = 32;
  constexpr int                THE_POINTS_PER_SIDE = THE_POINT_COUNT / 4;
  NCollection_Array1<gp_Pnt2d> aPnts(1, THE_POINT_COUNT);
  for (int anIdx = 0; anIdx < THE_POINTS_PER_SIDE; ++anIdx)
  {
    const double aParameter                    = static_cast<double>(anIdx) / THE_POINTS_PER_SIDE;
    aPnts(1 + anIdx)                           = gp_Pnt2d(aParameter, 0.0);
    aPnts(1 + THE_POINTS_PER_SIDE + anIdx)     = gp_Pnt2d(1.0, aParameter);
    aPnts(1 + 2 * THE_POINTS_PER_SIDE + anIdx) = gp_Pnt2d(1.0 - aParameter, 1.0);
    aPnts(1 + 3 * THE_POINTS_PER_SIDE + anIdx) = gp_Pnt2d(0.0, 1.0 - aParameter);
  }

  CSLib_Class2d
    aNegative(aPnts, -1.0, -std::numeric_limits<double>::infinity(), 0.0, 0.0, 1.0, 1.0);
  EXPECT_EQ(aNegative.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  EXPECT_EQ(aNegative.SiDans_OnMode(gp_Pnt2d(1.5, 0.5), -1.0), CSLib_Class2d::Result_Outside);

  CSLib_Class2d anExtremeTolerance(aPnts,
                                   std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::max(),
                                   0.0,
                                   0.0,
                                   1.0,
                                   1.0);
  for (size_t aQueryIdx = 0; aQueryIdx < 80; ++aQueryIdx)
  {
    EXPECT_EQ(anExtremeTolerance.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Uncertain);
  }

  NCollection_Array1<gp_Pnt2d> anExtremePnts(1, 4);
  anExtremePnts(1) = gp_Pnt2d(-0.5 * Precision::Infinite(), -0.5 * Precision::Infinite());
  anExtremePnts(2) = gp_Pnt2d(0.5 * Precision::Infinite(), -0.5 * Precision::Infinite());
  anExtremePnts(3) = gp_Pnt2d(0.5 * Precision::Infinite(), 0.5 * Precision::Infinite());
  anExtremePnts(4) = gp_Pnt2d(-0.5 * Precision::Infinite(), 0.5 * Precision::Infinite());
  CSLib_Class2d anExtremeCoordinates(anExtremePnts,
                                     0.0,
                                     0.0,
                                     -Precision::Infinite(),
                                     -Precision::Infinite(),
                                     Precision::Infinite(),
                                     Precision::Infinite());
  EXPECT_EQ(anExtremeCoordinates.SiDans(gp_Pnt2d(0.0, 0.0)), CSLib_Class2d::Result_Inside);
  EXPECT_EQ(anExtremeCoordinates.SiDans(gp_Pnt2d(0.75 * Precision::Infinite(), 0.0)),
            CSLib_Class2d::Result_Outside);

  const double  aLargeTolerance = 0.1 * Precision::Infinite();
  CSLib_Class2d aTolerantExtreme(anExtremePnts,
                                 aLargeTolerance,
                                 aLargeTolerance,
                                 -Precision::Infinite(),
                                 -Precision::Infinite(),
                                 Precision::Infinite(),
                                 Precision::Infinite());
  EXPECT_EQ(aTolerantExtreme.SiDans(gp_Pnt2d(0.55 * Precision::Infinite(), 0.0)),
            CSLib_Class2d::Result_Uncertain);
  EXPECT_EQ(aTolerantExtreme.SiDans(gp_Pnt2d(0.65 * Precision::Infinite(), 0.0)),
            CSLib_Class2d::Result_Outside);
  EXPECT_EQ(anExtremeCoordinates.SiDans_OnMode(gp_Pnt2d(0.0, 0.55 * Precision::Infinite()),
                                               aLargeTolerance),
            CSLib_Class2d::Result_Uncertain);
}

TEST_F(CSLibClass2dTest, LazyGridPreservesToleranceAcrossCellBoundary)
{
  constexpr int                THE_POINTS_PER_SIDE = 8;
  constexpr int                THE_POINT_COUNT     = 4 * THE_POINTS_PER_SIDE;
  constexpr double             THE_MIN             = 0.25;
  constexpr double             THE_MAX             = 0.75;
  constexpr double             THE_TOLERANCE       = 1.0e-4;
  NCollection_Array1<gp_Pnt2d> aPnts(1, THE_POINT_COUNT);
  for (int anIdx = 0; anIdx < THE_POINTS_PER_SIDE; ++anIdx)
  {
    const double aParameter = static_cast<double>(anIdx) / static_cast<double>(THE_POINTS_PER_SIDE);
    aPnts(1 + anIdx)        = gp_Pnt2d(THE_MIN + (THE_MAX - THE_MIN) * aParameter, THE_MIN);
    aPnts(1 + THE_POINTS_PER_SIDE + anIdx) =
      gp_Pnt2d(THE_MAX, THE_MIN + (THE_MAX - THE_MIN) * aParameter);
    aPnts(1 + 2 * THE_POINTS_PER_SIDE + anIdx) =
      gp_Pnt2d(THE_MAX - (THE_MAX - THE_MIN) * aParameter, THE_MAX);
    aPnts(1 + 3 * THE_POINTS_PER_SIDE + anIdx) =
      gp_Pnt2d(THE_MIN, THE_MAX - (THE_MAX - THE_MIN) * aParameter);
  }

  CSLib_Class2d aClassifier(aPnts, THE_TOLERANCE, THE_TOLERANCE, 0.0, 0.0, 1.0, 1.0);
  for (size_t aQueryIdx = 0; aQueryIdx < 64; ++aQueryIdx)
  {
    ASSERT_EQ(aClassifier.SiDans(gp_Pnt2d(0.5, 0.5)), CSLib_Class2d::Result_Inside);
  }

  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(THE_MIN - 0.5 * THE_TOLERANCE, 0.5)),
            CSLib_Class2d::Result_Uncertain);
}

// Test SiDans_OnMode
TEST_F(CSLibClass2dTest, SiDans_OnMode_PointInside)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 1.0);
  aPnts(4) = gp_Pnt2d(0.0, 1.0);

  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 0.0, 0.0, 1.0, 1.0);

  const gp_Pnt2d aPointInside(0.5, 0.5);
  EXPECT_EQ(aClassifier.SiDans_OnMode(aPointInside, 0.01), 1);
}

TEST_F(CSLibClass2dTest, SiDans_OnMode_NormalizesAndReplacesToleranceOnAnisotropicDomain)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(20.0, -1.0);
  aPnts(2) = gp_Pnt2d(80.0, -1.0);
  aPnts(3) = gp_Pnt2d(80.0, 1.0);
  aPnts(4) = gp_Pnt2d(20.0, 1.0);

  // Constructor tolerances are deliberately larger than the explicit one.
  CSLib_Class2d aClassifier(aPnts, 10.0, 1.0, 0.0, -2.0, 100.0, 2.0);
  EXPECT_EQ(aClassifier.SiDans_OnMode(gp_Pnt2d(50.0, 1.5), 0.01), CSLib_Class2d::Result_Outside);

  // The same original-space tolerance has different normalized U/V values.
  EXPECT_EQ(aClassifier.SiDans_OnMode(gp_Pnt2d(19.75, 0.0), 0.5), CSLib_Class2d::Result_Uncertain);
  EXPECT_EQ(aClassifier.SiDans_OnMode(gp_Pnt2d(19.0, 0.0), 0.5), CSLib_Class2d::Result_Outside);
  EXPECT_EQ(aClassifier.SiDans_OnMode(gp_Pnt2d(50.0, 1.25), 0.5), CSLib_Class2d::Result_Uncertain);
  EXPECT_EQ(aClassifier.SiDans_OnMode(gp_Pnt2d(50.0, 1.75), 0.5), CSLib_Class2d::Result_Outside);
}

// Test with degenerate polygon (less than 3 points effective)
TEST_F(CSLibClass2dTest, DegeneratePolygon_InvalidBounds)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 4);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 0.0);
  aPnts(3) = gp_Pnt2d(1.0, 1.0);
  aPnts(4) = gp_Pnt2d(0.0, 1.0);

  // Invalid bounds (umax <= umin)
  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 1.0, 0.0, 0.0, 1.0);

  // Should return 0 for any point with invalid bounds
  const gp_Pnt2d aPoint(0.5, 0.5);
  EXPECT_EQ(aClassifier.SiDans(aPoint), 0);
}

//=================================================================================================
// CSLib_NormalPolyDef Tests
//=================================================================================================

class CSLibNormalPolyDefTest : public ::testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Test Value function
TEST_F(CSLibNormalPolyDefTest, Value_ConstantPolynomial)
{
  // k0 = 0, lambda_0 = 1.0 -> f(X) = 1 * cos^0 * sin^0 = 1 (constant)
  NCollection_Array1<double> aLambda(0, 0);
  aLambda(0) = 1.0;

  CSLib_NormalPolyDef aPoly(0, aLambda);

  double aValue;
  EXPECT_TRUE(aPoly.Value(0.5, aValue));
  EXPECT_NEAR(aValue, 1.0, 1e-10);

  EXPECT_TRUE(aPoly.Value(1.0, aValue));
  EXPECT_NEAR(aValue, 1.0, 1e-10);
}

TEST_F(CSLibNormalPolyDefTest, Value_LinearPolynomial)
{
  // k0 = 1, lambda = [1, 1] -> f(X) = cos(X) + sin(X)
  NCollection_Array1<double> aLambda(0, 1);
  aLambda(0) = 1.0;
  aLambda(1) = 1.0;

  CSLib_NormalPolyDef aPoly(1, aLambda);

  double aValue;

  // At X = pi/4, cos(X) = sin(X) = sqrt(2)/2
  // f(pi/4) = cos(pi/4) + sin(pi/4) = sqrt(2)
  EXPECT_TRUE(aPoly.Value(M_PI / 4.0, aValue));
  EXPECT_NEAR(aValue, std::sqrt(2.0), 1e-10);
}

TEST_F(CSLibNormalPolyDefTest, Value_AtSingularPoints)
{
  // At singular points (X=0, pi/2, pi, 3*pi/2), the function returns 0
  // as a safety measure when computing would involve 0^n terms.
  // The early return protects against numerical issues.
  NCollection_Array1<double> aLambda(0, 2);
  aLambda(0) = 1.0;
  aLambda(1) = 1.0;
  aLambda(2) = 1.0;

  CSLib_NormalPolyDef aPoly(2, aLambda);

  double aValue;

  // At X = 0, sin(0) = 0; at X = pi/2, cos(pi/2) ~ 0
  // The function is designed to return 0 for these edge cases
  // to avoid numerical issues, but actual return depends on
  // the tolerance check RealSmall().
  // Test that the function doesn't crash at these points.
  EXPECT_TRUE(aPoly.Value(0.0, aValue));
  EXPECT_FALSE(std::isnan(aValue));
  EXPECT_FALSE(Precision::IsInfinite(aValue));

  EXPECT_TRUE(aPoly.Value(M_PI / 2.0, aValue));
  EXPECT_FALSE(std::isnan(aValue));
  EXPECT_FALSE(Precision::IsInfinite(aValue));
}

// Test Derivative function
TEST_F(CSLibNormalPolyDefTest, Derivative_AtRegularPoint)
{
  // k0 = 2, test derivative at non-singular point
  NCollection_Array1<double> aLambda(0, 2);
  aLambda(0) = 1.0;
  aLambda(1) = 0.0;
  aLambda(2) = 1.0;

  CSLib_NormalPolyDef aPoly(2, aLambda);

  double aDeriv;
  EXPECT_TRUE(aPoly.Derivative(M_PI / 4.0, aDeriv));
  // Derivative should be computed without crash
  EXPECT_FALSE(std::isnan(aDeriv));
  EXPECT_FALSE(Precision::IsInfinite(aDeriv));
}

TEST_F(CSLibNormalPolyDefTest, Derivative_AtSingularPoint)
{
  NCollection_Array1<double> aLambda(0, 2);
  aLambda(0) = 1.0;
  aLambda(1) = 1.0;
  aLambda(2) = 1.0;

  CSLib_NormalPolyDef aPoly(2, aLambda);

  double aDeriv;

  // At singular points, derivative should be 0
  EXPECT_TRUE(aPoly.Derivative(0.0, aDeriv));
  EXPECT_NEAR(aDeriv, 0.0, 1e-10);
}

// Test Values function (combined value and derivative)
TEST_F(CSLibNormalPolyDefTest, Values_Consistency)
{
  NCollection_Array1<double> aLambda(0, 2);
  aLambda(0) = 1.0;
  aLambda(1) = 2.0;
  aLambda(2) = 1.0;

  CSLib_NormalPolyDef aPoly(2, aLambda);

  double aF1, aD1;
  double aF2, aD2;

  // Get values using separate functions
  EXPECT_TRUE(aPoly.Value(0.7, aF1));
  EXPECT_TRUE(aPoly.Derivative(0.7, aD1));

  // Get values using combined function
  EXPECT_TRUE(aPoly.Values(0.7, aF2, aD2));

  // Results should be consistent
  EXPECT_NEAR(aF1, aF2, 1e-10);
  EXPECT_NEAR(aD1, aD2, 1e-10);
}

// Test numerical derivative approximation
TEST_F(CSLibNormalPolyDefTest, Derivative_NumericalApproximation)
{
  NCollection_Array1<double> aLambda(0, 2);
  aLambda(0) = 1.0;
  aLambda(1) = 2.0;
  aLambda(2) = 3.0;

  CSLib_NormalPolyDef aPoly(2, aLambda);

  const double aX = 0.8; // Away from singular points
  const double aH = 1e-6;

  double aFMinus, aFPlus, aAnalyticDeriv;

  EXPECT_TRUE(aPoly.Value(aX - aH, aFMinus));
  EXPECT_TRUE(aPoly.Value(aX + aH, aFPlus));
  EXPECT_TRUE(aPoly.Derivative(aX, aAnalyticDeriv));

  const double aNumericDeriv = (aFPlus - aFMinus) / (2.0 * aH);

  // Analytical derivative should match numerical approximation
  EXPECT_NEAR(aAnalyticDeriv, aNumericDeriv, 1e-4);
}

//=================================================================================================
// CSLib Enumeration Tests
//=================================================================================================

TEST(CSLibEnumTest, DerivativeStatus_AllValues)
{
  // Verify all enum values are distinct
  EXPECT_NE(CSLib_Done, CSLib_D1uIsNull);
  EXPECT_NE(CSLib_Done, CSLib_D1vIsNull);
  EXPECT_NE(CSLib_Done, CSLib_D1IsNull);
  EXPECT_NE(CSLib_Done, CSLib_D1uD1vRatioIsNull);
  EXPECT_NE(CSLib_Done, CSLib_D1vD1uRatioIsNull);
  EXPECT_NE(CSLib_Done, CSLib_D1uIsParallelD1v);
}

TEST(CSLibEnumTest, NormalStatus_AllValues)
{
  // Verify all enum values are distinct
  EXPECT_NE(CSLib_Singular, CSLib_Defined);
  EXPECT_NE(CSLib_Singular, CSLib_InfinityOfSolutions);
  EXPECT_NE(CSLib_Singular, CSLib_D1NuIsNull);
  EXPECT_NE(CSLib_Singular, CSLib_D1NvIsNull);
  EXPECT_NE(CSLib_Singular, CSLib_D1NIsNull);
  EXPECT_NE(CSLib_Singular, CSLib_D1NuNvRatioIsNull);
  EXPECT_NE(CSLib_Singular, CSLib_D1NvNuRatioIsNull);
  EXPECT_NE(CSLib_Singular, CSLib_D1NuIsParallelD1Nv);
}

//=================================================================================================
// CSLib Integration Tests
//=================================================================================================

class CSLibIntegrationTest : public ::testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Test normal computation for a planar surface
TEST_F(CSLibIntegrationTest, PlanarSurface_ConstantNormal)
{
  // For a plane z = 0, D1U = (1,0,0), D1V = (0,1,0)
  // Normal should be (0,0,1) at all points
  const gp_Vec aD1U(1.0, 0.0, 0.0);
  const gp_Vec aD1V(0.0, 1.0, 0.0);

  CSLib_DerivativeStatus aStatus;
  gp_Dir                 aNormal;

  CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

  EXPECT_EQ(aStatus, CSLib_Done);
  checkDirEqual(aNormal, gp_Dir(0.0, 0.0, 1.0));
}

// Test normal computation for a cylindrical surface
TEST_F(CSLibIntegrationTest, CylindricalSurface_VaryingNormal)
{
  // For a cylinder x = cos(u), y = sin(u), z = v
  // At u = 0: D1U = (0,1,0), D1V = (0,0,1), Normal = (1,0,0)
  // At u = pi/2: D1U = (-1,0,0), D1V = (0,0,1), Normal = (0,1,0)

  // At u = 0
  {
    const gp_Vec aD1U(0.0, 1.0, 0.0);
    const gp_Vec aD1V(0.0, 0.0, 1.0);

    CSLib_DerivativeStatus aStatus;
    gp_Dir                 aNormal;

    CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

    EXPECT_EQ(aStatus, CSLib_Done);
    checkDirEqual(aNormal, gp_Dir(1.0, 0.0, 0.0));
  }

  // At u = pi/2
  {
    const gp_Vec aD1U(-1.0, 0.0, 0.0);
    const gp_Vec aD1V(0.0, 0.0, 1.0);

    CSLib_DerivativeStatus aStatus;
    gp_Dir                 aNormal;

    CSLib::Normal(aD1U, aD1V, 1e-10, aStatus, aNormal);

    EXPECT_EQ(aStatus, CSLib_Done);
    checkDirEqual(aNormal, gp_Dir(0.0, 1.0, 0.0));
  }
}

// Test Class2d with complex polygon
TEST_F(CSLibIntegrationTest, Class2d_ComplexPolygon)
{
  // L-shaped polygon
  NCollection_Array1<gp_Pnt2d> aPnts(1, 6);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(2.0, 0.0);
  aPnts(3) = gp_Pnt2d(2.0, 1.0);
  aPnts(4) = gp_Pnt2d(1.0, 1.0);
  aPnts(5) = gp_Pnt2d(1.0, 2.0);
  aPnts(6) = gp_Pnt2d(0.0, 2.0);

  CSLib_Class2d aClassifier(aPnts, 1e-10, 1e-10, 0.0, 0.0, 2.0, 2.0);

  // Point inside the L
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(0.5, 0.5)), 1);

  // Point in the "notch" of the L (outside)
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(1.5, 1.5)), -1);

  // Point clearly outside
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(3.0, 3.0)), -1);
}

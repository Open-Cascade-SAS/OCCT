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

// New ExtremaCC implementation
#include <ExtremaCC_Curves.hxx>

// Old Extrema_ExtCC implementation
#include <Extrema_ExtCC.hxx>

// Geometry includes
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <random>

//! Test fixture for comparison tests between ExtremaCC and Extrema_ExtCC.
class ExtremaCC_ComparisonTest : public testing::Test
{
protected:
  static constexpr double THE_TOL     = 1.0e-6;
  static constexpr double THE_TOL_REL = 1.0e-4;

  //! Compare minimum distances found by both implementations.
  void CompareMinDistance(const ExtremaCC::Result& theNew,
                          Extrema_ExtCC&           theOld,
                          const char*              theTestName)
  {
    // Both must complete successfully
    ASSERT_TRUE(theNew.IsDone()) << theTestName << ": New implementation failed";
    ASSERT_TRUE(theOld.IsDone()) << theTestName << ": Old implementation failed";

    // Handle parallel/infinite solutions case
    if (theOld.IsParallel())
    {
      EXPECT_TRUE(theNew.IsInfinite()) << theTestName << ": Old reports parallel, new should too";
      return;
    }

    if (theOld.NbExt() == 0 && theNew.NbExt() == 0)
    {
      return; // Both found no extrema
    }

    // Find minimum from old implementation
    double aOldMinSqDist = RealLast();
    int    aOldMinIdx    = -1;
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      double aSqDist = theOld.SquareDistance(i);
      if (aSqDist < aOldMinSqDist)
      {
        aOldMinSqDist = aSqDist;
        aOldMinIdx    = i;
      }
    }

    // Find minimum from new implementation
    double aNewMinSqDist = theNew.MinSquareDistance();
    int    aNewMinIdx    = theNew.MinIndex();

    ASSERT_GE(aOldMinIdx, 1) << theTestName << ": Old implementation found no minimum";
    ASSERT_GE(aNewMinIdx, 0) << theTestName << ": New implementation found no minimum";

    // Compare minimum distances
    double aOldDist = std::sqrt(aOldMinSqDist);
    double aNewDist = std::sqrt(aNewMinSqDist);

    if (aOldDist > THE_TOL)
    {
      double aRelDiff = std::abs(aNewDist - aOldDist) / aOldDist;
      EXPECT_LT(aRelDiff, THE_TOL_REL) << theTestName << ": Distance mismatch - Old: " << aOldDist
                                       << ", New: " << aNewDist << ", RelDiff: " << aRelDiff;
    }
    else
    {
      EXPECT_NEAR(aNewDist, aOldDist, THE_TOL)
        << theTestName << ": Distance mismatch - Old: " << aOldDist << ", New: " << aNewDist;
    }
  }
};

//==================================================================================================
// Line-Line comparison tests
//==================================================================================================

TEST_F(ExtremaCC_ComparisonTest, LineLine_Intersecting)
{
  gp_Lin aLin1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLin2(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0));

  occ::handle<Geom_Line> aGeomLin1 = new Geom_Line(aLin1);
  occ::handle<Geom_Line> aGeomLin2 = new Geom_Line(aLin2);
  GeomAdaptor_Curve      anAdaptor1(aGeomLin1, -10.0, 10.0);
  GeomAdaptor_Curve      anAdaptor2(aGeomLin2, -10.0, 10.0);

  // New implementation
  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  // Old implementation
  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "LineLine_Intersecting");
}

TEST_F(ExtremaCC_ComparisonTest, LineLine_Skew)
{
  gp_Lin aLin1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLin2(gp_Pnt(0, 5, 3), gp_Dir(0, 1, 0));

  occ::handle<Geom_Line> aGeomLin1 = new Geom_Line(aLin1);
  occ::handle<Geom_Line> aGeomLin2 = new Geom_Line(aLin2);
  GeomAdaptor_Curve      anAdaptor1(aGeomLin1, -10.0, 10.0);
  GeomAdaptor_Curve      anAdaptor2(aGeomLin2, -10.0, 10.0);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "LineLine_Skew");
}

//==================================================================================================
// Circle-Line comparison tests
//==================================================================================================

TEST_F(ExtremaCC_ComparisonTest, CircleLine_Intersecting)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  occ::handle<Geom_Circle> aGeomCirc = new Geom_Circle(aCirc);
  occ::handle<Geom_Line>   aGeomLin  = new Geom_Line(aLin);
  GeomAdaptor_Curve        anAdaptor1(aGeomCirc);
  GeomAdaptor_Curve        anAdaptor2(aGeomLin, -20.0, 20.0);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "CircleLine_Intersecting");
}

TEST_F(ExtremaCC_ComparisonTest, CircleLine_Skew)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLin(gp_Pnt(0, 0, 5), gp_Dir(1, 1, 0));

  occ::handle<Geom_Circle> aGeomCirc = new Geom_Circle(aCirc);
  occ::handle<Geom_Line>   aGeomLin  = new Geom_Line(aLin);
  GeomAdaptor_Curve        anAdaptor1(aGeomCirc);
  GeomAdaptor_Curve        anAdaptor2(aGeomLin, -20.0, 20.0);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "CircleLine_Skew");
}

//==================================================================================================
// Circle-Circle comparison tests
//==================================================================================================

TEST_F(ExtremaCC_ComparisonTest, CircleCircle_Coplanar)
{
  gp_Circ aCirc1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCirc2(gp_Ax2(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  occ::handle<Geom_Circle> aGeomCirc1 = new Geom_Circle(aCirc1);
  occ::handle<Geom_Circle> aGeomCirc2 = new Geom_Circle(aCirc2);
  GeomAdaptor_Curve        anAdaptor1(aGeomCirc1);
  GeomAdaptor_Curve        anAdaptor2(aGeomCirc2);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "CircleCircle_Coplanar");
}

TEST_F(ExtremaCC_ComparisonTest, CircleCircle_NonCoplanar)
{
  gp_Circ aCirc1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCirc2(gp_Ax2(gp_Pnt(15, 0, 5), gp_Dir(1, 0, 0)), 5.0);

  occ::handle<Geom_Circle> aGeomCirc1 = new Geom_Circle(aCirc1);
  occ::handle<Geom_Circle> aGeomCirc2 = new Geom_Circle(aCirc2);
  GeomAdaptor_Curve        anAdaptor1(aGeomCirc1);
  GeomAdaptor_Curve        anAdaptor2(aGeomCirc2);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "CircleCircle_NonCoplanar");
}

//==================================================================================================
// Ellipse-Line comparison tests
//==================================================================================================

TEST_F(ExtremaCC_ComparisonTest, EllipseLine)
{
  gp_Elips anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Lin   aLin(gp_Pnt(30, 0, 0), gp_Dir(0, 1, 0));

  occ::handle<Geom_Ellipse> aGeomElips = new Geom_Ellipse(anElips);
  occ::handle<Geom_Line>    aGeomLin   = new Geom_Line(aLin);
  GeomAdaptor_Curve         anAdaptor1(aGeomElips);
  GeomAdaptor_Curve         anAdaptor2(aGeomLin, -30.0, 30.0);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "EllipseLine");
}

//==================================================================================================
// BSpline comparison tests
//==================================================================================================

TEST_F(ExtremaCC_ComparisonTest, BSplineLine)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 2, 0);
  aPoles(4) = gp_Pnt(3, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  gp_Lin                         aLin(gp_Pnt(1.5, 5, 0), gp_Dir(0, -1, 0));
  occ::handle<Geom_Line>         aGeomLin = new Geom_Line(aLin);

  GeomAdaptor_Curve anAdaptor1(aBSpline);
  GeomAdaptor_Curve anAdaptor2(aGeomLin, -10.0, 10.0);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "BSplineLine");
}

TEST_F(ExtremaCC_ComparisonTest, BSplineBSpline)
{
  // First BSpline - horizontal curve at y=0
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(0, 0, 0);
  aPoles1(2) = gp_Pnt(1, 1, 0);  // Slight upward curve
  aPoles1(3) = gp_Pnt(2, 1, 0);
  aPoles1(4) = gp_Pnt(3, 0, 0);

  NCollection_Array1<double> aKnots1(1, 2);
  aKnots1(1) = 0.0;
  aKnots1(2) = 1.0;

  NCollection_Array1<int> aMults1(1, 2);
  aMults1(1) = 4;
  aMults1(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline1 = new Geom_BSplineCurve(aPoles1, aKnots1, aMults1, 3);

  // Second BSpline - directly above at y=5, but dipping down in the middle
  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(0, 5, 0);
  aPoles2(2) = gp_Pnt(1, 3, 0);  // Dips down toward first curve
  aPoles2(3) = gp_Pnt(2, 3, 0);
  aPoles2(4) = gp_Pnt(3, 5, 0);

  NCollection_Array1<double> aKnots2(1, 2);
  aKnots2(1) = 0.0;
  aKnots2(2) = 1.0;

  NCollection_Array1<int> aMults2(1, 2);
  aMults2(1) = 4;
  aMults2(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline2 = new Geom_BSplineCurve(aPoles2, aKnots2, aMults2, 3);

  GeomAdaptor_Curve anAdaptor1(aBSpline1);
  GeomAdaptor_Curve anAdaptor2(aBSpline2);

  ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
  const ExtremaCC::Result& aNewResult = anExtCC.Perform(THE_TOL);

  Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);

  CompareMinDistance(aNewResult, anOldExtCC, "BSplineBSpline");
}

//==================================================================================================
// Performance comparison tests
//==================================================================================================

TEST_F(ExtremaCC_ComparisonTest, Performance_CircleCircle)
{
  gp_Circ aCirc1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCirc2(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 5.0);

  occ::handle<Geom_Circle> aGeomCirc1 = new Geom_Circle(aCirc1);
  occ::handle<Geom_Circle> aGeomCirc2 = new Geom_Circle(aCirc2);

  std::mt19937                     aGen(12345);
  std::uniform_real_distribution<> aDist(-20.0, 20.0);

  const int aNumIterations = 1000;

  // Generate random circle positions
  std::vector<gp_Pnt> aPositions;
  aPositions.reserve(aNumIterations);
  for (int i = 0; i < aNumIterations; ++i)
  {
    aPositions.emplace_back(aDist(aGen), aDist(aGen), aDist(aGen));
  }

  // Time new implementation
  auto aStartNew = std::chrono::high_resolution_clock::now();
  for (const auto& aPos : aPositions)
  {
    gp_Circ aCirc2Moved(gp_Ax2(aPos, gp_Dir(1, 0, 0)), 5.0);
    occ::handle<Geom_Circle> aGeomCirc2Moved = new Geom_Circle(aCirc2Moved);
    GeomAdaptor_Curve        anAdaptor1(aGeomCirc1);
    GeomAdaptor_Curve        anAdaptor2(aGeomCirc2Moved);

    ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
    const ExtremaCC::Result& aResult = anExtCC.Perform(THE_TOL);
    (void)aResult;
  }
  auto aEndNew      = std::chrono::high_resolution_clock::now();
  auto aDurationNew = std::chrono::duration_cast<std::chrono::microseconds>(aEndNew - aStartNew);

  // Time old implementation
  auto aStartOld = std::chrono::high_resolution_clock::now();
  for (const auto& aPos : aPositions)
  {
    gp_Circ aCirc2Moved(gp_Ax2(aPos, gp_Dir(1, 0, 0)), 5.0);
    occ::handle<Geom_Circle> aGeomCirc2Moved = new Geom_Circle(aCirc2Moved);
    GeomAdaptor_Curve        anAdaptor1(aGeomCirc1);
    GeomAdaptor_Curve        anAdaptor2(aGeomCirc2Moved);

    Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);
    (void)anOldExtCC;
  }
  auto aEndOld      = std::chrono::high_resolution_clock::now();
  auto aDurationOld = std::chrono::duration_cast<std::chrono::microseconds>(aEndOld - aStartOld);

  std::cout << "[          ] Performance_CircleCircle (" << aNumIterations
            << " iterations):" << std::endl;
  std::cout << "[          ]   Old Extrema_ExtCC: " << aDurationOld.count() << " us"
            << " (" << aDurationOld.count() / aNumIterations << " us/query)" << std::endl;
  std::cout << "[          ]   New ExtremaCC:     " << aDurationNew.count() << " us"
            << " (" << aDurationNew.count() / aNumIterations << " us/query)" << std::endl;
  std::cout << "[          ]   Speedup:           " << std::fixed << std::setprecision(2)
            << static_cast<double>(aDurationOld.count()) / aDurationNew.count() << "x" << std::endl;

  SUCCEED();
}

TEST_F(ExtremaCC_ComparisonTest, Performance_LineLine)
{
  std::mt19937                     aGen(54321);
  std::uniform_real_distribution<> aDist(-50.0, 50.0);
  std::uniform_real_distribution<> aDirDist(-1.0, 1.0);

  const int aNumIterations = 10000;

  // Pre-generate random lines
  std::vector<std::pair<gp_Lin, gp_Lin>> aLinePairs;
  aLinePairs.reserve(aNumIterations);
  for (int i = 0; i < aNumIterations; ++i)
  {
    gp_Pnt aP1(aDist(aGen), aDist(aGen), aDist(aGen));
    gp_Dir aD1(aDirDist(aGen) + 0.1, aDirDist(aGen), aDirDist(aGen));
    gp_Pnt aP2(aDist(aGen), aDist(aGen), aDist(aGen));
    gp_Dir aD2(aDirDist(aGen), aDirDist(aGen) + 0.1, aDirDist(aGen));
    aLinePairs.emplace_back(gp_Lin(aP1, aD1), gp_Lin(aP2, aD2));
  }

  // Time new implementation
  auto aStartNew = std::chrono::high_resolution_clock::now();
  for (const auto& aPair : aLinePairs)
  {
    occ::handle<Geom_Line> aGeomLin1 = new Geom_Line(aPair.first);
    occ::handle<Geom_Line> aGeomLin2 = new Geom_Line(aPair.second);
    GeomAdaptor_Curve      anAdaptor1(aGeomLin1, -100.0, 100.0);
    GeomAdaptor_Curve      anAdaptor2(aGeomLin2, -100.0, 100.0);

    ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
    const ExtremaCC::Result& aResult = anExtCC.Perform(THE_TOL);
    (void)aResult;
  }
  auto aEndNew      = std::chrono::high_resolution_clock::now();
  auto aDurationNew = std::chrono::duration_cast<std::chrono::microseconds>(aEndNew - aStartNew);

  // Time old implementation
  auto aStartOld = std::chrono::high_resolution_clock::now();
  for (const auto& aPair : aLinePairs)
  {
    occ::handle<Geom_Line> aGeomLin1 = new Geom_Line(aPair.first);
    occ::handle<Geom_Line> aGeomLin2 = new Geom_Line(aPair.second);
    GeomAdaptor_Curve      anAdaptor1(aGeomLin1, -100.0, 100.0);
    GeomAdaptor_Curve      anAdaptor2(aGeomLin2, -100.0, 100.0);

    Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);
    (void)anOldExtCC;
  }
  auto aEndOld      = std::chrono::high_resolution_clock::now();
  auto aDurationOld = std::chrono::duration_cast<std::chrono::microseconds>(aEndOld - aStartOld);

  std::cout << "[          ] Performance_LineLine (" << aNumIterations
            << " iterations):" << std::endl;
  std::cout << "[          ]   Old Extrema_ExtCC: " << aDurationOld.count() << " us"
            << " (" << aDurationOld.count() / aNumIterations << " us/query)" << std::endl;
  std::cout << "[          ]   New ExtremaCC:     " << aDurationNew.count() << " us"
            << " (" << aDurationNew.count() / aNumIterations << " us/query)" << std::endl;
  std::cout << "[          ]   Speedup:           " << std::fixed << std::setprecision(2)
            << static_cast<double>(aDurationOld.count()) / aDurationNew.count() << "x" << std::endl;

  SUCCEED();
}

TEST_F(ExtremaCC_ComparisonTest, Performance_BSplineBSpline)
{
  // Create two cubic BSpline curves (degree 3)
  // For degree 3 with 4 poles: need 4 + 3 + 1 = 8 knots (counted with multiplicity)
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(0, 0, 0);
  aPoles1(2) = gp_Pnt(2, 3, 1);
  aPoles1(3) = gp_Pnt(4, 3, 2);
  aPoles1(4) = gp_Pnt(6, 0, 1);

  NCollection_Array1<double> aKnots1(1, 2);
  aKnots1(1) = 0.0;
  aKnots1(2) = 1.0;

  NCollection_Array1<int> aMults1(1, 2);
  aMults1(1) = 4;  // degree + 1 = 4
  aMults1(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline1 = new Geom_BSplineCurve(aPoles1, aKnots1, aMults1, 3);

  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(1, 5, 0);
  aPoles2(2) = gp_Pnt(3, 8, 1);
  aPoles2(3) = gp_Pnt(5, 8, 2);
  aPoles2(4) = gp_Pnt(7, 5, 1);

  NCollection_Array1<double> aKnots2(1, 2);
  aKnots2(1) = 0.0;
  aKnots2(2) = 1.0;

  NCollection_Array1<int> aMults2(1, 2);
  aMults2(1) = 4;
  aMults2(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline2 = new Geom_BSplineCurve(aPoles2, aKnots2, aMults2, 3);

  GeomAdaptor_Curve anAdaptor1(aBSpline1);
  GeomAdaptor_Curve anAdaptor2(aBSpline2);

  const int aNumIterations = 1000;

  // Time new implementation
  auto aStartNew = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNumIterations; ++i)
  {
    ExtremaCC_Curves         anExtCC(anAdaptor1, anAdaptor2);
    const ExtremaCC::Result& aResult = anExtCC.Perform(THE_TOL);
    (void)aResult;
  }
  auto aEndNew      = std::chrono::high_resolution_clock::now();
  auto aDurationNew = std::chrono::duration_cast<std::chrono::microseconds>(aEndNew - aStartNew);

  // Time old implementation
  auto aStartOld = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNumIterations; ++i)
  {
    Extrema_ExtCC anOldExtCC(anAdaptor1, anAdaptor2);
    (void)anOldExtCC;
  }
  auto aEndOld      = std::chrono::high_resolution_clock::now();
  auto aDurationOld = std::chrono::duration_cast<std::chrono::microseconds>(aEndOld - aStartOld);

  std::cout << "[          ] Performance_BSplineBSpline (" << aNumIterations
            << " iterations):" << std::endl;
  std::cout << "[          ]   Old Extrema_ExtCC: " << aDurationOld.count() << " us"
            << " (" << aDurationOld.count() / aNumIterations << " us/query)" << std::endl;
  std::cout << "[          ]   New ExtremaCC:     " << aDurationNew.count() << " us"
            << " (" << aDurationNew.count() / aNumIterations << " us/query)" << std::endl;
  std::cout << "[          ]   Speedup:           " << std::fixed << std::setprecision(2)
            << static_cast<double>(aDurationOld.count()) / aDurationNew.count() << "x" << std::endl;

  SUCCEED();
}

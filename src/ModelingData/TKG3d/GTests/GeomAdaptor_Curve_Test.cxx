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

#include <Adaptor3d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

#include <thread>
#include <vector>

//=================================================================================================
// Test fixture for GeomAdaptor_Curve degenerated curve handling
//=================================================================================================

class GeomAdaptor_Curve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple 3D line for testing
    gp_Pnt aP1(0.0, 0.0, 0.0);
    gp_Lin aLine(aP1, gp_Dir(1.0, 1.0, 0.0));
    myLine = new Geom_Line(aLine);

    // Create a 3D circle for testing
    gp_Pnt  aCenter(5.0, 5.0, 0.0);
    gp_Circ aCirc(gp_Ax2(aCenter, gp_Dir(0.0, 0.0, 1.0)), 3.0);
    myCircle = new Geom_Circle(aCirc);
  }

  occ::handle<Geom_Line>   myLine;
  occ::handle<Geom_Circle> myCircle;
};

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_ValidParameters_Success)
{
  // Test loading with valid parameters
  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, 0.0, 10.0));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), 10.0);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_EqualParameters_Success)
{
  // Test loading with equal parameters (degenerated curve)
  // This should be allowed as it represents a point
  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, 5.0, 5.0));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), 5.0);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), 5.0);

  // Verify it evaluates to a single point
  gp_Pnt aP1 = anAdaptor.Value(5.0);
  gp_Pnt aP2 = myLine->Value(5.0);
  EXPECT_TRUE(aP1.IsEqual(aP2, Precision::Confusion()));
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_ParametersWithinConfusion_Success)
{
  // Test loading with parameters within Precision::Confusion()
  // This should be allowed (degenerated curve handling)
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 + Precision::Confusion() * 0.5;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_ParametersAtConfusionBoundary_Success)
{
  // Test loading with parameters exactly at the confusion tolerance boundary
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 + Precision::Confusion();

  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_FirstGreaterThanLastWithinConfusion_Success)
{
  // Test loading with theUFirst > theULast but within Precision::Confusion()
  // This represents a degenerated curve and should be allowed
  const double aParam1 = 5.0 + Precision::Confusion() * 0.5;
  const double aParam2 = 5.0;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_FirstGreaterThanLastBeyondConfusion_ThrowsException)
{
  // Test loading with UFirst > ULast + Precision::Confusion()
  // This should throw Standard_ConstructionError
  const double aParam1 = 10.0;
  const double aParam2 = 5.0;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_THROW(anAdaptor.Load(myLine, aParam1, aParam2), Standard_ConstructionError);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_FirstSlightlyGreaterThanLast_ThrowsException)
{
  // Test loading with UFirst slightly greater than ULast beyond tolerance
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 - Precision::Confusion() * 2.0;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_THROW(anAdaptor.Load(myLine, aParam1, aParam2), Standard_ConstructionError);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Constructor_DegeneratedCurve_Success)
{
  // Test constructor with degenerated curve parameters
  EXPECT_NO_THROW(GeomAdaptor_Curve anAdaptor(myCircle, 0.0, 0.0));

  GeomAdaptor_Curve anAdaptor(myCircle, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), 0.0);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Constructor_InvalidParameters_ThrowsException)
{
  // Test constructor with invalid parameters
  EXPECT_THROW(GeomAdaptor_Curve anAdaptor(myCircle, 10.0, 0.0), Standard_ConstructionError);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Load_NullCurve_ThrowsException)
{
  // Test loading with null curve
  occ::handle<Geom_Curve> aNullCurve;
  GeomAdaptor_Curve       anAdaptor;

  EXPECT_THROW(anAdaptor.Load(aNullCurve, 0.0, 10.0), Standard_NullObject);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, DegeneratedCurve_CircleAtZeroLength_Success)
{
  // Test degenerated circle (zero length arc)
  const double aParam = M_PI;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myCircle, aParam, aParam));

  // Verify the curve represents a single point
  gp_Pnt aPoint         = anAdaptor.Value(aParam);
  gp_Pnt aExpectedPoint = myCircle->Value(aParam);

  EXPECT_TRUE(aPoint.IsEqual(aExpectedPoint, Precision::Confusion()));
  EXPECT_TRUE(anAdaptor.IsClosed() || anAdaptor.FirstParameter() == anAdaptor.LastParameter());
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, DegeneratedCurve_TrimmedCurve_Success)
{
  // Create a trimmed curve and test degenerated case
  occ::handle<Geom_TrimmedCurve> aTrimmedCurve = new Geom_TrimmedCurve(myLine, 0.0, 20.0);

  const double      aParam = 10.0;
  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(aTrimmedCurve, aParam, aParam));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, ToleranceBoundary_NegativeCase_ThrowsException)
{
  // Test parameters just beyond the negative tolerance boundary
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 - Precision::Confusion() - 1e-10;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_THROW(anAdaptor.Load(myLine, aParam1, aParam2), Standard_ConstructionError);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, LoadWithoutParameters_Success)
{
  // Test loading curve without specifying parameters (uses curve's own parameters)
  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myCircle));

  EXPECT_NEAR(anAdaptor.FirstParameter(), myCircle->FirstParameter(), Precision::Confusion());
  EXPECT_NEAR(anAdaptor.LastParameter(), myCircle->LastParameter(), Precision::Confusion());
  EXPECT_TRUE(anAdaptor.IsPeriodic());
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, DegeneratedCurve_MultipleLocations_Success)
{
  // Test degenerated curves at different parameter locations
  const double aParams[] = {0.0, 1.0, -5.0, 100.0, M_PI};

  for (const double aParam : aParams)
  {
    GeomAdaptor_Curve anAdaptor;
    EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam, aParam));

    gp_Pnt aPoint1 = anAdaptor.Value(aParam);
    gp_Pnt aPoint2 = myLine->Value(aParam);
    EXPECT_TRUE(aPoint1.IsEqual(aPoint2, Precision::Confusion()));
  }
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, BoundaryConditions_VerySmallInterval_Success)
{
  // Test with very small but valid interval (just above tolerance)
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 + Precision::Confusion() + 1e-12;

  GeomAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//=================================================================================================

TEST_F(GeomAdaptor_Curve_Test, Constructor_WithValidRange_Success)
{
  // Test the 3-parameter constructor with valid range
  const double aFirst = 0.0;
  const double aLast  = 2.0 * M_PI;

  EXPECT_NO_THROW(GeomAdaptor_Curve anAdaptor(myCircle, aFirst, aLast));

  GeomAdaptor_Curve anAdaptor(myCircle, aFirst, aLast);
  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aFirst);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aLast);
  EXPECT_EQ(anAdaptor.GetType(), GeomAbs_Circle);
}

//=================================================================================================
// Concurrency
//=================================================================================================

namespace
{
// File scope so the worker lambda reads them without capturing: MSVC rejects an uncaptured
// constexpr local and Clang with -Werror rejects a captured one.
constexpr int THE_NB_THREADS = 8;
constexpr int THE_NB_EVALS   = 400;
} // namespace

// The supported way to evaluate one curve from several threads is one adaptor per thread.
//
// GeomAdaptor_Curve keeps the polynomial coefficients of the span it last evaluated, and that cache
// is deliberately local to the adaptor and unsynchronised: it is not shared between instances and
// takes no lock. Adaptor3d_Curve::ShallowCopy() is the per-thread constructor for it, and for
// GeomAdaptor_Curve it deliberately leaves the cache behind, so a copy starts empty and a thread
// never sees another's span. Copying from a shared adaptor is therefore itself race-free provided
// nothing evaluates that shared adaptor, which is what this test does.
//
// Each thread evaluates alternating ends of a three-span curve, so consecutive evaluations land in
// different spans and every one forces a cache rebuild, and checks the result against the curve's
// own evaluation, which does not go through an adaptor cache.
TEST_F(GeomAdaptor_Curve_Test, PerThreadShallowCopyEvaluatesCorrectlyAcrossSpans)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, -1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 3.0, 0.0);
  aPoles(5) = gp_Pnt(4.0, 0.0, 0.0);
  aPoles(6) = gp_Pnt(5.0, 2.0, 0.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.34;
  aKnots(3) = 0.67;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 4;

  const occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  const GeomAdaptor_Curve              aShared(aCurve);

  std::vector<std::thread> aThreads;
  std::vector<int>         aMismatches(THE_NB_THREADS, 0);
  for (int aThreadIndex = 0; aThreadIndex < THE_NB_THREADS; ++aThreadIndex)
  {
    aThreads.emplace_back([&aShared, &aCurve, &aMismatches, aThreadIndex]() {
      const occ::handle<Adaptor3d_Curve> aLocal = aShared.ShallowCopy();
      for (int anIter = 0; anIter < THE_NB_EVALS; ++anIter)
      {
        const double aParam = ((aThreadIndex + anIter) % 2 == 0) ? 0.05 + 0.001 * (anIter % 100)
                                                                 : 0.95 - 0.001 * (anIter % 100);
        if (aLocal->Value(aParam).Distance(aCurve->EvalD0(aParam)) > 1.0e-9)
        {
          ++aMismatches[aThreadIndex];
        }
      }
    });
  }
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }

  for (int aThreadIndex = 0; aThreadIndex < THE_NB_THREADS; ++aThreadIndex)
  {
    EXPECT_EQ(0, aMismatches[aThreadIndex])
      << "thread " << aThreadIndex << " read a point from the wrong span";
  }
}

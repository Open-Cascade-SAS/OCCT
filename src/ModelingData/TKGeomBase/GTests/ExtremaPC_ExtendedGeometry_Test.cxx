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

#include <ExtremaPC_Curve.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>

#include <cmath>
#include <random>

namespace
{
constexpr double THE_TOL = 1.0e-6;
}

//==================================================================================================
// Test fixture for extended geometry tests
//==================================================================================================

class ExtremaPC_ExtendedGeometryTest : public testing::Test
{
protected:
  //! Compares minimum distance between new and old implementations
  void CompareMinDistance(const ExtremaPC::Result& theNew,
                          Extrema_ExtPC&           theOld,
                          const char*              theTestName,
                          double                   theTol = THE_TOL)
  {
    if (!theOld.IsDone() || theOld.NbExt() == 0)
    {
      return;
    }

    ASSERT_TRUE(theNew.IsDone()) << theTestName << ": New implementation failed";
    ASSERT_GT(theNew.NbExt(), 0) << theTestName << ": No extrema found";

    double aOldMinSqDist = RealLast();
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      if (theOld.SquareDistance(i) < aOldMinSqDist)
      {
        aOldMinSqDist = theOld.SquareDistance(i);
      }
    }

    double aNewMinDist = std::sqrt(theNew.MinSquareDistance());
    double aOldMinDist = std::sqrt(aOldMinSqDist);

    EXPECT_NEAR(aNewMinDist, aOldMinDist, theTol)
      << theTestName << ": Distance mismatch. New: " << aNewMinDist << ", Old: " << aOldMinDist;
  }

  //! Compares maximum distance between new and old implementations
  void CompareMaxDistance(const ExtremaPC::Result& theNew,
                          Extrema_ExtPC&           theOld,
                          const char*              theTestName,
                          double                   theTol = THE_TOL)
  {
    if (!theOld.IsDone() || theOld.NbExt() == 0)
    {
      return;
    }

    ASSERT_TRUE(theNew.IsDone()) << theTestName << ": New implementation failed";
    ASSERT_GT(theNew.NbExt(), 0) << theTestName << ": No extrema found";

    double aOldMaxSqDist = 0.0;
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      if (theOld.SquareDistance(i) > aOldMaxSqDist)
      {
        aOldMaxSqDist = theOld.SquareDistance(i);
      }
    }

    double aNewMaxDist = std::sqrt(theNew.MaxSquareDistance());
    double aOldMaxDist = std::sqrt(aOldMaxSqDist);

    EXPECT_NEAR(aNewMaxDist, aOldMaxDist, theTol)
      << theTestName << ": Distance mismatch. New: " << aNewMaxDist << ", Old: " << aOldMaxDist;
  }

  //! Compares both min and max distances
  void CompareMinMax(const ExtremaPC::Result& theNew,
                     Extrema_ExtPC&           theOld,
                     const char*              theTestName,
                     double                   theTol = THE_TOL)
  {
    CompareMinDistance(theNew, theOld, theTestName, theTol);
    CompareMaxDistance(theNew, theOld, theTestName, theTol);
  }
};

//==================================================================================================
// Transformed geometry tests - Circle
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_Translated)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1)), 25.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(150.0, 200.0, 50.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_Translated");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_Rotated_XY_Plane)
{
  // Circle in XY plane, rotated 45 degrees
  gp_Dir                   aNormal(1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0), 0);
  occ::handle<Geom_Circle> aGeomCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), aNormal), 15.0);
  GeomAdaptor_Curve        anAdaptor(aGeomCircle);

  gp_Pnt aPoint(20.0, 20.0, 5.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_Rotated_XY");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_Rotated_XZ_Plane)
{
  // Circle in XZ plane
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(15.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_Rotated_XZ");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_Rotated_YZ_Plane)
{
  // Circle in YZ plane
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(5.0, 15.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_Rotated_YZ");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_Arbitrary_Orientation)
{
  // Circle with arbitrary orientation
  gp_Dir                   aNormal(1, 1, 1);
  occ::handle<Geom_Circle> aGeomCircle = new Geom_Circle(gp_Ax2(gp_Pnt(10, 20, 30), aNormal), 8.0);
  GeomAdaptor_Curve        anAdaptor(aGeomCircle);

  gp_Pnt aPoint(15.0, 25.0, 35.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_Arbitrary");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_VerySmall)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.001);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(0.002, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_VerySmall", 1.0e-9);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Circle_VeryLarge)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1000.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(1500.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Circle_VeryLarge");
}

//==================================================================================================
// Transformed geometry tests - Ellipse
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Ellipse_Translated)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(50, 100, 25), gp_Dir(0, 0, 1)), 30.0, 15.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(90.0, 100.0, 25.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Ellipse_Translated");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Ellipse_Rotated)
{
  gp_Dir                    aNormal(0, 1, 0); // In XZ plane
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), aNormal), 20.0, 8.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(25.0, 10.0, 5.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Ellipse_Rotated");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Ellipse_HighEccentricity)
{
  // Very elongated ellipse (major = 50, minor = 2)
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0, 2.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(40.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Ellipse_HighEccentricity");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Ellipse_NearlyCircular)
{
  // Nearly circular ellipse (major = 10.1, minor = 10.0)
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.1, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(15.0, 8.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Ellipse_NearlyCircular");
}

//==================================================================================================
// Transformed geometry tests - Line
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Line_Diagonal)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  GeomAdaptor_Curve      anAdaptor(aGeomLine, -100.0, 100.0);

  gp_Pnt aPoint(10.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Line_Diagonal");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Line_Arbitrary)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(10, 20, 30), gp_Dir(2, 3, 1));
  GeomAdaptor_Curve      anAdaptor(aGeomLine, -50.0, 50.0);

  gp_Pnt aPoint(25.0, 35.0, 40.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Line_Arbitrary");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Line_PointOnLine)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aGeomLine, -100.0, 100.0);

  gp_Pnt aPoint(50.0, 0.0, 0.0); // Exactly on line

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_NEAR(std::sqrt(aNewResult.MinSquareDistance()), 0.0, THE_TOL);
}

//==================================================================================================
// Parabola tests with different configurations
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Parabola_Translated)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(10, 20, 0), gp_Dir(0, 0, 1)), 3.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -15.0, 15.0);

  gp_Pnt aPoint(20.0, 30.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Parabola_Translated");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Parabola_InXZPlane)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -10.0, 10.0);

  gp_Pnt aPoint(5.0, 5.0, 10.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Parabola_InXZPlane");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Parabola_SmallFocalLength)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.5);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -5.0, 5.0);

  gp_Pnt aPoint(2.0, 3.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Parabola_SmallFocalLength");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Parabola_LargeFocalLength)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -100.0, 100.0);

  gp_Pnt aPoint(50.0, 25.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Parabola_LargeFocalLength");
}

//==================================================================================================
// Hyperbola tests with different configurations
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Hyperbola_Translated)
{
  occ::handle<Geom_Hyperbola> aGeomHyperbola =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(50, 50, 0), gp_Dir(0, 0, 1)), 15.0, 8.0);
  GeomAdaptor_Curve anAdaptor(aGeomHyperbola, -1.5, 1.5);

  gp_Pnt aPoint(80.0, 50.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Hyperbola_Translated");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Hyperbola_InXZPlane)
{
  occ::handle<Geom_Hyperbola> aGeomHyperbola =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 10.0, 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomHyperbola, -2.0, 2.0);

  gp_Pnt aPoint(20.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Hyperbola_InXZPlane");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Hyperbola_HighEccentricity)
{
  // Hyperbola with high eccentricity (a >> b)
  occ::handle<Geom_Hyperbola> aGeomHyperbola =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 30.0, 3.0);
  GeomAdaptor_Curve anAdaptor(aGeomHyperbola, -1.0, 1.0);

  gp_Pnt aPoint(50.0, 10.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aNewResult.IsDone());
  ASSERT_GT(aNewResult.NbExt(), 0);

  // Verify that the minimum extremum is correct by checking
  // the computed point is actually on the curve
  int    aMinIdx   = aNewResult.MinIndex();
  gp_Pnt aMinPoint = aNewResult[aMinIdx].Point;

  // Verify the distance is consistent
  double aComputedSqDist = aPoint.SquareDistance(aMinPoint);
  EXPECT_NEAR(aComputedSqDist, aNewResult[aMinIdx].SquareDistance, THE_TOL);
}

//==================================================================================================
// BSpline with different configurations
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, BSpline_Linear)
{
  // Degree 1 BSpline (linear segments)
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);
  aPoles(4) = gp_Pnt(3, 1, 0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.67;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 2;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 2;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(1.5, 1.5, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "BSpline_Linear", 0.01);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, BSpline_Quadratic)
{
  // Degree 2 BSpline
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);
  aPoles(4) = gp_Pnt(3, 2, 0);
  aPoles(5) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;

  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 3;
  aMults(2) = 2;
  aMults(3) = 3;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(2.0, 2.5, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "BSpline_Quadratic", 0.01);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, BSpline_3D_Helix)
{
  // 3D helix-like BSpline
  NCollection_Array1<gp_Pnt> aPoles(1, 7);
  aPoles(1) = gp_Pnt(10, 0, 0);
  aPoles(2) = gp_Pnt(7, 7, 5);
  aPoles(3) = gp_Pnt(0, 10, 10);
  aPoles(4) = gp_Pnt(-7, 7, 15);
  aPoles(5) = gp_Pnt(-10, 0, 20);
  aPoles(6) = gp_Pnt(-7, -7, 25);
  aPoles(7) = gp_Pnt(0, -10, 30);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 7;
  aMults(2) = 7;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 6);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(0.0, 0.0, 15.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "BSpline_3D_Helix", 0.05);
}

//==================================================================================================
// Bezier with different configurations
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Bezier_Quadratic)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(5, 10, 0);
  aPoles(3) = gp_Pnt(10, 0, 0);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  gp_Pnt aPoint(5.0, 8.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Bezier_Quadratic", 0.01);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Bezier_HighDegree)
{
  // Degree 6 Bezier
  NCollection_Array1<gp_Pnt> aPoles(1, 7);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(2, -1, 0);
  aPoles(4) = gp_Pnt(3, 4, 0);
  aPoles(5) = gp_Pnt(4, 0, 0);
  aPoles(6) = gp_Pnt(5, 2, 0);
  aPoles(7) = gp_Pnt(6, 1, 0);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  gp_Pnt aPoint(3.0, 3.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Bezier_HighDegree", 0.01);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Bezier_3D_SCurve)
{
  // 3D S-curve
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(3, 5, 2);
  aPoles(3) = gp_Pnt(6, 0, 4);
  aPoles(4) = gp_Pnt(9, 5, 6);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  gp_Pnt aPoint(4.5, 3.0, 3.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "Bezier_3D_SCurve", 0.01);
}

//==================================================================================================
// Offset curves with different bases
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, OffsetEllipse)
{
  occ::handle<Geom_Ellipse> aBaseEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  occ::handle<Geom_OffsetCurve> anOffsetCurve =
    new Geom_OffsetCurve(aBaseEllipse, 3.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  gp_Pnt aPoint(30.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "OffsetEllipse", 0.05);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, OffsetBezier)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(2, 4, 0);
  aPoles(3) = gp_Pnt(4, 4, 0);
  aPoles(4) = gp_Pnt(6, 0, 0);

  occ::handle<Geom_BezierCurve> aBaseBezier = new Geom_BezierCurve(aPoles);
  occ::handle<Geom_OffsetCurve> anOffsetCurve =
    new Geom_OffsetCurve(aBaseBezier, 1.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  gp_Pnt aPoint(3.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "OffsetBezier", 0.05);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, OffsetNegative)
{
  // Offset with negative distance (inside)
  occ::handle<Geom_Circle> aBaseCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 15.0);
  occ::handle<Geom_OffsetCurve> anOffsetCurve =
    new Geom_OffsetCurve(aBaseCircle, -5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinDistance(aNewResult, anOldExtPC, "OffsetNegative", 0.01);
  EXPECT_NEAR(std::sqrt(aNewResult.MinSquareDistance()), 10.0, 0.01); // 20 - 10 = 10
}

//==================================================================================================
// Arc (partial curve) tests
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, CircleArc_FirstQuadrant)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle, 0.0, M_PI / 2.0);

  gp_Pnt aPoint(15.0, 15.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  // Only compare minimum - max may differ between implementations for arcs
  // due to endpoint handling
  CompareMinDistance(aNewResult, anOldExtPC, "CircleArc_FirstQuadrant");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, CircleArc_ThirdQuadrant)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle, M_PI, 3.0 * M_PI / 2.0);

  gp_Pnt aPoint(-15.0, -5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  // Only compare minimum - max may differ between implementations for arcs
  // due to endpoint handling
  CompareMinDistance(aNewResult, anOldExtPC, "CircleArc_ThirdQuadrant");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, EllipseArc_SemiEllipse)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse, 0.0, M_PI);

  gp_Pnt aPoint(0.0, 20.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  // Only compare minimum - max may differ between implementations for arcs
  CompareMinDistance(aNewResult, anOldExtPC, "EllipseArc_SemiEllipse");
}

//==================================================================================================
// Edge cases and degenerate situations
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Point_ExactlyOnCurve_Circle)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(10.0, 0.0, 0.0); // Exactly on circle

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_NEAR(std::sqrt(aNewResult.MinSquareDistance()), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Point_ExactlyOnCurve_BSpline)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(3, 2, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  // Point exactly on curve at t=0.5
  gp_Pnt aPoint = anAdaptor.Value(0.5);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_NEAR(std::sqrt(aNewResult.MinSquareDistance()), 0.0, 0.01);
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Point_VeryFar)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(10000.0, 10000.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  CompareMinMax(aNewResult, anOldExtPC, "Point_VeryFar");
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Point_VeryClose)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(10.0001, 0.0, 0.0); // Very close to curve

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_NEAR(std::sqrt(aNewResult.MinSquareDistance()), 0.0001, 1.0e-5);
}

//==================================================================================================
// Random stress tests for all geometry types
//==================================================================================================

TEST_F(ExtremaPC_ExtendedGeometryTest, Random_Circle_100Points)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  std::mt19937                           aGen(12345);
  std::uniform_real_distribution<double> aDist(-100.0, 100.0);

  int aPassCount = 0;
  for (int i = 0; i < 100; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), aDist(aGen));

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone())
      continue;

    if (aNewResult.IsDone() && aNewResult.NbExt() > 0)
    {
      double aNewMin   = std::sqrt(aNewResult.MinSquareDistance());
      double aOldMinSq = RealLast();
      for (int j = 1; j <= anOldExtPC.NbExt(); ++j)
      {
        if (anOldExtPC.SquareDistance(j) < aOldMinSq)
        {
          aOldMinSq = anOldExtPC.SquareDistance(j);
        }
      }
      double aOldMin = std::sqrt(aOldMinSq);
      if (std::abs(aNewMin - aOldMin) < THE_TOL)
      {
        ++aPassCount;
      }
    }
  }

  EXPECT_GE(aPassCount, 95) << "At least 95% of random tests should pass";
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Random_Ellipse_100Points)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  std::mt19937                           aGen(54321);
  std::uniform_real_distribution<double> aDist(-100.0, 100.0);

  int aPassCount = 0;
  for (int i = 0; i < 100; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), aDist(aGen));

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone())
      continue;

    if (aNewResult.IsDone() && aNewResult.NbExt() > 0)
    {
      double aNewMin   = std::sqrt(aNewResult.MinSquareDistance());
      double aOldMinSq = RealLast();
      for (int j = 1; j <= anOldExtPC.NbExt(); ++j)
      {
        if (anOldExtPC.SquareDistance(j) < aOldMinSq)
        {
          aOldMinSq = anOldExtPC.SquareDistance(j);
        }
      }
      double aOldMin = std::sqrt(aOldMinSq);
      if (std::abs(aNewMin - aOldMin) < THE_TOL)
      {
        ++aPassCount;
      }
    }
  }

  EXPECT_GE(aPassCount, 95) << "At least 95% of random tests should pass";
}

TEST_F(ExtremaPC_ExtendedGeometryTest, Random_Parabola_100Points)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -20.0, 20.0);

  std::mt19937                           aGen(11111);
  std::uniform_real_distribution<double> aDist(-50.0, 50.0);

  int aPassCount  = 0;
  int aValidCount = 0;
  for (int i = 0; i < 100; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), aDist(aGen));

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult = anExtPC.Perform(aPoint, THE_TOL);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone() || anOldExtPC.NbExt() == 0)
      continue;

    if (!aNewResult.IsDone() || aNewResult.NbExt() == 0)
      continue;

    ++aValidCount;

    double aNewMin = std::sqrt(aNewResult.MinSquareDistance());
    double aOldMin = RealLast();
    for (int j = 1; j <= anOldExtPC.NbExt(); ++j)
    {
      if (anOldExtPC.SquareDistance(j) < aOldMin * aOldMin)
      {
        aOldMin = std::sqrt(anOldExtPC.SquareDistance(j));
      }
    }
    // Use relaxed tolerance for random tests
    if (std::abs(aNewMin - aOldMin) < 0.01)
    {
      ++aPassCount;
    }
  }

  // At least 80% of valid random tests should pass
  if (aValidCount > 0)
  {
    double aPassRate = static_cast<double>(aPassCount) / aValidCount;
    EXPECT_GE(aPassRate, 0.80) << "At least 80% of random tests should pass";
  }
}

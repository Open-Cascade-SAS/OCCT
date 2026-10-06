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

#include <cmath>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Precision.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

namespace
{
// Circle of radius theRadius at height theHeight on a cylinder of radius
// theCylRadius around OZ; the pcurve is the corresponding iso line in UV.
GeomLib_CheckCurveOnSurface makeCircleOnCylinderCheck(const double theCircRadius,
                                                      const double theCylRadius,
                                                      const double theHeight)
{
  const double aFirst = 0.0, aLast = 2.0 * M_PI;

  const occ::handle<Geom_Circle> aCirc = new Geom_Circle(
    gp_Ax2(gp_Pnt(0.0, 0.0, theHeight), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    theCircRadius);
  const occ::handle<GeomAdaptor_Curve> aC3d = new GeomAdaptor_Curve(aCirc, aFirst, aLast);

  const occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    theCylRadius);
  const occ::handle<Geom2d_Line> aPLine =
    new Geom2d_Line(gp_Pnt2d(0.0, theHeight), gp_Dir2d(1.0, 0.0));

  const occ::handle<Geom2dAdaptor_Curve>      aC2d = new Geom2dAdaptor_Curve(aPLine, aFirst, aLast);
  const occ::handle<GeomAdaptor_Surface>      aSurf = new GeomAdaptor_Surface(aCyl);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS  = new Adaptor3d_CurveOnSurface(aC2d, aSurf);

  GeomLib_CheckCurveOnSurface aCheck;
  aCheck.Init(aC3d, Precision::PConfusion());
  aCheck.Perform(aCoS);
  return aCheck;
}

GeomLib_CheckCurveOnSurface makeLineOnPlaneCheck(const gp_Dir&   theCurveDirection,
                                                 const gp_Dir2d& thePCurveDirection,
                                                 const double    theFirst,
                                                 const double    theLast)
{
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), theCurveDirection);
  const occ::handle<GeomAdaptor_Curve> aC3d = new GeomAdaptor_Curve(aLine, theFirst, theLast);

  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)));
  const occ::handle<Geom2d_Line> aPLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), thePCurveDirection);
  const occ::handle<Geom2dAdaptor_Curve> aC2d  = new Geom2dAdaptor_Curve(aPLine, theFirst, theLast);
  const occ::handle<GeomAdaptor_Surface> aSurf = new GeomAdaptor_Surface(aPlane);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS = new Adaptor3d_CurveOnSurface(aC2d, aSurf);

  GeomLib_CheckCurveOnSurface aCheck;
  aCheck.Init(aC3d, Precision::PConfusion());
  aCheck.Perform(aCoS);
  return aCheck;
}

GeomLib_CheckCurveOnSurface makeCircleAndEllipseCheck(const double theMajorRadius,
                                                      const double theMinorRadius,
                                                      const double theFirst,
                                                      const double theLast)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
                    2.0);
  const occ::handle<GeomAdaptor_Curve> aC3d = new GeomAdaptor_Curve(aCircle, theFirst, theLast);

  const occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)),
                       theMajorRadius,
                       theMinorRadius);
  const occ::handle<Geom2dAdaptor_Curve> aC2d =
    new Geom2dAdaptor_Curve(anEllipse, theFirst, theLast);
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)));
  const occ::handle<GeomAdaptor_Surface>      aSurf = new GeomAdaptor_Surface(aPlane);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS  = new Adaptor3d_CurveOnSurface(aC2d, aSurf);

  GeomLib_CheckCurveOnSurface aCheck;
  aCheck.Init(aC3d, Precision::PConfusion());
  aCheck.Perform(aCoS);
  return aCheck;
}
} // namespace

TEST(GeomLib_CheckCurveOnSurfaceTest, AnalyticCoincident_ReportsZeroDeviation)
{
  const GeomLib_CheckCurveOnSurface aCheck = makeCircleOnCylinderCheck(2.0, 2.0, 5.0);

  EXPECT_TRUE(aCheck.IsDone());
  EXPECT_LE(aCheck.MaxDistance(), Precision::Confusion());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, AnalyticDeviating_ReportsActualDeviation)
{
  // The 3D circle is 0.05 smaller than the cylinder carrying the pcurve: the
  // deviation is constant and equal to the radius difference. This guards the
  // constant-distance shortcut against swallowing real deviations.
  const GeomLib_CheckCurveOnSurface aCheck = makeCircleOnCylinderCheck(1.95, 2.0, 5.0);

  EXPECT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), 0.05, 1.0e-9);
}

TEST(GeomLib_CheckCurveOnSurfaceTest, LineOnPlane_ReportsZeroDeviation)
{
  const GeomLib_CheckCurveOnSurface aCheck =
    makeLineOnPlaneCheck(gp_Dir(1.0, 0.0, 0.0), gp_Dir2d(1.0, 0.0), -10.0, 10.0);

  EXPECT_TRUE(aCheck.IsDone());
  EXPECT_LE(aCheck.MaxDistance(), Precision::Confusion());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, DeviatingLines_ReportsEndpointMaximum)
{
  const GeomLib_CheckCurveOnSurface aCheck =
    makeLineOnPlaneCheck(gp_Dir(1.0, 0.0, 0.0), gp_Dir2d(0.0, 1.0), -2.0, 3.0);

  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), std::sqrt(18.0), Precision::Confusion());
  EXPECT_DOUBLE_EQ(aCheck.MaxParameter(), 3.0);
}

TEST(GeomLib_CheckCurveOnSurfaceTest, CircleAndEllipse_ReportsAnalyticMaximum)
{
  const GeomLib_CheckCurveOnSurface aCheck = makeCircleAndEllipseCheck(4.0, 1.0, 0.0, 2.0 * M_PI);

  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), 2.0, Precision::Confusion());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, NegativeTrim_ReportsInteriorMaximum)
{
  const GeomLib_CheckCurveOnSurface aCheck = makeCircleAndEllipseCheck(2.0, 1.0, -2.0, -1.0);

  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCheck.MaxParameter(), -0.5 * M_PI, Precision::PConfusion());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, AnalyticAliasing_ReportsActualDeviation)
{
  const double aFirst = 0.0, aLast = 80.0 * M_PI;

  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
                    4.0);
  const occ::handle<GeomAdaptor_Curve> aC3d = new GeomAdaptor_Curve(aCircle, aFirst, aLast);

  const occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(
    gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    3.0,
    1.0);
  const occ::handle<Geom2d_Line> aPLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(3.0, 4.0));
  const occ::handle<Geom2dAdaptor_Curve>      aC2d = new Geom2dAdaptor_Curve(aPLine, aFirst, aLast);
  const occ::handle<GeomAdaptor_Surface>      aSurf = new GeomAdaptor_Surface(aTorus);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS  = new Adaptor3d_CurveOnSurface(aC2d, aSurf);

  GeomLib_CheckCurveOnSurface aCheck;
  aCheck.Init(aC3d, Precision::PConfusion());
  aCheck.Perform(aCoS);

  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), 8.0, Precision::Confusion());
}

namespace
{
// The rational curve and the surface boundary have identical weights, so their
// separation is bounded by the two pole offsets and attains its maximum at an end.
GeomLib_CheckCurveOnSurface checkRationalSplineResidual(double  theOrigin,
                                                        double  theOffset,
                                                        double  theUVOrigin,
                                                        bool    theRationalPCurve,
                                                        double& theExpectedDistance,
                                                        double  theHeightChange = 0.5)
{
  NCollection_Array1<double>   aKnots(1, 2), aUKnots(1, 2), aWeights(1, 2);
  NCollection_Array1<int>      aMults(1, 2);
  NCollection_Array1<gp_Pnt>   aPoles(1, 2);
  NCollection_Array1<gp_Pnt2d> aUVPoles(1, 2);
  NCollection_Array2<gp_Pnt>   aSurfacePoles(1, 2, 1, 2);
  NCollection_Array2<double>   aSurfaceWeights(1, 2, 1, 2);
  theExpectedDistance = 0.0;
  for (size_t anIndex = 0; anIndex < 2; ++anIndex)
  {
    const double aParameter    = static_cast<double>(anIndex);
    aKnots.ChangeAt(anIndex)   = aParameter;
    aUKnots.ChangeAt(anIndex)  = theUVOrigin + aParameter;
    aMults.ChangeAt(anIndex)   = 2;
    aWeights.ChangeAt(anIndex) = anIndex == 0 ? 1.0 : 0.7;
    aPoles.ChangeAt(anIndex)   = gp_Pnt(aParameter, 0.0, theOrigin + theHeightChange * aParameter);
    aUVPoles.ChangeAt(anIndex) = gp_Pnt2d(theUVOrigin + aParameter, 0.0);
    const double aHeight       = aPoles.At(anIndex).Z() + theOffset;
    theExpectedDistance = std::max(theExpectedDistance, std::abs(aHeight - aPoles.At(anIndex).Z()));
    for (int aColumn = 1; aColumn <= 2; ++aColumn)
    {
      aSurfacePoles(static_cast<int>(anIndex) + 1, aColumn) =
        gp_Pnt(aParameter, aColumn - 1.0, aHeight);
      aSurfaceWeights(static_cast<int>(anIndex) + 1, aColumn) =
        theRationalPCurve ? 1.0 : aWeights.At(anIndex);
    }
  }
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 1);
  const occ::handle<Geom_BSplineSurface> aSurface =
    new Geom_BSplineSurface(aSurfacePoles, aSurfaceWeights, aUKnots, aKnots, aMults, aMults, 1, 1);
  const occ::handle<Geom2d_BSplineCurve> aPCurve =
    theRationalPCurve ? new Geom2d_BSplineCurve(aUVPoles, aWeights, aKnots, aMults, 1)
                      : new Geom2d_BSplineCurve(aUVPoles, aKnots, aMults, 1);
  const occ::handle<GeomAdaptor_Curve>        aCurveAdaptor   = new GeomAdaptor_Curve(aCurve);
  const occ::handle<GeomAdaptor_Surface>      aSurfaceAdaptor = new GeomAdaptor_Surface(aSurface);
  const occ::handle<Geom2dAdaptor_Curve>      aPCurveAdaptor  = new Geom2dAdaptor_Curve(aPCurve);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
    new Adaptor3d_CurveOnSurface(aPCurveAdaptor, aSurfaceAdaptor);
  GeomLib_CheckCurveOnSurface aCheck(aCurveAdaptor);
  aCheck.Perform(aCoS);
  return aCheck;
}
} // namespace

TEST(GeomLib_CheckCurveOnSurfaceTest, RationalSplineResidualAtLargeCoordinates)
{
  for (double anOrigin : {0.0, 1000.0, -1000.0, 1.0e6})
  {
    for (double anOffset : {0.0, 1.0e-6, -1.0e-6})
    {
      SCOPED_TRACE(::testing::Message() << "origin=" << anOrigin << " offset=" << anOffset);
      double                            anExpected;
      const GeomLib_CheckCurveOnSurface aCheck =
        checkRationalSplineResidual(anOrigin, anOffset, 0.0, false, anExpected);
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_NEAR(aCheck.MaxDistance(), anExpected, 1.0e-18);
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, RationalPCurveResidualAtLargeParameters)
{
  for (double aUVOrigin : {1.0e8, -1.0e8, 1.0e12})
  {
    for (double anOffset : {0.0, 0x1p-20, -0x1p-20})
    {
      SCOPED_TRACE(::testing::Message() << "UV origin=" << aUVOrigin << " offset=" << anOffset);
      double                            anExpected;
      const GeomLib_CheckCurveOnSurface aCheck =
        checkRationalSplineResidual(0.0, anOffset, aUVOrigin, true, anExpected);
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_NEAR(aCheck.MaxDistance(), anExpected, 1.0e-18);
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineResidualPreservesSmallEndpointAtLargeCoordinateSpan)
{
  for (double anOffset : {1.0e-6, -1.0e-6})
  {
    SCOPED_TRACE(anOffset);
    double                            anExpected;
    const GeomLib_CheckCurveOnSurface aCheck =
      checkRationalSplineResidual(1.0e12, anOffset, 0.0, false, anExpected, -1.0e12);
    ASSERT_TRUE(aCheck.IsDone());
    EXPECT_NEAR(aCheck.MaxDistance(), anExpected, 1.0e-18);
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineResidualFindsInteriorMaximum)
{
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 1.0;
  NCollection_Array1<int> aCurveMults(1, 2), aLinearMults(1, 2);
  aCurveMults.Init(3);
  aLinearMults.Init(2);
  NCollection_Array1<gp_Pnt> aCurvePoles(1, 3);
  aCurvePoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, 1000.0);
  aCurvePoles.ChangeAt(1) = gp_Pnt(0.5, 0.0, 1000.0 + 2.0e-6);
  aCurvePoles.ChangeAt(2) = gp_Pnt(1.0, 0.0, 1000.0);
  NCollection_Array1<gp_Pnt2d> aUVPoles(1, 2);
  aUVPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aUVPoles.ChangeAt(1) = gp_Pnt2d(1.0, 0.0);
  NCollection_Array2<gp_Pnt> aSurfacePoles(1, 2, 1, 2);
  for (int aRow = 1; aRow <= 2; ++aRow)
  {
    for (int aColumn = 1; aColumn <= 2; ++aColumn)
    {
      aSurfacePoles(aRow, aColumn) = gp_Pnt(aRow - 1.0, aColumn - 1.0, 1000.0);
    }
  }
  const occ::handle<GeomAdaptor_Curve> aCurve =
    new GeomAdaptor_Curve(new Geom_BSplineCurve(aCurvePoles, aKnots, aCurveMults, 2));
  const occ::handle<Geom2dAdaptor_Curve> aPCurve =
    new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aLinearMults, 1));
  const occ::handle<GeomAdaptor_Surface> aSurface = new GeomAdaptor_Surface(
    new Geom_BSplineSurface(aSurfacePoles, aKnots, aKnots, aLinearMults, aLinearMults, 1, 1));
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
    new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
  GeomLib_CheckCurveOnSurface aCheck(aCurve);
  aCheck.Perform(aCoS);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), 0.5 * (aCurvePoles.At(1).Z() - 1000.0), 1.0e-18);
  EXPECT_NEAR(aCheck.MaxParameter(), 0.5, Precision::PConfusion());
}

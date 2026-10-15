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

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>
#include <Geom_UndefinedValue.hxx>
#include <Geom_Circle.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

TEST(GeomLib_CheckCurveOnSurfaceTest, CompositionRetainsShiftedSurfaceParameters)
{
  constexpr double aLargeOrigin = 1073741824.0;
  for (const double anOrigin : {0.0, aLargeOrigin, -aLargeOrigin})
  {
    SCOPED_TRACE(anOrigin);
    NCollection_Array1<gp_Pnt> aCurvePoles(1, 3);
    aCurvePoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, 0.0);
    aCurvePoles.ChangeAt(1) = gp_Pnt(0.5, 0.0, 0.0);
    aCurvePoles.ChangeAt(2) = gp_Pnt(1.0, 1.0, 0.0);
    NCollection_Array1<gp_Pnt2d> aPCurvePoles(1, 3);
    NCollection_Array2<gp_Pnt>   aSurfacePoles(1, 3, 1, 2);
    for (size_t anIndex = 0; anIndex < 3; ++anIndex)
    {
      aPCurvePoles.ChangeAt(anIndex)     = gp_Pnt2d(anOrigin + 0.5 * double(anIndex), 0.0);
      aSurfacePoles.ChangeAt(anIndex, 0) = aCurvePoles.At(anIndex);
      aSurfacePoles.ChangeAt(anIndex, 1) =
        aCurvePoles.At(anIndex).Translated(gp_Vec(0.0, 0.0, 1.0));
    }
    NCollection_Array1<double> aKnots(1, 2), aUKnots(1, 2);
    aKnots.ChangeAt(0)  = 0.0;
    aKnots.ChangeAt(1)  = 1.0;
    aUKnots.ChangeAt(0) = anOrigin;
    aUKnots.ChangeAt(1) = anOrigin + 1.0;
    NCollection_Array1<int> aUMultiplicities(1, 2), aVMultiplicities(1, 2);
    aUMultiplicities.Init(3);
    aVMultiplicities.Init(2);
    const occ::handle<Geom_BSplineCurve> aCurve =
      new Geom_BSplineCurve(aCurvePoles, aKnots, aUMultiplicities, 2);
    const occ::handle<Geom2d_BSplineCurve> aPCurve =
      new Geom2d_BSplineCurve(aPCurvePoles, aKnots, aUMultiplicities, 2);
    const occ::handle<Geom_BSplineSurface>      aSurface   = new Geom_BSplineSurface(aSurfacePoles,
                                                                              aUKnots,
                                                                              aKnots,
                                                                              aUMultiplicities,
                                                                              aVMultiplicities,
                                                                              2,
                                                                              1);
    const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
    const occ::handle<Adaptor3d_CurveOnSurface> anOther =
      new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve),
                                   new GeomAdaptor_Surface(aSurface));
    GeomLib_CheckCurveOnSurface aCheck(aReference);
    aCheck.Perform(anOther);
    ASSERT_TRUE(aCheck.IsDone());
    EXPECT_LE(aCheck.MaxDistance(), RealEpsilon());
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, DenseKnotsKeepConstantDeviation)
{
  constexpr int              aNbSpans = 128;
  constexpr double           aGap     = 0.125;
  NCollection_Array1<gp_Pnt> aCurvePoles(1, 3);
  aCurvePoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, aGap);
  aCurvePoles.ChangeAt(1) = gp_Pnt(0.5, 0.0, aGap);
  aCurvePoles.ChangeAt(2) = gp_Pnt(1.0, 1.0, aGap);
  NCollection_Array1<gp_Pnt2d> aPCurvePoles(1, 3);
  NCollection_Array2<gp_Pnt>   aSurfacePoles(1, 3, 1, 2);
  for (size_t anIndex = 0; anIndex < 3; ++anIndex)
  {
    aPCurvePoles.ChangeAt(anIndex) = gp_Pnt2d(0.5 * double(anIndex), 0.0);
    aSurfacePoles.ChangeAt(anIndex, 0) =
      gp_Pnt(aCurvePoles.At(anIndex).X(), aCurvePoles.At(anIndex).Y(), 0.0);
    aSurfacePoles.ChangeAt(anIndex, 1) =
      aSurfacePoles.At(anIndex, 0).Translated(gp_Vec(0.0, 0.0, 1.0));
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 1.0;
  NCollection_Array1<int> aUMultiplicities(1, 2), aVMultiplicities(1, 2);
  aUMultiplicities.Init(3);
  aVMultiplicities.Init(2);
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aCurvePoles, aKnots, aUMultiplicities, 2);
  const occ::handle<Geom2d_BSplineCurve> aPCurve =
    new Geom2d_BSplineCurve(aPCurvePoles, aKnots, aUMultiplicities, 2);
  const occ::handle<Geom_BSplineSurface> aSurface = new Geom_BSplineSurface(aSurfacePoles,
                                                                            aKnots,
                                                                            aKnots,
                                                                            aUMultiplicities,
                                                                            aVMultiplicities,
                                                                            2,
                                                                            1);
  for (int anIndex = 1; anIndex < aNbSpans; ++anIndex)
  {
    const double aParameter = double(anIndex) / aNbSpans;
    aCurve->InsertKnot(aParameter);
    aPCurve->InsertKnot(aParameter);
  }
  const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
  const occ::handle<Adaptor3d_CurveOnSurface> anOther =
    new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve),
                                 new GeomAdaptor_Surface(aSurface));
  GeomLib_CheckCurveOnSurface aCheck(aReference);
  for (const bool isParallel : {false, true})
  {
    SCOPED_TRACE(isParallel);
    aCheck.SetParallel(isParallel);
    aCheck.Perform(anOther);
    ASSERT_TRUE(aCheck.IsDone());
    EXPECT_NEAR(aCheck.MaxDistance(), aGap, RealEpsilon());
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, MaximumIsIndependentOfParameterScale)
{
  constexpr double aGap = 0.2;
  for (const double aParameterScale : {1.0, 1.e-12})
  {
    SCOPED_TRACE(aParameterScale);
    NCollection_Array1<gp_Pnt> aPoles3d(1, 3);
    aPoles3d.ChangeAt(0) = gp_Pnt(0.0, 0.0, 0.0);
    aPoles3d.ChangeAt(1) = gp_Pnt(0.5, 1.0, 0.0);
    aPoles3d.ChangeAt(2) = gp_Pnt(1.0, 0.0, 0.0);
    NCollection_Array1<gp_Pnt2d> aPoles2d(1, 3);
    aPoles2d.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
    aPoles2d.ChangeAt(1) = gp_Pnt2d(0.5, 1.0 + aGap);
    aPoles2d.ChangeAt(2) = gp_Pnt2d(1.0, 0.0);
    NCollection_Array1<double> aKnots(1, 2);
    aKnots.ChangeAt(0) = 0.0;
    aKnots.ChangeAt(1) = aParameterScale;
    NCollection_Array1<int> aMultiplicities(1, 2);
    aMultiplicities.Init(3);
    const occ::handle<Geom_BSplineCurve> aCurve =
      new Geom_BSplineCurve(aPoles3d, aKnots, aMultiplicities, 2);
    const occ::handle<Geom2d_BSplineCurve> aPCurve =
      new Geom2d_BSplineCurve(aPoles2d, aKnots, aMultiplicities, 2);
    const occ::handle<Geom_Plane>               aPlane     = new Geom_Plane(gp_Ax3());
    const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
    const occ::handle<Adaptor3d_CurveOnSurface> anOther =
      new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve),
                                   new GeomAdaptor_Surface(aPlane));
    GeomLib_CheckCurveOnSurface aCheck(aReference);
    for (const bool isParallel : {false, true})
    {
      SCOPED_TRACE(isParallel);
      aCheck.SetParallel(isParallel);
      aCheck.Perform(anOther);
      ASSERT_EQ(aCheck.ErrorStatus(), 0);
      EXPECT_NEAR(aCheck.MaxDistance(), 0.5 * aGap, Precision::Confusion());
      EXPECT_GE(aCheck.MaxParameter(), 0.0);
      EXPECT_LE(aCheck.MaxParameter(), aParameterScale);
      const gp_Pnt2d aUV = aPCurve->EvalD0(aCheck.MaxParameter());
      EXPECT_NEAR(aCurve->EvalD0(aCheck.MaxParameter()).Distance(aPlane->EvalD0(aUV.X(), aUV.Y())),
                  aCheck.MaxDistance(),
                  Precision::Confusion());
    }
  }
}

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

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineResidualWithTranslatedParameters)
{
  for (const double aUVOrigin : {0.0, 1.0e8, -1.0e8})
  {
    for (const double aMixedTerm : {0.0, 0.5})
    {
      SCOPED_TRACE(::testing::Message() << "UV origin=" << aUVOrigin << " mixed=" << aMixedTerm);
      NCollection_Array1<double> aKnots(1, 2), aUKnots(1, 2), aVKnots(1, 2);
      aKnots.ChangeAt(0)  = 0.0;
      aKnots.ChangeAt(1)  = 1.0;
      aUKnots.ChangeAt(0) = aUVOrigin - 2.0;
      aUKnots.ChangeAt(1) = aUVOrigin + 2.0;
      aVKnots.ChangeAt(0) = aUVOrigin + 4.0;
      aVKnots.ChangeAt(1) = aUVOrigin + 6.0;
      NCollection_Array1<int> aCurveMults(1, 2), aSurfaceMults(1, 2);
      aCurveMults.Init(3);
      aSurfaceMults.Init(2);
      NCollection_Array1<gp_Pnt>   aCurvePoles(1, 3);
      NCollection_Array1<gp_Pnt2d> aUVPoles(1, 3);
      for (size_t anIndex = 0; anIndex < 3; ++anIndex)
      {
        const double aT               = 0.5 * double(anIndex);
        aCurvePoles.ChangeAt(anIndex) = gp_Pnt(1.0e6 + 2.0 * aT, 1.0e6 + aT, 1000.0);
        aUVPoles.ChangeAt(anIndex) =
          gp_Pnt2d(aUVOrigin - 2.0 + 4.0 * aT, aUVOrigin + 4.0 + 2.0 * aT);
      }
      aCurvePoles.ChangeAt(1).SetZ(1000.0 + 2.0e-6);
      aCurvePoles.ChangeAt(2).SetZ(1000.0 + aMixedTerm);
      NCollection_Array2<gp_Pnt> aSurfacePoles(1, 2, 1, 2);
      for (size_t aU = 0; aU < 2; ++aU)
      {
        for (size_t aV = 0; aV < 2; ++aV)
        {
          aSurfacePoles.ChangeAt(aU, aV) = gp_Pnt(1.0e6 + double(aU + aV),
                                                  1.0e6 + 2.0 * double(aU) - double(aV),
                                                  1000.0 + aMixedTerm * double(aU * aV));
        }
      }
      const occ::handle<GeomAdaptor_Curve> aCurve =
        new GeomAdaptor_Curve(new Geom_BSplineCurve(aCurvePoles, aKnots, aCurveMults, 2));
      const occ::handle<Geom2dAdaptor_Curve> aPCurve =
        new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aCurveMults, 2));
      const occ::handle<GeomAdaptor_Surface> aSurface =
        new GeomAdaptor_Surface(new Geom_BSplineSurface(aSurfacePoles,
                                                        aUKnots,
                                                        aVKnots,
                                                        aSurfaceMults,
                                                        aSurfaceMults,
                                                        1,
                                                        1));
      const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
        new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
      GeomLib_CheckCurveOnSurface aCheck(aCurve);
      aCheck.Perform(aCoS);
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_NEAR(aCheck.MaxDistance(), 0.5 * (aCurvePoles.At(1).Z() - 1000.0), 1.0e-18);
      EXPECT_NEAR(aCheck.MaxParameter(), 0.5, Precision::PConfusion());
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, AffineSplineResidualAcrossKnotSpans)
{
  for (const double aUVOrigin : {0.0, 1.0e8, -1.0e8})
  {
    SCOPED_TRACE(aUVOrigin);
    NCollection_Array1<double> aKnots(1, 4), aUKnots(1, 2), aVKnots(1, 2);
    NCollection_Array1<int>    aMults(1, 4), aSurfaceMults(1, 2);
    for (size_t anIndex = 0; anIndex < 4; ++anIndex)
    {
      aKnots.ChangeAt(anIndex) = double(anIndex);
      aMults.ChangeAt(anIndex) = anIndex == 0 || anIndex == 3 ? 3 : 1;
    }
    aUKnots.ChangeAt(0) = aUVOrigin;
    aUKnots.ChangeAt(1) = aUVOrigin + 3.0;
    aVKnots.ChangeAt(0) = 0.0;
    aVKnots.ChangeAt(1) = 1.0;
    aSurfaceMults.Init(2);

    NCollection_Array1<gp_Pnt>   aPoles(1, 5);
    NCollection_Array1<gp_Pnt2d> aUVPoles(1, 5);
    const double                 aParameters[] = {0.0, 0.5, 1.5, 2.5, 3.0};
    for (size_t anIndex = 0; anIndex < 5; ++anIndex)
    {
      aPoles.ChangeAt(anIndex)   = gp_Pnt(1.0e12 + 2.0 * aParameters[anIndex], 0.0, 1000.0);
      aUVPoles.ChangeAt(anIndex) = gp_Pnt2d(aUVOrigin + aParameters[anIndex], 0.0);
    }
    aPoles.ChangeAt(2).SetZ(1000.0 + 2.0e-6);
    NCollection_Array2<gp_Pnt> aSurfacePoles(1, 2, 1, 2);
    for (size_t aU = 0; aU < 2; ++aU)
    {
      for (size_t aV = 0; aV < 2; ++aV)
      {
        aSurfacePoles.ChangeAt(aU, aV) = gp_Pnt(1.0e12 + 6.0 * double(aU), double(aV), 1000.0);
      }
    }
    const occ::handle<GeomAdaptor_Curve> aCurve =
      new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aMults, 2));
    const occ::handle<Geom2dAdaptor_Curve> aPCurve =
      new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aMults, 2));
    const occ::handle<GeomAdaptor_Surface> aSurface = new GeomAdaptor_Surface(
      new Geom_BSplineSurface(aSurfacePoles, aUKnots, aVKnots, aSurfaceMults, aSurfaceMults, 1, 1));
    const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
      new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
    for (const bool isParallel : {false, true})
    {
      SCOPED_TRACE(isParallel);
      GeomLib_CheckCurveOnSurface aCheck(aCurve);
      aCheck.SetParallel(isParallel);
      aCheck.Perform(aCoS);
      ASSERT_TRUE(aCheck.IsDone());
      // The middle basis peaks at 3/4 at t = 1.5.
      EXPECT_NEAR(aCheck.MaxDistance(), 0.75 * (aPoles.At(2).Z() - 1000.0), 1.0e-18);
      EXPECT_NEAR(aCheck.MaxParameter(), 1.5, Precision::PConfusion());
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineMaximumAcrossDegreesAndParameterRanges)
{
  for (const double aMixedTerm : {0.0, 0.5})
  {
    SCOPED_TRACE(aMixedTerm);
    for (const int aDegree : {3, 7, 15, 25})
    {
      for (const double anOffset : {2.0e-6, 0.02, 2.0})
      {
        for (const auto& aRange :
             {std::pair{0.0, 1.0}, std::pair{-10000.0, 8.0}, std::pair{10000.0, 0.125}})
        {
          for (const int aPoleIndex : {0, aDegree / 2, aDegree})
          {
            const auto [aFirst, aSpan] = aRange;
            SCOPED_TRACE(::testing::Message()
                         << "degree=" << aDegree << " offset=" << anOffset << " first=" << aFirst
                         << " span=" << aSpan << " pole=" << aPoleIndex);
            NCollection_Array1<double> aKnots(1, 2), aSurfaceKnots(1, 2);
            aKnots.ChangeAt(0)        = aFirst;
            aKnots.ChangeAt(1)        = aFirst + aSpan;
            aSurfaceKnots.ChangeAt(0) = 0.0;
            aSurfaceKnots.ChangeAt(1) = 1.0;
            NCollection_Array1<int> aMults(1, 2), aSurfaceMults(1, 2);
            aMults.Init(aDegree + 1);
            aSurfaceMults.Init(2);
            NCollection_Array1<gp_Pnt>   aPoles(1, aDegree + 1);
            NCollection_Array1<gp_Pnt2d> aUVPoles(1, aDegree + 1);
            aPoles.Init(gp_Pnt(0.0, 0.0, 1000.0));
            aUVPoles.Init(gp_Pnt2d(0.0, 0.0));
            aPoles.ChangeAt(aPoleIndex).SetZ(1000.0 + anOffset);
            NCollection_Array2<gp_Pnt> aSurfacePoles(1, 2, 1, 2);
            for (size_t aU = 0; aU < 2; ++aU)
            {
              for (size_t aV = 0; aV < 2; ++aV)
              {
                aSurfacePoles.ChangeAt(aU, aV) =
                  gp_Pnt(double(aU), double(aV), 1000.0 + aMixedTerm * double(aU * aV));
              }
            }
            const occ::handle<GeomAdaptor_Curve> aCurve =
              new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aMults, aDegree));
            const occ::handle<Geom2dAdaptor_Curve> aPCurve =
              new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aMults, aDegree));
            const occ::handle<GeomAdaptor_Surface> aSurface =
              new GeomAdaptor_Surface(new Geom_BSplineSurface(aSurfacePoles,
                                                              aSurfaceKnots,
                                                              aSurfaceKnots,
                                                              aSurfaceMults,
                                                              aSurfaceMults,
                                                              1,
                                                              1));
            const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
              new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
            GeomLib_CheckCurveOnSurface aCheck(aCurve);
            aCheck.Perform(aCoS);
            ASSERT_TRUE(aCheck.IsDone());
            // A Bernstein basis peaks at index / degree.
            const double aParameter = double(aPoleIndex) / aDegree;
            double       aBinomial  = 1.0;
            for (int anIndex = 1; anIndex <= aPoleIndex; ++anIndex)
            {
              aBinomial *= double(aDegree - anIndex + 1) / anIndex;
            }
            const double aMaximum = aBinomial * std::pow(aParameter, aPoleIndex)
                                    * std::pow(1.0 - aParameter, aDegree - aPoleIndex);
            const double anExpected = aMaximum * (aPoles.At(aPoleIndex).Z() - 1000.0);
            EXPECT_NEAR(aCheck.MaxDistance(), anExpected, std::max(1.0e-18, 1.0e-14 * anExpected));
            EXPECT_NEAR(aCheck.MaxParameter(),
                        aFirst + aSpan * aParameter,
                        aSpan * Precision::Confusion());
          }
        }
      }
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, MatchingSplinesWithLargeConstantCoordinate)
{
  NCollection_Array1<double> aKnots(size_t{2});
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 1.0;
  NCollection_Array1<int> aLinearMults(size_t{2}), aQuadraticMults(size_t{2});
  aLinearMults.Init(2);
  aQuadraticMults.Init(3);
  NCollection_Array1<gp_Pnt> aPoles(size_t{2});
  aPoles.ChangeAt(0) = gp_Pnt(1.0e307, 0.0, 0.0);
  aPoles.ChangeAt(1) = gp_Pnt(1.0e307, 1.0, 0.0);
  NCollection_Array1<gp_Pnt2d> aUVPoles(size_t{2});
  aUVPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aUVPoles.ChangeAt(1) = gp_Pnt2d(1.0, 0.0);
  NCollection_Array2<gp_Pnt> aSurfacePoles(size_t{3}, size_t{3});
  for (size_t aU = 0; aU < 3; ++aU)
  {
    for (size_t aV = 0; aV < 3; ++aV)
    {
      aSurfacePoles.ChangeAt(aU, aV) = gp_Pnt(1.0e307, 0.5 * double(aU), 0.5 * double(aV));
    }
  }
  const occ::handle<GeomAdaptor_Curve> aCurve =
    new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aLinearMults, 1));
  const occ::handle<Geom2dAdaptor_Curve> aPCurve =
    new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aLinearMults, 1));
  const occ::handle<GeomAdaptor_Surface> aSurface = new GeomAdaptor_Surface(
    new Geom_BSplineSurface(aSurfacePoles, aKnots, aKnots, aQuadraticMults, aQuadraticMults, 2, 2));
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
    new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
  GeomLib_CheckCurveOnSurface aCheck(aCurve);
  aCheck.Perform(aCoS);
  ASSERT_TRUE(aCheck.IsDone());
  // The remaining coordinates have unit scale and use two-component arithmetic.
  EXPECT_LE(aCheck.MaxDistance(), 100.0 * RealEpsilon() * RealEpsilon());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineResidualWithScaledEndpointContributions)
{
  struct Case
  {
    int    Degree;    // Degree of the endpoint basis function.
    double Span;      // Full knot span; the tested interval is [0, 1].
    double Pole;      // Large endpoint coordinate.
    double Tolerance; // Error allowed for the small residual.
  };

  const std::array<Case, 2> aCases = {
    {{25, 1.0e13, 1.0e300, 1.0e-28}, {2, 1.0e200, 1.0e308, 1.0e-106}}};
  for (const auto& aCase : aCases)
  {
    SCOPED_TRACE(::testing::Message() << "degree=" << aCase.Degree << " span=" << aCase.Span);
    NCollection_Array1<double> aKnots(size_t{2}), aSurfaceKnots(size_t{2});
    aKnots.ChangeAt(0)        = 0.0;
    aKnots.ChangeAt(1)        = aCase.Span;
    aSurfaceKnots.ChangeAt(0) = 0.0;
    aSurfaceKnots.ChangeAt(1) = 1.0;
    NCollection_Array1<int> aMults(size_t{2}), aSurfaceMults(size_t{2});
    aMults.Init(aCase.Degree + 1);
    aSurfaceMults.Init(2);
    const size_t                 aNbPoles = static_cast<size_t>(aCase.Degree) + 1;
    NCollection_Array1<gp_Pnt>   aPoles(aNbPoles);
    NCollection_Array1<gp_Pnt2d> aUVPoles(aNbPoles);
    aPoles.Init(gp_Pnt(1.0, 0.0, 0.0));
    aPoles.ChangeAt(aNbPoles - 1).SetX(aCase.Pole);
    aUVPoles.Init(gp_Pnt2d(0.0, 0.0));
    NCollection_Array2<gp_Pnt> aSurfacePoles(size_t{2}, size_t{2});
    for (size_t aU = 0; aU < 2; ++aU)
    {
      for (size_t aV = 0; aV < 2; ++aV)
      {
        aSurfacePoles.ChangeAt(aU, aV) = gp_Pnt(1.0 + double(aU), double(aV), double(aU * aV));
      }
    }
    const occ::handle<GeomAdaptor_Curve> aCurve =
      new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aMults, aCase.Degree), 0.0, 1.0);
    const occ::handle<Geom2dAdaptor_Curve> aPCurve =
      new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aMults, aCase.Degree),
                              0.0,
                              1.0);
    const occ::handle<GeomAdaptor_Surface> aSurface =
      new GeomAdaptor_Surface(new Geom_BSplineSurface(aSurfacePoles,
                                                      aSurfaceKnots,
                                                      aSurfaceKnots,
                                                      aSurfaceMults,
                                                      aSurfaceMults,
                                                      1,
                                                      1));
    const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
      new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
    GeomLib_CheckCurveOnSurface aCheck(aCurve);
    aCheck.Perform(aCoS);
    ASSERT_TRUE(aCheck.IsDone());
    // Scale before forming the power, which is below the range of double.
    double anExpected = aCase.Pole;
    for (size_t anIndex = 0; anIndex < static_cast<size_t>(aCase.Degree); ++anIndex)
    {
      anExpected /= aCase.Span;
    }
    EXPECT_NEAR(aCheck.MaxDistance(), anExpected, aCase.Tolerance);
    EXPECT_NEAR(aCheck.MaxParameter(), 1.0, Precision::PConfusion());
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineResidualAcrossSurfacePatches)
{
  NCollection_Array1<double> aSurfaceKnots(size_t{4});
  NCollection_Array1<int>    aSurfaceMults(size_t{4});
  for (size_t anIndex = 0; anIndex < 4; ++anIndex)
  {
    aSurfaceKnots.ChangeAt(anIndex) = double(anIndex);
    aSurfaceMults.ChangeAt(anIndex) = anIndex == 0 || anIndex == 3 ? 3 : 1;
  }
  const std::array<double, 5> aGreville  = {0.0, 0.5, 1.5, 2.5, 3.0};
  const std::array<double, 5> aQuadratic = {0.0, 0.0, 2.0, 6.0, 9.0};
  NCollection_Array2<gp_Pnt>  aSurfacePoles(size_t{5}, size_t{5});
  for (size_t aU = 0; aU < 5; ++aU)
  {
    for (size_t aV = 0; aV < 5; ++aV)
    {
      aSurfacePoles.ChangeAt(aU, aV) = gp_Pnt(1.0e12 + aGreville[aU],
                                              -1.0e12 + aGreville[aV],
                                              1000.0 + 0.5 * aGreville[aU] * aGreville[aV]);
    }
  }
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_BSplineSurface(aSurfacePoles,
                                                    aSurfaceKnots,
                                                    aSurfaceKnots,
                                                    aSurfaceMults,
                                                    aSurfaceMults,
                                                    2,
                                                    2));
  for (const bool hasInnerKnots : {false, true})
  {
    SCOPED_TRACE(hasInnerKnots);
    const size_t               aNbPoles = hasInnerKnots ? 5 : 3;
    NCollection_Array1<double> aKnots(hasInnerKnots ? size_t{4} : size_t{2});
    NCollection_Array1<int>    aMults(aKnots.Size());
    for (size_t anIndex = 0; anIndex < aKnots.Size(); ++anIndex)
    {
      aKnots.ChangeAt(anIndex) = hasInnerKnots ? double(anIndex) : 3.0 * double(anIndex);
      aMults.ChangeAt(anIndex) = anIndex == 0 || anIndex + 1 == aKnots.Size() ? 3 : 1;
    }
    NCollection_Array1<gp_Pnt>   aPoles(aNbPoles);
    NCollection_Array1<gp_Pnt2d> aUVPoles(aNbPoles);
    for (size_t anIndex = 0; anIndex < aNbPoles; ++anIndex)
    {
      const double aT            = hasInnerKnots ? aGreville[anIndex] : 1.5 * double(anIndex);
      const double aSquare       = hasInnerKnots ? aQuadratic[anIndex] : (anIndex == 2 ? 9.0 : 0.0);
      aPoles.ChangeAt(anIndex)   = gp_Pnt(1.0e12 + aT, -1.0e12 + aT, 1000.0 + 0.5 * aSquare);
      aUVPoles.ChangeAt(anIndex) = gp_Pnt2d(aT, aT);
    }
    const size_t aMiddle = aNbPoles / 2;
    const double aBase   = aPoles.At(aMiddle).Z();
    aPoles.ChangeAt(aMiddle).SetZ(aBase + 2.0e-6);
    const occ::handle<GeomAdaptor_Curve> aCurve =
      new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aMults, 2));
    const occ::handle<Geom2dAdaptor_Curve> aPCurve =
      new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aMults, 2));
    const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
      new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
    for (const bool isParallel : {false, true})
    {
      SCOPED_TRACE(isParallel);
      GeomLib_CheckCurveOnSurface aCheck(aCurve);
      aCheck.SetParallel(isParallel);
      aCheck.Perform(aCoS);
      ASSERT_TRUE(aCheck.IsDone());
      const double aBasisMaximum = hasInnerKnots ? 0.75 : 0.5;
      EXPECT_NEAR(aCheck.MaxDistance(), aBasisMaximum * (aPoles.At(aMiddle).Z() - aBase), 1.0e-18);
      EXPECT_NEAR(aCheck.MaxParameter(), 1.5, Precision::PConfusion());

      const occ::handle<Adaptor3d_CurveOnSurface> aShiftedCoS =
        new Adaptor3d_CurveOnSurface(aPCurve,
                                     new GeomAdaptor_Surface(occ::down_cast<Geom_BSplineSurface>(
                                       aSurface->Surface()->Translated(gp_Vec(0.0, 0.0, 1.0)))));
      aCheck.Perform(aShiftedCoS);
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_DOUBLE_EQ(aCheck.MaxDistance(), 1.0);

      aCheck.Perform(aCoS);
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_NEAR(aCheck.MaxDistance(), aBasisMaximum * (aPoles.At(aMiddle).Z() - aBase), 1.0e-18);
      EXPECT_NEAR(aCheck.MaxParameter(), 1.5, Precision::PConfusion());
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, RationalSurfaceResidualAcrossKnotSpans)
{
  for (const bool hasInnerKnots : {false, true})
  {
    SCOPED_TRACE(hasInnerKnots);
    const size_t               aNbPoles = hasInnerKnots ? 5 : 3;
    const size_t               aMiddle  = aNbPoles / 2;
    const double               aLast    = hasInnerKnots ? 3.0 : 1.0;
    NCollection_Array1<double> aSurfaceKnots(hasInnerKnots ? size_t{4} : size_t{2});
    NCollection_Array1<int>    aSurfaceMults(aSurfaceKnots.Size());
    for (size_t anIndex = 0; anIndex < aSurfaceKnots.Size(); ++anIndex)
    {
      aSurfaceKnots.ChangeAt(anIndex) = double(anIndex);
      aSurfaceMults.ChangeAt(anIndex) = anIndex == 0 || anIndex + 1 == aSurfaceKnots.Size() ? 3 : 1;
    }
    NCollection_Array2<gp_Pnt> aSurfacePoles(aNbPoles, aNbPoles);
    NCollection_Array2<double> aSurfaceWeights(aNbPoles, aNbPoles);
    aSurfacePoles.Init(gp_Pnt(1.0e12, -1.0e12, 1000.0));
    aSurfacePoles.ChangeAt(aMiddle, aMiddle).SetZ(1000.0 + 2.0e-6);
    for (size_t aU = 0; aU < aNbPoles; ++aU)
    {
      for (size_t aV = 0; aV < aNbPoles; ++aV)
      {
        aSurfaceWeights.ChangeAt(aU, aV) =
          (aU == aMiddle ? 2.0 : 1.0) * (aV == aMiddle ? 3.0 : 1.0);
      }
    }
    const occ::handle<GeomAdaptor_Surface> aSurface =
      new GeomAdaptor_Surface(new Geom_BSplineSurface(aSurfacePoles,
                                                      aSurfaceWeights,
                                                      aSurfaceKnots,
                                                      aSurfaceKnots,
                                                      aSurfaceMults,
                                                      aSurfaceMults,
                                                      2,
                                                      2));
    const std::array<double, 4> aKnotValues = {0.0, 0.25, 0.75, 1.0};
    const std::array<double, 5> aGreville   = {0.0, 0.125, 0.5, 0.875, 1.0};
    NCollection_Array1<double>  aKnots(aKnotValues.size()), aWeights(aGreville.size());
    NCollection_Array1<int>     aMults(aKnotValues.size());
    for (size_t anIndex = 0; anIndex < aKnotValues.size(); ++anIndex)
    {
      aKnots.ChangeAt(anIndex) = aKnotValues[anIndex];
      aMults.ChangeAt(anIndex) = anIndex == 0 || anIndex + 1 == aKnotValues.size() ? 3 : 1;
    }
    aWeights.Init(1.0);
    aWeights.ChangeAt(2) = 2.0;
    NCollection_Array1<gp_Pnt> aPoles(aGreville.size());
    aPoles.Init(gp_Pnt(1.0e12, -1.0e12, 1000.0));
    NCollection_Array1<gp_Pnt2d> aUVPoles(aGreville.size());
    for (size_t anIndex = 0; anIndex < aGreville.size(); ++anIndex)
    {
      const double aParameter    = aLast * aGreville[anIndex];
      aUVPoles.ChangeAt(anIndex) = gp_Pnt2d(aParameter, aParameter);
    }
    // The middle basis peaks at 1/2 for Bezier, or 3/4 with the inner knots.
    // Tensor weights factor into independent rational bases in U and V.
    const double aBasis   = hasInnerKnots ? 0.75 : 0.5;
    const double aMaximum = (2.0 * aBasis / (1.0 + aBasis)) * (3.0 * aBasis / (1.0 + 2.0 * aBasis));
    const double anExpected = aMaximum * (aSurfacePoles.At(aMiddle, aMiddle).Z() - 1000.0);
    for (const bool isCurveRational : {false, true})
    {
      SCOPED_TRACE(isCurveRational);
      const occ::handle<Geom_BSplineCurve> aCurveGeometry =
        isCurveRational ? new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2)
                        : new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
      const occ::handle<GeomAdaptor_Curve> aCurve = new GeomAdaptor_Curve(aCurveGeometry);
      for (const bool isPCurveRational : {false, true})
      {
        SCOPED_TRACE(isPCurveRational);
        const occ::handle<Geom2d_BSplineCurve> aPCurveGeometry =
          isPCurveRational ? new Geom2d_BSplineCurve(aUVPoles, aWeights, aKnots, aMults, 2)
                           : new Geom2d_BSplineCurve(aUVPoles, aKnots, aMults, 2);
        const occ::handle<Geom2dAdaptor_Curve> aPCurve = new Geom2dAdaptor_Curve(aPCurveGeometry);
        const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
          new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
        for (const bool isParallel : {false, true})
        {
          SCOPED_TRACE(isParallel);
          GeomLib_CheckCurveOnSurface aCheck(aCurve);
          aCheck.SetParallel(isParallel);
          aCheck.Perform(aCoS);
          ASSERT_TRUE(aCheck.IsDone());
          EXPECT_NEAR(aCheck.MaxDistance(), anExpected, 1.0e-18);
          EXPECT_NEAR(aCheck.MaxParameter(), 0.5, Precision::PConfusion());
        }
      }
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SurfaceResidualPreservesEndSpanExtrapolation)
{
  NCollection_Array1<double> aKnots(size_t{2});
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 1.0;
  NCollection_Array1<int> aLinearMults(size_t{2});
  aLinearMults.Init(2);
  NCollection_Array1<gp_Pnt> aPoles(size_t{2});
  aPoles.Init(gp_Pnt(0.0, 0.0, 0.0));
  const occ::handle<GeomAdaptor_Curve> aCurve =
    new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aLinearMults, 1));
  for (const double aV : {0.0, 2.0})
  {
    SCOPED_TRACE(aV);
    NCollection_Array1<gp_Pnt2d> aUVPoles(size_t{2});
    aUVPoles.ChangeAt(0) = gp_Pnt2d(0.0, aV);
    aUVPoles.ChangeAt(1) = gp_Pnt2d(1.0, aV);
    const occ::handle<Geom2dAdaptor_Curve> aPCurve =
      new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aKnots, aLinearMults, 1));
    for (const int aVDegree : {2, 9, Geom_BSplineSurface::MaxDegree()})
    {
      SCOPED_TRACE(aVDegree);
      NCollection_Array1<int> aVMults(size_t{2});
      aVMults.Init(aVDegree + 1);
      NCollection_Array2<gp_Pnt> aSurfacePoles(size_t{2}, static_cast<size_t>(aVDegree) + 1);
      aSurfacePoles.Init(gp_Pnt(0.0, 0.0, 0.0));
      aSurfacePoles.ChangeAt(1, static_cast<size_t>(aVDegree)).SetZ(aV == 0.0 ? RealLast() : 1.0);
      const occ::handle<GeomAdaptor_Surface> aSurface = new GeomAdaptor_Surface(
        new Geom_BSplineSurface(aSurfacePoles, aKnots, aKnots, aLinearMults, aVMults, 1, aVDegree));
      const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
        new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
      GeomLib_CheckCurveOnSurface aCheck(aCurve);
      aCheck.Perform(aCoS);
      ASSERT_TRUE(aCheck.IsDone());
      // Z = U * V^degree; V = 0 is finite even when cache coefficients are too large.
      EXPECT_DOUBLE_EQ(aCheck.MaxDistance(), aV == 0.0 ? 0.0 : std::ldexp(1.0, aVDegree));
      if (aV != 0.0)
      {
        EXPECT_NEAR(aCheck.MaxParameter(), 1.0, Precision::PConfusion());
      }
    }
  }
}

TEST(GeomLib_CheckCurveOnSurfaceTest, SplineMaximumWhenIntervalsCrossCurveKnots)
{
  constexpr size_t           aNbKnots = 112;
  constexpr size_t           aPeak    = 55;
  NCollection_Array1<double> aKnots(aNbKnots), aLinearKnots(size_t{2});
  NCollection_Array1<int>    aMults(aNbKnots), aLinearMults(size_t{2});
  NCollection_Array1<gp_Pnt> aPoles(aNbKnots);
  for (size_t anIndex = 0; anIndex < aNbKnots; ++anIndex)
  {
    aKnots.ChangeAt(anIndex) = double(anIndex);
    aMults.ChangeAt(anIndex) = anIndex == 0 || anIndex + 1 == aNbKnots ? 2 : 1;
    aPoles.ChangeAt(anIndex) = gp_Pnt(anIndex == aPeak ? 1.0 : 0.0, 0.0, 0.0);
  }
  aLinearKnots.ChangeAt(0) = 0.0;
  aLinearKnots.ChangeAt(1) = double(aNbKnots - 1);
  aLinearMults.Init(2);
  NCollection_Array1<gp_Pnt2d> aUVPoles(size_t{2});
  aUVPoles.Init(gp_Pnt2d(0.0, 0.0));
  NCollection_Array1<double> aSurfaceKnots(size_t{2});
  aSurfaceKnots.ChangeAt(0) = 0.0;
  aSurfaceKnots.ChangeAt(1) = 1.0;
  NCollection_Array2<gp_Pnt> aSurfacePoles(size_t{2}, size_t{2});
  for (size_t aU = 0; aU < 2; ++aU)
  {
    for (size_t aV = 0; aV < 2; ++aV)
    {
      aSurfacePoles.ChangeAt(aU, aV) = gp_Pnt(double(aU), double(aV), double(aU * aV));
    }
  }
  const occ::handle<GeomAdaptor_Curve> aCurve =
    new GeomAdaptor_Curve(new Geom_BSplineCurve(aPoles, aKnots, aMults, 1));
  const occ::handle<Geom2dAdaptor_Curve> aPCurve =
    new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aUVPoles, aLinearKnots, aLinearMults, 1));
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_BSplineSurface(aSurfacePoles,
                                                    aSurfaceKnots,
                                                    aSurfaceKnots,
                                                    aLinearMults,
                                                    aLinearMults,
                                                    1,
                                                    1));
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
    new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
  GeomLib_CheckCurveOnSurface aCheck(aCurve);
  aCheck.Perform(aCoS);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aCheck.MaxDistance(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCheck.MaxParameter(), double(aPeak), Precision::Confusion());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, BezierMaximumWithinTrimmedRange)
{
  for (const double anOffset : {2.0e-6, 0.02, 2.0})
  {
    for (const auto& aRange : {std::pair{0.0, 1.0}, std::pair{0.0, 0.25}, std::pair{0.5, 1.0}})
    {
      const auto [aFirst, aLast] = aRange;
      SCOPED_TRACE(::testing::Message()
                   << "offset=" << anOffset << " first=" << aFirst << " last=" << aLast);
      NCollection_Array1<gp_Pnt> aPoles(1, 4);
      for (size_t anIndex = 0; anIndex < 4; ++anIndex)
      {
        aPoles.ChangeAt(anIndex) = gp_Pnt(double(anIndex) / 3.0, 0.0, 0.0);
      }
      aPoles.ChangeAt(1).SetZ(anOffset);
      NCollection_Array1<gp_Pnt2d> aUVPoles(1, 2);
      aUVPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
      aUVPoles.ChangeAt(1) = gp_Pnt2d(1.0, 0.0);
      const occ::handle<GeomAdaptor_Curve> aCurve =
        new GeomAdaptor_Curve(new Geom_BezierCurve(aPoles), aFirst, aLast);
      const occ::handle<Geom2dAdaptor_Curve> aPCurve =
        new Geom2dAdaptor_Curve(new Geom2d_BezierCurve(aUVPoles), aFirst, aLast);
      const occ::handle<GeomAdaptor_Surface> aSurface = new GeomAdaptor_Surface(
        new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0))));
      const occ::handle<Adaptor3d_CurveOnSurface> aCoS =
        new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
      GeomLib_CheckCurveOnSurface aCheck(aCurve);
      aCheck.Perform(aCoS);
      ASSERT_TRUE(aCheck.IsDone());
      // Deviation: 3 * offset * t * (1 - t)^2, peaking at 1/3.
      const double aParameter = std::clamp(1.0 / 3.0, aFirst, aLast);
      const double anExpected =
        3.0 * anOffset * aParameter * (1.0 - aParameter) * (1.0 - aParameter);
      EXPECT_NEAR(aCheck.MaxDistance(), anExpected, std::max(1.0e-18, 1.0e-14 * anExpected));
      EXPECT_NEAR(aCheck.MaxParameter(), aParameter, Precision::Confusion());
    }
  }
}

namespace
{
constexpr double THE_INTERVAL_BOUNDARY = 1.0;
constexpr double THE_LAST_PARAMETER    = 2.0;
constexpr double THE_CURVE_OFFSET      = 0.125;

// Curve with a failing second interval.
class GeomLib_IntervalFailureCurve : public Adaptor3d_Curve
{
public:
  enum class FailureMode
  {
    None,
    NonFinite,
    PositiveInfinite,
    NegativeInfinite,
    UndefinedValue
  };

  explicit GeomLib_IntervalFailureCurve(FailureMode theFailure);
  occ::handle<Adaptor3d_Curve> ShallowCopy() const override;
  double                       FirstParameter() const override;
  double                       LastParameter() const override;
  GeomAbs_CurveType            GetType() const override;
  gp_Pnt                       EvalD0(double theParameter) const override;

private:
  FailureMode myFailure; // Evaluation failure in the second interval.
};

//==================================================================================================

GeomLib_IntervalFailureCurve::GeomLib_IntervalFailureCurve(FailureMode theFailure)
    : myFailure(theFailure)
{
}

//==================================================================================================

occ::handle<Adaptor3d_Curve> GeomLib_IntervalFailureCurve::ShallowCopy() const
{
  return new GeomLib_IntervalFailureCurve(myFailure);
}

//==================================================================================================

double GeomLib_IntervalFailureCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double GeomLib_IntervalFailureCurve::LastParameter() const
{
  return THE_LAST_PARAMETER;
}

//==================================================================================================

GeomAbs_CurveType GeomLib_IntervalFailureCurve::GetType() const
{
  return GeomAbs_OtherCurve;
}

//==================================================================================================

gp_Pnt GeomLib_IntervalFailureCurve::EvalD0(double theParameter) const
{
  if (theParameter > THE_INTERVAL_BOUNDARY)
  {
    if (myFailure == FailureMode::UndefinedValue)
    {
      throw Geom_UndefinedValue("Undefined curve value");
    }
    if (myFailure == FailureMode::NonFinite)
    {
      return gp_Pnt(theParameter, 0.0, std::numeric_limits<double>::quiet_NaN());
    }
    if (myFailure == FailureMode::PositiveInfinite || myFailure == FailureMode::NegativeInfinite)
    {
      return gp_Pnt(theParameter,
                    0.0,
                    myFailure == FailureMode::PositiveInfinite ? Precision::Infinite()
                                                               : -Precision::Infinite());
    }
  }
  return gp_Pnt(theParameter, 0.0, THE_CURVE_OFFSET);
}

//==================================================================================================

occ::handle<Adaptor3d_CurveOnSurface> makeTwoIntervalPCurve()
{
  constexpr int                aDegree  = 1;
  constexpr int                aNbKnots = 3;
  NCollection_Array1<gp_Pnt2d> aPoles(1, aNbKnots);
  NCollection_Array1<double>   aKnots(1, aNbKnots);
  NCollection_Array1<int>      aMults(1, aNbKnots);
  for (size_t anIndex = 0; anIndex < aKnots.Size(); ++anIndex)
  {
    aPoles.ChangeAt(anIndex) = gp_Pnt2d(static_cast<double>(anIndex), 0.0);
    aKnots.ChangeAt(anIndex) = static_cast<double>(anIndex);
    aMults.ChangeAt(anIndex) = anIndex == 0 || anIndex + 1 == aKnots.Size() ? aDegree + 1 : 1;
  }
  const occ::handle<Geom2dAdaptor_Curve> aPCurve =
    new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aPoles, aKnots, aMults, aDegree));
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3()));
  return new Adaptor3d_CurveOnSurface(aPCurve, aSurface);
}
} // namespace

//==================================================================================================

TEST(GeomLib_CheckCurveOnSurfaceTest, FailedIntervalDoesNotReportPartialMaximum)
{
  const double                aFiniteDistance = std::sqrt(Precision::Infinite());
  GeomLib_CheckCurveOnSurface aFiniteCheck(
    new GeomAdaptor_Curve(new Geom_Line(gp_Pnt(0.0, 0.0, aFiniteDistance), gp_Dir(1.0, 0.0, 0.0)),
                          0.0,
                          THE_LAST_PARAMETER));
  aFiniteCheck.Perform(makeTwoIntervalPCurve());
  ASSERT_TRUE(aFiniteCheck.IsDone());
  EXPECT_DOUBLE_EQ(aFiniteCheck.MaxDistance(), aFiniteDistance);

  for (const GeomLib_IntervalFailureCurve::FailureMode aFailure :
       {GeomLib_IntervalFailureCurve::FailureMode::NonFinite,
        GeomLib_IntervalFailureCurve::FailureMode::PositiveInfinite,
        GeomLib_IntervalFailureCurve::FailureMode::NegativeInfinite,
        GeomLib_IntervalFailureCurve::FailureMode::UndefinedValue})
  {
    SCOPED_TRACE(static_cast<int>(aFailure));
    for (const bool aParallel : {false, true})
    {
      SCOPED_TRACE(aParallel);
      GeomLib_CheckCurveOnSurface aCheck(new GeomLib_IntervalFailureCurve(aFailure));
      aCheck.SetParallel(aParallel);
      EXPECT_NO_THROW(aCheck.Perform(makeTwoIntervalPCurve()));
      EXPECT_FALSE(aCheck.IsDone());
      EXPECT_EQ(aCheck.ErrorStatus(), 3);
      EXPECT_EQ(aCheck.MaxDistance(), RealLast());
      EXPECT_EQ(aCheck.MaxParameter(), 0.0);
    }
  }
}

//==================================================================================================

TEST(GeomLib_CheckCurveOnSurfaceTest, RepeatedPerformResetsStatusAndDistance)
{
  GeomLib_CheckCurveOnSurface aCheck(
    new GeomLib_IntervalFailureCurve(GeomLib_IntervalFailureCurve::FailureMode::None));
  const occ::handle<Adaptor3d_CurveOnSurface> aPCurve = makeTwoIntervalPCurve();
  aCheck.Perform(aPCurve);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_DOUBLE_EQ(aCheck.MaxDistance(), THE_CURVE_OFFSET);

  aCheck.Perform(nullptr);
  EXPECT_FALSE(aCheck.IsDone());
  EXPECT_EQ(aCheck.ErrorStatus(), 1);
  EXPECT_EQ(aCheck.MaxDistance(), RealLast());
  EXPECT_EQ(aCheck.MaxParameter(), 0.0);

  aCheck.Perform(aPCurve);
  ASSERT_TRUE(aCheck.IsDone());
  EXPECT_EQ(aCheck.ErrorStatus(), 0);
  EXPECT_DOUBLE_EQ(aCheck.MaxDistance(), THE_CURVE_OFFSET);
}

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

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepLib_ValidateEdge.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Plane.hxx>
#include <GeomConvert.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

#include <array>

TEST(BRepCheckTest, ExactPlaneDistanceIsIndependentOfParameterOrigin)
{
  constexpr double aLargeCoordinate = 281474976710656.0;
  for (const double anOrigin : {0.0, aLargeCoordinate, -aLargeCoordinate})
  {
    for (const double aGap : {0.0, 0.01})
    {
      for (const bool isRational : {false, true})
      {
        for (const bool isBSpline : {false, true})
        {
          SCOPED_TRACE(anOrigin);
          SCOPED_TRACE(aGap);
          SCOPED_TRACE(isRational);
          SCOPED_TRACE(isBSpline);
          NCollection_Array1<gp_Pnt> aPoles(1, 2);
          aPoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, aGap);
          aPoles.ChangeAt(1) = gp_Pnt(1.0, 0.0, aGap);
          NCollection_Array1<double> aWeights(1, 2);
          aWeights(1) = 1.0;
          aWeights(2) = 2.0;
          const occ::handle<Geom_BezierCurve> aBezier =
            isRational ? new Geom_BezierCurve(aPoles, aWeights) : new Geom_BezierCurve(aPoles);
          NCollection_Array1<gp_Pnt2d> aPoles2d(1, 2);
          for (size_t anIndex = 0; anIndex < aPoles2d.Size(); ++anIndex)
          {
            aPoles2d.ChangeAt(anIndex) = gp_Pnt2d(-anOrigin + double(anIndex), 0.0);
          }
          const occ::handle<Geom2d_BezierCurve> aBezier2d =
            isRational ? new Geom2d_BezierCurve(aPoles2d, aWeights)
                       : new Geom2d_BezierCurve(aPoles2d);
          const occ::handle<Geom_Curve>   aCurve = isBSpline
                                                     ? GeomConvert::CurveToBSplineCurve(aBezier)
                                                     : occ::handle<Geom_Curve>(aBezier);
          const occ::handle<Geom2d_Curve> aPCurve =
            isBSpline ? Geom2dConvert::CurveToBSplineCurve(aBezier2d)
                      : occ::handle<Geom2d_Curve>(aBezier2d);
          const occ::handle<Geom_Plane> aPlane =
            new Geom_Plane(gp_Ax3(gp_Pnt(anOrigin, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
          const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
          const occ::handle<Adaptor3d_CurveOnSurface> anOther =
            new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve, 0.0, 1.0),
                                         new GeomAdaptor_Surface(aPlane));
          BRepLib_ValidateEdge aValidator(aReference, anOther, true);
          aValidator.SetExactMethod(true);
          aValidator.Process();
          ASSERT_TRUE(aValidator.IsDone());
          EXPECT_EQ(aValidator.CheckTolerance(Precision::Confusion()), aGap == 0.0);
          GeomLib_CheckCurveOnSurface aDistance(aReference);
          aDistance.Perform(anOther);
          ASSERT_TRUE(aDistance.IsDone());
          EXPECT_NEAR(aDistance.MaxDistance(), aGap, RealEpsilon());
        }
      }
    }
  }
}

TEST(BRepCheckTest, SplineToleranceDoesNotHideRepresentableGap)
{
  constexpr double aLargeCoordinate      = 65537.0;
  constexpr double aGap                  = 0.25;
  const double     aCoordinateResolution = Epsilon(aLargeCoordinate);
  const double     aBorderlineTolerance  = aGap - 0.5 * aCoordinateResolution;
  const double     aSmallerTolerance     = aGap - 2.0 * aCoordinateResolution;
  for (const double aCoordinate : std::array<double, 3>{0.0, aLargeCoordinate, -aLargeCoordinate})
  {
    for (const bool isBSpline : std::array<bool, 2>{false, true})
    {
      SCOPED_TRACE(aCoordinate);
      SCOPED_TRACE(isBSpline);
      NCollection_Array1<gp_Pnt> aPoles(1, 2);
      aPoles(1) = gp_Pnt(aCoordinate + aGap, 0.0, 0.0);
      aPoles(2) = gp_Pnt(aCoordinate + aGap, 1.0, 0.0);
      occ::handle<Geom_Curve> aCurve;
      if (isBSpline)
      {
        NCollection_Array1<double> aKnots(1, 2);
        NCollection_Array1<int>    aMultiplicities(1, 2);
        aKnots(1) = 0.0;
        aKnots(2) = 1.0;
        aMultiplicities.Init(2);
        aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
      }
      else
      {
        aCurve = new Geom_BezierCurve(aPoles);
      }
      const occ::handle<Geom2d_Line> aPCurve =
        new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
      const occ::handle<Geom_Plane> aPlane = new Geom_Plane(
        gp_Ax3(gp_Pnt(aCoordinate, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)));
      const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
      const occ::handle<Adaptor3d_CurveOnSurface> anOther =
        new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve, 0.0, 1.0),
                                     new GeomAdaptor_Surface(aPlane));

      for (const bool isExact : std::array<bool, 2>{false, true})
      {
        SCOPED_TRACE(isExact);
        BRepLib_ValidateEdge aValidator(aReference, anOther, true);
        aValidator.SetExactMethod(isExact);
        aValidator.Process();

        ASSERT_TRUE(aValidator.IsDone());
        EXPECT_FALSE(aValidator.CheckTolerance(aBorderlineTolerance));
        EXPECT_FALSE(aValidator.CheckTolerance(aSmallerTolerance));
        EXPECT_TRUE(aValidator.CheckTolerance(aGap + aCoordinateResolution));
      }
    }
  }
}

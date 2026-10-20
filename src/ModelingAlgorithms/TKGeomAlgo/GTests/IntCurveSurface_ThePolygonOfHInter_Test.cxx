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

#include <IntCurveSurface_ThePolygonOfHInter.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(IntCurveSurface_ThePolygonOfHInter_Test, ClosedFlagReflectsStoredState)
{
  occ::handle<Geom_Line> aLine =
    new Geom_Line(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)));
  occ::handle<GeomAdaptor_Curve>     anAdaptor = new GeomAdaptor_Curve(aLine);
  IntCurveSurface_ThePolygonOfHInter aPolygon(anAdaptor, 6);

  EXPECT_FALSE(aPolygon.Closed());
  aPolygon.Closed(true);
  EXPECT_TRUE(aPolygon.Closed());
}

//==================================================================================================

TEST(IntCurveSurface_ThePolygonOfHInter_Test, CollapsedChordRetainsMidpointDeparture)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.ChangeAt(0)                             = gp_Pnt(0, 0, 0);
  aPoles.ChangeAt(1)                             = gp_Pnt(-.125, 0, 0);
  aPoles.ChangeAt(2)                             = gp_Pnt(.75, 0, 0);
  const occ::handle<Geom_BezierCurve>  aCurve    = new Geom_BezierCurve(aPoles);
  const occ::handle<GeomAdaptor_Curve> anAdaptor = new GeomAdaptor_Curve(aCurve);
  NCollection_Array1<double>           aParameters(1, 4);
  aParameters.ChangeAt(0) = 0;
  aParameters.ChangeAt(1) = .25;
  aParameters.ChangeAt(2) = .5;
  aParameters.ChangeAt(3) = 1;
  const IntCurveSurface_ThePolygonOfHInter aExplicit(anAdaptor, aParameters);
  EXPECT_GE(aExplicit.DeflectionOverEstimation(), .015625);
  EXPECT_LE(aExplicit.Bounding().Get().Xmin, -.015625);
  const IntCurveSurface_ThePolygonOfHInter aUniform(anAdaptor, 5);
  EXPECT_GE(aUniform.DeflectionOverEstimation(), .015625);
  aPoles.Init(gp_Pnt(1, 2, 3));
  const occ::handle<Geom_BezierCurve>      aConstant        = new Geom_BezierCurve(aPoles);
  const occ::handle<GeomAdaptor_Curve>     aConstantAdaptor = new GeomAdaptor_Curve(aConstant);
  const IntCurveSurface_ThePolygonOfHInter aConstantPolygon(aConstantAdaptor, 4);
  EXPECT_EQ(aConstantPolygon.DeflectionOverEstimation(), 0.0);
}
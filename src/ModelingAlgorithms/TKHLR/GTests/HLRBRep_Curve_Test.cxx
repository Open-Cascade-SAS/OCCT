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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_BezierCurve.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Trsf.hxx>
#include <gtest/gtest.h>

TEST(HLRBRep_CurveTest, NearlyAxialBezierRetainsProjectedDerivative)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles.ChangeAt(0)                          = gp_Pnt(0.0, 0.0, 0.0);
  aPoles.ChangeAt(1)                          = gp_Pnt(1.0, 0.0, 1.e13);
  const occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  const TopoDS_Edge                   anEdge  = BRepBuilderAPI_MakeEdge(aBezier);
  HLRAlgo_Projector                   aProjector(gp_Trsf(), false, 1.0);
  HLRBRep_Curve                       aCurve;
  aCurve.Curve(anEdge);
  aCurve.Projector(&aProjector);
  gp_Pnt2d aStart, anEnd, aPoint;
  gp_Vec2d aD1, aD2;
  aCurve.D0(0.0, aStart);
  aCurve.D0(1.0, anEnd);
  EXPECT_DOUBLE_EQ(anEnd.X() - aStart.X(), 1.0);
  aCurve.D1(0.5, aPoint, aD1);
  EXPECT_DOUBLE_EQ(aD1.X(), 1.0);
  aCurve.D2(0.5, aPoint, aD1, aD2);
  EXPECT_DOUBLE_EQ(aD1.X(), 1.0);
}

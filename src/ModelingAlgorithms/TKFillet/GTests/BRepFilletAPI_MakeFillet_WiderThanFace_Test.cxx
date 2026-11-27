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

// A fillet wider than the face it sits on. Related report:
// - https://github.com/Open-Cascade-SAS/OCCT/issues/1177

#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
//! Returns the edge of the shape whose middle is at the given point.
TopoDS_Edge findEdgeAt(const TopoDS_Shape& theShape, const gp_Pnt& theMiddle)
{
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    const gp_Pnt aMiddle = aCurve.Value(0.5 * (aCurve.FirstParameter() + aCurve.LastParameter()));
    if (aMiddle.Distance(theMiddle) < 1.e-7)
    {
      return anEdge;
    }
  }
  return TopoDS_Edge();
}

//! Volume of the shape.
double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}
} // namespace

// r = 5 on a 4 wide face: the arc is tangent to the side face and passes through the far edge of
// the top face, centre (5, 6 - sqrt(2 w r - w^2)). Removed area 4.927428.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1177_FilletWiderThanItsFaceEndsAtTheFarEdge)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(5.0, findEdgeAt(aBox, gp_Pnt(0.0, 5.0, 6.0)));
  ASSERT_NO_THROW(aFillet.Build());
  ASSERT_TRUE(aFillet.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
  EXPECT_NEAR(volumeOf(aFillet.Shape()), 240.0 - 10.0 * 4.927428, 1.e-4);
}

// r = 4.5 on the edge between a 4 wide face and a 3 high one: the arc passes through both far
// edges. Removed area 3.426266.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1177_FilletWiderThanBothFacesEndsAtBothFarEdges)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(4.0, 10.0, 3.0).Shape();
  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(4.5, findEdgeAt(aBox, gp_Pnt(0.0, 5.0, 3.0)));
  ASSERT_NO_THROW(aFillet.Build());
  ASSERT_TRUE(aFillet.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
  EXPECT_NEAR(volumeOf(aFillet.Shape()), 120.0 - 10.0 * 3.426266, 1.e-4);
}

// The same fillet again after a finished one: the second fillet pivots at the first one's
// tangent edge, so the result depends on the order (213.812565 for 3 then 3 on the 4 wide face).
TEST(BRepFilletAPI_MakeFilletTest, OCCT1177_FilletNextToAFinishedFilletPivotsAtItsTangentEdge)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  BRepFilletAPI_MakeFillet aFirst(aBox);
  aFirst.Add(3.0, findEdgeAt(aBox, gp_Pnt(0.0, 5.0, 6.0)));
  ASSERT_NO_THROW(aFirst.Build());
  ASSERT_TRUE(aFirst.IsDone());
  const TopoDS_Shape aRounded = aFirst.Shape();

  BRepFilletAPI_MakeFillet aSecond(aRounded);
  aSecond.Add(3.0, findEdgeAt(aRounded, gp_Pnt(4.0, 5.0, 6.0)));
  ASSERT_NO_THROW(aSecond.Build());
  ASSERT_TRUE(aSecond.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aSecond.Shape()).IsValid());
  EXPECT_NEAR(volumeOf(aSecond.Shape()), 213.812565, 1.e-4);
}

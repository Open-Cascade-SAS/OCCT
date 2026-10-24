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

#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOP    = 20.0;
constexpr double THE_FLOOR  = 17.5;
constexpr double THE_RIM_Y  = 88.097;
constexpr double THE_WALL_X = 55.25;

//! Plate with a pocket that has a vertical end wall at x = THE_WALL_X and a side wall at
//! y = THE_RIM_Y that leans inward by 10 degrees.
TopoDS_Shape makePocketedPlate()
{
  const TopoDS_Shape aPlate =
    BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(60.0, 100.0, THE_TOP)).Shape();

  // The profile is drawn in the plane x = 5 and extruded along +X; it overshoots the top plane.
  const double               aSlope  = std::tan(10.0 * M_PI / 180.0);
  const double               aYFloor = THE_RIM_Y + (THE_TOP - THE_FLOOR) * aSlope;
  const double               aYAbove = THE_RIM_Y - 1.0 * aSlope;
  BRepBuilderAPI_MakePolygon aProfile;
  aProfile.Add(gp_Pnt(5.0, 65.0, THE_FLOOR));
  aProfile.Add(gp_Pnt(5.0, aYFloor, THE_FLOOR));
  aProfile.Add(gp_Pnt(5.0, aYAbove, THE_TOP + 1.0));
  aProfile.Add(gp_Pnt(5.0, 65.0, THE_TOP + 1.0));
  aProfile.Close();
  const TopoDS_Shape aTool = BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aProfile.Wire()).Face(),
                                                   gp_Vec(THE_WALL_X - 5.0, 0.0, 0.0))
                               .Shape();
  return BRepAlgoAPI_Cut(aPlate, aTool).Shape();
}

//! Selects the edges of a shape with the given predicate and rounds them with one radius.
template <typename Predicate>
TopoDS_Shape roundEdges(const TopoDS_Shape& theShape, const double theRadius, Predicate thePick)
{
  BRepFilletAPI_MakeFillet aFillet(theShape, ChFi3d_Rational);
  aFillet.SetParams(1.e-2, 1.e-4, 1.e-5, 1.e-4, 1.e-5, 1.e-3);
  aFillet.SetContinuity(GeomAbs_C1, 1.e-2);
  size_t aNbEdges = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    if (thePick(BRepAdaptor_Curve(anEdge)))
    {
      aFillet.Add(theRadius, anEdge);
      ++aNbEdges;
    }
  }
  EXPECT_GT(aNbEdges, 0u);
  aFillet.Build();
  return aFillet.IsDone() ? aFillet.Shape() : TopoDS_Shape();
}

bool isInsidePlate(const gp_Pnt& thePnt)
{
  return thePnt.X() > 1.0 && thePnt.X() < 59.0 && thePnt.Y() > 1.0 && thePnt.Y() < 99.0;
}
} // namespace

// The rim blend of a pocket meets a bottom round of the same wall. The blend ends on the round,
// which has no edge matching the arc edge of the rim, and Build() must return instead of crashing.
TEST(BRepFilletAPI_MakeFilletTest, RimBlendEndingOnBottomRoundDoesNotCrash)
{
  TopoDS_Shape aShape = makePocketedPlate();
  ASSERT_FALSE(aShape.IsNull());

  // The edge between the end wall and the leaning wall.
  aShape = roundEdges(aShape, 2.0, [](const BRepAdaptor_Curve& theCurve) {
    const gp_Pnt aFirst = theCurve.Value(theCurve.FirstParameter());
    const gp_Pnt aLast  = theCurve.Value(theCurve.LastParameter());
    return theCurve.GetType() == GeomAbs_Line && isInsidePlate(aFirst) && isInsidePlate(aLast)
           && std::abs(aLast.Z() - aFirst.Z()) > 0.5 && aFirst.X() > 50.0 && aLast.X() > 50.0
           && aFirst.Y() > 80.0 && aLast.Y() > 80.0;
  });
  ASSERT_FALSE(aShape.IsNull());

  // Every edge of the pocket floor, so the round runs along the walls and the rounded corner.
  aShape = roundEdges(aShape, 1.5, [](const BRepAdaptor_Curve& theCurve) {
    const gp_Pnt aFirst = theCurve.Value(theCurve.FirstParameter());
    const gp_Pnt aLast  = theCurve.Value(theCurve.LastParameter());
    const gp_Pnt aMid =
      theCurve.Value(0.5 * (theCurve.FirstParameter() + theCurve.LastParameter()));
    return isInsidePlate(aFirst) && isInsidePlate(aLast) && std::abs(aFirst.Z() - THE_FLOOR) < 1.e-6
           && std::abs(aMid.Z() - THE_FLOOR) < 1.e-6 && std::abs(aLast.Z() - THE_FLOOR) < 1.e-6;
  });
  ASSERT_FALSE(aShape.IsNull());

  // The straight rim edge on the leaning wall. Used to terminate the process with SIGSEGV in
  // Build().
  const TopoDS_Shape aResult = roundEdges(aShape, 1.0, [](const BRepAdaptor_Curve& theCurve) {
    const gp_Pnt aFirst = theCurve.Value(theCurve.FirstParameter());
    const gp_Pnt aLast  = theCurve.Value(theCurve.LastParameter());
    return theCurve.GetType() == GeomAbs_Line && isInsidePlate(aFirst) && isInsidePlate(aLast)
           && std::abs(aFirst.Z() - THE_TOP) < 1.e-6 && std::abs(aLast.Z() - THE_TOP) < 1.e-6
           && std::abs(aFirst.Y() - aLast.Y()) < 1.e-6 && aFirst.Y() > 80.0;
  });

  // The blend is declined here, so only the return of Build() is checked.
  (void)aResult;
}

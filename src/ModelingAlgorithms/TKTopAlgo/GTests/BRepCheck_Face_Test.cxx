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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Face.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom_Line.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

namespace
{

TopoDS_Wire makeRectangle(const double theXMin,
                          const double theYMin,
                          const double theXMax,
                          const double theYMax)
{
  const gp_Pnt aP1(theXMin, theYMin, 0.0);
  const gp_Pnt aP2(theXMax, theYMin, 0.0);
  const gp_Pnt aP3(theXMax, theYMax, 0.0);
  const gp_Pnt aP4(theXMin, theYMax, 0.0);

  BRepBuilderAPI_MakeEdge anEdgeBuilders[4] = {BRepBuilderAPI_MakeEdge(aP1, aP2),
                                               BRepBuilderAPI_MakeEdge(aP2, aP3),
                                               BRepBuilderAPI_MakeEdge(aP3, aP4),
                                               BRepBuilderAPI_MakeEdge(aP4, aP1)};
  BRepBuilderAPI_MakeWire aWireBuilder;
  for (BRepBuilderAPI_MakeEdge& anEdgeBuilder : anEdgeBuilders)
  {
    if (!anEdgeBuilder.IsDone())
    {
      return TopoDS_Wire();
    }
    aWireBuilder.Add(anEdgeBuilder.Edge());
  }
  return aWireBuilder.IsDone() ? aWireBuilder.Wire() : TopoDS_Wire();
}

TopoDS_Face makePlanarFace()
{
  const TopoDS_Wire anOuterWire = makeRectangle(0.0, 0.0, 100.0, 100.0);
  if (anOuterWire.IsNull())
  {
    return TopoDS_Face();
  }

  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                                       anOuterWire,
                                       true);
  return aFaceBuilder.IsDone() ? aFaceBuilder.Face() : TopoDS_Face();
}

bool addCircularHole(TopoDS_Face& theFace, const gp_Pnt& theCenter, const double theRadius)
{
  BRepBuilderAPI_MakeEdge anEdgeBuilder(
    gp_Circ(gp_Ax2(theCenter, gp_Dir(0.0, 0.0, 1.0)), theRadius));
  if (!anEdgeBuilder.IsDone())
  {
    return false;
  }

  BRepBuilderAPI_MakeWire aWireBuilder(anEdgeBuilder.Edge());
  if (!aWireBuilder.IsDone())
  {
    return false;
  }

  TopoDS_Wire aHole = aWireBuilder.Wire();
  aHole.Reverse();
  BRep_Builder().Add(theFace, aHole);
  return true;
}

BRepCheck_Status classifyWires(const TopoDS_Face& theFace)
{
  BRepCheck_Face aChecker(theFace);
  return aChecker.ClassifyWires();
}

} // namespace

//=================================================================================================

TEST(BRepCheck_FaceTest, ClassifyWires_DisjointHolesAreValid)
{
  TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  for (int aRow = 0; aRow < 5; ++aRow)
  {
    for (int aColumn = 0; aColumn < 8; ++aColumn)
    {
      const gp_Pnt aCenter(10.0 + 11.0 * aColumn, 10.0 + 11.0 * aRow, 0.0);
      ASSERT_TRUE(addCircularHole(aFace, aCenter, 2.0));
    }
  }

  EXPECT_EQ(classifyWires(aFace), BRepCheck_NoError);
  EXPECT_TRUE(BRepCheck_Analyzer(aFace).IsValid());
}

//=================================================================================================

TEST(BRepCheck_FaceTest, ClassifyWires_OffsetPCurve)
{
  TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  const gp_Pnt2d             aCenter2d(50.0, 50.0);
  occ::handle<Geom2d_Circle> aBasis =
    new Geom2d_Circle(gp_Ax2d(aCenter2d, gp_Dir2d(1.0, 0.0)), 4.0);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aBasis, 1.0);
  const double                    aRadius  = anOffset->Value(0.0).Distance(aCenter2d);

  BRepBuilderAPI_MakeEdge anEdgeBuilder(
    gp_Circ(gp_Ax2(gp_Pnt(50.0, 50.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), aRadius));
  ASSERT_TRUE(anEdgeBuilder.IsDone());

  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  BRep_Builder().UpdateEdge(anEdge, anOffset, aFace, Precision::Confusion());
  BRepBuilderAPI_MakeWire aWireBuilder(anEdge);
  ASSERT_TRUE(aWireBuilder.IsDone());

  TopoDS_Wire aHole = aWireBuilder.Wire();
  aHole.Reverse();
  BRep_Builder().Add(aFace, aHole);

  EXPECT_EQ(classifyWires(aFace), BRepCheck_NoError);
}

//=================================================================================================

TEST(BRepCheck_FaceTest, ClassifyWires_OpenWireFallbackIsOrientationIndependent)
{
  TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  BRepBuilderAPI_MakeEdge anEdgeBuilder(gp_Pnt(120.0, 10.0, 0.0), gp_Pnt(130.0, 10.0, 0.0));
  ASSERT_TRUE(anEdgeBuilder.IsDone());
  BRepBuilderAPI_MakeWire aWireBuilder(anEdgeBuilder.Edge());
  ASSERT_TRUE(aWireBuilder.IsDone());
  BRep_Builder().Add(aFace, aWireBuilder.Wire());

  const BRepCheck_Status aForwardStatus = classifyWires(aFace);
  const TopoDS_Face      aReversedFace  = TopoDS::Face(aFace.Reversed());
  EXPECT_EQ(aForwardStatus, BRepCheck_NoError);
  EXPECT_EQ(classifyWires(aReversedFace), aForwardStatus);
}

//=================================================================================================

TEST(BRepCheck_FaceTest, ClassifyWires_UnboundedWireFallbackIsOrientationIndependent)
{
  TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  const occ::handle<Geom_Line> aLine =
    new Geom_Line(gp_Lin(gp_Pnt(0.0, 120.0, 0.0), gp_Dir(1.0, 0.0, 0.0)));
  BRepBuilderAPI_MakeEdge anEdgeBuilder(aLine);
  ASSERT_TRUE(anEdgeBuilder.IsDone());
  BRepBuilderAPI_MakeWire aWireBuilder(anEdgeBuilder.Edge());
  ASSERT_TRUE(aWireBuilder.IsDone());
  BRep_Builder().Add(aFace, aWireBuilder.Wire());

  const BRepCheck_Status aForwardStatus = classifyWires(aFace);
  const TopoDS_Face      aReversedFace  = TopoDS::Face(aFace.Reversed());
  EXPECT_EQ(aForwardStatus, BRepCheck_NoError);
  EXPECT_EQ(classifyWires(aReversedFace), aForwardStatus);
}

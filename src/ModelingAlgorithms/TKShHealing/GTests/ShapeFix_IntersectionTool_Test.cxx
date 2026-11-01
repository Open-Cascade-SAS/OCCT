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

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Precision.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_IntersectionTool.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pln.hxx>

TEST(ShapeFix_IntersectionToolTest, SplitLaterEdgeContinuesFirstEdgeScan)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(0.0, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(4.0, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(4.0, 4.0, 0.0));
  aPolygon.Add(gp_Pnt(-4.0, -4.0, 0.0));
  aPolygon.Add(gp_Pnt(-4.0, 4.0, 0.0));
  aPolygon.Add(gp_Pnt(0.0, 4.0, 0.0));
  aPolygon.Close();
  ASSERT_TRUE(aPolygon.IsDone());
  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0)),
                                       aPolygon.Wire(),
                                       true);
  ASSERT_TRUE(aFaceBuilder.IsDone());

  // The first vertex lies inside edge 3. Splitting that edge continues the inner
  // loop with num1 == 1, so the cached first edge must be refreshed for later pairs.
  occ::handle<ShapeExtend_WireData>     aWire    = new ShapeExtend_WireData(aPolygon.Wire());
  const occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  ShapeFix_IntersectionTool             aTool(aContext, Precision::Confusion());
  int                                   aNbSplit = 0, aNbCut = 0, aNbRemoved = 0;
  EXPECT_TRUE(aTool.FixSelfIntersectWire(aWire, aFaceBuilder.Face(), aNbSplit, aNbCut, aNbRemoved));
  EXPECT_EQ(aNbSplit, 1);
  EXPECT_EQ(aNbCut, 0);
  EXPECT_EQ(aNbRemoved, 0);
  EXPECT_EQ(aWire->NbEdges(), 7);
  EXPECT_TRUE(BRepCheck_Analyzer(aWire->Wire()).IsValid());
}

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
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

//==================================================================================================

TEST(BRepOffsetAPI_MakeOffsetTest, CollapsedOffsetReportsFailure)
{
  const gp_Pnt  aPoints[]     = {gp_Pnt(13.9390666941141, 7.9420793981839504, 35.856828275703201),
                                 gp_Pnt(13.9523335293328, 7.9409297860578096, 35.833849443048798),
                                 gp_Pnt(13.912458477387499, 7.9435331519039503, 35.902915058972503)};
  const double  aTolerances[] = {1.0e-4, 1.0e-4, 1.0e-3};
  TopoDS_Vertex aVertices[3];
  BRep_Builder  aBuilder;
  for (size_t anIndex = 0; anIndex < 3; ++anIndex)
  {
    aVertices[anIndex] = BRepBuilderAPI_MakeVertex(aPoints[anIndex]);
    aBuilder.UpdateVertex(aVertices[anIndex], aTolerances[anIndex]);
  }
  TopoDS_Wire aSpine;
  aBuilder.MakeWire(aSpine);
  BRepBuilderAPI_MakeEdge anEdges[] = {{aVertices[0], aVertices[1]},
                                       {aVertices[2], aVertices[0]},
                                       {aVertices[2], aVertices[1]}};
  for (BRepBuilderAPI_MakeEdge& anEdge : anEdges)
  {
    ASSERT_TRUE(anEdge.IsDone());
    aBuilder.Add(aSpine, anEdge.Edge());
  }

  BRepOffsetAPI_MakeOffset anOffset(aSpine);
  // The inward offset exceeds the thin contour's width.
  anOffset.Perform(-1.3157);
  EXPECT_FALSE(anOffset.IsDone());
}

//==================================================================================================

TEST(BRepOffsetAPI_MakeOffsetTest, ClosedOffsetsKeepClosedWires)
{
  constexpr double           aSide = 10.0;
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(0.0, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(aSide, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(aSide, aSide, 0.0));
  aPolygon.Add(gp_Pnt(0.0, aSide, 0.0));
  aPolygon.Close();
  ASSERT_TRUE(aPolygon.IsDone());
  for (const double aDistance : {-1.0, 1.0})
  {
    SCOPED_TRACE(aDistance);
    BRepOffsetAPI_MakeOffset anOffset(aPolygon.Wire());
    anOffset.Perform(aDistance);
    ASSERT_TRUE(anOffset.IsDone());
    const TopoDS_Shape& aResult = anOffset.Shape();
    ASSERT_FALSE(aResult.IsNull());
    EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
    size_t aNbWires = 0;
    for (TopExp_Explorer anExp(aResult, TopAbs_WIRE); anExp.More(); anExp.Next())
    {
      ++aNbWires;
      BRepCheck_Wire aCheck(TopoDS::Wire(anExp.Current()));
      EXPECT_EQ(aCheck.Closed(), BRepCheck_NoError);
    }
    EXPECT_EQ(aNbWires, 1u);
    GProp_GProps aProperties;
    BRepGProp::LinearProperties(aResult, aProperties);
    const double aLength =
      aDistance > 0.0 ? 4.0 * aSide + 2.0 * M_PI * aDistance : 4.0 * (aSide + 2.0 * aDistance);
    EXPECT_NEAR(aProperties.Mass(), aLength, Precision::Confusion());
  }
}

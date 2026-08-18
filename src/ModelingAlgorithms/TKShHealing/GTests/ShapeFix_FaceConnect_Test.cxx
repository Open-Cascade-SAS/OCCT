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
#include <BRepGProp.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Map.hxx>
#include <ShapeFix_FaceConnect.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>

#include <gtest/gtest.h>

#include <initializer_list>

namespace
{
//=================================================================================================

TopoDS_Face makeRectangle(const double theXMin, const double theXMax)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(theXMin, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(theXMax, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(theXMax, 1.0, 0.0));
  aPolygon.Add(gp_Pnt(theXMin, 1.0, 0.0));
  aPolygon.Close();
  return BRepBuilderAPI_MakeFace(aPolygon.Wire()).Face();
}

//=================================================================================================

TopoDS_Shell makeShell(const TopoDS_Face& theFace1, const TopoDS_Face& theFace2)
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, theFace1);
  aBuilder.Add(aShell, theFace2);
  return aShell;
}

//=================================================================================================

TopoDS_Shell makeShell(std::initializer_list<TopoDS_Face> theFaces)
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  for (const TopoDS_Face& aFace : theFaces)
  {
    aBuilder.Add(aShell, aFace);
  }
  return aShell;
}

//=================================================================================================

size_t countUniqueEdges(const TopoDS_Shape& theShape)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  for (TopExp_Explorer anEdgeExplorer(theShape, TopAbs_EDGE); anEdgeExplorer.More();
       anEdgeExplorer.Next())
  {
    anEdges.Add(anEdgeExplorer.Current());
  }
  return anEdges.Extent();
}

//=================================================================================================

double surfaceArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProperties;
  BRepGProp::SurfaceProperties(theShape, aProperties);
  return aProperties.Mass();
}
} // namespace

//=================================================================================================

TEST(ShapeFix_FaceConnectTest, RejectsNullFaces)
{
  ShapeFix_FaceConnect aConnector;
  const TopoDS_Face    aFace = makeRectangle(0.0, 1.0);
  const TopoDS_Face    aNullFace;

  EXPECT_FALSE(aConnector.Add(aNullFace, aFace));
  EXPECT_FALSE(aConnector.Add(aFace, aNullFace));
  EXPECT_FALSE(aConnector.Add(aNullFace, aNullFace));
}

TEST(ShapeFix_FaceConnectTest, AcceptsSelfAndDuplicateConnections)
{
  ShapeFix_FaceConnect aConnector;
  const TopoDS_Face    aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face    aFace2 = makeRectangle(1.0, 2.0);

  EXPECT_TRUE(aConnector.Add(aFace1, aFace1));
  EXPECT_TRUE(aConnector.Add(aFace1, aFace2));
  EXPECT_TRUE(aConnector.Add(aFace1, aFace2));
}

TEST(ShapeFix_FaceConnectTest, NoConnectionsReturnOriginalShell)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  const TopoDS_Shell   aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_TRUE(aResult.IsSame(aShell));
  EXPECT_EQ(countUniqueEdges(aResult), 8);
}

TEST(ShapeFix_FaceConnectTest, ConnectsCoincidentBoundaries)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const double       anInputArea = surfaceArea(aShell);
  const TopoDS_Shell aResult     = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 7);
  EXPECT_NEAR(anInputArea, 2.0, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), anInputArea, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), 2.0, 1.0e-12);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeFix_FaceConnectTest, ConnectsBoundariesWithinSewingTolerance)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0001, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const double       anInputArea = surfaceArea(aShell);
  const TopoDS_Shell aResult     = aConnector.Build(aShell, 1.0e-3, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 7);
  EXPECT_NEAR(anInputArea, 1.9999, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), anInputArea, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), 1.9999, 1.0e-12);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeFix_FaceConnectTest, LeavesBoundariesBeyondSewingToleranceSeparate)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.01, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-4, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 8);
}

TEST(ShapeFix_FaceConnectTest, ClearDiscardsConnections)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  aConnector.Clear();
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_TRUE(aResult.IsSame(aShell));
  EXPECT_EQ(countUniqueEdges(aResult), 8);
}

TEST(ShapeFix_FaceConnectTest, CanBeReusedAfterClear)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  aConnector.Clear();
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 7);
}

TEST(ShapeFix_FaceConnectTest, EmptyShellIsReturnedUnchanged)
{
  BRep_Builder aBuilder;
  TopoDS_Shell anEmptyShell;
  aBuilder.MakeShell(anEmptyShell);

  ShapeFix_FaceConnect aConnector;
  const TopoDS_Shell   aResult = aConnector.Build(anEmptyShell, 1.0e-6, 1.0e-7);

  EXPECT_TRUE(aResult.IsSame(anEmptyShell));
  EXPECT_EQ(countUniqueEdges(aResult), 0);
}

TEST(ShapeFix_FaceConnectTest, SelfConnectionPreservesSingleFaceTopology)
{
  const TopoDS_Face aFace = makeRectangle(0.0, 1.0);
  BRep_Builder      aBuilder;
  TopoDS_Shell      aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, aFace);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace, aFace));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 4);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeFix_FaceConnectTest, ConnectsReversedFaces)
{
  TopoDS_Face aFace1 = makeRectangle(0.0, 1.0);
  TopoDS_Face aFace2 = makeRectangle(1.0, 2.0);
  aFace1.Reverse();
  aFace2.Reverse();
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 7);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeFix_FaceConnectTest, ConnectsThreeFaceChain)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Face  aFace3 = makeRectangle(2.0, 3.0);
  const TopoDS_Shell aShell = makeShell({aFace1, aFace2, aFace3});

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  ASSERT_TRUE(aConnector.Add(aFace2, aFace3));
  const double       anInputArea = surfaceArea(aShell);
  const TopoDS_Shell aResult     = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 10);
  EXPECT_NEAR(anInputArea, 3.0, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), anInputArea, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), 3.0, 1.0e-12);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeFix_FaceConnectTest, LeavesUnregisteredThirdFaceSeparate)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Face  aFace3 = makeRectangle(2.0, 3.0);
  const TopoDS_Shell aShell = makeShell({aFace1, aFace2, aFace3});

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 11);
}

TEST(ShapeFix_FaceConnectTest, ConnectsTwoIndependentFacePairs)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Face  aFace3 = makeRectangle(10.0, 11.0);
  const TopoDS_Face  aFace4 = makeRectangle(11.0, 12.0);
  const TopoDS_Shell aShell = makeShell({aFace1, aFace2, aFace3, aFace4});

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  ASSERT_TRUE(aConnector.Add(aFace3, aFace4));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 14);
  // A shell with disconnected components is invalid by construction; sewing
  // the two requested pairs must not hide that input condition.
  EXPECT_FALSE(BRepCheck_Analyzer(aShell).IsValid());
  EXPECT_FALSE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeFix_FaceConnectTest, DuplicateConnectionDoesNotChangeResult)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  ASSERT_TRUE(aConnector.Add(aFace2, aFace1));
  const TopoDS_Shell aResult = aConnector.Build(aShell, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult), 7);
}

TEST(ShapeFix_FaceConnectTest, RebuildingResultIsStable)
{
  const TopoDS_Face  aFace1 = makeRectangle(0.0, 1.0);
  const TopoDS_Face  aFace2 = makeRectangle(1.0, 2.0);
  const TopoDS_Shell aShell = makeShell(aFace1, aFace2);

  ShapeFix_FaceConnect aConnector;
  ASSERT_TRUE(aConnector.Add(aFace1, aFace2));
  const TopoDS_Shell aResult1 = aConnector.Build(aShell, 1.0e-6, 1.0e-7);
  const TopoDS_Shell aResult2 = aConnector.Build(aResult1, 1.0e-6, 1.0e-7);

  EXPECT_EQ(countUniqueEdges(aResult1), 7);
  EXPECT_EQ(countUniqueEdges(aResult2), 7);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult2).IsValid());
}

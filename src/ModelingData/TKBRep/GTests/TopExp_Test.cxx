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

#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <gtest/gtest.h>

TEST(TopExp_Test, MapShapes_Faces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceMap;
  TopExp::MapShapes(aBox, TopAbs_FACE, aFaceMap);

  EXPECT_EQ(aFaceMap.Extent(), 6) << "A box should have exactly 6 faces";
}

TEST(TopExp_Test, MapShapes_Edges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgeMap;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdgeMap);

  EXPECT_EQ(anEdgeMap.Extent(), 12) << "A box should have exactly 12 edges";
}

TEST(TopExp_Test, MapShapes_Vertices)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVertexMap;
  TopExp::MapShapes(aBox, TopAbs_VERTEX, aVertexMap);

  EXPECT_EQ(aVertexMap.Extent(), 8) << "A box should have exactly 8 vertices";
}

TEST(TopExp_Test, FirstVertex_LastVertex)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  TopoDS_Vertex aFirstVertex = TopExp::FirstVertex(anEdge);
  TopoDS_Vertex aLastVertex  = TopExp::LastVertex(anEdge);

  EXPECT_FALSE(aFirstVertex.IsNull()) << "First vertex should not be null";
  EXPECT_FALSE(aLastVertex.IsNull()) << "Last vertex should not be null";
  EXPECT_FALSE(aFirstVertex.IsSame(aLastVertex))
    << "First and last vertices of a box edge should be distinct";
}

TEST(TopExp_Test, Vertices)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());

  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(anEdge, aV1, aV2);

  EXPECT_FALSE(aV1.IsNull()) << "First vertex from Vertices() should not be null";
  EXPECT_FALSE(aV2.IsNull()) << "Second vertex from Vertices() should not be null";
}

TEST(TopExp_Test, CommonVertex)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgeMap;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdgeMap);
  ASSERT_GE(anEdgeMap.Extent(), 2);

  // Find two adjacent edges (sharing a common vertex)
  bool isFound = false;
  for (int i = 1; i <= anEdgeMap.Extent() && !isFound; ++i)
  {
    const TopoDS_Edge& anEdge1 = TopoDS::Edge(anEdgeMap(i));
    for (int j = i + 1; j <= anEdgeMap.Extent() && !isFound; ++j)
    {
      const TopoDS_Edge& anEdge2 = TopoDS::Edge(anEdgeMap(j));
      TopoDS_Vertex      aCommonVertex;
      if (TopExp::CommonVertex(anEdge1, anEdge2, aCommonVertex))
      {
        EXPECT_FALSE(aCommonVertex.IsNull()) << "Common vertex should not be null";
        isFound = true;
      }
    }
  }

  EXPECT_TRUE(isFound) << "Should find at least one pair of adjacent edges in a box";
}

TEST(TopExp_Test, MapShapesAndAncestors)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);

  EXPECT_EQ(anEdgeFaceMap.Extent(), 12) << "A box should have 12 edges in the ancestor map";

  // Each edge of a box is shared by exactly 2 faces
  for (int i = 1; i <= anEdgeFaceMap.Extent(); ++i)
  {
    const NCollection_List<TopoDS_Shape>& aFaceList = anEdgeFaceMap(i);
    EXPECT_EQ(aFaceList.Extent(), 2) << "Each box edge should be shared by exactly 2 faces";
  }
}

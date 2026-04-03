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

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <gp_Ax2.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepFilletAPI_MakeChamferTest, SymmetricChamfer)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeChamfer aChamfer(aBox);
  aChamfer.Add(2.0, anEdge);
  const TopoDS_Shape& aResult = aChamfer.Shape();
  ASSERT_TRUE(aChamfer.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFilletAPI_MakeChamferTest, AsymmetricChamfer)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
  const TopoDS_Face& aFace  = TopoDS::Face(anEdgeFaceMap.FindFromKey(anEdge).First());

  BRepFilletAPI_MakeChamfer aChamfer(aBox);
  aChamfer.Add(1.0, 3.0, anEdge, aFace);
  const TopoDS_Shape& aResult = aChamfer.Shape();
  ASSERT_TRUE(aChamfer.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFilletAPI_MakeChamferTest, ChamferMoreFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeChamfer aChamfer(aBox);
  aChamfer.Add(2.0, anEdge);
  const TopoDS_Shape& aResult = aChamfer.Shape();
  ASSERT_TRUE(aChamfer.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aResult, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_GT(aFaceCount, 6);
}

// Regression test for issue #1163: chamfer after boolean fusion must not crash
// due to null shapes in IntersectMoreCorner when topology maps carry stale data.
// Edges at vertices shared by 3+ faces are selected to exercise the complex
// corner-processing paths (IntersectMoreCorner / PerformOneCorner).
TEST(BRepFilletAPI_MakeChamferTest, ChamferAfterBooleanFusion)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  gp_Ax2                   anAxis(gp_Pnt(5.0, 0.0, 5.0), gp_Dir(0.0, 1.0, 0.0));
  BRepPrimAPI_MakeCylinder aCylMaker(anAxis, 3.0, 10.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_TRUE(aCylMaker.IsDone());

  BRepAlgoAPI_Fuse aFuse(aBox, aCyl);
  ASSERT_TRUE(aFuse.IsDone());
  const TopoDS_Shape& aFused = aFuse.Shape();

  // Build vertex-to-face count map to identify complex vertices (3+ faces).
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aVtxFaceMap;
  TopExp::MapShapesAndAncestors(aFused, TopAbs_VERTEX, TopAbs_FACE, aVtxFaceMap);

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(aFused, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);

  // Select edges at vertices with 3+ adjacent faces (complex topology).
  // The box-cylinder fusion geometry above is deterministic and always produces
  // such edges at the intersection seam, so the assertion below is reliable.
  BRepFilletAPI_MakeChamfer aChamfer(aFused);
  int                       anEdgeCount = 0;
  for (TopExp_Explorer anEdgeExp(aFused, TopAbs_EDGE); anEdgeExp.More() && anEdgeCount < 4;
       anEdgeExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
    if (!anEdgeFaceMap.Contains(anEdge) || anEdgeFaceMap.FindFromKey(anEdge).IsEmpty())
      continue;

    // Check if any vertex of this edge has 3+ faces.
    bool hasComplexVertex = false;
    for (TopExp_Explorer aVtxExp(anEdge, TopAbs_VERTEX); aVtxExp.More(); aVtxExp.Next())
    {
      if (aVtxFaceMap.Contains(aVtxExp.Current())
          && aVtxFaceMap.FindFromKey(aVtxExp.Current()).Size() >= 3)
      {
        hasComplexVertex = true;
        break;
      }
    }
    if (!hasComplexVertex)
      continue;

    const TopoDS_Face& aFace = TopoDS::Face(anEdgeFaceMap.FindFromKey(anEdge).First());
    aChamfer.Add(0.5, 0.5, anEdge, aFace);
    anEdgeCount++;
  }
  ASSERT_GT(anEdgeCount, 0);

  // Must not crash; may succeed or fail gracefully with IsDone() == false.
  try
  {
    aChamfer.Build();
    if (aChamfer.IsDone())
    {
      BRepCheck_Analyzer anAnalyzer(aChamfer.Shape());
      EXPECT_TRUE(anAnalyzer.IsValid());
    }
  }
  catch (const Standard_Failure&)
  {
    // Exception instead of crash is acceptable.
  }
}

// Regression test for issue #1163: sequential chamfers on the same shape
// must not crash due to stale TShape pointers in internal topology maps.
// Each iteration selects a different edge (by index) and verifies the shape
// topology changes, ensuring that successive operations actually modify the
// topology maps that trigger the bug.
TEST(BRepFilletAPI_MakeChamferTest, SequentialChamferNoCrash)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopoDS_Shape aShape        = aBox;
  int          aPrevEdges    = 0;
  int          aSuccessCount = 0;

  for (int i = 0; i < 3; ++i)
  {
    // Count edges and pick a different one each iteration.
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgeMap;
    TopExp::MapShapes(aShape, TopAbs_EDGE, anEdgeMap);
    if (anEdgeMap.IsEmpty())
      break;

    // Verify topology actually changed after each successful chamfer.
    if (i > 0 && aPrevEdges > 0)
    {
      EXPECT_NE(anEdgeMap.Extent(), aPrevEdges);
    }
    aPrevEdges = anEdgeMap.Extent();

    // Select edge at different position each iteration to avoid re-chamfering the same edge.
    int                anIdx  = (i * 3 + 1) % anEdgeMap.Extent() + 1;
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeMap(anIdx));

    BRepFilletAPI_MakeChamfer aChamfer(aShape);
    aChamfer.Add(1.0, anEdge);

    try
    {
      aChamfer.Build();
      if (aChamfer.IsDone())
      {
        aShape = aChamfer.Shape();
        aSuccessCount++;
      }
      else
      {
        break;
      }
    }
    catch (const Standard_ConstructionError&)
    {
      // Null-guard from Fix 4 (Arcprolbis) - acceptable graceful failure.
      break;
    }
    catch (const StdFail_NotDone&)
    {
      // Null-guard from Fix 3 (Arcprol) or existing Fv check - acceptable.
      break;
    }
  }
  // Verify at least one chamfer iteration succeeded, ensuring the test
  // meaningfully exercises topology changes rather than trivially passing.
  EXPECT_GE(aSuccessCount, 1);
}

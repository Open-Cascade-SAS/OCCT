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
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepGProp.hxx>
#include <ChFiDS_ChamfMode.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRep_Tool.hxx>
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
#include <TopoDS_Vertex.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>

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

// Chamfer every edge of a flat 50x50x10 slab with distance 5. The top and
// bottom chamfers of each 50x10 side face (5 + 5) exactly consume the 10 mm
// height, so the intervening side faces must be removed and the opposing
// chamfers must meet cleanly, leaving no degenerate mid edges (issue #1177).
TEST(BRepFilletAPI_MakeChamferTest, Issue1177_ChamferAllEdgesFlatBox_SucceedsWithoutCrash)
{
  BRepPrimAPI_MakeBox aBoxMaker(50.0, 50.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  BRepFilletAPI_MakeChamfer aChamfer(aBox);
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    aChamfer.Add(5.0, TopoDS::Edge(anExp.Current()));
  }

  ASSERT_NO_THROW(aChamfer.Build()) << "Chamfer build must not crash";

  EXPECT_TRUE(aChamfer.IsDone())
    << "Chamfering all edges of a 50x50x10 slab with d=5 should succeed (issue #1177)";

  if (!aChamfer.IsDone())
  {
    return;
  }

  const TopoDS_Shape& aResult = aChamfer.Shape();
  ASSERT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer anAnalyzer(aResult, true, false, true);
  EXPECT_TRUE(anAnalyzer.IsValid());

  double aMaxTolerance = 0.0;
  for (TopExp_Explorer aVertexExp(aResult, TopAbs_VERTEX); aVertexExp.More(); aVertexExp.Next())
  {
    aMaxTolerance =
      std::max(aMaxTolerance, BRep_Tool::Tolerance(TopoDS::Vertex(aVertexExp.Current())));
  }
  for (TopExp_Explorer anEdgeExp(aResult, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    aMaxTolerance =
      std::max(aMaxTolerance, BRep_Tool::Tolerance(TopoDS::Edge(anEdgeExp.Current())));
  }
  EXPECT_LE(aMaxTolerance, 1.01e-4)
    << "Chamfer construction must not hide inconsistent edge parameterization with tolerance";
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
    {
      continue;
    }

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
    {
      continue;
    }

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
    {
      break;
    }

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

TEST(BRepFilletAPI_MakeChamferTest, PenetrationChamferOnClosedSplineEdge)
{
  double aReferenceVolume = 0.0;
  for (double aRadius : {10.0, std::nextafter(10.0, 0.0)})
  {
    SCOPED_TRACE(aRadius);
    const double aScale    = aRadius / 10.0;
    TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(aRadius, 50.0 * aScale).Shape();
    gp_Trsf      aTransform;
    aTransform.SetTranslation(gp_Vec(1.e-10, 0.0, 0.0));
    aCylinder.Move(TopLoc_Location(aTransform));
    const TopoDS_Shape aBox =
      BRepPrimAPI_MakeBox(gp_Pnt(-20.0 * aScale, -20.0 * aScale, -20.0 * aScale),
                          40.0 * aScale,
                          40.0 * aScale,
                          40.0 * aScale)
        .Shape();
    BRepAlgoAPI_Fuse aFuse(aCylinder, aBox);
    ASSERT_TRUE(aFuse.IsDone());
    BRepBuilderAPI_NurbsConvert aConversion(aFuse.Shape());
    ASSERT_TRUE(aConversion.IsDone());
    const TopoDS_Shape& aShape = aConversion.Shape();
    TopoDS_Face         aFace;
    TopoDS_Edge         anEdge;
    for (TopExp_Explorer aFaceIt(aShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
    {
      const TopoDS_Face& aCandidateFace = TopoDS::Face(aFaceIt.Current());
      if (!BRepAdaptor_Surface(aCandidateFace).IsUPeriodic())
      {
        continue;
      }
      for (TopExp_Explorer anEdgeIt(aCandidateFace, TopAbs_EDGE); anEdgeIt.More(); anEdgeIt.Next())
      {
        const TopoDS_Edge& aCandidateEdge = TopoDS::Edge(anEdgeIt.Current());
        BRepAdaptor_Curve  aCurve(aCandidateEdge);
        if (aCurve.IsClosed()
            && std::abs(aCurve.Value(aCurve.FirstParameter()).Z() - 20.0 * aScale)
                 <= Precision::Confusion())
        {
          aFace  = aCandidateFace;
          anEdge = aCandidateEdge;
        }
      }
    }
    ASSERT_FALSE(anEdge.IsNull());
    BRepFilletAPI_MakeChamfer aChamfer(aShape);
    aChamfer.SetMode(ChFiDS_ConstThroatWithPenetrationChamfer);
    ASSERT_NO_THROW(aChamfer.Add(aScale, 2.0 * aScale, anEdge, aFace));
    aChamfer.Build();
    ASSERT_TRUE(aChamfer.IsDone());
    const TopoDS_Shape& aResult = aChamfer.Shape();
    EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces, aSolids;
    TopExp::MapShapes(aResult, TopAbs_FACE, aFaces);
    TopExp::MapShapes(aResult, TopAbs_SOLID, aSolids);
    EXPECT_EQ(aFaces.Extent(), 9);
    EXPECT_EQ(aSolids.Extent(), 1);
    GProp_GProps aProperties;
    BRepGProp::VolumeProperties(aResult, aProperties, 1.e-9);
    if (aRadius == 10.0)
    {
      aReferenceVolume = aProperties.Mass();
    }
    EXPECT_NEAR(aProperties.Mass(), aReferenceVolume, aReferenceVolume * 1.e-7);
    for (TopExp_Explorer anEdgeIt(aResult, TopAbs_EDGE); anEdgeIt.More(); anEdgeIt.Next())
    {
      EXPECT_LE(BRep_Tool::Tolerance(TopoDS::Edge(anEdgeIt.Current())), 1.e-4);
    }
    for (TopExp_Explorer aVertexIt(aResult, TopAbs_VERTEX); aVertexIt.More(); aVertexIt.Next())
    {
      EXPECT_LE(BRep_Tool::Tolerance(TopoDS::Vertex(aVertexIt.Current())), 1.e-4);
    }
  }
}

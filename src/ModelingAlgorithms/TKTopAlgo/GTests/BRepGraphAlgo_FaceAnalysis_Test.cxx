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
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphAlgo_FaceAnalysis.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Circle.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

TEST(BRepGraphAlgo_FaceAnalysisTest, NormalFace_Unaffected)
{
  // A normal box face should not be altered.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbEdgesBefore = aGraph.Defs().NbEdges();
  const int aNbFacesBefore = aGraph.Defs().NbFaces();

  BRepGraphAlgo_FaceAnalysis::Options aOpts;
  aOpts.MinTolerance = 1.0e-8;
  BRepGraphAlgo_FaceAnalysis::Result aResult = BRepGraphAlgo_FaceAnalysis::Perform(aGraph, aOpts);

  EXPECT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSmallEdges, 0);
  EXPECT_EQ(aResult.NbDeletedFaces, 0);
  EXPECT_EQ(aResult.DegeneratedEdges.Length(), 0);
  EXPECT_EQ(aResult.DeletedFaces.Length(), 0);

  // Edge and face counts unchanged.
  EXPECT_EQ(aGraph.Defs().NbEdges(), aNbEdgesBefore);
  EXPECT_EQ(aGraph.Defs().NbFaces(), aNbFacesBefore);
}

TEST(BRepGraphAlgo_FaceAnalysisTest, SmallEdge_Detection)
{
  // Create a face with one small edge (shorter than MinTolerance but above Precision::Confusion).
  // Quadrilateral with a tiny bottom edge (1e-5 units long).
  const gp_Pnt aP1(0, 0, 0);
  const gp_Pnt aP2(10, 0, 0);
  const gp_Pnt aP3(10, 10, 0);
  const gp_Pnt aP4(0, 1.0e-5, 0); // tiny edge aP1->aP4

  BRepBuilderAPI_MakeWire aMW;
  aMW.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP3, aP4).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());
  ASSERT_TRUE(aMW.IsDone());

  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  ASSERT_FALSE(aFace.IsNull());

  BRepGraph aGraph;
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_FaceAnalysis::Options aOpts;
  aOpts.MinTolerance = 1.0e-2; // large enough to catch the 1e-5 edge
  BRepGraphAlgo_FaceAnalysis::Result aResult = BRepGraphAlgo_FaceAnalysis::Perform(aGraph, aOpts);

  EXPECT_TRUE(aResult.IsDone);
  // The tiny edge aP4->aP1 should be detected as small.
  EXPECT_GE(aResult.NbSmallEdges, 1);
  EXPECT_GE(aResult.DegeneratedEdges.Length(), 1);
}

TEST(BRepGraphAlgo_FaceAnalysisTest, SmallFace_Removal)
{
  // Create a face where ALL edges are tiny — face should be removed.
  // Use edges small enough to be detected but above Precision::Confusion.
  const double aTiny = 1.0e-5;
  const gp_Pnt aP1(0, 0, 0);
  const gp_Pnt aP2(aTiny, 0, 0);
  const gp_Pnt aP3(aTiny, aTiny, 0);
  const gp_Pnt aP4(0, aTiny, 0);

  BRepBuilderAPI_MakeWire aMW;
  aMW.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP3, aP4).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());
  ASSERT_TRUE(aMW.IsDone());

  // Also add a normal face so we have a compound.
  TopoDS_Face aTinyFace = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  ASSERT_FALSE(aTinyFace.IsNull());

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  TopoDS_Face aNormalFace;
  for (TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aNormalFace = TopoDS::Face(anExp.Current());
    break;
  }

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aTinyFace);
  aBB.Add(aCompound, aNormalFace);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_FaceAnalysis::Options aOpts;
  aOpts.MinTolerance = 1.0e-2; // large enough to catch 1e-5 edges
  BRepGraphAlgo_FaceAnalysis::Result aResult = BRepGraphAlgo_FaceAnalysis::Perform(aGraph, aOpts);

  EXPECT_TRUE(aResult.IsDone);
  // The tiny face should be removed.
  EXPECT_GE(aResult.NbDeletedFaces, 1);
  EXPECT_GE(aResult.DeletedFaces.Length(), 1);
}

TEST(BRepGraphAlgo_FaceAnalysisTest, VertexGluing_AveragedCoordinates)
{
  // Create a face with a small edge; verify vertices are merged.
  const gp_Pnt aP1(0, 0, 0);
  const gp_Pnt aP2(10, 0, 0);
  const gp_Pnt aP3(10, 10, 0);
  const gp_Pnt aP4(0, 1.0e-5, 0); // small edge aP4->aP1

  BRepBuilderAPI_MakeWire aMW;
  aMW.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP3, aP4).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());
  ASSERT_TRUE(aMW.IsDone());

  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aMW.Wire(), true);
  ASSERT_FALSE(aFace.IsNull());

  BRepGraph aGraph;
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_FaceAnalysis::Options aOpts;
  aOpts.MinTolerance = 1.0e-2;
  BRepGraphAlgo_FaceAnalysis::Result aResult = BRepGraphAlgo_FaceAnalysis::Perform(aGraph, aOpts);

  EXPECT_TRUE(aResult.IsDone);
  EXPECT_GE(aResult.NbSmallEdges, 1);

  // After merging, the small edge's start and end vertices should point to the same index.
  for (int i = 0; i < aResult.DegeneratedEdges.Length(); ++i)
  {
    const int anEdgeIdx = aResult.DegeneratedEdges.Value(i).Index;
    const auto& anEdge = aGraph.Defs().Edge(anEdgeIdx);
    EXPECT_TRUE(anEdge.IsDegenerate);
    // Vertices merged: start == end.
    EXPECT_EQ(anEdge.StartVertexIdx, anEdge.EndVertexIdx);
  }
}

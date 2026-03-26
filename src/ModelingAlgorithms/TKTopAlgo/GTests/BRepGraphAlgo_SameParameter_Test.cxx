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
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_SameParameter.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

namespace
{

//! Extract faces from a box shape using BRepBuilderAPI_Copy with copyGeom=true.
NCollection_Sequence<TopoDS_Face> extractCopiedFaces(const TopoDS_Shape& theBox)
{
  NCollection_Sequence<TopoDS_Face> aFaces;
  for (TopExp_Explorer anExp(theBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), /*copyGeom=*/true);
    aFaces.Append(TopoDS::Face(aCopier.Shape()));
  }
  return aFaces;
}

} // namespace

TEST(BRepGraphAlgo_SameParameterTest, Enforce_BoxEdge_SetsSameParameter)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    aGraph.Builder().MutEdge(BRepGraph_EdgeId(anEdgeIdx))->SameParameter = false;
  }

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anEdgeIdx);
    const bool             isOk = BRepGraphAlgo_SameParameter::Enforce(aGraph, anEdgeId, 1.0e-04);
    EXPECT_TRUE(isOk) << "Edge " << anEdgeIdx << " failed SameParameter enforcement";
    EXPECT_TRUE(BRepGraph_Tool::Edge::SameParameter(aGraph, anEdgeId));
  }
}

TEST(BRepGraphAlgo_SameParameterTest, Enforce_CylinderEdge_ToleranceReasonable)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    aGraph.Builder().MutEdge(BRepGraph_EdgeId(anEdgeIdx))->SameParameter = false;
  }

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anEdgeIdx);
    (void)BRepGraphAlgo_SameParameter::Enforce(aGraph, anEdgeId, 1.0e-04);
    EXPECT_LT(BRepGraph_Tool::Edge::Tolerance(aGraph, anEdgeId), 1.0)
      << "Edge " << anEdgeIdx << " tolerance is unreasonably large";
  }
}

TEST(BRepGraphAlgo_SameParameterTest, Perform_BatchSetsFlags)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    aGraph.Builder().MutEdge(BRepGraph_EdgeId(anEdgeIdx))->SameParameter = false;
  }

  NCollection_IndexedMap<BRepGraph_EdgeId> anEdgeIndices;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    anEdgeIndices.Add(BRepGraph_EdgeId(anEdgeIdx));
  }

  const double aTolerance    = 1.0e-04;
  const double aMaxTolerance = 1.0;
  (void)BRepGraphAlgo_SameParameter::Perform(aGraph, anEdgeIndices, aTolerance);

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anEdgeIdx);
    EXPECT_TRUE(BRepGraph_Tool::Edge::SameParameter(aGraph, anEdgeId))
      << "Edge " << anEdgeIdx << " SameParameter not set";
    EXPECT_LT(BRepGraph_Tool::Edge::Tolerance(aGraph, anEdgeId), aMaxTolerance)
      << "Edge " << anEdgeIdx << " tolerance is unreasonably large";
  }
}

TEST(BRepGraphAlgo_SameParameterTest, Enforce_NoCurve3d_SetsFlag)
{
  BRepPrimAPI_MakeSphere aSphereMaker(10.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anEdgeIdx))
        || !BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      const BRepGraph_EdgeId anEdgeId(anEdgeIdx);
      aGraph.Builder().MutEdge(anEdgeId)->SameParameter = false;
      const bool isOk = BRepGraphAlgo_SameParameter::Enforce(aGraph, anEdgeId, 1.0e-04);
      EXPECT_TRUE(isOk);
      EXPECT_TRUE(BRepGraph_Tool::Edge::SameParameter(aGraph, anEdgeId));
      break;
    }
  }
}

TEST(BRepGraphAlgo_SameParameterTest, Enforce_AfterSewing_SewnEdgesAreValid)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  NCollection_Sequence<TopoDS_Face> aFaces = extractCopiedFaces(aBox);
  ASSERT_EQ(aFaces.Length(), 6);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int aFaceIdx = 1; aFaceIdx <= aFaces.Length(); ++aFaceIdx)
  {
    aBB.Add(aCompound, aFaces.Value(aFaceIdx));
  }

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.NbSewnEdges, 12);

  int aNbSameParam = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    if (!aCoEdgeIdxs.IsEmpty()
        && BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      if (BRepGraph_Tool::Edge::SameParameter(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
      {
        ++aNbSameParam;
      }
    }
  }
  EXPECT_GT(aNbSameParam, 0);
}

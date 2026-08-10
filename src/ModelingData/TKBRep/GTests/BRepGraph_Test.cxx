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

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_Iterator.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Face.hxx>

#include <atomic>
#include <type_traits>
#include <tuple>

#include <gtest/gtest.h>

namespace
{
static_assert(!std::is_convertible_v<BRepGraph_FaceId, BRepGraph_EdgeId>);
static_assert(!std::is_constructible_v<BRepGraph_FaceId, BRepGraph_EdgeId>);
static_assert(std::is_convertible_v<BRepGraph_FaceId, BRepGraph_NodeId>);
static_assert(std::is_constructible_v<BRepGraph_ParentExplorer,
                                      const BRepGraph&,
                                      BRepGraph_FaceId,
                                      BRepGraph_NodeId::Kind>);
static_assert(std::is_constructible_v<BRepGraph_ChildExplorer,
                                      const BRepGraph&,
                                      BRepGraph_FaceId,
                                      BRepGraph_NodeId::Kind>);
static_assert(std::is_constructible_v<BRepGraph_ChildExplorer,
                                      const BRepGraph&,
                                      BRepGraph_ProductId,
                                      BRepGraph_NodeId::Kind>);
static_assert(!std::is_convertible_v<BRepGraph_FaceSurfaceRepId, BRepGraph_EdgeCurve3DRepId>);
static_assert(!std::is_constructible_v<BRepGraph_FaceSurfaceRepId, BRepGraph_EdgeCurve3DRepId>);
static_assert(std::is_convertible_v<BRepGraph_FaceSurfaceRepId, BRepGraph_RepId>);

template <typename ContainerT, typename IdT>
bool containsId(ContainerT theIds, const IdT theId)
{
  for (const IdT& anId : theIds)
  {
    if (anId == theId)
    {
      return true;
    }
  }
  return false;
}

static int componentKey(const BRepGraph_NodeId theNode)
{
  return theNode.Index * BRepGraph_NodeId::THE_KIND_COUNT + static_cast<int>(theNode.NodeKind);
}

static NCollection_DynamicArray<BRepGraph_EdgeId> collectFreeEdges(const BRepGraph& theGraph)
{
  NCollection_DynamicArray<BRepGraph_EdgeId> aResult(16);
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId))
    {
      continue;
    }

    NCollection_Map<int> aOwningFaces;
    for (BRepGraph_ParentExplorer aFaceExp(theGraph, anEdgeId, BRepGraph_NodeId::Kind::Face);
         aFaceExp.More();
         aFaceExp.Next())
    {
      aOwningFaces.Add(aFaceExp.Current().DefId.Index);
      if (aOwningFaces.Extent() > 1)
      {
        break;
      }
    }

    if (aOwningFaces.Extent() == 1)
    {
      aResult.Append(anEdgeId);
    }
  }
  return aResult;
}

static BRepGraph_EdgeId addCompatibleReplacementEdge(BRepGraph&             theGraph,
                                                     const BRepGraph_EdgeId theOldEdge,
                                                     const bool             theReversed)
{
  const BRepGraph_VertexRefId aStartRef = BRepGraph_Tool::Edge::StartVertexId(theGraph, theOldEdge);
  const BRepGraph_VertexRefId anEndRef  = BRepGraph_Tool::Edge::EndVertexId(theGraph, theOldEdge);
  const BRepGraph_VertexId aStartVertex = theGraph.Refs().Vertices().Entry(aStartRef).ChildVertexId;
  const BRepGraph_VertexId anEndVertex  = theGraph.Refs().Vertices().Entry(anEndRef).ChildVertexId;
  const BRepGraph_VertexId aNewStart    = theReversed ? anEndVertex : aStartVertex;
  const BRepGraph_VertexId aNewEnd      = theReversed ? aStartVertex : anEndVertex;
  const gp_Pnt             aP0          = BRepGraph_Tool::Vertex::Pnt(theGraph, aNewStart);
  const gp_Pnt             aP1          = BRepGraph_Tool::Vertex::Pnt(theGraph, aNewEnd);
  const double             aLength      = aP0.Distance(aP1);
  occ::handle<Geom_Curve>  aCurve       = new Geom_Line(gp_Lin(aP0, gp_Dir(gp_Vec(aP0, aP1))));
  return theGraph.Editor().Edges().Add(aNewStart, aNewEnd, aCurve, 0.0, aLength, 1.0e-7);
}

static BRepGraph_NodeId componentRootOfFace(const BRepGraph&       theGraph,
                                            const BRepGraph_FaceId theFaceId)
{
  for (BRepGraph_ParentExplorer aSolidExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Solid);
       aSolidExp.More();
       aSolidExp.Next())
  {
    return aSolidExp.Current().DefId;
  }

  for (BRepGraph_ParentExplorer aShellExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Shell);
       aShellExp.More();
       aShellExp.Next())
  {
    return aShellExp.Current().DefId;
  }

  return theFaceId;
}

static int countFaceComponents(const BRepGraph& theGraph)
{
  NCollection_Map<int> aRoots;
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    aRoots.Add(componentKey(componentRootOfFace(theGraph, aFaceIt.CurrentId())));
  }
  return aRoots.Extent();
}

static uint32_t countSameDomainFaces(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  const occ::handle<Geom_Surface>& aSurface = BRepGraph_Tool::Face::Surface(theGraph, theFace);
  if (aSurface.IsNull())
  {
    return 0;
  }

  uint32_t aCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId anOtherFace = aFaceIt.CurrentId();
    if (anOtherFace != theFace && BRepGraph_Tool::Face::Surface(theGraph, anOtherFace) == aSurface)
    {
      ++aCount;
    }
  }
  return aCount;
}

class BRepGraphTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = myGraph.Shapes().Add(aBox);
  }

  BRepGraph myGraph;
};

} // namespace

TEST_F(BRepGraphTest, Build_SimpleBox_IsNotEmpty)
{
  EXPECT_FALSE(myGraph.IsEmpty());
}

TEST_F(BRepGraphTest, Build_SimpleBox_CorrectCounts)
{
  EXPECT_EQ(myGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(myGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(myGraph.Topo().Wires().Nb(), 6);
  EXPECT_EQ(myGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), 8);
}

TEST_F(BRepGraphTest, Build_SimpleBox_SurfaceCount)
{
  EXPECT_EQ(myGraph.Topo().Faces().Nb(), 6);
}

TEST_F(BRepGraphTest, Face_Surface_IsValid)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface rep";
  }
}

TEST_F(BRepGraphTest, Edge_CurveAndVertices_AreValid)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (!BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId))
        << "Edge " << anEdgeId.Index << " has no Curve3D rep";
    }
    EXPECT_TRUE(BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdgeId).IsValid())
      << "Edge " << anEdgeId.Index << " has invalid StartVertexId";
    EXPECT_TRUE(BRepGraph_Tool::Edge::EndVertexId(myGraph, anEdgeId).IsValid())
      << "Edge " << anEdgeId.Index << " has invalid EndVertexId";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWire_Exists)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    EXPECT_TRUE(anOuterWire.IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has no outer wire";
  }
}

TEST_F(BRepGraphTest, FaceDef_HasValidSurface)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface rep";
  }
}

TEST_F(BRepGraphTest, FindPCurveCoEdgeId_ValidPair)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    if (!anOuterWire.IsValid())
    {
      continue;
    }
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      BRepGraph_TestTools::CoEdgesOfWire(myGraph, anOuterWire);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(aCoEdge.ChildEdgeId)))
      {
        continue;
      }
      const BRepGraph_CoEdgeId aPCurveId =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph,
                                                 BRepGraph_EdgeId(aCoEdge.ChildEdgeId),
                                                 aFaceIt.CurrentId());
      EXPECT_TRUE(aPCurveId.IsValid()) << "Missing PCurve for edge " << aCoEdge.ChildEdgeId.Index
                                       << " on face " << aFaceIt.CurrentId().Index;
    }
  }
}

TEST_F(BRepGraphTest, UID_Unique)
{
  NCollection_FlatMap<BRepGraph_UID> aUIDSet;
  for (BRepGraph_SolidIterator aSolidIt(myGraph); aSolidIt.More(); aSolidIt.Next())
  {
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aSolidIt.CurrentId()))));
  }
  for (BRepGraph_ShellIterator aShellIt(myGraph); aShellIt.More(); aShellIt.Next())
  {
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aShellIt.CurrentId()))));
  }
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aFaceIt.CurrentId()))));
  }
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aWireIt.CurrentId()))));
  }
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(anEdgeIt.CurrentId()))));
  }
  for (BRepGraph_VertexIterator aVertexIt(myGraph); aVertexIt.More(); aVertexIt.Next())
  {
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aVertexIt.CurrentId()))));
  }
  // Surface/Curve geometry UIDs are no longer separate nodes; geometry is stored inline on defs.
}

TEST_F(BRepGraphTest, UID_NodeIdRoundTrip)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId     aFaceId   = BRepGraph_NodeId(aFaceIt.CurrentId());
    const BRepGraph_UID& aUID      = myGraph.UIDs().Of(aFaceId);
    BRepGraph_NodeId     aResolved = myGraph.UIDs().NodeIdFrom(aUID);
    EXPECT_EQ(aResolved, aFaceId) << "Round trip failed for face " << aFaceIt.CurrentId().Index;
  }
}

TEST_F(BRepGraphTest, SameDomainFaces_Box_Empty)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_EQ(countSameDomainFaces(myGraph, aFaceId), 0)
      << "Box face " << aFaceId.Index << " should have no same-domain faces";
  }
}

TEST_F(BRepGraphTest, Decompose_TwoSeparateFaces)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  TopExp_Explorer    anExp1(aBox1.Shape(), TopAbs_FACE);
  TopExp_Explorer    anExp2(aBox2.Shape(), TopAbs_FACE);
  const TopoDS_Face& aFace1 = TopoDS::Face(anExp1.Current());
  const TopoDS_Face& aFace2 = TopoDS::Face(anExp2.Current());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFace1);
  aBuilder.Add(aCompound, aFace2);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);

  EXPECT_EQ(countFaceComponents(aGraph), 2);
}

TEST_F(BRepGraphTest, ReBuild_UIDMonotonic)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aBox);
  const uint32_t                                       aGen1      = aGraph.UIDs().Generation();

  // Access a UID from the first build to verify it works.
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  const BRepGraph_NodeId aFirstFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aFirstUID = aGraph.UIDs().Of(aFirstFace);
  EXPECT_TRUE(aFirstUID.IsValid());

  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aBox);
  const uint32_t                                       aGen2      = aGraph.UIDs().Generation();

  EXPECT_GT(aGen2, aGen1);

  // Verify all face UIDs in second build are valid and have new generation.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId aNodeId(aFaceIt.CurrentId());
    BRepGraph_UID    aUID = aGraph.UIDs().Of(aNodeId);
    EXPECT_TRUE(aUID.IsValid()) << "Face " << aFaceIt.CurrentId().Index
                                << " should have a valid UID";
  }

  // First build's UID should no longer be valid in the new generation.
  EXPECT_FALSE(aGraph.UIDs().Has(aFirstUID));
}

TEST_F(BRepGraphTest, DetectMissingPCurves_ValidBox_Empty)
{
  NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> aMissing(16);
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    for (BRepGraph_ChildExplorer anEdgeExp(myGraph, aFaceId, BRepGraph_NodeId::Kind::Edge);
         anEdgeExp.More();
         anEdgeExp.Next())
    {
      const BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::FromNodeId(anEdgeExp.Current().DefId);
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId))
      {
        continue;
      }

      if (!BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph, anEdgeId, aFaceId).IsValid())
      {
        aMissing.Append(std::make_pair(anEdgeId, aFaceId));
      }
    }
  }
  EXPECT_EQ(aMissing.Size(), 0);
}

TEST_F(BRepGraphTest, DetectDegenerateWires_ValidBox_Empty)
{
  NCollection_DynamicArray<BRepGraph_WireId> aDegenerate(16);
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId  = aWireIt.CurrentId();
    int                    aNbEdges = 0;
    for (BRepGraph_ChildExplorer anEdgeExp(myGraph, aWireId, BRepGraph_NodeId::Kind::Edge);
         anEdgeExp.More();
         anEdgeExp.Next())
    {
      ++aNbEdges;
    }

    if (aNbEdges < 2)
    {
      aDegenerate.Append(aWireId);
      continue;
    }

    bool isOuterWire = false;
    for (BRepGraph_ParentExplorer aFaceExp(myGraph, aWireId, BRepGraph_NodeId::Kind::Face);
         aFaceExp.More();
         aFaceExp.Next())
    {
      const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::FromNodeId(aFaceExp.Current().DefId);
      if (BRepGraph_Tool::Face::OuterWire(myGraph, aFaceId) == aWireId)
      {
        isOuterWire = true;
        break;
      }
    }

    if (isOuterWire && !BRepGraph_Tool::Wire::IsClosed(myGraph, aWireIt.CurrentId()))
    {
      aDegenerate.Append(aWireId);
    }
  }
  EXPECT_EQ(aDegenerate.Size(), 0);
}

TEST_F(BRepGraphTest, MutableEdge_ModifyTolerance)
{
  double anOrigTol = BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId::Start());
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
    myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
  myGraph.Editor().Edges().SetTolerance(anEdge, anOrigTol * 2.0);
  EXPECT_NEAR(BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId::Start()),
              anOrigTol * 2.0,
              1.0e-15);
}

TEST_F(BRepGraphTest, NbFacesOfEdge_SharedEdge)
{
  // In a box, each non-degenerate edge is shared by exactly 2 faces.
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeIt.CurrentId()))
    {
      const uint32_t aCount = myGraph.Topo().Edges().NbFaces(anEdgeIt.CurrentId());
      EXPECT_EQ(aCount, 2u) << "Edge " << anEdgeIt.CurrentId().Index
                            << " should be shared by 2 faces";
    }
  }
}

TEST_F(BRepGraphTest, FreeEdges_ClosedBox_Empty)
{
  NCollection_DynamicArray<BRepGraph_EdgeId> aFree = collectFreeEdges(myGraph);
  EXPECT_EQ(aFree.Size(), 0);
}

TEST_F(BRepGraphTest, RecordHistory_BasicEntry)
{
  size_t           aBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("TestOp",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aBefore + 1);
  EXPECT_TRUE(
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(aBefore).OperationName.IsEqual(
      "TestOp"));
}

TEST_F(BRepGraphTest, ReplaceEdge_Substitution)
{
  // Get the first wire and its first edge via incidence refs.
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIds.Size(), 1);
  const BRepGraphInc::CoEdgeDef& anOldCoEdge =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIds.Value(0));
  const BRepGraph_EdgeId anOldEdgeId = anOldCoEdge.ChildEdgeId;

  const BRepGraph_EdgeId aNewEdgeId = addCompatibleReplacementEdge(myGraph, anOldEdgeId, false);
  ASSERT_TRUE(aNewEdgeId.IsValid());

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  // Verify the substitution via the updated incidence refs.
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdsAfter =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIdsAfter.Size(), 1);
  const BRepGraphInc::CoEdgeDef& aNewCoEdge =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIdsAfter.Value(0));
  EXPECT_EQ(aNewCoEdge.ChildEdgeId.Index, aNewEdgeId.Index);
}

TEST_F(BRepGraphTest, ParallelBuild_SameAsSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aSeqGraph;
  aSeqGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 =
    aSeqGraph.Shapes().Add(aBox, BRepGraph::ShapesView::Options{{}, true, false, false});
  ASSERT_FALSE(aSeqGraph.IsEmpty());

  BRepGraph aParGraph;
  aParGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 =
    aParGraph.Shapes().Add(aBox, BRepGraph::ShapesView::Options{{}, true, false, true});
  ASSERT_FALSE(aParGraph.IsEmpty());

  EXPECT_EQ(aParGraph.Topo().Solids().Nb(), aSeqGraph.Topo().Solids().Nb());
  EXPECT_EQ(aParGraph.Topo().Shells().Nb(), aSeqGraph.Topo().Shells().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Wires().Nb(), aSeqGraph.Topo().Wires().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
  EXPECT_EQ(aParGraph.Topo().Vertices().Nb(), aSeqGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
}

TEST_F(BRepGraphTest, ParallelBuild_CompoundOfFaces)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (TopExp_Explorer anExp(aBox1.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aBuilder.Add(aCompound, anExp.Current());
  }
  for (TopExp_Explorer anExp(aBox2.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aBuilder.Add(aCompound, anExp.Current());
  }

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 =
    aGraph.Shapes().Add(aCompound, BRepGraph::ShapesView::Options{{}, true, false, true});
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
}

// ===================================================================
// Group 1: Shape Round-Trip
// ===================================================================

TEST_F(BRepGraphTest, ReconstructFace_EachBoxFace_SameSubShapeCounts)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId   aFaceId    = BRepGraph_NodeId(aFaceIt.CurrentId());
    const TopoDS_Shape anOrigFace = myGraph.Shapes().Original(aFaceId);
    ASSERT_FALSE(anOrigFace.IsNull());
    const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(aFaceIt.CurrentId());

    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anOrigVerts, anOrigEdges,
      anOrigWires;
    TopExp::MapShapes(anOrigFace, TopAbs_VERTEX, anOrigVerts);
    TopExp::MapShapes(anOrigFace, TopAbs_EDGE, anOrigEdges);
    TopExp::MapShapes(anOrigFace, TopAbs_WIRE, anOrigWires);

    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aReconVerts, aReconEdges,
      aReconWires;
    TopExp::MapShapes(aReconstructed, TopAbs_VERTEX, aReconVerts);
    TopExp::MapShapes(aReconstructed, TopAbs_EDGE, aReconEdges);
    TopExp::MapShapes(aReconstructed, TopAbs_WIRE, aReconWires);

    EXPECT_EQ(aReconVerts.Extent(), anOrigVerts.Extent())
      << "Vertex count mismatch for face " << aFaceIt.CurrentId().Index;
    EXPECT_EQ(aReconEdges.Extent(), anOrigEdges.Extent())
      << "Edge count mismatch for face " << aFaceIt.CurrentId().Index;
    EXPECT_EQ(aReconWires.Extent(), anOrigWires.Extent())
      << "Wire count mismatch for face " << aFaceIt.CurrentId().Index;

    // Verify same surface handle.
    const TopoDS_Face&               anOrigF = TopoDS::Face(anOrigFace);
    const TopoDS_Face&               aReconF = TopoDS::Face(aReconstructed);
    TopLoc_Location                  aLoc1, aLoc2;
    const occ::handle<Geom_Surface>& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
    const occ::handle<Geom_Surface>& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
    EXPECT_EQ(aSurf1.get(), aSurf2.get())
      << "Surface handle differs for face " << aFaceIt.CurrentId().Index;
  }
}

TEST_F(BRepGraphTest, ReconstructFace_AfterEdgeReplace_ContainsNewEdge)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIds.Size(), 1);
  const BRepGraph_EdgeId anOldEdgeId =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIds.Value(0)).ChildEdgeId;

  const BRepGraph_EdgeId aNewEdgeId = addCompatibleReplacementEdge(myGraph, anOldEdgeId, false);
  ASSERT_TRUE(aNewEdgeId.IsValid());

  // Get 3D curve handles from graph for old/new edges.
  occ::handle<Geom_Curve> aNewCurve  = BRepGraph_Tool::Edge::Curve(myGraph, aNewEdgeId);
  occ::handle<Geom_Curve> anOldCurve = BRepGraph_Tool::Edge::Curve(myGraph, anOldEdgeId);

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  // Reconstruct face 0 (the face owning wire 0).
  ASSERT_TRUE(BRepGraph_TestTools::FaceUsesWire(myGraph,
                                                BRepGraph_FaceId::Start(),
                                                BRepGraph_WireId::Start()));
  const uint32_t     aFaceIdx       = 0;
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx));

  // Check via 3D curve handle identity (reconstructed edges have new TShapes).
  bool isNewFound = false;
  bool isOldFound = false;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopLoc_Location         aLoc;
    double                  aFirst, aLast;
    occ::handle<Geom_Curve> aCurve =
      BRep_Tool::Curve(TopoDS::Edge(anExp.Current()), aLoc, aFirst, aLast);
    if (!aCurve.IsNull() && !aNewCurve.IsNull() && aCurve.get() == aNewCurve.get())
    {
      isNewFound = true;
    }
    if (!aCurve.IsNull() && !anOldCurve.IsNull() && aCurve.get() == anOldCurve.get())
    {
      isOldFound = true;
    }
  }
  EXPECT_TRUE(isNewFound) << "New edge curve not found in reconstructed face";
  EXPECT_FALSE(isOldFound) << "Old edge curve still present in reconstructed face";
}

TEST_F(BRepGraphTest, ReconstructShape_SolidRoot_SameFaceCount)
{
  BRepGraph_NodeId   aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(aSolidId);

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST_F(BRepGraphTest, ReconstructShape_FaceRoot_ReturnsSameShape)
{
  BRepGraph_NodeId   aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(aFaceId);
  const TopoDS_Shape anOriginal     = myGraph.Shapes().Original(aFaceId);
  ASSERT_FALSE(anOriginal.IsNull());

  // Reconstructed face should have the same surface handle.
  const TopoDS_Face&               anOrigF = TopoDS::Face(anOriginal);
  const TopoDS_Face&               aReconF = TopoDS::Face(aReconstructed);
  TopLoc_Location                  aLoc1, aLoc2;
  const occ::handle<Geom_Surface>& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
  const occ::handle<Geom_Surface>& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
  EXPECT_EQ(aSurf1.get(), aSurf2.get());
}

TEST_F(BRepGraphTest, Shape_Unmodified_ReturnsSameShape)
{
  BRepGraph_NodeId   aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  TopoDS_Shape       aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape anOrig = myGraph.Shapes().Original(aFaceId);
  ASSERT_FALSE(anOrig.IsNull());
  EXPECT_TRUE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_AfterReplaceEdge_DiffersFromOriginal)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIds.Size(), 1);
  const BRepGraph_EdgeId anOldEdgeId =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIds.Value(0)).ChildEdgeId;
  const BRepGraph_EdgeId aNewEdgeId = addCompatibleReplacementEdge(myGraph, anOldEdgeId, false);
  ASSERT_TRUE(aNewEdgeId.IsValid());

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  // Find the face that owns wire 0.
  uint32_t aFaceIdx    = 0;
  bool     isFaceFound = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceIt.CurrentId(), BRepGraph_WireId::Start()))
    {
      aFaceIdx    = aFaceIt.CurrentId().Index;
      isFaceFound = true;
      break;
    }
  }
  ASSERT_TRUE(isFaceFound);
  BRepGraph_NodeId   aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
  TopoDS_Shape       aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape anOrig = myGraph.Shapes().Original(aFaceId);
  ASSERT_FALSE(anOrig.IsNull());
  EXPECT_FALSE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_WireKind_Valid)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeId::Kind::Wire, 0);
  TopoDS_Shape     aShape = myGraph.Shapes().Shape(aWireId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_WIRE);
}

TEST_F(BRepGraphTest, Shape_EdgeKind_Valid)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape     aShape = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_EDGE);
}

TEST_F(BRepGraphTest, Shape_VertexKind_Valid)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeId::Kind::Vertex, 0);
  TopoDS_Shape     aShape = myGraph.Shapes().Shape(aVtxId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_VERTEX);
}

TEST_F(BRepGraphTest, OwnGen_MutableEdge_PropagatesSubtreeGenUp)
{
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);

  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start()).MarkDirty();

  // Edge was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);

  // Check propagation up to parent wire and face (SubtreeGen).
  if (BRepGraph_EdgeId::Start().IsValid(myGraph.Topo().Edges().Nb()))
  {
    // Find a wire containing this edge.
    BRepGraph_WiresOfEdge aWireIt = myGraph.Topo().Edges().WiresOf(BRepGraph_EdgeId::Start());
    if (aWireIt.More())
    {
      const BRepGraph_WireId aWireId = aWireIt.CurrentId();
      EXPECT_GT(myGraph.Topo().Wires().Definition(aWireId).SubtreeGen, 0u);
      // Check propagation to owning face.
      for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
      {
        if (BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceIt.CurrentId(), aWireId))
        {
          EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceIt.CurrentId()).SubtreeGen, 0u);
          break;
        }
      }
    }
  }
}

TEST_F(BRepGraphTest, ReconstructShape_WireKind_NoThrow)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeId::Kind::Wire, 0);
  TopoDS_Shape     aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aWireId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraphTest, HasOriginalShape_AfterBuild_True)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId aFaceId(aFaceIt.CurrentId());
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(aFaceId))
      << "Face " << aFaceIt.CurrentId().Index
      << " should have original shape after BRepGraph::ShapesView::Add()";
  }
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    BRepGraph_NodeId anEdgeId(anEdgeIt.CurrentId());
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(anEdgeId))
      << "Edge " << anEdgeIt.CurrentId().Index
      << " should have original shape after BRepGraph::ShapesView::Add()";
  }
}

TEST_F(BRepGraphTest, Shape_CachedOnSecondCall)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeId::Kind::Vertex, 0);
  TopoDS_Shape     aFirst  = myGraph.Shapes().Shape(aVtxId);
  TopoDS_Shape     aSecond = myGraph.Shapes().Shape(aVtxId);
  EXPECT_TRUE(aFirst.IsSame(aSecond));
}

TEST_F(BRepGraphTest, Shape_InvalidatedAfterMutation)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape     aBefore = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aBefore.IsNull());

  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.123);
  TopoDS_Shape anAfter = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(anAfter.IsNull());

  // After mutation, Shape() reconstructs a new edge - different TShape.
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

TEST_F(BRepGraphTest, DefaultBuild_AssignsValidUIDs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 =
    aGraph.Shapes().Add(aBoxMaker.Shape());

  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aUID = aGraph.UIDs().Of(aFaceId);

  EXPECT_TRUE(aUID.IsValid());
  EXPECT_TRUE(aGraph.UIDs().Has(aUID));
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aUID), aFaceId);
}

TEST_F(BRepGraphTest, UIDsGeneration_IncrementsAcrossBuilds)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 =
    aGraph.Shapes().Add(aBoxMaker1.Shape());
  const uint32_t aGeneration1 = aGraph.UIDs().Generation();

  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 =
    aGraph.Shapes().Add(aBoxMaker2.Shape());
  const uint32_t aGeneration2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGeneration1, 0u);
  EXPECT_EQ(aGeneration2, aGeneration1 + 1);
}

TEST_F(BRepGraphTest, StaleUID_HasReturnsFalseAfterRebuild)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 =
    aGraph.Shapes().Add(aBoxMaker1.Shape());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  const BRepGraph_UID anOldUID =
    aGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  ASSERT_TRUE(anOldUID.IsValid());
  ASSERT_TRUE(aGraph.UIDs().Has(anOldUID));

  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 =
    aGraph.Shapes().Add(aBoxMaker2.Shape());

  EXPECT_FALSE(aGraph.UIDs().Has(anOldUID));
  EXPECT_FALSE(aGraph.UIDs().NodeIdFrom(anOldUID).IsValid());
}

TEST(BRepGraph_UIDsViewTest, ReverseLookupStaysCurrentAfterProgrammaticAdd)
{
  BRepGraph aGraph;

  const BRepGraph_VertexId aFirstVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  const BRepGraph_UID aFirstUID = aGraph.UIDs().Of(aFirstVertex);
  ASSERT_TRUE(aFirstUID.IsValid());
  ASSERT_EQ(aGraph.UIDs().NodeIdFrom(aFirstUID), aFirstVertex);

  const BRepGraph_VertexId aSecondVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 0.001);
  const BRepGraph_UID aSecondUID = aGraph.UIDs().Of(aSecondVertex);
  ASSERT_TRUE(aSecondUID.IsValid());

  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aFirstUID), aFirstVertex);
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aSecondUID), aSecondVertex);
  EXPECT_TRUE(aGraph.UIDs().Has(aFirstUID));
  EXPECT_TRUE(aGraph.UIDs().Has(aSecondUID));
}

// ===================================================================
// Group 2: History Tracking
// ===================================================================

TEST_F(BRepGraphTest, RecordHistory_MultipleRecords_SequenceNumbers)
{
  const size_t aBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("OpA",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("OpB",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("OpC",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aBefore + 3);

  // Check monotonically increasing sequence numbers.
  for (size_t anIdx = aBefore + 1; anIdx < aBefore + 3; ++anIdx)
  {
    EXPECT_GT(
      myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(anIdx).SequenceNumber,
      myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(anIdx - 1).SequenceNumber)
      << "SequenceNumber not monotonically increasing at index " << anIdx;
  }

  EXPECT_TRUE(
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(aBefore).OperationName.IsEqual(
      "OpA"));
  EXPECT_TRUE(myGraph.LayerRegistry()
                .Ensure<BRepGraph_LayerHistory>()
                ->Record(aBefore + 1)
                .OperationName.IsEqual("OpB"));
  EXPECT_TRUE(myGraph.LayerRegistry()
                .Ensure<BRepGraph_LayerHistory>()
                ->Record(aBefore + 2)
                .OperationName.IsEqual("OpC"));
}

TEST_F(BRepGraphTest, FindOriginal_SingleHop_ReturnsSource)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_LinearVector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "TestHop");

  BRepGraph_NodeId anOriginal =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginal(anEdge1);
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraphTest, FindDerived_SingleHop_ContainsTarget)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_LinearVector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "TestHop");

  NCollection_LinearVector<BRepGraph_NodeId> aDerived =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anEdge0);
  bool isFound = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == anEdge1)
    {
      isFound = true;
      break;
    }
  }
  EXPECT_TRUE(isFound) << "Edge1 not found in FindDerived(Edge0)";
}

TEST_F(BRepGraphTest, ApplyModification_MultiStepChain_FindOriginalTracesBack)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  BRepGraph_NodeId anEdge2(BRepGraph_NodeId::Kind::Edge, 2);

  // Step 1: edge0 -> edge1
  auto aModifier1 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_LinearVector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };
  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier1, "Step1");

  // Step 2: edge1 -> edge2
  auto aModifier2 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_LinearVector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge2);
    return aResult;
  };
  myGraph.Editor().Gen().ApplyModification(anEdge1, aModifier2, "Step2");

  // FindOriginal from edge2 should trace back to edge0.
  BRepGraph_NodeId anOriginal =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginal(anEdge2);
  EXPECT_EQ(anOriginal, anEdge0);

  // FindDerived from edge0 returns leaf-only transitive descendants.
  // edge1 is an intermediate (it has further derived edge2), so only edge2 is returned.
  NCollection_LinearVector<BRepGraph_NodeId> aDerived =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anEdge0);
  bool isEdge2Found = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == anEdge2)
    {
      isEdge2Found = true;
    }
  }
  EXPECT_TRUE(isEdge2Found) << "Edge2 not found in FindDerived(Edge0)";

  // edge1 can be found by querying FindDerived on the intermediate step.
  NCollection_LinearVector<BRepGraph_NodeId> aDerived1 =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anEdge1);
  bool isEdge2FromEdge1 = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived1)
  {
    if (aDerivedId == anEdge2)
    {
      isEdge2FromEdge1 = true;
    }
  }
  EXPECT_TRUE(isEdge2FromEdge1) << "Edge2 not found in FindDerived(Edge1)";
}

// ===================================================================
// Group 3: Mutation APIs
// ===================================================================

TEST_F(BRepGraphTest, AddPCurve_NewPCurve_RetrievableViaFindPCurveCoEdgeId)
{
  BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FaceId aFaceId(0);

  occ::handle<Geom2d_Line> aCurve2d = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  std::ignore = myGraph.Editor().CoEdges().Add(anEdgeId, aFaceId, aCurve2d, 0.0, 1.0);

  const BRepGraph_CoEdgeId aRetrievedId =
    BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph, anEdgeId, aFaceId);
  EXPECT_TRUE(aRetrievedId.IsValid());
  if (aRetrievedId.IsValid())
  {
    EXPECT_TRUE(myGraph.Topo().CoEdges().Definition(aRetrievedId).Curve2DRepId.IsValid());
    bool isListedByEdge = false;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : myGraph.Topo().Edges().CoEdges(anEdgeId))
    {
      if (aCoEdgeId == aRetrievedId)
      {
        isListedByEdge = true;
        break;
      }
    }
    EXPECT_TRUE(isListedByEdge) << "Created face-bound coedge must be present in edge relations";
  }
  EXPECT_TRUE(myGraph.ValidateRelations());
}

TEST_F(BRepGraphTest, ReplaceEdge_Reversed_OrientationFlipped)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIds.Size(), 1);

  const BRepGraphInc::CoEdgeDef& anOrigCoEdge =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIds.Value(0));
  const BRepGraph_EdgeId anOldEdgeId       = anOrigCoEdge.ChildEdgeId;
  TopAbs_Orientation     anOrigOrientation = anOrigCoEdge.Orientation;

  const BRepGraph_EdgeId aNewEdgeId = addCompatibleReplacementEdge(myGraph, anOldEdgeId, true);
  ASSERT_TRUE(aNewEdgeId.IsValid());

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, true);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdsAfter =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIdsAfter.Size(), 1);
  const BRepGraphInc::CoEdgeDef& aNewCoEdge =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIdsAfter.Value(0));
  EXPECT_EQ(aNewCoEdge.ChildEdgeId.Index, aNewEdgeId.Index);

  // Orientation should be flipped relative to original.
  TopAbs_Orientation anExpected =
    (anOrigOrientation == TopAbs_FORWARD) ? TopAbs_REVERSED : TopAbs_FORWARD;
  EXPECT_EQ(aNewCoEdge.Orientation, anExpected);
}

TEST_F(BRepGraphTest, ReplaceEdge_UpdatesEdgeToCoEdgeRelations)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIds.Size(), 1);

  const BRepGraph_CoEdgeId aCoEdgeId   = aCoEdgeIds.Value(0);
  const BRepGraph_EdgeId   anOldEdgeId = myGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  const BRepGraph_EdgeId   aNewEdgeId  = addCompatibleReplacementEdge(myGraph, anOldEdgeId, false);
  ASSERT_TRUE(aNewEdgeId.IsValid());

  auto hasCoEdge = [&](const BRepGraph_EdgeId theEdgeId) {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      myGraph.Topo().Edges().CoEdges(theEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdge : aCoEdges)
    {
      if (aCoEdge == aCoEdgeId)
      {
        return true;
      }
    }
    return false;
  };

  ASSERT_TRUE(hasCoEdge(anOldEdgeId));

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  EXPECT_FALSE(hasCoEdge(anOldEdgeId));
  EXPECT_TRUE(hasCoEdge(aNewEdgeId));
}

TEST_F(BRepGraphTest, RemoveCoEdge_PrunesOrphanAndKeepsRelationsConsistent)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgeIdsBefore =
    myGraph.Topo().Wires().Relations(BRepGraph_WireId::Start()).CoEdgeIds;
  ASSERT_GE(aCoEdgeIdsBefore.Size(), 1);

  const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIdsBefore.Value(0);
  const BRepGraph_EdgeId   anEdgeId  = myGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;

  ASSERT_TRUE(myGraph.Editor().Wires().RemoveCoEdge(BRepGraph_WireId::Start(), aCoEdgeId));

  const NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgeIdsAfter =
    myGraph.Topo().Wires().Relations(BRepGraph_WireId::Start()).CoEdgeIds;
  EXPECT_EQ(aCoEdgeIdsAfter.Size(), aCoEdgeIdsBefore.Size() - 1);
  EXPECT_TRUE(aCoEdgeId.IsRemoved(myGraph));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_WireId& aWireId : myGraph.Topo().Edges().WiresOf(anEdgeId))
  {
    EXPECT_NE(aWireId, BRepGraph_WireId::Start());
  }

  for (const BRepGraph_CoEdgeId& aEdgeCoEdgeId : myGraph.Topo().Edges().CoEdges(anEdgeId))
  {
    EXPECT_NE(aEdgeCoEdgeId, aCoEdgeId);
  }
}

TEST_F(BRepGraphTest, CleanupRemovedRefs_ManuallyRemovedEdge_UnbindsEdgeWireRelations)
{
  const BRepGraph_WireId                              aWireId = BRepGraph_WireId::Start();
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds;
  ASSERT_GE(aCoEdgeIds.Size(), 1);

  const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIds.Value(0);
  const BRepGraph_EdgeId   anEdgeId  = myGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(containsId(myGraph.Topo().Edges().WiresOf(anEdgeId), aWireId));

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdgeId));

  myGraph.Editor().Gen().CleanupRemovedReferences();

  EXPECT_TRUE(aCoEdgeId.IsRemoved(myGraph));
  EXPECT_FALSE(containsId(myGraph.Topo().Edges().WiresOf(anEdgeId), aWireId));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveChild_PreservesSharedChildAndKeepsRelationsConsistent)
{
  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_CompoundId aCompoundId = myGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompoundId.IsValid());

  const NCollection_LinearVector<BRepGraph_ChildRefId>& aChildRefsBefore =
    BRepGraph_TestTools::ChildRefsOfParent(myGraph, BRepGraph_NodeId(aCompoundId));
  ASSERT_EQ(aChildRefsBefore.Size(), 1);

  const BRepGraph_ChildRefId    aChildRefId = aChildRefsBefore.Value(0);
  const BRepGraphInc::ChildRef& aRef        = myGraph.Refs().Children().Entry(aChildRefId);
  const BRepGraph_NodeId        aChildId    = aRef.ChildNodeId;

  ASSERT_TRUE(myGraph.Editor().Compounds().RemoveChild(aCompoundId, aChildRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(myGraph, BRepGraph_NodeId(aCompoundId)), 0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(aChildId));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
  EXPECT_EQ(myGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aCompoundId)).Size(), 0);
}

TEST_F(BRepGraphTest, RemoveChild_UsesDistinctRefsForSharedChild)
{
  ASSERT_GT(myGraph.Topo().Solids().Nb(), 0);
  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(aSolidId));
  const BRepGraph_CompoundId aFirstCompound =
    myGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aFirstCompound.IsValid());

  NCollection_LinearVector<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId                 aSecondCompound =
    myGraph.Editor().Compounds().Add(anEmptyChildren.ToArray1());
  ASSERT_TRUE(aSecondCompound.IsValid());

  const NCollection_LinearVector<BRepGraph_ChildRefId>& aFirstRefs =
    BRepGraph_TestTools::ChildRefsOfParent(myGraph, BRepGraph_NodeId(aFirstCompound));
  ASSERT_EQ(aFirstRefs.Size(), 1);
  const BRepGraph_ChildRefId aFirstRefId = aFirstRefs.Value(0);

  const BRepGraph_ChildRefId aSecondRefId =
    myGraph.Editor().Compounds().Append(aSecondCompound, BRepGraph_NodeId(aSolidId));
  ASSERT_TRUE(aSecondRefId.IsValid());
  ASSERT_NE(aFirstRefId, aSecondRefId);
  myGraph.Editor().Gen().CleanupRemovedReferences();

  const BRepGraph_Validate::Result aValidDistinctRefs =
    BRepGraph_Validate::Perform(myGraph, BRepGraph_Validate::Options::Audit());
  ASSERT_TRUE(aValidDistinctRefs.IsValid());
  BRepGraph_CompoundsOfChild aCompoundsBefore(
    myGraph,
    myGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aSolidId)));
  EXPECT_TRUE(containsId(aCompoundsBefore, aFirstCompound));
  EXPECT_TRUE(containsId(aCompoundsBefore, aSecondCompound));

  ASSERT_TRUE(myGraph.Editor().Compounds().RemoveChild(aFirstCompound, aFirstRefId));
  EXPECT_TRUE(aFirstRefId.IsRemoved(myGraph));
  EXPECT_FALSE(aSecondRefId.IsRemoved(myGraph));
  BRepGraph_CompoundsOfChild aCompoundsAfterFirstRemove(
    myGraph,
    myGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aSolidId)));
  EXPECT_FALSE(containsId(aCompoundsAfterFirstRemove, aFirstCompound));
  EXPECT_TRUE(containsId(aCompoundsAfterFirstRemove, aSecondCompound));
  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(myGraph, BRepGraph_NodeId(aFirstCompound)),
            0);
  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(myGraph, BRepGraph_NodeId(aSecondCompound)),
            1);

  ASSERT_TRUE(myGraph.Editor().Compounds().RemoveChild(aSecondCompound, aSecondRefId));
  EXPECT_TRUE(aSecondRefId.IsRemoved(myGraph));
  BRepGraph_CompoundsOfChild aCompoundsAfterSecondRemove(
    myGraph,
    myGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aSolidId)));
  EXPECT_FALSE(containsId(aCompoundsAfterSecondRemove, aSecondCompound));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveFace_PrunesOrphanAndKeepsRelationsConsistent)
{
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefsBefore =
    BRepGraph_TestTools::FaceRefsOfShell(myGraph, BRepGraph_ShellId::Start());
  ASSERT_GE(aFaceRefsBefore.Size(), 1);
  const size_t aNbFaceRefsBefore = aFaceRefsBefore.Size();

  const BRepGraph_FaceRefId    aFaceRefId = aFaceRefsBefore.Value(0);
  const BRepGraphInc::FaceRef& aRef       = myGraph.Refs().Faces().Entry(aFaceRefId);
  const BRepGraph_FaceId       aFaceId    = aRef.ChildFaceId;

  ASSERT_TRUE(myGraph.Editor().Shells().RemoveFace(BRepGraph_ShellId::Start(), aFaceRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId::Start()),
            aNbFaceRefsBefore - 1);
  EXPECT_TRUE(aFaceId.IsRemoved(myGraph));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_SolidId& aSolidId : BRepGraph_SolidsOfShell(
         myGraph,
         myGraph.Topo().Shells().Relations(BRepGraph_ShellId::Start()).ParentShellRefIds))
  {
    EXPECT_NE(aSolidId, BRepGraph_SolidId());
  }

  for (const BRepGraph_ShellId& aShellId :
       BRepGraph_ShellsOfFace(myGraph, myGraph.Topo().Faces().Relations(aFaceId).ParentFaceRefIds))
  {
    EXPECT_NE(aShellId, BRepGraph_ShellId::Start());
  }
}

TEST_F(BRepGraphTest, RemoveShell_PrunesOrphanAndKeepsRelationsConsistent)
{
  const NCollection_LinearVector<BRepGraph_ShellRefId>& aShellRefsBefore =
    BRepGraph_TestTools::ShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start());
  ASSERT_GE(aShellRefsBefore.Size(), 1);
  const size_t aNbShellRefsBefore = aShellRefsBefore.Size();

  const BRepGraph_ShellRefId    aShellRefId = aShellRefsBefore.Value(0);
  const BRepGraphInc::ShellRef& aRef        = myGraph.Refs().Shells().Entry(aShellRefId);
  const BRepGraph_ShellId       aShellId    = aRef.ChildShellId;

  ASSERT_TRUE(myGraph.Editor().Solids().RemoveShell(BRepGraph_SolidId::Start(), aShellRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start()),
            aNbShellRefsBefore - 1);
  EXPECT_TRUE(aShellId.IsRemoved(myGraph));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_CompSolidId& aCompSolidId : BRepGraph_CompSolidsOfSolid(
         myGraph,
         myGraph.Topo().Solids().Relations(BRepGraph_SolidId::Start()).ParentSolidRefIds))
  {
    EXPECT_NE(aCompSolidId, BRepGraph_CompSolidId());
  }

  for (const BRepGraph_SolidId& aSolidId :
       BRepGraph_SolidsOfShell(myGraph,
                               myGraph.Topo().Shells().Relations(aShellId).ParentShellRefIds))
  {
    EXPECT_NE(aSolidId, BRepGraph_SolidId::Start());
  }
}

TEST_F(BRepGraphTest, RemoveWire_PrunesOrphanAndKeepsRelationsConsistent)
{
  BRepGraph_FaceId    aFaceId;
  BRepGraph_WireRefId aWireRefId;
  bool                isFound = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More() && !isFound; aFaceIt.Next())
  {
    const BRepGraph_FaceId                               aCandidateFaceId = aFaceIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
      BRepGraph_TestTools::WireRefsOfFace(myGraph, aCandidateFaceId);
    for (const BRepGraph_WireRefId& aCandidateWireRefId : aWireRefs)
    {
      if (myGraph.Refs().Wires().Entry(aCandidateWireRefId).ChildWireId
          == BRepGraph_WireId::Start())
      {
        aFaceId    = aCandidateFaceId;
        aWireRefId = aCandidateWireRefId;
        isFound    = true;
        break;
      }
    }
  }

  ASSERT_TRUE(isFound);
  const size_t aNbWireRefsBefore = BRepGraph_TestTools::CountWireRefsOfFace(myGraph, aFaceId);

  ASSERT_TRUE(myGraph.Editor().Faces().RemoveWire(aFaceId, aWireRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountWireRefsOfFace(myGraph, aFaceId), aNbWireRefsBefore - 1);
  EXPECT_FALSE(BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceId, BRepGraph_WireId::Start()));
  EXPECT_TRUE(BRepGraph_WireId::Start().IsRemoved(myGraph));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_FaceId& aWireFaceId : BRepGraph_FacesOfWire(
         myGraph,
         myGraph.Topo().Wires().Relations(BRepGraph_WireId::Start()).ParentWireRefIds))
  {
    EXPECT_NE(aWireFaceId, aFaceId);
  }
}

TEST_F(BRepGraphTest, RemoveOccurrence_PrunesOccurrenceSubtreeAndKeepsRelationsConsistent)
{
  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  ASSERT_TRUE(aAssemblyId.IsValid());
  const BRepGraph_OccurrenceId anOccId =
    myGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aOccurrenceRefsBefore =
    myGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  ASSERT_EQ(aOccurrenceRefsBefore.Size(), 1);
  const BRepGraph_OccurrenceRefId anOccurrenceRefId = aOccurrenceRefsBefore.Value(0);

  ASSERT_TRUE(myGraph.Editor().Products().RemoveOccurrence(aAssemblyId, anOccurrenceRefId));

  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aAssemblyId).Size(), 0);
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(anOccId));
  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aAssemblyId), 0);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveShapeRoot_PrunesUniqueTopologyRoot)
{
  const BRepGraph_ProductId aPartId          = BRepGraph_ProductId::Start();
  const BRepGraph_NodeId    aShapeRootBefore = myGraph.Topo().Products().ShapeRoot(aPartId);
  ASSERT_TRUE(aShapeRootBefore.IsValid());

  ASSERT_TRUE(myGraph.Editor().Products().RemoveShapeRoot(aPartId));

  EXPECT_FALSE(myGraph.Topo().Products().ShapeRoot(aPartId).IsValid());
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aShapeRootBefore));
  EXPECT_FALSE(myGraph.Topo().Products().IsPart(aPartId));
  EXPECT_FALSE(myGraph.Topo().Products().IsAssembly(aPartId));
  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aPartId), 0);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveVertex_ClearsBoundarySlotAndPrunesUniqueVertex)
{
  const BRepGraph_VertexId aStartVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion());
  const BRepGraph_VertexId anEndVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(10.0, 0.0, 0.0), Precision::Confusion());
  ASSERT_TRUE(aStartVertex.IsValid());
  ASSERT_TRUE(anEndVertex.IsValid());

  const BRepGraph_EdgeId anEdge = myGraph.Editor().Edges().Add(aStartVertex,
                                                               anEndVertex,
                                                               occ::handle<Geom_Curve>(),
                                                               0.0,
                                                               1.0,
                                                               Precision::Confusion());
  ASSERT_TRUE(anEdge.IsValid());

  const BRepGraph_VertexRefId aStartRefId =
    myGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;
  ASSERT_TRUE(aStartRefId.IsValid());

  ASSERT_TRUE(myGraph.Editor().Edges().RemoveVertex(anEdge, aStartRefId));

  EXPECT_FALSE(myGraph.Topo().Edges().Definition(anEdge).StartVertexRefId.IsValid());
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aStartVertex));
  EXPECT_FALSE(BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdge).IsValid());
  EXPECT_TRUE(BRepGraph_Tool::Edge::EndVertexId(myGraph, anEdge).IsValid());
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveNode_EdgeWithReplacement_ReparentsAllCoEdges)
{
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    BRepGraph_TestTools::CoEdgesOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeIds.Size(), 1);

  const BRepGraph_EdgeId anOldEdgeId =
    myGraph.Topo().CoEdges().Definition(aCoEdgeIds.Value(0)).ChildEdgeId;
  const BRepGraph_EdgeId aNewEdgeId((anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb());

  myGraph.Editor().Gen().ReplaceNode(anOldEdgeId, aNewEdgeId);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& anOldCoEdges =
    myGraph.Topo().Edges().CoEdges(anOldEdgeId);
  EXPECT_EQ(anOldCoEdges.Size(), 0);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aNewCoEdges =
    myGraph.Topo().Edges().CoEdges(aNewEdgeId);
  EXPECT_GT(aNewCoEdges.Size(), 0);
  for (const BRepGraph_CoEdgeId& aNewCoEdgeId : aNewCoEdges)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aNewCoEdgeId);
    EXPECT_EQ(aCoEdge.ChildEdgeId, aNewEdgeId);
  }
}

TEST_F(BRepGraphTest, MutableVertex_ChangePoint_Verified)
{
  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMutVert =
    myGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
  myGraph.Editor().Vertices().SetPoint(aMutVert, gp_Pnt(99.0, 99.0, 99.0));

  const BRepGraphInc::VertexDef& aVert =
    myGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start());
  EXPECT_NEAR(aVert.Point.X(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Y(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Z(), 99.0, Precision::Confusion());
}

// ===================================================================
// Group 4: Geometric Queries
// ===================================================================

TEST_F(BRepGraphTest, EdgeDef_HasValidCurve3d)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeIt.CurrentId()))
    {
      continue;
    }

    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeIt.CurrentId()))
      << "Edge " << anEdgeIt.CurrentId().Index << " has no Curve3D rep";
  }
}

// ===================================================================
// Group 7: Detection Methods
// ===================================================================

TEST_F(BRepGraphTest, FreeEdges_SingleFace_AllEdgesFree)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  const TopoDS_Face&  aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 = aGraph.Shapes().Add(aFace);
  ASSERT_FALSE(aGraph.IsEmpty());

  NCollection_DynamicArray<BRepGraph_EdgeId> aFreeEdges = collectFreeEdges(aGraph);
  EXPECT_EQ(aFreeEdges.Size(), 4);
}

TEST_F(BRepGraphTest, Decompose_ThreeDisconnectedFaces_ThreeComponents)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);
  BRepPrimAPI_MakeBox aBox3(30.0, 30.0, 30.0);

  TopExp_Explorer anExp1(aBox1.Shape(), TopAbs_FACE);
  TopExp_Explorer anExp2(aBox2.Shape(), TopAbs_FACE);
  TopExp_Explorer anExp3(aBox3.Shape(), TopAbs_FACE);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, anExp1.Current());
  aBuilder.Add(aCompound, anExp2.Current());
  aBuilder.Add(aCompound, anExp3.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 3);

  EXPECT_EQ(countFaceComponents(aGraph), 3);
}

TEST_F(BRepGraphTest, DetectToleranceConflicts_ManualConflict_Detected)
{
  // Find two edges that share the same Curve3d handle.
  bool           isConflictSetUp = false;
  const uint32_t aNbEdges        = myGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges) && !isConflictSetUp; ++anEdgeId)
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId)
        || !BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId))
    {
      continue;
    }

    for (BRepGraph_EdgeId anOtherId(anEdgeId.Index + 1); anOtherId.IsValid(aNbEdges); ++anOtherId)
    {
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, anOtherId)
          || !BRepGraph_Tool::Edge::HasCurve(myGraph, anOtherId))
      {
        continue;
      }
      if (BRepGraph_Tool::Edge::Curve(myGraph, anEdgeId).get()
          != BRepGraph_Tool::Edge::Curve(myGraph, anOtherId).get())
      {
        continue;
      }

      // Set very different tolerances on two edges sharing the same curve.
      myGraph.Editor().Edges().SetTolerance(anEdgeId, 0.001);
      myGraph.Editor().Edges().SetTolerance(anOtherId, 1.0);

      isConflictSetUp = true;
      break;
    }
  }

  if (isConflictSetUp)
  {
    NCollection_DynamicArray<BRepGraph_EdgeId> aConflicts(16);
    NCollection_Map<int>                       aConflictKeys;
    for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId)
          || !BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId))
      {
        continue;
      }

      const occ::handle<Geom_Curve>& aCurve = BRepGraph_Tool::Edge::Curve(myGraph, anEdgeId);
      double aMinTol                        = myGraph.Topo().Edges().Definition(anEdgeId).Tolerance;
      double aMaxTol                        = aMinTol;
      NCollection_DynamicArray<BRepGraph_EdgeId> aCurveEdges(4);
      aCurveEdges.Append(anEdgeId);
      for (BRepGraph_EdgeIterator anOtherIt(myGraph); anOtherIt.More(); anOtherIt.Next())
      {
        const BRepGraph_EdgeId anOtherId = anOtherIt.CurrentId();
        if (anOtherId == anEdgeId || BRepGraph_Tool::Edge::Degenerated(myGraph, anOtherId)
            || !BRepGraph_Tool::Edge::HasCurve(myGraph, anOtherId)
            || BRepGraph_Tool::Edge::Curve(myGraph, anOtherId).get() != aCurve.get())
        {
          continue;
        }

        const double aTol = myGraph.Topo().Edges().Definition(anOtherId).Tolerance;
        aMinTol           = std::min(aMinTol, aTol);
        aMaxTol           = std::max(aMaxTol, aTol);
        aCurveEdges.Append(anOtherId);
      }

      if (aCurveEdges.Size() > 1 && aMaxTol - aMinTol > 0.5)
      {
        for (const BRepGraph_EdgeId& aConflictId : aCurveEdges)
        {
          if (aConflictKeys.Add(aConflictId.Index))
          {
            aConflicts.Append(aConflictId);
          }
        }
      }
    }
    EXPECT_GE(aConflicts.Size(), 1);
  }
}

TEST_F(BRepGraphTest, Build_EmptyCompound_IsEmptyZeroCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 = aGraph.Shapes().Add(aCompound);
  EXPECT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 0);
}

TEST_F(BRepGraphTest, TopoNode_GenericLookup_MatchesTypedAccess)
{
  BRepGraph_NodeId             aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraphInc::BaseDef* aBase = myGraph.Topo().Gen().TopoEntity(aFaceId);
  EXPECT_NE(aBase, nullptr);

  // Invalid node id should return nullptr.
  BRepGraph_NodeId             anInvalidId;
  const BRepGraphInc::BaseDef* anInvalidBase = myGraph.Topo().Gen().TopoEntity(anInvalidId);
  EXPECT_EQ(anInvalidBase, nullptr);
}

// ===================================================================
// Group 9: Node Counts and Identity
// ===================================================================

TEST_F(BRepGraphTest, NbNodes_Box_TotalCount)
{
  // NbNodes should equal sum of all per-kind counts (topology + assembly).
  size_t anExpected = static_cast<size_t>(myGraph.Topo().Solids().Nb())
                      + myGraph.Topo().Shells().Nb() + myGraph.Topo().Faces().Nb()
                      + myGraph.Topo().Wires().Nb() + myGraph.Topo().CoEdges().Nb()
                      + myGraph.Topo().Edges().Nb() + myGraph.Topo().Vertices().Nb()
                      + myGraph.Topo().Compounds().Nb() + myGraph.Topo().CompSolids().Nb()
                      + myGraph.Topo().Products().Nb() + myGraph.Topo().Occurrences().Nb();
  EXPECT_EQ(myGraph.Topo().Gen().NbNodes(), anExpected);
}

TEST_F(BRepGraphTest, HasUID_ValidFace_ReturnsTrue)
{
  const BRepGraph_UID& aUID = myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  EXPECT_TRUE(myGraph.UIDs().Has(aUID));
}

TEST_F(BRepGraphTest, HasUID_InvalidUID_ReturnsFalse)
{
  BRepGraph_UID anInvalidUID = BRepGraph_UID::Invalid();
  EXPECT_FALSE(myGraph.UIDs().Has(anInvalidUID));
}

TEST_F(BRepGraphTest, NodeIdFromUID_InvalidUID_ReturnsInvalid)
{
  BRepGraph_UID    anInvalidUID = BRepGraph_UID::Invalid();
  BRepGraph_NodeId aResolved    = myGraph.UIDs().NodeIdFrom(anInvalidUID);
  EXPECT_FALSE(aResolved.IsValid());
}

TEST_F(BRepGraphTest, Allocator_DefaultConstructor_NotNull)
{
  EXPECT_FALSE(myGraph.Allocator().IsNull());
}

TEST_F(BRepGraphTest, Build_DefaultAllocator_IsNotEmpty)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_FALSE(aGraph.Allocator().IsNull());
}

// ===================================================================
// Group 10: Topology Structure
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_BoxOuterWires)
{
  // All outer wires of a box face should be closed.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    ASSERT_TRUE(anOuterWire.IsValid());
    EXPECT_TRUE(BRepGraph_Tool::Wire::IsClosed(myGraph, anOuterWire))
      << "Outer wire of face " << aFaceIt.CurrentId().Index << " should be closed";
  }
}

TEST_F(BRepGraphTest, Face_InnerWireRefs_BoxHasNone)
{
  // Box faces have no holes, so each face should have one wire ref.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
      BRepGraph_TestTools::WireRefsOfFace(myGraph, aFaceIt.CurrentId());
    EXPECT_EQ(aWireRefs.Size(), 1)
      << "Box face " << aFaceIt.CurrentId().Index << " should have one wire";
  }
}

TEST_F(BRepGraphTest, Face_Orientation_ValidValue)
{
  // Verify face orientations in the shell's incidence refs.
  ASSERT_EQ(myGraph.Topo().Shells().Nb(), 1);
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(myGraph, BRepGraph_ShellId::Start());
  for (size_t aRefIdx = 0; aRefIdx < aFaceRefs.Size(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aFaceRef = myGraph.Refs().Faces().Entry(aFaceRefs.Value(aRefIdx));
    TopAbs_Orientation           anOri    = aFaceRef.Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED)
      << "Face ref " << aRefIdx << " has unexpected orientation " << anOri;
  }
}

TEST_F(BRepGraphTest, Shell_ContainsSixFaces)
{
  ASSERT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId::Start()), 6);
}

TEST_F(BRepGraphTest, Solid_ContainsOneShell)
{
  ASSERT_EQ(myGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start()), 1);
}

TEST_F(BRepGraphTest, Edge_ParamRange_ValidBounds)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeIt.CurrentId()))
    {
      continue;
    }
    const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(myGraph, anEdgeIt.CurrentId());
    EXPECT_LT(aFirst, aLast) << "Edge " << anEdgeIt.CurrentId().Index
                             << " has invalid parameter range [" << aFirst << ", " << aLast << "]";
  }
}

TEST_F(BRepGraphTest, Vertex_TolerancePositive)
{
  for (BRepGraph_VertexIterator aVertexIt(myGraph); aVertexIt.More(); aVertexIt.Next())
  {
    EXPECT_GT(BRepGraph_Tool::Vertex::Tolerance(myGraph, aVertexIt.CurrentId()), 0.0)
      << "Vertex " << aVertexIt.CurrentId().Index << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Edge_TolerancePositive)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_GT(BRepGraph_Tool::Edge::Tolerance(myGraph, anEdgeIt.CurrentId()), 0.0)
      << "Edge " << anEdgeIt.CurrentId().Index << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Face_ToleranceNonNegative)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_GE(BRepGraph_Tool::Face::Tolerance(myGraph, aFaceIt.CurrentId()), 0.0)
      << "Face " << aFaceIt.CurrentId().Index << " has negative tolerance";
  }
}

// ===================================================================
// Group 13: Mutation (Extended)
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_DerivedFromCoedgeChain)
{
  // Wire closure is now derived from the ordered coedge chain.
  const BRepGraph_WireId anOuterWire =
    BRepGraph_TestTools::OuterWireOfFace(myGraph, BRepGraph_FaceId::Start());
  ASSERT_TRUE(anOuterWire.IsValid());
  // Outer wires of a box face should be closed.
  EXPECT_TRUE(BRepGraph_Tool::Wire::IsClosed(myGraph, anOuterWire));
}

// ===================================================================
// Group 14: Build From Different Root Shapes
// ===================================================================

TEST_F(BRepGraphTest, Build_SingleFace_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  const TopoDS_Face&  aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes17 = aGraph.Shapes().Add(aFace);
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 4);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 4);
}

TEST_F(BRepGraphTest, Build_Shell_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes18 =
    aGraph.Shapes().Add(anExp.Current());
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
}

TEST_F(BRepGraphTest, Build_CompoundOfTwoSolids)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1.Shape());
  aBuilder.Add(aCompound, aBox2.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes19 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
}

TEST_F(BRepGraphTest, ReconstructShape_ShellRoot_SameFaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes20 =
    aGraph.Shapes().Add(anExp.Current());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 1);

  BRepGraph_NodeId   aShellId(BRepGraph_NodeId::Kind::Shell, 0);
  const TopoDS_Shape aReconstructed = aGraph.Shapes().Reconstruct(aShellId);

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aReconstructed, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

// ===================================================================
// Group 15: UID Properties
// ===================================================================

TEST_F(BRepGraphTest, UID_FaceIsTopology)
{
  EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0)).IsTopology());
}

TEST_F(BRepGraphTest, UID_AllTopoNodesHaveValidUIDs)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(aFaceIt.CurrentId())).IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has invalid UID";
  }
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(anEdgeIt.CurrentId())).IsValid())
      << "Edge " << anEdgeIt.CurrentId().Index << " has invalid UID";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWireIdx_MatchesFaceDef)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    EXPECT_TRUE(anOuterWire.IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has no outer wire";
    if (!anOuterWire.IsValid())
    {
      continue;
    }
    EXPECT_LT(anOuterWire.Index, myGraph.Topo().Wires().Nb())
      << "Face " << aFaceIt.CurrentId().Index << " outer wire index out of range";
  }
}

TEST_F(BRepGraphTest, Wire_CoEdges_FourEdgesPerBoxFace)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    ASSERT_TRUE(anOuterWire.IsValid());
    EXPECT_EQ(BRepGraph_TestTools::CountCoEdgesOfWire(myGraph, anOuterWire), 4)
      << "Box face " << aFaceIt.CurrentId().Index << " should have 4 coedges in its outer wire";
  }
}

// ===================================================================
// Group: History Enabled Flag
// ===================================================================

TEST_F(BRepGraphTest, SetHistoryEnabled_DefaultTrue)
{
  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());
}

TEST_F(BRepGraphTest, SetHistoryEnabled_DisableAndQuery)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);
  EXPECT_FALSE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(true);
  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());
}

TEST_F(BRepGraphTest, RecordHistory_Disabled_NoRecordAdded)
{
  const size_t aBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);

  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("ShouldNotRecord",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aBefore);
}

TEST_F(BRepGraphTest, RecordHistory_ReEnabled_RecordsAgain)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);

  const size_t aBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("Skipped",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aBefore);

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(true);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("Recorded",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aBefore + 1);
  EXPECT_TRUE(
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(aBefore).OperationName.IsEqual(
      "Recorded"));
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_NoHistoryNoDerivedEdges)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);

  const size_t aNbHistBefore =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_LinearVector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "NoHistory");

  // No history records should be added.
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbHistBefore);
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_ModifierStillRuns)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);

  bool             isModifierCalled = false;
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    isModifierCalled = true;
    NCollection_LinearVector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge0);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "CheckModifier");
  EXPECT_TRUE(isModifierCalled);
}

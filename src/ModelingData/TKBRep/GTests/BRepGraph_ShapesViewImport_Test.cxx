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
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_Iterator.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_RelatedIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_LinearVector.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <gtest/gtest.h>

namespace
{
template <class theIteratorType>
static uint32_t countIterator(theIteratorType theIterator)
{
  uint32_t aCount = 0;
  for (; theIterator.More(); theIterator.Next())
  {
    ++aCount;
  }
  return aCount;
}

static bool edgeHasFace(const BRepGraph&       theGraph,
                        const BRepGraph_EdgeId theEdge,
                        const BRepGraph_FaceId theFace)
{
  for (BRepGraph_FacesOfEdge aFaceIt = theGraph.Topo().Edges().FacesOf(theEdge); aFaceIt.More();
       aFaceIt.Next())
  {
    if (aFaceIt.CurrentId() == theFace)
    {
      return true;
    }
  }
  return false;
}

static uint32_t countSharedEdges(const BRepGraph&       theGraph,
                                 const BRepGraph_FaceId theFaceA,
                                 const BRepGraph_FaceId theFaceB)
{
  uint32_t aCount = 0;
  for (BRepGraph_RelatedIterator anEdgeIt(theGraph, BRepGraph_NodeId(theFaceA)); anEdgeIt.More();
       anEdgeIt.Next())
  {
    if (anEdgeIt.CurrentRelation() == BRepGraph_RelatedIterator::RelationKind::BoundaryEdge
        && edgeHasFace(theGraph, BRepGraph_EdgeId::FromNodeId(anEdgeIt.Current()), theFaceB))
    {
      ++aCount;
    }
  }
  return aCount;
}

static uint32_t countAdjacentFaces(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  uint32_t aCount = 0;
  for (BRepGraph_RelatedIterator anIt(theGraph, BRepGraph_NodeId(theFace)); anIt.More();
       anIt.Next())
  {
    if (anIt.CurrentRelation() == BRepGraph_RelatedIterator::RelationKind::AdjacentFace)
    {
      ++aCount;
    }
  }
  return aCount;
}

static bool containsEdge(const NCollection_LinearVector<BRepGraph_EdgeId>& theEdges,
                         const BRepGraph_EdgeId                            theEdge)
{
  for (const BRepGraph_EdgeId& anEdge : theEdges)
  {
    if (anEdge == theEdge)
    {
      return true;
    }
  }
  return false;
}

static uint32_t countAdjacentEdgesOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph))
  {
    return 0;
  }

  NCollection_LinearVector<BRepGraph_EdgeId> anAdjacentEdges;
  for (BRepGraph_DefsVertexOfEdge aVertexIt(theGraph, theEdge); aVertexIt.More(); aVertexIt.Next())
  {
    for (const BRepGraph_EdgeId& anAdjacentEdgeId :
         theGraph.Topo().Vertices().Edges(aVertexIt.CurrentId()))
    {
      if (anAdjacentEdgeId == theEdge || anAdjacentEdgeId.IsRemoved(theGraph)
          || containsEdge(anAdjacentEdges, anAdjacentEdgeId))
      {
        continue;
      }
      anAdjacentEdges.Append(anAdjacentEdgeId);
    }
  }
  return static_cast<uint32_t>(anAdjacentEdges.Size());
}
} // namespace

// ============================================================
// Task 2A: Programmatic Node Addition API
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, AddVertex_ReturnsValidId)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aVtxId = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_TRUE(aVtxId.IsValid());
  EXPECT_EQ(aVtxId.Index, 0);

  const BRepGraphInc::VertexDef& aVtxDef =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start());
  EXPECT_NEAR(aVtxDef.Point.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Point.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVtxDef.Tolerance, 0.001, 1e-10);
}

TEST(BRepGraph_ShapesViewImportTest, AddEdge_WithCurve)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10.0, 0.0, 0.0), 0.001);

  occ::handle<Geom_Line> aLine    = new Geom_Line(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)));
  BRepGraph_EdgeId       anEdgeId = aGraph.Editor().Edges().Add(aV1, aV2, aLine, 0.0, 10.0, 0.001);

  EXPECT_TRUE(anEdgeId.IsValid());

  const BRepGraphInc::EdgeDef& anEdgeDef =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start());
  EXPECT_EQ(aGraph.Refs()
              .Vertices()
              .Entry(BRepGraph_Tool::Edge::StartVertexId(aGraph, BRepGraph_EdgeId::Start()))
              .ChildVertexId,
            aV1);
  EXPECT_EQ(aGraph.Refs()
              .Vertices()
              .Entry(BRepGraph_Tool::Edge::EndVertexId(aGraph, BRepGraph_EdgeId::Start()))
              .ChildVertexId,
            aV2);
  EXPECT_TRUE(anEdgeDef.Curve3DRepId.IsValid());
  const std::pair<double, double> anEdgeRange =
    BRepGraph_Tool::Edge::Range(aGraph, BRepGraph_EdgeId::Start());
  EXPECT_NEAR(anEdgeRange.first, 0.0, 1e-10);
  EXPECT_NEAR(anEdgeRange.second, 10.0, 1e-10);
}

TEST(BRepGraph_ShapesViewImportTest, AddWire_ClosedRectangle)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Editor().Edges().Add(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Editor().Edges().Add(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Editor().Edges().Add(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Editor().Edges().Add(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  EXPECT_TRUE(aWireId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountCoEdgesOfWire(aGraph, BRepGraph_WireId::Start()), 4);
  EXPECT_TRUE(BRepGraph_Tool::Wire::IsClosed(aGraph, BRepGraph_WireId::Start()));
}

TEST(BRepGraph_ShapesViewImportTest, AddFace_WithSurface)
{
  BRepGraph aGraph;

  // Build a simple rectangular face programmatically.
  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Editor().Edges().Add(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Editor().Edges().Add(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Editor().Edges().Add(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Editor().Edges().Add(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());

  occ::handle<Geom_Plane>                    aPlane = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId                           aFaceId =
    aGraph.Editor().Faces().Add(aPlane, aWireId, aInnerWires.ToArray1(), 0.001);

  EXPECT_TRUE(aFaceId.IsValid());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);

  const BRepGraphInc::FaceDef& aFaceDef =
    aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start());
  EXPECT_TRUE(aFaceDef.SurfaceRepId.IsValid());
}

TEST(BRepGraph_ShapesViewImportTest, AddWireToFace_AppendsInnerWireRefAndRelations)
{
  BRepGraph aGraph;

  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Editor().Edges().Add(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Editor().Edges().Add(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Editor().Edges().Add(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Editor().Edges().Add(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aOuterCoEdges;
  aOuterCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aOuterCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aOuterCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aOuterCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  NCollection_LinearVector<BRepGraph_CoEdgeId> aInnerCoEdges;
  aInnerCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aInnerCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aInnerCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aInnerCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  const BRepGraph_WireId aOuterWire  = aGraph.Editor().Wires().Add(aOuterCoEdges.ToArray1());
  const BRepGraph_WireId anInnerWire = aGraph.Editor().Wires().Add(aInnerCoEdges.ToArray1());

  occ::handle<Geom_Plane>                    aPlane = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId                     aFaceId =
    aGraph.Editor().Faces().Add(aPlane, aOuterWire, anInnerWires.ToArray1(), 0.001);

  const BRepGraph_WireRefId aWireRef =
    aGraph.Editor().Faces().Append(aFaceId, anInnerWire, TopAbs_FORWARD);
  ASSERT_TRUE(aWireRef.IsValid());

  const BRepGraphInc::FaceRelations& aFaceRelations = aGraph.Topo().Faces().Relations(aFaceId);
  EXPECT_EQ(aFaceRelations.WireRefIds.Size(), 2);
  EXPECT_EQ(aFaceRelations.WireRefIds.Value(1), aWireRef);

  const BRepGraphInc::WireRef& aRef = aGraph.Refs().Wires().Entry(aWireRef);
  EXPECT_EQ(aRef.ChildWireId, anInnerWire);

  EXPECT_EQ(aGraph.Topo().Wires().Relations(anInnerWire).ParentWireRefIds.Size(), 1);
}

TEST(BRepGraph_ShapesViewImportTest, AddEdge_InvalidVertex_ReturnsInvalidAndDoesNotAppend)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aVertexId = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 0.001);

  const BRepGraph_EdgeId anEdgeId = aGraph.Editor().Edges().Add(aVertexId,
                                                                BRepGraph_VertexId(42),
                                                                occ::handle<Geom_Curve>(),
                                                                0.0,
                                                                1.0,
                                                                0.001);

  EXPECT_FALSE(anEdgeId.IsValid());
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
}

TEST(BRepGraph_ShapesViewImportTest, AddWire_InvalidEdge_ReturnsInvalidAndDoesNotAppend)
{
  BRepGraph aGraph;

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(BRepGraph_EdgeId(17), TopAbs_FORWARD));

  const BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  EXPECT_FALSE(aWireId.IsValid());
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().CoEdges().Nb(), 0);
}

TEST(BRepGraph_ShapesViewImportTest, AddFace_InvalidOuterWire_ReturnsInvalidAndDoesNotAppend)
{
  BRepGraph aGraph;

  occ::handle<Geom_Plane>                    aPlane = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId                     aFaceId =
    aGraph.Editor().Faces().Add(aPlane, BRepGraph_WireId(9), anInnerWires.ToArray1(), 0.001);

  EXPECT_FALSE(aFaceId.IsValid());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
}

TEST(BRepGraph_ShapesViewImportTest, AddShellAndSolid)
{
  BRepGraph         aGraph;
  BRepGraph_ShellId aShellId = aGraph.Editor().Shells().Add();
  EXPECT_TRUE(aShellId.IsValid());

  BRepGraph_SolidId aSolidId = aGraph.Editor().Solids().Add();
  EXPECT_TRUE(aSolidId.IsValid());
}

// ============================================================
// Incremental Build (flattened Add)
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, AppendTwoBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Get two faces from the box.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Shape        aFace1 = anExp.Current();
  BRepBuilderAPI_Copy aCopy1(aFace1, true);
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  TopoDS_Shape        aFace2 = anExp.Current();
  BRepBuilderAPI_Copy aCopy2(aFace2, true);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
    aGraph.Shapes().Add(aCopy1.Shape());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);

  // Append second face.
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    aGraph.Shapes().Add(aCopy2.Shape(), BRepGraph::ShapesView::Options{{}, false, true, false});
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);
}

// ============================================================
// Task 2C: Soft Node Removal
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, RemoveVertex_IsRemoved)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aVtxId = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aVtxId));

  aGraph.Editor().Gen().RemoveNode(aVtxId);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aVtxId));
}

TEST(BRepGraph_ShapesViewImportTest, RemoveFaceFromBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aBox);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  BRepGraph_FaceId aFaceId(0);
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aFaceId));

  aGraph.Editor().Gen().RemoveNode(aFaceId);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aFaceId));

  // Other faces should not be removed.
  const uint32_t aNbFaces = aGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId anOtherFaceId(1); anOtherFaceId.IsValid(aNbFaces); ++anOtherFaceId)
  {
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(anOtherFaceId));
  }
}

TEST(BRepGraph_ShapesViewImportTest, RemoveInvalidNode_NoError)
{
  BRepGraph        aGraph;
  BRepGraph_NodeId anInvalidId;
  EXPECT_FALSE(aGraph.Topo().Gen().IsValid(anInvalidId));
  EXPECT_FALSE(aGraph.Topo().Gen().IsActive(anInvalidId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(anInvalidId));
  aGraph.Editor().Gen().RemoveNode(anInvalidId); // Should not crash.
}

TEST(BRepGraph_ShapesViewImportTest, RemoveAlreadyRemovedNode_NoError)
{
  BRepGraph aGraph;

  const BRepGraph_VertexId aVertexId = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 0.001);
  ASSERT_TRUE(aVertexId.IsValid());

  aGraph.Editor().Gen().RemoveNode(aVertexId);
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aVertexId));

  EXPECT_NO_THROW(aGraph.Editor().Gen().RemoveNode(aVertexId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aVertexId));
}

// ============================================================
// Item 1: Complete Construction API (Shell/Solid linking)
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, AddFace_CreatesUsage)
{
  BRepGraph aGraph;

  // Build a face programmatically.
  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Editor().Edges().Add(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Editor().Edges().Add(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Editor().Edges().Add(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Editor().Edges().Add(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  BRepGraph_WireId        aWireId = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  occ::handle<Geom_Plane> aPlane  = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId                           aFaceId =
    aGraph.Editor().Faces().Add(aPlane, aWireId, aInnerWires.ToArray1(), 0.001);

  // Create shell and link face to it.
  BRepGraph_ShellId         aShellId   = aGraph.Editor().Shells().Add();
  const BRepGraph_FaceRefId aFaceRefId = aGraph.Editor().Shells().Append(aShellId, aFaceId);
  EXPECT_TRUE(aFaceRefId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(aGraph, BRepGraph_ShellId::Start()), 1);
}

TEST(BRepGraph_ShapesViewImportTest, AddFace_InvalidNodes_NoMutation)
{
  BRepGraph aGraph;

  const BRepGraph_ShellId   aShellId = aGraph.Editor().Shells().Add();
  const BRepGraph_FaceRefId aFaceRefId =
    aGraph.Editor().Shells().Append(aShellId, BRepGraph_FaceId(4));
  EXPECT_FALSE(aFaceRefId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(aGraph, aShellId), 0);
}

TEST(BRepGraph_ShapesViewImportTest, AddShell_CreatesUsage)
{
  BRepGraph aGraph;

  BRepGraph_ShellId aShellId = aGraph.Editor().Shells().Add();
  BRepGraph_SolidId aSolidId = aGraph.Editor().Solids().Add();

  const BRepGraph_ShellRefId aShellRefId = aGraph.Editor().Solids().Append(aSolidId, aShellId);
  EXPECT_TRUE(aShellRefId.IsValid());

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(aGraph, BRepGraph_SolidId::Start()), 1);
}

TEST(BRepGraph_ShapesViewImportTest, MutInvalidTopologyDefs_ThrowProgramError)
{
  BRepGraph aGraph;
#if !defined(No_Exception)
  EXPECT_THROW(std::ignore = aGraph.Editor().Vertices().Mut(BRepGraph_VertexId(7)),
               Standard_ProgramError);
  EXPECT_THROW(std::ignore = aGraph.Editor().Edges().Mut(BRepGraph_EdgeId(7)),
               Standard_ProgramError);
  EXPECT_THROW(std::ignore = aGraph.Editor().Wires().Mut(BRepGraph_WireId(7)),
               Standard_ProgramError);
  EXPECT_THROW(std::ignore = aGraph.Editor().Faces().Mut(BRepGraph_FaceId(7)),
               Standard_ProgramError);
  EXPECT_THROW(std::ignore = aGraph.Editor().Shells().Mut(BRepGraph_ShellId(7)),
               Standard_ProgramError);
  EXPECT_THROW(std::ignore = aGraph.Editor().Solids().Mut(BRepGraph_SolidId(7)),
               Standard_ProgramError);
  EXPECT_THROW(std::ignore = aGraph.Editor().CoEdges().Mut(BRepGraph_CoEdgeId(7)),
               Standard_ProgramError);
#endif
}

TEST(BRepGraph_ShapesViewImportTest, AddCompound_WithChildren)
{
  BRepGraph aGraph;

  BRepGraph_SolidId aSolid1 = aGraph.Editor().Solids().Add();
  BRepGraph_SolidId aSolid2 = aGraph.Editor().Solids().Add();

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aSolid1);
  aChildren.Append(aSolid2);

  BRepGraph_CompoundId aCompId = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  EXPECT_TRUE(aCompId.IsValid());
  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1);

  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(aGraph, BRepGraph_CompoundId::Start()), 2);
}

TEST(BRepGraph_ShapesViewImportTest, AddCompSolid_WithSolids)
{
  BRepGraph aGraph;

  BRepGraph_SolidId aSolid1 = aGraph.Editor().Solids().Add();
  BRepGraph_SolidId aSolid2 = aGraph.Editor().Solids().Add();

  NCollection_LinearVector<BRepGraph_SolidId> aSolids;
  aSolids.Append(aSolid1);
  aSolids.Append(aSolid2);

  BRepGraph_CompSolidId aCSolId = aGraph.Editor().CompSolids().Add(aSolids.ToArray1());
  EXPECT_TRUE(aCSolId.IsValid());
  EXPECT_EQ(aGraph.Topo().CompSolids().Nb(), 1);

  const BRepGraphInc::CompSolidDef& aCSolDef =
    aGraph.Topo().CompSolids().Definition(BRepGraph_CompSolidId::Start());
  std::ignore = aCSolDef;
  EXPECT_EQ(BRepGraph_TestTools::CountSolidRefsOfCompSolid(aGraph, BRepGraph_CompSolidId::Start()),
            2);
}

TEST(BRepGraph_ShapesViewImportTest, FullSolid_ProgrammaticConstruction)
{
  BRepGraph aGraph;

  // Build a single-face shell solid.
  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Editor().Edges().Add(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Editor().Edges().Add(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Editor().Edges().Add(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Editor().Edges().Add(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  BRepGraph_WireId        aWireId = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  occ::handle<Geom_Plane> aPlane  = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId                           aFaceId =
    aGraph.Editor().Faces().Add(aPlane, aWireId, aInnerWires.ToArray1(), 0.001);

  BRepGraph_ShellId aShellId = aGraph.Editor().Shells().Add();
  aGraph.Editor().Shells().Append(aShellId, aFaceId);

  BRepGraph_SolidId aSolidId = aGraph.Editor().Solids().Add();
  aGraph.Editor().Solids().Append(aSolidId, aShellId);

  // Verify the hierarchy.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(aGraph, BRepGraph_SolidId::Start()), 1);
}

// ============================================================
// Item 2: Field-Level Mutation via Editor() for All Def Types
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, MutableFaceDefinition_ChangesTolerance)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aBox);
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const double anOrigTol = BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId::Start());
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFaceDef =
      aGraph.Editor().Faces().Mut(BRepGraph_FaceId::Start());
    aGraph.Editor().Faces().SetTolerance(aFaceDef, 0.5);
  }
  EXPECT_NEAR(BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId::Start()), 0.5, 1e-10);
  EXPECT_GT(aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).OwnGen, 0u);
  std::ignore = anOrigTol;
}

TEST(BRepGraph_ShapesViewImportTest, MutableShellDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aBox);
  ASSERT_GT(aGraph.Topo().Shells().Nb(), 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::ShellDef> aShellDef =
      aGraph.Editor().Shells().Mut(BRepGraph_ShellId::Start());
    aShellDef.MarkDirty();
  }
  EXPECT_GT(aGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).OwnGen, 0u);
}

TEST(BRepGraph_ShapesViewImportTest, MutableSolidDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 = aGraph.Shapes().Add(aBox);
  ASSERT_GT(aGraph.Topo().Solids().Nb(), 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::SolidDef> aSolidDef =
      aGraph.Editor().Solids().Mut(BRepGraph_SolidId::Start());
    aSolidDef.MarkDirty();
  }
  EXPECT_GT(aGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).OwnGen, 0u);
}

TEST(BRepGraph_ShapesViewImportTest, MutableCompoundDefinition)
{
  BRepGraph                                  aGraph;
  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  std::ignore = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_EQ(aGraph.Topo().Compounds().Nb(), 1);

  {
    BRepGraph_MutGuard<BRepGraphInc::CompoundDef> aCompDef =
      aGraph.Editor().Compounds().Mut(BRepGraph_CompoundId::Start());
    aCompDef.MarkDirty();
  }
  EXPECT_GT(aGraph.Topo().Compounds().Definition(BRepGraph_CompoundId::Start()).OwnGen, 0u);
}

TEST(BRepGraph_ShapesViewImportTest, MutableCompSolidDefinition)
{
  BRepGraph                                   aGraph;
  NCollection_LinearVector<BRepGraph_SolidId> aSolids;
  std::ignore = aGraph.Editor().CompSolids().Add(aSolids.ToArray1());
  ASSERT_EQ(aGraph.Topo().CompSolids().Nb(), 1);

  {
    BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> aCSolDef =
      aGraph.Editor().CompSolids().Mut(BRepGraph_CompSolidId::Start());
    aCSolDef.MarkDirty();
  }
  EXPECT_GT(aGraph.Topo().CompSolids().Definition(BRepGraph_CompSolidId::Start()).OwnGen, 0u);
}

// ============================================================
// Item 3: Definition Traversal Skips Removed Nodes
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, SkipsRemovedFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 = aGraph.Shapes().Add(aBox);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Remove 2 faces.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 3));

  uint32_t aCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_FALSE(aFaceIt.CurrentId().IsRemoved(aGraph));
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraph_ShapesViewImportTest, SkipsRemovedEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 = aGraph.Shapes().Add(aBox);
  const uint32_t                                       aNbEdges   = aGraph.Topo().Edges().Nb();
  ASSERT_GT(aNbEdges, 0);

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0));

  uint32_t aCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_FALSE(anEdgeIt.CurrentId().IsRemoved(aGraph));
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbEdges - 1);
}

TEST(BRepGraph_ShapesViewImportTest, SkipsFirstNode)
{
  BRepGraph aGraph;
  std::ignore = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  std::ignore = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 0.001);
  std::ignore = aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 0.001);

  // Remove the first vertex.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, 0));

  uint32_t aCount = 0;
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    EXPECT_FALSE(aVertexIt.CurrentId().IsRemoved(aGraph));
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

// ============================================================
// Item 4: Cascading Soft Removal
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, RemoveFace_RemovesWiresAndEdges)
{
  BRepGraph aGraph;

  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);
  BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 10, 0), 0.001);
  BRepGraph_VertexId aV3 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 10, 0), 0.001);

  occ::handle<Geom_Line> aL0 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(10, 10, 0), gp_Dir(-1, 0, 0));
  occ::handle<Geom_Line> aL3 = new Geom_Line(gp_Pnt(0, 10, 0), gp_Dir(0, -1, 0));

  BRepGraph_EdgeId aE0 = aGraph.Editor().Edges().Add(aV0, aV1, aL0, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE1 = aGraph.Editor().Edges().Add(aV1, aV2, aL1, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE2 = aGraph.Editor().Edges().Add(aV2, aV3, aL2, 0.0, 10.0, 0.001);
  BRepGraph_EdgeId aE3 = aGraph.Editor().Edges().Add(aV3, aV0, aL3, 0.0, 10.0, 0.001);

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE0, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE1, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE2, TopAbs_FORWARD));
  aCoEdges.Append(aGraph.Editor().CoEdges().Add(aE3, TopAbs_FORWARD));

  BRepGraph_WireId        aWireId = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  occ::handle<Geom_Plane> aPlane  = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> aInnerWires;
  BRepGraph_FaceId                           aFaceId =
    aGraph.Editor().Faces().Add(aPlane, aWireId, aInnerWires.ToArray1(), 0.001);

  // Remove the face subgraph.
  aGraph.Editor().Gen().RemoveSubgraph(aFaceId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aFaceId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aWireId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE0));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE1));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE2));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aE3));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV0));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV1));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV2));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aV3));
}

TEST(BRepGraph_ShapesViewImportTest, RemoveSolid_CascadesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 = aGraph.Shapes().Add(aBox);

  BRepGraph_SolidId aSolidId(0);
  aGraph.Editor().Gen().RemoveSubgraph(aSolidId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aSolidId));

  // All shells should be removed.
  const uint32_t aNbShells = aGraph.Topo().Shells().Nb();
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShells); ++aShellId)
  {
    EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aShellId));
  }

  // All faces should be removed.
  const uint32_t aNbFaces = aGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aFaceId));
  }
}

TEST(BRepGraph_ShapesViewImportTest, RemoveSubgraph_SharedFace_PreservesSharedEdgesAndVertices)
{
  // Build a box (6 faces). Remove one face. Shared edges and vertices must
  // remain active because other faces still reference them.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 = aGraph.Shapes().Add(aBox);

  const uint32_t aNbFaces    = aGraph.Topo().Faces().Nb();
  const uint32_t aNbEdges    = aGraph.Topo().Edges().Nb();
  const uint32_t aNbVertices = aGraph.Topo().Vertices().Nb();
  ASSERT_EQ(aNbFaces, 6);
  ASSERT_EQ(aNbEdges, 12);
  ASSERT_EQ(aNbVertices, 8);

  // Remove face 0.
  const BRepGraph_FaceId aRemovedFace(0);
  aGraph.Editor().Gen().RemoveSubgraph(aRemovedFace);

  // The removed face must be removed.
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aRemovedFace));

  // Other faces must remain active.
  for (uint32_t aIdx = 1; aIdx < aNbFaces; ++aIdx)
  {
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(BRepGraph_FaceId(aIdx)));
  }

  // All 12 edges of a box are shared by exactly 2 faces.
  // Removing 1 face leaves each shared edge with at least 1 active parent face.
  // Therefore all edges must remain active.
  for (uint32_t aIdx = 0; aIdx < aNbEdges; ++aIdx)
  {
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(BRepGraph_EdgeId(aIdx)));
  }

  // All 8 vertices of a box are shared by 3 edges. All edges are still active,
  // so all vertices must remain active.
  for (uint32_t aIdx = 0; aIdx < aNbVertices; ++aIdx)
  {
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(BRepGraph_VertexId(aIdx)));
  }
}

// ============================================================
// Item 5: Edge Adjacency Queries
// ============================================================

TEST(BRepGraph_ShapesViewImportTest, FacesOfEdge_BoxSharedEdge)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 = aGraph.Shapes().Add(aBox);

  // Every edge in a box is shared by exactly 2 faces.
  const uint32_t aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const uint32_t aNbFaces = countIterator(aGraph.Topo().Edges().FacesOf(anEdgeId));
    EXPECT_EQ(aNbFaces, 2) << "Edge " << anEdgeId.Index << " has " << aNbFaces << " faces";
  }
}

TEST(BRepGraph_ShapesViewImportTest, SharedEdges_AdjacentBoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 = aGraph.Shapes().Add(aBox);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Count total shared edge pairs across all face pairs.
  int            aSharingPairs = 0;
  const uint32_t aNbFaces      = aGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceA(0); aFaceA.IsValid(aNbFaces); ++aFaceA)
  {
    for (BRepGraph_FaceId aFaceB(aFaceA.Index + 1); aFaceB.IsValid(aNbFaces); ++aFaceB)
    {
      if (countSharedEdges(aGraph, aFaceA, aFaceB) > 0)
      {
        ++aSharingPairs;
      }
    }
  }
  // A box has 12 edges, each shared by 2 faces, so 12 sharing pairs.
  EXPECT_EQ(aSharingPairs, 12);
}

TEST(BRepGraph_ShapesViewImportTest, AdjacentFaces_BoxFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 = aGraph.Shapes().Add(aBox);
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Each face of a box is adjacent to 4 other faces.
  const uint32_t aNbFaces = aGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const uint32_t aNbAdjacentFaces = countAdjacentFaces(aGraph, aFaceId);
    EXPECT_EQ(aNbAdjacentFaces, 4)
      << "Face " << aFaceId.Index << " has " << aNbAdjacentFaces << " adjacent faces";
  }
}

TEST(BRepGraph_ShapesViewImportTest, FacesOfEdge_NoFaces_Programmatic)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 0.001);
  BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(10, 0, 0), 0.001);

  occ::handle<Geom_Line> aLine    = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  BRepGraph_EdgeId       anEdgeId = aGraph.Editor().Edges().Add(aV0, aV1, aLine, 0.0, 10.0, 0.001);

  // Edge not in any face => empty result.
  EXPECT_EQ(countIterator(aGraph.Topo().Edges().FacesOf(anEdgeId)), 0);
}

// ============ Topology adjacency methods ============

TEST(BRepGraph_ShapesViewImportTest, EdgesOfFace_Box_HasEdges)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  // Each box face has 4 edges (rectangular loop).
  uint32_t aNbEdges = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_FaceId::Start(),
                                     BRepGraph_NodeId::Kind::Edge);
       anExp.More();
       anExp.Next())
  {
    ++aNbEdges;
  }
  EXPECT_EQ(aNbEdges, 4);
}

TEST(BRepGraph_ShapesViewImportTest, VerticesOfEdge_Box_HasTwoVertices)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  uint32_t aNbVertices = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_EdgeId::Start(),
                                     BRepGraph_NodeId::Kind::Vertex);
       anExp.More();
       anExp.Next())
  {
    ++aNbVertices;
  }
  EXPECT_EQ(aNbVertices, 2);
}

TEST(BRepGraph_ShapesViewImportTest, EdgesOfVertex_Box_ThreeEdges)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  // Each box corner vertex is shared by 3 edges.
  const NCollection_LinearVector<BRepGraph_EdgeId>& aEdges =
    aGraph.Topo().Vertices().Edges(BRepGraph_VertexId::Start());
  EXPECT_EQ(aEdges.Size(), 3);
}

TEST(BRepGraph_ShapesViewImportTest, AdjacentEdges_Box_SharedVertex)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes17 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  // Box edge shares 2 vertices, each with 3 incident edges.
  // Adjacent = (3 - 1) + (3 - 1) - overlap = at least 4 adjacent edges.
  EXPECT_GE(countAdjacentEdgesOfEdge(aGraph, BRepGraph_EdgeId::Start()), 4);
}

TEST(BRepGraph_ShapesViewImportTest, NbFacesOfEdge_Box_TwoFaces)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes18 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  // Every box edge is shared by exactly 2 faces (manifold).
  EXPECT_EQ(aGraph.Topo().Edges().NbFaces(BRepGraph_EdgeId::Start()), 2);
}

TEST(BRepGraph_ShapesViewImportTest, IsManifoldEdge_Box_True)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes19 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  EXPECT_TRUE(BRepGraph_Tool::Edge::IsManifold(aGraph, BRepGraph_EdgeId::Start()));
  EXPECT_FALSE(BRepGraph_Tool::Edge::IsBoundary(aGraph, BRepGraph_EdgeId::Start()));
}

TEST(BRepGraph_ShapesViewImportTest, InvalidInput_ReturnsEmpty)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes20 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  // Out-of-range typed ids return empty results.
  EXPECT_EQ(aGraph.Topo().Vertices().Edges(BRepGraph_VertexId(999)).Size(), 0);
  EXPECT_EQ(countAdjacentEdgesOfEdge(aGraph, BRepGraph_EdgeId(999)), 0);
}

TEST(BRepGraph_ShapesViewImportTest, AddWithHistory_PreservesRequestedAddedNodes)
{
  BRepGraph aGraph;
  aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);

  BRepGraph::ShapesView::Options anOptions;
  anOptions.TrackAddedNodes = true;

  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  const TopoDS_Shape                  aBox = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().AddWithHistory(aBox,
                                   anInputs,
                                   occ::handle<BRepTools_History>(),
                                   TCollection_AsciiString("Test:DisabledHistory"),
                                   anOptions);

  ASSERT_TRUE(aResult.IsOk());
  EXPECT_GT(aResult.AddedNodes.Extent(), 0);

  NCollection_LinearVector<BRepGraph_NodeId> aRoots;
  aRoots.Append(aResult.TopologyRoot);
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> aInputsFromRoots;
  aGraph.Shapes().CollectHistoryInputs(aRoots.ToArray1(), aInputsFromRoots);
  EXPECT_GT(aInputsFromRoots.Extent(), 0);

  EXPECT_EQ(aGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), 0);
}

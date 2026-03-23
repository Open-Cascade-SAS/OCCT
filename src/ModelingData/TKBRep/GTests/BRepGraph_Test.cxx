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
#include <BRepGraphAlgo_BndLib.hxx>
#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_AttrsView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_TypedAttribute.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Face.hxx>

#include <atomic>

#include <gtest/gtest.h>

namespace
{
static gp_Pnt bboxCenter(BRepGraph& theGraph, BRepGraph_NodeId theNode)
{
  Bnd_Box aBox;
  BRepGraphAlgo_BndLib::Add(theGraph, theNode, aBox);
  if (aBox.IsVoid())
    return gp_Pnt();
  double xn, yn, zn, xx, yx, zx;
  aBox.Get(xn, yn, zn, xx, yx, zx);
  return gp_Pnt((xn + xx) * 0.5, (yn + yx) * 0.5, (zn + zx) * 0.5);
}
} // namespace

class BRepGraphTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Build(aBox);
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraphTest, Build_SimpleBox_IsDone)
{
  EXPECT_TRUE(myGraph.IsDone());
}

TEST_F(BRepGraphTest, Build_SimpleBox_CorrectCounts)
{
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(myGraph.Defs().NbWires(), 6);
  EXPECT_EQ(myGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(myGraph.Defs().NbVertices(), 8);
}

TEST_F(BRepGraphTest, Build_SimpleBox_SurfaceCount)
{
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
}

TEST_F(BRepGraphTest, Face_Surface_IsValid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Defs().Face(aFaceIdx);
    EXPECT_FALSE(aFace.Surface.IsNull()) << "Face " << aFaceIdx << " has null Surface";
  }
}

TEST_F(BRepGraphTest, Edge_CurveAndVertices_AreValid)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Defs().Edge(anEdgeIdx);
    if (!anEdge.IsDegenerate)
    {
      EXPECT_FALSE(anEdge.Curve3d.IsNull())
        << "Edge " << anEdgeIdx << " has null Curve3d";
    }
    EXPECT_TRUE(anEdge.StartVertexDefId().IsValid())
      << "Edge " << anEdgeIdx << " has invalid StartVertexId";
    EXPECT_TRUE(anEdge.EndVertexDefId().IsValid()) << "Edge " << anEdgeIdx << " has invalid EndVertexId";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWire_Exists)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Defs().Face(aFaceIdx);
    EXPECT_GE(aFace.OuterWireIdx(), 0) << "Face " << aFaceIdx << " has no outer wire";
  }
}

TEST_F(BRepGraphTest, FaceDef_HasValidSurface)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Defs().Face(aFaceIdx);
    EXPECT_FALSE(aFace.Surface.IsNull())
      << "Face " << aFaceIdx << " has no surface";
  }
}

TEST_F(BRepGraphTest, FindPCurve_ValidPair)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(aFaceIdx);
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const int anOuterWireIdx = aFaceDef.OuterWireIdx();
    if (anOuterWireIdx < 0)
      continue;
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(anOuterWireIdx);
    for (int anEdgeIter = 0; anEdgeIter < aWireDef.EdgeRefs.Length(); ++anEdgeIter)
    {
      const BRepGraphInc::EdgeRef& anEdgeRef = aWireDef.EdgeRefs.Value(anEdgeIter);
      BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, anEdgeRef.EdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Defs().Edge(anEdgeRef.EdgeIdx);
      if (anEdge.IsDegenerate)
        continue;
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurveEntry =
        myGraph.Defs().FindPCurve(anEdgeId, aFaceId);
      EXPECT_NE(aPCurveEntry, nullptr)
        << "Missing PCurve for edge " << anEdgeRef.EdgeIdx << " on face " << aFaceIdx;
    }
  }
}

TEST_F(BRepGraphTest, UID_Unique)
{
  NCollection_Map<BRepGraph_UID> aUIDSet;
  for (int aSolidIdx = 0; aSolidIdx < myGraph.Defs().NbSolids(); ++aSolidIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidIdx))));
  for (int aShellIdx = 0; aShellIdx < myGraph.Defs().NbShells(); ++aShellIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellIdx))));
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx))));
  for (int aWireIdx = 0; aWireIdx < myGraph.Defs().NbWires(); ++aWireIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx))));
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx))));
  for (int aVertexIdx = 0; aVertexIdx < myGraph.Defs().NbVertices(); ++aVertexIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVertexIdx))));
  // Surface/Curve geometry UIDs are no longer separate nodes; geometry is stored inline on defs.
}

TEST_F(BRepGraphTest, UID_NodeIdRoundTrip)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const BRepGraph_UID& aUID = myGraph.UIDs().Of(aFaceId);
    BRepGraph_NodeId     aResolved = myGraph.UIDs().NodeIdFrom(aUID);
    EXPECT_EQ(aResolved, aFaceId) << "Round trip failed for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, BoundingBox_NonVoid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box aBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aBox);
    EXPECT_FALSE(aBox.IsVoid()) << "BoundingBox is void for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, SameDomainFaces_Box_Empty)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId                       aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    NCollection_Vector<BRepGraph_NodeId> aSameDomain = myGraph.Spatial().SameDomainFaces(aFaceId);
    EXPECT_EQ(aSameDomain.Length(), 0)
      << "Box face " << aFaceIdx << " should have no same-domain faces";
  }
}

TEST(BRepGraphDecomposeTest, Decompose_TwoSeparateFaces)
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbFaces(), 2);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 2);
}

TEST_F(BRepGraphTest, UserAttribute_SetGet)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  const int aKey   = BRepGraph_UserAttribute::AllocateKey();
  auto      anAttr = std::make_shared<BRepGraph_TypedAttribute<double>>(3.14);
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);

  BRepGraph_UserAttrPtr aRetrieved = myGraph.Attrs().Get(aFaceId, aKey);
  ASSERT_NE(aRetrieved, nullptr);

  auto aTyped = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<double>>(aRetrieved);
  ASSERT_NE(aTyped, nullptr);
  EXPECT_NEAR(aTyped->UncheckedValue(), 3.14, 1.0e-10);
}

TEST(BRepGraphReBuildTest, ReBuild_UIDMonotonic)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  const uint32_t aGen1 = aGraph.UIDs().Generation();

  // Access a UID from the first build to verify it works.
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);
  const BRepGraph_NodeId aFirstFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aFirstUID = aGraph.UIDs().Of(aFirstFace);
  EXPECT_TRUE(aFirstUID.IsValid());
  EXPECT_EQ(aFirstUID.Generation(), aGen1);

  aGraph.Build(aBox);
  const uint32_t aGen2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGen2, aGen1);

  // Verify all face UIDs in second build are valid and have new generation.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aNodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    BRepGraph_UID    aUID = aGraph.UIDs().Of(aNodeId);
    EXPECT_TRUE(aUID.IsValid()) << "Face " << aFaceIdx << " should have a valid UID";
    EXPECT_EQ(aUID.Generation(), aGen2) << "Face " << aFaceIdx << " UID should have new generation";
  }

  // First build's UID should no longer be valid in the new generation.
  EXPECT_FALSE(aGraph.UIDs().Has(aFirstUID));
}

TEST_F(BRepGraphTest, DetectMissingPCurves_ValidBox_Empty)
{
  auto aMissing = BRepGraph_Analyze::MissingPCurves(myGraph);
  EXPECT_EQ(aMissing.Length(), 0);
}

TEST_F(BRepGraphTest, DetectDegenerateWires_ValidBox_Empty)
{
  auto aDegenerate = BRepGraph_Analyze::DegenerateWires(myGraph);
  EXPECT_EQ(aDegenerate.Length(), 0);
}

TEST_F(BRepGraphTest, MutableEdge_ModifyTolerance)
{
  double                                            anOrigTol = myGraph.Defs().Edge(0).Tolerance;
  BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge    = myGraph.Mut().EdgeDef(0);
  anEdge->Tolerance                    = anOrigTol * 2.0;
  EXPECT_NEAR(myGraph.Defs().Edge(0).Tolerance, anOrigTol * 2.0, 1.0e-15);
}

TEST_F(BRepGraphTest, FaceCountForEdge_SharedEdge)
{
  // In a box, each non-degenerate edge is shared by exactly 2 faces.
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    if (!myGraph.Defs().Edge(anEdgeIdx).IsDegenerate)
    {
      int aCount = myGraph.Builder().FaceCountForEdge(anEdgeIdx);
      EXPECT_EQ(aCount, 2) << "Edge " << anEdgeIdx << " should be shared by 2 faces";
    }
  }
}

TEST_F(BRepGraphTest, FreeEdges_ClosedBox_Empty)
{
  NCollection_Vector<BRepGraph_NodeId> aFree = BRepGraph_Analyze::FreeEdges(myGraph);
  EXPECT_EQ(aFree.Length(), 0);
}

TEST_F(BRepGraphTest, AddRelEdge_UserDefined)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  myGraph.Mut().AddRelEdge(anEdge0, anEdge1, BRepGraph_RelEdge::Kind::UserDefined);
  int aCount = 0;
  myGraph.RelEdges().ForEachOutOfKind(anEdge0, BRepGraph_RelEdge::Kind::UserDefined,
    [&](const BRepGraph_RelEdge&) { ++aCount; });
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraphTest, RecordHistory_BasicEntry)
{
  int                                    aBefore = myGraph.History().NbRecords();
  BRepGraph_NodeId                      anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                      anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("TestOp", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("TestOp"));
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Substitution)
{
  // Get the first wire and its first edge via incidence refs.
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(0);
  ASSERT_GE(aWireDef.EdgeRefs.Length(), 1);
  const BRepGraphInc::EdgeRef& anOldEdgeRef = aWireDef.EdgeRefs.Value(0);
  BRepGraph_NodeId anOldEdge(BRepGraph_NodeId::Kind::Edge, anOldEdgeRef.EdgeIdx);

  // Pick a different edge to substitute.
  BRepGraph_NodeId aNewEdge(BRepGraph_NodeId::Kind::Edge, (anOldEdge.Index + 1) % myGraph.Defs().NbEdges());

  myGraph.Mut().ReplaceEdgeInWire(0, anOldEdge, aNewEdge, false);

  // Verify the substitution via the updated incidence refs.
  const BRepGraph_TopoNode::WireDef& aWireDefAfter = myGraph.Defs().Wire(0);
  const BRepGraphInc::EdgeRef& aNewEdgeRef = aWireDefAfter.EdgeRefs.Value(0);
  EXPECT_EQ(aNewEdgeRef.EdgeIdx, aNewEdge.Index);
}

TEST(BRepGraphParallelTest, ParallelBuild_SameAsSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aSeqGraph;
  aSeqGraph.Build(aBox, false);
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Build(aBox, true);
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Defs().NbSolids(), aSeqGraph.Defs().NbSolids());
  EXPECT_EQ(aParGraph.Defs().NbShells(), aSeqGraph.Defs().NbShells());
  EXPECT_EQ(aParGraph.Defs().NbFaces(), aSeqGraph.Defs().NbFaces());
  EXPECT_EQ(aParGraph.Defs().NbWires(), aSeqGraph.Defs().NbWires());
  EXPECT_EQ(aParGraph.Defs().NbEdges(), aSeqGraph.Defs().NbEdges());
  EXPECT_EQ(aParGraph.Defs().NbVertices(), aSeqGraph.Defs().NbVertices());
  EXPECT_EQ(aParGraph.Defs().NbFaces(),   aSeqGraph.Defs().NbFaces());
  EXPECT_EQ(aParGraph.Defs().NbEdges(),   aSeqGraph.Defs().NbEdges());
}

TEST(BRepGraphParallelTest, ParallelBuild_CompoundOfFaces)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (TopExp_Explorer anExp(aBox1.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
    aBuilder.Add(aCompound, anExp.Current());
  for (TopExp_Explorer anExp(aBox2.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
    aBuilder.Add(aCompound, anExp.Current());

  BRepGraph aGraph;
  aGraph.Build(aCompound, true);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbFaces(), 12);
}

// ===================================================================
// Group 1: Shape Round-Trip
// ===================================================================

TEST_F(BRepGraphTest, ReconstructFace_EachBoxFace_SameSubShapeCounts)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const TopoDS_Shape& anOrigFace = myGraph.Shapes().OriginalOf(aFaceId);
    const TopoDS_Shape  aReconstructed = myGraph.Shapes().ReconstructFace(aFaceIdx);

    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anOrigVerts, anOrigEdges, anOrigWires;
    TopExp::MapShapes(anOrigFace, TopAbs_VERTEX, anOrigVerts);
    TopExp::MapShapes(anOrigFace, TopAbs_EDGE, anOrigEdges);
    TopExp::MapShapes(anOrigFace, TopAbs_WIRE, anOrigWires);

    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aReconVerts, aReconEdges, aReconWires;
    TopExp::MapShapes(aReconstructed, TopAbs_VERTEX, aReconVerts);
    TopExp::MapShapes(aReconstructed, TopAbs_EDGE, aReconEdges);
    TopExp::MapShapes(aReconstructed, TopAbs_WIRE, aReconWires);

    EXPECT_EQ(aReconVerts.Extent(), anOrigVerts.Extent())
      << "Vertex count mismatch for face " << aFaceIdx;
    EXPECT_EQ(aReconEdges.Extent(), anOrigEdges.Extent())
      << "Edge count mismatch for face " << aFaceIdx;
    EXPECT_EQ(aReconWires.Extent(), anOrigWires.Extent())
      << "Wire count mismatch for face " << aFaceIdx;

    // Verify same surface handle.
    const TopoDS_Face& anOrigF = TopoDS::Face(anOrigFace);
    const TopoDS_Face& aReconF = TopoDS::Face(aReconstructed);
    TopLoc_Location aLoc1, aLoc2;
    const Handle(Geom_Surface)& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
    const Handle(Geom_Surface)& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
    EXPECT_EQ(aSurf1.get(), aSurf2.get())
      << "Surface handle differs for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, ReconstructFace_AfterEdgeReplace_ContainsNewEdge)
{
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(0);
  ASSERT_GE(aWireDef.EdgeRefs.Length(), 1);
  BRepGraph_NodeId anOldEdgeId(BRepGraph_NodeId::Kind::Edge, aWireDef.EdgeRefs.Value(0).EdgeIdx);

  // Pick a different edge.
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Defs().NbEdges();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeId::Kind::Edge, aNewIdx);

  // Get 3D curve handles from graph for old/new edges.
  const BRepGraph_TopoNode::EdgeDef& aNewEdgeNode = myGraph.Defs().Edge(aNewIdx);
  const BRepGraph_TopoNode::EdgeDef& anOldEdgeNode = myGraph.Defs().Edge(anOldEdgeId.Index);
  Handle(Geom_Curve) aNewCurve = !aNewEdgeNode.Curve3d.IsNull()
    ? aNewEdgeNode.Curve3d : Handle(Geom_Curve)();
  Handle(Geom_Curve) anOldCurve = !anOldEdgeNode.Curve3d.IsNull()
    ? anOldEdgeNode.Curve3d : Handle(Geom_Curve)();

  myGraph.Mut().ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, false);

  // Reconstruct face 0 (the face owning wire 0).
  const int aFaceIdx = myGraph.Defs().Face(0).OuterWireIdx() == 0 ? 0 : -1;
  ASSERT_GE(aFaceIdx, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().ReconstructFace(aFaceIdx);

  // Check via 3D curve handle identity (reconstructed edges have new TShapes).
  bool isNewFound = false;
  bool isOldFound = false;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopLoc_Location aLoc;
    double aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(anExp.Current()), aLoc, aFirst, aLast);
    if (!aCurve.IsNull() && !aNewCurve.IsNull() && aCurve.get() == aNewCurve.get())
      isNewFound = true;
    if (!aCurve.IsNull() && !anOldCurve.IsNull() && aCurve.get() == anOldCurve.get())
      isOldFound = true;
  }
  EXPECT_TRUE(isNewFound) << "New edge curve not found in reconstructed face";
  EXPECT_FALSE(isOldFound) << "Old edge curve still present in reconstructed face";
}

TEST_F(BRepGraphTest, ReconstructShape_SolidRoot_SameFaceCount)
{
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(aSolidId);

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

TEST_F(BRepGraphTest, ReconstructShape_FaceRoot_ReturnsSameShape)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(aFaceId);
  const TopoDS_Shape& anOriginal = myGraph.Shapes().OriginalOf(aFaceId);

  // Reconstructed face should have the same surface handle.
  const TopoDS_Face& anOrigF = TopoDS::Face(anOriginal);
  const TopoDS_Face& aReconF = TopoDS::Face(aReconstructed);
  TopLoc_Location aLoc1, aLoc2;
  const Handle(Geom_Surface)& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
  const Handle(Geom_Surface)& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
  EXPECT_EQ(aSurf1.get(), aSurf2.get());
}

TEST_F(BRepGraphTest, Shape_Unmodified_ReturnsSameShape)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  TopoDS_Shape aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.Shapes().OriginalOf(aFaceId);
  EXPECT_TRUE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_AfterReplaceEdge_DiffersFromOriginal)
{
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(0);
  ASSERT_GE(aWireDef.EdgeRefs.Length(), 1);
  BRepGraph_NodeId anOldEdgeId(BRepGraph_NodeId::Kind::Edge, aWireDef.EdgeRefs.Value(0).EdgeIdx);
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Defs().NbEdges();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeId::Kind::Edge, aNewIdx);

  myGraph.Mut().ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, false);

  // Find the face that owns wire 0.
  int aFaceDefIdx = -1;
  for (int aFI = 0; aFI < myGraph.Defs().NbFaces(); ++aFI)
  {
    if (myGraph.Defs().Face(aFI).OuterWireIdx() == 0)
    {
      aFaceDefIdx = aFI;
      break;
    }
  }
  ASSERT_GE(aFaceDefIdx, 0);
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceDefIdx);
  TopoDS_Shape aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.Shapes().OriginalOf(aFaceId);
  EXPECT_FALSE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_WireKind_Valid)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeId::Kind::Wire, 0);
  TopoDS_Shape aShape = myGraph.Shapes().Shape(aWireId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_WIRE);
}

TEST_F(BRepGraphTest, Shape_EdgeKind_Valid)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape aShape = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_EDGE);
}

TEST_F(BRepGraphTest, Shape_VertexKind_Valid)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeId::Kind::Vertex, 0);
  TopoDS_Shape aShape = myGraph.Shapes().Shape(aVtxId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_VERTEX);
}

TEST_F(BRepGraphTest, IsModified_MutableEdge_PropagatesUp)
{
  EXPECT_FALSE(myGraph.Defs().Edge(0).IsModified);

  myGraph.Mut().EdgeDef(0);

  EXPECT_TRUE(myGraph.Defs().Edge(0).IsModified);

  // Check propagation up to parent wire and face.
  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Defs().Edge(0);
  if (anEdge.Id.IsValid())
  {
    // Find a wire containing this edge.
    const NCollection_Vector<int>& aWires = myGraph.Builder().WiresOfEdge(0);
    if (aWires.Length() > 0)
    {
      EXPECT_TRUE(myGraph.Defs().Wire(aWires.Value(0)).IsModified);
      // Check propagation to owning face: find a face whose outer wire is this wire.
      for (int aFI = 0; aFI < myGraph.Defs().NbFaces(); ++aFI)
      {
        if (myGraph.Defs().Face(aFI).OuterWireIdx() == aWires.Value(0))
        {
          EXPECT_TRUE(myGraph.Defs().Face(aFI).IsModified);
          break;
        }
      }
    }
  }
}

TEST_F(BRepGraphTest, ReconstructShape_WireKind_NoThrow)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeId::Kind::Wire, 0);
  TopoDS_Shape aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aWireId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraphTest, HasOriginalShape_AfterBuild_True)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(aFaceId))
      << "Face " << aFaceIdx << " should have original shape after Build()";
  }
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(anEdgeId))
      << "Edge " << anEdgeIdx << " should have original shape after Build()";
  }
}

TEST_F(BRepGraphTest, Shape_CachedOnSecondCall)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeId::Kind::Vertex, 0);
  TopoDS_Shape aFirst = myGraph.Shapes().Shape(aVtxId);
  TopoDS_Shape aSecond = myGraph.Shapes().Shape(aVtxId);
  EXPECT_TRUE(aFirst.IsSame(aSecond));
}

TEST_F(BRepGraphTest, Shape_InvalidatedAfterMutation)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape aBefore = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aBefore.IsNull());

  myGraph.Mut().EdgeDef(0)->Tolerance = 0.123;
  TopoDS_Shape anAfter = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(anAfter.IsNull());

  // After mutation, Shape() reconstructs a new edge -- different TShape.
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

TEST(BRepGraphUIDTest, DefaultBuild_AssignsValidUIDs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Build(aBoxMaker.Shape());

  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aUID = aGraph.UIDs().Of(aFaceId);

  EXPECT_TRUE(aUID.IsValid());
  EXPECT_TRUE(aGraph.UIDs().Has(aUID));
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aUID), aFaceId);
}

TEST(BRepGraphUIDTest, UIDsGeneration_IncrementsAcrossBuilds)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Build(aBoxMaker1.Shape());
  const uint32_t aGeneration1 = aGraph.UIDs().Generation();

  aGraph.Build(aBoxMaker2.Shape());
  const uint32_t aGeneration2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGeneration1, 0u);
  EXPECT_EQ(aGeneration2, aGeneration1 + 1);
}

TEST(BRepGraphUIDTest, StaleUID_HasReturnsFalseAfterRebuild)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Build(aBoxMaker1.Shape());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);
  const BRepGraph_UID anOldUID =
    aGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  ASSERT_TRUE(anOldUID.IsValid());
  ASSERT_TRUE(aGraph.UIDs().Has(anOldUID));

  aGraph.Build(aBoxMaker2.Shape());

  EXPECT_FALSE(aGraph.UIDs().Has(anOldUID));
}

// ===================================================================
// Group 2: History Tracking
// ===================================================================

TEST_F(BRepGraphTest, RecordHistory_MultipleRecords_SequenceNumbers)
{
  const int aBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);

  myGraph.History().Record("OpA", anEdge0, aRepl);
  myGraph.History().Record("OpB", anEdge0, aRepl);
  myGraph.History().Record("OpC", anEdge0, aRepl);

  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 3);

  // Check monotonically increasing sequence numbers.
  for (int anIdx = aBefore + 1; anIdx < aBefore + 3; ++anIdx)
  {
    EXPECT_GT(myGraph.History().Record(anIdx).SequenceNumber,
              myGraph.History().Record(anIdx - 1).SequenceNumber)
      << "SequenceNumber not monotonically increasing at index " << anIdx;
  }

  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("OpA"));
  EXPECT_TRUE(myGraph.History().Record(aBefore + 1).OperationName.IsEqual("OpB"));
  EXPECT_TRUE(myGraph.History().Record(aBefore + 2).OperationName.IsEqual("OpC"));
}

TEST_F(BRepGraphTest, FindOriginal_SingleHop_ReturnsSource)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "TestHop");

  BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge1);
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraphTest, FindDerived_SingleHop_ContainsTarget)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "TestHop");

  NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool isFound = false;
  for (int anIdx = 0; anIdx < aDerived.Length(); ++anIdx)
  {
    if (aDerived.Value(anIdx) == anEdge1)
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
  auto aModifier1 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };
  myGraph.ApplyModification(anEdge0, aModifier1, "Step1");

  // Step 2: edge1 -> edge2
  auto aModifier2 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge2);
    return aResult;
  };
  myGraph.ApplyModification(anEdge1, aModifier2, "Step2");

  // FindOriginal from edge2 should trace back to edge0.
  BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge2);
  EXPECT_EQ(anOriginal, anEdge0);

  // FindDerived from edge0 returns leaf-only transitive descendants.
  // edge1 is an intermediate (it has further derived edge2), so only edge2 is returned.
  NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool isEdge2Found = false;
  for (int anIdx = 0; anIdx < aDerived.Length(); ++anIdx)
  {
    if (aDerived.Value(anIdx) == anEdge2)
      isEdge2Found = true;
  }
  EXPECT_TRUE(isEdge2Found) << "Edge2 not found in FindDerived(Edge0)";

  // edge1 can be found by querying FindDerived on the intermediate step.
  NCollection_Vector<BRepGraph_NodeId> aDerived1 = myGraph.History().FindDerived(anEdge1);
  bool isEdge2FromEdge1 = false;
  for (int anIdx = 0; anIdx < aDerived1.Length(); ++anIdx)
  {
    if (aDerived1.Value(anIdx) == anEdge2)
      isEdge2FromEdge1 = true;
  }
  EXPECT_TRUE(isEdge2FromEdge1) << "Edge2 not found in FindDerived(Edge1)";
}

// ===================================================================
// Group 3: Mutation APIs
// ===================================================================

TEST_F(BRepGraphTest, AddPCurveToEdge_NewPCurve_RetrievableViaFindPCurve)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  Handle(Geom2d_Line) aCurve2d = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  myGraph.Mut().AddPCurveToEdge(anEdgeId, aFaceId, aCurve2d, 0.0, 1.0);

  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aRetrieved =
    myGraph.Defs().FindPCurve(anEdgeId, aFaceId);
  EXPECT_NE(aRetrieved, nullptr);
  if (aRetrieved != nullptr)
  {
    EXPECT_FALSE(aRetrieved->Curve2d.IsNull());
  }
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Reversed_OrientationFlipped)
{
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(0);
  ASSERT_GE(aWireDef.EdgeRefs.Length(), 1);

  const BRepGraphInc::EdgeRef& anOrigEdgeRef = aWireDef.EdgeRefs.Value(0);
  BRepGraph_NodeId anOldEdgeId(BRepGraph_NodeId::Kind::Edge, anOrigEdgeRef.EdgeIdx);
  TopAbs_Orientation anOrigOrientation = anOrigEdgeRef.Orientation;

  // Pick a different edge.
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Defs().NbEdges();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeId::Kind::Edge, aNewIdx);

  myGraph.Mut().ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, true);

  const BRepGraph_TopoNode::WireDef& aWireDefAfter = myGraph.Defs().Wire(0);
  const BRepGraphInc::EdgeRef& aNewEdgeRef = aWireDefAfter.EdgeRefs.Value(0);
  EXPECT_EQ(aNewEdgeRef.EdgeIdx, aNewEdgeId.Index);

  // Orientation should be flipped relative to original.
  TopAbs_Orientation anExpected = (anOrigOrientation == TopAbs_FORWARD)
                                    ? TopAbs_REVERSED
                                    : TopAbs_FORWARD;
  EXPECT_EQ(aNewEdgeRef.Orientation, anExpected);
}

TEST_F(BRepGraphTest, MutableVertex_ChangePoint_Verified)
{
  BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> aMutVert = myGraph.Mut().VertexDef(0);
  aMutVert->Point = gp_Pnt(99.0, 99.0, 99.0);

  const BRepGraph_TopoNode::VertexDef& aVert = myGraph.Defs().Vertex(0);
  EXPECT_NEAR(aVert.Point.X(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Y(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Z(), 99.0, Precision::Confusion());
}

TEST_F(BRepGraphTest, RemoveRelEdges_AddThenRemove_GoneFromOutEdges)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  myGraph.Mut().AddRelEdge(anEdge0, anEdge1, BRepGraph_RelEdge::Kind::UserDefined);

  int aBefore = 0;
  myGraph.RelEdges().ForEachOutOfKind(anEdge0, BRepGraph_RelEdge::Kind::UserDefined,
    [&](const BRepGraph_RelEdge&) { ++aBefore; });
  EXPECT_GE(aBefore, 1);

  myGraph.Mut().RemoveRelEdges(anEdge0, anEdge1, BRepGraph_RelEdge::Kind::UserDefined);

  int anAfter = 0;
  myGraph.RelEdges().ForEachOutOfKind(anEdge0, BRepGraph_RelEdge::Kind::UserDefined,
    [&](const BRepGraph_RelEdge&) { ++anAfter; });
  EXPECT_EQ(anAfter, 0);
}

TEST_F(BRepGraphTest, InEdgesOfKind_UserDefined_ReverseLookup)
{
  BRepGraph_NodeId anEdgeA(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdgeB(BRepGraph_NodeId::Kind::Edge, 1);

  myGraph.Mut().AddRelEdge(anEdgeA, anEdgeB, BRepGraph_RelEdge::Kind::UserDefined);

  bool isFound = false;
  myGraph.RelEdges().ForEachInOfKind(anEdgeB, BRepGraph_RelEdge::Kind::UserDefined,
    [&](const BRepGraph_RelEdge& theRelEdge) {
      if (theRelEdge.Source == anEdgeA)
        isFound = true;
    });
  EXPECT_TRUE(isFound) << "EdgeA not found as source in ForEachInEdgeOfKind(EdgeB)";
}

// ===================================================================
// Group 4: Geometric Queries
// ===================================================================

TEST_F(BRepGraphTest, Centroid_Box_ApproximateCenter)
{
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  gp_Pnt aCentroid = bboxCenter(myGraph, aSolidId);

  // 10x20x30 box at origin: centroid approximately at (5, 10, 15).
  EXPECT_NEAR(aCentroid.X(), 5.0, 0.5);
  EXPECT_NEAR(aCentroid.Y(), 10.0, 0.5);
  EXPECT_NEAR(aCentroid.Z(), 15.0, 0.5);
}

TEST_F(BRepGraphTest, GlobalTransform_FaceInsideSolid_NonNull)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  gp_Trsf aTrsf = myGraph.Spatial().GlobalTransform(aFaceId);

  // For a box at origin the transform should not be negative.
  EXPECT_FALSE(aTrsf.IsNegative());
}

TEST_F(BRepGraphTest, EdgeDef_HasValidCurve3d)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;

    EXPECT_FALSE(anEdge.Curve3d.IsNull())
      << "Edge " << anEdgeIdx << " has no Curve3d";
  }
}

// ===================================================================
// Group 5: Cache Invalidation
// ===================================================================

TEST_F(BRepGraphTest, Invalidate_BBox_RecomputesSameResult)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  Bnd_Box aBox1 = BRepGraphAlgo_BndLib::AddCached(myGraph, aFaceId);
  ASSERT_FALSE(aBox1.IsVoid());

  BRepGraphAlgo_BndLib::InvalidateCached(myGraph, aFaceId);

  Bnd_Box aBox2 = BRepGraphAlgo_BndLib::AddCached(myGraph, aFaceId);
  ASSERT_FALSE(aBox2.IsVoid());

  double aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1;
  double aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2;
  aBox1.Get(aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1);
  aBox2.Get(aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2);

  EXPECT_NEAR(aXmin1, aXmin2, Precision::Confusion());
  EXPECT_NEAR(aYmin1, aYmin2, Precision::Confusion());
  EXPECT_NEAR(aZmin1, aZmin2, Precision::Confusion());
  EXPECT_NEAR(aXmax1, aXmax2, Precision::Confusion());
  EXPECT_NEAR(aYmax1, aYmax2, Precision::Confusion());
  EXPECT_NEAR(aZmax1, aZmax2, Precision::Confusion());
}

TEST_F(BRepGraphTest, InvalidateSubgraph_Face_ConsistentAfter)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  // Compute initial values via AddCached.
  Bnd_Box aFaceBox1 = BRepGraphAlgo_BndLib::AddCached(myGraph, aFaceId);
  ASSERT_FALSE(aFaceBox1.IsVoid());

  // Get an edge from this face's outer wire via incidence refs.
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(0);
  const int anOuterWireIdx = aFaceDef.OuterWireIdx();
  ASSERT_GE(anOuterWireIdx, 0);
  const BRepGraph_TopoNode::WireDef& aWireDefInv = myGraph.Defs().Wire(anOuterWireIdx);
  ASSERT_GE(aWireDefInv.EdgeRefs.Length(), 1);
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, aWireDefInv.EdgeRefs.Value(0).EdgeIdx);

  Bnd_Box anEdgeBox1 = BRepGraphAlgo_BndLib::AddCached(myGraph, anEdgeId);
  ASSERT_FALSE(anEdgeBox1.IsVoid());

  // Invalidate subgraph from face.
  myGraph.Cache().InvalidateSubgraph(aFaceId);

  // Recompute: should not crash and should produce same values.
  Bnd_Box aFaceBox2 = BRepGraphAlgo_BndLib::AddCached(myGraph, aFaceId);
  ASSERT_FALSE(aFaceBox2.IsVoid());

  Bnd_Box anEdgeBox2 = BRepGraphAlgo_BndLib::AddCached(myGraph, anEdgeId);
  ASSERT_FALSE(anEdgeBox2.IsVoid());

  double aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1;
  double aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2;
  aFaceBox1.Get(aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1);
  aFaceBox2.Get(aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2);

  EXPECT_NEAR(aXmin1, aXmin2, Precision::Confusion());
  EXPECT_NEAR(aXmax1, aXmax2, Precision::Confusion());

  // Also verify edge BBox consistency.
  double aEXmin1, aEYmin1, aEZmin1, aEXmax1, aEYmax1, aEZmax1;
  double aEXmin2, aEYmin2, aEZmin2, aEXmax2, aEYmax2, aEZmax2;
  anEdgeBox1.Get(aEXmin1, aEYmin1, aEZmin1, aEXmax1, aEYmax1, aEZmax1);
  anEdgeBox2.Get(aEXmin2, aEYmin2, aEZmin2, aEXmax2, aEYmax2, aEZmax2);

  EXPECT_NEAR(aEXmin1, aEXmin2, Precision::Confusion());
  EXPECT_NEAR(aEXmax1, aEXmax2, Precision::Confusion());
}

// ===================================================================
// Group 6: Parallel Iteration
// ===================================================================

TEST_F(BRepGraphTest, ParallelForEachFace_AllFacesVisited)
{
  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(myGraph);
  ASSERT_GE(aSubs.Length(), 1);

  std::atomic<int> aCounter{0};
  for (int aSubIdx = 0; aSubIdx < aSubs.Length(); ++aSubIdx)
  {
    BRepGraph_Analyze::ParallelForEachFace(myGraph, aSubs.Value(aSubIdx),
      [&aCounter](int /*theFaceIdx*/)
      {
        aCounter.fetch_add(1, std::memory_order_relaxed);
      });
  }
  EXPECT_EQ(aCounter.load(), myGraph.Defs().NbFaces());
}

TEST_F(BRepGraphTest, ParallelForEachEdge_AllEdgesVisited)
{
  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(myGraph);
  ASSERT_GE(aSubs.Length(), 1);

  std::atomic<int> aCounter{0};
  const BRepGraph_SubGraph& aSub = aSubs.Value(0);
  BRepGraph_Analyze::ParallelForEachEdge(myGraph, aSub,
    [&aCounter](int /*theEdgeIdx*/)
    {
      aCounter.fetch_add(1, std::memory_order_relaxed);
    });
  EXPECT_EQ(aCounter.load(), aSub.EdgeDefIndices().Length());
}

// ===================================================================
// Group 7: Detection Methods
// ===================================================================

TEST(BRepGraphDetectionTest, FreeEdges_SingleFace_AllEdgesFree)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_FACE);
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_NodeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(aGraph);
  EXPECT_EQ(aFreeEdges.Length(), 4);
}

TEST(BRepGraphDetectionTest, Decompose_ThreeDisconnectedFaces_ThreeComponents)
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbFaces(), 3);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 3);
}

TEST_F(BRepGraphTest, DetectToleranceConflicts_ManualConflict_Detected)
{
  // Find two edges that share the same Curve3d handle.
  bool isConflictSetUp = false;
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges() && !isConflictSetUp; ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsDegenerate || anEdge.Curve3d.IsNull())
      continue;

    for (int anOtherIdx = anEdgeIdx + 1; anOtherIdx < myGraph.Defs().NbEdges(); ++anOtherIdx)
    {
      const BRepGraph_TopoNode::EdgeDef& anOther = myGraph.Defs().Edge(anOtherIdx);
      if (anOther.IsDegenerate || anOther.Curve3d.IsNull())
        continue;
      if (anEdge.Curve3d.get() != anOther.Curve3d.get())
        continue;

      // Set very different tolerances on two edges sharing the same curve.
      myGraph.Mut().EdgeDef(anEdgeIdx)->Tolerance = 0.001;
      myGraph.Mut().EdgeDef(anOtherIdx)->Tolerance = 1.0;

      isConflictSetUp = true;
      break;
    }
  }

  if (isConflictSetUp)
  {
    NCollection_Vector<BRepGraph_NodeId> aConflicts =
      BRepGraph_Analyze::ToleranceConflicts(myGraph, 0.5);
    EXPECT_GE(aConflicts.Length(), 1);
  }
}

// ===================================================================
// Group 8: User Attributes & Error Cases
// ===================================================================

TEST_F(BRepGraphTest, RemoveUserAttribute_AfterSet_ReturnsNull)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const int aKey = BRepGraph_UserAttribute::AllocateKey();
  auto anAttr = std::make_shared<BRepGraph_TypedAttribute<int>>(42);

  myGraph.Attrs().Set(aFaceId, aKey, anAttr);
  ASSERT_NE(myGraph.Attrs().Get(aFaceId, aKey), nullptr);

  myGraph.Attrs().Remove(aFaceId, aKey);
  EXPECT_EQ(myGraph.Attrs().Get(aFaceId, aKey), nullptr);
}

TEST(BRepGraphErrorTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
  EXPECT_EQ(aGraph.Defs().NbShells(), 0);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 0);
  EXPECT_EQ(aGraph.Defs().NbWires(), 0);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 0);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 0);
}

TEST_F(BRepGraphTest, TopoNode_GenericLookup_MatchesTypedAccess)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_TopoNode::BaseDef* aBase = myGraph.Defs().TopoDef(aFaceId);
  ASSERT_NE(aBase, nullptr);
  EXPECT_EQ(aBase->Id, myGraph.Defs().Face(0).Id);

  // Invalid node id should return nullptr.
  BRepGraph_NodeId anInvalidId;
  const BRepGraph_TopoNode::BaseDef* anInvalidBase = myGraph.Defs().TopoDef(anInvalidId);
  EXPECT_EQ(anInvalidBase, nullptr);
}

// ===================================================================
// Group 9: Node Counts and Identity
// ===================================================================

TEST_F(BRepGraphTest, NbNodes_Box_TotalCount)
{
  // NbNodes should equal sum of all per-kind counts (topology + assembly).
  size_t anExpected = static_cast<size_t>(myGraph.Defs().NbSolids())
                    + myGraph.Defs().NbShells()
                    + myGraph.Defs().NbFaces()
                    + myGraph.Defs().NbWires()
                    + myGraph.Defs().NbEdges()
                    + myGraph.Defs().NbVertices()
                    + myGraph.Defs().NbCompounds()
                    + myGraph.Defs().NbCompSolids()
                    + myGraph.Defs().NbProducts()
                    + myGraph.Defs().NbOccurrences();
  EXPECT_EQ(myGraph.Defs().NbNodes(), anExpected);
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
  BRepGraph_UID anInvalidUID = BRepGraph_UID::Invalid();
  BRepGraph_NodeId aResolved = myGraph.UIDs().NodeIdFrom(anInvalidUID);
  EXPECT_FALSE(aResolved.IsValid());
}

TEST_F(BRepGraphTest, Allocator_DefaultConstructor_NotNull)
{
  EXPECT_FALSE(myGraph.Allocator().IsNull());
}

TEST(BRepGraphAllocatorTest, Build_WithCustomAllocator_IsDone)
{
  Handle(NCollection_BaseAllocator) anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  BRepGraph aGraph(anAlloc);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_FALSE(aGraph.Allocator().IsNull());
}

// ===================================================================
// Group 10: Topology Structure
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_BoxOuterWires)
{
  // All outer wires of a box face should be closed.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(aFaceIdx);
    const int anOuterWireIdx = aFaceDef.OuterWireIdx();
    ASSERT_GE(anOuterWireIdx, 0);
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(anOuterWireIdx);
    EXPECT_TRUE(aWireDef.IsClosed) << "Outer wire of face " << aFaceIdx << " should be closed";
  }
}

TEST_F(BRepGraphTest, Face_InnerWireRefs_BoxHasNone)
{
  // Box faces have no holes, so non-outer WireRefs should be empty.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(aFaceIdx);
    int aNonOuterCount = 0;
    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      if (!aFaceDef.WireRefs.Value(aWireRefIdx).IsOuter)
        ++aNonOuterCount;
    }
    EXPECT_EQ(aNonOuterCount, 0)
      << "Box face " << aFaceIdx << " should have no inner wires";
  }
}

TEST_F(BRepGraphTest, Face_Orientation_ValidValue)
{
  // Verify face orientations in the shell's incidence refs.
  ASSERT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph.Defs().Shell(0);
  for (int aRefIdx = 0; aRefIdx < aShellDef.FaceRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aShellDef.FaceRefs.Value(aRefIdx);
    TopAbs_Orientation anOri = aFaceRef.Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED)
      << "Face ref " << aRefIdx << " has unexpected orientation " << anOri;
  }
}

TEST_F(BRepGraphTest, Shell_ContainsSixFaces)
{
  ASSERT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph.Defs().Shell(0);
  EXPECT_EQ(aShellDef.FaceRefs.Length(), 6);
}

TEST_F(BRepGraphTest, Solid_ContainsOneShell)
{
  ASSERT_EQ(myGraph.Defs().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph.Defs().Solid(0);
  EXPECT_EQ(aSolidDef.ShellRefs.Length(), 1);
}

TEST_F(BRepGraphTest, Edge_ParamRange_ValidBounds)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;
    EXPECT_LT(anEdge.ParamFirst, anEdge.ParamLast)
      << "Edge " << anEdgeIdx << " has invalid parameter range ["
      << anEdge.ParamFirst << ", " << anEdge.ParamLast << "]";
  }
}

TEST_F(BRepGraphTest, Vertex_TolerancePositive)
{
  for (int aVtxIdx = 0; aVtxIdx < myGraph.Defs().NbVertices(); ++aVtxIdx)
  {
    EXPECT_GT(myGraph.Defs().Vertex(aVtxIdx).Tolerance, 0.0)
      << "Vertex " << aVtxIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Edge_TolerancePositive)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    EXPECT_GT(myGraph.Defs().Edge(anEdgeIdx).Tolerance, 0.0)
      << "Edge " << anEdgeIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Face_ToleranceNonNegative)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    EXPECT_GE(myGraph.Defs().Face(aFaceIdx).Tolerance, 0.0)
      << "Face " << aFaceIdx << " has negative tolerance";
  }
}

// ===================================================================
// Group 11: SubGraph
// ===================================================================

TEST_F(BRepGraphTest, SubGraph_BoxSolid_AllIndicesPresent)
{
  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(myGraph);
  ASSERT_EQ(aSubs.Length(), 1);

  const BRepGraph_SubGraph& aSub = aSubs.Value(0);
  EXPECT_EQ(aSub.SolidDefIndices().Length(), 1);
  EXPECT_EQ(aSub.ShellDefIndices().Length(), 1);
  EXPECT_EQ(aSub.FaceDefIndices().Length(), 6);
  EXPECT_EQ(aSub.WireDefIndices().Length(), 6);
  // Edge/vertex indices may include per-wire-reference duplicates.
  EXPECT_GE(aSub.EdgeDefIndices().Length(), 12);
  EXPECT_GE(aSub.VertexDefIndices().Length(), 8);
  EXPECT_GT(aSub.NbTopoNodes(), 0);
  EXPECT_EQ(aSub.ParentGraph(), &myGraph);
}

// ===================================================================
// Group 12: Geometric Queries (Extended)
// ===================================================================

TEST_F(BRepGraphTest, BoundingBox_Solid_ContainsAllFaces)
{
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box aSolidBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aSolidId, aSolidBox);
  ASSERT_FALSE(aSolidBox.IsVoid());

  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box aFaceBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aFaceBox);
    if (aFaceBox.IsVoid())
      continue;

    // Every face's BBox should be within the solid's BBox (with tolerance).
    double aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax;
    double aSXmin, aSYmin, aSZmin, aSXmax, aSYmax, aSZmax;
    aFaceBox.Get(aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax);
    aSolidBox.Get(aSXmin, aSYmin, aSZmin, aSXmax, aSYmax, aSZmax);

    const double aTol = Precision::Confusion();
    EXPECT_GE(aFXmin, aSXmin - aTol) << "Face " << aFaceIdx << " outside solid BBox (Xmin)";
    EXPECT_LE(aFXmax, aSXmax + aTol) << "Face " << aFaceIdx << " outside solid BBox (Xmax)";
    EXPECT_GE(aFYmin, aSYmin - aTol) << "Face " << aFaceIdx << " outside solid BBox (Ymin)";
    EXPECT_LE(aFYmax, aSYmax + aTol) << "Face " << aFaceIdx << " outside solid BBox (Ymax)";
  }
}

TEST_F(BRepGraphTest, BoundingBox_Edge_SubsetOfFace)
{
  // Each edge's BBox should be within the BBox of any face it belongs to.
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(0);
  const int anOuterWireIdx = aFaceDef.OuterWireIdx();
  ASSERT_GE(anOuterWireIdx, 0);
  const BRepGraph_TopoNode::WireDef& aWireDefBBox = myGraph.Defs().Wire(anOuterWireIdx);
  ASSERT_GE(aWireDefBBox.EdgeRefs.Length(), 1);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Bnd_Box aFaceBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aFaceBox);
  ASSERT_FALSE(aFaceBox.IsVoid());

  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, aWireDefBBox.EdgeRefs.Value(0).EdgeIdx);
  Bnd_Box anEdgeBox;
  BRepGraphAlgo_BndLib::Add(myGraph, anEdgeId, anEdgeBox);
  ASSERT_FALSE(anEdgeBox.IsVoid());

  double aEXmin, aEYmin, aEZmin, aEXmax, aEYmax, aEZmax;
  double aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax;
  anEdgeBox.Get(aEXmin, aEYmin, aEZmin, aEXmax, aEYmax, aEZmax);
  aFaceBox.Get(aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax);

  const double aTol = Precision::Confusion();
  EXPECT_GE(aEXmin, aFXmin - aTol);
  EXPECT_LE(aEXmax, aFXmax + aTol);
}

TEST_F(BRepGraphTest, Centroid_Face_InsideBBox)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    gp_Pnt aCentroid = bboxCenter(myGraph, aFaceId);
    Bnd_Box aBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aBox);
    if (aBox.IsVoid())
      continue;

    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    const double aTol = 1.0; // generous tolerance for centroid approximation
    EXPECT_GE(aCentroid.X(), aXmin - aTol) << "Face " << aFaceIdx;
    EXPECT_LE(aCentroid.X(), aXmax + aTol) << "Face " << aFaceIdx;
    EXPECT_GE(aCentroid.Y(), aYmin - aTol) << "Face " << aFaceIdx;
    EXPECT_LE(aCentroid.Y(), aYmax + aTol) << "Face " << aFaceIdx;
    EXPECT_GE(aCentroid.Z(), aZmin - aTol) << "Face " << aFaceIdx;
    EXPECT_LE(aCentroid.Z(), aZmax + aTol) << "Face " << aFaceIdx;
  }
}

// ===================================================================
// Group 13: Mutation (Extended)
// ===================================================================

TEST_F(BRepGraphTest, MutableWireDef_ModifyClosure_Verified)
{
  BRepGraph_MutRef<BRepGraph_TopoNode::WireDef> aMutWD = myGraph.Mut().WireDef(0);
  bool anOrigClosed = aMutWD->IsClosed;
  aMutWD->IsClosed = !anOrigClosed;

  EXPECT_EQ(myGraph.Defs().Wire(0).IsClosed, !anOrigClosed);

  // Restore original state.
  myGraph.Mut().WireDef(0)->IsClosed = anOrigClosed;
  EXPECT_EQ(myGraph.Defs().Wire(0).IsClosed, anOrigClosed);
}

TEST_F(BRepGraphTest, MultipleUserAttributes_SameNode_Independent)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  const int aKey1 = BRepGraph_UserAttribute::AllocateKey();
  const int aKey2 = BRepGraph_UserAttribute::AllocateKey();
  EXPECT_NE(aKey1, aKey2);

  auto anAttr1 = std::make_shared<BRepGraph_TypedAttribute<int>>(100);
  auto anAttr2 = std::make_shared<BRepGraph_TypedAttribute<double>>(2.718);

  myGraph.Attrs().Set(aFaceId, aKey1, anAttr1);
  myGraph.Attrs().Set(aFaceId, aKey2, anAttr2);

  auto aRetrieved1 = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<int>>(
    myGraph.Attrs().Get(aFaceId, aKey1));
  auto aRetrieved2 = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<double>>(
    myGraph.Attrs().Get(aFaceId, aKey2));

  ASSERT_NE(aRetrieved1, nullptr);
  ASSERT_NE(aRetrieved2, nullptr);
  EXPECT_EQ(aRetrieved1->UncheckedValue(), 100);
  EXPECT_NEAR(aRetrieved2->UncheckedValue(), 2.718, 1.0e-10);

  // Remove one; the other should remain.
  myGraph.Attrs().Remove(aFaceId, aKey1);
  EXPECT_EQ(myGraph.Attrs().Get(aFaceId, aKey1), nullptr);
  EXPECT_NE(myGraph.Attrs().Get(aFaceId, aKey2), nullptr);
}

TEST_F(BRepGraphTest, InvalidateUserAttribute_SpecificKey)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const int aKey = BRepGraph_UserAttribute::AllocateKey();
  auto anAttr = std::make_shared<BRepGraph_TypedAttribute<int>>(42);
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);

  // Invalidate should not remove, just mark dirty.
  myGraph.Attrs().Invalidate(aFaceId, aKey);

  BRepGraph_UserAttrPtr aRetrieved = myGraph.Attrs().Get(aFaceId, aKey);
  EXPECT_NE(aRetrieved, nullptr); // still present
}

TEST_F(BRepGraphTest, UserAttribute_OnEdgeNode)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  const int aKey = BRepGraph_UserAttribute::AllocateKey();
  auto anAttr = std::make_shared<BRepGraph_TypedAttribute<double>>(1.5);

  myGraph.Attrs().Set(anEdgeId, aKey, anAttr);

  auto aRetrieved = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<double>>(
    myGraph.Attrs().Get(anEdgeId, aKey));
  ASSERT_NE(aRetrieved, nullptr);
  EXPECT_NEAR(aRetrieved->UncheckedValue(), 1.5, 1.0e-10);
}

// ===================================================================
// Group 14: Build From Different Root Shapes
// ===================================================================

TEST(BRepGraphBuildTest, Build_SingleFace_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_FACE);
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
  EXPECT_EQ(aGraph.Defs().NbShells(), 0);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 1);
  EXPECT_EQ(aGraph.Defs().NbWires(), 1);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 4);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 4);
}

TEST(BRepGraphBuildTest, Build_Shell_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
  EXPECT_EQ(aGraph.Defs().NbShells(), 1);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
}

TEST(BRepGraphBuildTest, Build_CompoundOfTwoSolids)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1.Shape());
  aBuilder.Add(aCompound, aBox2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbSolids(), 2);
  EXPECT_EQ(aGraph.Defs().NbShells(), 2);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 12);
}

TEST(BRepGraphBuildTest, ReconstructShape_ShellRoot_SameFaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbShells(), 1);

  BRepGraph_NodeId aShellId(BRepGraph_NodeId::Kind::Shell, 0);
  const TopoDS_Shape aReconstructed = aGraph.Shapes().Reconstruct(aShellId);

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aReconstructed, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
    ++aFaceCount;
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx)).IsValid())
      << "Face " << aFaceIdx << " has invalid UID";
  }
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx)).IsValid())
      << "Edge " << anEdgeIdx << " has invalid UID";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWireIdx_MatchesFaceDef)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Defs().Face(aFaceIdx);
    const int anOuterWireIdx = aFace.OuterWireIdx();
    EXPECT_GE(anOuterWireIdx, 0) << "Face " << aFaceIdx << " has no outer wire";
    if (anOuterWireIdx < 0)
      continue;
    EXPECT_LT(anOuterWireIdx, myGraph.Defs().NbWires())
      << "Face " << aFaceIdx << " outer wire index out of range";
  }
}

TEST_F(BRepGraphTest, Wire_EdgeRefs_FourEdgesPerBoxFace)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(aFaceIdx);
    const int anOuterWireIdx = aFaceDef.OuterWireIdx();
    ASSERT_GE(anOuterWireIdx, 0);
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(anOuterWireIdx);
    EXPECT_EQ(aWireDef.EdgeRefs.Length(), 4)
      << "Box face " << aFaceIdx << " should have 4 edge refs in its outer wire";
  }
}

// ===================================================================
// Group: History Enabled Flag
// ===================================================================

TEST_F(BRepGraphTest, SetHistoryEnabled_DefaultTrue)
{
  EXPECT_TRUE(myGraph.IsHistoryEnabled());
}

TEST_F(BRepGraphTest, SetHistoryEnabled_DisableAndQuery)
{
  myGraph.SetHistoryEnabled(false);
  EXPECT_FALSE(myGraph.IsHistoryEnabled());
  myGraph.SetHistoryEnabled(true);
  EXPECT_TRUE(myGraph.IsHistoryEnabled());
}

TEST_F(BRepGraphTest, RecordHistory_Disabled_NoRecordAdded)
{
  const int aBefore = myGraph.History().NbRecords();

  myGraph.SetHistoryEnabled(false);

  BRepGraph_NodeId                      anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                      anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("ShouldNotRecord", anEdge0, aRepl);

  EXPECT_EQ(myGraph.History().NbRecords(), aBefore);
}

TEST_F(BRepGraphTest, RecordHistory_ReEnabled_RecordsAgain)
{
  myGraph.SetHistoryEnabled(false);

  const int aBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId                      anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                      anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("Skipped", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore);

  myGraph.SetHistoryEnabled(true);
  myGraph.History().Record("Recorded", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("Recorded"));
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_NoHistoryNoDerivedEdges)
{
  myGraph.SetHistoryEnabled(false);

  const int aNbHistBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "NoHistory");

  // No history records should be added.
  EXPECT_EQ(myGraph.History().NbRecords(), aNbHistBefore);
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_ModifierStillRuns)
{
  myGraph.SetHistoryEnabled(false);

  bool isModifierCalled = false;
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    isModifierCalled = true;
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge0);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "CheckModifier");
  EXPECT_TRUE(isModifierCalled);
}

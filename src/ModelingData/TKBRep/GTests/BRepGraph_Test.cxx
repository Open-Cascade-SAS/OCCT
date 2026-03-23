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
#include <BRepGraph_Analyze.hxx>
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

class BRepGraphTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.SetUIDEnabled(true);
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
  EXPECT_EQ(myGraph.NbSolidDefs(), 1);
  EXPECT_EQ(myGraph.NbShellDefs(), 1);
  EXPECT_EQ(myGraph.NbFaceDefs(), 6);
  EXPECT_EQ(myGraph.NbWireDefs(), 6);
  EXPECT_EQ(myGraph.NbEdgeDefs(), 12);
  EXPECT_EQ(myGraph.NbVertexDefs(), 8);
}

TEST_F(BRepGraphTest, Build_SimpleBox_SurfaceCount)
{
  EXPECT_EQ(myGraph.NbSurfaces(), 6);
}

TEST_F(BRepGraphTest, Face_SurfNodeId_IsValid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    EXPECT_TRUE(aFace.SurfNodeId.IsValid()) << "Face " << aFaceIdx << " has invalid SurfNodeId";
  }
}

TEST_F(BRepGraphTest, Edge_CurveAndVertices_AreValid)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.EdgeDef(anEdgeIdx);
    if (!anEdge.IsDegenerate)
    {
      EXPECT_TRUE(anEdge.CurveNodeId.IsValid())
        << "Edge " << anEdgeIdx << " has invalid CurveNodeId";
    }
    EXPECT_TRUE(anEdge.StartVertexDefId.IsValid())
      << "Edge " << anEdgeIdx << " has invalid StartVertexId";
    EXPECT_TRUE(anEdge.EndVertexDefId.IsValid()) << "Edge " << anEdgeIdx << " has invalid EndVertexId";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWire_Exists)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    ASSERT_FALSE(aFace.Usages.IsEmpty()) << "Face " << aFaceIdx << " has no usages";
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    EXPECT_TRUE(aFaceUsage.OuterWireUsage.IsValid()) << "Face " << aFaceIdx << " has no OuterWireUsage";
  }
}

TEST_F(BRepGraphTest, SurfaceOf_Bidirectional)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    BRepGraph_NodeId aSurfId = myGraph.SurfaceOf(aFaceId);
    ASSERT_TRUE(aSurfId.IsValid());

    const NCollection_Vector<BRepGraph_NodeId>& aUsers  = myGraph.FacesOnSurface(aSurfId);
    bool                                        isFound = false;
    for (int anIdx = 0; anIdx < aUsers.Length(); ++anIdx)
    {
      if (aUsers.Value(anIdx) == aFaceId)
      {
        isFound = true;
        break;
      }
    }
    EXPECT_TRUE(isFound) << "Face " << aFaceIdx << " not found in FacesOnSurface()";
  }
}

TEST_F(BRepGraphTest, PCurveOf_ValidPair)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    ASSERT_FALSE(aFace.Usages.IsEmpty());
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    if (!aFaceUsage.OuterWireUsage.IsValid())
      continue;
    const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(
      myGraph.WireUsage(aFaceUsage.OuterWireUsage.Index).DefId.Index);
    for (int anEdgeIter = 0; anEdgeIter < aWire.OrderedEdges.Length(); ++anEdgeIter)
    {
      BRepGraph_NodeId anEdgeId = aWire.OrderedEdges.Value(anEdgeIter).EdgeDefId;
      const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.EdgeDef(anEdgeId.Index);
      if (anEdge.IsDegenerate)
        continue;
      BRepGraph_NodeId aPCurveId = myGraph.PCurveOf(anEdgeId, aFaceId);
      EXPECT_TRUE(aPCurveId.IsValid())
        << "Missing PCurve for edge " << anEdgeId.Index << " on face " << aFaceIdx;
    }
  }
}

TEST_F(BRepGraphTest, UID_Unique)
{
  NCollection_Map<BRepGraph_UID, BRepGraph_UID::Hasher> aUIDSet;
  for (int aSolidIdx = 0; aSolidIdx < myGraph.NbSolidDefs(); ++aSolidIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Solid, aSolidIdx))));
  for (int aShellIdx = 0; aShellIdx < myGraph.NbShellDefs(); ++aShellIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Shell, aShellIdx))));
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Face, aFaceIdx))));
  for (int aWireIdx = 0; aWireIdx < myGraph.NbWireDefs(); ++aWireIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aWireIdx))));
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Edge, anEdgeIdx))));
  for (int aVertexIdx = 0; aVertexIdx < myGraph.NbVertexDefs(); ++aVertexIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVertexIdx))));
  for (int aSurfIdx = 0; aSurfIdx < myGraph.NbSurfaces(); ++aSurfIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Surface, aSurfIdx))));
  for (int aCurveIdx = 0; aCurveIdx < myGraph.NbCurves(); ++aCurveIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Curve, aCurveIdx))));
  for (int aPCurveIdx = 0; aPCurveIdx < myGraph.NbPCurves(); ++aPCurveIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::PCurve, aPCurveIdx))));
}

TEST_F(BRepGraphTest, UID_NodeIdRoundTrip)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    const BRepGraph_UID& aUID = myGraph.UIDOf(aFaceId);
    BRepGraph_NodeId     aResolved = myGraph.NodeIdFromUID(aUID);
    EXPECT_EQ(aResolved, aFaceId) << "Round trip failed for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, BoundingBox_NonVoid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    Bnd_Box          aBox = myGraph.BoundingBox(aFaceId);
    EXPECT_FALSE(aBox.IsVoid()) << "BoundingBox is void for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, SameDomainFaces_Box_Empty)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId                       aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    NCollection_Vector<BRepGraph_NodeId> aSameDomain = myGraph.SameDomainFaces(aFaceId);
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
  EXPECT_EQ(aGraph.NbFaceDefs(), 2);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 2);
}

TEST_F(BRepGraphTest, UserAttribute_SetGet)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);

  const int aKey   = BRepGraph_UserAttribute::AllocateKey();
  auto      anAttr = std::make_shared<BRepGraph_TypedAttribute<double>>(3.14);
  myGraph.SetUserAttribute(aFaceId, aKey, anAttr);

  BRepGraph_UserAttrPtr aRetrieved = myGraph.GetUserAttribute(aFaceId, aKey);
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
  aGraph.SetUIDEnabled(true);
  aGraph.Build(aBox);
  const size_t   aCountAfterFirst = aGraph.NbNodes();
  const uint32_t aGen1            = aGraph.Generation();

  aGraph.Build(aBox);
  const uint32_t aGen2 = aGraph.Generation();

  EXPECT_GT(aGen2, aGen1);

  size_t aMinCounter = SIZE_MAX;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aNodeId(BRepGraph_NodeKind::Face, aFaceIdx);
    if (aGraph.UIDOf(aNodeId).Counter() < aMinCounter)
      aMinCounter = aGraph.UIDOf(aNodeId).Counter();
  }
  EXPECT_GE(aMinCounter, aCountAfterFirst);
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
  double                    anOrigTol = myGraph.EdgeDef(0).Tolerance;
  BRepGraph_TopoNode::EdgeDef& anEdge    = myGraph.MutableEdgeDef(0);
  anEdge.Tolerance                    = anOrigTol * 2.0;
  EXPECT_NEAR(myGraph.EdgeDef(0).Tolerance, anOrigTol * 2.0, 1.0e-15);
}

TEST_F(BRepGraphTest, FaceCountForEdge_SharedEdge)
{
  // In a box, each non-degenerate edge is shared by exactly 2 faces.
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    if (!myGraph.EdgeDef(anEdgeIdx).IsDegenerate)
    {
      int aCount = myGraph.FaceCountForEdge(anEdgeIdx);
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
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  myGraph.AddRelEdge(anEdge0, anEdge1, BRepGraph_RelKind::UserDefined);
  int aCount = 0;
  myGraph.ForEachOutEdgeOfKind(anEdge0, BRepGraph_RelKind::UserDefined,
    [&](const BRepGraph_RelEdge&) { ++aCount; });
  EXPECT_EQ(aCount, 1);
}

TEST_F(BRepGraphTest, RecordHistory_BasicEntry)
{
  int                                    aBefore = myGraph.NbHistoryRecords();
  BRepGraph_NodeId                      anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId                      anEdge1(BRepGraph_NodeKind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.RecordHistory("TestOp", anEdge0, aRepl);
  EXPECT_EQ(myGraph.NbHistoryRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.HistoryRecord(aBefore).OperationName.IsEqual("TestOp"));
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Substitution)
{
  // Get the first wire and its first edge.
  const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anOldEdge = aWire.OrderedEdges.Value(0).EdgeDefId;

  // Pick a different edge to substitute.
  BRepGraph_NodeId aNewEdge(BRepGraph_NodeKind::Edge, (anOldEdge.Index + 1) % myGraph.NbEdgeDefs());

  myGraph.ReplaceEdgeInWire(0, anOldEdge, aNewEdge, false);

  // Verify the substitution.
  EXPECT_EQ(myGraph.WireDef(0).OrderedEdges.Value(0).EdgeDefId, aNewEdge);
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

  EXPECT_EQ(aParGraph.NbSolidDefs(), aSeqGraph.NbSolidDefs());
  EXPECT_EQ(aParGraph.NbShellDefs(), aSeqGraph.NbShellDefs());
  EXPECT_EQ(aParGraph.NbFaceDefs(), aSeqGraph.NbFaceDefs());
  EXPECT_EQ(aParGraph.NbWireDefs(), aSeqGraph.NbWireDefs());
  EXPECT_EQ(aParGraph.NbEdgeDefs(), aSeqGraph.NbEdgeDefs());
  EXPECT_EQ(aParGraph.NbVertexDefs(), aSeqGraph.NbVertexDefs());
  EXPECT_EQ(aParGraph.NbSurfaces(), aSeqGraph.NbSurfaces());
  EXPECT_EQ(aParGraph.NbCurves(), aSeqGraph.NbCurves());
  EXPECT_EQ(aParGraph.NbPCurves(), aSeqGraph.NbPCurves());
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
  EXPECT_EQ(aGraph.NbFaceDefs(), 12);
}

// ===================================================================
// Group 1: Shape Round-Trip
// ===================================================================

TEST_F(BRepGraphTest, ReconstructFace_EachBoxFace_SameSubShapeCounts)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    const TopoDS_Shape& anOrigFace = myGraph.OriginalOf(aFaceId);
    const TopoDS_Shape  aReconstructed = myGraph.ReconstructFace(aFaceIdx);

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
  const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anOldEdgeId = aWire.OrderedEdges.Value(0).EdgeDefId;

  // Pick a different edge.
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.NbEdgeDefs();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeKind::Edge, aNewIdx);

  // Get 3D curve handles from graph for old/new edges.
  const BRepGraph_TopoNode::EdgeDef& aNewEdgeNode = myGraph.EdgeDef(aNewIdx);
  const BRepGraph_TopoNode::EdgeDef& anOldEdgeNode = myGraph.EdgeDef(anOldEdgeId.Index);
  Handle(Geom_Curve) aNewCurve = aNewEdgeNode.CurveNodeId.IsValid()
    ? myGraph.Curve(aNewEdgeNode.CurveNodeId.Index).CurveGeom : nullptr;
  Handle(Geom_Curve) anOldCurve = anOldEdgeNode.CurveNodeId.IsValid()
    ? myGraph.Curve(anOldEdgeNode.CurveNodeId.Index).CurveGeom : nullptr;

  myGraph.ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, false);

  // Reconstruct the face owning this wire.
  ASSERT_FALSE(aWire.Usages.IsEmpty());
  const BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph.WireUsage(aWire.Usages.First().Index);
  int aFaceIdx = myGraph.FaceUsage(aWireUsage.OwnerFaceUsage.Index).DefId.Index;
  const TopoDS_Shape aReconstructed = myGraph.ReconstructFace(aFaceIdx);

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
  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  const TopoDS_Shape aReconstructed = myGraph.ReconstructShape(aSolidId);

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

TEST_F(BRepGraphTest, ReconstructShape_FaceRoot_ReturnsSameShape)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const TopoDS_Shape aReconstructed = myGraph.ReconstructShape(aFaceId);
  const TopoDS_Shape& anOriginal = myGraph.OriginalOf(aFaceId);

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
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  TopoDS_Shape aShape = myGraph.Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.OriginalOf(aFaceId);
  EXPECT_TRUE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_AfterReplaceEdge_DiffersFromOriginal)
{
  const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anOldEdgeId = aWire.OrderedEdges.Value(0).EdgeDefId;
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.NbEdgeDefs();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeKind::Edge, aNewIdx);

  myGraph.ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, false);

  ASSERT_FALSE(aWire.Usages.IsEmpty());
  const BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph.WireUsage(aWire.Usages.First().Index);
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, myGraph.FaceUsage(aWireUsage.OwnerFaceUsage.Index).DefId.Index);
  TopoDS_Shape aShape = myGraph.Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.OriginalOf(aFaceId);
  EXPECT_FALSE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_WireKind_Valid)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeKind::Wire, 0);
  TopoDS_Shape aShape = myGraph.Shape(aWireId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_WIRE);
}

TEST_F(BRepGraphTest, Shape_EdgeKind_Valid)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  TopoDS_Shape aShape = myGraph.Shape(anEdgeId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_EDGE);
}

TEST_F(BRepGraphTest, Shape_VertexKind_Valid)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeKind::Vertex, 0);
  TopoDS_Shape aShape = myGraph.Shape(aVtxId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_VERTEX);
}

TEST_F(BRepGraphTest, IsModified_MutableEdge_PropagatesUp)
{
  EXPECT_FALSE(myGraph.EdgeDef(0).IsModified);

  myGraph.MutableEdgeDef(0);

  EXPECT_TRUE(myGraph.EdgeDef(0).IsModified);

  // Check propagation up to parent wire and face.
  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.EdgeDef(0);
  if (anEdge.Id.IsValid())
  {
    // Find a wire containing this edge.
    const NCollection_Vector<int>& aWires = myGraph.WiresOfEdge(0);
    if (aWires.Length() > 0)
    {
      EXPECT_TRUE(myGraph.WireDef(aWires.Value(0)).IsModified);
      const BRepGraph_TopoNode::WireDef& aWireNode = myGraph.WireDef(aWires.Value(0));
      if (!aWireNode.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::WireUsage& aWireUsageNode = myGraph.WireUsage(aWireNode.Usages.First().Index);
        if (aWireUsageNode.OwnerFaceUsage.IsValid())
        {
          EXPECT_TRUE(myGraph.FaceDef(myGraph.FaceUsage(aWireUsageNode.OwnerFaceUsage.Index).DefId.Index).IsModified);
        }
      }
    }
  }
}

TEST_F(BRepGraphTest, ReconstructShape_WireKind_NoThrow)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeKind::Wire, 0);
  TopoDS_Shape aShape;
  EXPECT_NO_THROW(aShape = myGraph.ReconstructShape(aWireId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraphTest, HasOriginalShape_AfterBuild_True)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    EXPECT_TRUE(myGraph.HasOriginalShape(aFaceId))
      << "Face " << aFaceIdx << " should have original shape after Build()";
  }
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, anEdgeIdx);
    EXPECT_TRUE(myGraph.HasOriginalShape(anEdgeId))
      << "Edge " << anEdgeIdx << " should have original shape after Build()";
  }
}

TEST_F(BRepGraphTest, Shape_CachedOnSecondCall)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeKind::Vertex, 0);
  TopoDS_Shape aFirst = myGraph.Shape(aVtxId);
  TopoDS_Shape aSecond = myGraph.Shape(aVtxId);
  EXPECT_TRUE(aFirst.IsSame(aSecond));
}

TEST_F(BRepGraphTest, Shape_InvalidatedAfterMutation)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  TopoDS_Shape aBefore = myGraph.Shape(anEdgeId);
  EXPECT_FALSE(aBefore.IsNull());

  myGraph.MutableEdgeDef(0).Tolerance = 0.123;
  TopoDS_Shape anAfter = myGraph.Shape(anEdgeId);
  EXPECT_FALSE(anAfter.IsNull());

  // After mutation, Shape() reconstructs a new edge — different TShape.
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

// ===================================================================
// Group 2: History Tracking
// ===================================================================

TEST_F(BRepGraphTest, RecordHistory_MultipleRecords_SequenceNumbers)
{
  const int aBefore = myGraph.NbHistoryRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);

  myGraph.RecordHistory("OpA", anEdge0, aRepl);
  myGraph.RecordHistory("OpB", anEdge0, aRepl);
  myGraph.RecordHistory("OpC", anEdge0, aRepl);

  EXPECT_EQ(myGraph.NbHistoryRecords(), aBefore + 3);

  // Check monotonically increasing sequence numbers.
  for (int anIdx = aBefore + 1; anIdx < aBefore + 3; ++anIdx)
  {
    EXPECT_GT(myGraph.HistoryRecord(anIdx).SequenceNumber,
              myGraph.HistoryRecord(anIdx - 1).SequenceNumber)
      << "SequenceNumber not monotonically increasing at index " << anIdx;
  }

  EXPECT_TRUE(myGraph.HistoryRecord(aBefore).OperationName.IsEqual("OpA"));
  EXPECT_TRUE(myGraph.HistoryRecord(aBefore + 1).OperationName.IsEqual("OpB"));
  EXPECT_TRUE(myGraph.HistoryRecord(aBefore + 2).OperationName.IsEqual("OpC"));
}

TEST_F(BRepGraphTest, FindOriginal_SingleHop_ReturnsSource)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "TestHop");

  BRepGraph_NodeId anOriginal = myGraph.FindOriginal(anEdge1);
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraphTest, FindDerived_SingleHop_ContainsTarget)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "TestHop");

  NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.FindDerived(anEdge0);
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
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);
  BRepGraph_NodeId anEdge2(BRepGraph_NodeKind::Edge, 2);

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
  BRepGraph_NodeId anOriginal = myGraph.FindOriginal(anEdge2);
  EXPECT_EQ(anOriginal, anEdge0);

  // FindDerived from edge0 should contain both edge1 and edge2.
  NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.FindDerived(anEdge0);
  bool isEdge1Found = false;
  bool isEdge2Found = false;
  for (int anIdx = 0; anIdx < aDerived.Length(); ++anIdx)
  {
    if (aDerived.Value(anIdx) == anEdge1)
      isEdge1Found = true;
    if (aDerived.Value(anIdx) == anEdge2)
      isEdge2Found = true;
  }
  EXPECT_TRUE(isEdge1Found) << "Edge1 not found in FindDerived(Edge0)";
  EXPECT_TRUE(isEdge2Found) << "Edge2 not found in FindDerived(Edge0)";
}

// ===================================================================
// Group 3: Mutation APIs
// ===================================================================

TEST_F(BRepGraphTest, AddPCurveToEdge_NewPCurve_RetrievableViaPCurveOf)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);

  Handle(Geom2d_Line) aCurve2d = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  BRepGraph_NodeId aPCurveId = myGraph.AddPCurveToEdge(anEdgeId, aFaceId, aCurve2d, 0.0, 1.0);

  ASSERT_TRUE(aPCurveId.IsValid());

  BRepGraph_NodeId aRetrievedId = myGraph.PCurveOf(anEdgeId, aFaceId);
  EXPECT_TRUE(aRetrievedId.IsValid());
  EXPECT_FALSE(myGraph.PCurve(aRetrievedId.Index).Curve2d.IsNull());
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Reversed_OrientationFlipped)
{
  const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);

  const BRepGraph_TopoNode::WireDef::EdgeEntry& anOrigEntry = aWire.OrderedEdges.Value(0);
  BRepGraph_NodeId anOldEdgeId = anOrigEntry.EdgeDefId;
  TopAbs_Orientation anOrigOrientation = anOrigEntry.OrientationInWire;

  // Pick a different edge.
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.NbEdgeDefs();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeKind::Edge, aNewIdx);

  myGraph.ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, true);

  const BRepGraph_TopoNode::WireDef::EdgeEntry& aNewEntry = myGraph.WireDef(0).OrderedEdges.Value(0);
  EXPECT_EQ(aNewEntry.EdgeDefId, aNewEdgeId);

  // Orientation should be flipped relative to original.
  TopAbs_Orientation anExpected = (anOrigOrientation == TopAbs_FORWARD)
                                    ? TopAbs_REVERSED
                                    : TopAbs_FORWARD;
  EXPECT_EQ(aNewEntry.OrientationInWire, anExpected);
}

TEST_F(BRepGraphTest, MutableVertex_ChangePoint_Verified)
{
  BRepGraph_TopoNode::VertexDef& aMutVert = myGraph.MutableVertexDef(0);
  aMutVert.Point = gp_Pnt(99.0, 99.0, 99.0);

  const BRepGraph_TopoNode::VertexDef& aVert = myGraph.VertexDef(0);
  EXPECT_NEAR(aVert.Point.X(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Y(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Z(), 99.0, Precision::Confusion());
}

TEST_F(BRepGraphTest, RemoveRelEdges_AddThenRemove_GoneFromOutEdges)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  myGraph.AddRelEdge(anEdge0, anEdge1, BRepGraph_RelKind::UserDefined);

  int aBefore = 0;
  myGraph.ForEachOutEdgeOfKind(anEdge0, BRepGraph_RelKind::UserDefined,
    [&](const BRepGraph_RelEdge&) { ++aBefore; });
  EXPECT_GE(aBefore, 1);

  myGraph.RemoveRelEdges(anEdge0, anEdge1, BRepGraph_RelKind::UserDefined);

  int anAfter = 0;
  myGraph.ForEachOutEdgeOfKind(anEdge0, BRepGraph_RelKind::UserDefined,
    [&](const BRepGraph_RelEdge&) { ++anAfter; });
  EXPECT_EQ(anAfter, 0);
}

TEST_F(BRepGraphTest, InEdgesOfKind_UserDefined_ReverseLookup)
{
  BRepGraph_NodeId anEdgeA(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdgeB(BRepGraph_NodeKind::Edge, 1);

  myGraph.AddRelEdge(anEdgeA, anEdgeB, BRepGraph_RelKind::UserDefined);

  bool isFound = false;
  myGraph.ForEachInEdgeOfKind(anEdgeB, BRepGraph_RelKind::UserDefined,
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
  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  gp_Pnt aCentroid = myGraph.Centroid(aSolidId);

  // 10x20x30 box at origin: centroid approximately at (5, 10, 15).
  EXPECT_NEAR(aCentroid.X(), 5.0, 0.5);
  EXPECT_NEAR(aCentroid.Y(), 10.0, 0.5);
  EXPECT_NEAR(aCentroid.Z(), 15.0, 0.5);
}

TEST_F(BRepGraphTest, GlobalTransform_FaceInsideSolid_NonNull)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  gp_Trsf aTrsf = myGraph.GlobalTransform(aFaceId);

  // For a box at origin the transform should not be negative.
  EXPECT_FALSE(aTrsf.IsNegative());
}

TEST_F(BRepGraphTest, CurveOf_EdgesOnCurve_Bidirectional)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.EdgeDef(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;

    BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, anEdgeIdx);
    BRepGraph_NodeId aCurveId = myGraph.CurveOf(anEdgeId);
    ASSERT_TRUE(aCurveId.IsValid()) << "CurveOf invalid for edge " << anEdgeIdx;

    const NCollection_Vector<BRepGraph_NodeId>& aUsers = myGraph.EdgesOnCurve(aCurveId);
    bool isFound = false;
    for (int anIdx = 0; anIdx < aUsers.Length(); ++anIdx)
    {
      if (aUsers.Value(anIdx) == anEdgeId)
      {
        isFound = true;
        break;
      }
    }
    EXPECT_TRUE(isFound) << "Edge " << anEdgeIdx << " not found in EdgesOnCurve()";
  }
}

// ===================================================================
// Group 5: Cache Invalidation
// ===================================================================

TEST_F(BRepGraphTest, Invalidate_BBox_RecomputesSameResult)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);

  const Bnd_Box aBox1 = myGraph.BoundingBox(aFaceId);
  ASSERT_FALSE(aBox1.IsVoid());

  myGraph.Invalidate(aFaceId);

  const Bnd_Box aBox2 = myGraph.BoundingBox(aFaceId);
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
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);

  // Compute initial values.
  const Bnd_Box aFaceBox1 = myGraph.BoundingBox(aFaceId);
  ASSERT_FALSE(aFaceBox1.IsVoid());

  // Get an edge from this face's outer wire.
  const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(0);
  ASSERT_FALSE(aFace.Usages.IsEmpty());
  const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
  ASSERT_TRUE(aFaceUsage.OuterWireUsage.IsValid());
  const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(
    myGraph.WireUsage(aFaceUsage.OuterWireUsage.Index).DefId.Index);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anEdgeId = aWire.OrderedEdges.Value(0).EdgeDefId;

  Bnd_Box anEdgeBox1 = myGraph.BoundingBox(anEdgeId);
  ASSERT_FALSE(anEdgeBox1.IsVoid());

  // Invalidate subgraph from face.
  myGraph.InvalidateSubgraph(aFaceId);

  // Recompute: should not crash and should produce same values.
  const Bnd_Box aFaceBox2 = myGraph.BoundingBox(aFaceId);
  ASSERT_FALSE(aFaceBox2.IsVoid());

  const Bnd_Box anEdgeBox2 = myGraph.BoundingBox(anEdgeId);
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
  EXPECT_EQ(aCounter.load(), myGraph.NbFaceDefs());
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
  EXPECT_EQ(aGraph.NbFaceDefs(), 3);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 3);
}

TEST_F(BRepGraphTest, DetectToleranceConflicts_ManualConflict_Detected)
{
  // Find an edge that shares a curve with another edge.
  bool isConflictSetUp = false;
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.EdgeDef(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;

    BRepGraph_NodeId aCurveId = myGraph.CurveOf(
      BRepGraph_NodeId(BRepGraph_NodeKind::Edge, anEdgeIdx));
    if (!aCurveId.IsValid())
      continue;

    const NCollection_Vector<BRepGraph_NodeId>& aUsers = myGraph.EdgesOnCurve(aCurveId);
    if (aUsers.Length() < 2)
      continue;

    // Set very different tolerances on two edges sharing the same curve.
    int aFirstIdx = aUsers.Value(0).Index;
    int aSecondIdx = aUsers.Value(1).Index;

    myGraph.MutableEdgeDef(aFirstIdx).Tolerance = 0.001;
    myGraph.MutableEdgeDef(aSecondIdx).Tolerance = 1.0;

    isConflictSetUp = true;
    break;
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
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const int aKey = BRepGraph_UserAttribute::AllocateKey();
  auto anAttr = std::make_shared<BRepGraph_TypedAttribute<int>>(42);

  myGraph.SetUserAttribute(aFaceId, aKey, anAttr);
  ASSERT_NE(myGraph.GetUserAttribute(aFaceId, aKey), nullptr);

  myGraph.RemoveUserAttribute(aFaceId, aKey);
  EXPECT_EQ(myGraph.GetUserAttribute(aFaceId, aKey), nullptr);
}

TEST(BRepGraphErrorTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.NbSolidDefs(), 0);
  EXPECT_EQ(aGraph.NbShellDefs(), 0);
  EXPECT_EQ(aGraph.NbFaceDefs(), 0);
  EXPECT_EQ(aGraph.NbWireDefs(), 0);
  EXPECT_EQ(aGraph.NbEdgeDefs(), 0);
  EXPECT_EQ(aGraph.NbVertexDefs(), 0);
}

TEST_F(BRepGraphTest, TopoNode_GenericLookup_MatchesTypedAccess)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const BRepGraph_TopoNode::BaseDef* aBase = myGraph.TopoDef(aFaceId);
  ASSERT_NE(aBase, nullptr);
  EXPECT_EQ(aBase->Id, myGraph.FaceDef(0).Id);

  // Geometry kind should return nullptr.
  BRepGraph_NodeId aSurfId(BRepGraph_NodeKind::Surface, 0);
  const BRepGraph_TopoNode::BaseDef* aSurfBase = myGraph.TopoDef(aSurfId);
  EXPECT_EQ(aSurfBase, nullptr);
}

// ===================================================================
// Group 9: Node Counts and Identity
// ===================================================================

TEST_F(BRepGraphTest, NbNodes_Box_TotalCount)
{
  // NbNodes should equal sum of all per-kind counts.
  size_t anExpected = static_cast<size_t>(myGraph.NbSolidDefs())
                    + myGraph.NbShellDefs()
                    + myGraph.NbFaceDefs()
                    + myGraph.NbWireDefs()
                    + myGraph.NbEdgeDefs()
                    + myGraph.NbVertexDefs()
                    + myGraph.NbSurfaces()
                    + myGraph.NbCurves()
                    + myGraph.NbPCurves();
  EXPECT_EQ(myGraph.NbNodes(), anExpected);
}

TEST_F(BRepGraphTest, HasUID_ValidFace_ReturnsTrue)
{
  const BRepGraph_UID& aUID = myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Face, 0));
  EXPECT_TRUE(myGraph.HasUID(aUID));
}

TEST_F(BRepGraphTest, HasUID_InvalidUID_ReturnsFalse)
{
  BRepGraph_UID anInvalidUID = BRepGraph_UID::Invalid();
  EXPECT_FALSE(myGraph.HasUID(anInvalidUID));
}

TEST_F(BRepGraphTest, NodeIdFromUID_InvalidUID_ReturnsInvalid)
{
  BRepGraph_UID anInvalidUID = BRepGraph_UID::Invalid();
  BRepGraph_NodeId aResolved = myGraph.NodeIdFromUID(anInvalidUID);
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
  EXPECT_EQ(aGraph.NbFaceDefs(), 6);
  EXPECT_FALSE(aGraph.Allocator().IsNull());
}

// ===================================================================
// Group 10: Topology Structure
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_BoxOuterWires)
{
  // All outer wires of a box face should be closed.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    ASSERT_FALSE(aFace.Usages.IsEmpty());
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    ASSERT_TRUE(aFaceUsage.OuterWireUsage.IsValid());
    const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(
      myGraph.WireUsage(aFaceUsage.OuterWireUsage.Index).DefId.Index);
    EXPECT_TRUE(aWire.IsClosed) << "Outer wire of face " << aFaceIdx << " should be closed";
  }
}

TEST_F(BRepGraphTest, Face_InnerWireUsages_BoxHasNone)
{
  // Box faces have no holes, so InnerWireUsages should be empty.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    ASSERT_FALSE(aFace.Usages.IsEmpty());
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    EXPECT_EQ(aFaceUsage.InnerWireUsages.Length(), 0)
      << "Box face " << aFaceIdx << " should have no inner wires";
  }
}

TEST_F(BRepGraphTest, Face_Orientation_ValidValue)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    ASSERT_FALSE(aFace.Usages.IsEmpty());
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    TopAbs_Orientation anOri = aFaceUsage.Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED)
      << "Face " << aFaceIdx << " has unexpected orientation " << anOri;
  }
}

TEST_F(BRepGraphTest, Shell_ContainsSixFaces)
{
  ASSERT_EQ(myGraph.NbShellDefs(), 1);
  const BRepGraph_TopoNode::ShellDef& aShell = myGraph.ShellDef(0);
  ASSERT_FALSE(aShell.Usages.IsEmpty());
  const BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph.ShellUsage(aShell.Usages.First().Index);
  EXPECT_EQ(aShellUsage.FaceUsages.Length(), 6);
}

TEST_F(BRepGraphTest, Solid_ContainsOneShell)
{
  ASSERT_EQ(myGraph.NbSolidDefs(), 1);
  const BRepGraph_TopoNode::SolidDef& aSolid = myGraph.SolidDef(0);
  ASSERT_FALSE(aSolid.Usages.IsEmpty());
  const BRepGraph_TopoNode::SolidUsage& aSolidUsage = myGraph.SolidUsage(aSolid.Usages.First().Index);
  EXPECT_EQ(aSolidUsage.ShellUsages.Length(), 1);
}

TEST_F(BRepGraphTest, Edge_ParamRange_ValidBounds)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.EdgeDef(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;
    EXPECT_LT(anEdge.ParamFirst, anEdge.ParamLast)
      << "Edge " << anEdgeIdx << " has invalid parameter range ["
      << anEdge.ParamFirst << ", " << anEdge.ParamLast << "]";
  }
}

TEST_F(BRepGraphTest, Vertex_TolerancePositive)
{
  for (int aVtxIdx = 0; aVtxIdx < myGraph.NbVertexDefs(); ++aVtxIdx)
  {
    EXPECT_GT(myGraph.VertexDef(aVtxIdx).Tolerance, 0.0)
      << "Vertex " << aVtxIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Edge_TolerancePositive)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    EXPECT_GT(myGraph.EdgeDef(anEdgeIdx).Tolerance, 0.0)
      << "Edge " << anEdgeIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Face_ToleranceNonNegative)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    EXPECT_GE(myGraph.FaceDef(aFaceIdx).Tolerance, 0.0)
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
  BRepGraph_NodeId aSolidId(BRepGraph_NodeKind::Solid, 0);
  Bnd_Box aSolidBox = myGraph.BoundingBox(aSolidId);
  ASSERT_FALSE(aSolidBox.IsVoid());

  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    Bnd_Box aFaceBox = myGraph.BoundingBox(aFaceId);
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
  const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(0);
  ASSERT_FALSE(aFace.Usages.IsEmpty());
  const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
  ASSERT_TRUE(aFaceUsage.OuterWireUsage.IsValid());
  const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(
    myGraph.WireUsage(aFaceUsage.OuterWireUsage.Index).DefId.Index);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  Bnd_Box aFaceBox = myGraph.BoundingBox(aFaceId);
  ASSERT_FALSE(aFaceBox.IsVoid());

  BRepGraph_NodeId anEdgeId = aWire.OrderedEdges.Value(0).EdgeDefId;
  Bnd_Box anEdgeBox = myGraph.BoundingBox(anEdgeId);
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    gp_Pnt aCentroid = myGraph.Centroid(aFaceId);
    Bnd_Box aBox = myGraph.BoundingBox(aFaceId);
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

TEST_F(BRepGraphTest, MutableWire_ModifyClosure_Verified)
{
  BRepGraph_TopoNode::WireDef& aMutWire = myGraph.MutableWireDef(0);
  bool anOrigClosed = aMutWire.IsClosed;
  aMutWire.IsClosed = !anOrigClosed;

  EXPECT_EQ(myGraph.WireDef(0).IsClosed, !anOrigClosed);

  // Restore original state.
  aMutWire.IsClosed = anOrigClosed;
  EXPECT_EQ(myGraph.WireDef(0).IsClosed, anOrigClosed);
}

TEST_F(BRepGraphTest, MultipleUserAttributes_SameNode_Independent)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);

  const int aKey1 = BRepGraph_UserAttribute::AllocateKey();
  const int aKey2 = BRepGraph_UserAttribute::AllocateKey();
  EXPECT_NE(aKey1, aKey2);

  auto anAttr1 = std::make_shared<BRepGraph_TypedAttribute<int>>(100);
  auto anAttr2 = std::make_shared<BRepGraph_TypedAttribute<double>>(2.718);

  myGraph.SetUserAttribute(aFaceId, aKey1, anAttr1);
  myGraph.SetUserAttribute(aFaceId, aKey2, anAttr2);

  auto aRetrieved1 = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<int>>(
    myGraph.GetUserAttribute(aFaceId, aKey1));
  auto aRetrieved2 = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<double>>(
    myGraph.GetUserAttribute(aFaceId, aKey2));

  ASSERT_NE(aRetrieved1, nullptr);
  ASSERT_NE(aRetrieved2, nullptr);
  EXPECT_EQ(aRetrieved1->UncheckedValue(), 100);
  EXPECT_NEAR(aRetrieved2->UncheckedValue(), 2.718, 1.0e-10);

  // Remove one; the other should remain.
  myGraph.RemoveUserAttribute(aFaceId, aKey1);
  EXPECT_EQ(myGraph.GetUserAttribute(aFaceId, aKey1), nullptr);
  EXPECT_NE(myGraph.GetUserAttribute(aFaceId, aKey2), nullptr);
}

TEST_F(BRepGraphTest, InvalidateUserAttribute_SpecificKey)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const int aKey = BRepGraph_UserAttribute::AllocateKey();
  auto anAttr = std::make_shared<BRepGraph_TypedAttribute<int>>(42);
  myGraph.SetUserAttribute(aFaceId, aKey, anAttr);

  // Invalidate should not remove, just mark dirty.
  myGraph.InvalidateUserAttribute(aFaceId, aKey);

  BRepGraph_UserAttrPtr aRetrieved = myGraph.GetUserAttribute(aFaceId, aKey);
  EXPECT_NE(aRetrieved, nullptr); // still present
}

TEST_F(BRepGraphTest, UserAttribute_OnEdgeNode)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  const int aKey = BRepGraph_UserAttribute::AllocateKey();
  auto anAttr = std::make_shared<BRepGraph_TypedAttribute<double>>(1.5);

  myGraph.SetUserAttribute(anEdgeId, aKey, anAttr);

  auto aRetrieved = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<double>>(
    myGraph.GetUserAttribute(anEdgeId, aKey));
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
  EXPECT_EQ(aGraph.NbSolidDefs(), 0);
  EXPECT_EQ(aGraph.NbShellDefs(), 0);
  EXPECT_EQ(aGraph.NbFaceDefs(), 1);
  EXPECT_EQ(aGraph.NbWireDefs(), 1);
  EXPECT_EQ(aGraph.NbEdgeDefs(), 4);
  EXPECT_EQ(aGraph.NbVertexDefs(), 4);
}

TEST(BRepGraphBuildTest, Build_Shell_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.NbSolidDefs(), 0);
  EXPECT_EQ(aGraph.NbShellDefs(), 1);
  EXPECT_EQ(aGraph.NbFaceDefs(), 6);
  EXPECT_EQ(aGraph.NbEdgeDefs(), 12);
  EXPECT_EQ(aGraph.NbVertexDefs(), 8);
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
  EXPECT_EQ(aGraph.NbSolidDefs(), 2);
  EXPECT_EQ(aGraph.NbShellDefs(), 2);
  EXPECT_EQ(aGraph.NbFaceDefs(), 12);
}

TEST(BRepGraphBuildTest, ReconstructShape_ShellRoot_SameFaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.NbShellDefs(), 1);

  BRepGraph_NodeId aShellId(BRepGraph_NodeKind::Shell, 0);
  const TopoDS_Shape aReconstructed = aGraph.ReconstructShape(aShellId);

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aReconstructed, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

// ===================================================================
// Group 15: UID Properties
// ===================================================================

TEST_F(BRepGraphTest, UID_FaceIsTopology_SurfaceIsGeometry)
{
  EXPECT_TRUE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Face, 0)).IsTopology());
  EXPECT_FALSE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Face, 0)).IsGeometry());

  EXPECT_FALSE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Surface, 0)).IsTopology());
  EXPECT_TRUE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Surface, 0)).IsGeometry());
}

TEST_F(BRepGraphTest, UID_AllGeomNodesHaveValidUIDs)
{
  for (int aSurfIdx = 0; aSurfIdx < myGraph.NbSurfaces(); ++aSurfIdx)
  {
    EXPECT_TRUE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Surface, aSurfIdx)).IsValid())
      << "Surface " << aSurfIdx << " has invalid UID";
  }
  for (int aCurveIdx = 0; aCurveIdx < myGraph.NbCurves(); ++aCurveIdx)
  {
    EXPECT_TRUE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::Curve, aCurveIdx)).IsValid())
      << "Curve " << aCurveIdx << " has invalid UID";
  }
  for (int aPCurveIdx = 0; aPCurveIdx < myGraph.NbPCurves(); ++aPCurveIdx)
  {
    EXPECT_TRUE(myGraph.UIDOf(BRepGraph_NodeId(BRepGraph_NodeKind::PCurve, aPCurveIdx)).IsValid())
      << "PCurve " << aPCurveIdx << " has invalid UID";
  }
}

TEST_F(BRepGraphTest, Wire_OwnerFaceUsage_MatchesFaceOuterWire)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    if (aFace.Usages.IsEmpty())
      continue;
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    if (!aFaceUsage.OuterWireUsage.IsValid())
      continue;
    const BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph.WireUsage(aFaceUsage.OuterWireUsage.Index);
    EXPECT_EQ(myGraph.FaceUsage(aWireUsage.OwnerFaceUsage.Index).DefId.Index, aFaceIdx)
      << "Wire usage " << aFaceUsage.OuterWireUsage.Index << " owner doesn't match face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, Wire_OrderedEdges_FourEdgesPerBoxFace)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaceDefs(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.FaceDef(aFaceIdx);
    ASSERT_FALSE(aFace.Usages.IsEmpty());
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.FaceUsage(aFace.Usages.First().Index);
    ASSERT_TRUE(aFaceUsage.OuterWireUsage.IsValid());
    const BRepGraph_TopoNode::WireDef& aWire = myGraph.WireDef(
      myGraph.WireUsage(aFaceUsage.OuterWireUsage.Index).DefId.Index);
    EXPECT_EQ(aWire.OrderedEdges.Length(), 4)
      << "Box face " << aFaceIdx << " should have 4 edges in its outer wire";
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
  const int aBefore = myGraph.NbHistoryRecords();

  myGraph.SetHistoryEnabled(false);

  BRepGraph_NodeId                      anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId                      anEdge1(BRepGraph_NodeKind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.RecordHistory("ShouldNotRecord", anEdge0, aRepl);

  EXPECT_EQ(myGraph.NbHistoryRecords(), aBefore);
}

TEST_F(BRepGraphTest, RecordHistory_ReEnabled_RecordsAgain)
{
  myGraph.SetHistoryEnabled(false);

  const int aBefore = myGraph.NbHistoryRecords();

  BRepGraph_NodeId                      anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId                      anEdge1(BRepGraph_NodeKind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.RecordHistory("Skipped", anEdge0, aRepl);
  EXPECT_EQ(myGraph.NbHistoryRecords(), aBefore);

  myGraph.SetHistoryEnabled(true);
  myGraph.RecordHistory("Recorded", anEdge0, aRepl);
  EXPECT_EQ(myGraph.NbHistoryRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.HistoryRecord(aBefore).OperationName.IsEqual("Recorded"));
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_NoHistoryNoDerivedEdges)
{
  myGraph.SetHistoryEnabled(false);

  const int aNbHistBefore = myGraph.NbHistoryRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "NoHistory");

  // No history records should be added.
  EXPECT_EQ(myGraph.NbHistoryRecords(), aNbHistBefore);
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_ModifierStillRuns)
{
  myGraph.SetHistoryEnabled(false);

  bool isModifierCalled = false;
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);

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

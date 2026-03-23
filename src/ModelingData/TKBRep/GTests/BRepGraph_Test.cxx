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
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom2d_Line.hxx>
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
  EXPECT_EQ(myGraph.NbSolids(), 1);
  EXPECT_EQ(myGraph.NbShells(), 1);
  EXPECT_EQ(myGraph.NbFaces(), 6);
  EXPECT_EQ(myGraph.NbWires(), 6);
  EXPECT_EQ(myGraph.NbEdges(), 12);
  EXPECT_EQ(myGraph.NbVertices(), 8);
}

TEST_F(BRepGraphTest, Build_SimpleBox_SurfaceCount)
{
  EXPECT_EQ(myGraph.NbSurfaces(), 6);
}

TEST_F(BRepGraphTest, Face_SurfNodeId_IsValid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = myGraph.Face(aFaceIdx);
    EXPECT_TRUE(aFace.SurfNodeId.IsValid()) << "Face " << aFaceIdx << " has invalid SurfNodeId";
  }
}

TEST_F(BRepGraphTest, Edge_CurveAndVertices_AreValid)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::Edge& anEdge = myGraph.Edge(anEdgeIdx);
    if (!anEdge.IsDegenerate)
    {
      EXPECT_TRUE(anEdge.CurveNodeId.IsValid())
        << "Edge " << anEdgeIdx << " has invalid CurveNodeId";
    }
    EXPECT_TRUE(anEdge.StartVertexId.IsValid())
      << "Edge " << anEdgeIdx << " has invalid StartVertexId";
    EXPECT_TRUE(anEdge.EndVertexId.IsValid()) << "Edge " << anEdgeIdx << " has invalid EndVertexId";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWire_Exists)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = myGraph.Face(aFaceIdx);
    EXPECT_TRUE(aFace.OuterWireId.IsValid()) << "Face " << aFaceIdx << " has no OuterWireId";
  }
}

TEST_F(BRepGraphTest, SurfaceOf_Bidirectional)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = myGraph.Face(aFaceIdx);
    BRepGraph_NodeId                aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);

    if (!aFace.OuterWireId.IsValid())
      continue;
    const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(aFace.OuterWireId.Index);
    for (int anEdgeIter = 0; anEdgeIter < aWire.OrderedEdges.Length(); ++anEdgeIter)
    {
      BRepGraph_NodeId                anEdgeId = aWire.OrderedEdges.Value(anEdgeIter).EdgeId;
      const BRepGraph_TopoNode::Edge& anEdge   = myGraph.Edge(anEdgeId.Index);
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
  for (int aSolidIdx = 0; aSolidIdx < myGraph.NbSolids(); ++aSolidIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Solid(aSolidIdx).UID));
  for (int aShellIdx = 0; aShellIdx < myGraph.NbShells(); ++aShellIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Shell(aShellIdx).UID));
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Face(aFaceIdx).UID));
  for (int aWireIdx = 0; aWireIdx < myGraph.NbWires(); ++aWireIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Wire(aWireIdx).UID));
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Edge(anEdgeIdx).UID));
  for (int aVertexIdx = 0; aVertexIdx < myGraph.NbVertices(); ++aVertexIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Vertex(aVertexIdx).UID));
  for (int aSurfIdx = 0; aSurfIdx < myGraph.NbSurfaces(); ++aSurfIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Surf(aSurfIdx).UID));
  for (int aCurveIdx = 0; aCurveIdx < myGraph.NbCurves(); ++aCurveIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.Curve(aCurveIdx).UID));
  for (int aPCurveIdx = 0; aPCurveIdx < myGraph.NbPCurves(); ++aPCurveIdx)
    EXPECT_TRUE(aUIDSet.Add(myGraph.PCurve(aPCurveIdx).UID));
}

TEST_F(BRepGraphTest, UID_NodeIdRoundTrip)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace     = myGraph.Face(aFaceIdx);
    BRepGraph_NodeId                aResolved = myGraph.NodeIdFromUID(aFace.UID);
    EXPECT_EQ(aResolved, aFace.Id) << "Round trip failed for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, BoundingBox_NonVoid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);
    Bnd_Box          aBox = myGraph.BoundingBox(aFaceId);
    EXPECT_FALSE(aBox.IsVoid()) << "BoundingBox is void for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, SameDomainFaces_Box_Empty)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
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
  EXPECT_EQ(aGraph.NbFaces(), 2);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = aGraph.Decompose();
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
  aGraph.Build(aBox);
  const size_t   aCountAfterFirst = aGraph.NbNodes();
  const uint32_t aGen1            = aGraph.Generation();

  aGraph.Build(aBox);
  const uint32_t aGen2 = aGraph.Generation();

  EXPECT_GT(aGen2, aGen1);

  size_t aMinCounter = SIZE_MAX;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.NbFaces(); ++aFaceIdx)
  {
    if (aGraph.Face(aFaceIdx).UID.Counter() < aMinCounter)
      aMinCounter = aGraph.Face(aFaceIdx).UID.Counter();
  }
  EXPECT_GE(aMinCounter, aCountAfterFirst);
}

TEST_F(BRepGraphTest, DetectMissingPCurves_ValidBox_Empty)
{
  auto aMissing = myGraph.DetectMissingPCurves();
  EXPECT_EQ(aMissing.Length(), 0);
}

TEST_F(BRepGraphTest, DetectDegenerateWires_ValidBox_Empty)
{
  auto aDegenerate = myGraph.DetectDegenerateWires();
  EXPECT_EQ(aDegenerate.Length(), 0);
}

TEST_F(BRepGraphTest, MutableEdge_ModifyTolerance)
{
  double                    anOrigTol = myGraph.Edge(0).Tolerance;
  BRepGraph_TopoNode::Edge& anEdge    = myGraph.MutableEdge(0);
  anEdge.Tolerance                    = anOrigTol * 2.0;
  EXPECT_NEAR(myGraph.Edge(0).Tolerance, anOrigTol * 2.0, 1.0e-15);
}

TEST_F(BRepGraphTest, FaceCountForEdge_SharedEdge)
{
  // In a box, each non-degenerate edge is shared by exactly 2 faces.
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
  {
    if (!myGraph.Edge(anEdgeIdx).IsDegenerate)
    {
      int aCount = myGraph.FaceCountForEdge(anEdgeIdx);
      EXPECT_EQ(aCount, 2) << "Edge " << anEdgeIdx << " should be shared by 2 faces";
    }
  }
}

TEST_F(BRepGraphTest, FreeEdges_ClosedBox_Empty)
{
  NCollection_Vector<BRepGraph_NodeId> aFree = myGraph.FreeEdges();
  EXPECT_EQ(aFree.Length(), 0);
}

TEST_F(BRepGraphTest, AddRelEdge_UserDefined)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  int aBefore = myGraph.NbRelEdges();
  myGraph.AddRelEdge(anEdge0, anEdge1, BRepGraph_RelKind::UserDefined);
  EXPECT_EQ(myGraph.NbRelEdges(), aBefore + 1);

  NCollection_Vector<int> aOut = myGraph.OutEdgesOfKind(anEdge0, BRepGraph_RelKind::UserDefined);
  EXPECT_EQ(aOut.Length(), 1);
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
  EXPECT_TRUE(myGraph.History(aBefore).OperationName.IsEqual("TestOp"));
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Substitution)
{
  // Get the first wire and its first edge.
  const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anOldEdge = aWire.OrderedEdges.Value(0).EdgeId;

  // Pick a different edge to substitute.
  BRepGraph_NodeId aNewEdge(BRepGraph_NodeKind::Edge, (anOldEdge.Index + 1) % myGraph.NbEdges());

  myGraph.ReplaceEdgeInWire(0, anOldEdge, aNewEdge, false);

  // Verify the substitution.
  EXPECT_EQ(myGraph.Wire(0).OrderedEdges.Value(0).EdgeId, aNewEdge);
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

  EXPECT_EQ(aParGraph.NbSolids(), aSeqGraph.NbSolids());
  EXPECT_EQ(aParGraph.NbShells(), aSeqGraph.NbShells());
  EXPECT_EQ(aParGraph.NbFaces(), aSeqGraph.NbFaces());
  EXPECT_EQ(aParGraph.NbWires(), aSeqGraph.NbWires());
  EXPECT_EQ(aParGraph.NbEdges(), aSeqGraph.NbEdges());
  EXPECT_EQ(aParGraph.NbVertices(), aSeqGraph.NbVertices());
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
  EXPECT_EQ(aGraph.NbFaces(), 12);
}

// ===================================================================
// Group 1: Shape Round-Trip
// ===================================================================

TEST_F(BRepGraphTest, ReconstructFace_EachBoxFace_SameSubShapeCounts)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const TopoDS_Shape& anOrigFace = myGraph.Face(aFaceIdx).OriginalShape;
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
  const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anOldEdgeId = aWire.OrderedEdges.Value(0).EdgeId;

  // Pick a different edge.
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.NbEdges();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeKind::Edge, aNewIdx);

  myGraph.ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, false);

  // Reconstruct the face owning this wire.
  int aFaceIdx = aWire.OwnerFaceId.Index;
  const TopoDS_Shape aReconstructed = myGraph.ReconstructFace(aFaceIdx);

  // Collect TShapes from reconstructed face.
  const TopoDS_Shape& aNewEdgeShape = myGraph.Edge(aNewIdx).OriginalShape;
  const TopoDS_Shape& anOldEdgeShape = myGraph.Edge(anOldEdgeId.Index).OriginalShape;

  bool isNewFound = false;
  bool isOldFound = false;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    if (anExp.Current().TShape() == aNewEdgeShape.TShape())
      isNewFound = true;
    if (anExp.Current().TShape() == anOldEdgeShape.TShape())
      isOldFound = true;
  }
  EXPECT_TRUE(isNewFound) << "New edge TShape not found in reconstructed face";
  EXPECT_FALSE(isOldFound) << "Old edge TShape still present in reconstructed face";
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

TEST_F(BRepGraphTest, ReconstructShape_FaceRoot_ReturnsOriginalShape)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const TopoDS_Shape aReconstructed = myGraph.ReconstructShape(aFaceId);
  const TopoDS_Shape& anOriginal = myGraph.Face(0).OriginalShape;
  EXPECT_TRUE(aReconstructed.IsSame(anOriginal));
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
    EXPECT_GT(myGraph.History(anIdx).SequenceNumber,
              myGraph.History(anIdx - 1).SequenceNumber)
      << "SequenceNumber not monotonically increasing at index " << anIdx;
  }

  EXPECT_TRUE(myGraph.History(aBefore).OperationName.IsEqual("OpA"));
  EXPECT_TRUE(myGraph.History(aBefore + 1).OperationName.IsEqual("OpB"));
  EXPECT_TRUE(myGraph.History(aBefore + 2).OperationName.IsEqual("OpC"));
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
  const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(0);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);

  const BRepGraph_TopoNode::Wire::EdgeEntry& anOrigEntry = aWire.OrderedEdges.Value(0);
  BRepGraph_NodeId anOldEdgeId = anOrigEntry.EdgeId;
  TopAbs_Orientation anOrigOrientation = anOrigEntry.OrientationInWire;

  // Pick a different edge.
  int aNewIdx = (anOldEdgeId.Index + 1) % myGraph.NbEdges();
  BRepGraph_NodeId aNewEdgeId(BRepGraph_NodeKind::Edge, aNewIdx);

  myGraph.ReplaceEdgeInWire(0, anOldEdgeId, aNewEdgeId, true);

  const BRepGraph_TopoNode::Wire::EdgeEntry& aNewEntry = myGraph.Wire(0).OrderedEdges.Value(0);
  EXPECT_EQ(aNewEntry.EdgeId, aNewEdgeId);

  // Orientation should be flipped relative to original.
  TopAbs_Orientation anExpected = (anOrigOrientation == TopAbs_FORWARD)
                                    ? TopAbs_REVERSED
                                    : TopAbs_FORWARD;
  EXPECT_EQ(aNewEntry.OrientationInWire, anExpected);
}

TEST_F(BRepGraphTest, MutableVertex_ChangePoint_Verified)
{
  BRepGraph_TopoNode::Vertex& aMutVert = myGraph.MutableVertex(0);
  aMutVert.Point = gp_Pnt(99.0, 99.0, 99.0);

  const BRepGraph_TopoNode::Vertex& aVert = myGraph.Vertex(0);
  EXPECT_NEAR(aVert.Point.X(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Y(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Z(), 99.0, Precision::Confusion());
}

TEST_F(BRepGraphTest, RemoveRelEdges_AddThenRemove_GoneFromOutEdges)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  myGraph.AddRelEdge(anEdge0, anEdge1, BRepGraph_RelKind::UserDefined);

  NCollection_Vector<int> aBefore = myGraph.OutEdgesOfKind(anEdge0,
                                                           BRepGraph_RelKind::UserDefined);
  EXPECT_GE(aBefore.Length(), 1);

  myGraph.RemoveRelEdges(anEdge0, anEdge1, BRepGraph_RelKind::UserDefined);

  // RemoveRelEdges invalidates edges by clearing Source/Target (append-only array).
  // Verify the invalidated edge has cleared Source and Target.
  NCollection_Vector<int> anAfter = myGraph.OutEdgesOfKind(anEdge0,
                                                           BRepGraph_RelKind::UserDefined);
  for (int anIdx = 0; anIdx < anAfter.Length(); ++anIdx)
  {
    const BRepGraph_RelEdge& aRel = myGraph.RelEdge(anAfter.Value(anIdx));
    EXPECT_FALSE(aRel.Source.IsValid()) << "Removed edge should have invalid Source";
    EXPECT_FALSE(aRel.Target.IsValid()) << "Removed edge should have invalid Target";
  }
}

TEST_F(BRepGraphTest, InEdgesOfKind_UserDefined_ReverseLookup)
{
  BRepGraph_NodeId anEdgeA(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdgeB(BRepGraph_NodeKind::Edge, 1);

  myGraph.AddRelEdge(anEdgeA, anEdgeB, BRepGraph_RelKind::UserDefined);

  NCollection_Vector<int> anInEdges = myGraph.InEdgesOfKind(anEdgeB,
                                                            BRepGraph_RelKind::UserDefined);
  ASSERT_GE(anInEdges.Length(), 1);

  bool isFound = false;
  for (int anIdx = 0; anIdx < anInEdges.Length(); ++anIdx)
  {
    const BRepGraph_RelEdge& aRelEdge = myGraph.RelEdge(anInEdges.Value(anIdx));
    if (aRelEdge.Source == anEdgeA)
    {
      isFound = true;
      break;
    }
  }
  EXPECT_TRUE(isFound) << "EdgeA not found as source in InEdgesOfKind(EdgeB)";
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
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::Edge& anEdge = myGraph.Edge(anEdgeIdx);
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
  const BRepGraph_TopoNode::Face& aFace = myGraph.Face(0);
  ASSERT_TRUE(aFace.OuterWireId.IsValid());
  const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(aFace.OuterWireId.Index);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);
  BRepGraph_NodeId anEdgeId = aWire.OrderedEdges.Value(0).EdgeId;

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
  NCollection_Vector<BRepGraph_SubGraph> aSubs = myGraph.Decompose();
  ASSERT_GE(aSubs.Length(), 1);

  std::atomic<int> aCounter{0};
  for (int aSubIdx = 0; aSubIdx < aSubs.Length(); ++aSubIdx)
  {
    myGraph.ParallelForEachFace(aSubs.Value(aSubIdx),
      [&aCounter](int /*theFaceIdx*/)
      {
        aCounter.fetch_add(1, std::memory_order_relaxed);
      });
  }
  EXPECT_EQ(aCounter.load(), myGraph.NbFaces());
}

TEST_F(BRepGraphTest, ParallelForEachEdge_AllEdgesVisited)
{
  NCollection_Vector<BRepGraph_SubGraph> aSubs = myGraph.Decompose();
  ASSERT_GE(aSubs.Length(), 1);

  std::atomic<int> aCounter{0};
  const BRepGraph_SubGraph& aSub = aSubs.Value(0);
  myGraph.ParallelForEachEdge(aSub,
    [&aCounter](int /*theEdgeIdx*/)
    {
      aCounter.fetch_add(1, std::memory_order_relaxed);
    });
  EXPECT_EQ(aCounter.load(), aSub.EdgeIndices().Length());
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

  NCollection_Vector<BRepGraph_NodeId> aFreeEdges = aGraph.FreeEdges();
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
  EXPECT_EQ(aGraph.NbFaces(), 3);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = aGraph.Decompose();
  EXPECT_EQ(aSubs.Length(), 3);
}

TEST_F(BRepGraphTest, DetectToleranceConflicts_ManualConflict_Detected)
{
  // Find an edge that shares a curve with another edge.
  bool isConflictSetUp = false;
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::Edge& anEdge = myGraph.Edge(anEdgeIdx);
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

    myGraph.MutableEdge(aFirstIdx).Tolerance = 0.001;
    myGraph.MutableEdge(aSecondIdx).Tolerance = 1.0;

    isConflictSetUp = true;
    break;
  }

  if (isConflictSetUp)
  {
    NCollection_Vector<BRepGraph_NodeId> aConflicts =
      myGraph.DetectToleranceConflicts(0.5);
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

TEST_F(BRepGraphTest, Area_ThrowsProgramError)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_THROW(myGraph.Area(aFaceId), Standard_ProgramError);
}

TEST_F(BRepGraphTest, Length_ThrowsProgramError)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  EXPECT_THROW(myGraph.Length(anEdgeId), Standard_ProgramError);
}

TEST(BRepGraphErrorTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.NbSolids(), 0);
  EXPECT_EQ(aGraph.NbShells(), 0);
  EXPECT_EQ(aGraph.NbFaces(), 0);
  EXPECT_EQ(aGraph.NbWires(), 0);
  EXPECT_EQ(aGraph.NbEdges(), 0);
  EXPECT_EQ(aGraph.NbVertices(), 0);
}

TEST_F(BRepGraphTest, TopoNode_GenericLookup_MatchesTypedAccess)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const BRepGraph_TopoNode::Base* aBase = myGraph.TopoNode(aFaceId);
  ASSERT_NE(aBase, nullptr);
  EXPECT_EQ(aBase->UID, myGraph.Face(0).UID);

  // Geometry kind should return nullptr.
  BRepGraph_NodeId aSurfId(BRepGraph_NodeKind::Surface, 0);
  const BRepGraph_TopoNode::Base* aSurfBase = myGraph.TopoNode(aSurfId);
  EXPECT_EQ(aSurfBase, nullptr);
}

// ===================================================================
// Group 9: Node Counts and Identity
// ===================================================================

TEST_F(BRepGraphTest, NbNodes_Box_TotalCount)
{
  // NbNodes should equal sum of all per-kind counts.
  size_t anExpected = static_cast<size_t>(myGraph.NbSolids())
                    + myGraph.NbShells()
                    + myGraph.NbFaces()
                    + myGraph.NbWires()
                    + myGraph.NbEdges()
                    + myGraph.NbVertices()
                    + myGraph.NbSurfaces()
                    + myGraph.NbCurves()
                    + myGraph.NbPCurves();
  EXPECT_EQ(myGraph.NbNodes(), anExpected);
}

TEST_F(BRepGraphTest, HasUID_ValidFace_ReturnsTrue)
{
  const BRepGraph_UID& aUID = myGraph.Face(0).UID;
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
  EXPECT_EQ(aGraph.NbFaces(), 6);
  EXPECT_FALSE(aGraph.Allocator().IsNull());
}

// ===================================================================
// Group 10: Topology Structure
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_BoxOuterWires)
{
  // All outer wires of a box face should be closed.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = myGraph.Face(aFaceIdx);
    ASSERT_TRUE(aFace.OuterWireId.IsValid());
    const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(aFace.OuterWireId.Index);
    EXPECT_TRUE(aWire.IsClosed) << "Outer wire of face " << aFaceIdx << " should be closed";
  }
}

TEST_F(BRepGraphTest, Face_InnerWireIds_BoxHasNone)
{
  // Box faces have no holes, so InnerWireIds should be empty.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    EXPECT_EQ(myGraph.Face(aFaceIdx).InnerWireIds.Length(), 0)
      << "Box face " << aFaceIdx << " should have no inner wires";
  }
}

TEST_F(BRepGraphTest, Face_Orientation_ValidValue)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    TopAbs_Orientation anOri = myGraph.Face(aFaceIdx).Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED)
      << "Face " << aFaceIdx << " has unexpected orientation " << anOri;
  }
}

TEST_F(BRepGraphTest, Shell_ContainsSixFaces)
{
  ASSERT_EQ(myGraph.NbShells(), 1);
  EXPECT_EQ(myGraph.Shell(0).Faces.Length(), 6);
}

TEST_F(BRepGraphTest, Solid_ContainsOneShell)
{
  ASSERT_EQ(myGraph.NbSolids(), 1);
  EXPECT_EQ(myGraph.Solid(0).Shells.Length(), 1);
}

TEST_F(BRepGraphTest, Edge_ParamRange_ValidBounds)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::Edge& anEdge = myGraph.Edge(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;
    EXPECT_LT(anEdge.ParamFirst, anEdge.ParamLast)
      << "Edge " << anEdgeIdx << " has invalid parameter range ["
      << anEdge.ParamFirst << ", " << anEdge.ParamLast << "]";
  }
}

TEST_F(BRepGraphTest, Vertex_TolerancePositive)
{
  for (int aVtxIdx = 0; aVtxIdx < myGraph.NbVertices(); ++aVtxIdx)
  {
    EXPECT_GT(myGraph.Vertex(aVtxIdx).Tolerance, 0.0)
      << "Vertex " << aVtxIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Edge_TolerancePositive)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.NbEdges(); ++anEdgeIdx)
  {
    EXPECT_GT(myGraph.Edge(anEdgeIdx).Tolerance, 0.0)
      << "Edge " << anEdgeIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Face_ToleranceNonNegative)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    EXPECT_GE(myGraph.Face(aFaceIdx).Tolerance, 0.0)
      << "Face " << aFaceIdx << " has negative tolerance";
  }
}

// ===================================================================
// Group 11: SubGraph
// ===================================================================

TEST_F(BRepGraphTest, SubGraph_BoxSolid_AllIndicesPresent)
{
  NCollection_Vector<BRepGraph_SubGraph> aSubs = myGraph.Decompose();
  ASSERT_EQ(aSubs.Length(), 1);

  const BRepGraph_SubGraph& aSub = aSubs.Value(0);
  EXPECT_EQ(aSub.SolidIndices().Length(), 1);
  EXPECT_EQ(aSub.ShellIndices().Length(), 1);
  EXPECT_EQ(aSub.FaceIndices().Length(), 6);
  EXPECT_EQ(aSub.WireIndices().Length(), 6);
  // Edge/vertex indices may include per-wire-reference duplicates.
  EXPECT_GE(aSub.EdgeIndices().Length(), 12);
  EXPECT_GE(aSub.VertexIndices().Length(), 8);
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

  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
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
  const BRepGraph_TopoNode::Face& aFace = myGraph.Face(0);
  ASSERT_TRUE(aFace.OuterWireId.IsValid());
  const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(aFace.OuterWireId.Index);
  ASSERT_GE(aWire.OrderedEdges.Length(), 1);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  Bnd_Box aFaceBox = myGraph.BoundingBox(aFaceId);
  ASSERT_FALSE(aFaceBox.IsVoid());

  BRepGraph_NodeId anEdgeId = aWire.OrderedEdges.Value(0).EdgeId;
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
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
  BRepGraph_TopoNode::Wire& aMutWire = myGraph.MutableWire(0);
  bool anOrigClosed = aMutWire.IsClosed;
  aMutWire.IsClosed = !anOrigClosed;

  EXPECT_EQ(myGraph.Wire(0).IsClosed, !anOrigClosed);

  // Restore original state.
  aMutWire.IsClosed = anOrigClosed;
  EXPECT_EQ(myGraph.Wire(0).IsClosed, anOrigClosed);
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
  EXPECT_EQ(aGraph.NbSolids(), 0);
  EXPECT_EQ(aGraph.NbShells(), 0);
  EXPECT_EQ(aGraph.NbFaces(), 1);
  EXPECT_EQ(aGraph.NbWires(), 1);
  EXPECT_EQ(aGraph.NbEdges(), 4);
  EXPECT_EQ(aGraph.NbVertices(), 4);
}

TEST(BRepGraphBuildTest, Build_Shell_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.NbSolids(), 0);
  EXPECT_EQ(aGraph.NbShells(), 1);
  EXPECT_EQ(aGraph.NbFaces(), 6);
  EXPECT_EQ(aGraph.NbEdges(), 12);
  EXPECT_EQ(aGraph.NbVertices(), 8);
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
  EXPECT_EQ(aGraph.NbSolids(), 2);
  EXPECT_EQ(aGraph.NbShells(), 2);
  EXPECT_EQ(aGraph.NbFaces(), 12);
}

TEST(BRepGraphBuildTest, ReconstructShape_ShellRoot_SameFaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.NbShells(), 1);

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
  EXPECT_TRUE(myGraph.Face(0).UID.IsTopology());
  EXPECT_FALSE(myGraph.Face(0).UID.IsGeometry());

  EXPECT_FALSE(myGraph.Surf(0).UID.IsTopology());
  EXPECT_TRUE(myGraph.Surf(0).UID.IsGeometry());
}

TEST_F(BRepGraphTest, UID_AllGeomNodesHaveValidUIDs)
{
  for (int aSurfIdx = 0; aSurfIdx < myGraph.NbSurfaces(); ++aSurfIdx)
  {
    EXPECT_TRUE(myGraph.Surf(aSurfIdx).UID.IsValid())
      << "Surface " << aSurfIdx << " has invalid UID";
  }
  for (int aCurveIdx = 0; aCurveIdx < myGraph.NbCurves(); ++aCurveIdx)
  {
    EXPECT_TRUE(myGraph.Curve(aCurveIdx).UID.IsValid())
      << "Curve " << aCurveIdx << " has invalid UID";
  }
  for (int aPCurveIdx = 0; aPCurveIdx < myGraph.NbPCurves(); ++aPCurveIdx)
  {
    EXPECT_TRUE(myGraph.PCurve(aPCurveIdx).UID.IsValid())
      << "PCurve " << aPCurveIdx << " has invalid UID";
  }
}

TEST_F(BRepGraphTest, Wire_OwnerFaceId_MatchesFaceOuterWire)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = myGraph.Face(aFaceIdx);
    if (!aFace.OuterWireId.IsValid())
      continue;
    const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(aFace.OuterWireId.Index);
    EXPECT_EQ(aWire.OwnerFaceId.Index, aFaceIdx)
      << "Wire " << aFace.OuterWireId.Index << " owner doesn't match face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, Wire_OrderedEdges_FourEdgesPerBoxFace)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = myGraph.Face(aFaceIdx);
    ASSERT_TRUE(aFace.OuterWireId.IsValid());
    const BRepGraph_TopoNode::Wire& aWire = myGraph.Wire(aFace.OuterWireId.Index);
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
  EXPECT_TRUE(myGraph.History(aBefore).OperationName.IsEqual("Recorded"));
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_NoHistoryNoDerivedEdges)
{
  myGraph.SetHistoryEnabled(false);

  const int aNbHistBefore = myGraph.NbHistoryRecords();
  const int aNbRelBefore  = myGraph.NbRelEdges();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeKind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/)
  {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.ApplyModification(anEdge0, aModifier, "NoHistory");

  // No history records or DerivedFrom edges should be added.
  EXPECT_EQ(myGraph.NbHistoryRecords(), aNbHistBefore);
  EXPECT_EQ(myGraph.NbRelEdges(), aNbRelBefore);
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

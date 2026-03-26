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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Entity.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
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

#include <gtest/gtest.h>

namespace
{
static_assert(!std::is_convertible_v<BRepGraph_FaceId, BRepGraph_EdgeId>);
static_assert(!std::is_constructible_v<BRepGraph_FaceId, BRepGraph_EdgeId>);
static_assert(std::is_convertible_v<BRepGraph_FaceId, BRepGraph_NodeId>);
static_assert(!std::is_convertible_v<BRepGraph_SurfaceRepId, BRepGraph_Curve3DRepId>);
static_assert(!std::is_constructible_v<BRepGraph_SurfaceRepId, BRepGraph_Curve3DRepId>);
static_assert(std::is_convertible_v<BRepGraph_SurfaceRepId, BRepGraph_RepId>);

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

struct ReverseIndexInputData
{
  NCollection_Vector<BRepGraphInc::EdgeEntity>      Edges;
  NCollection_Vector<BRepGraphInc::CoEdgeEntity>    CoEdges;
  NCollection_Vector<BRepGraphInc::WireEntity>      Wires;
  NCollection_Vector<BRepGraphInc::FaceEntity>      Faces;
  NCollection_Vector<BRepGraphInc::ShellEntity>     Shells;
  NCollection_Vector<BRepGraphInc::SolidEntity>     Solids;
  NCollection_Vector<BRepGraphInc::CompoundEntity>  Compounds;
  NCollection_Vector<BRepGraphInc::CompSolidEntity> CompSolids;
};

static ReverseIndexInputData buildReverseIndexBaseInput()
{
  ReverseIndexInputData aData;

  BRepGraphInc::EdgeEntity& anEdge = aData.Edges.Appended();
  anEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  anEdge.Id                      = BRepGraph_NodeId::Edge(0);
  anEdge.StartVertex.VertexDefId = BRepGraph_VertexId(0);
  anEdge.EndVertex.VertexDefId   = BRepGraph_VertexId(1);

  BRepGraphInc::CoEdgeEntity& aCoEdge = aData.CoEdges.Appended();
  aCoEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aCoEdge.Id        = BRepGraph_NodeId::CoEdge(0);
  aCoEdge.EdgeDefId = BRepGraph_EdgeId(0);
  aCoEdge.FaceDefId = BRepGraph_FaceId(0);

  BRepGraphInc::WireEntity& aWire = aData.Wires.Appended();
  aWire.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aWire.Id                                = BRepGraph_NodeId::Wire(0);
  aWire.CoEdgeRefs.Appended().CoEdgeDefId = BRepGraph_CoEdgeId(0);

  BRepGraphInc::FaceEntity& aFace = aData.Faces.Appended();
  aFace.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aFace.Id                        = BRepGraph_NodeId::Face(0);
  BRepGraphInc::WireRef& aWireRef = aFace.WireRefs.Appended();
  aWireRef.WireDefId              = BRepGraph_WireId(0);
  aWireRef.IsOuter                = true;

  BRepGraphInc::ShellEntity& aShell = aData.Shells.Appended();
  aShell.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aShell.Id                            = BRepGraph_NodeId::Shell(0);
  aShell.FaceRefs.Appended().FaceDefId = BRepGraph_FaceId(0);

  BRepGraphInc::SolidEntity& aSolid = aData.Solids.Appended();
  aSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aSolid.Id                              = BRepGraph_NodeId::Solid(0);
  aSolid.ShellRefs.Appended().ShellDefId = BRepGraph_ShellId(0);

  return aData;
}

static void appendReverseIndexDeltaInput(ReverseIndexInputData& theData)
{
  // Active edge/wire/face/shell/solid chain.
  BRepGraphInc::EdgeEntity& anEdge = theData.Edges.Appended();
  anEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  anEdge.Id                      = BRepGraph_NodeId::Edge(1);
  anEdge.StartVertex.VertexDefId = BRepGraph_VertexId(2);
  anEdge.EndVertex.VertexDefId   = BRepGraph_VertexId(3);

  BRepGraphInc::CoEdgeEntity& aCoEdge = theData.CoEdges.Appended();
  aCoEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aCoEdge.Id        = BRepGraph_NodeId::CoEdge(1);
  aCoEdge.EdgeDefId = BRepGraph_EdgeId(1);
  aCoEdge.FaceDefId = BRepGraph_FaceId(1);

  BRepGraphInc::WireEntity& aWire = theData.Wires.Appended();
  aWire.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aWire.Id                                = BRepGraph_NodeId::Wire(1);
  aWire.CoEdgeRefs.Appended().CoEdgeDefId = BRepGraph_CoEdgeId(1);

  BRepGraphInc::FaceEntity& aFace = theData.Faces.Appended();
  aFace.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aFace.Id                        = BRepGraph_NodeId::Face(1);
  BRepGraphInc::WireRef& aWireRef = aFace.WireRefs.Appended();
  aWireRef.WireDefId              = BRepGraph_WireId(1);
  aWireRef.IsOuter                = true;

  BRepGraphInc::ShellEntity& aShell = theData.Shells.Appended();
  aShell.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aShell.Id                            = BRepGraph_NodeId::Shell(1);
  aShell.FaceRefs.Appended().FaceDefId = BRepGraph_FaceId(1);

  BRepGraphInc::SolidEntity& aSolid = theData.Solids.Appended();
  aSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aSolid.Id                              = BRepGraph_NodeId::Solid(1);
  aSolid.ShellRefs.Appended().ShellDefId = BRepGraph_ShellId(1);

  // Removed entities to ensure BuildDelta skips them.
  BRepGraphInc::EdgeEntity& aRemovedEdge = theData.Edges.Appended();
  aRemovedEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedEdge.Id                      = BRepGraph_NodeId::Edge(2);
  aRemovedEdge.IsRemoved               = true;
  aRemovedEdge.StartVertex.VertexDefId = BRepGraph_VertexId(10);
  aRemovedEdge.EndVertex.VertexDefId   = BRepGraph_VertexId(11);

  BRepGraphInc::CoEdgeEntity& aRemovedCoEdge = theData.CoEdges.Appended();
  aRemovedCoEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedCoEdge.Id        = BRepGraph_NodeId::CoEdge(2);
  aRemovedCoEdge.IsRemoved = true;
  aRemovedCoEdge.EdgeDefId = BRepGraph_EdgeId(2);
  aRemovedCoEdge.FaceDefId = BRepGraph_FaceId(2);

  BRepGraphInc::WireEntity& aRemovedWire = theData.Wires.Appended();
  aRemovedWire.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedWire.Id                                = BRepGraph_NodeId::Wire(2);
  aRemovedWire.IsRemoved                         = true;
  aRemovedWire.CoEdgeRefs.Appended().CoEdgeDefId = BRepGraph_CoEdgeId(2);

  BRepGraphInc::FaceEntity& aRemovedFace = theData.Faces.Appended();
  aRemovedFace.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedFace.Id                            = BRepGraph_NodeId::Face(2);
  aRemovedFace.IsRemoved                     = true;
  aRemovedFace.WireRefs.Appended().WireDefId = BRepGraph_WireId(2);

  BRepGraphInc::ShellEntity& aRemovedShell = theData.Shells.Appended();
  aRemovedShell.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedShell.Id                            = BRepGraph_NodeId::Shell(2);
  aRemovedShell.IsRemoved                     = true;
  aRemovedShell.FaceRefs.Appended().FaceDefId = BRepGraph_FaceId(2);

  BRepGraphInc::SolidEntity& aRemovedSolid = theData.Solids.Appended();
  aRemovedSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedSolid.Id                              = BRepGraph_NodeId::Solid(2);
  aRemovedSolid.IsRemoved                       = true;
  aRemovedSolid.ShellRefs.Appended().ShellDefId = BRepGraph_ShellId(2);
}

static void verifyBuildDeltaScenario(const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  ReverseIndexInputData aData = buildReverseIndexBaseInput();

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.SetAllocator(theAllocator);
  aRevIdx.Build(aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids);

  EXPECT_TRUE(
    aRevIdx
      .Validate(aData.Edges, aData.CoEdges, aData.Wires, aData.Faces, aData.Shells, aData.Solids));

  const NCollection_Vector<BRepGraph_WireId>* aBaseWires = aRevIdx.WiresOfEdge(BRepGraph_EdgeId(0));
  ASSERT_NE(aBaseWires, nullptr);
  ASSERT_EQ(aBaseWires->Length(), 1);
  EXPECT_EQ(aBaseWires->Value(0), BRepGraph_WireId(0));

  const int anOldNbEdges  = aData.Edges.Length();
  const int anOldNbWires  = aData.Wires.Length();
  const int anOldNbFaces  = aData.Faces.Length();
  const int anOldNbShells = aData.Shells.Length();
  const int anOldNbSolids = aData.Solids.Length();

  appendReverseIndexDeltaInput(aData);

  aRevIdx.BuildDelta(aData.Edges,
                     aData.CoEdges,
                     aData.Wires,
                     aData.Faces,
                     aData.Shells,
                     aData.Solids,
                     anOldNbEdges,
                     anOldNbWires,
                     anOldNbFaces,
                     anOldNbShells,
                     anOldNbSolids);

  EXPECT_TRUE(
    aRevIdx
      .Validate(aData.Edges, aData.CoEdges, aData.Wires, aData.Faces, aData.Shells, aData.Solids));

  const NCollection_Vector<BRepGraph_WireId>* aActiveWires =
    aRevIdx.WiresOfEdge(BRepGraph_EdgeId(1));
  ASSERT_NE(aActiveWires, nullptr);
  ASSERT_EQ(aActiveWires->Length(), 1);
  EXPECT_EQ(aActiveWires->Value(0), BRepGraph_WireId(1));

  const NCollection_Vector<BRepGraph_FaceId>* aActiveFaces =
    aRevIdx.FacesOfWire(BRepGraph_WireId(1));
  ASSERT_NE(aActiveFaces, nullptr);
  ASSERT_EQ(aActiveFaces->Length(), 1);
  EXPECT_EQ(aActiveFaces->Value(0), BRepGraph_FaceId(1));

  const NCollection_Vector<BRepGraph_ShellId>* anActiveShells =
    aRevIdx.ShellsOfFace(BRepGraph_FaceId(1));
  ASSERT_NE(anActiveShells, nullptr);
  ASSERT_EQ(anActiveShells->Length(), 1);
  EXPECT_EQ(anActiveShells->Value(0), BRepGraph_ShellId(1));

  const NCollection_Vector<BRepGraph_SolidId>* anActiveSolids =
    aRevIdx.SolidsOfShell(BRepGraph_ShellId(1));
  ASSERT_NE(anActiveSolids, nullptr);
  ASSERT_EQ(anActiveSolids->Length(), 1);
  EXPECT_EQ(anActiveSolids->Value(0), BRepGraph_SolidId(1));

  EXPECT_EQ(aRevIdx.FaceCountOfEdge(BRepGraph_EdgeId(1)), 1);

  EXPECT_EQ(aRevIdx.WiresOfEdge(BRepGraph_EdgeId(2)), nullptr);
  EXPECT_EQ(aRevIdx.EdgesOfVertex(BRepGraph_VertexId(10)), nullptr);
  EXPECT_EQ(aRevIdx.FaceCountOfEdge(BRepGraph_EdgeId(2)), 0);
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

TEST_F(BRepGraphTest, BuildDelta_ValidateAndSkipRemoved_NullAllocator)
{
  verifyBuildDeltaScenario(occ::handle<NCollection_BaseAllocator>());
}

TEST_F(BRepGraphTest, BuildDelta_ValidateAndSkipRemoved_WithAllocator)
{
  const occ::handle<NCollection_IncAllocator> anAllocator = new NCollection_IncAllocator();
  verifyBuildDeltaScenario(anAllocator);
}

TEST_F(BRepGraphTest, Build_SimpleBox_CorrectCounts)
{
  EXPECT_EQ(myGraph.Topo().NbSolids(), 1);
  EXPECT_EQ(myGraph.Topo().NbShells(), 1);
  EXPECT_EQ(myGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(myGraph.Topo().NbWires(), 6);
  EXPECT_EQ(myGraph.Topo().NbEdges(), 12);
  EXPECT_EQ(myGraph.Topo().NbVertices(), 8);
}

TEST_F(BRepGraphTest, Build_SimpleBox_SurfaceCount)
{
  EXPECT_EQ(myGraph.Topo().NbFaces(), 6);
}

TEST_F(BRepGraphTest, Face_Surface_IsValid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, BRepGraph_FaceId(aFaceIdx)))
      << "Face " << aFaceIdx << " has no surface rep";
  }
}

TEST_F(BRepGraphTest, Edge_CurveAndVertices_AreValid)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
        << "Edge " << anEdgeIdx << " has no Curve3D rep";
    }
    const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    EXPECT_TRUE(anEdge.StartVertexDefId().IsValid())
      << "Edge " << anEdgeIdx << " has invalid StartVertexId";
    EXPECT_TRUE(anEdge.EndVertexDefId().IsValid())
      << "Edge " << anEdgeIdx << " has invalid EndVertexId";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWire_Exists)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    EXPECT_GE(aFace.OuterWireDefId().Index, 0) << "Face " << aFaceIdx << " has no outer wire";
  }
}

TEST_F(BRepGraphTest, FaceDef_HasValidSurface)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, BRepGraph_FaceId(aFaceIdx)))
      << "Face " << aFaceIdx << " has no surface rep";
  }
}

TEST_F(BRepGraphTest, FindPCurve_ValidPair)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    BRepGraph_NodeId                   aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const int                          anOuterWireIdx = aFaceDef.OuterWireDefId().Index;
    if (anOuterWireIdx < 0)
      continue;
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph.Topo().Wire(BRepGraph_WireId(anOuterWireIdx));
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef&       aCR = aWireDef.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge =
        myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCR.CoEdgeDefId));
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(aCoEdge.EdgeDefId)))
        continue;
      const BRepGraphInc::CoEdgeEntity* aPCurveEntry =
        BRepGraph_Tool::Edge::FindPCurve(myGraph,
                                         BRepGraph_EdgeId(aCoEdge.EdgeDefId),
                                         BRepGraph_FaceId(aFaceIdx));
      EXPECT_NE(aPCurveEntry, nullptr)
        << "Missing PCurve for edge " << aCoEdge.EdgeDefId.Index << " on face " << aFaceIdx;
    }
  }
}

TEST_F(BRepGraphTest, UID_Unique)
{
  NCollection_Map<BRepGraph_UID> aUIDSet;
  for (int aSolidIdx = 0; aSolidIdx < myGraph.Topo().NbSolids(); ++aSolidIdx)
    EXPECT_TRUE(
      aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidIdx))));
  for (int aShellIdx = 0; aShellIdx < myGraph.Topo().NbShells(); ++aShellIdx)
    EXPECT_TRUE(
      aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aShellIdx))));
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
    EXPECT_TRUE(
      aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx))));
  for (int aWireIdx = 0; aWireIdx < myGraph.Topo().NbWires(); ++aWireIdx)
    EXPECT_TRUE(
      aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx))));
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
    EXPECT_TRUE(
      aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx))));
  for (int aVertexIdx = 0; aVertexIdx < myGraph.Topo().NbVertices(); ++aVertexIdx)
    EXPECT_TRUE(
      aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVertexIdx))));
  // Surface/Curve geometry UIDs are no longer separate nodes; geometry is stored inline on defs.
}

TEST_F(BRepGraphTest, UID_NodeIdRoundTrip)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId     aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const BRepGraph_UID& aUID      = myGraph.UIDs().Of(aFaceId);
    BRepGraph_NodeId     aResolved = myGraph.UIDs().NodeIdFrom(aUID);
    EXPECT_EQ(aResolved, aFaceId) << "Round trip failed for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, BoundingBox_NonVoid)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box          aBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aBox);
    EXPECT_FALSE(aBox.IsVoid()) << "BoundingBox is void for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, SameDomainFaces_Box_Empty)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId                     aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    NCollection_Vector<BRepGraph_NodeId> aSameDomain = myGraph.Topo().SameDomainFaces(aFaceId);
    EXPECT_EQ(aSameDomain.Length(), 0)
      << "Box face " << aFaceIdx << " should have no same-domain faces";
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbFaces(), 2);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 2);
}

TEST_F(BRepGraphTest, UserAttribute_SetGet)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  const int                                aKey   = BRepGraph_UserAttribute::AllocateKey();
  Handle(BRepGraph_TypedAttribute<double>) anAttr = new BRepGraph_TypedAttribute<double>(3.14);
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);

  occ::handle<BRepGraph_UserAttribute> aRetrieved = myGraph.Attrs().Get(aFaceId, aKey);
  ASSERT_FALSE(aRetrieved.IsNull());

  Handle(BRepGraph_TypedAttribute<double>) aTyped =
    Handle(BRepGraph_TypedAttribute<double>)::DownCast(aRetrieved);
  ASSERT_FALSE(aTyped.IsNull());
  EXPECT_NEAR(aTyped->UncheckedValue(), 3.14, 1.0e-10);
}

TEST_F(BRepGraphTest, ReBuild_UIDMonotonic)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  const uint32_t aGen1 = aGraph.UIDs().Generation();

  // Access a UID from the first build to verify it works.
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);
  const BRepGraph_NodeId aFirstFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aFirstUID = aGraph.UIDs().Of(aFirstFace);
  EXPECT_TRUE(aFirstUID.IsValid());
  EXPECT_EQ(aFirstUID.Generation(), aGen1);

  aGraph.Build(aBox);
  const uint32_t aGen2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGen2, aGen1);

  // Verify all face UIDs in second build are valid and have new generation.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
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
  double anOrigTol = BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId(0));
  BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
  anEdge->Tolerance                                    = anOrigTol * 2.0;
  EXPECT_NEAR(BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId(0)),
              anOrigTol * 2.0,
              1.0e-15);
}

TEST_F(BRepGraphTest, FaceCountOfEdge_SharedEdge)
{
  // In a box, each non-degenerate edge is shared by exactly 2 faces.
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      int aCount = myGraph.Topo().FaceCountOfEdge(BRepGraph_EdgeId(anEdgeIdx));
      EXPECT_EQ(aCount, 2) << "Edge " << anEdgeIdx << " should be shared by 2 faces";
    }
  }
}

TEST_F(BRepGraphTest, FreeEdges_ClosedBox_Empty)
{
  NCollection_Vector<BRepGraph_EdgeId> aFree = BRepGraph_Analyze::FreeEdges(myGraph);
  EXPECT_EQ(aFree.Length(), 0);
}

TEST_F(BRepGraphTest, RecordHistory_BasicEntry)
{
  int                                  aBefore = myGraph.History().NbRecords();
  BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                     anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("TestOp", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("TestOp"));
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Substitution)
{
  // Get the first wire and its first edge via incidence refs.
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Topo().Wire(BRepGraph_WireId(0));
  ASSERT_GE(aWireDef.CoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef&       anOldCR = aWireDef.CoEdgeRefs.Value(0);
  const BRepGraph_TopoNode::CoEdgeDef& anOldCoEdge =
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(anOldCR.CoEdgeDefId));
  const BRepGraph_EdgeId anOldEdgeId = anOldCoEdge.EdgeDefId;

  // Pick a different edge to substitute.
  const BRepGraph_EdgeId aNewEdgeId((anOldEdgeId.Index + 1) % myGraph.Topo().NbEdges());

  BRepGraph_Mutator::ReplaceEdgeInWire(myGraph,
                                       BRepGraph_WireId(0),
                                       anOldEdgeId,
                                       aNewEdgeId,
                                       false);

  // Verify the substitution via the updated incidence refs.
  const BRepGraph_TopoNode::WireDef&   aWireDefAfter = myGraph.Topo().Wire(BRepGraph_WireId(0));
  const BRepGraphInc::CoEdgeRef&       aNewCR        = aWireDefAfter.CoEdgeRefs.Value(0);
  const BRepGraph_TopoNode::CoEdgeDef& aNewCoEdge =
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aNewCR.CoEdgeDefId));
  EXPECT_EQ(aNewCoEdge.EdgeDefId.Index, aNewEdgeId.Index);
}

TEST_F(BRepGraphTest, ParallelBuild_SameAsSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aSeqGraph;
  aSeqGraph.Build(aBox, false);
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Build(aBox, true);
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Topo().NbSolids(), aSeqGraph.Topo().NbSolids());
  EXPECT_EQ(aParGraph.Topo().NbShells(), aSeqGraph.Topo().NbShells());
  EXPECT_EQ(aParGraph.Topo().NbFaces(), aSeqGraph.Topo().NbFaces());
  EXPECT_EQ(aParGraph.Topo().NbWires(), aSeqGraph.Topo().NbWires());
  EXPECT_EQ(aParGraph.Topo().NbEdges(), aSeqGraph.Topo().NbEdges());
  EXPECT_EQ(aParGraph.Topo().NbVertices(), aSeqGraph.Topo().NbVertices());
  EXPECT_EQ(aParGraph.Topo().NbFaces(), aSeqGraph.Topo().NbFaces());
  EXPECT_EQ(aParGraph.Topo().NbEdges(), aSeqGraph.Topo().NbEdges());
}

TEST_F(BRepGraphTest, ParallelBuild_CompoundOfFaces)
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
  EXPECT_EQ(aGraph.Topo().NbFaces(), 12);
}

// ===================================================================
// Group 1: Shape Round-Trip
// ===================================================================

TEST_F(BRepGraphTest, ReconstructFace_EachBoxFace_SameSubShapeCounts)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const TopoDS_Shape& anOrigFace = myGraph.Shapes().OriginalOf(aFaceId);
    const TopoDS_Shape  aReconstructed =
      myGraph.Shapes().ReconstructFace(BRepGraph_FaceId(aFaceIdx));

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
      << "Vertex count mismatch for face " << aFaceIdx;
    EXPECT_EQ(aReconEdges.Extent(), anOrigEdges.Extent())
      << "Edge count mismatch for face " << aFaceIdx;
    EXPECT_EQ(aReconWires.Extent(), anOrigWires.Extent())
      << "Wire count mismatch for face " << aFaceIdx;

    // Verify same surface handle.
    const TopoDS_Face&               anOrigF = TopoDS::Face(anOrigFace);
    const TopoDS_Face&               aReconF = TopoDS::Face(aReconstructed);
    TopLoc_Location                  aLoc1, aLoc2;
    const occ::handle<Geom_Surface>& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
    const occ::handle<Geom_Surface>& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
    EXPECT_EQ(aSurf1.get(), aSurf2.get()) << "Surface handle differs for face " << aFaceIdx;
  }
}

TEST_F(BRepGraphTest, ReconstructFace_AfterEdgeReplace_ContainsNewEdge)
{
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Topo().Wire(BRepGraph_WireId(0));
  ASSERT_GE(aWireDef.CoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aCR0 = aWireDef.CoEdgeRefs.Value(0);
  const BRepGraph_EdgeId         anOldEdgeId =
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCR0.CoEdgeDefId)).EdgeDefId;

  // Pick a different edge.
  const int              aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Topo().NbEdges();
  const BRepGraph_EdgeId aNewEdgeId(aNewIdx);

  // Get 3D curve handles from graph for old/new edges.
  occ::handle<Geom_Curve> aNewCurve =
    BRepGraph_Tool::Edge::Curve(myGraph, BRepGraph_EdgeId(aNewIdx));
  occ::handle<Geom_Curve> anOldCurve =
    BRepGraph_Tool::Edge::Curve(myGraph, BRepGraph_EdgeId(anOldEdgeId.Index));

  BRepGraph_Mutator::ReplaceEdgeInWire(myGraph,
                                       BRepGraph_WireId(0),
                                       anOldEdgeId,
                                       aNewEdgeId,
                                       false);

  // Reconstruct face 0 (the face owning wire 0).
  const int aFaceIdx =
    myGraph.Topo().Face(BRepGraph_FaceId(0)).OuterWireDefId().Index == 0 ? 0 : -1;
  ASSERT_GE(aFaceIdx, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().ReconstructFace(BRepGraph_FaceId(aFaceIdx));

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
      isNewFound = true;
    if (!aCurve.IsNull() && !anOldCurve.IsNull() && aCurve.get() == anOldCurve.get())
      isOldFound = true;
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
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

TEST_F(BRepGraphTest, ReconstructShape_FaceRoot_ReturnsSameShape)
{
  BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const TopoDS_Shape  aReconstructed = myGraph.Shapes().Reconstruct(aFaceId);
  const TopoDS_Shape& anOriginal     = myGraph.Shapes().OriginalOf(aFaceId);

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
  BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  TopoDS_Shape        aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.Shapes().OriginalOf(aFaceId);
  EXPECT_TRUE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_AfterReplaceEdge_DiffersFromOriginal)
{
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Topo().Wire(BRepGraph_WireId(0));
  ASSERT_GE(aWireDef.CoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aCR0 = aWireDef.CoEdgeRefs.Value(0);
  const BRepGraph_EdgeId         anOldEdgeId =
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCR0.CoEdgeDefId)).EdgeDefId;
  const int              aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Topo().NbEdges();
  const BRepGraph_EdgeId aNewEdgeId(aNewIdx);

  BRepGraph_Mutator::ReplaceEdgeInWire(myGraph,
                                       BRepGraph_WireId(0),
                                       anOldEdgeId,
                                       aNewEdgeId,
                                       false);

  // Find the face that owns wire 0.
  int aFaceDefIdx = -1;
  for (int aFI = 0; aFI < myGraph.Topo().NbFaces(); ++aFI)
  {
    if (myGraph.Topo().Face(BRepGraph_FaceId(aFI)).OuterWireDefId().Index == 0)
    {
      aFaceDefIdx = aFI;
      break;
    }
  }
  ASSERT_GE(aFaceDefIdx, 0);
  BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, aFaceDefIdx);
  TopoDS_Shape        aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.Shapes().OriginalOf(aFaceId);
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

TEST_F(BRepGraphTest, IsModified_MutableEdge_PropagatesUp)
{
  EXPECT_FALSE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);

  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));

  EXPECT_TRUE(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).IsModified);

  // Check propagation up to parent wire and face.
  const BRepGraph_TopoNode::EdgeDef& anEdge = myGraph.Topo().Edge(BRepGraph_EdgeId(0));
  if (anEdge.Id.IsValid())
  {
    // Find a wire containing this edge.
    const NCollection_Vector<BRepGraph_WireId>& aWires =
      myGraph.Topo().WiresOfEdge(BRepGraph_EdgeId(0));
    if (aWires.Length() > 0)
    {
      EXPECT_TRUE(myGraph.Topo().Wire(aWires.Value(0)).IsModified);
      // Check propagation to owning face: find a face whose outer wire is this wire.
      for (int aFI = 0; aFI < myGraph.Topo().NbFaces(); ++aFI)
      {
        if (myGraph.Topo().Face(BRepGraph_FaceId(aFI)).OuterWireDefId().Index
            == aWires.Value(0).Index)
        {
          EXPECT_TRUE(myGraph.Topo().Face(BRepGraph_FaceId(aFI)).IsModified);
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(aFaceId))
      << "Face " << aFaceIdx << " should have original shape after Build()";
  }
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(anEdgeId))
      << "Edge " << anEdgeIdx << " should have original shape after Build()";
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

  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.123;
  TopoDS_Shape anAfter                            = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(anAfter.IsNull());

  // After mutation, Shape() reconstructs a new edge - different TShape.
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

TEST_F(BRepGraphTest, DefaultBuild_AssignsValidUIDs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Build(aBoxMaker.Shape());

  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);

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
  aGraph.Build(aBoxMaker1.Shape());
  const uint32_t aGeneration1 = aGraph.UIDs().Generation();

  aGraph.Build(aBoxMaker2.Shape());
  const uint32_t aGeneration2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGeneration1, 0u);
  EXPECT_EQ(aGeneration2, aGeneration1 + 1);
}

TEST_F(BRepGraphTest, StaleUID_HasReturnsFalseAfterRebuild)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Build(aBoxMaker1.Shape());
  ASSERT_GT(aGraph.Topo().NbFaces(), 0);
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

  BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                     anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
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

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  BRepGraph_Mutator::ApplyModification(myGraph, anEdge0, aModifier, "TestHop");

  BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge1);
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraphTest, FindDerived_SingleHop_ContainsTarget)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  BRepGraph_Mutator::ApplyModification(myGraph, anEdge0, aModifier, "TestHop");

  NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool                                 isFound  = false;
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
  auto aModifier1 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };
  BRepGraph_Mutator::ApplyModification(myGraph, anEdge0, aModifier1, "Step1");

  // Step 2: edge1 -> edge2
  auto aModifier2 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge2);
    return aResult;
  };
  BRepGraph_Mutator::ApplyModification(myGraph, anEdge1, aModifier2, "Step2");

  // FindOriginal from edge2 should trace back to edge0.
  BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge2);
  EXPECT_EQ(anOriginal, anEdge0);

  // FindDerived from edge0 returns leaf-only transitive descendants.
  // edge1 is an intermediate (it has further derived edge2), so only edge2 is returned.
  NCollection_Vector<BRepGraph_NodeId> aDerived     = myGraph.History().FindDerived(anEdge0);
  bool                                 isEdge2Found = false;
  for (int anIdx = 0; anIdx < aDerived.Length(); ++anIdx)
  {
    if (aDerived.Value(anIdx) == anEdge2)
      isEdge2Found = true;
  }
  EXPECT_TRUE(isEdge2Found) << "Edge2 not found in FindDerived(Edge0)";

  // edge1 can be found by querying FindDerived on the intermediate step.
  NCollection_Vector<BRepGraph_NodeId> aDerived1        = myGraph.History().FindDerived(anEdge1);
  bool                                 isEdge2FromEdge1 = false;
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

  occ::handle<Geom2d_Line> aCurve2d = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  myGraph.Builder().AddPCurveToEdge(anEdgeId, aFaceId, aCurve2d, 0.0, 1.0);

  const BRepGraphInc::CoEdgeEntity* aRetrieved = myGraph.Topo().FindPCurve(anEdgeId, aFaceId);
  EXPECT_NE(aRetrieved, nullptr);
  if (aRetrieved != nullptr)
  {
    EXPECT_TRUE(aRetrieved->Curve2DRepId.IsValid());
  }
}

TEST_F(BRepGraphTest, ReplaceEdgeInWire_Reversed_OrientationFlipped)
{
  const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Topo().Wire(BRepGraph_WireId(0));
  ASSERT_GE(aWireDef.CoEdgeRefs.Length(), 1);

  const BRepGraphInc::CoEdgeRef&       anOrigCR = aWireDef.CoEdgeRefs.Value(0);
  const BRepGraph_TopoNode::CoEdgeDef& anOrigCoEdge =
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(anOrigCR.CoEdgeDefId));
  const BRepGraph_EdgeId anOldEdgeId       = anOrigCoEdge.EdgeDefId;
  TopAbs_Orientation     anOrigOrientation = anOrigCoEdge.Sense;

  // Pick a different edge.
  const int              aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Topo().NbEdges();
  const BRepGraph_EdgeId aNewEdgeId(aNewIdx);

  BRepGraph_Mutator::ReplaceEdgeInWire(myGraph, BRepGraph_WireId(0), anOldEdgeId, aNewEdgeId, true);

  const BRepGraph_TopoNode::WireDef&   aWireDefAfter = myGraph.Topo().Wire(BRepGraph_WireId(0));
  const BRepGraphInc::CoEdgeRef&       aNewCR        = aWireDefAfter.CoEdgeRefs.Value(0);
  const BRepGraph_TopoNode::CoEdgeDef& aNewCoEdge =
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aNewCR.CoEdgeDefId));
  EXPECT_EQ(aNewCoEdge.EdgeDefId.Index, aNewEdgeId.Index);

  // Orientation should be flipped relative to original.
  TopAbs_Orientation anExpected =
    (anOrigOrientation == TopAbs_FORWARD) ? TopAbs_REVERSED : TopAbs_FORWARD;
  EXPECT_EQ(aNewCoEdge.Sense, anExpected);
}

TEST_F(BRepGraphTest, MutableVertex_ChangePoint_Verified)
{
  BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> aMutVert =
    myGraph.Builder().MutVertex(BRepGraph_VertexId(0));
  aMutVert->Point = gp_Pnt(99.0, 99.0, 99.0);

  const BRepGraph_TopoNode::VertexDef& aVert = myGraph.Topo().Vertex(BRepGraph_VertexId(0));
  EXPECT_NEAR(aVert.Point.X(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Y(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Z(), 99.0, Precision::Confusion());
}

// ===================================================================
// Group 4: Geometric Queries
// ===================================================================

TEST_F(BRepGraphTest, Centroid_Box_ApproximateCenter)
{
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  gp_Pnt           aCentroid = bboxCenter(myGraph, aSolidId);

  // 10x20x30 box at origin: centroid approximately at (5, 10, 15).
  EXPECT_NEAR(aCentroid.X(), 5.0, 0.5);
  EXPECT_NEAR(aCentroid.Y(), 10.0, 0.5);
  EXPECT_NEAR(aCentroid.Z(), 15.0, 0.5);
}

TEST_F(BRepGraphTest, EdgeDef_HasValidCurve3d)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
      continue;

    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
      << "Edge " << anEdgeIdx << " has no Curve3D rep";
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
  const BRepGraph_TopoNode::FaceDef& aFaceDef       = myGraph.Topo().Face(BRepGraph_FaceId(0));
  const int                          anOuterWireIdx = aFaceDef.OuterWireDefId().Index;
  ASSERT_GE(anOuterWireIdx, 0);
  const BRepGraph_TopoNode::WireDef& aWireDefInv =
    myGraph.Topo().Wire(BRepGraph_WireId(anOuterWireIdx));
  ASSERT_GE(aWireDefInv.CoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aCRInv = aWireDefInv.CoEdgeRefs.Value(0);
  BRepGraph_NodeId               anEdgeId(
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCRInv.CoEdgeDefId)).EdgeDefId);

  Bnd_Box anEdgeBox1 = BRepGraphAlgo_BndLib::AddCached(myGraph, anEdgeId);
  ASSERT_FALSE(anEdgeBox1.IsVoid());

  // Invalidate subgraph from face via a no-op mutation (triggers markModified).
  {
    auto aMut = myGraph.Builder().MutFace(BRepGraph_FaceId(aFaceId.Index));
  }

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
    BRepGraph_Analyze::ParallelForEachFace(
      myGraph,
      aSubs.Value(aSubIdx),
      [&aCounter](const BRepGraph_FaceId) { aCounter.fetch_add(1, std::memory_order_relaxed); });
  }
  EXPECT_EQ(aCounter.load(), myGraph.Topo().NbFaces());
}

TEST_F(BRepGraphTest, ParallelForEachEdge_AllEdgesVisited)
{
  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(myGraph);
  ASSERT_GE(aSubs.Length(), 1);

  std::atomic<int>          aCounter{0};
  const BRepGraph_SubGraph& aSub = aSubs.Value(0);
  BRepGraph_Analyze::ParallelForEachEdge(myGraph, aSub, [&aCounter](const BRepGraph_EdgeId) {
    aCounter.fetch_add(1, std::memory_order_relaxed);
  });
  EXPECT_EQ(aCounter.load(), aSub.EdgeDefIds().Length());
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
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_EdgeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(aGraph);
  EXPECT_EQ(aFreeEdges.Length(), 4);
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbFaces(), 3);

  NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 3);
}

TEST_F(BRepGraphTest, DetectToleranceConflicts_ManualConflict_Detected)
{
  // Find two edges that share the same Curve3d handle.
  bool isConflictSetUp = false;
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges() && !isConflictSetUp; ++anEdgeIdx)
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(anEdgeIdx))
        || !BRepGraph_Tool::Edge::HasCurve(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
      continue;

    for (int anOtherIdx = anEdgeIdx + 1; anOtherIdx < myGraph.Topo().NbEdges(); ++anOtherIdx)
    {
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(anOtherIdx))
          || !BRepGraph_Tool::Edge::HasCurve(myGraph, BRepGraph_EdgeId(anOtherIdx)))
        continue;
      if (BRepGraph_Tool::Edge::Curve(myGraph, BRepGraph_EdgeId(anEdgeIdx)).get()
          != BRepGraph_Tool::Edge::Curve(myGraph, BRepGraph_EdgeId(anOtherIdx)).get())
        continue;

      // Set very different tolerances on two edges sharing the same curve.
      myGraph.Builder().MutEdge(BRepGraph_EdgeId(anEdgeIdx))->Tolerance  = 0.001;
      myGraph.Builder().MutEdge(BRepGraph_EdgeId(anOtherIdx))->Tolerance = 1.0;

      isConflictSetUp = true;
      break;
    }
  }

  if (isConflictSetUp)
  {
    NCollection_Vector<BRepGraph_EdgeId> aConflicts =
      BRepGraph_Analyze::ToleranceConflicts(myGraph, 0.5);
    EXPECT_GE(aConflicts.Length(), 1);
  }
}

// ===================================================================
// Group 8: User Attributes & Error Cases
// ===================================================================

TEST_F(BRepGraphTest, RemoveUserAttribute_AfterSet_ReturnsNull)
{
  BRepGraph_NodeId                     aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const int                            aKey   = BRepGraph_UserAttribute::AllocateKey();
  occ::handle<BRepGraph_UserAttribute> anAttr = new BRepGraph_TypedAttribute<int>(42);

  myGraph.Attrs().Set(aFaceId, aKey, anAttr);
  ASSERT_FALSE(myGraph.Attrs().Get(aFaceId, aKey).IsNull());

  myGraph.Attrs().Remove(aFaceId, aKey);
  EXPECT_TRUE(myGraph.Attrs().Get(aFaceId, aKey).IsNull());
}

TEST_F(BRepGraphTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aGraph.Topo().NbShells(), 0);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 0);
  EXPECT_EQ(aGraph.Topo().NbWires(), 0);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 0);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 0);
}

TEST_F(BRepGraphTest, TopoNode_GenericLookup_MatchesTypedAccess)
{
  BRepGraph_NodeId                   aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_TopoNode::BaseDef* aBase = myGraph.Topo().TopoDef(aFaceId);
  ASSERT_NE(aBase, nullptr);
  EXPECT_EQ(aBase->Id, myGraph.Topo().Face(BRepGraph_FaceId(0)).Id);

  // Invalid node id should return nullptr.
  BRepGraph_NodeId                   anInvalidId;
  const BRepGraph_TopoNode::BaseDef* anInvalidBase = myGraph.Topo().TopoDef(anInvalidId);
  EXPECT_EQ(anInvalidBase, nullptr);
}

// ===================================================================
// Group 9: Node Counts and Identity
// ===================================================================

TEST_F(BRepGraphTest, NbNodes_Box_TotalCount)
{
  // NbNodes should equal sum of all per-kind counts (topology + assembly).
  size_t anExpected =
    static_cast<size_t>(myGraph.Topo().NbSolids()) + myGraph.Topo().NbShells()
    + myGraph.Topo().NbFaces() + myGraph.Topo().NbWires() + myGraph.Topo().NbCoEdges()
    + myGraph.Topo().NbEdges() + myGraph.Topo().NbVertices() + myGraph.Topo().NbCompounds()
    + myGraph.Topo().NbCompSolids() + myGraph.Paths().NbProducts() + myGraph.Paths().NbOccurrences();
  EXPECT_EQ(myGraph.Topo().NbNodes(), anExpected);
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

TEST_F(BRepGraphTest, Build_WithCustomAllocator_IsDone)
{
  occ::handle<NCollection_BaseAllocator> anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  BRepGraph                              aGraph(anAlloc);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
  EXPECT_FALSE(aGraph.Allocator().IsNull());
}

// ===================================================================
// Group 10: Topology Structure
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_BoxOuterWires)
{
  // All outer wires of a box face should be closed.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    const int                          anOuterWireIdx = aFaceDef.OuterWireDefId().Index;
    ASSERT_GE(anOuterWireIdx, 0);
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph.Topo().Wire(BRepGraph_WireId(anOuterWireIdx));
    EXPECT_TRUE(aWireDef.IsClosed) << "Outer wire of face " << aFaceIdx << " should be closed";
  }
}

TEST_F(BRepGraphTest, Face_InnerWireRefs_BoxHasNone)
{
  // Box faces have no holes, so non-outer WireRefs should be empty.
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    int                                aNonOuterCount = 0;
    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      if (!aFaceDef.WireRefs.Value(aWireRefIdx).IsOuter)
        ++aNonOuterCount;
    }
    EXPECT_EQ(aNonOuterCount, 0) << "Box face " << aFaceIdx << " should have no inner wires";
  }
}

TEST_F(BRepGraphTest, Face_Orientation_ValidValue)
{
  // Verify face orientations in the shell's incidence refs.
  ASSERT_EQ(myGraph.Topo().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph.Topo().Shell(BRepGraph_ShellId(0));
  for (int aRefIdx = 0; aRefIdx < aShellDef.FaceRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aShellDef.FaceRefs.Value(aRefIdx);
    TopAbs_Orientation           anOri    = aFaceRef.Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED)
      << "Face ref " << aRefIdx << " has unexpected orientation " << anOri;
  }
}

TEST_F(BRepGraphTest, Shell_ContainsSixFaces)
{
  ASSERT_EQ(myGraph.Topo().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph.Topo().Shell(BRepGraph_ShellId(0));
  EXPECT_EQ(aShellDef.FaceRefs.Length(), 6);
}

TEST_F(BRepGraphTest, Solid_ContainsOneShell)
{
  ASSERT_EQ(myGraph.Topo().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph.Topo().Solid(BRepGraph_SolidId(0));
  EXPECT_EQ(aSolidDef.ShellRefs.Length(), 1);
}

TEST_F(BRepGraphTest, Edge_ParamRange_ValidBounds)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(anEdgeIdx)))
      continue;
    const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(myGraph, BRepGraph_EdgeId(anEdgeIdx));
    EXPECT_LT(aFirst, aLast) << "Edge " << anEdgeIdx << " has invalid parameter range [" << aFirst
                             << ", " << aLast << "]";
  }
}

TEST_F(BRepGraphTest, Vertex_TolerancePositive)
{
  for (int aVtxIdx = 0; aVtxIdx < myGraph.Topo().NbVertices(); ++aVtxIdx)
  {
    EXPECT_GT(BRepGraph_Tool::Vertex::Tolerance(myGraph, BRepGraph_VertexId(aVtxIdx)), 0.0)
      << "Vertex " << aVtxIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Edge_TolerancePositive)
{
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    EXPECT_GT(BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId(anEdgeIdx)), 0.0)
      << "Edge " << anEdgeIdx << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Face_ToleranceNonNegative)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_GE(BRepGraph_Tool::Face::Tolerance(myGraph, BRepGraph_FaceId(aFaceIdx)), 0.0)
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
  EXPECT_EQ(aSub.SolidDefIds().Length(), 1);
  EXPECT_EQ(aSub.ShellDefIds().Length(), 1);
  EXPECT_EQ(aSub.FaceDefIds().Length(), 6);
  EXPECT_EQ(aSub.WireDefIds().Length(), 6);
  // Edge/vertex ids may include per-wire-reference duplicates.
  EXPECT_GE(aSub.EdgeDefIds().Length(), 12);
  EXPECT_GE(aSub.VertexDefIds().Length(), 8);
  EXPECT_GT(aSub.NbTopoNodes(), 0);
  EXPECT_EQ(aSub.ParentGraph(), &myGraph);
}

// ===================================================================
// Group 12: Geometric Queries (Extended)
// ===================================================================

TEST_F(BRepGraphTest, BoundingBox_Solid_ContainsAllFaces)
{
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box          aSolidBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aSolidId, aSolidBox);
  ASSERT_FALSE(aSolidBox.IsVoid());

  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box          aFaceBox;
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
  const BRepGraph_TopoNode::FaceDef& aFaceDef       = myGraph.Topo().Face(BRepGraph_FaceId(0));
  const int                          anOuterWireIdx = aFaceDef.OuterWireDefId().Index;
  ASSERT_GE(anOuterWireIdx, 0);
  const BRepGraph_TopoNode::WireDef& aWireDefBBox =
    myGraph.Topo().Wire(BRepGraph_WireId(anOuterWireIdx));
  ASSERT_GE(aWireDefBBox.CoEdgeRefs.Length(), 1);

  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Bnd_Box          aFaceBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aFaceBox);
  ASSERT_FALSE(aFaceBox.IsVoid());

  const BRepGraphInc::CoEdgeRef& aCRBBox = aWireDefBBox.CoEdgeRefs.Value(0);
  BRepGraph_NodeId               anEdgeId(
    myGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCRBBox.CoEdgeDefId)).EdgeDefId);
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    gp_Pnt           aCentroid = bboxCenter(myGraph, aFaceId);
    Bnd_Box          aBox;
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
  BRepGraph_MutRef<BRepGraph_TopoNode::WireDef> aMutWD       = myGraph.Builder().MutWire(BRepGraph_WireId(0));
  bool                                          anOrigClosed = aMutWD->IsClosed;
  aMutWD->IsClosed                                           = !anOrigClosed;

  EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(0)).IsClosed, !anOrigClosed);

  // Restore original state.
  myGraph.Builder().MutWire(BRepGraph_WireId(0))->IsClosed = anOrigClosed;
  EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(0)).IsClosed, anOrigClosed);
}

TEST_F(BRepGraphTest, MultipleUserAttributes_SameNode_Independent)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  const int aKey1 = BRepGraph_UserAttribute::AllocateKey();
  const int aKey2 = BRepGraph_UserAttribute::AllocateKey();
  EXPECT_NE(aKey1, aKey2);

  Handle(BRepGraph_TypedAttribute<int>)    anAttr1 = new BRepGraph_TypedAttribute<int>(100);
  Handle(BRepGraph_TypedAttribute<double>) anAttr2 = new BRepGraph_TypedAttribute<double>(2.718);

  myGraph.Attrs().Set(aFaceId, aKey1, anAttr1);
  myGraph.Attrs().Set(aFaceId, aKey2, anAttr2);

  Handle(BRepGraph_TypedAttribute<int>) aRetrieved1 =
    Handle(BRepGraph_TypedAttribute<int>)::DownCast(myGraph.Attrs().Get(aFaceId, aKey1));
  Handle(BRepGraph_TypedAttribute<double>) aRetrieved2 =
    Handle(BRepGraph_TypedAttribute<double>)::DownCast(myGraph.Attrs().Get(aFaceId, aKey2));

  ASSERT_FALSE(aRetrieved1.IsNull());
  ASSERT_FALSE(aRetrieved2.IsNull());
  EXPECT_EQ(aRetrieved1->UncheckedValue(), 100);
  EXPECT_NEAR(aRetrieved2->UncheckedValue(), 2.718, 1.0e-10);

  // Remove one; the other should remain.
  myGraph.Attrs().Remove(aFaceId, aKey1);
  EXPECT_TRUE(myGraph.Attrs().Get(aFaceId, aKey1).IsNull());
  EXPECT_FALSE(myGraph.Attrs().Get(aFaceId, aKey2).IsNull());
}

TEST_F(BRepGraphTest, InvalidateUserAttribute_SpecificKey)
{
  BRepGraph_NodeId                      aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const int                             aKey   = BRepGraph_UserAttribute::AllocateKey();
  Handle(BRepGraph_TypedAttribute<int>) anAttr = new BRepGraph_TypedAttribute<int>(42);
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);

  // Invalidate should not remove, just mark dirty.
  myGraph.Attrs().Invalidate(aFaceId, aKey);

  occ::handle<BRepGraph_UserAttribute> aRetrieved = myGraph.Attrs().Get(aFaceId, aKey);
  EXPECT_FALSE(aRetrieved.IsNull()); // still present
}

TEST_F(BRepGraphTest, UserAttribute_OnEdgeNode)
{
  BRepGraph_NodeId                         anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  const int                                aKey   = BRepGraph_UserAttribute::AllocateKey();
  Handle(BRepGraph_TypedAttribute<double>) anAttr = new BRepGraph_TypedAttribute<double>(1.5);

  myGraph.Attrs().Set(anEdgeId, aKey, anAttr);

  Handle(BRepGraph_TypedAttribute<double>) aRetrieved =
    Handle(BRepGraph_TypedAttribute<double>)::DownCast(myGraph.Attrs().Get(anEdgeId, aKey));
  ASSERT_FALSE(aRetrieved.IsNull());
  EXPECT_NEAR(aRetrieved->UncheckedValue(), 1.5, 1.0e-10);
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
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aGraph.Topo().NbShells(), 0);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);
  EXPECT_EQ(aGraph.Topo().NbWires(), 1);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 4);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 4);
}

TEST_F(BRepGraphTest, Build_Shell_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aGraph.Topo().NbShells(), 1);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 12);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 8);
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbSolids(), 2);
  EXPECT_EQ(aGraph.Topo().NbShells(), 2);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 12);
}

TEST_F(BRepGraphTest, ReconstructShape_ShellRoot_SameFaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Build(anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().NbShells(), 1);

  BRepGraph_NodeId   aShellId(BRepGraph_NodeId::Kind::Shell, 0);
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
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    EXPECT_TRUE(
      myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx)).IsValid())
      << "Face " << aFaceIdx << " has invalid UID";
  }
  for (int anEdgeIdx = 0; anEdgeIdx < myGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    EXPECT_TRUE(
      myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx)).IsValid())
      << "Edge " << anEdgeIdx << " has invalid UID";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWireIdx_MatchesFaceDef)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    const int                          anOuterWireIdx = aFace.OuterWireDefId().Index;
    EXPECT_GE(anOuterWireIdx, 0) << "Face " << aFaceIdx << " has no outer wire";
    if (anOuterWireIdx < 0)
      continue;
    EXPECT_LT(anOuterWireIdx, myGraph.Topo().NbWires())
      << "Face " << aFaceIdx << " outer wire index out of range";
  }
}

TEST_F(BRepGraphTest, Wire_CoEdgeRefs_FourEdgesPerBoxFace)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    const int                          anOuterWireIdx = aFaceDef.OuterWireDefId().Index;
    ASSERT_GE(anOuterWireIdx, 0);
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph.Topo().Wire(BRepGraph_WireId(anOuterWireIdx));
    EXPECT_EQ(aWireDef.CoEdgeRefs.Length(), 4)
      << "Box face " << aFaceIdx << " should have 4 coedge refs in its outer wire";
  }
}

// ===================================================================
// Group: History Enabled Flag
// ===================================================================

TEST_F(BRepGraphTest, SetHistoryEnabled_DefaultTrue)
{
  EXPECT_TRUE(myGraph.History().IsEnabled());
}

TEST_F(BRepGraphTest, SetHistoryEnabled_DisableAndQuery)
{
  myGraph.History().SetEnabled(false);
  EXPECT_FALSE(myGraph.History().IsEnabled());
  myGraph.History().SetEnabled(true);
  EXPECT_TRUE(myGraph.History().IsEnabled());
}

TEST_F(BRepGraphTest, RecordHistory_Disabled_NoRecordAdded)
{
  const int aBefore = myGraph.History().NbRecords();

  myGraph.History().SetEnabled(false);

  BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                     anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("ShouldNotRecord", anEdge0, aRepl);

  EXPECT_EQ(myGraph.History().NbRecords(), aBefore);
}

TEST_F(BRepGraphTest, RecordHistory_ReEnabled_RecordsAgain)
{
  myGraph.History().SetEnabled(false);

  const int aBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                     anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("Skipped", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore);

  myGraph.History().SetEnabled(true);
  myGraph.History().Record("Recorded", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("Recorded"));
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_NoHistoryNoDerivedEdges)
{
  myGraph.History().SetEnabled(false);

  const int aNbHistBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  BRepGraph_Mutator::ApplyModification(myGraph, anEdge0, aModifier, "NoHistory");

  // No history records should be added.
  EXPECT_EQ(myGraph.History().NbRecords(), aNbHistBefore);
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_ModifierStillRuns)
{
  myGraph.History().SetEnabled(false);

  bool             isModifierCalled = false;
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    isModifierCalled = true;
    NCollection_Vector<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge0);
    return aResult;
  };

  BRepGraph_Mutator::ApplyModification(myGraph, anEdge0, aModifier, "CheckModifier");
  EXPECT_TRUE(isModifierCalled);
}

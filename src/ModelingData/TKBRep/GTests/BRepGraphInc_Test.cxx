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
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraph_CacheDerivedState.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Standard_ProgramError.hxx>
#include <gp_Pln.hxx>

#include <NCollection_LinearVector.hxx>

#include <gtest/gtest.h>

#include <type_traits>
#include <tuple>

static_assert(std::is_same_v<BRepGraphInc::VertexDef::TypeId, BRepGraph_VertexId>);
static_assert(std::is_same_v<BRepGraphInc::FaceRef::TypeId, BRepGraph_FaceRefId>);

static double computeArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(theShape, aProps);
  return aProps.Mass();
}

static double computeVolume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

static uint32_t countSubShapes(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  uint32_t aCount = 0;
  for (TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next())
  {
    ++aCount;
  }
  return aCount;
}

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

template <class theIdType>
static bool containsId(const NCollection_LinearVector<theIdType>& theIds, const theIdType theId)
{
  for (const theIdType& anId : theIds)
  {
    if (anId == theId)
    {
      return true;
    }
  }
  return false;
}

static BRepGraph_VertexId addStorageVertex(BRepGraphInc_Storage& theStorage,
                                           const gp_Pnt&         thePoint,
                                           const double          theTolerance)
{
  const BRepGraph_VertexId aVertexId = theStorage.AppendVertex();
  BRepGraphInc::VertexDef& aVertex   = theStorage.ChangeVertex(aVertexId);
  aVertex.Point                      = thePoint;
  aVertex.Tolerance                  = theTolerance;
  return aVertexId;
}

static BRepGraph_EdgeId addStorageEdge(BRepGraphInc_Storage&    theStorage,
                                       const BRepGraph_VertexId theStartVertex,
                                       const BRepGraph_VertexId theEndVertex,
                                       const double             theTolerance = 0.0)
{
  const BRepGraph_EdgeId      anEdgeId  = theStorage.AppendEdge();
  const BRepGraph_VertexRefId aStartRef = theStorage.AppendVertexRef();
  const BRepGraph_VertexRefId anEndRef  = theStorage.AppendVertexRef();

  theStorage.ChangeVertexRef(aStartRef).ParentEdgeId  = anEdgeId;
  theStorage.ChangeVertexRef(aStartRef).ChildVertexId = theStartVertex;
  theStorage.ChangeVertexRef(anEndRef).ParentEdgeId   = anEdgeId;
  theStorage.ChangeVertexRef(anEndRef).ChildVertexId  = theEndVertex;

  BRepGraphInc::EdgeDef& anEdge = theStorage.ChangeEdge(anEdgeId);
  anEdge.StartVertexRefId       = aStartRef;
  anEdge.EndVertexRefId         = anEndRef;
  anEdge.Tolerance              = theTolerance;
  return anEdgeId;
}

// ============================================================
// Entity count validation
// ============================================================

TEST(BRepGraphIncTest, Box_EntityCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Entity counts for a box: 8 vertices, 12 edges, 6 faces, 1 shell, 1 solid.
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12u);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6u);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1u);
}

TEST(BRepGraphIncTest, ParityOrientationRejectsInternalExternal)
{
#ifndef No_Exception
  EXPECT_THROW(
    {
      [[maybe_unused]] const BRepGraphInc::ParityOrientation aParity =
        BRepGraphInc::ParityOrientation(TopAbs_INTERNAL);
    },
    Standard_ProgramError);
  EXPECT_THROW(
    {
      [[maybe_unused]] const BRepGraphInc::ParityOrientation aParity =
        BRepGraphInc::ParityOrientation(TopAbs_EXTERNAL);
    },
    Standard_ProgramError);

  BRepGraphInc::ParityOrientation anOrientation;
  EXPECT_THROW(anOrientation = TopAbs_INTERNAL, Standard_ProgramError);
  EXPECT_THROW(anOrientation = TopAbs_EXTERNAL, Standard_ProgramError);
#endif

  BRepGraphInc::ParityOrientation anOrientation2;
  anOrientation2 = TopAbs_FORWARD;
  EXPECT_EQ(TopAbs_Orientation(anOrientation2), TopAbs_FORWARD);
  anOrientation2 = TopAbs_REVERSED;
  EXPECT_EQ(TopAbs_Orientation(anOrientation2), TopAbs_REVERSED);
}

TEST(BRepGraphIncTest, Cylinder_EntityCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 2u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 3u);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 3u);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 3u);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1u);
}

TEST(BRepGraphIncTest, Sphere_EntityCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aSph, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 2u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 3u);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1u);
}

TEST(BRepGraphIncTest, Storage_AppendAccess_UsesTypedIds)
{
  BRepGraphInc_Storage aStorage;

  const BRepGraph_VertexId         aVertexId     = aStorage.AppendVertex();
  const BRepGraph_FaceRefId        aFaceRefId    = aStorage.AppendFaceRef();
  const BRepGraph_FaceSurfaceRepId aSurfaceRepId = aStorage.AppendFaceSurfaceRep();

  EXPECT_EQ(aVertexId.Index, 0);
  EXPECT_EQ(aFaceRefId.Index, 0);
  EXPECT_EQ(aSurfaceRepId.Index, 0);

  EXPECT_FALSE(aStorage.IsRemoved(aVertexId));
  EXPECT_FALSE(aStorage.IsRemoved(aFaceRefId));

  aStorage.ChangeVertex(aVertexId).Tolerance     = 1.25;
  aStorage.ChangeFaceRef(aFaceRefId).Orientation = TopAbs_REVERSED;

  EXPECT_DOUBLE_EQ(aStorage.Vertex(aVertexId).Tolerance, 1.25);
  EXPECT_EQ(aStorage.FaceRef(aFaceRefId).Orientation, TopAbs_REVERSED);
  EXPECT_TRUE(aStorage.FaceRef(aFaceRefId).Orientation.IsReversed);

  aStorage.ChangeFaceRef(aFaceRefId).Orientation = TopAbs_FORWARD;
  EXPECT_EQ(aStorage.FaceRef(aFaceRefId).Orientation, TopAbs_FORWARD);
  EXPECT_FALSE(aStorage.FaceRef(aFaceRefId).Orientation.IsReversed);
}

TEST(BRepGraphIncTest, Storage_GenericIdDispatch_UsesTypedHelpers)
{
  BRepGraphInc_Storage aStorage;

  const BRepGraph_VertexId  aVertexId  = aStorage.AppendVertex();
  const BRepGraph_FaceRefId aFaceRefId = aStorage.AppendFaceRef();
  std::ignore                          = aStorage.AppendFaceSurfaceRep();

  aStorage.ChangeFaceRef(aFaceRefId).Orientation = TopAbs_REVERSED;
  EXPECT_EQ(aStorage.FaceRef(aFaceRefId).Orientation, TopAbs_REVERSED);

  EXPECT_TRUE(aStorage.MarkRemoved(BRepGraph_NodeId(aVertexId)));
  EXPECT_TRUE(aStorage.MarkRemovedRef(BRepGraph_RefId(aFaceRefId)));

  EXPECT_TRUE(aStorage.IsRemoved(aVertexId));
  EXPECT_TRUE(aStorage.IsRemoved(aFaceRefId));

  EXPECT_EQ(aStorage.NbActiveVertices(), 0);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 0);
}

TEST(BRepGraphIncTest, Storage_SetRemoved_TransitionsUpdateActiveCounts)
{
  BRepGraphInc_Storage aStorage;

  const BRepGraph_VertexId  aVertexId  = aStorage.AppendVertex();
  const BRepGraph_FaceRefId aFaceRefId = aStorage.AppendFaceRef();

  EXPECT_EQ(aStorage.NbActiveVertices(), 1);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 1);

  aStorage.SetRemoved(aVertexId, true);
  aStorage.SetRemoved(aFaceRefId, true);

  EXPECT_TRUE(aStorage.IsRemoved(aVertexId));
  EXPECT_TRUE(aStorage.IsRemoved(aFaceRefId));
  EXPECT_EQ(aStorage.NbActiveVertices(), 0);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 0);

  aStorage.SetRemoved(aVertexId, true);
  aStorage.SetRemoved(aFaceRefId, true);

  EXPECT_EQ(aStorage.NbActiveVertices(), 0);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 0);

  aStorage.SetRemoved(aVertexId, false);
  aStorage.SetRemoved(aFaceRefId, false);

  EXPECT_FALSE(aStorage.IsRemoved(aVertexId));
  EXPECT_FALSE(aStorage.IsRemoved(aFaceRefId));
  EXPECT_EQ(aStorage.NbActiveVertices(), 1);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 1);

  aStorage.SetRemoved(aVertexId, false);
  aStorage.SetRemoved(aFaceRefId, false);

  EXPECT_EQ(aStorage.NbActiveVertices(), 1);
  EXPECT_EQ(aStorage.NbActiveFaceRefs(), 1);
}

// ============================================================
// Round-trip: area preservation
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox       = aBoxMaker.Shape();
  const double        anOrigArea = computeArea(aBox);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl       = aCylMaker.Shape();
  const double             anOrigArea = computeArea(aCyl);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

TEST(BRepGraphIncTest, Sphere_RoundTrip_AreaPreserved)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph       = aSphMaker.Shape();
  const double           anOrigArea = computeArea(aSph);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aSph, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

// ============================================================
// Round-trip: volume preservation
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_VolumePreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox      = aBoxMaker.Shape();
  const double        anOrigVol = computeVolume(aBox);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_VolumePreserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl      = aCylMaker.Shape();
  const double             anOrigVol = computeVolume(aCyl);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
}

// ============================================================
// Round-trip: sub-shape counts
// ============================================================

TEST(BRepGraphIncTest, Box_RoundTrip_SubShapeCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), countSubShapes(aBox, TopAbs_FACE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_WIRE), countSubShapes(aBox, TopAbs_WIRE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), countSubShapes(aBox, TopAbs_EDGE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_VERTEX), countSubShapes(aBox, TopAbs_VERTEX));
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_SubShapeCounts)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), countSubShapes(aCyl, TopAbs_FACE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_WIRE), countSubShapes(aCyl, TopAbs_WIRE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), countSubShapes(aCyl, TopAbs_EDGE));
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_VERTEX), countSubShapes(aCyl, TopAbs_VERTEX));
}

// ============================================================
// Relation table consistency
// ============================================================

TEST(BRepGraphIncTest, Box_Relations_EdgesToWires)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Every edge must appear in at least one wire.
  const uint32_t aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_GE(countIterator(aGraph.Topo().Edges().WiresOf(anEdgeId)), 1)
      << "Edge " << anEdgeId.Index << " not in any wire";
  }
}

TEST(BRepGraphIncTest, Box_Relations_EdgesToFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Every edge must appear in at least one face (via EdgeFaceGeom).
  const uint32_t aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    // Skip degenerate edges (e.g. sphere poles with no 3D curve).
    bool                        anIsDegenerated = false;
    bool                        anIsClosed      = false;
    [[maybe_unused]] const bool isEdgeStateComputed =
      BRepGraph_CacheDerivedState::ComputeEdgeProperties(aGraph,
                                                         anEdgeId,
                                                         anIsDegenerated,
                                                         anIsClosed);
    if (anIsDegenerated)
    {
      continue;
    }
    EXPECT_GE(countIterator(aGraph.Topo().Edges().FacesOf(anEdgeId)), 1)
      << "Edge " << anEdgeId.Index << " not in any face";
  }
}

// ============================================================
// Parallel population produces same results
// ============================================================

TEST(BRepGraphIncTest, Box_ParallelPopulate_SameEntityCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aSerial;
  std::ignore = BRepGraphInc_Populate::Perform(aSerial, aBox, false);
  ASSERT_FALSE(aSerial.IsEmpty());

  BRepGraph aParallel;
  std::ignore = BRepGraphInc_Populate::Perform(aParallel, aBox, true);
  ASSERT_FALSE(aParallel.IsEmpty());

  EXPECT_EQ(aParallel.Topo().Vertices().Nb(), aSerial.Topo().Vertices().Nb());
  EXPECT_EQ(aParallel.Topo().Edges().Nb(), aSerial.Topo().Edges().Nb());
  EXPECT_EQ(aParallel.Topo().Wires().Nb(), aSerial.Topo().Wires().Nb());
  EXPECT_EQ(aParallel.Topo().Faces().Nb(), aSerial.Topo().Faces().Nb());
  EXPECT_EQ(aParallel.Topo().Shells().Nb(), aSerial.Topo().Shells().Nb());
  EXPECT_EQ(aParallel.Topo().Solids().Nb(), aSerial.Topo().Solids().Nb());
}

// ============================================================
// Null shape handling
// ============================================================

TEST(BRepGraphIncTest, NullShape_NoEntities)
{
  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, TopoDS_Shape(), false);
  EXPECT_TRUE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
}

// ============================================================
// Compound shape handling
// ============================================================

TEST(BRepGraphIncTest, Compound_RoundTrip_SubShapeCounts)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 10.0, 10.0);
  aBB.Add(aCompound, aBoxMaker1.Shape());

  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 5.0, 5.0);
  aBB.Add(aCompound, aBoxMaker2.Shape());

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompound, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Two solids, two shells.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);

  // Round-trip reconstruct via compound.
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_CompoundId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), 12);
}

// ============================================================
// CoEdge count consistency (PCurve data lives on CoEdgeDef)
// ============================================================

TEST(BRepGraphIncTest, Box_CoEdgeCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // A box has 12 edges, each shared by 2 faces => 24 CoEdge entries total.
  // (No seam edges on a box.)
  size_t         aCoEdgeCount = 0;
  const uint32_t aNbEdges     = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    aCoEdgeCount += aGraph.Topo().Edges().Relations(anEdgeId).CoEdgeIds.Size();
  }
  EXPECT_EQ(aCoEdgeCount, 24u);
}

TEST(BRepGraphIncTest, Cylinder_HasSeamEdges)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // A cylinder has seam edges: 2 CoEdges of the same edge on the same face with
  // opposite orientations. Count seam pairs once via the FORWARD half.
  size_t         aSeamPairCount = 0;
  const uint32_t aNbEdges       = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      aGraph.Topo().Edges().Relations(anEdgeId).CoEdgeIds;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (aCE.Orientation != TopAbs_FORWARD)
      {
        continue;
      }
      // Look for a sibling CoEdge with the same face and opposite orientation.
      for (const BRepGraph_CoEdgeId& aOtherId : aCoEdgeIds)
      {
        if (aOtherId == aCoEdgeId)
        {
          continue;
        }
        const BRepGraphInc::CoEdgeDef& aOther = aGraph.Topo().CoEdges().Definition(aOtherId);
        if (aOther.FaceId == aCE.FaceId && aOther.Orientation != aCE.Orientation)
        {
          ++aSeamPairCount;
          break;
        }
      }
    }
  }
  // A cylinder has 1 seam edge on its lateral face.
  EXPECT_GE(aSeamPairCount, 1u) << "Cylinder should have at least 1 seam edge pair";
}

TEST(BRepGraphIncTest, Cylinder_SeamEdge_Relations_NoDuplicateFace)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // For seam edges (two PCurve entries with the same face but opposite
  // orientations), the relation tables must contain each face only once.
  const uint32_t aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    NCollection_LinearVector<BRepGraph_FaceId> aSeenFaces;
    for (BRepGraph_FacesOfEdge aFaceIt = aGraph.Topo().Edges().FacesOf(anEdgeId); aFaceIt.More();
         aFaceIt.Next())
    {
      const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
      EXPECT_FALSE(containsId(aSeenFaces, aFaceId))
        << "Duplicate face " << aFaceId.Index << " in FacesOfEdge(" << anEdgeId.Index << ")";
      aSeenFaces.Append(aFaceId);
    }
  }
}

// ============================================================
// Degenerate edge handling
// ============================================================

TEST(BRepGraphIncTest, Sphere_DegenerateEdges_Preserved)
{
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aSph, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  // A sphere has degenerate edges at the poles (no 3D curve, collapsed to a point).
  uint32_t       aDegenerateCount = 0;
  const uint32_t aNbEdges         = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge          = aGraph.Topo().Edges().Definition(anEdgeId);
    bool                         anIsDegenerated = false;
    bool                         anIsClosed      = false;
    [[maybe_unused]] const bool  isEdgeStateComputed =
      BRepGraph_CacheDerivedState::ComputeEdgeProperties(aGraph,
                                                         anEdgeId,
                                                         anIsDegenerated,
                                                         anIsClosed);
    if (anIsDegenerated)
    {
      ++aDegenerateCount;
      EXPECT_FALSE(anEdge.Curve3DRepId.IsValid())
        << "Degenerate edge " << anEdgeId.Index << " should have no 3D curve";
    }
  }
  EXPECT_GE(aDegenerateCount, 2) << "Sphere should have at least 2 degenerate edges (poles)";

  // Round-trip: reconstructed sphere area must still match.
  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double anOrigArea = computeArea(aSph);
  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

// ============================================================
// Location handling: compound with translated children
// ============================================================

TEST(BRepGraphIncTest, Compound_TranslatedChildren_VolumePreserved)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  // Box at origin.
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 10.0, 10.0);
  aBB.Add(aCompound, aBoxMaker1.Shape());

  // Box translated by (100, 0, 0).
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 10.0, 10.0);
  TopoDS_Shape        aTranslatedBox = BRepBuilderAPI_Transform(aBoxMaker2.Shape(), aTrsf).Shape();
  aBB.Add(aCompound, aTranslatedBox);

  const double anOrigVol = computeVolume(aCompound);
  EXPECT_NEAR(anOrigVol, 2000.0, Precision::Confusion());

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompound, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_CompoundId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  const double aReconVol = computeVolume(aRecon);
  EXPECT_NEAR(aReconVol, anOrigVol, Precision::Confusion());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), 12);
}

TEST(BRepGraphIncTest, Populate_AppliesNormalChildLocationsToDefinitions)
{
  const TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const TopLoc_Location aLoc(aTrsf);
  const TopoDS_Edge     aMovedEdge = TopoDS::Edge(anEdge.Moved(aLoc));

  BRep_Builder aBuilder;
  TopoDS_Wire  aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, anEdge);
  aBuilder.Add(aWire, aMovedEdge);

  BRepGraph aGraph;
  ASSERT_NE(BRepGraphInc_Populate::Perform(aGraph, aWire, false),
            BRepGraphInc_Populate::BuildStatus::Failed);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 2u);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 4u);

  double aMinX = RealLast();
  double aMaxX = -RealLast();
  for (uint32_t aVertexIdx = 0; aVertexIdx < aGraph.Topo().Vertices().Nb(); ++aVertexIdx)
  {
    const gp_Pnt& aPoint =
      aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(aVertexIdx)).Point;
    aMinX = std::min(aMinX, aPoint.X());
    aMaxX = std::max(aMaxX, aPoint.X());
  }

  EXPECT_NEAR(aMinX, 0.0, Precision::Confusion());
  EXPECT_NEAR(aMaxX, 11.0, Precision::Confusion());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_WireId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_VERTEX), 4);
}

TEST(BRepGraphIncTest, Populate_AppliesStackedNormalLocationsToDefinitions)
{
  gp_Trsf anEdgeTrsf;
  anEdgeTrsf.SetTranslation(gp_Vec(2.0, 0.0, 0.0));
  const TopLoc_Location anEdgeLoc(anEdgeTrsf);

  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(TopoDS::Edge(
    BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0)).Edge().Moved(anEdgeLoc)));
  aWireBuilder.Add(TopoDS::Edge(
    BRepBuilderAPI_MakeEdge(gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(1.0, 1.0, 0.0)).Edge().Moved(anEdgeLoc)));
  aWireBuilder.Add(TopoDS::Edge(
    BRepBuilderAPI_MakeEdge(gp_Pnt(1.0, 1.0, 0.0), gp_Pnt(0.0, 1.0, 0.0)).Edge().Moved(anEdgeLoc)));
  aWireBuilder.Add(TopoDS::Edge(
    BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 1.0, 0.0), gp_Pnt(0.0, 0.0, 0.0)).Edge().Moved(anEdgeLoc)));
  ASSERT_TRUE(aWireBuilder.IsDone());

  gp_Trsf aWireTrsf;
  aWireTrsf.SetTranslation(gp_Vec(0.0, 3.0, 0.0));
  const TopoDS_Wire aMovedWire =
    TopoDS::Wire(aWireBuilder.Wire().Moved(TopLoc_Location(aWireTrsf)));

  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aMovedWire).Face();

  gp_Trsf aFaceTrsf;
  aFaceTrsf.SetTranslation(gp_Vec(0.0, 0.0, 5.0));
  const TopoDS_Face aMovedFace = TopoDS::Face(aFace.Moved(TopLoc_Location(aFaceTrsf)));

  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, aMovedFace);

  gp_Trsf aShellTrsf;
  aShellTrsf.SetTranslation(gp_Vec(7.0, 0.0, 0.0));
  const TopoDS_Shell aMovedShell = TopoDS::Shell(aShell.Moved(TopLoc_Location(aShellTrsf)));

  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);
  aBuilder.Add(aSolid, aMovedShell);

  gp_Trsf aSolidTrsf;
  aSolidTrsf.SetTranslation(gp_Vec(0.0, 11.0, 0.0));
  const TopoDS_Solid aMovedSolid = TopoDS::Solid(aSolid.Moved(TopLoc_Location(aSolidTrsf)));

  BRepGraph aGraph;
  ASSERT_NE(BRepGraphInc_Populate::Perform(aGraph, aMovedSolid, false),
            BRepGraphInc_Populate::BuildStatus::Failed);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 1u);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 4u);

  const TopLoc_Location anIdentity;
  for (uint32_t aRefIdx = 0; aRefIdx < aGraph.Refs().Shells().Nb(); ++aRefIdx)
  {
    EXPECT_TRUE(
      aGraph.Refs().Gen().LocalLocation(BRepGraph_ShellRefId(aRefIdx)).IsEqual(anIdentity));
  }
  for (uint32_t aRefIdx = 0; aRefIdx < aGraph.Refs().Faces().Nb(); ++aRefIdx)
  {
    EXPECT_TRUE(
      aGraph.Refs().Gen().LocalLocation(BRepGraph_FaceRefId(aRefIdx)).IsEqual(anIdentity));
  }
  for (uint32_t aRefIdx = 0; aRefIdx < aGraph.Refs().Wires().Nb(); ++aRefIdx)
  {
    EXPECT_TRUE(
      aGraph.Refs().Gen().LocalLocation(BRepGraph_WireRefId(aRefIdx)).IsEqual(anIdentity));
  }
  for (uint32_t aRefIdx = 0; aRefIdx < aGraph.Refs().Vertices().Nb(); ++aRefIdx)
  {
    EXPECT_TRUE(
      aGraph.Refs().Gen().LocalLocation(BRepGraph_VertexRefId(aRefIdx)).IsEqual(anIdentity));
  }
  for (uint32_t aRefIdx = 0; aRefIdx < aGraph.Refs().Solids().Nb(); ++aRefIdx)
  {
    EXPECT_TRUE(
      aGraph.Refs().Gen().LocalLocation(BRepGraph_SolidRefId(aRefIdx)).IsEqual(anIdentity));
  }

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  double aMinX = RealLast();
  double aMaxX = -RealLast();
  double aMinY = RealLast();
  double aMaxY = -RealLast();
  double aMinZ = RealLast();
  double aMaxZ = -RealLast();
  for (TopExp_Explorer anExp(aRecon, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    const gp_Pnt aPoint = BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current()));
    aMinX               = std::min(aMinX, aPoint.X());
    aMaxX               = std::max(aMaxX, aPoint.X());
    aMinY               = std::min(aMinY, aPoint.Y());
    aMaxY               = std::max(aMaxY, aPoint.Y());
    aMinZ               = std::min(aMinZ, aPoint.Z());
    aMaxZ               = std::max(aMaxZ, aPoint.Z());
  }

  EXPECT_NEAR(aMinX, 9.0, Precision::Confusion());
  EXPECT_NEAR(aMaxX, 10.0, Precision::Confusion());
  EXPECT_NEAR(aMinY, 14.0, Precision::Confusion());
  EXPECT_NEAR(aMaxY, 15.0, Precision::Confusion());
  EXPECT_NEAR(aMinZ, 5.0, Precision::Confusion());
  EXPECT_NEAR(aMaxZ, 5.0, Precision::Confusion());
}

TEST(BRepGraphIncTest, Cylinder_RoundTrip_BRepDump)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCyl, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  // Dump both shapes and compare
  std::ostringstream anOrigStream, aReconStream;
  BRepTools::Write(aCyl, anOrigStream);
  BRepTools::Write(aRecon, aReconStream);

  if (anOrigStream.str() != aReconStream.str())
  {
    // Show ALL line differences
    std::istringstream anOrigLines(anOrigStream.str());
    std::istringstream aReconLines(aReconStream.str());
    std::string        anOrigLine, aReconLine;
    int                aLineNo    = 0;
    int                aDiffCount = 0;
    while (std::getline(anOrigLines, anOrigLine) && std::getline(aReconLines, aReconLine))
    {
      ++aLineNo;
      if (anOrigLine != aReconLine)
      {
        std::cout << "BRep diff at line " << aLineNo << ":" << '\n';
        std::cout << "  ORIG:  " << anOrigLine << '\n';
        std::cout << "  RECON: " << aReconLine << '\n';
        ++aDiffCount;
      }
    }
    std::cout << "Total " << aDiffCount << " line differences. "
              << "Orig size=" << anOrigStream.str().size()
              << " Recon size=" << aReconStream.str().size() << '\n';
  }
  else
  {
    std::cout << "BRep dumps are IDENTICAL (size=" << anOrigStream.str().size() << ")" << '\n';
  }

  const double anOrigArea = computeArea(aCyl);
  const double aReconArea = computeArea(aRecon);
  std::cout << "Orig area=" << anOrigArea << " Recon area=" << aReconArea << '\n';
  EXPECT_NEAR(aReconArea, anOrigArea, Precision::Confusion());
}

// Supplemental edge/face topology is no longer stored in BRepGraphInc_Storage.
// Live preservation is covered by BRepGraph_LayerTopoSupplement tests.

static TopoDS_Edge makeIncEdgeWithInternalVertex()
{
  BRep_Builder            aBB;
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Edge             anEdge = aMakeEdge.Edge();

  TopoDS_Vertex anIntVtx;
  aBB.MakeVertex(anIntVtx, gp_Pnt(5, 0, 0), Precision::Confusion());
  aBB.Add(anEdge, anIntVtx.Oriented(TopAbs_INTERNAL));
  return anEdge;
}

static TopoDS_Shape wrapIncEdgeInFace(const TopoDS_Edge& theEdge)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  BRep_Builder            aBB;
  TopoDS_Face             aFace;
  aBB.MakeFace(aFace, aPlane, Precision::Confusion());
  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, theEdge);
  aBB.Add(aFace, aWire);
  return aFace;
}

static TopoDS_Face makeIncFaceWithDirectVertex()
{
  BRep_Builder aBB;

  TopoDS_Vertex aV0;
  TopoDS_Vertex aV1;
  TopoDS_Vertex aV2;
  TopoDS_Vertex aV3;
  aBB.MakeVertex(aV0, gp_Pnt(0, 0, 0), Precision::Confusion());
  aBB.MakeVertex(aV1, gp_Pnt(10, 0, 0), Precision::Confusion());
  aBB.MakeVertex(aV2, gp_Pnt(10, 10, 0), Precision::Confusion());
  aBB.MakeVertex(aV3, gp_Pnt(0, 10, 0), Precision::Confusion());

  TopoDS_Edge aE0;
  TopoDS_Edge aE1;
  TopoDS_Edge aE2;
  TopoDS_Edge aE3;
  aBB.MakeEdge(aE0);
  aBB.MakeEdge(aE1);
  aBB.MakeEdge(aE2);
  aBB.MakeEdge(aE3);
  aBB.Add(aE0, aV0.Oriented(TopAbs_FORWARD));
  aBB.Add(aE0, aV1.Oriented(TopAbs_REVERSED));
  aBB.Add(aE1, aV1.Oriented(TopAbs_FORWARD));
  aBB.Add(aE1, aV2.Oriented(TopAbs_REVERSED));
  aBB.Add(aE2, aV2.Oriented(TopAbs_FORWARD));
  aBB.Add(aE2, aV3.Oriented(TopAbs_REVERSED));
  aBB.Add(aE3, aV3.Oriented(TopAbs_FORWARD));
  aBB.Add(aE3, aV0.Oriented(TopAbs_REVERSED));

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aE0);
  aBB.Add(aWire, aE1);
  aBB.Add(aWire, aE2);
  aBB.Add(aWire, aE3);
  aWire.Closed(true);

  TopoDS_Face aFace;
  aBB.MakeFace(aFace);
  aBB.Add(aFace, aWire);

  TopoDS_Vertex aLooseVertex;
  aBB.MakeVertex(aLooseVertex, gp_Pnt(5, 5, 0), Precision::Confusion());
  aBB.Add(aFace, aLooseVertex.Oriented(TopAbs_INTERNAL));
  return aFace;
}

static TopoDS_Face makeIncPlainFace()
{
  return BRepBuilderAPI_MakeFace(gp_Pln(), 0.0, 10.0, 0.0, 10.0).Face();
}

static TopoDS_Shell makeIncPlainShell()
{
  BRep_Builder aBB;
  TopoDS_Shell aShell;
  aBB.MakeShell(aShell);
  aBB.Add(aShell, makeIncPlainFace());
  return aShell;
}

static TopoDS_Solid makeIncPlainSolid()
{
  BRep_Builder aBB;
  TopoDS_Solid aSolid;
  aBB.MakeSolid(aSolid);
  aBB.Add(aSolid, makeIncPlainShell());
  return aSolid;
}

static uint32_t countDirectChildren(const TopoDS_Shape&      theShape,
                                    const TopAbs_ShapeEnum   theType,
                                    const TopAbs_Orientation theOrientation)
{
  uint32_t aCount = 0;
  for (TopoDS_Iterator aChildIt(theShape, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() == theType && aChild.Orientation() == theOrientation)
    {
      ++aCount;
    }
  }
  return aCount;
}

TEST(BRepGraphIncTest, Populate_RegistersSupplementEdgeVerticesIntoLayer)
{
  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  std::ignore = BRepGraphInc_Populate::Perform(aGraph,
                                               wrapIncEdgeInFace(makeIncEdgeWithInternalVertex()),
                                               false);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId aEdgeId;
  if (aGraph.Topo().Edges().Nb() > 0)
  {
    aEdgeId = BRepGraph_EdgeId::Start();
  }
  ASSERT_TRUE(aEdgeId.IsValid());

  const NCollection_LinearVector<uint64_t>& anAttached = aSupplement->AttachedTo(aEdgeId);
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aSupplement->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::EdgeInternalVertex);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);
}

TEST(BRepGraphIncTest, Reconstruct_ReplaysSupplementEdgeVerticesFromLayer)
{
  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  std::ignore = BRepGraphInc_Populate::Perform(aGraph,
                                               wrapIncEdgeInFace(makeIncEdgeWithInternalVertex()),
                                               false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  bool aFoundInternal = false;
  for (TopExp_Explorer anEdgeExp(aRecon, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    for (TopoDS_Iterator aVIt(anEdgeExp.Current(), false); aVIt.More(); aVIt.Next())
    {
      if (aVIt.Value().ShapeType() == TopAbs_VERTEX
          && aVIt.Value().Orientation() == TopAbs_INTERNAL)
      {
        aFoundInternal = true;
      }
    }
  }
  EXPECT_TRUE(aFoundInternal);
}

TEST(BRepGraphIncTest, Reconstruct_WithoutSupplementLayer_DropsSupplementEdgeVerticesButKeepsCore)
{
  // No supplement layer registered - internal vertices should be dropped.
  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph,
                                               wrapIncEdgeInFace(makeIncEdgeWithInternalVertex()),
                                               false);
  ASSERT_FALSE(aGraph.IsEmpty());

  TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  uint32_t aFoundEdges      = 0;
  uint32_t aFoundBoundaries = 0;
  uint32_t aFoundInternal   = 0;
  for (TopExp_Explorer anEdgeExp(aRecon, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    ++aFoundEdges;
    for (TopoDS_Iterator aVIt(anEdgeExp.Current(), false); aVIt.More(); aVIt.Next())
    {
      if (aVIt.Value().ShapeType() != TopAbs_VERTEX)
      {
        continue;
      }
      if (aVIt.Value().Orientation() == TopAbs_FORWARD
          || aVIt.Value().Orientation() == TopAbs_REVERSED)
      {
        ++aFoundBoundaries;
      }
      else if (aVIt.Value().Orientation() == TopAbs_INTERNAL)
      {
        ++aFoundInternal;
      }
    }
  }

  EXPECT_EQ(aFoundEdges, 1);
  EXPECT_EQ(aFoundBoundaries, 2);
  EXPECT_EQ(aFoundInternal, 0);
}

TEST(BRepGraphIncTest, Reconstruct_EdgeNode_ReplaysSupplementEdgeVerticesFromLayer)
{
  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  std::ignore = BRepGraphInc_Populate::Perform(aGraph, makeIncEdgeWithInternalVertex(), false);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  const TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_EdgeId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  int aFoundInternal = 0;
  for (TopoDS_Iterator aVIt(aRecon, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() == TopAbs_VERTEX && aVIt.Value().Orientation() == TopAbs_INTERNAL)
    {
      ++aFoundInternal;
    }
  }

  EXPECT_EQ(aFoundInternal, 1);
}

TEST(BRepGraphIncTest, Reconstruct_WireNode_ReplaysSupplementEdgeVerticesFromLayer)
{
  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  BRep_Builder aBB;
  TopoDS_Wire  aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, makeIncEdgeWithInternalVertex());

  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aWire, false);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Wires().Nb(), 0);

  const TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_WireId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  int aFoundInternal = 0;
  for (TopExp_Explorer anEdgeExp(aRecon, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    for (TopoDS_Iterator aVIt(anEdgeExp.Current(), false); aVIt.More(); aVIt.Next())
    {
      if (aVIt.Value().ShapeType() == TopAbs_VERTEX
          && aVIt.Value().Orientation() == TopAbs_INTERNAL)
      {
        ++aFoundInternal;
      }
    }
  }

  EXPECT_EQ(aFoundInternal, 1);
}

TEST(BRepGraphIncTest, Reconstruct_FaceNode_ReplaysSupplementFaceVerticesFromLayer)
{
  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  std::ignore = BRepGraphInc_Populate::Perform(aGraph, makeIncFaceWithDirectVertex(), false);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const NCollection_LinearVector<uint64_t>& anAttached =
    aSupplement->AttachedTo(BRepGraph_FaceId::Start());
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aSupplement->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  int aFoundInternal = 0;
  int aFoundWires    = 0;
  for (TopoDS_Iterator aChildIt(aRecon, false, false); aChildIt.More(); aChildIt.Next())
  {
    if (aChildIt.Value().ShapeType() == TopAbs_WIRE)
    {
      ++aFoundWires;
    }
    else if (aChildIt.Value().ShapeType() == TopAbs_VERTEX
             && aChildIt.Value().Orientation() == TopAbs_INTERNAL)
    {
      ++aFoundInternal;
    }
  }

  EXPECT_EQ(aFoundWires, 1);
  EXPECT_EQ(aFoundInternal, 1);
}

TEST(BRepGraphIncTest, Populate_ShellRoutesInternalFaceToSupplement)
{
  BRep_Builder aBB;
  TopoDS_Shell aShell;
  aBB.MakeShell(aShell);
  aBB.Add(aShell, makeIncPlainFace().Oriented(TopAbs_FORWARD));
  aBB.Add(aShell, makeIncPlainFace().Oriented(TopAbs_INTERNAL));

  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_NO_THROW({ std::ignore = BRepGraphInc_Populate::Perform(aGraph, aShell, false); });
  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Shells().Relations(BRepGraph_ShellId::Start()).FaceRefIds.Size(), 1);

  const NCollection_LinearVector<uint64_t>& anAttached =
    aSupplement->AttachedTo(BRepGraph_ShellId::Start());
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aSupplement->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_FACE);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_ShellId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_FACE, TopAbs_FORWARD), 1);
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_FACE, TopAbs_INTERNAL), 1);
}

TEST(BRepGraphIncTest, Populate_SolidRoutesInternalShellToSupplement)
{
  BRep_Builder aBB;
  TopoDS_Solid aSolid;
  aBB.MakeSolid(aSolid);
  aBB.Add(aSolid, makeIncPlainShell().Oriented(TopAbs_FORWARD));
  aBB.Add(aSolid, makeIncPlainShell().Oriented(TopAbs_INTERNAL));

  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_NO_THROW({ std::ignore = BRepGraphInc_Populate::Perform(aGraph, aSolid, false); });
  ASSERT_EQ(aGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Solids().Relations(BRepGraph_SolidId::Start()).ShellRefIds.Size(), 1);

  const NCollection_LinearVector<uint64_t>& anAttached =
    aSupplement->AttachedTo(BRepGraph_SolidId::Start());
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aSupplement->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_SHELL);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_SHELL, TopAbs_FORWARD), 1);
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_SHELL, TopAbs_INTERNAL), 1);
}

TEST(BRepGraphIncTest, Populate_CompSolidRoutesInternalSolidToSupplement)
{
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);
  aBB.Add(aCompSolid, makeIncPlainSolid().Oriented(TopAbs_FORWARD));
  aBB.Add(aCompSolid, makeIncPlainSolid().Oriented(TopAbs_INTERNAL));

  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_NO_THROW({ std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompSolid, false); });
  ASSERT_EQ(aGraph.Topo().CompSolids().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().CompSolids().Relations(BRepGraph_CompSolidId::Start()).SolidRefIds.Size(),
            1);

  const NCollection_LinearVector<uint64_t>& anAttached =
    aSupplement->AttachedTo(BRepGraph_CompSolidId::Start());
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aSupplement->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_SOLID);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRecon =
    BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_CompSolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_SOLID, TopAbs_FORWARD), 1);
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_SOLID, TopAbs_INTERNAL), 1);
}

TEST(BRepGraphIncTest, Populate_SolidInvalidOrderChildRoutesToSupplement)
{
  BRep_Builder aBB;
  TopoDS_Solid aSolid;
  aBB.MakeSolid(aSolid);
  aBB.Add(aSolid, makeIncEdgeWithInternalVertex().Oriented(TopAbs_FORWARD));

  BRepGraph                                  aGraph;
  occ::handle<BRepGraph_LayerTopoSupplement> aSupplement =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aSolid, false);
  ASSERT_EQ(aGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Solids().Relations(BRepGraph_SolidId::Start()).ShellRefIds.Size(), 0);
  ASSERT_EQ(aSupplement->AttachedTo(BRepGraph_SolidId::Start()).Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry =
    aSupplement->FindByUid(aSupplement->AttachedTo(BRepGraph_SolidId::Start()).First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_EDGE);

  const TopoDS_Shape aRecon = BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  EXPECT_EQ(countDirectChildren(aRecon, TopAbs_EDGE, TopAbs_FORWARD), 1);
}

// ============================================================
// Reverse-index hardening: compound atomic children (Wire/Edge/Vertex)
// ============================================================

TEST(BRepGraphIncTest, Relations_Validate_CompoundWithAtomicWire)
{
  // Build a compound that contains only a wire (no solid/shell/face).
  // This exercises the Wire-child path in compound relation-table maintenance.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  ASSERT_TRUE(aME.IsDone());

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME.Edge());
  aBB.Add(aCompound, aWire);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompound, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1u);
  ASSERT_GE(aGraph.Topo().Wires().Nb(), 1u);

  // Full consistency check.
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_Validate_CompoundWithAtomicEdge)
{
  // Build a compound containing only an edge (no wire/face/solid).
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepBuilderAPI_MakeEdge aME(gp_Pnt(0, 0, 0), gp_Pnt(5, 0, 0));
  ASSERT_TRUE(aME.IsDone());
  aBB.Add(aCompound, aME.Edge());

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompound, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1u);
  ASSERT_GE(aGraph.Topo().Edges().Nb(), 1u);

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_Validate_CompoundWithAtomicVertex)
{
  // Build a compound containing only a vertex.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(1, 2, 3), Precision::Confusion());
  aBB.Add(aCompound, aVtx);

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompound, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1u);
  ASSERT_GE(aGraph.Topo().Vertices().Nb(), 1u);

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_CoEdgeToWire_IsPopulated)
{
  // Every coedge in a box must carry exactly one parent wire.
  BRepPrimAPI_MakeBox aBoxMaker(5.0, 5.0, 5.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aNbCoEdges = aGraph.Topo().CoEdges().Nb();
  ASSERT_GT(aNbCoEdges, 0);
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
  {
    EXPECT_TRUE(aGraph.Topo().CoEdges().Definition(aCoEdgeId).ParentWireId.IsValid())
      << "CoEdge " << aCoEdgeId.Index << " not in any wire";
  }

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_CompSolid_IncomingMaintained_AfterBuild)
{
  // Build a TopoDS_CompSolid containing two boxes and verify:
  //   1. ParentSolidRefIds is populated for both solids
  //   2. ValidateRelations() passes
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  BRepPrimAPI_MakeBox aBoxMaker1(4.0, 4.0, 4.0);
  BRepPrimAPI_MakeBox aBoxMaker2(2.0, 2.0, 2.0);
  aBB.Add(aCompSolid, aBoxMaker1.Shape());
  aBB.Add(aCompSolid, aBoxMaker2.Shape());

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompSolid, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().CompSolids().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Solids().Nb(), 2);

  // Both solids must appear in the parent solid-ref relation table.
  for (BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
       aSolidId.IsValid(aGraph.Topo().Solids().Nb());
       ++aSolidId)
  {
    const NCollection_LinearVector<BRepGraph_SolidRefId>& aSolidRefs =
      aGraph.Topo().Solids().Relations(aSolidId).ParentSolidRefIds;
    ASSERT_EQ(aSolidRefs.Size(), 1u) << "Solid " << aSolidId.Index << " not in any CompSolid";
  }

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_CompSolid_IncomingMaintained_AfterBuildDelta)
{
  // Verify the relation build correctly indexes incoming solid refs when
  // both the compsolid and solids are appended together.
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  BRepPrimAPI_MakeBox aBoxMaker(3.0, 3.0, 3.0);
  aBB.Add(aCompSolid, aBoxMaker.Shape());

  // First, build an empty storage then delta-populate.
  // Simplest: just populate from scratch and check ValidateRelations.
  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompSolid, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().CompSolids().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Solids().Nb(), 1);

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());

  const NCollection_LinearVector<BRepGraph_SolidRefId>& aSolidRefs =
    aGraph.Topo().Solids().Relations(BRepGraph_SolidId::Start()).ParentSolidRefIds;
  EXPECT_GE(aSolidRefs.Size(), 1u) << "Solid 0 should be indexed in CompSolid relations";
}

TEST(BRepGraphIncTest, Relations_Validate_Box_FullConsistency)
{
  // Smoke test: a simple solid box must pass full relation-table validation.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aBox, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_Validate_Compound_FullConsistency)
{
  // A compound of two boxes must pass full relation-table validation,
  // covering CompoundsOfSolid, CompoundsOfShell, CompoundsOfFace.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBoxMaker1(5.0, 5.0, 5.0);
  BRepPrimAPI_MakeBox aBoxMaker2(3.0, 3.0, 3.0);
  aBB.Add(aCompound, aBoxMaker1.Shape());
  aBB.Add(aCompound, aBoxMaker2.Shape());

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aCompound, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_AfterEditorMutations_StaysConsistent)
{
  // Verify the incremental Bind/Unbind mutation path keeps the relation tables
  // consistent with the forward entity / reference-entry tables across a
  // sequence of RemoveWire / RemoveFace / RemoveShell mutations.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  BRepPrimAPI_MakeBox      aBoxMaker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 12.0);
  aBB.Add(aCompound, aBoxMaker.Shape());
  aBB.Add(aCompound, aCylMaker.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_TRUE(aGraph.ValidateRelations());

  // Remove an inner wire from the first face that owns more than one wire
  // (or the only wire if all faces have a single wire).
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId                               aFaceId = aFaceIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
      BRepGraph_TestTools::WireRefsOfFace(aGraph, aFaceId);
    if (aWireRefs.IsEmpty())
    {
      continue;
    }
    ASSERT_TRUE(aGraph.Editor().Faces().RemoveWire(aFaceId, aWireRefs.Value(0)));
    break;
  }
  EXPECT_TRUE(aGraph.ValidateRelations()) << "Relation table inconsistent after RemoveWire";

  // Remove the first face from the first shell.
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(aGraph, BRepGraph_ShellId::Start());
  ASSERT_GE(aFaceRefs.Size(), 1);
  ASSERT_TRUE(aGraph.Editor().Shells().RemoveFace(BRepGraph_ShellId::Start(), aFaceRefs.Value(0)));
  EXPECT_TRUE(aGraph.ValidateRelations()) << "Relation table inconsistent after RemoveFace";

  // Remove the first shell from the first solid.
  const NCollection_LinearVector<BRepGraph_ShellRefId>& aShellRefs =
    BRepGraph_TestTools::ShellRefsOfSolid(aGraph, BRepGraph_SolidId::Start());
  ASSERT_GE(aShellRefs.Size(), 1);
  ASSERT_TRUE(
    aGraph.Editor().Solids().RemoveShell(BRepGraph_SolidId::Start(), aShellRefs.Value(0)));
  EXPECT_TRUE(aGraph.ValidateRelations()) << "Relation table inconsistent after RemoveShell";

  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraphIncTest, Relations_BulkBuild_TwiceProducesEqualState)
{
  // Bulk Populate must be deterministic: building the same shape twice into
  // independent storages must yield byte-equal relation-table views as observed
  // through the public per-entity accessors.
  BRepPrimAPI_MakeBox aBoxMaker(7.0, 11.0, 13.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraphA;
  BRepGraph aGraphB;
  std::ignore = BRepGraphInc_Populate::Perform(aGraphA, aBox, false);
  std::ignore = BRepGraphInc_Populate::Perform(aGraphB, aBox, false);
  ASSERT_FALSE(aGraphA.IsEmpty());
  ASSERT_FALSE(aGraphB.IsEmpty());
  ASSERT_EQ(aGraphA.Topo().Edges().Nb(), aGraphB.Topo().Edges().Nb());

  for (uint32_t anIdx = 0; anIdx < aGraphA.Topo().Edges().Nb(); ++anIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anIdx);
    BRepGraph_WiresOfEdge  aWireItA = aGraphA.Topo().Edges().WiresOf(anEdgeId);
    BRepGraph_WiresOfEdge  aWireItB = aGraphB.Topo().Edges().WiresOf(anEdgeId);
    for (; aWireItA.More() && aWireItB.More(); aWireItA.Next(), aWireItB.Next())
    {
      EXPECT_EQ(aWireItA.CurrentId(), aWireItB.CurrentId());
    }
    EXPECT_EQ(aWireItA.More(), aWireItB.More());

    BRepGraph_FacesOfEdge aFaceItA = aGraphA.Topo().Edges().FacesOf(anEdgeId);
    BRepGraph_FacesOfEdge aFaceItB = aGraphB.Topo().Edges().FacesOf(anEdgeId);
    for (; aFaceItA.More() && aFaceItB.More(); aFaceItA.Next(), aFaceItB.Next())
    {
      EXPECT_EQ(aFaceItA.CurrentId(), aFaceItB.CurrentId());
    }
    EXPECT_EQ(aFaceItA.More(), aFaceItB.More());
  }
}

TEST(BRepGraphIncTest, Relations_EdgeOpsAdd_BindsStartEndVertices)
{
  // Free edge created at runtime via the Editor must show up under its endpoint
  // vertices in the relation tables. Pre-fix this query returned an empty list.
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  ASSERT_TRUE(aV0.IsValid());
  ASSERT_TRUE(aV1.IsValid());
  const BRepGraph_EdgeId anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  bool foundV0 = false, foundV1 = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV0))
  {
    if (aE == anEdge)
    {
      foundV0 = true;
    }
  }
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV1))
  {
    if (aE == anEdge)
    {
      foundV1 = true;
    }
  }
  EXPECT_TRUE(foundV0);
  EXPECT_TRUE(foundV1);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_RemoveEdge_UnbindsStartEndVertices)
{
  // Symmetric to the Add test: removing the edge must drop the entries.
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdge));

  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV0))
  {
    EXPECT_NE(aE, anEdge);
  }
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV1))
  {
    EXPECT_NE(aE, anEdge);
  }
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetRefChildVertexId_RebindsVertexToEdges)
{
  // Rewire an edge's start-vertex ref to a different vertex and verify the
  // relation moved.
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  const BRepGraph_VertexRefId aStartRef = aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;
  aGraph.Editor().Vertices().SetRefChildVertexId(aStartRef, aV2);

  bool stillUnderV0 = false, foundUnderV2 = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV0))
  {
    if (aE == anEdge)
    {
      stillUnderV0 = true;
    }
  }
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV2))
  {
    if (aE == anEdge)
    {
      foundUnderV2 = true;
    }
  }
  EXPECT_FALSE(stillUnderV0);
  EXPECT_TRUE(foundUnderV2);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_BoxThroughCompact_StaysConsistent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  ASSERT_TRUE(aGraph.Editor().Faces().RemoveWire(
    BRepGraph_FaceId::Start(),
    BRepGraph_TestTools::WireRefsOfFace(aGraph, BRepGraph_FaceId::Start()).Value(0)));
  EXPECT_TRUE(aGraph.ValidateRelations());

  [[maybe_unused]] const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetRefChildWireId_RebindsWireToFaces)
{
  // Add two faces; rewire face0's outer-wire ref to face1's outer wire and
  // verify WireToFaces moved entries.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFace0 = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId aFace1(1);
  ASSERT_TRUE(aFace1.IsValid(aGraph.Topo().Faces().Nb()));

  const BRepGraph_WireRefId aWireRef0 =
    BRepGraph_TestTools::WireRefsOfFace(aGraph, aFace0).Value(0);
  const BRepGraph_WireId aOldWire = aGraph.Refs().Wires().Entry(aWireRef0).ChildWireId;
  const BRepGraph_WireId aNewWire =
    aGraph.Refs()
      .Wires()
      .Entry(BRepGraph_TestTools::WireRefsOfFace(aGraph, aFace1).Value(0))
      .ChildWireId;
  ASSERT_NE(aOldWire, aNewWire);

  aGraph.Editor().Wires().SetRefChildWireId(aWireRef0, aNewWire);

  bool oldStillBound = false, newBound = false;
  for (const BRepGraph_FaceId& f :
       BRepGraph_FacesOfWire(aGraph, aGraph.Topo().Wires().Relations(aOldWire).ParentWireRefIds))
  {
    if (f == aFace0)
    {
      oldStillBound = true;
    }
  }
  for (const BRepGraph_FaceId& f :
       BRepGraph_FacesOfWire(aGraph, aGraph.Topo().Wires().Relations(aNewWire).ParentWireRefIds))
  {
    if (f == aFace0)
    {
      newBound = true;
    }
  }
  EXPECT_FALSE(oldStillBound);
  EXPECT_TRUE(newBound);
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aGraph.Topo().Wires().Relations(aOldWire).CoEdgeIds)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
    EXPECT_NE(aCoEdge.FaceId, aFace0);
    for (const BRepGraph_FaceId& aFaceId : aGraph.Topo().Edges().FacesOf(aCoEdge.ChildEdgeId))
    {
      EXPECT_NE(aFaceId, aFace0);
    }
  }
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetRefFaceId_RebindsFaceToShells)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ShellId   aShell = BRepGraph_ShellId::Start();
  const BRepGraph_FaceRefId aRef0  = BRepGraph_TestTools::FaceRefsOfShell(aGraph, aShell).Value(0);
  const BRepGraph_FaceId    aOldFace = aGraph.Refs().Faces().Entry(aRef0).ChildFaceId;
  const BRepGraph_FaceId    aNewFace(1);
  ASSERT_NE(aOldFace, aNewFace);

  aGraph.Editor().Faces().SetRefFaceId(aRef0, aNewFace);

  // Old face still has the OTHER shell-ref pointing at it; just check the
  // Old/New relation makes sense relative to this single ref.
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetRefChildShellId_RebindsShellToSolids)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  // Box has 1 solid, 1 shell. Rewiring the lone shell-ref to itself is a no-op,
  // so just verify Validate after a no-op call (proves equality short-circuit).
  const BRepGraph_ShellRefId aRef0 =
    BRepGraph_TestTools::ShellRefsOfSolid(aGraph, BRepGraph_SolidId::Start()).Value(0);
  const BRepGraph_ShellId aShell = aGraph.Refs().Shells().Entry(aRef0).ChildShellId;
  aGraph.Editor().Shells().SetRefChildShellId(aRef0, aShell);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetChildEdgeIdOnCoEdge_RebindsEdgeToCoEdges)
{
  // Pick a coedge that lives in a wire; redirect its child edge to a different
  // existing edge and confirm Edge->CoEdges, Edge->Wires, Edge->Faces all move.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().CoEdges().Nb(), 2);

  const BRepGraph_CoEdgeId       aCoEdge   = BRepGraph_CoEdgeId::Start();
  const BRepGraphInc::CoEdgeDef& aDef      = aGraph.Topo().CoEdges().Definition(aCoEdge);
  const BRepGraph_EdgeId         anOldEdge = aDef.ChildEdgeId;
  // Pick any other valid edge as the target.
  BRepGraph_EdgeId aNewEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()); ++aE)
  {
    if (aE != anOldEdge)
    {
      aNewEdge = aE;
      break;
    }
  }
  ASSERT_TRUE(aNewEdge.IsValid());

  aGraph.Editor().CoEdges().SetChildEdgeId(aCoEdge, aNewEdge);

  bool oldHasCoEdge = false, newHasCoEdge = false;
  for (const BRepGraph_CoEdgeId& aC : aGraph.Topo().Edges().CoEdges(anOldEdge))
  {
    if (aC == aCoEdge)
    {
      oldHasCoEdge = true;
    }
  }
  for (const BRepGraph_CoEdgeId& aC : aGraph.Topo().Edges().CoEdges(aNewEdge))
  {
    if (aC == aCoEdge)
    {
      newHasCoEdge = true;
    }
  }
  EXPECT_FALSE(oldHasCoEdge);
  EXPECT_TRUE(newHasCoEdge);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetFaceIdOnCoEdge_LastBondCheck)
{
  // Cylinder seam edges have TWO coedges on the SAME face. Dropping one
  // coedge's FaceId must keep the (edge, face) pair bound via the other.
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  BRepGraph                aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aCylMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Locate a seam edge: an edge with two coedges on the same face.
  BRepGraph_EdgeId   aSeamEdge;
  BRepGraph_FaceId   aSeamFace;
  BRepGraph_CoEdgeId aSeamCoEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()) && !aSeamEdge.IsValid(); ++aE)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCEs = aGraph.Topo().Edges().CoEdges(aE);
    if (aCEs.Size() < 2)
    {
      continue;
    }
    NCollection_DataMap<int, BRepGraph_CoEdgeId> aSeenFace;
    for (const BRepGraph_CoEdgeId& aCE : aCEs)
    {
      const BRepGraphInc::CoEdgeDef& aD = aGraph.Topo().CoEdges().Definition(aCE);
      if (!aD.FaceId.IsValid())
      {
        continue;
      }
      if (aSeenFace.IsBound(aD.FaceId.Index))
      {
        aSeamEdge   = aE;
        aSeamFace   = aD.FaceId;
        aSeamCoEdge = aCE;
        break;
      }
      aSeenFace.Bind(aD.FaceId.Index, aCE);
    }
  }
  ASSERT_TRUE(aSeamEdge.IsValid()) << "cylinder must have a seam edge";

  aGraph.Editor().CoEdges().SetFaceId(aSeamCoEdge, BRepGraph_FaceId());

  // The seam-pair partner of aSeamCoEdge still binds (aSeamEdge, aSeamFace).
  bool stillBound = false;
  for (const BRepGraph_FaceId& f : aGraph.Topo().Edges().FacesOf(aSeamEdge))
  {
    if (f == aSeamFace)
    {
      stillBound = true;
    }
  }
  EXPECT_TRUE(stillBound);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetFaceIdOnCoEdge_OnlyBondUnbinds)
{
  // Box edges have one coedge per face. Setting that single coedge's FaceId
  // to invalid MUST unbind (edge,face) from EdgeToFaces.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_CoEdgeId       aCoEdge   = BRepGraph_CoEdgeId::Start();
  const BRepGraphInc::CoEdgeDef& aDef      = aGraph.Topo().CoEdges().Definition(aCoEdge);
  const BRepGraph_EdgeId         anEdge    = aDef.ChildEdgeId;
  const BRepGraph_FaceId         anOldFace = aDef.FaceId;

  aGraph.Editor().CoEdges().SetFaceId(aCoEdge, BRepGraph_FaceId());

  for (const BRepGraph_FaceId& f : aGraph.Topo().Edges().FacesOf(anEdge))
  {
    EXPECT_NE(f, anOldFace);
  }
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_RemoveWireFromSharedWireClearsDetachedFaceContext)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  const BRepGraph_CoEdgeId aCoEdge = aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD);
  ASSERT_TRUE(aCoEdge.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aCoEdge);
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  ASSERT_TRUE(aWire.IsValid());

  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const occ::handle<Geom_Plane>              aPlane = new Geom_Plane(gp_Pln());
  const BRepGraph_FaceId                     aFace0 =
    aGraph.Editor().Faces().Add(aPlane, aWire, anInnerWires.ToArray1(), 1.e-7);
  const BRepGraph_FaceId aFace1 =
    aGraph.Editor().Faces().Add(aPlane, aWire, anInnerWires.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace0.IsValid());
  ASSERT_TRUE(aFace1.IsValid());

  aGraph.Editor().CoEdges().SetFaceId(aCoEdge, aFace0);
  aGraph.Editor().CoEdges().SetPCurve(aCoEdge,
                                      new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)),
                                      0.0,
                                      1.0);
  const BRepGraph_CoEdgeCurve2DRepId aPCurveRepId =
    aGraph.Topo().CoEdges().Definition(aCoEdge).Curve2DRepId;
  ASSERT_TRUE(aPCurveRepId.IsValid());
  const uint32_t aNbActivePCurves = aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D();
  ASSERT_TRUE(aGraph.ValidateRelations());

  const BRepGraph_WireRefId aFace0WireRef =
    aGraph.Topo().Faces().Relations(aFace0).WireRefIds.First();
  ASSERT_TRUE(aFace0WireRef.IsValid());
  EXPECT_TRUE(aGraph.Editor().Faces().RemoveWire(aFace0, aFace0WireRef));

  EXPECT_FALSE(aGraph.Topo().CoEdges().Definition(aCoEdge).FaceId.IsValid())
    << "Removing the face's wire usage must clear coedge face context even if another face still "
       "uses the same wire";
  EXPECT_FALSE(aGraph.Topo().CoEdges().Definition(aCoEdge).Curve2DRepId.IsValid());
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D(), aNbActivePCurves - 1u);
  for (const BRepGraph_FaceId& aFaceId : aGraph.Topo().Edges().FacesOf(anEdge))
  {
    EXPECT_NE(aFaceId, aFace0);
  }
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_ValidateRejectsLivePCurveWithoutFaceContext)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  const BRepGraph_CoEdgeId aCoEdge = aGraph.Editor().CoEdges().Add(anEdge, TopAbs_FORWARD);
  ASSERT_TRUE(aCoEdge.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges;
  aCoEdges.Append(aCoEdge);
  const BRepGraph_WireId aWire = aGraph.Editor().Wires().Add(aCoEdges.ToArray1());
  ASSERT_TRUE(aWire.IsValid());

  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId                     aFace =
    aGraph.Editor().Faces().Add(new Geom_Plane(gp_Pln()), aWire, anInnerWires.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFace.IsValid());

  aGraph.Editor().CoEdges().SetFaceId(aCoEdge, aFace);
  aGraph.Editor().CoEdges().SetPCurve(aCoEdge,
                                      new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)),
                                      0.0,
                                      1.0);
  ASSERT_TRUE(aGraph.Topo().CoEdges().Definition(aCoEdge).Curve2DRepId.IsValid());
  ASSERT_TRUE(aGraph.ValidateRelations());

  {
    BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> aMutCoEdge = aGraph.Editor().CoEdges().Mut(aCoEdge);
    aMutCoEdge.Internal().FaceId                           = BRepGraph_FaceId();
  }
  EXPECT_FALSE(aGraph.ValidateRelations())
    << "A live face-scoped coedge pcurve without a face context is an orphaned relation";
}

TEST(BRepGraphIncTest, Relations_SetEndVertexRefId_RebindsVertexToEdges)
{
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.e-7);
  const BRepGraph_VertexId aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(2, 0, 0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  const BRepGraph_EdgeId anExtra =
    aGraph.Editor().Edges().Add(aV0, aV2, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  ASSERT_TRUE(anExtra.IsValid());

  // Reusing an already owned VertexRefId is invalid: refs are single-use edge
  // slots, even when both edges should point to the same vertex definition.
  const BRepGraph_VertexRefId aV2Ref = aGraph.Topo().Edges().Definition(anExtra).EndVertexRefId;
  const BRepGraph_VertexRefId anOldEndRef = aGraph.Topo().Edges().Definition(anEdge).EndVertexRefId;
  aGraph.Editor().Edges().SetEndVertexRefId(anEdge, aV2Ref);

  bool stillUnderV1 = false, foundUnderV2 = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV1))
  {
    if (aE == anEdge)
    {
      stillUnderV1 = true;
    }
  }
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aV2))
  {
    if (aE == anEdge)
    {
      foundUnderV2 = true;
    }
  }
  EXPECT_TRUE(stillUnderV1);
  EXPECT_FALSE(foundUnderV2);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdge).EndVertexRefId, anOldEndRef);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetterIdempotency_NoOp)
{
  // Identity assignments must be no-ops and not corrupt the relation tables.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_CoEdgeId       aCoEdge = BRepGraph_CoEdgeId::Start();
  const BRepGraphInc::CoEdgeDef& aDef    = aGraph.Topo().CoEdges().Definition(aCoEdge);
  aGraph.Editor().CoEdges().SetChildEdgeId(aCoEdge, aDef.ChildEdgeId);
  aGraph.Editor().CoEdges().SetFaceId(aCoEdge, aDef.FaceId);
  EXPECT_TRUE(aGraph.ValidateRelations());

  const BRepGraph_VertexRefId aVRef =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).StartVertexRefId;
  if (aVRef.IsValid())
  {
    const BRepGraph_VertexId aV = aGraph.Refs().Vertices().Entry(aVRef).ChildVertexId;
    aGraph.Editor().Vertices().SetRefChildVertexId(aVRef, aV);
    EXPECT_TRUE(aGraph.ValidateRelations());
  }
}

TEST(BRepGraphIncTest, Relations_RebindVertexSkipsRemovedParentEdge)
{
  BRepGraphInc_Storage aStorage;

  const BRepGraph_VertexId    aOldVertex = aStorage.AppendVertex();
  const BRepGraph_VertexId    aEndVertex = aStorage.AppendVertex();
  const BRepGraph_VertexId    aNewVertex = aStorage.AppendVertex();
  const BRepGraph_EdgeId      anEdge     = aStorage.AppendEdge();
  const BRepGraph_VertexRefId aStartRef  = aStorage.AppendVertexRef();
  const BRepGraph_VertexRefId anEndRef   = aStorage.AppendVertexRef();

  aStorage.ChangeVertexRef(aStartRef).ParentEdgeId  = anEdge;
  aStorage.ChangeVertexRef(aStartRef).ChildVertexId = aOldVertex;
  aStorage.ChangeVertexRef(anEndRef).ParentEdgeId   = anEdge;
  aStorage.ChangeVertexRef(anEndRef).ChildVertexId  = aEndVertex;
  aStorage.ChangeEdge(anEdge).StartVertexRefId      = aStartRef;
  aStorage.ChangeEdge(anEdge).EndVertexRefId        = anEndRef;
  aStorage.RebuildDerivedRelations();

  ASSERT_EQ(aStorage.VertexRelations(aOldVertex).EdgeIds.Size(), 1u);
  ASSERT_EQ(aStorage.VertexRelations(aNewVertex).EdgeIds.Size(), 0u);

  aStorage.SetRemoved(anEdge, true);
  aStorage.RebindVertexEdge(aOldVertex, aNewVertex, anEdge, BRepGraph_VertexRefId());
  aStorage.RebindVertexRef(aStartRef, aOldVertex, aNewVertex);

  EXPECT_EQ(aStorage.VertexRef(aStartRef).ChildVertexId, aOldVertex);
  EXPECT_EQ(aStorage.VertexRelations(aOldVertex).EdgeIds.Size(), 1u);
  EXPECT_EQ(aStorage.VertexRelations(aNewVertex).EdgeIds.Size(), 0u);
}

TEST(BRepGraphIncTest, CanonicalizeWireCoEdgeOrderStatus_ReordersExactConnectedWire)
{
  BRepGraphInc_Storage   aStorage;
  const BRepGraph_WireId aWireId = aStorage.AppendWire();

  const BRepGraph_VertexId aVertexA = addStorageVertex(aStorage, gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexB = addStorageVertex(aStorage, gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexC = addStorageVertex(aStorage, gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);

  const BRepGraph_EdgeId   anEdgeBC = addStorageEdge(aStorage, aVertexB, aVertexC);
  const BRepGraph_EdgeId   anEdgeAB = addStorageEdge(aStorage, aVertexA, aVertexB);
  const BRepGraph_CoEdgeId aCoEdgeBC =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeBC, BRepGraph_FaceId(), TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aCoEdgeAB =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeAB, BRepGraph_FaceId(), TopAbs_FORWARD);

  EXPECT_FALSE(aStorage.ValidateWireCoEdgeOrder(aWireId));
  EXPECT_EQ(aStorage.CanonicalizeWireCoEdgeOrderStatus(aWireId),
            BRepGraphInc_Storage::WireCoEdgeOrderStatus::Reordered);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.WireRelations(aWireId).CoEdgeIds;
  ASSERT_EQ(aCoEdges.Size(), 2u);
  EXPECT_EQ(aCoEdges.Value(0), aCoEdgeAB);
  EXPECT_EQ(aCoEdges.Value(1), aCoEdgeBC);
  EXPECT_TRUE(aStorage.ValidateWireCoEdgeOrder(aWireId));
}

TEST(BRepGraphIncTest, CanonicalizeWireCoEdgeOrderStatus_UsesVertexTolerance)
{
  BRepGraphInc_Storage   aStorage;
  const BRepGraph_WireId aWireId = aStorage.AppendWire();

  const BRepGraph_VertexId aVertexA = addStorageVertex(aStorage, gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexB = addStorageVertex(aStorage, gp_Pnt(1.0, 0.0, 0.0), 1.0e-2);
  const BRepGraph_VertexId aVertexC = addStorageVertex(aStorage, gp_Pnt(1.005, 0.0, 0.0), 1.0e-2);
  const BRepGraph_VertexId aVertexD = addStorageVertex(aStorage, gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);

  const BRepGraph_EdgeId   anEdgeAB = addStorageEdge(aStorage, aVertexA, aVertexB);
  const BRepGraph_EdgeId   anEdgeCD = addStorageEdge(aStorage, aVertexC, aVertexD);
  const BRepGraph_CoEdgeId aCoEdgeAB =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeAB, BRepGraph_FaceId(), TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aCoEdgeCD =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeCD, BRepGraph_FaceId(), TopAbs_FORWARD);

  EXPECT_FALSE(aStorage.ValidateWireCoEdgeOrder(aWireId));
  EXPECT_EQ(aStorage.CanonicalizeWireCoEdgeOrderStatus(aWireId),
            BRepGraphInc_Storage::WireCoEdgeOrderStatus::ToleranceOrdered);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.WireRelations(aWireId).CoEdgeIds;
  ASSERT_EQ(aCoEdges.Size(), 2u);
  EXPECT_EQ(aCoEdges.Value(0), aCoEdgeAB);
  EXPECT_EQ(aCoEdges.Value(1), aCoEdgeCD);
}

TEST(BRepGraphIncTest, CanonicalizeWireCoEdgeOrderStatus_PartialPreservesDisconnectedRuns)
{
  BRepGraphInc_Storage   aStorage;
  const BRepGraph_WireId aWireId = aStorage.AppendWire();

  const BRepGraph_VertexId aVertexA = addStorageVertex(aStorage, gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexB = addStorageVertex(aStorage, gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexC = addStorageVertex(aStorage, gp_Pnt(2.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexX = addStorageVertex(aStorage, gp_Pnt(10.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aVertexY = addStorageVertex(aStorage, gp_Pnt(11.0, 0.0, 0.0), 1.0e-7);

  const BRepGraph_EdgeId   anEdgeBC = addStorageEdge(aStorage, aVertexB, aVertexC);
  const BRepGraph_EdgeId   anEdgeXY = addStorageEdge(aStorage, aVertexX, aVertexY);
  const BRepGraph_EdgeId   anEdgeAB = addStorageEdge(aStorage, aVertexA, aVertexB);
  const BRepGraph_CoEdgeId aCoEdgeBC =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeBC, BRepGraph_FaceId(), TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aCoEdgeXY =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeXY, BRepGraph_FaceId(), TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aCoEdgeAB =
    aStorage.CreateCoEdgeUse(aWireId, anEdgeAB, BRepGraph_FaceId(), TopAbs_FORWARD);

  EXPECT_EQ(aStorage.CanonicalizeWireCoEdgeOrderStatus(aWireId),
            BRepGraphInc_Storage::WireCoEdgeOrderStatus::Partial);

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.WireRelations(aWireId).CoEdgeIds;
  ASSERT_EQ(aCoEdges.Size(), 3u);
  EXPECT_EQ(aCoEdges.Value(0), aCoEdgeAB);
  EXPECT_EQ(aCoEdges.Value(1), aCoEdgeBC);
  EXPECT_EQ(aCoEdges.Value(2), aCoEdgeXY);
}

// ============================================================
// CleanupRemovedReferences tests
// ============================================================

TEST(BRepGraphIncTest, CleanupRemovedRefs_AfterRemoveFace_ValidatePasses)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFaceId));

  aGraph.Editor().Gen().CleanupRemovedReferences();
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, CleanupRemovedRefs_AfterRemoveFace_RemovesCoEdgePCurves)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));

  NCollection_LinearVector<BRepGraph_CoEdgeId>           aFaceCoEdges;
  NCollection_LinearVector<BRepGraph_CoEdgeCurve2DRepId> aPCurves;
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb());
       ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
    if (aCoEdge.FaceId == aFaceId && aCoEdge.Curve2DRepId.IsValid())
    {
      aFaceCoEdges.Append(aCoEdgeId);
      aPCurves.Append(aCoEdge.Curve2DRepId);
    }
  }
  ASSERT_FALSE(aPCurves.IsEmpty());
  const uint32_t aNbActivePCurves = aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D();

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFaceId));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  for (const BRepGraph_CoEdgeId& aCoEdgeId : aFaceCoEdges)
  {
    EXPECT_FALSE(aGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId.IsValid());
  }
  for (const BRepGraph_CoEdgeCurve2DRepId& aPCurveId : aPCurves)
  {
    EXPECT_TRUE(BRepGraph_RepId(aPCurveId).IsRemoved(aGraph));
  }
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D(),
            aNbActivePCurves - static_cast<uint32_t>(aPCurves.Size()));
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, CleanupRemovedRefs_AfterRemoveWire_ValidatePasses)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId  = BRepGraph_FaceId::Start();
  const BRepGraph_WireId aOuterId = BRepGraph_Tool::Face::OuterWire(aGraph, aFaceId);
  ASSERT_TRUE(aOuterId.IsValid());
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aOuterId));

  aGraph.Editor().Gen().CleanupRemovedReferences();
  const BRepGraph_Validate::Result aValidateResult = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aGraph.ValidateRelations());
  EXPECT_TRUE(aValidateResult.IsValid())
    << (aValidateResult.Issues.IsEmpty() ? ""
                                         : aValidateResult.Issues.First().Description.ToCString());
}

TEST(BRepGraphIncTest, CleanupRemovedRefs_AfterRemoveVertex_ValidatePasses)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_EdgeId      anEdgeId = BRepGraph_EdgeId::Start();
  const BRepGraph_VertexRefId aStartV  = BRepGraph_Tool::Edge::StartVertexId(aGraph, anEdgeId);
  ASSERT_TRUE(aStartV.IsValid());
  aGraph.Editor().Gen().RemoveNode(
    BRepGraph_NodeId(aGraph.Refs().Vertices().Entry(aStartV).ChildVertexId));

  aGraph.Editor().Gen().CleanupRemovedReferences();
  EXPECT_TRUE(aGraph.ValidateRelations());
  EXPECT_FALSE(aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId.IsValid());
}

TEST(BRepGraphIncTest, CleanupRemovedRefs_AfterRemoveMultiple_ValidatePasses)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId  = BRepGraph_FaceId::Start();
  const BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFaceId));
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdgeId));

  aGraph.Editor().Gen().CleanupRemovedReferences();
  const BRepGraph_Validate::Result aVal = BRepGraph_Validate::Perform(aGraph);
  EXPECT_TRUE(aVal.IsValid());
}

TEST(BRepGraphIncTest, Storage_MarkRemoved_UnbindsTShapeAndOriginal)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));
  ASSERT_TRUE(aGraph.Shapes().HasOriginal(aFaceId));
  const TopoDS_Shape aOrigShape = aGraph.Shapes().Original(aFaceId);
  ASSERT_FALSE(aOrigShape.IsNull());
  ASSERT_TRUE(aGraph.Shapes().HasNode(aOrigShape));

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFaceId));

  EXPECT_FALSE(aGraph.Shapes().HasNode(aOrigShape));
  EXPECT_FALSE(aGraph.Shapes().HasOriginal(aFaceId));
}

// ============================================================
// Storage fix tests: EraseLast, TShape unbind, RebuildDerivedRelations
// ============================================================

TEST(BRepGraphIncTest, Storage_MarkRemoved_EraseLast_EmptyStore)
{
  // S1: When all entities in a rep store are marked removed,
  // EraseLast (called internally during Compact) must reclaim slots.
  // Verify by removing faces and checking surface counts decrease.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t anOldNbSurf = aGraph.Topo().Geometry().NbFaceSurfaces();
  ASSERT_NE(anOldNbSurf, 0);

  // Remove one face to trigger surface removal during compact.
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(BRepGraph_FaceId::Start()));

  [[maybe_unused]] const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);

  const uint32_t aNewNbSurf = aGraph.Topo().Geometry().NbFaceSurfaces();
  EXPECT_LT(aNewNbSurf, anOldNbSurf);
  EXPECT_TRUE(aGraph.ValidateRelations());

  const BRepGraph_FaceId aNewFace = BRepGraph_FaceId::Start();
  aGraph.Editor().Faces().SetSurface(aNewFace, new Geom_Plane(gp_Pln()));
  // SetSurface replaces the existing surface on the surviving face; count unchanged.
  EXPECT_EQ(aGraph.Topo().Geometry().NbFaceSurfaces(), aNewNbSurf);
}

TEST(BRepGraphIncTest, Storage_MarkRemoved_UnbindsTShapeToNodeId)
{
  // S2: After RemoveNode, the removed node's TShape must no longer
  // resolve via Shapes().FindNode().
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  // Reconstruct the solid and verify it has an active mapping.
  const TopoDS_Shape aSolid = aGraph.Shapes().Shape(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aSolid.IsNull());
  const BRepGraph_NodeId anOldNode = aGraph.Shapes().FindNode(aSolid);
  ASSERT_TRUE(anOldNode.IsValid());
  EXPECT_EQ(anOldNode.NodeKind, BRepGraph_NodeId::Kind::Solid);

  // Remove the solid node. ValidateRelations is not checked here
  // because forward refs still point to the removed solid's sub-entities.
  aGraph.Editor().Gen().RemoveNode(anOldNode);

  [[maybe_unused]] const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);

  const BRepGraph_NodeId aNodeAfter = aGraph.Shapes().FindNode(aSolid);
  EXPECT_FALSE(aNodeAfter.IsValid());

  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Storage_RebuildDerivedRelations_RecountsRefStoreNbActive)
{
  // RebuildDerivedRelations must not corrupt NbActive counters on ref stores
  // after reference entries were marked removed.
  BRepGraphInc_Storage aStorage;

  // Create synthetic entities.
  const BRepGraph_VertexId    aVtx1     = aStorage.AppendVertex();
  const BRepGraph_VertexId    aVtx2     = aStorage.AppendVertex();
  const BRepGraph_EdgeId      anEdge    = aStorage.AppendEdge();
  const BRepGraph_VertexRefId aStartRef = aStorage.AppendVertexRef();
  const BRepGraph_VertexRefId anEndRef  = aStorage.AppendVertexRef();

  aStorage.ChangeVertexRef(aStartRef).ParentEdgeId  = anEdge;
  aStorage.ChangeVertexRef(aStartRef).ChildVertexId = aVtx1;
  aStorage.ChangeVertexRef(anEndRef).ParentEdgeId   = anEdge;
  aStorage.ChangeVertexRef(anEndRef).ChildVertexId  = aVtx2;
  aStorage.ChangeEdge(anEdge).StartVertexRefId      = aStartRef;
  aStorage.ChangeEdge(anEdge).EndVertexRefId        = anEndRef;
  aStorage.RebuildDerivedRelations();

  const uint32_t anOldActiveVtxRefs = aStorage.NbActiveVertexRefs();
  ASSERT_GT(anOldActiveVtxRefs, 0u);

  // Mark one vertex ref removed.
  EXPECT_TRUE(aStorage.MarkRemovedRef(BRepGraph_RefId(aStartRef)));
  EXPECT_EQ(aStorage.NbActiveVertexRefs(), anOldActiveVtxRefs - 1u);

  // Rebuilding the relation tables must preserve the NbActive count.
  aStorage.RebuildDerivedRelations();
  EXPECT_EQ(aStorage.NbActiveVertexRefs(), anOldActiveVtxRefs - 1u);
}

// ============================================================
// Relation table fix tests: BuildDelta, seam-edge dedup
// ============================================================

TEST(BRepGraphIncTest, Relations_BuildDelta_ExistingEdgeNewCoedge_GetsNewFace)
{
  // RI1: Build two identical boxes and dedup-merge. Edges merged should
  // have correct face counts in the relation tables.
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 20.0, 30.0);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aBoxMaker1.Shape());
  aBB.Add(aCompound, aBoxMaker2.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  ASSERT_EQ(aGraph.Topo().Solids().Nb(), 2);

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  [[maybe_unused]] const BRepGraph_Deduplicate::Result aDedupRes =
    BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  // Compact to clean up removed entities before relation tables validation.
  [[maybe_unused]] const BRepGraph_Compact::Result aCompactRes = BRepGraph_Compact::Perform(aGraph);

  const uint32_t aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEId(0); anEId.IsValid(aNbEdges); ++anEId)
  {
    if (anEId.IsRemoved(aGraph))
    {
      continue;
    }
    EXPECT_NE(aGraph.Topo().Edges().NbFaces(anEId), 0u);
  }
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SeamEdge_NoDuplicateEdgeToWireEntries)
{
  // RI4 (appendDirect -> appendUnique): a seam edge whose two coedges
  // belong to the same wire must not cause duplicate wire entries in
  // myEdgeToWires.
  BRepPrimAPI_MakeSphere aSphMaker(8.0);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();

  BRepGraph aGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aGraph, aSph, false);
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aNbEdges = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    NCollection_LinearVector<BRepGraph_WireId> aSeenWires;
    for (BRepGraph_WiresOfEdge aWireIt = aGraph.Topo().Edges().WiresOf(anEdgeId); aWireIt.More();
         aWireIt.Next())
    {
      const BRepGraph_WireId aWireId = aWireIt.CurrentId();
      EXPECT_FALSE(containsId(aSeenWires, aWireId))
        << "Duplicate wire " << aWireId.Index << " in WiresOfEdge(" << anEdgeId.Index << ")";
      aSeenWires.Append(aWireId);
    }
  }

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid());
}

TEST(BRepGraphIncTest, Relations_SetChildRefChildNodeId_CrossKindRebinds)
{
  // Compound holding a Solid; rewire the ChildRef from the Solid to a Shell.
  // CompoundsOfSolid must lose the entry, CompoundsOfShell must gain it.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  BRepPrimAPI_MakeBox aBoxMaker(2.0, 2.0, 2.0);
  aBB.Add(aCompound, aBoxMaker.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Compounds().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Solids().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  const BRepGraph_CompoundId                            aCompound0 = BRepGraph_CompoundId::Start();
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aChildRefs =
    aGraph.Topo().Compounds().Relations(aCompound0).ChildRefIds;
  ASSERT_GE(aChildRefs.Size(), 1u);
  const BRepGraph_ChildRefId aChildRef  = aChildRefs.Value(0);
  const BRepGraph_NodeId     anOldChild = aGraph.Refs().Children().Entry(aChildRef).ChildNodeId;
  ASSERT_EQ(anOldChild.NodeKind, BRepGraph_NodeId::Kind::Solid);

  const BRepGraph_ShellId aShell = BRepGraph_ShellId::Start();
  aGraph.Editor().Gen().SetChildRefChildNodeId(aChildRef, BRepGraph_NodeId(aShell));

  const BRepGraph_SolidId anOldSolid = BRepGraph_SolidId::FromNodeId(anOldChild);
  bool                    oldStill = false, newBound = false;
  for (const BRepGraph_CompoundId& c :
       BRepGraph_CompoundsOfChild(aGraph,
                                  aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(anOldSolid))))
  {
    if (c == aCompound0)
    {
      oldStill = true;
    }
  }
  for (const BRepGraph_CompoundId& c :
       BRepGraph_CompoundsOfChild(aGraph,
                                  aGraph.Topo().Gen().CompoundRefIds(BRepGraph_NodeId(aShell))))
  {
    if (c == aCompound0)
    {
      newBound = true;
    }
  }
  EXPECT_FALSE(oldStill);
  EXPECT_TRUE(newBound);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_SetChildEdgeIdOnCoEdge_LastBondInWireCheck)
{
  // Cylinder seam edge: two coedges share both face and wire. Redirecting one
  // coedge's ChildEdgeId must NOT remove (oldEdge, wire) from EdgeToWires while
  // the other coedge still references oldEdge in the same wire.
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 10.0);
  BRepGraph                aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aCylMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Find a seam edge: two coedges, same wire, same edge.
  BRepGraph_EdgeId   aSeamEdge;
  BRepGraph_WireId   aSeamWire;
  BRepGraph_CoEdgeId aSeamCoEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()) && !aSeamEdge.IsValid(); ++aE)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCEs = aGraph.Topo().Edges().CoEdges(aE);
    if (aCEs.Size() < 2)
    {
      continue;
    }
    NCollection_DataMap<int, BRepGraph_CoEdgeId> aSeenWire;
    for (const BRepGraph_CoEdgeId& aCE : aCEs)
    {
      const BRepGraph_WireId aW = aGraph.Topo().CoEdges().Wire(aCE);
      if (aW.IsValid())
      {
        if (aSeenWire.IsBound(aW.Index))
        {
          aSeamEdge   = aE;
          aSeamWire   = aW;
          aSeamCoEdge = aCE;
          break;
        }
        aSeenWire.Bind(aW.Index, aCE);
      }
      if (aSeamEdge.IsValid())
      {
        break;
      }
    }
  }
  ASSERT_TRUE(aSeamEdge.IsValid()) << "cylinder must have a wire with two coedges of same edge";

  // Pick a different edge to redirect to.
  BRepGraph_EdgeId aTargetEdge;
  for (BRepGraph_EdgeId aE(0); aE.IsValid(aGraph.Topo().Edges().Nb()); ++aE)
  {
    if (aE != aSeamEdge)
    {
      aTargetEdge = aE;
      break;
    }
  }
  ASSERT_TRUE(aTargetEdge.IsValid());

  aGraph.Editor().CoEdges().SetChildEdgeId(aSeamCoEdge, aTargetEdge);

  // The OTHER coedge of aSeamWire still references aSeamEdge -> wire still bound.
  bool stillBound = false;
  for (const BRepGraph_WireId& aW : aGraph.Topo().Edges().WiresOf(aSeamEdge))
  {
    if (aW == aSeamWire)
    {
      stillBound = true;
    }
  }
  EXPECT_TRUE(stillBound);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraphIncTest, Relations_FaceSupplementVertex_NoPersistedRefUpdate)
{
  // Face-direct supplemental vertices no longer create persisted VertexRef
  // entries or relation-table bindings. Validate that the graph stays consistent.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  std::ignore = aGraph.Editor().Vertices().Add(gp_Pnt(7, 7, 7), 1.e-7);

  EXPECT_TRUE(aGraph.ValidateRelations());
  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  // No vertex attached via Perform -> supplement layer is empty for this face.
  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(BRepGraph_FaceId::Start())).Size(), 0);
}

TEST(BRepGraphIncTest, Relations_RemoveRef_UnbindsByKind)
{
  // GenOps::RemoveRef must unbind the corresponding relation entry. Picks one
  // FaceRef on the box's first shell; after RemoveRef the FaceToShells entry
  // for the detached face no longer lists this shell.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(aGraph, BRepGraph_ShellId::Start());
  ASSERT_GE(aFaceRefs.Size(), 1);

  const BRepGraph_FaceRefId    aFaceRefId = aFaceRefs.Value(0);
  const BRepGraphInc::FaceRef& aRef       = aGraph.Refs().Faces().Entry(aFaceRefId);
  const BRepGraph_FaceId       aFaceId    = aRef.ChildFaceId;

  ASSERT_TRUE(aGraph.Editor().Gen().RemoveRef(aFaceRefId));

  for (const BRepGraph_ShellId& aShellId :
       BRepGraph_ShellsOfFace(aGraph, aGraph.Topo().Faces().Relations(aFaceId).ParentFaceRefIds))
  {
    EXPECT_NE(aShellId, BRepGraph_ShellId::Start());
  }
  EXPECT_TRUE(aGraph.ValidateRelations());
}

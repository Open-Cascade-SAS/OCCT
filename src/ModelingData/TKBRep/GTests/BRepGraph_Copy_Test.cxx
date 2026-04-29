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

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <cmath>
#include <iostream>

#include <gtest/gtest.h>

namespace
{

class CopyTestCacheValue : public BRepGraph_CacheValue
{
public:
  DEFINE_STANDARD_RTTI_INLINE(CopyTestCacheValue, BRepGraph_CacheValue)
  CopyTestCacheValue() = default;
};

const occ::handle<BRepGraph_CacheKind>& copyTestCacheKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10026"), "CopyTestAttr");
  return THE_KIND;
}

} // namespace

TEST(BRepGraph_CopyTest, CopyBox_FaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  // Verify reconstructed shape has correct face count.
  TopoDS_Shape aCopy = aCopyGraph.Shapes().Reconstruct(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aCopy.IsNull());

  int aNbFaces = 0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(aNbFaces, 6);
}

TEST(BRepGraph_CopyTest, CopyBox_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  TopoDS_Shape aCopy = aCopyGraph.Shapes().Reconstruct(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aCopy.IsNull());

  // Sum face areas since result may be a compound.
  double aCopyArea = 0.0;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(anExp.Current(), aProps);
    aCopyArea += std::abs(aProps.Mass());
  }

  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_CopyTest, CopyBox_GeometryIsIndependent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // Deep copy: surface handles must be different objects.
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  ASSERT_GT(aCopyGraph.Topo().Faces().Nb(), 0);
  EXPECT_NE(BRepGraph_Tool::Face::Surface(aCopyGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get());
}

TEST(BRepGraph_CopyTest, CopyBox_SharedGeometry)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // theCopyGeom = false: geometry is shared.
  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, false);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 6);

  // Light copy: surface handles must be the same objects.
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  ASSERT_GT(aCopyGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(BRepGraph_Tool::Face::Surface(aCopyGraph, BRepGraph_FaceId::Start()).get(),
            BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get());
}

TEST(BRepGraph_CopyTest, CopyBox_PreservesFreshNodeCache)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId                  aFaceId(0);
  const occ::handle<BRepGraph_CacheValue> anAttr = new CopyTestCacheValue();
  aGraph.Cache().Set(aFaceId, copyTestCacheKind(), anAttr);
  ASSERT_TRUE(aGraph.Cache().Has(aFaceId, copyTestCacheKind()));

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  EXPECT_TRUE(aCopyGraph.Cache().Has(aFaceId, copyTestCacheKind()));
  EXPECT_FALSE(aCopyGraph.Cache().Get(aFaceId, copyTestCacheKind()).IsNull());
  EXPECT_TRUE(aCopyGraph.Cache().CacheKindIter(aFaceId).More());
}

TEST(BRepGraph_CopyTest, CopyBox_DoesNotPreserveStaleNodeCache)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId(0);
  aGraph.Cache().Set(aFaceId, copyTestCacheKind(), new CopyTestCacheValue());
  ASSERT_TRUE(aGraph.Cache().Has(aFaceId, copyTestCacheKind()));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace = aGraph.Editor().Faces().Mut(aFaceId);
    aGraph.Editor().Faces().SetTolerance(aFace, aFace->Tolerance + 0.1);
  }

  ASSERT_FALSE(aGraph.Cache().Has(aFaceId, copyTestCacheKind()));
  ASSERT_TRUE(aGraph.Cache().Get(aFaceId, copyTestCacheKind()).IsNull());
  ASSERT_FALSE(aGraph.Cache().CacheKindIter(aFaceId).More());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  EXPECT_FALSE(aCopyGraph.Cache().Has(aFaceId, copyTestCacheKind()));
  EXPECT_TRUE(aCopyGraph.Cache().Get(aFaceId, copyTestCacheKind()).IsNull());
  EXPECT_FALSE(aCopyGraph.Cache().CacheKindIter(aFaceId).More());
}

TEST(BRepGraph_CopyTest, CopyBox_PreservesFreshFaceRefCache)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Refs().Faces().Nb(), 0);

  const BRepGraph_FaceRefId aFaceRef(0);
  aGraph.Cache().Set(aFaceRef, copyTestCacheKind(), new CopyTestCacheValue());
  ASSERT_TRUE(aGraph.Cache().Has(aFaceRef, copyTestCacheKind()));

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  EXPECT_TRUE(aCopyGraph.Cache().Has(aFaceRef, copyTestCacheKind()));
  EXPECT_FALSE(aCopyGraph.Cache().Get(aFaceRef, copyTestCacheKind()).IsNull());
  EXPECT_TRUE(aCopyGraph.Cache().CacheKindIter(aFaceRef).More());
}

TEST(BRepGraph_CopyTest, CopyBox_DoesNotPreserveStaleFaceRefCache)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Refs().Faces().Nb(), 0);

  const BRepGraph_FaceRefId aFaceRef(0);
  aGraph.Cache().Set(aFaceRef, copyTestCacheKind(), new CopyTestCacheValue());
  ASSERT_TRUE(aGraph.Cache().Has(aFaceRef, copyTestCacheKind()));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aRef = aGraph.Editor().Faces().MutRef(aFaceRef);
    aGraph.Editor().Faces().SetRefOrientation(aRef, TopAbs::Reverse(aRef->Orientation));
  }

  ASSERT_FALSE(aGraph.Cache().Has(aFaceRef, copyTestCacheKind()));
  ASSERT_TRUE(aGraph.Cache().Get(aFaceRef, copyTestCacheKind()).IsNull());
  ASSERT_FALSE(aGraph.Cache().CacheKindIter(aFaceRef).More());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  EXPECT_FALSE(aCopyGraph.Cache().Has(aFaceRef, copyTestCacheKind()));
  EXPECT_TRUE(aCopyGraph.Cache().Get(aFaceRef, copyTestCacheKind()).IsNull());
  EXPECT_FALSE(aCopyGraph.Cache().CacheKindIter(aFaceRef).More());
}

TEST(BRepGraph_CopyTest, CopyCylinder_FaceCount)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 20.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 =
    BRepGraph_Builder::Add(aGraph, aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_CopyTest, CopySingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, aFaceId.Index);
  BRepGraph              aCopyGraph = BRepGraph_Copy::CopyNode(aGraph, aFaceNode, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 1);

  // A box face has 1 wire with 4 edges and 4 vertices.
  EXPECT_GT(aCopyGraph.Topo().Vertices().Nb(), 0);
  EXPECT_GT(aCopyGraph.Topo().Edges().Nb(), 0);
  EXPECT_GT(aCopyGraph.Topo().Wires().Nb(), 0);

  // No shells/solids in a single-face sub-graph.
  EXPECT_EQ(aCopyGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aCopyGraph.Topo().Solids().Nb(), 0);

  TopoDS_Shape aCopiedFace = aCopyGraph.Shapes().Reconstruct(BRepGraph_FaceId::Start());
  ASSERT_FALSE(aCopiedFace.IsNull());
  EXPECT_EQ(aCopiedFace.ShapeType(), TopAbs_FACE);

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aCopiedFace, aProps);
  EXPECT_GT(std::abs(aProps.Mass()), 1.0);
}

TEST(BRepGraph_CopyTest, CopyFacesOnly_Compound)
{
  // Build graph from individual faces (no shells/solids).
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
    aBB.Add(aCompound, aCopier.Shape());
  }

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);
  ASSERT_EQ(aGraph.Topo().Solids().Nb(), 0);
  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 0);

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aCopyGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aCopyGraph.Topo().Shells().Nb(), 0);
}

// ============================================================
// SameParameter / SameRange round-trip
// ============================================================

TEST(BRepGraph_CopyTest, CopyBox_SameParameter_Preserved)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // All edges in the copied graph must preserve SameParameter = true.
  const int aNbEdges = aCopyGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = aCopyGraph.Topo().Edges().Definition(anEdgeId);
    EXPECT_TRUE(anEdge.SameParameter)
      << "Copied edge " << anEdgeId.Index << " lost SameParameter flag";
    EXPECT_TRUE(anEdge.SameRange) << "Copied edge " << anEdgeId.Index << " lost SameRange flag";
  }
}

// ============================================================
// Regularity (BRep_CurveOn2Surfaces) preserved via EncodeRegularity
// ============================================================

TEST(BRepGraph_CopyTest, FusedBoxes_Regularity_AreaPreserved)
{
  // Fuse two adjacent boxes to produce a shape with regularity edges.
  // Verify that the copy preserves area (geometry integrity).
  TopoDS_Shape     aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape     aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse aFuser(aBox1, aBox2);
  ASSERT_TRUE(aFuser.IsDone());
  const TopoDS_Shape& aFused = aFuser.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aFused, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 =
    BRepGraph_Builder::Add(aGraph, aFused);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // Verify node counts match.
  EXPECT_EQ(aCopyGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aCopyGraph.Topo().Edges().Nb(), aGraph.Topo().Edges().Nb());

  // Verify area is preserved by summing individual face areas.
  double    aCopyArea = 0.0;
  const int aNbFaces  = aCopyGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    TopoDS_Shape aFace = aCopyGraph.Shapes().Reconstruct(aFaceId);
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    aCopyArea += std::abs(aProps.Mass());
  }
  EXPECT_NEAR(aCopyArea, anOrigArea, anOrigArea * 0.01);
}

// ============================================================
// UID preservation
// ============================================================

TEST(BRepGraph_CopyTest, CopyBox_UIDsPreserved)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph aCopyGraph = BRepGraph_Copy::Perform(aGraph, true);
  ASSERT_TRUE(aCopyGraph.IsDone());

  // Helper to check UIDs for a given node kind.
  auto checkUIDs = [&](BRepGraph_NodeId::Kind theKind, int theCount, const char* theLabel) {
    for (int anIdx = 0; anIdx < theCount; ++anIdx)
    {
      BRepGraph_NodeId aNodeId(theKind, anIdx);
      BRepGraph_UID    anOrigUID = aGraph.UIDs().Of(aNodeId);
      BRepGraph_UID    aCopyUID  = aCopyGraph.UIDs().Of(aNodeId);
      EXPECT_EQ(aCopyUID.IsValid(), anOrigUID.IsValid())
        << "UID validity mismatch at " << theLabel << " " << anIdx;
      if (anOrigUID.IsValid())
      {
        EXPECT_EQ(aCopyUID, anOrigUID) << "UID mismatch at " << theLabel << " " << anIdx;
      }
    }
  };

  // Verify UIDs for all topology and geometry node types.
  checkUIDs(BRepGraph_NodeId::Kind::Vertex, aGraph.Topo().Vertices().Nb(), "vertex");
  checkUIDs(BRepGraph_NodeId::Kind::Edge, aGraph.Topo().Edges().Nb(), "edge");
  checkUIDs(BRepGraph_NodeId::Kind::Wire, aGraph.Topo().Wires().Nb(), "wire");
  checkUIDs(BRepGraph_NodeId::Kind::Face, aGraph.Topo().Faces().Nb(), "face");
  checkUIDs(BRepGraph_NodeId::Kind::Shell, aGraph.Topo().Shells().Nb(), "shell");
  checkUIDs(BRepGraph_NodeId::Kind::Solid, aGraph.Topo().Solids().Nb(), "solid");
  // Geometry is now stored inline on face/edge defs; no separate geometry UIDs to check.
}

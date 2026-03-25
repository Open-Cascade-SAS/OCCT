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
#include <BRepGraph.hxx>
#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_AnalyzeView.hxx>
#include <BRepGraph_AttrsView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphAlgo_BndLib.hxx>

#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
//! Concrete subclass for testing user attributes.
class TestUserAttribute : public BRepGraph_UserAttribute
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TestUserAttribute, BRepGraph_UserAttribute)
  TestUserAttribute() = default;
};

//! Compute the center of a bounding box for the given node.
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

class BRepGraphViewsTest : public testing::Test
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

// ---------- DefsView ----------

TEST_F(BRepGraphViewsTest, DefsView_NbFaces)
{
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
}

TEST_F(BRepGraphViewsTest, DefsView_NbSolids)
{
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
}

TEST_F(BRepGraphViewsTest, DefsView_NbShells)
{
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
}

TEST_F(BRepGraphViewsTest, DefsView_NbWires)
{
  EXPECT_EQ(myGraph.Defs().NbWires(), 6);
}

TEST_F(BRepGraphViewsTest, DefsView_NbEdges)
{
  EXPECT_EQ(myGraph.Defs().NbEdges(), 12);
}

TEST_F(BRepGraphViewsTest, DefsView_NbVertices)
{
  EXPECT_EQ(myGraph.Defs().NbVertices(), 8);
}

TEST_F(BRepGraphViewsTest, DefsView_FaceAccessor_Valid)
{
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = myGraph.Defs().Face(BRepGraph_FaceId(anIdx));
    EXPECT_TRUE(aFace.Id.IsValid()) << "Face " << anIdx << " has invalid Id";
  }
}

TEST_F(BRepGraphViewsTest, DefsView_TopoDef_Valid)
{
  BRepGraph_NodeId                   aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_TopoNode::BaseDef* aBase = myGraph.Defs().TopoDef(aFaceId);
  ASSERT_NE(aBase, nullptr);
  EXPECT_EQ(aBase->Id, myGraph.Defs().Face(BRepGraph_FaceId(0)).Id);
}

TEST_F(BRepGraphViewsTest, DefsView_NbNodes_Positive)
{
  EXPECT_GT(myGraph.Defs().NbNodes(), 0u);
}

// ---------- DefsView Geometry ----------

TEST_F(BRepGraphViewsTest, DefsView_FaceSurface_NonNull)
{
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, BRepGraph_FaceId(anIdx)))
      << "Face " << anIdx << " has no surface representation";
  }
}

TEST_F(BRepGraphViewsTest, DefsView_EdgeCurve3d_NonNull)
{
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, BRepGraph_EdgeId(anIdx)))
      << "Edge " << anIdx << " has no Curve3D representation";
  }
}

TEST_F(BRepGraphViewsTest, DefsView_FindPCurve_NoCrash)
{
  // FindPCurve may or may not return a non-null pointer for an arbitrary edge/face pair.
  // Just verify it does not crash.
  (void)BRepGraph_Tool::Edge::FindPCurve(myGraph, BRepGraph_EdgeId(0), BRepGraph_FaceId(0));
}

// ---------- UIDsView ----------

TEST_F(BRepGraphViewsTest, UIDsView_Of_Valid)
{
  BRepGraph_NodeId     aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID& aUID = myGraph.UIDs().Of(aFaceId);
  EXPECT_TRUE(aUID.IsValid());
}

TEST_F(BRepGraphViewsTest, UIDsView_Generation_Positive)
{
  EXPECT_GT(myGraph.UIDs().Generation(), 0u);
}

// ---------- SpatialView ----------

TEST_F(BRepGraphViewsTest, SpatialView_AdjacentFaces_FourPerBoxFace)
{
  BRepGraph_NodeId                     aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  NCollection_Vector<BRepGraph_NodeId> aResult = myGraph.Spatial().AdjacentFaces(aFaceId);
  EXPECT_EQ(aResult.Length(), 4);
}

TEST_F(BRepGraphViewsTest, SpatialView_FacesOfEdge_TwoPerBoxEdge)
{
  BRepGraph_NodeId                     anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aResult = myGraph.Spatial().FacesOfEdge(anEdgeId);
  EXPECT_EQ(aResult.Length(), 2);
}

// ---------- BndLib ----------

TEST_F(BRepGraphViewsTest, BndLib_BoundingBox_NonVoid)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Bnd_Box          aBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aBox);
  EXPECT_FALSE(aBox.IsVoid());
}

TEST_F(BRepGraphViewsTest, BndLib_Centroid_InsideBBox)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  gp_Pnt           aCentroid = bboxCenter(myGraph, aFaceId);
  Bnd_Box          aBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aBox);
  ASSERT_FALSE(aBox.IsVoid());

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  const double aTol = 1.0;
  EXPECT_GE(aCentroid.X(), aXmin - aTol);
  EXPECT_LE(aCentroid.X(), aXmax + aTol);
  EXPECT_GE(aCentroid.Y(), aYmin - aTol);
  EXPECT_LE(aCentroid.Y(), aYmax + aTol);
}

// ---------- AttrsView ----------

TEST_F(BRepGraphViewsTest, AttrsView_SetGet_RoundTrip)
{
  BRepGraph_NodeId                     aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const int                            aKey   = 42;
  occ::handle<BRepGraph_UserAttribute> anAttr = new TestUserAttribute();
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);
  occ::handle<BRepGraph_UserAttribute> aRetrieved = myGraph.Attrs().Get(aFaceId, aKey);
  EXPECT_EQ(aRetrieved, anAttr);
}

TEST_F(BRepGraphViewsTest, AttrsView_Remove_Works)
{
  BRepGraph_NodeId                     aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const int                            aKey   = 99;
  occ::handle<BRepGraph_UserAttribute> anAttr = new TestUserAttribute();
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);
  EXPECT_TRUE(myGraph.Attrs().Remove(aFaceId, aKey));
  EXPECT_TRUE(myGraph.Attrs().Get(aFaceId, aKey).IsNull());
}

// ---------- ShapesView ----------

TEST_F(BRepGraphViewsTest, ShapesView_Shape_NonNull)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  EXPECT_FALSE(myGraph.Shapes().Shape(aFaceId).IsNull());
}

TEST_F(BRepGraphViewsTest, ShapesView_HasOriginal_True)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  EXPECT_TRUE(myGraph.Shapes().HasOriginal(aFaceId));
}

// ---------- MutView ----------

TEST_F(BRepGraphViewsTest, MutView_EdgeDef_MarksModified)
{
  {
    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge = myGraph.MutEdge(BRepGraph_EdgeId(0));
  }
  EXPECT_TRUE(myGraph.Defs().Edge(BRepGraph_EdgeId(0)).IsModified);
}

// ---------- BuilderView ----------

TEST_F(BRepGraphViewsTest, BuilderView_AddVertexDef_Works)
{
  const int        aNbBefore = myGraph.Defs().NbVertices();
  BRepGraph_NodeId aVtx      = myGraph.Builder().AddVertexDef(gp_Pnt(1, 2, 3), 0.001);
  EXPECT_TRUE(aVtx.IsValid());
  EXPECT_EQ(myGraph.Defs().NbVertices(), aNbBefore + 1);
}

TEST_F(BRepGraphViewsTest, BuilderView_IsRemoved_False)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  EXPECT_FALSE(myGraph.Defs().IsRemoved(aFaceId));
}

// ---------- AnalyzeView ----------

TEST_F(BRepGraphViewsTest, AnalyzeView_FreeEdges_MatchesStatic)
{
  NCollection_Vector<BRepGraph_EdgeId> aViewResult = myGraph.Analyze().FreeEdges();
  NCollection_Vector<BRepGraph_EdgeId> aFlatResult = BRepGraph_Analyze::FreeEdges(myGraph);
  EXPECT_EQ(aViewResult.Length(), aFlatResult.Length());
}

TEST_F(BRepGraphViewsTest, AnalyzeView_DegenerateWires_MatchesStatic)
{
  NCollection_Vector<BRepGraph_WireId> aViewResult = myGraph.Analyze().DegenerateWires();
  NCollection_Vector<BRepGraph_WireId> aFlatResult = BRepGraph_Analyze::DegenerateWires(myGraph);
  EXPECT_EQ(aViewResult.Length(), aFlatResult.Length());
}

TEST_F(BRepGraphViewsTest, AnalyzeView_Decompose_MatchesStatic)
{
  NCollection_Vector<BRepGraph_SubGraph> aViewResult = myGraph.Analyze().Decompose();
  NCollection_Vector<BRepGraph_SubGraph> aFlatResult = BRepGraph_Analyze::Decompose(myGraph);
  EXPECT_EQ(aViewResult.Length(), aFlatResult.Length());
}

// ---------- History() accessor ----------

TEST_F(BRepGraphViewsTest, History_ConstAccessor)
{
  const BRepGraph& aConstGraph = myGraph;
  EXPECT_TRUE(aConstGraph.History().IsEnabled());
}

TEST_F(BRepGraphViewsTest, History_MutableAccessor)
{
  myGraph.History().SetEnabled(false);
  EXPECT_FALSE(myGraph.History().IsEnabled());
  myGraph.History().SetEnabled(true);
  EXPECT_TRUE(myGraph.History().IsEnabled());
}

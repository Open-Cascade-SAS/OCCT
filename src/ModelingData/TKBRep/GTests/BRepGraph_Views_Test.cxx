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

#include <BRepGraph.hxx>
#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_AnalyzeView.hxx>
#include <BRepGraph_AttrsView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_UsagesView.hxx>

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
    TestUserAttribute() = default;
  };
}

class BRepGraphViewsTest : public testing::Test
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

// ---------- DefsView ----------

TEST_F(BRepGraphViewsTest, DefsView_NbFaces_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbFaces(), myGraph.NbFaceDefs());
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
}

TEST_F(BRepGraphViewsTest, DefsView_NbSolids_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbSolids(), myGraph.NbSolidDefs());
}

TEST_F(BRepGraphViewsTest, DefsView_NbShells_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbShells(), myGraph.NbShellDefs());
}

TEST_F(BRepGraphViewsTest, DefsView_NbWires_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbWires(), myGraph.NbWireDefs());
}

TEST_F(BRepGraphViewsTest, DefsView_NbEdges_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbEdges(), myGraph.NbEdgeDefs());
  EXPECT_EQ(myGraph.Defs().NbEdges(), 12);
}

TEST_F(BRepGraphViewsTest, DefsView_NbVertices_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbVertices(), myGraph.NbVertexDefs());
  EXPECT_EQ(myGraph.Defs().NbVertices(), 8);
}

TEST_F(BRepGraphViewsTest, DefsView_FaceAccessor_MatchesFlat)
{
  for (int anIdx = 0; anIdx < myGraph.NbFaceDefs(); ++anIdx)
  {
    EXPECT_EQ(&myGraph.Defs().Face(anIdx), &myGraph.FaceDefinition(anIdx));
  }
}

TEST_F(BRepGraphViewsTest, DefsView_TopoDef_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_EQ(myGraph.Defs().TopoDef(aFaceId), myGraph.TopoDef(aFaceId));
}

TEST_F(BRepGraphViewsTest, DefsView_NbNodes_MatchesFlat)
{
  EXPECT_EQ(myGraph.Defs().NbNodes(), myGraph.NbNodes());
}

// ---------- UsagesView ----------

TEST_F(BRepGraphViewsTest, UsagesView_NbFaces_MatchesFlat)
{
  EXPECT_EQ(myGraph.Usages().NbFaces(), myGraph.NbFaceUsages());
}

TEST_F(BRepGraphViewsTest, UsagesView_FaceAccessor_MatchesFlat)
{
  for (int anIdx = 0; anIdx < myGraph.NbFaceUsages(); ++anIdx)
  {
    EXPECT_EQ(&myGraph.Usages().Face(anIdx), &myGraph.FaceUsageNode(anIdx));
  }
}

// ---------- GeomView ----------

TEST_F(BRepGraphViewsTest, GeomView_NbSurfaces_MatchesFlat)
{
  EXPECT_EQ(myGraph.Geom().NbSurfaces(), myGraph.NbSurfaces());
  EXPECT_EQ(myGraph.Geom().NbSurfaces(), 6);
}

TEST_F(BRepGraphViewsTest, GeomView_NbCurves_MatchesFlat)
{
  EXPECT_EQ(myGraph.Geom().NbCurves(), myGraph.NbCurves());
}

TEST_F(BRepGraphViewsTest, GeomView_SurfaceOf_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_EQ(myGraph.Geom().SurfaceOf(aFaceId), myGraph.SurfaceOf(aFaceId));
}

TEST_F(BRepGraphViewsTest, GeomView_CurveOf_MatchesFlat)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  EXPECT_EQ(myGraph.Geom().CurveOf(anEdgeId), myGraph.CurveOf(anEdgeId));
}

TEST_F(BRepGraphViewsTest, GeomView_PCurveOf_MatchesFlat)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_EQ(myGraph.Geom().PCurveOf(anEdgeId, aFaceId),
            myGraph.PCurveOf(anEdgeId, aFaceId));
}

// ---------- UIDsView ----------

TEST_F(BRepGraphViewsTest, UIDsView_IsEnabled_MatchesFlat)
{
  EXPECT_EQ(myGraph.UIDs().IsEnabled(), myGraph.IsUIDEnabled());
  EXPECT_TRUE(myGraph.UIDs().IsEnabled());
}

TEST_F(BRepGraphViewsTest, UIDsView_Of_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_EQ(myGraph.UIDs().Of(aFaceId), myGraph.UIDOf(aFaceId));
}

TEST_F(BRepGraphViewsTest, UIDsView_Generation_MatchesFlat)
{
  EXPECT_EQ(myGraph.UIDs().Generation(), myGraph.Generation());
}

// ---------- SpatialView ----------

TEST_F(BRepGraphViewsTest, SpatialView_AdjacentFaces_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  NCollection_Vector<BRepGraph_NodeId> aViewResult   = myGraph.Spatial().AdjacentFaces(aFaceId);
  NCollection_Vector<BRepGraph_NodeId> aFlatResult   = myGraph.AdjacentFaces(aFaceId);
  EXPECT_EQ(aViewResult.Length(), aFlatResult.Length());
}

TEST_F(BRepGraphViewsTest, SpatialView_FacesOfEdge_MatchesFlat)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeKind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aViewResult = myGraph.Spatial().FacesOfEdge(anEdgeId);
  NCollection_Vector<BRepGraph_NodeId> aFlatResult = myGraph.FacesOfEdge(anEdgeId);
  EXPECT_EQ(aViewResult.Length(), aFlatResult.Length());
}

// ---------- CacheView ----------

TEST_F(BRepGraphViewsTest, CacheView_BoundingBox_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  Bnd_Box aViewBox = myGraph.Cache().BoundingBox(aFaceId);
  Bnd_Box aFlatBox = myGraph.BoundingBox(aFaceId);
  EXPECT_FALSE(aViewBox.IsVoid());
  EXPECT_FALSE(aFlatBox.IsVoid());
}

TEST_F(BRepGraphViewsTest, CacheView_Centroid_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  gp_Pnt aViewPnt = myGraph.Cache().Centroid(aFaceId);
  gp_Pnt aFlatPnt = myGraph.Centroid(aFaceId);
  EXPECT_NEAR(aViewPnt.Distance(aFlatPnt), 0.0, Precision::Confusion());
}

// ---------- AttrsView ----------

TEST_F(BRepGraphViewsTest, AttrsView_SetGet_RoundTrip)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const int aKey = 42;
  BRepGraph_UserAttrPtr anAttr = std::make_shared<TestUserAttribute>();
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);
  BRepGraph_UserAttrPtr aRetrieved = myGraph.Attrs().Get(aFaceId, aKey);
  EXPECT_EQ(aRetrieved.get(), anAttr.get());
}

TEST_F(BRepGraphViewsTest, AttrsView_Remove_Works)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  const int aKey = 99;
  BRepGraph_UserAttrPtr anAttr = std::make_shared<TestUserAttribute>();
  myGraph.Attrs().Set(aFaceId, aKey, anAttr);
  EXPECT_TRUE(myGraph.Attrs().Remove(aFaceId, aKey));
  EXPECT_EQ(myGraph.Attrs().Get(aFaceId, aKey), nullptr);
}

// ---------- ShapesView ----------

TEST_F(BRepGraphViewsTest, ShapesView_Shape_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_FALSE(myGraph.Shapes().Shape(aFaceId).IsNull());
}

TEST_F(BRepGraphViewsTest, ShapesView_HasOriginal_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_EQ(myGraph.Shapes().HasOriginal(aFaceId), myGraph.HasOriginalShape(aFaceId));
}

// ---------- MutView ----------

TEST_F(BRepGraphViewsTest, MutView_EdgeDef_MatchesFlat)
{
  EXPECT_EQ(&myGraph.Mut().EdgeDef(0), &myGraph.MutableEdgeDefinition(0));
}

// ---------- BuilderView ----------

TEST_F(BRepGraphViewsTest, BuilderView_AddVertexDef_Works)
{
  const int aNbBefore = myGraph.NbVertexDefs();
  BRepGraph_NodeId aVtx = myGraph.Builder().AddVertexDef(gp_Pnt(1, 2, 3), 0.001);
  EXPECT_TRUE(aVtx.IsValid());
  EXPECT_EQ(myGraph.NbVertexDefs(), aNbBefore + 1);
}

TEST_F(BRepGraphViewsTest, BuilderView_IsRemoved_MatchesFlat)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeKind::Face, 0);
  EXPECT_EQ(myGraph.Builder().IsRemoved(aFaceId), myGraph.IsRemoved(aFaceId));
}

// ---------- AnalyzeView ----------

TEST_F(BRepGraphViewsTest, AnalyzeView_FreeEdges_MatchesStatic)
{
  NCollection_Vector<BRepGraph_NodeId> aViewResult = myGraph.Analyze().FreeEdges();
  NCollection_Vector<BRepGraph_NodeId> aFlatResult = BRepGraph_Analyze::FreeEdges(myGraph);
  EXPECT_EQ(aViewResult.Length(), aFlatResult.Length());
}

TEST_F(BRepGraphViewsTest, AnalyzeView_DegenerateWires_MatchesStatic)
{
  NCollection_Vector<BRepGraph_NodeId> aViewResult = myGraph.Analyze().DegenerateWires();
  NCollection_Vector<BRepGraph_NodeId> aFlatResult = BRepGraph_Analyze::DegenerateWires(myGraph);
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

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

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepGraph_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <OSD_Timer.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <Precision.hxx>
#include <Standard_Handle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <cmath>
#include <iostream>

#include <gtest/gtest.h>

namespace
{

BRepGraph_CoEdgeId firstCoEdgeOfFace(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  for (BRepGraph_RefsWireOfFace aWireRefIt(theGraph, theFaceId); aWireRefIt.More();
       aWireRefIt.Next())
  {
    const BRepGraph_WireId aWireId =
      theGraph.Refs().Wires().Entry(aWireRefIt.CurrentId()).ChildWireId;
    for (BRepGraph_CoEdgesOfWire aCoEdgeIt(theGraph, aWireId); aCoEdgeIt.More(); aCoEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
      if (theGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceId == theFaceId)
      {
        return aCoEdgeId;
      }
    }
  }
  return BRepGraph_CoEdgeId();
}

occ::handle<Poly_Polygon3D> makePolygon3D(const gp_Pnt& theFirst, const gp_Pnt& theSecond)
{
  occ::handle<Poly_Polygon3D> aPolygon = new Poly_Polygon3D(2, false);
  aPolygon->ChangeNodes().SetValue(1, theFirst);
  aPolygon->ChangeNodes().SetValue(2, theSecond);
  return aPolygon;
}

occ::handle<Poly_Polygon2D> makePolygon2D(const gp_Pnt2d& theFirst, const gp_Pnt2d& theSecond)
{
  occ::handle<Poly_Polygon2D> aPolygon = new Poly_Polygon2D(2);
  aPolygon->ChangeNodes().SetValue(1, theFirst);
  aPolygon->ChangeNodes().SetValue(2, theSecond);
  return aPolygon;
}

occ::handle<Poly_PolygonOnTriangulation> makePolygonOnTri()
{
  occ::handle<Poly_PolygonOnTriangulation> aPolygon = new Poly_PolygonOnTriangulation(2, false);
  aPolygon->SetNode(1, 1);
  aPolygon->SetNode(2, 2);
  return aPolygon;
}

void expectVerticesTransformed(const BRepGraph& theSource,
                               const BRepGraph& theResult,
                               const gp_Trsf&   theTrsf)
{
  const uint32_t aNbV = theSource.Topo().Vertices().Nb();
  ASSERT_EQ(theResult.Topo().Vertices().Nb(), aNbV);
  for (BRepGraph_VertexId aVId(0); aVId.IsValid(aNbV); ++aVId)
  {
    gp_Pnt anExpected = BRepGraph_Tool::Vertex::Pnt(theSource, aVId);
    anExpected.Transform(theTrsf);
    const gp_Pnt aTrans = BRepGraph_Tool::Vertex::Pnt(theResult, aVId);
    EXPECT_NEAR(aTrans.X(), anExpected.X(), Precision::Confusion()) << "vertex " << aVId.Index;
    EXPECT_NEAR(aTrans.Y(), anExpected.Y(), Precision::Confusion()) << "vertex " << aVId.Index;
    EXPECT_NEAR(aTrans.Z(), anExpected.Z(), Precision::Confusion()) << "vertex " << aVId.Index;
  }
}

} // namespace

TEST(BRepGraph_TransformTest, TranslateBox_FaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 200.0, 300.0));

  BRepGraph aResultGraph;
  ASSERT_TRUE(
    BRepGraph_Transform::Perform(aGraph, aResultGraph, aTrsf, BRepGraph_Copy::GeomPolicy::Copy));
  ASSERT_FALSE(aResultGraph.IsEmpty());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_TransformTest, TranslateBox_AreaPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);
  const double anOrigArea = aOrigProps.Mass();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  BRepGraph aResultGraph;
  ASSERT_TRUE(
    BRepGraph_Transform::Perform(aGraph, aResultGraph, aTrsf, BRepGraph_Copy::GeomPolicy::Copy));
  ASSERT_FALSE(aResultGraph.IsEmpty());

  // Verify area is preserved by summing individual face areas.
  double         aTransArea = 0.0;
  const uint32_t aNbFaces   = aResultGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    TopoDS_Shape aFace = aResultGraph.Shapes().Reconstruct(aFaceId);
    GProp_GProps aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    aTransArea += std::abs(aProps.Mass());
  }

  EXPECT_NEAR(aTransArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_TransformTest, TranslateBox_VertexPointsShifted)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  const double aDx = 100.0, aDy = 200.0, aDz = 300.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  BRepGraph aResultGraph;
  ASSERT_TRUE(
    BRepGraph_Transform::Perform(aGraph, aResultGraph, aTrsf, BRepGraph_Copy::GeomPolicy::Copy));
  ASSERT_FALSE(aResultGraph.IsEmpty());
  ASSERT_EQ(aResultGraph.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  // Verify that all vertices have been shifted.
  const uint32_t aNbVertices = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const gp_Pnt anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);
    const gp_Pnt aTransPt = BRepGraph_Tool::Vertex::Pnt(aResultGraph, aVertexId);
    EXPECT_NEAR(aTransPt.X(), anOrigPt.X() + aDx, Precision::Confusion())
      << "Vertex " << aVertexId.Index << " X mismatch";
    EXPECT_NEAR(aTransPt.Y(), anOrigPt.Y() + aDy, Precision::Confusion())
      << "Vertex " << aVertexId.Index << " Y mismatch";
    EXPECT_NEAR(aTransPt.Z(), anOrigPt.Z() + aDz, Precision::Confusion())
      << "Vertex " << aVertexId.Index << " Z mismatch";
  }
}

TEST(BRepGraph_TransformTest, PerformIntoNonEmptyTargetDoesNotMoveExistingVertices)
{
  BRepGraph aSource;
  aSource.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aSourceBuild =
    aSource.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aSource.IsEmpty());

  BRepGraph                aTarget;
  const BRepGraph_VertexId anExistingVertex =
    aTarget.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.0e-7);
  const gp_Pnt anOriginalPoint = BRepGraph_Tool::Vertex::Pnt(aTarget, anExistingVertex);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 200.0, 300.0));

  ASSERT_TRUE(BRepGraph_Transform::Perform(aSource,
                                           aTarget,
                                           aTrsf,
                                           BRepGraph_Copy::GeomPolicy::Copy,
                                           BRepGraph_Copy::MeshPolicy::Drop));

  const gp_Pnt anAfterPoint = BRepGraph_Tool::Vertex::Pnt(aTarget, anExistingVertex);
  EXPECT_NEAR(anAfterPoint.X(), anOriginalPoint.X(), Precision::Confusion());
  EXPECT_NEAR(anAfterPoint.Y(), anOriginalPoint.Y(), Precision::Confusion());
  EXPECT_NEAR(anAfterPoint.Z(), anOriginalPoint.Z(), Precision::Confusion());
}

TEST(BRepGraph_TransformTest, LocationOnly_NoCopyGeom)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  const double aDx = 50.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, 0.0, 0.0));

  // GeomPolicy::Share: location-only, no geometry modification.
  BRepGraph aResultGraph;
  ASSERT_TRUE(
    BRepGraph_Transform::Perform(aGraph, aResultGraph, aTrsf, BRepGraph_Copy::GeomPolicy::Share));
  ASSERT_FALSE(aResultGraph.IsEmpty());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aResultGraph.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  // Vertex definition points must NOT be modified (location-only mode).
  const uint32_t aNbVertices = aGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const gp_Pnt anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);
    const gp_Pnt aGraphPt = BRepGraph_Tool::Vertex::Pnt(aResultGraph, aVertexId);
    EXPECT_NEAR(aGraphPt.X(), anOrigPt.X(), Precision::Confusion())
      << "Vertex " << aVertexId.Index << " point should not be modified";
  }

  // Verify the transform is stored on the shape-root OccurrenceRef's LocalLocation.
  ASSERT_GT(aResultGraph.Topo().Products().Nb(), 0);
  const BRepGraph_ProductId             aProductId = BRepGraph_ProductId::Start();
  const BRepGraphInc::ProductRelations& aProductRelations =
    aResultGraph.Topo().Products().Relations(aProductId);
  ASSERT_GE(aProductRelations.OccurrenceRefIds.Size(), 1);
  const BRepGraphInc::OccurrenceRef& aOccRef =
    aResultGraph.Refs().Occurrences().Entry(aProductRelations.OccurrenceRefIds.Value(0));
  const TopLoc_Location& aRootLoc = aOccRef.LocalLocation;
  EXPECT_FALSE(aRootLoc.IsIdentity());
  const gp_Trsf aProductTrsf = aRootLoc.Transformation();
  EXPECT_NEAR(aProductTrsf.Value(1, 4), aDx, Precision::Confusion());
  EXPECT_NEAR(aProductTrsf.Value(2, 4), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProductTrsf.Value(3, 4), 0.0, Precision::Confusion());

  // Verify that reconstructed solid + LocalLocation produces correct geometry.
  ASSERT_GT(aResultGraph.Topo().Solids().Nb(), 0);
  TopoDS_Shape aTransSolid = aResultGraph.Shapes().Reconstruct(BRepGraph_SolidId::Start());
  ASSERT_FALSE(aTransSolid.IsNull());
  aTransSolid.Location(aRootLoc);

  GProp_GProps aOrigProps;
  BRepGProp::SurfaceProperties(aBox, aOrigProps);

  GProp_GProps aTransProps;
  BRepGProp::SurfaceProperties(aTransSolid, aTransProps);
  EXPECT_NEAR(aTransProps.CentreOfMass().X(), aOrigProps.CentreOfMass().X() + aDx, 1.0);
}

TEST(BRepGraph_TransformTest, TransformSingleFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10.0, 20.0, 30.0));

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, aFaceId.Index);
  BRepGraph              aResultGraph;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResultGraph,
                                                 aFaceNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy)
                .IsValid());
  ASSERT_FALSE(aResultGraph.IsEmpty());
  EXPECT_EQ(aResultGraph.Topo().Faces().Nb(), 1);
}

TEST(BRepGraph_TransformTest, CopyMesh_TriangulationNodesTransformed)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  // Manually create a triangulation with known node positions on the first face.
  const BRepGraph_FaceId          aFaceId = BRepGraph_FaceId::Start();
  occ::handle<Poly_Triangulation> aSrcTri = new Poly_Triangulation(3, 1, false);
  aSrcTri->SetNode(1, gp_Pnt(1.0, 2.0, 3.0));
  aSrcTri->SetNode(2, gp_Pnt(4.0, 5.0, 6.0));
  aSrcTri->SetNode(3, gp_Pnt(7.0, 8.0, 9.0));
  aSrcTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aSrcTri->Deflection(0.1);

  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aSrcTri);
  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aSrcTri);

  const double aDx = 5.0, aDy = 10.0, aDz = 15.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  // Transform with MeshPolicy::Copy.
  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::Perform(aGraph,
                                           aResult,
                                           aTrsf,
                                           BRepGraph_Copy::GeomPolicy::Copy,
                                           BRepGraph_Copy::MeshPolicy::Copy));
  ASSERT_FALSE(aResult.IsEmpty());

  // The persistent triangulation on the first face must be present and transformed.
  const occ::handle<Poly_Triangulation>& aNewTri =
    aResult.Mesh().Persistent().Faces().Triangulation(aFaceId);
  ASSERT_FALSE(aNewTri.IsNull());
  ASSERT_EQ(aNewTri->NbNodes(), 3);

  EXPECT_NEAR(aNewTri->Node(1).X(), 1.0 + aDx, Precision::Confusion());
  EXPECT_NEAR(aNewTri->Node(1).Y(), 2.0 + aDy, Precision::Confusion());
  EXPECT_NEAR(aNewTri->Node(1).Z(), 3.0 + aDz, Precision::Confusion());
  EXPECT_NEAR(aNewTri->Node(3).X(), 7.0 + aDx, Precision::Confusion());

  // The source triangulation must not be mutated.
  EXPECT_NEAR(aSrcTri->Node(1).X(), 1.0, Precision::Confusion());

  // Deflection must scale (translation has scale = 1, so no change).
  EXPECT_NEAR(aNewTri->Deflection(), 0.1, Precision::Confusion());
}

TEST(BRepGraph_TransformTest, CopyMesh_False_TriangulationInvalidated)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId          aFaceId = BRepGraph_FaceId::Start();
  occ::handle<Poly_Triangulation> aTri    = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0, 0, 0));
  aTri->SetNode(2, gp_Pnt(1, 0, 0));
  aTri->SetNode(3, gp_Pnt(0, 1, 0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));

  // Default: MeshPolicy::Drop -> triangulations are discarded.
  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::Perform(aGraph,
                                           aResult,
                                           aTrsf,
                                           BRepGraph_Copy::GeomPolicy::Copy,
                                           BRepGraph_Copy::MeshPolicy::Drop));
  ASSERT_FALSE(aResult.IsEmpty());

  EXPECT_TRUE(aResult.Mesh().Persistent().Faces().Triangulation(aFaceId).IsNull());
  EXPECT_FALSE(aResult.Mesh().Effective().Faces().Has(aFaceId));
}

TEST(BRepGraph_TransformTest, MoveRef_ChildRef_ComposesLocation)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());
  ASSERT_EQ(aGraph.Refs().Children().IdsOf(aCompound).Size(), 1);
  const BRepGraph_ChildRefId aChildRef = aGraph.Refs().Children().IdsOf(aCompound).First();

  EXPECT_TRUE(aGraph.Refs().Children().Entry(aChildRef).LocalLocation.IsIdentity());

  const double aDx = 42.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, 0.0, 0.0));

  const bool aOk = BRepGraph_Transform::MoveRef(aGraph, aChildRef, aTrsf);
  EXPECT_TRUE(aOk);
  EXPECT_NEAR(aGraph.Refs().Children().Entry(aChildRef).LocalLocation.Transformation().Value(1, 4),
              aDx,
              Precision::Confusion());

  BRepGraph_Transform::MoveRef(aGraph, aChildRef, aTrsf);
  EXPECT_NEAR(aGraph.Refs().Children().Entry(aChildRef).LocalLocation.Transformation().Value(1, 4),
              2.0 * aDx,
              Precision::Confusion());
}

TEST(BRepGraph_TransformTest, MoveRef_OccurrenceRef_ComposesLocationAndRejectsScale)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aProductId = BRepGraph_ProductId::Start();
  ASSERT_FALSE(aGraph.Topo().Products().Relations(aProductId).OccurrenceRefIds.IsEmpty());
  const BRepGraph_OccurrenceRefId anOccRef =
    aGraph.Topo().Products().Relations(aProductId).OccurrenceRefIds.First();

  gp_Trsf aTrans;
  aTrans.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  EXPECT_TRUE(BRepGraph_Transform::MoveRef(aGraph, anOccRef, aTrans));
  EXPECT_NEAR(
    aGraph.Refs().Occurrences().Entry(anOccRef).LocalLocation.Transformation().Value(1, 4),
    1.0,
    Precision::Confusion());

  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(), 2.0); // scale factor != 1

  const bool aOk = BRepGraph_Transform::MoveRef(aGraph, anOccRef, aScale);
  EXPECT_FALSE(aOk);
  EXPECT_NEAR(
    aGraph.Refs().Occurrences().Entry(anOccRef).LocalLocation.Transformation().Value(1, 4),
    1.0,
    Precision::Confusion());
}

TEST(BRepGraph_TransformTest, TransformNode_FaceKind_VertexPointsShifted)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const double aDx = 5.0, aDy = 10.0, aDz = 15.0;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, aFaceId.Index);
  BRepGraph              aSourceSubgraph;
  const BRepGraph_NodeId aCopiedFaceNode =
    BRepGraph_Copy::CopyNode(aGraph,
                             aSourceSubgraph,
                             aFaceNode,
                             BRepGraph_Copy::GeomPolicy::Copy,
                             BRepGraph_Copy::MeshPolicy::Drop);
  ASSERT_TRUE(aCopiedFaceNode.IsValid());
  ASSERT_FALSE(aSourceSubgraph.IsEmpty());

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aFaceNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), 1);
  expectVerticesTransformed(aSourceSubgraph, aResult, aTrsf);
}

TEST(BRepGraph_TransformTest, TransformNode_ShellKind)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Shells().Nb(), 1);

  const BRepGraph_ShellId aShellId = BRepGraph_ShellId::Start();
  const BRepGraph_NodeId  aShellNode(BRepGraph_NodeId::Kind::Shell, aShellId.Index);
  BRepGraph               aSourceSubgraph;
  const BRepGraph_NodeId  aCopiedShellNode =
    BRepGraph_Copy::CopyNode(aGraph,
                             aSourceSubgraph,
                             aShellNode,
                             BRepGraph_Copy::GeomPolicy::Copy,
                             BRepGraph_Copy::MeshPolicy::Drop);
  ASSERT_TRUE(aCopiedShellNode.IsValid());
  ASSERT_FALSE(aSourceSubgraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aShellNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  // The shell copy should have the same number of faces as the source shell.
  EXPECT_EQ(aResult.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  // All vertices must be shifted by the translation.
  expectVerticesTransformed(aSourceSubgraph, aResult, aTrsf);
}

TEST(BRepGraph_TransformTest, TransformNode_SolidKind)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Solids().Nb(), 1);

  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_NodeId  aSolidNode(BRepGraph_NodeId::Kind::Solid, aSolidId.Index);
  BRepGraph               aSourceSubgraph;
  const BRepGraph_NodeId  aCopiedSolidNode =
    BRepGraph_Copy::CopyNode(aGraph,
                             aSourceSubgraph,
                             aSolidNode,
                             BRepGraph_Copy::GeomPolicy::Copy,
                             BRepGraph_Copy::MeshPolicy::Drop);
  ASSERT_TRUE(aCopiedSolidNode.IsValid());
  ASSERT_FALSE(aSourceSubgraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 50.0, 0.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aSolidNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  EXPECT_EQ(aResult.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Shells().Nb(), aGraph.Topo().Shells().Nb());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  expectVerticesTransformed(aSourceSubgraph, aResult, aTrsf);
}

TEST(BRepGraph_TransformTest, TransformNode_VertexKind)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Vertices().Nb(), 1);

  const BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
  const BRepGraph_NodeId   aVertexNode(BRepGraph_NodeId::Kind::Vertex, aVertexId.Index);
  const gp_Pnt             anOrigPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aVertexNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  EXPECT_EQ(aResult.Topo().Vertices().Nb(), 1);
  const gp_Pnt aTransPt = BRepGraph_Tool::Vertex::Pnt(aResult, BRepGraph_VertexId::Start());
  EXPECT_NEAR(aTransPt.X(), anOrigPt.X() + 1.0, Precision::Confusion());
  EXPECT_NEAR(aTransPt.Y(), anOrigPt.Y() + 2.0, Precision::Confusion());
  EXPECT_NEAR(aTransPt.Z(), anOrigPt.Z() + 3.0, Precision::Confusion());
}

TEST(BRepGraph_TransformTest, TransformNode_CompoundKind)
{
  // ensureCompound must walk children, transform each, and preserve count.
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  BRepPrimAPI_MakeBox aBox1(5.0, 5.0, 5.0);
  BRepPrimAPI_MakeBox aBox2(3.0, 3.0, 3.0);
  aBB.Add(aCompound, aBox1.Shape());
  aBB.Add(aCompound, aBox2.Shape());

  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Compounds().Nb(), 1);

  const BRepGraph_CompoundId aCompoundId = BRepGraph_CompoundId::Start();
  const BRepGraph_NodeId     aCompoundNode(BRepGraph_NodeId::Kind::Compound, aCompoundId.Index);
  BRepGraph                  aSourceSubgraph;
  const BRepGraph_NodeId     aCopiedCompoundNode =
    BRepGraph_Copy::CopyNode(aGraph,
                             aSourceSubgraph,
                             aCompoundNode,
                             BRepGraph_Copy::GeomPolicy::Copy,
                             BRepGraph_Copy::MeshPolicy::Drop);
  ASSERT_TRUE(aCopiedCompoundNode.IsValid());
  ASSERT_FALSE(aSourceSubgraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aCompoundNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  EXPECT_EQ(aResult.Topo().Compounds().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Solids().Nb(), aGraph.Topo().Solids().Nb());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());

  expectVerticesTransformed(aSourceSubgraph, aResult, aTrsf);
}

TEST(BRepGraph_TransformTest, TransformNode_CompSolidKind)
{
  // ensureCompSolid must walk solid refs and transform each.
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);
  BRepPrimAPI_MakeBox aBoxMaker(4.0, 4.0, 4.0);
  aBB.Add(aCompSolid, aBoxMaker.Shape());

  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompSolid);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().CompSolids().Nb(), 1);

  const BRepGraph_CompSolidId aCompSolidId = BRepGraph_CompSolidId::Start();
  const BRepGraph_NodeId      aCompSolidNode(BRepGraph_NodeId::Kind::CompSolid, aCompSolidId.Index);
  BRepGraph                   aSourceSubgraph;
  const BRepGraph_NodeId      aCopiedCompSolidNode =
    BRepGraph_Copy::CopyNode(aGraph,
                             aSourceSubgraph,
                             aCompSolidNode,
                             BRepGraph_Copy::GeomPolicy::Copy,
                             BRepGraph_Copy::MeshPolicy::Drop);
  ASSERT_TRUE(aCopiedCompSolidNode.IsValid());
  ASSERT_FALSE(aSourceSubgraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 7.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aCompSolidNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  EXPECT_EQ(aResult.Topo().CompSolids().Nb(), 1);
  EXPECT_EQ(aResult.Topo().Solids().Nb(), aGraph.Topo().Solids().Nb());

  expectVerticesTransformed(aSourceSubgraph, aResult, aTrsf);
}

TEST(BRepGraph_TransformTest, TransformNode_NegativeScale_VertexPointsMirrored)
{
  // Smoke-tests the negative-scale geometry path through TransformNode: every
  // vertex point should be mirrored about the origin and the result graph must
  // remain coherent (same face/edge/vertex counts, valid IsDone).
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 10.0, 10.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_NodeId  aSolidNode(BRepGraph_NodeId::Kind::Solid, aSolidId.Index);
  BRepGraph               aSourceSubgraph;
  const BRepGraph_NodeId  aCopiedSolidNode =
    BRepGraph_Copy::CopyNode(aGraph,
                             aSourceSubgraph,
                             aSolidNode,
                             BRepGraph_Copy::GeomPolicy::Copy,
                             BRepGraph_Copy::MeshPolicy::Drop);
  ASSERT_TRUE(aCopiedSolidNode.IsValid());
  ASSERT_FALSE(aSourceSubgraph.IsEmpty());

  gp_Trsf aTrsf;
  aTrsf.SetMirror(gp_Pnt(0.0, 0.0, 0.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aSolidNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Drop)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());
  EXPECT_EQ(aResult.Topo().Faces().Nb(), aGraph.Topo().Faces().Nb());
  EXPECT_EQ(aResult.Topo().Vertices().Nb(), aGraph.Topo().Vertices().Nb());

  expectVerticesTransformed(aSourceSubgraph, aResult, aTrsf);
}

TEST(BRepGraph_TransformTest, TransformNode_CopyGeomAndMesh_DropsRuntimeCache)
{
  // Regression test: when TransformNode is called with GeomPolicy::Copy and
  // MeshPolicy::Copy, the runtime cache is not copied or transformed.
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 10.0, 10.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  // Attach a cached triangulation on the first face.
  const BRepGraph_FaceId          aFaceId  = BRepGraph_FaceId::Start();
  occ::handle<Poly_Triangulation> aLodTri2 = new Poly_Triangulation(3, 1, false);
  aLodTri2->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aLodTri2->SetNode(2, gp_Pnt(2.0, 0.0, 0.0));
  aLodTri2->SetNode(3, gp_Pnt(0.0, 2.0, 0.0));
  aLodTri2->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aLodTri2->Deflection(1.0);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aLodTri2);

  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_NodeId  aSolidNode =
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aSolidId.Index);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(5.0, 0.0, 0.0));

  // Previously failing path: GeomPolicy::Copy, MeshPolicy::Copy.
  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aSolidNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Copy)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  // Runtime cache is not copied or transformed.
  const BRepGraph_CacheMesh::FaceMeshEntry* aEntry = aResult.Mesh().Cache().Faces().Entry(aFaceId);
  EXPECT_EQ(aEntry, nullptr);
}

TEST(BRepGraph_TransformTest, TransformNode_CopyGeomAndMesh_CopiesPersistentMeshAndDropsCache)
{
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 10.0, 10.0);
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId   aFaceId   = BRepGraph_FaceId::Start();
  const BRepGraph_CoEdgeId aCoEdgeId = firstCoEdgeOfFace(aGraph, aFaceId);
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId anEdgeId = aGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));

  occ::handle<Poly_Triangulation> anUnusedTri = new Poly_Triangulation(3, 1, false);
  anUnusedTri->SetNode(1, gp_Pnt(100.0, 0.0, 0.0));
  anUnusedTri->SetNode(2, gp_Pnt(101.0, 0.0, 0.0));
  anUnusedTri->SetNode(3, gp_Pnt(100.0, 1.0, 0.0));
  anUnusedTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, anUnusedTri);
  aGraph.Editor().Edges().SetPersistentPolygon3D(
    anEdgeId,
    makePolygon3D(gp_Pnt(100.0, 0.0, 0.0), gp_Pnt(101.0, 0.0, 0.0)));
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(
    aCoEdgeId,
    makePolygon2D(gp_Pnt2d(100.0, 0.0), gp_Pnt2d(101.0, 0.0)));
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, makePolygonOnTri());

  occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTri->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTri->SetNode(3, gp_Pnt(0.0, 1.0, 0.0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);
  occ::handle<Poly_Polygon3D> aPolygon3D =
    makePolygon3D(gp_Pnt(1.0, 2.0, 3.0), gp_Pnt(4.0, 5.0, 6.0));
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  occ::handle<Poly_Polygon2D> aPolygon2D = makePolygon2D(gp_Pnt2d(7.0, 8.0), gp_Pnt2d(9.0, 10.0));
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri = makePolygonOnTri();
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  aGraph.Mesh().Editor().Faces().SetCachedTriangulation(aFaceId, aTri);
  aGraph.Mesh().Editor().Edges().SetCachedPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Mesh().Editor().CoEdges().SetCachedPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Mesh().Editor().CoEdges().AppendCachedPolygonOnTri(aCoEdgeId, aPolygonOnTri);

  const BRepGraph_NodeId aSolidNode(BRepGraph_SolidId::Start());
  gp_Trsf                aTrsf;
  aTrsf.SetTranslation(gp_Vec(5.0, 0.0, 0.0));

  BRepGraph aResult;
  ASSERT_TRUE(BRepGraph_Transform::TransformNode(aGraph,
                                                 aResult,
                                                 aSolidNode,
                                                 aTrsf,
                                                 BRepGraph_Copy::GeomPolicy::Copy,
                                                 BRepGraph_Copy::MeshPolicy::Copy)
                .IsValid());
  ASSERT_FALSE(aResult.IsEmpty());

  const BRepGraph_CoEdgeId aResultFirstCoEdgeId = firstCoEdgeOfFace(aResult, aFaceId);
  ASSERT_TRUE(aResultFirstCoEdgeId.IsValid(aResult.Topo().CoEdges().Nb()));
  const BRepGraph_EdgeId aResultEdgeId =
    aResult.Topo().CoEdges().Definition(aResultFirstCoEdgeId).ChildEdgeId;
  ASSERT_TRUE(aResultEdgeId.IsValid(aResult.Topo().Edges().Nb()));

  EXPECT_EQ(aResult.Mesh().Cache().Edges().Entry(aResultEdgeId), nullptr);

  const occ::handle<Poly_Polygon3D>& aPersistentPolygon3D =
    aResult.Mesh().Persistent().Edges().Polygon3D(aResultEdgeId);
  ASSERT_FALSE(aPersistentPolygon3D.IsNull());
  EXPECT_NEAR(aPersistentPolygon3D->Nodes().Value(1).X(), 1.0 + 5.0, Precision::Confusion());

  const BRepGraph_CoEdgeId aCopiedCoEdgeId = aResultFirstCoEdgeId;
  EXPECT_FALSE(aResult.Mesh().Cache().CoEdges().Has(aCopiedCoEdgeId));

  const BRepGraphInc::CoEdgeDef& aCopiedCoEdgeDef =
    aResult.Topo().CoEdges().Definition(aCopiedCoEdgeId);
  const occ::handle<Poly_Polygon2D>& aPersistentPolygon2D =
    aResult.Mesh().Persistent().CoEdges().PolygonOnSurface(aCopiedCoEdgeId);
  ASSERT_FALSE(aPersistentPolygon2D.IsNull());
  EXPECT_NEAR(aPersistentPolygon2D->Nodes().Value(1).X(), 7.0, Precision::Confusion());

  ASSERT_TRUE(aCopiedCoEdgeDef.FaceId.IsValid(aResult.Topo().Faces().Nb()));
  const occ::handle<Poly_Triangulation>& aCopiedTri =
    aResult.Mesh().Persistent().Faces().Triangulation(aCopiedCoEdgeDef.FaceId);
  ASSERT_FALSE(aCopiedTri.IsNull());
  EXPECT_NEAR(aCopiedTri->Node(2).X(), 1.0 + 5.0, Precision::Confusion());

  const occ::handle<Poly_PolygonOnTriangulation>& aPersistentPolygonOnTri =
    aResult.Mesh().Persistent().Edges().PolygonOnTriangulation(aResultEdgeId,
                                                               aCopiedCoEdgeDef.FaceId);
  ASSERT_FALSE(aPersistentPolygonOnTri.IsNull());
  const occ::handle<Poly_Triangulation>& aPersistentTri =
    aResult.Mesh().Persistent().Faces().Triangulation(aCopiedCoEdgeDef.FaceId);
  ASSERT_FALSE(aPersistentTri.IsNull());
  EXPECT_NEAR(aPersistentTri->Node(2).X(), 1.0 + 5.0, Precision::Confusion());
}

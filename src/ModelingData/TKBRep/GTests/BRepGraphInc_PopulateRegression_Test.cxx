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
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Bnd_Box.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
TopLoc_Location translationLocation(const double theX, const double theY, const double theZ)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(theX, theY, theZ));
  return TopLoc_Location(aTrsf);
}

Bnd_Box geometryBounds(const TopoDS_Shape& theShape)
{
  Bnd_Box aBox;
  BRepBndLib::AddOptimal(theShape, aBox, false, false);
  return aBox;
}

Bnd_Box meshBounds(const TopoDS_Shape& theShape)
{
  Bnd_Box aBox;
  BRepBndLib::Add(theShape, aBox, true);
  return aBox;
}

void expectBoxNear(const Bnd_Box& theActual, const Bnd_Box& theExpected, const double theTol)
{
  const Bnd_Box::Limits anActual   = theActual.Get();
  const Bnd_Box::Limits anExpected = theExpected.Get();
  EXPECT_NEAR(anActual.Xmin, anExpected.Xmin, theTol);
  EXPECT_NEAR(anActual.Ymin, anExpected.Ymin, theTol);
  EXPECT_NEAR(anActual.Zmin, anExpected.Zmin, theTol);
  EXPECT_NEAR(anActual.Xmax, anExpected.Xmax, theTol);
  EXPECT_NEAR(anActual.Ymax, anExpected.Ymax, theTol);
  EXPECT_NEAR(anActual.Zmax, anExpected.Zmax, theTol);
}

TopoDS_Shape firstSubShape(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  TopExp_Explorer anExp(theShape, theType);
  return anExp.More() ? anExp.Current() : TopoDS_Shape();
}

TopoDS_Face firstFaceOfBox(const double theSize)
{
  TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(theSize, theSize, theSize).Shape();
  TopoDS_Shape aFace = firstSubShape(aBox, TopAbs_FACE);
  return TopoDS::Face(aFace);
}

uint32_t countSourcePolygonOnTriangulation(const TopoDS_Shape& theShape,
                                           const bool          theUseFaceLocation,
                                           bool&               theHasLocatedTriangulation)
{
  uint32_t aCount = 0;
  for (TopExp_Explorer aFaceExp(theShape, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    const TopoDS_Face&                    aFace = TopoDS::Face(aFaceExp.Current());
    TopLoc_Location                       aTriLoc;
    const occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, aTriLoc);
    if (aTri.IsNull())
    {
      continue;
    }
    theHasLocatedTriangulation = theHasLocatedTriangulation || !aTriLoc.IsIdentity();

    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge&    anEdge     = TopoDS::Edge(anEdgeExp.Current());
      const TopLoc_Location aLookupLoc = theUseFaceLocation ? aTriLoc : TopLoc_Location();
      if (!BRep_Tool::PolygonOnTriangulation(anEdge, aTri, aLookupLoc).IsNull())
      {
        ++aCount;
      }
    }
  }
  return aCount;
}

uint32_t countGraphPolygonOnTriangulation(const BRepGraph& theGraph)
{
  uint32_t aCount = 0;
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(theGraph.Topo().CoEdges().Nb());
       ++aCoEdgeId)
  {
    if (theGraph.Topo().CoEdges().Definition(aCoEdgeId).PolygonOnTriRepId.IsValid())
    {
      ++aCount;
    }
  }
  return aCount;
}

TopoDS_Face makeNoWireNaturalSphereFace()
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(
    aFace,
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 10.0),
    Precision::Confusion());
  aBuilder.NaturalRestriction(aFace, true);
  return aFace;
}

uint32_t countDegeneratedEdges(const TopoDS_Shape& theShape)
{
  uint32_t aCount = 0;
  for (TopExp_Explorer anEdgeExp(theShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeExp.Current())))
    {
      ++aCount;
    }
  }
  return aCount;
}

TopoDS_Compound makeNestedCompound(const TopLoc_Location& theRootLocation,
                                   const TopLoc_Location& theInnerLocation,
                                   const TopLoc_Location& theSolidLocation)
{
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.2);
  EXPECT_TRUE(aMesher.IsDone());

  BRep_Builder       aBuilder;
  const TopoDS_Solid aMovedSolid = TopoDS::Solid(TopoDS::Solid(aBox).Moved(theSolidLocation));

  TopoDS_Compound anInner;
  aBuilder.MakeCompound(anInner);
  aBuilder.Add(anInner, aMovedSolid);
  const TopoDS_Compound aMovedInner = TopoDS::Compound(anInner.Moved(theInnerLocation));

  TopoDS_Compound aRoot;
  aBuilder.MakeCompound(aRoot);
  aBuilder.Add(aRoot, aMovedInner);
  return TopoDS::Compound(aRoot.Moved(theRootLocation));
}
} // namespace

TEST(BRepGraphInc_PopulateRegressionTest, LocatedFacePolygonOnTriangulation_IsCaptured)
{
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.5);
  ASSERT_TRUE(aMesher.IsDone());
  const TopoDS_Shape aMovedBox = aBox.Moved(translationLocation(5.0, 6.0, 7.0));

  bool           hasLocatedTriangulation = false;
  const uint32_t aSourceCount =
    countSourcePolygonOnTriangulation(aMovedBox, true, hasLocatedTriangulation);
  bool           hasLocatedTriangulationForIdentityProbe = false;
  const uint32_t anIdentityLookupCount =
    countSourcePolygonOnTriangulation(aMovedBox, false, hasLocatedTriangulationForIdentityProbe);

  ASSERT_TRUE(hasLocatedTriangulation);
  ASSERT_GT(aSourceCount, 0u);
  EXPECT_EQ(anIdentityLookupCount, 0u);

  BRepGraph aGraph;
  ASSERT_NE(BRepGraphInc_Populate::Perform(aGraph, aMovedBox, false),
            BRepGraphInc_Populate::BuildStatus::Failed);
  EXPECT_GT(countGraphPolygonOnTriangulation(aGraph), 0u);
}

TEST(BRepGraphInc_PopulateRegressionTest, NoWireNaturalSphere_DegenerateBoundaryEdgesRoundTrip)
{
  const TopoDS_Face aFace = makeNoWireNaturalSphereFace();

  BRepGraph aGraph;
  ASSERT_NE(BRepGraphInc_Populate::Perform(aGraph, aFace, false),
            BRepGraphInc_Populate::BuildStatus::Failed);

  uint32_t aDegenerateCount = 0;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeId))
    {
      ++aDegenerateCount;
      EXPECT_FALSE(aGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId.IsValid());
    }
  }
  ASSERT_GE(aDegenerateCount, 2u);

  const TopoDS_Shape aReconstructed =
    BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aReconstructed.IsNull());
  EXPECT_GE(countDegeneratedEdges(aReconstructed), 2u);
}

TEST(BRepGraphInc_PopulateRegressionTest, CompoundReferencesKeepDirectSourceLocationsAtEveryLevel)
{
  const TopLoc_Location aRootLoc   = translationLocation(0.0, 0.0, 11.0);
  const TopLoc_Location anInnerLoc = translationLocation(0.0, 7.0, 0.0);
  const TopLoc_Location aSolidLoc  = translationLocation(5.0, 0.0, 0.0);
  const TopoDS_Compound aMovedRoot = makeNestedCompound(aRootLoc, anInnerLoc, aSolidLoc);

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aMovedRoot);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_EQ(aResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Compound);
  ASSERT_TRUE(aResult.Product.IsValid());

  const BRepGraphInc::ProductRelations& aProductRelations =
    aGraph.Topo().Products().Relations(aResult.Product);
  ASSERT_EQ(aProductRelations.OccurrenceRefIds.Size(), 1u);
  const BRepGraphInc::OccurrenceRef& anOccurrenceRef =
    aGraph.Refs().Occurrences().Entry(aProductRelations.OccurrenceRefIds.Value(0));
  EXPECT_TRUE(anOccurrenceRef.LocalLocation.IsEqual(aRootLoc));

  const BRepGraph_CompoundId                            aRootCompound(aResult.TopologyRoot);
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aRootRefs =
    aGraph.Topo().Compounds().Relations(aRootCompound).ChildRefIds;
  ASSERT_EQ(aRootRefs.Size(), 1u);
  const BRepGraph_ChildRefId aRootChildRef = aRootRefs.Value(0);
  EXPECT_TRUE(aGraph.Refs().Gen().LocalLocation(aRootChildRef).IsEqual(anInnerLoc));

  const BRepGraph_NodeId anInnerNode = aGraph.Refs().Children().Entry(aRootChildRef).ChildNodeId;
  ASSERT_EQ(anInnerNode.NodeKind, BRepGraph_NodeId::Kind::Compound);
  const NCollection_LinearVector<BRepGraph_ChildRefId>& anInnerRefs =
    aGraph.Topo().Compounds().Relations(BRepGraph_CompoundId(anInnerNode)).ChildRefIds;
  ASSERT_EQ(anInnerRefs.Size(), 1u);
  EXPECT_TRUE(aGraph.Refs().Gen().LocalLocation(anInnerRefs.Value(0)).IsEqual(aSolidLoc));
}

TEST(BRepGraphInc_PopulateRegressionTest, ShapeViewFindNode_AcceptsOriginalPlacedSubshapes)
{
  const TopLoc_Location aRootLoc   = translationLocation(0.0, 0.0, 11.0);
  const TopLoc_Location anInnerLoc = translationLocation(0.0, 7.0, 0.0);
  const TopLoc_Location aSolidLoc  = translationLocation(5.0, 0.0, 0.0);
  const TopoDS_Compound aMovedRoot = makeNestedCompound(aRootLoc, anInnerLoc, aSolidLoc);

  const TopoDS_Shape anOriginalSolid = firstSubShape(aMovedRoot, TopAbs_SOLID);
  const TopoDS_Shape anOriginalFace  = firstSubShape(aMovedRoot, TopAbs_FACE);
  ASSERT_FALSE(anOriginalSolid.IsNull());
  ASSERT_FALSE(anOriginalFace.IsNull());

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aMovedRoot);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());
  ASSERT_TRUE(aResult.Product.IsValid());

  EXPECT_EQ(aGraph.Shapes().FindNode(aMovedRoot), aResult.TopologyRoot);
  EXPECT_EQ(aGraph.Shapes().FindNode(anOriginalSolid).NodeKind, BRepGraph_NodeId::Kind::Solid);
  EXPECT_EQ(aGraph.Shapes().FindNode(anOriginalFace).NodeKind, BRepGraph_NodeId::Kind::Face);

  const TopoDS_Shape aProductShape = aGraph.Shapes().Shape(aResult.Product);
  ASSERT_FALSE(aProductShape.IsNull());
  expectBoxNear(geometryBounds(aProductShape), geometryBounds(aMovedRoot), 1.0e-6);
  expectBoxNear(meshBounds(aProductShape), meshBounds(aMovedRoot), 1.0e-6);

  TopoDS_Shape aRootLocalShape = aMovedRoot;
  aRootLocalShape.Location(TopLoc_Location());
  const TopoDS_Shape aTopologyShape = aGraph.Shapes().Shape(aResult.TopologyRoot);
  ASSERT_FALSE(aTopologyShape.IsNull());
  expectBoxNear(geometryBounds(aTopologyShape), geometryBounds(aRootLocalShape), 1.0e-6);
}

TEST(BRepGraphInc_PopulateRegressionTest, Compact_PreservesAssemblyShapeLookupAndPlacement)
{
  const TopLoc_Location aRootLoc   = translationLocation(0.0, 0.0, 11.0);
  const TopLoc_Location anInnerLoc = translationLocation(0.0, 7.0, 0.0);
  const TopLoc_Location aSolidLoc  = translationLocation(5.0, 0.0, 0.0);
  const TopoDS_Compound aMovedRoot = makeNestedCompound(aRootLoc, anInnerLoc, aSolidLoc);

  const TopoDS_Shape anOriginalSolid = firstSubShape(aMovedRoot, TopAbs_SOLID);
  const TopoDS_Shape anOriginalFace  = firstSubShape(aMovedRoot, TopAbs_FACE);
  ASSERT_FALSE(anOriginalSolid.IsNull());
  ASSERT_FALSE(anOriginalFace.IsNull());

  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aMovedRoot);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());
  ASSERT_TRUE(aResult.Product.IsValid());

  BRepGraph::ShapesView::Options aNoProduct;
  aNoProduct.CreateAutoProduct = false;
  const BRepGraph::ShapesView::Result aJunk =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape(), aNoProduct);
  ASSERT_TRUE(aJunk.IsOk());
  ASSERT_TRUE(aJunk.TopologyRoot.IsValid());
  aGraph.Editor().Gen().RemoveSubgraph(aJunk.TopologyRoot);

  const BRepGraph_Compact::Result aCompact = BRepGraph_Compact::Perform(aGraph);
  ASSERT_GT(aCompact.NbNodesBefore, aCompact.NbNodesAfter);

  EXPECT_EQ(aGraph.Shapes().FindNode(aMovedRoot), aResult.TopologyRoot);
  EXPECT_EQ(aGraph.Shapes().FindNode(anOriginalSolid).NodeKind, BRepGraph_NodeId::Kind::Solid);
  EXPECT_EQ(aGraph.Shapes().FindNode(anOriginalFace).NodeKind, BRepGraph_NodeId::Kind::Face);

  const BRepGraph_CompoundId aRootCompound(aGraph.Shapes().FindNode(aMovedRoot));
  ASSERT_TRUE(aRootCompound.IsValid());
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aRootRefs =
    aGraph.Topo().Compounds().Relations(aRootCompound).ChildRefIds;
  ASSERT_EQ(aRootRefs.Size(), 1u);
  EXPECT_TRUE(aGraph.Refs().Gen().LocalLocation(aRootRefs.Value(0)).IsEqual(anInnerLoc));

  const BRepGraph_NodeId anInnerNode =
    aGraph.Refs().Children().Entry(aRootRefs.Value(0)).ChildNodeId;
  ASSERT_EQ(anInnerNode.NodeKind, BRepGraph_NodeId::Kind::Compound);
  const NCollection_LinearVector<BRepGraph_ChildRefId>& anInnerRefs =
    aGraph.Topo().Compounds().Relations(BRepGraph_CompoundId(anInnerNode)).ChildRefIds;
  ASSERT_EQ(anInnerRefs.Size(), 1u);
  EXPECT_TRUE(aGraph.Refs().Gen().LocalLocation(anInnerRefs.Value(0)).IsEqual(aSolidLoc));

  TopoDS_Shape aRootLocalShape = aMovedRoot;
  aRootLocalShape.Location(TopLoc_Location());
  const TopoDS_Shape aReconstructedRoot = BRepGraphInc_Reconstruct::Node(aGraph, aRootCompound);
  ASSERT_FALSE(aReconstructedRoot.IsNull());
  {
    SCOPED_TRACE("topology root reconstruction");
    expectBoxNear(geometryBounds(aReconstructedRoot), geometryBounds(aRootLocalShape), 1.0e-6);
  }

  const TopoDS_Shape aProductShape = aGraph.Shapes().Shape(aResult.Product);
  ASSERT_FALSE(aProductShape.IsNull());
  {
    SCOPED_TRACE("product reconstruction");
    expectBoxNear(geometryBounds(aProductShape), geometryBounds(aMovedRoot), 1.0e-6);
  }
}

TEST(BRepGraphInc_PopulateRegressionTest, Append_ReusesSameLocatedDefinitionAcrossCalls)
{
  const TopoDS_Shape aBox      = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  const TopoDS_Shape aMovedBox = aBox.Moved(translationLocation(5.0, 0.0, 0.0));

  BRepGraph aGraph;
  ASSERT_NE(BRepGraphInc_Populate::Append(aGraph, aMovedBox, false),
            BRepGraphInc_Populate::BuildStatus::Failed);
  const uint32_t         aNbSolidsAfterFirst = aGraph.Topo().Solids().Nb();
  const BRepGraph_NodeId aFirstNode          = aGraph.Shapes().FindNode(aMovedBox);
  ASSERT_TRUE(aFirstNode.IsValid());

  ASSERT_NE(BRepGraphInc_Populate::Append(aGraph, aMovedBox, false),
            BRepGraphInc_Populate::BuildStatus::Failed);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsAfterFirst);
  EXPECT_EQ(aGraph.Shapes().FindNode(aMovedBox), aFirstNode);

  const TopoDS_Shape aSecondPlacement = aBox.Moved(translationLocation(9.0, 0.0, 0.0));
  ASSERT_NE(BRepGraphInc_Populate::Append(aGraph, aSecondPlacement, false),
            BRepGraphInc_Populate::BuildStatus::Failed);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsAfterFirst + 1u);
  EXPECT_NE(aGraph.Shapes().FindNode(aSecondPlacement), aFirstNode);
}

TEST(BRepGraphInc_PopulateRegressionTest, PersistentTriangulationBounds_AgreeWithMovedNodes)
{
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.2);
  ASSERT_TRUE(aMesher.IsDone());
  for (TopExp_Explorer aFaceExp(aBox, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    TopLoc_Location                 aTriLoc;
    occ::handle<Poly_Triangulation> aTri =
      BRep_Tool::Triangulation(TopoDS::Face(aFaceExp.Current()), aTriLoc);
    if (!aTri.IsNull())
    {
      aTri->UpdateCachedMinMax();
    }
  }

  const TopoDS_Shape aMovedBox = aBox.Moved(translationLocation(25.0, -3.0, 8.0));

  BRepGraph aGraph;
  ASSERT_NE(BRepGraphInc_Populate::Perform(aGraph, aMovedBox, false),
            BRepGraphInc_Populate::BuildStatus::Failed);

  bool hasTriangulation = false;
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aGraph.Topo().Faces().Nb()); ++aFaceId)
  {
    const occ::handle<Poly_Triangulation>& aTri =
      aGraph.Mesh().Persistent().Faces().Triangulation(aFaceId);
    if (aTri.IsNull())
    {
      continue;
    }
    hasTriangulation = true;
    EXPECT_FALSE(aTri->HasCachedMinMax());
  }
  EXPECT_TRUE(hasTriangulation);

  const TopoDS_Shape aReconstructed =
    BRepGraphInc_Reconstruct::Node(aGraph, BRepGraph_SolidId::Start());
  ASSERT_FALSE(aReconstructed.IsNull());
  expectBoxNear(meshBounds(aReconstructed), meshBounds(aMovedBox), 1.0e-6);
}

TEST(BRepGraphInc_PopulateRegressionTest, Perform_ReplacesGraphThroughClearSemantics)
{
  BRepGraph aGraph;
  ASSERT_NE(
    BRepGraphInc_Populate::Perform(aGraph, BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape(), false),
    BRepGraphInc_Populate::BuildStatus::Failed);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  TopoDS_Vertex aVertex;
  BRep_Builder().MakeVertex(aVertex, gp_Pnt(42.0, 0.0, 0.0), Precision::Confusion());
  ASSERT_NE(aLayer->AddAttachment(BRepGraph_NodeId(BRepGraph_SolidId::Start()),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape,
                                  aVertex),
            0u);

  const BRepGraph_EdgeId      anEdgeId = BRepGraph_EdgeId::Start();
  occ::handle<Poly_Polygon3D> aPolygon = new Poly_Polygon3D(2, false);
  aGraph.Mesh().Editor().Edges().SetCachedPolygon3D(anEdgeId, aPolygon);
  ASSERT_NE(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr);

  ASSERT_NE(
    BRepGraphInc_Populate::Perform(aGraph, BRepPrimAPI_MakeBox(2.0, 2.0, 2.0).Shape(), false),
    BRepGraphInc_Populate::BuildStatus::Failed);
  EXPECT_TRUE(aLayer->AttachedTo(BRepGraph_NodeId(BRepGraph_SolidId::Start())).IsEmpty());
  EXPECT_EQ(aGraph.Mesh().Cache().Edges().Entry(anEdgeId), nullptr);
}

TEST(BRepGraphInc_PopulateRegressionTest, FlattenedTraversal_SkipsInternalExternalChildren)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Face aForwardFace = firstFaceOfBox(1.0);
  aForwardFace.Orientation(TopAbs_FORWARD);
  TopoDS_Face anInternalFace = firstFaceOfBox(2.0);
  anInternalFace.Orientation(TopAbs_INTERNAL);
  TopoDS_Face anExternalFace = firstFaceOfBox(3.0);
  anExternalFace.Orientation(TopAbs_EXTERNAL);

  aBuilder.Add(aCompound, aForwardFace);
  aBuilder.Add(aCompound, anInternalFace);
  aBuilder.Add(aCompound, anExternalFace);

  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct                 = false;
  anOptions.Flatten                           = true;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aCompound, anOptions);
  ASSERT_TRUE(aResult.IsOk());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1u);
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());
  EXPECT_EQ(aResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Face);
}

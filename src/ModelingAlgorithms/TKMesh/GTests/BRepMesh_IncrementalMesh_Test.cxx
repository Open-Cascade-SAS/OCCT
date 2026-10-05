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

#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <IMeshData_Status.hxx>
#include <IMeshTools_Parameters.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

// Test OCC26407: BRepMesh_Delaun must not fail on a planar polygon with frontier edges.
// The key check is that GetStatusFlags() == 0 (success) after meshing.
// Migrated from QABugs_19.cxx OCC26407
TEST(BRepMesh_IncrementalMeshTest, OCC26407_PlanarPolygonMeshStatus)
{
  // Hardcoded octagon-like polygon lying in the Z=88.5 plane
  NCollection_Array1<gp_Pnt> aPnts(1, 8);
  aPnts(1) = gp_Pnt(587.90000000000009094947, 40.6758179230516248026106, 88.5);
  aPnts(2) = gp_Pnt(807.824182076948432040808, 260.599999999999965893949, 88.5);
  aPnts(3) =
    gp_Pnt(644.174182076948454778176, 424.249999999999943156581, 88.5000000000000142108547);
  aPnts(4) = gp_Pnt(629.978025792618950617907, 424.25, 88.5);
  aPnts(5) = gp_Pnt(793.628025792618700506864, 260.599999999999852207111, 88.5);
  aPnts(6) = gp_Pnt(587.900000000000204636308, 54.8719742073813492311274, 88.5);
  aPnts(7) = gp_Pnt(218.521974207381418864315, 424.250000000000056843419, 88.5);
  aPnts(8) = gp_Pnt(204.325817923051886282337, 424.249999999999943156581, 88.5);

  NCollection_Array1<TopoDS_Vertex> aVertices(1, aPnts.Length());
  for (int anIndex = aPnts.Lower(); anIndex <= aPnts.Upper(); ++anIndex)
  {
    aVertices(anIndex) = BRepBuilderAPI_MakeVertex(aPnts(anIndex));
  }

  BRepBuilderAPI_MakeWire aWireBuilder;
  for (int anIndex = aVertices.Lower(); anIndex <= aVertices.Upper(); ++anIndex)
  {
    const TopoDS_Vertex& aV = aVertices(anIndex);
    const int aNextIndex    = anIndex == aVertices.Upper() ? aVertices.Lower() : anIndex + 1;
    const TopoDS_Vertex& aW = aVertices(aNextIndex);
    aWireBuilder.Add(BRepBuilderAPI_MakeEdge(aV, aW));
  }
  ASSERT_TRUE(aWireBuilder.IsDone()) << "Wire construction failed";

  const gp_Pnt& aV0         = aPnts(1);
  const gp_Pnt& aV1         = aPnts(2);
  const gp_Pnt& aV2         = aPnts(aPnts.Upper());
  const gp_Vec  aFaceNormal = gp_Vec(aV0, aV1).Crossed(gp_Vec(aV0, aV2));

  const TopoDS_Face aFace = BRepBuilderAPI_MakeFace(gp_Pln(aV0, aFaceNormal), aWireBuilder.Wire());

  BRepMesh_IncrementalMesh aMesher(aFace, 1.e-7);
  EXPECT_EQ(aMesher.GetStatusFlags(), 0)
    << "Meshing of the planar polygon face should succeed (status 0)";
}

TEST(BRepMesh_IncrementalMeshTest, TrimmedCylinder_RespectsUVRangeAndDeflection)
{
  constexpr double THE_RADIUS     = 5.0;
  constexpr double THE_FIRST_U    = 0.4;
  constexpr double THE_LAST_U     = 5.7;
  constexpr double THE_FIRST_V    = -2.0;
  constexpr double THE_LAST_V     = 7.0;
  constexpr double THE_DEFLECTION = 0.05;

  occ::handle<Geom_CylindricalSurface> aCylinder =
    new Geom_CylindricalSurface(gp_Ax3(gp::Origin(), gp::DZ()), THE_RADIUS);
  BRepBuilderAPI_MakeFace aFaceBuilder(aCylinder,
                                       THE_FIRST_U,
                                       THE_LAST_U,
                                       THE_FIRST_V,
                                       THE_LAST_V,
                                       Precision::Confusion());
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  BRepMesh_IncrementalMesh aMesher(aFace, THE_DEFLECTION);
  ASSERT_TRUE(aMesher.IsDone());

  TopLoc_Location                       aLocation;
  const occ::handle<Poly_Triangulation> aTriangulation = BRep_Tool::Triangulation(aFace, aLocation);
  ASSERT_FALSE(aTriangulation.IsNull());
  ASSERT_TRUE(aTriangulation->HasUVNodes());
  ASSERT_GT(aTriangulation->NbTriangles(), 0);

  for (int aNodeIdx = 1; aNodeIdx <= aTriangulation->NbNodes(); ++aNodeIdx)
  {
    const gp_Pnt2d aUV = aTriangulation->UVNode(aNodeIdx);
    EXPECT_GE(aUV.X(), THE_FIRST_U - Precision::PConfusion());
    EXPECT_LE(aUV.X(), THE_LAST_U + Precision::PConfusion());
    EXPECT_GE(aUV.Y(), THE_FIRST_V - Precision::PConfusion());
    EXPECT_LE(aUV.Y(), THE_LAST_V + Precision::PConfusion());

    const gp_Pnt anExpectedPoint = aCylinder->Value(aUV.X(), aUV.Y());
    const gp_Pnt anActualPoint   = aTriangulation->Node(aNodeIdx).Transformed(aLocation);
    EXPECT_LE(anActualPoint.Distance(anExpectedPoint), Precision::Confusion());
  }

  for (int aTriangleIdx = 1; aTriangleIdx <= aTriangulation->NbTriangles(); ++aTriangleIdx)
  {
    int aNode1 = 0;
    int aNode2 = 0;
    int aNode3 = 0;
    aTriangulation->Triangle(aTriangleIdx).Get(aNode1, aNode2, aNode3);

    const gp_Pnt2d aUV1            = aTriangulation->UVNode(aNode1);
    const gp_Pnt2d aUV2            = aTriangulation->UVNode(aNode2);
    const gp_Pnt2d aUV3            = aTriangulation->UVNode(aNode3);
    const gp_Pnt   anExpectedPoint = aCylinder->Value((aUV1.X() + aUV2.X() + aUV3.X()) / 3.0,
                                                    (aUV1.Y() + aUV2.Y() + aUV3.Y()) / 3.0);
    const gp_Pnt   aPoint1         = aTriangulation->Node(aNode1).Transformed(aLocation);
    const gp_Pnt   aPoint2         = aTriangulation->Node(aNode2).Transformed(aLocation);
    const gp_Pnt   aPoint3         = aTriangulation->Node(aNode3).Transformed(aLocation);
    const gp_Pnt   anActualPoint((aPoint1.XYZ() + aPoint2.XYZ() + aPoint3.XYZ()) / 3.0);
    EXPECT_LE(anActualPoint.Distance(anExpectedPoint), THE_DEFLECTION);
  }
}

TEST(BRepMesh_IncrementalMeshTest, BooleanTrimmedTorus_CrossingSeamProducesDenseMesh)
{
  BRepPrimAPI_MakeBox aBoxBuilder(gp_Pnt(-187.0, -20.0, -67.5), gp_Pnt(187.0, 0.0, 67.5));
  const TopoDS_Shape  aBox = aBoxBuilder.Shape();
  ASSERT_TRUE(aBoxBuilder.IsDone());

  const gp_Circ           aCircle(gp_Ax2(gp_Pnt(200.0, -20.0, 0.0), gp_Dir(0.0, -1.0, 0.0)), 17.5);
  BRepBuilderAPI_MakeEdge anEdgeBuilder(aCircle);
  const TopoDS_Edge       anEdge = anEdgeBuilder.Edge();
  ASSERT_TRUE(anEdgeBuilder.IsDone());

  BRepBuilderAPI_MakeWire aWireBuilder(anEdge);
  const TopoDS_Wire       aWire = aWireBuilder.Wire();
  ASSERT_TRUE(aWireBuilder.IsDone());

  BRepBuilderAPI_MakeFace aFaceBuilder(aWire, true);
  const TopoDS_Face       aProfile = aFaceBuilder.Face();
  ASSERT_TRUE(aFaceBuilder.IsDone());

  const gp_Ax1          anAxis(gp_Pnt(0.0, -20.0, 0.0), gp_Dir(0.0, 0.0, -1.0));
  BRepPrimAPI_MakeRevol aRingBuilder(aProfile, anAxis, 2.0 * M_PI);
  const TopoDS_Shape    aRing = aRingBuilder.Shape();
  ASSERT_TRUE(aRingBuilder.IsDone());

  BRepAlgoAPI_Fuse aFuse(aBox, aRing);
  aFuse.Build();
  ASSERT_TRUE(aFuse.IsDone());

  const TopoDS_Shape aFused = aFuse.Shape();
  ASSERT_FALSE(aFused.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aFused).IsValid());

  IMeshTools_Parameters aParameters;
  aParameters.Deflection = 0.1;
  aParameters.Angle      = 0.4;
  BRepMesh_IncrementalMesh aMesher(aFused, aParameters);
  ASSERT_TRUE(aMesher.IsDone());

  int aTorusFaces     = 0;
  int aTorusNodes     = 0;
  int aTorusTriangles = 0;
  for (TopExp_Explorer anExplorer(aFused, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Face&  aFace = TopoDS::Face(anExplorer.Current());
    BRepAdaptor_Surface aSurface(aFace);
    if (aSurface.GetType() != GeomAbs_Torus)
    {
      continue;
    }

    ++aTorusFaces;
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(aFace, aLocation);
    ASSERT_FALSE(aTriangulation.IsNull());
    aTorusNodes += aTriangulation->NbNodes();
    aTorusTriangles += aTriangulation->NbTriangles();
  }

  ASSERT_GT(aTorusFaces, 0);
  // The regression produced fewer than 450 triangles for the whole fused shape.
  EXPECT_GT(aTorusNodes, 1000);
  EXPECT_GT(aTorusTriangles, 1000);
}

TEST(BRepMesh_IncrementalMeshTest, ClosedCylinder_SeamPolygonsReferenceValidMeshNodes)
{
  BRepPrimAPI_MakeCylinder aCylinderBuilder(5.0, 10.0);
  const TopoDS_Shape       aCylinder = aCylinderBuilder.Shape();
  ASSERT_TRUE(aCylinderBuilder.IsDone());

  BRepMesh_IncrementalMesh aMesher(aCylinder, 0.1);
  ASSERT_TRUE(aMesher.IsDone());

  int aSeamEdges = 0;
  for (TopExp_Explorer aFaceExplorer(aCylinder, TopAbs_FACE); aFaceExplorer.More();
       aFaceExplorer.Next())
  {
    const TopoDS_Face&                    aFace = TopoDS::Face(aFaceExplorer.Current());
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(aFace, aLocation);
    ASSERT_FALSE(aTriangulation.IsNull());

    for (TopExp_Explorer anEdgeExplorer(aFace, TopAbs_EDGE); anEdgeExplorer.More();
         anEdgeExplorer.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExplorer.Current());
      if (!BRep_Tool::IsClosed(anEdge, aFace))
      {
        continue;
      }

      const occ::handle<Poly_PolygonOnTriangulation>& aForwardPolygon =
        BRep_Tool::PolygonOnTriangulation(anEdge, aTriangulation, aLocation);
      const TopoDS_Edge aReversedEdge = TopoDS::Edge(anEdge.Reversed());
      const occ::handle<Poly_PolygonOnTriangulation>& aReversedPolygon =
        BRep_Tool::PolygonOnTriangulation(aReversedEdge, aTriangulation, aLocation);
      ASSERT_FALSE(aForwardPolygon.IsNull());
      ASSERT_FALSE(aReversedPolygon.IsNull());

      for (int aNodeIndex = 1; aNodeIndex <= aForwardPolygon->NbNodes(); ++aNodeIndex)
      {
        EXPECT_GE(aForwardPolygon->Node(aNodeIndex), 1);
        EXPECT_LE(aForwardPolygon->Node(aNodeIndex), aTriangulation->NbNodes());
      }
      for (int aNodeIndex = 1; aNodeIndex <= aReversedPolygon->NbNodes(); ++aNodeIndex)
      {
        EXPECT_GE(aReversedPolygon->Node(aNodeIndex), 1);
        EXPECT_LE(aReversedPolygon->Node(aNodeIndex), aTriangulation->NbNodes());
      }

      ++aSeamEdges;
    }
  }

  EXPECT_GT(aSeamEdges, 0);
}

// Migrated from tests/bugs/mesh/bug32692_3.  An unbounded cylindrical face
// must be handled without a crash and must report the expected mesh status.
TEST(BRepMesh_IncrementalMeshTest, OCC32692_UnboundedCylinderFaceReportsFailure)
{
  const occ::handle<Geom_CylindricalSurface> aCylinder =
    new Geom_CylindricalSurface(gp::XOY(), 10.0);
  BRepBuilderAPI_MakeFace aFaceBuilder(aCylinder, Precision::Confusion());
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  BRepMesh_IncrementalMesh aMesher(aFace, 0.01, false, 0.5, true);
  EXPECT_TRUE(aMesher.IsDone());
  EXPECT_EQ(aMesher.GetStatusFlags(), IMeshData_OpenWire | IMeshData_Failure | IMeshData_Outdated);

  int aNbFaces = 0;
  for (TopExp_Explorer anExplorer(aFace, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(aNbFaces, 1);

  TopLoc_Location                       aLocation;
  const occ::handle<Poly_Triangulation> aTriangulation = BRep_Tool::Triangulation(aFace, aLocation);
  int                                   aNbNodes       = 0;
  int                                   aNbTriangles   = 0;
  if (!aTriangulation.IsNull())
  {
    aNbNodes     = aTriangulation->NbNodes();
    aNbTriangles = aTriangulation->NbTriangles();
  }
  EXPECT_EQ(aNbNodes, 0);
  EXPECT_EQ(aNbTriangles, 0);
}

// Migrated from tests/bugs/demo/bug25445.  The DRAW test verifies that the
// angular deflection is forwarded to BRepMesh by comparing the two meshes.
TEST(BRepMesh_IncrementalMeshTest, OCC25445_AngularDeflectionChangesMesh)
{
  struct MeshCounts
  {
    int NbNodes;
    int NbTriangles;
  };

  const auto countMeshElements = [](const TopoDS_Shape& theShape, const double theAngleDegrees) {
    IMeshTools_Parameters aParameters;
    aParameters.Deflection = 0.01;
    aParameters.Angle      = theAngleDegrees * M_PI / 180.0;

    BRepMesh_IncrementalMesh aMesher(theShape, aParameters);
    EXPECT_TRUE(aMesher.IsDone());

    int aNbNodes     = 0;
    int aNbTriangles = 0;
    for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
    {
      TopLoc_Location                       aLocation;
      const occ::handle<Poly_Triangulation> aTriangulation =
        BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
      EXPECT_FALSE(aTriangulation.IsNull());
      if (!aTriangulation.IsNull())
      {
        aNbNodes += aTriangulation->NbNodes();
        aNbTriangles += aTriangulation->NbTriangles();
      }
    }
    return MeshCounts{aNbNodes, aNbTriangles};
  };

  const TopoDS_Shape aCoarseAngleShape = BRepPrimAPI_MakeCone(100.0, 10.0, 100.0).Shape();
  const TopoDS_Shape aFineAngleShape   = BRepPrimAPI_MakeCone(100.0, 10.0, 100.0).Shape();

  const MeshCounts aCoarseMesh = countMeshElements(aCoarseAngleShape, 10.0);
  const MeshCounts aFineMesh   = countMeshElements(aFineAngleShape, 1.0);

  EXPECT_NE(aCoarseMesh.NbNodes, aFineMesh.NbNodes);
  EXPECT_NE(aCoarseMesh.NbTriangles, aFineMesh.NbTriangles);
}

// Migrated from tests/bugs/mesh/bug30234.  A zero-radius free edge is a
// legitimate degenerate input and must not acquire a face triangulation.
TEST(BRepMesh_IncrementalMeshTest, OCC30234_ZeroLengthFreeEdgeHasNoTriangles)
{
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp::Origin(), gp::DZ()), 0.0);
  BRepBuilderAPI_MakeEdge        aEdgeBuilder(aCircle);
  ASSERT_TRUE(aEdgeBuilder.IsDone());

  IMeshTools_Parameters aParameters;
  aParameters.Deflection = 0.005;
  aParameters.Relative   = true;
  BRepMesh_IncrementalMesh aMesher(aEdgeBuilder.Edge(), aParameters);
  EXPECT_TRUE(aMesher.IsDone());

  int aNbNodes     = 0;
  int aNbTriangles = 0;
  for (TopExp_Explorer anExplorer(aEdgeBuilder.Edge(), TopAbs_FACE); anExplorer.More();
       anExplorer.Next())
  {
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
    ASSERT_FALSE(aTriangulation.IsNull());
    aNbNodes += aTriangulation->NbNodes();
    aNbTriangles += aTriangulation->NbTriangles();
  }
  EXPECT_EQ(aNbNodes, 0);
  EXPECT_EQ(aNbTriangles, 0);
}

// Migrated from tests/bugs/mesh/bug31125.  Meshing an empty compound is a
// no-op and must complete without dereferencing a missing face.
TEST(BRepMesh_IncrementalMeshTest, OCC31125_EmptyCompoundDoesNotCrash)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound anEmptyCompound;
  aBuilder.MakeCompound(anEmptyCompound);

  IMeshTools_Parameters aParameters;
  aParameters.Deflection = 1.0;
  EXPECT_NO_THROW(BRepMesh_IncrementalMesh(anEmptyCompound, aParameters));
}

// Migrated from tests/bugs/mesh/bug31461.  A second pass with
// AllowQualityDecrease must replace the fine sphere mesh with the requested
// coarser one.
TEST(BRepMesh_IncrementalMeshTest, OCC31461_AllowQualityDecreaseRebuildsMesh)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  struct MeshCounts
  {
    int NbNodes;
    int NbTriangles;
  };

  const auto countMeshElements = [](const TopoDS_Shape& theShape) {
    int aNbNodes     = 0;
    int aNbTriangles = 0;
    for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
    {
      TopLoc_Location                       aLocation;
      const occ::handle<Poly_Triangulation> aTriangulation =
        BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
      EXPECT_FALSE(aTriangulation.IsNull());
      if (!aTriangulation.IsNull())
      {
        aNbNodes += aTriangulation->NbNodes();
        aNbTriangles += aTriangulation->NbTriangles();
      }
    }
    return MeshCounts{aNbNodes, aNbTriangles};
  };

  IMeshTools_Parameters aFineParameters;
  aFineParameters.Deflection = 0.01;
  BRepMesh_IncrementalMesh aFineMesher(aSphere, aFineParameters);
  ASSERT_TRUE(aFineMesher.IsDone());
  const MeshCounts aFineMesh = countMeshElements(aSphere);
  EXPECT_EQ(aFineMesh.NbNodes, 5106);
  EXPECT_EQ(aFineMesh.NbTriangles, 10108);

  IMeshTools_Parameters aCoarseParameters;
  aCoarseParameters.Deflection           = 0.1;
  aCoarseParameters.AllowQualityDecrease = true;
  BRepMesh_IncrementalMesh aCoarseMesher(aSphere, aCoarseParameters);
  ASSERT_TRUE(aCoarseMesher.IsDone());
  const MeshCounts aCoarseMesh = countMeshElements(aSphere);
  EXPECT_LT(aCoarseMesh.NbNodes, aFineMesh.NbNodes);
  EXPECT_LT(aCoarseMesh.NbTriangles, aFineMesh.NbTriangles);

  BRepTools::CleanGeometry(aSphere);
  IMeshTools_Parameters aFineAfterCleanParameters;
  aFineAfterCleanParameters.Deflection = 0.01;
  BRepMesh_IncrementalMesh aFineAfterCleanMesher(aSphere, aFineAfterCleanParameters);
  ASSERT_TRUE(aFineAfterCleanMesher.IsDone());

  BRepTools::Clean(aSphere);
  const MeshCounts aAfterCleanMesh = countMeshElements(aSphere);
  EXPECT_EQ(aAfterCleanMesh.NbNodes, aCoarseMesh.NbNodes);
  EXPECT_EQ(aAfterCleanMesh.NbTriangles, aCoarseMesh.NbTriangles);
}

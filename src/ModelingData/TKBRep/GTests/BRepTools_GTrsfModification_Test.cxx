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
#include <BRepTools_GTrsfModification.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom2d_Line.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
occ::handle<Poly_Triangulation> makeTriangulation(const bool theHasNormals)
{
  occ::handle<Poly_Triangulation> aTriangulation =
    new Poly_Triangulation(3, 1, false, theHasNormals);
  aTriangulation->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTriangulation->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTriangulation->SetNode(3, gp_Pnt(0.0, 1.0, 1.0));
  aTriangulation->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aTriangulation->Deflection(0.25);
  if (theHasNormals)
  {
    const gp_Dir aNormal(0.0, -1.0, 1.0);
    for (int anIndex = 1; anIndex <= 3; ++anIndex)
    {
      aTriangulation->SetNormal(anIndex, aNormal);
    }
  }
  return aTriangulation;
}

gp_GTrsf makeGTrsf(const gp_Mat& theMatrix)
{
  gp_GTrsf aGTrsf;
  aGTrsf.SetVectorialPart(theMatrix);
  return aGTrsf;
}

void expectPointNear(const gp_Pnt& theActual, const gp_Pnt& theExpected)
{
  EXPECT_LT(theActual.Distance(theExpected), 1.0e-12);
}
} // namespace

TEST(BRepTools_GTrsfModificationTest, Triangulation_ReflectionPreservesWindingAndNormals)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Poly_Triangulation> aSource = makeTriangulation(true);
  aSource->UpdateCachedMinMax();
  ASSERT_TRUE(aSource->HasCachedMinMax());
  aBuilder.MakeFace(aFace, aSource);

  const gp_GTrsf aGTrsf = makeGTrsf(gp_Mat(-2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0));
  BRepTools_GTrsfModification     aModification(aGTrsf);
  occ::handle<Poly_Triangulation> aResult;
  ASSERT_TRUE(aModification.NewTriangulation(aFace, aResult));
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_FALSE(aResult->HasCachedMinMax());
  EXPECT_TRUE(aSource->HasCachedMinMax());

  int aNode1 = 0, aNode2 = 0, aNode3 = 0;
  aResult->Triangle(1).Get(aNode1, aNode2, aNode3);
  EXPECT_EQ(aNode1, 1);
  EXPECT_EQ(aNode2, 3);
  EXPECT_EQ(aNode3, 2);

  const gp_Dir anExpectedNormal(0.0, -1.0 / 3.0, 1.0 / 4.0);
  for (int anIndex = 1; anIndex <= 3; ++anIndex)
  {
    EXPECT_LT(aResult->Normal(anIndex).Angle(anExpectedNormal), 1.0e-6);
  }
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 1.0);
  EXPECT_DOUBLE_EQ(aSource->Deflection(), 0.25);
  expectPointNear(aSource->Node(2), gp_Pnt(1.0, 0.0, 0.0));
}

TEST(BRepTools_GTrsfModificationTest, Triangulation_NegativeUniformScalePreservesOrientation)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Poly_Triangulation> aSource = makeTriangulation(true);
  aBuilder.MakeFace(aFace, aSource);

  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(0.0, 0.0, 0.0), -2.0);
  BRepTools_GTrsfModification     aModification{gp_GTrsf(aScale)};
  occ::handle<Poly_Triangulation> aResult;
  ASSERT_TRUE(aModification.NewTriangulation(aFace, aResult));

  int aNode1 = 0, aNode2 = 0, aNode3 = 0;
  aResult->Triangle(1).Get(aNode1, aNode2, aNode3);
  EXPECT_EQ(aNode1, 1);
  EXPECT_EQ(aNode2, 3);
  EXPECT_EQ(aNode3, 2);

  const gp_Dir anExpectedNormal(0.0, 1.0, -1.0);
  EXPECT_LT(aResult->Normal(1).Angle(anExpectedNormal), 1.0e-6);
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 0.5);
}

TEST(BRepTools_GTrsfModificationTest, Triangulation_NormalTransformationIsScaleInvariant)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Poly_Triangulation> aSource = makeTriangulation(true);
  aBuilder.MakeFace(aFace, aSource);

  const double aScales[] = {Precision::PConfusion(), Precision::Infinite()};
  for (const double aScale : aScales)
  {
    BRepTools_GTrsfModification aModification{
      makeGTrsf(gp_Mat(aScale, 0.0, 0.0, 0.0, aScale, 0.0, 0.0, 0.0, aScale))};
    occ::handle<Poly_Triangulation> aResult;
    ASSERT_TRUE(aModification.NewTriangulation(aFace, aResult));
    EXPECT_LT(aResult->Normal(1).Angle(aSource->Normal(1)), 1.0e-6);
  }
}

TEST(BRepTools_GTrsfModificationTest, Triangulation_SingularAffinityHandlesNormals)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Poly_Triangulation> aSource = makeTriangulation(true);
  aBuilder.MakeFace(aFace, aSource);

  BRepTools_GTrsfModification aRankTwoModification{
    makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0))};
  occ::handle<Poly_Triangulation> aResult;
  ASSERT_TRUE(aRankTwoModification.NewTriangulation(aFace, aResult));
  ASSERT_TRUE(aResult->HasNormals());
  EXPECT_LT(aResult->Normal(1).Angle(gp_Dir(0.0, 0.0, 1.0)), 1.0e-6);

  BRepTools_GTrsfModification aRankOneModification{
    makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0))};
  ASSERT_TRUE(aRankOneModification.NewTriangulation(aFace, aResult));
  ASSERT_TRUE(aResult->HasNormals());
  EXPECT_LT(aResult->Normal(1).Angle(aSource->Normal(1)), 1.0e-6);
}

TEST(BRepTools_GTrsfModificationTest, Surface_LocatedBezierTransformsEvaluatedGeometry)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles(1, 1)                                   = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2, 1)                                   = gp_Pnt(2.0, 0.0, 1.0);
  aPoles(1, 2)                                   = gp_Pnt(0.0, 3.0, 1.0);
  aPoles(2, 2)                                   = gp_Pnt(2.0, 3.0, 2.0);
  const occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aPoles);

  gp_Trsf aLocationTrsf;
  aLocationTrsf.SetTranslation(gp_Vec(4.0, -2.0, 5.0));
  const TopLoc_Location aLocation(aLocationTrsf);
  BRep_Builder          aBuilder;
  TopoDS_Face           aFace;
  aBuilder.MakeFace(aFace, aSurface, aLocation, 0.2);

  const gp_GTrsf              aGTrsf(gp_Mat(2.0, 0.5, 0.0, 0.0, 3.0, 0.25, 0.0, 0.0, 4.0),
                                     gp_XYZ(7.0, 8.0, 9.0));
  BRepTools_GTrsfModification aModification(aGTrsf);
  occ::handle<Geom_Surface>   aResult;
  TopLoc_Location             aResultLocation;
  double                      aTolerance     = 0.0;
  bool                        toReverseWires = true;
  bool                        toReverseFace  = true;
  ASSERT_TRUE(
    aModification
      .NewSurface(aFace, aResult, aResultLocation, aTolerance, toReverseWires, toReverseFace));
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(aResultLocation.IsIdentity());
  EXPECT_FALSE(toReverseWires);
  EXPECT_FALSE(toReverseFace);

  gp_Pnt anExpected = aSurface->Value(0.35, 0.65).Transformed(aLocationTrsf);
  aGTrsf.Transforms(anExpected.ChangeCoord());
  expectPointNear(aResult->Value(0.35, 0.65), anExpected);
  expectPointNear(aSurface->Pole(2, 2), gp_Pnt(2.0, 3.0, 2.0));
  EXPECT_DOUBLE_EQ(aTolerance, 0.8);
}

TEST(BRepTools_GTrsfModificationTest, Curve_LocatedBezierPreservesRangeAndSource)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1)                                  = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2)                                  = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3)                                  = gp_Pnt(3.0, 2.0, 1.0);
  const occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);

  gp_Trsf aLocationTrsf;
  aLocationTrsf.SetTranslation(gp_Vec(-3.0, 4.0, 2.0));
  const TopLoc_Location aLocation(aLocationTrsf);
  BRep_Builder          aBuilder;
  TopoDS_Edge           anEdge;
  aBuilder.MakeEdge(anEdge, aCurve, aLocation, 0.1);
  aBuilder.Range(anEdge, 0.2, 0.8);

  const gp_GTrsf              aGTrsf(gp_Mat(1.0, 0.5, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 3.0),
                                     gp_XYZ(2.0, -1.0, 5.0));
  BRepTools_GTrsfModification aModification(aGTrsf);
  occ::handle<Geom_Curve>     aResult;
  TopLoc_Location             aResultLocation;
  double                      aTolerance = 0.0;
  ASSERT_TRUE(aModification.NewCurve(anEdge, aResult, aResultLocation, aTolerance));
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(aResultLocation.IsIdentity());
  EXPECT_DOUBLE_EQ(aResult->FirstParameter(), 0.2);
  EXPECT_DOUBLE_EQ(aResult->LastParameter(), 0.8);

  gp_Pnt anExpected = aCurve->Value(0.4).Transformed(aLocationTrsf);
  aGTrsf.Transforms(anExpected.ChangeCoord());
  expectPointNear(aResult->Value(0.4), anExpected);
  expectPointNear(aCurve->Pole(2), gp_Pnt(1.0, 2.0, 0.0));
}

TEST(BRepTools_GTrsfModificationTest, Polygon3D_RepresentationLocationTransformsToEdgeFrame)
{
  NCollection_Array1<gp_Pnt> aNodes(1, 2);
  aNodes(1) = gp_Pnt(1.0, 2.0, 3.0);
  aNodes(2) = gp_Pnt(4.0, 5.0, 6.0);
  NCollection_Array1<double> aParameters(1, 2);
  aParameters(1)                             = 0.25;
  aParameters(2)                             = 0.75;
  const occ::handle<Poly_Polygon3D> aPolygon = new Poly_Polygon3D(aNodes, aParameters);
  aPolygon->Deflection(0.5);

  gp_Trsf aRepresentationTrsf;
  aRepresentationTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf anEdgeTrsf;
  anEdgeTrsf.SetTranslation(gp_Vec(0.0, 20.0, 0.0));
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.UpdateEdge(anEdge, aPolygon, TopLoc_Location(aRepresentationTrsf));
  anEdge.Location(TopLoc_Location(anEdgeTrsf));

  const gp_GTrsf aGTrsf = makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0));
  BRepTools_GTrsfModification aModification(aGTrsf);
  occ::handle<Poly_Polygon3D> aResult;
  ASSERT_TRUE(aModification.NewPolygon(anEdge, aResult));
  ASSERT_FALSE(aResult.IsNull());

  TopLoc_Location aSourceLocation;
  ASSERT_FALSE(BRep_Tool::Polygon3D(anEdge, aSourceLocation).IsNull());
  for (int anIndex = 1; anIndex <= aResult->NbNodes(); ++anIndex)
  {
    gp_Pnt anExpected = aPolygon->Nodes()(anIndex).Transformed(aSourceLocation.Transformation());
    aGTrsf.Transforms(anExpected.ChangeCoord());
    const gp_Pnt anActual =
      aResult->Nodes()(anIndex).Transformed(anEdge.Location().Transformation());
    expectPointNear(anActual, anExpected);
    EXPECT_DOUBLE_EQ(aResult->Parameters()(anIndex), aParameters(anIndex));
  }
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 2.0);
  EXPECT_DOUBLE_EQ(aPolygon->Deflection(), 0.5);
}

TEST(BRepTools_GTrsfModificationTest, MissingPolygonRepresentationsReturnFalse)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  TopoDS_Face  aFace;
  aBuilder.MakeEdge(anEdge);
  const occ::handle<Poly_Triangulation> aTriangulation = makeTriangulation(false);
  aBuilder.MakeFace(aFace, aTriangulation);

  BRepTools_GTrsfModification              aModification{gp_GTrsf()};
  occ::handle<Poly_Polygon3D>              aPolygon3d = new Poly_Polygon3D(2, false);
  occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTriangulation =
    new Poly_PolygonOnTriangulation(2, false);
  EXPECT_FALSE(aModification.NewPolygon(anEdge, aPolygon3d));
  EXPECT_TRUE(aPolygon3d.IsNull());
  EXPECT_FALSE(aModification.NewPolygonOnTriangulation(anEdge, aFace, aPolygonOnTriangulation));
  EXPECT_TRUE(aPolygonOnTriangulation.IsNull());
}

TEST(BRepTools_GTrsfModificationTest, Curve2dAndVertexParameterPreserveParametrization)
{
  NCollection_Array2<gp_Pnt> aSurfacePoles(1, 2, 1, 2);
  aSurfacePoles(1, 1)                            = gp_Pnt(0.0, 0.0, 0.0);
  aSurfacePoles(2, 1)                            = gp_Pnt(1.0, 0.0, 0.0);
  aSurfacePoles(1, 2)                            = gp_Pnt(0.0, 1.0, 0.0);
  aSurfacePoles(2, 2)                            = gp_Pnt(1.0, 1.0, 0.0);
  const occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aSurfacePoles);

  NCollection_Array1<gp_Pnt> aCurvePoles(1, 2);
  aCurvePoles(1)                             = gp_Pnt(0.0, 0.0, 0.0);
  aCurvePoles(2)                             = gp_Pnt(1.0, 0.0, 0.0);
  const occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aCurvePoles);

  BRep_Builder  aBuilder;
  TopoDS_Face   aFace;
  TopoDS_Edge   anEdge;
  TopoDS_Vertex aVertex;
  aBuilder.MakeFace(aFace, aSurface, 0.05);
  aBuilder.MakeEdge(anEdge, aCurve, 0.05);
  aBuilder.Range(anEdge, 0.2, 0.8);
  const occ::handle<Geom2d_Line> aPcurve =
    new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
  aBuilder.UpdateEdge(anEdge, aPcurve, aFace, 0.05);
  aBuilder.Range(anEdge, aFace, 0.2, 0.8);
  aBuilder.MakeVertex(aVertex, aCurve->Value(0.25), 0.02);
  aBuilder.Add(anEdge, aVertex);
  aBuilder.UpdateVertex(aVertex, 0.25, anEdge, 0.02);

  double aSourceFirst = 0.0, aSourceLast = 0.0;
  ASSERT_FALSE(BRep_Tool::CurveOnSurface(anEdge, aFace, aSourceFirst, aSourceLast).IsNull());

  const gp_GTrsf aGTrsf = makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 1.0));
  BRepTools_GTrsfModification aModification(aGTrsf);
  occ::handle<Geom2d_Curve>   aResultCurve;
  double                      aTolerance = 0.0;
  ASSERT_TRUE(aModification
                .NewCurve2d(anEdge, aFace, TopoDS_Edge(), TopoDS_Face(), aResultCurve, aTolerance));
  ASSERT_FALSE(aResultCurve.IsNull());
  EXPECT_DOUBLE_EQ(aResultCurve->FirstParameter(), aSourceFirst);
  EXPECT_DOUBLE_EQ(aResultCurve->LastParameter(), aSourceLast);
  EXPECT_LT(aResultCurve->Value(0.4).Distance(aPcurve->Value(0.4)), 1.0e-12);
  EXPECT_DOUBLE_EQ(aTolerance, 0.15);

  double aParameter = 0.0;
  ASSERT_TRUE(aModification.NewParameter(aVertex, anEdge, aParameter, aTolerance));
  EXPECT_DOUBLE_EQ(aParameter, 0.25);
  EXPECT_DOUBLE_EQ(aTolerance, 0.06);
}

TEST(BRepTools_GTrsfModificationTest, Modifier_LocatedSurfaceAndMeshStayCoincident)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles(1, 1)                                         = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2, 1)                                         = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(1, 2)                                         = gp_Pnt(0.0, 1.0, 1.0);
  aPoles(2, 2)                                         = gp_Pnt(1.0, 1.0, 1.0);
  const occ::handle<Geom_BezierSurface> aSurface       = new Geom_BezierSurface(aPoles);
  const occ::handle<Poly_Triangulation> aTriangulation = makeTriangulation(false);

  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace, aSurface, 0.1);
  aBuilder.UpdateFace(aFace, aTriangulation);
  gp_Trsf aLocationTrsf;
  aLocationTrsf.SetTranslation(gp_Vec(10.0, 20.0, 30.0));
  aFace.Location(TopLoc_Location(aLocationTrsf));

  const gp_GTrsf aGTrsf(gp_Mat(2.0, 0.5, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0), gp_XYZ(5.0, 6.0, 7.0));
  occ::handle<BRepTools_GTrsfModification> aModification = new BRepTools_GTrsfModification(aGTrsf);
  BRepTools_Modifier                       aModifier(aFace, aModification);
  ASSERT_TRUE(aModifier.IsDone());
  const TopoDS_Face aResultFace = TopoDS::Face(aModifier.ModifiedShape(aFace));
  ASSERT_FALSE(aResultFace.IsNull());

  TopLoc_Location           aResultSurfaceLocation;
  occ::handle<Geom_Surface> aResultSurface =
    BRep_Tool::Surface(aResultFace, aResultSurfaceLocation);
  ASSERT_FALSE(aResultSurface.IsNull());
  gp_Pnt anExpectedSurface = aSurface->Value(0.25, 0.75).Transformed(aLocationTrsf);
  aGTrsf.Transforms(anExpectedSurface.ChangeCoord());
  expectPointNear(
    aResultSurface->Value(0.25, 0.75).Transformed(aResultSurfaceLocation.Transformation()),
    anExpectedSurface);

  TopLoc_Location                       aResultMeshLocation;
  const occ::handle<Poly_Triangulation> aResultTriangulation =
    BRep_Tool::Triangulation(aResultFace, aResultMeshLocation);
  ASSERT_FALSE(aResultTriangulation.IsNull());
  for (int anIndex = 1; anIndex <= aTriangulation->NbNodes(); ++anIndex)
  {
    gp_Pnt anExpectedNode = aTriangulation->Node(anIndex).Transformed(aLocationTrsf);
    aGTrsf.Transforms(anExpectedNode.ChangeCoord());
    expectPointNear(
      aResultTriangulation->Node(anIndex).Transformed(aResultMeshLocation.Transformation()),
      anExpectedNode);
  }
}

TEST(BRepTools_GTrsfModificationTest, Triangulation_LocatedFaceUsesLocalCoordinates)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Poly_Triangulation> aSource = makeTriangulation(false);
  aBuilder.MakeFace(aFace, aSource);

  gp_Trsf aLocationTrsf;
  aLocationTrsf.SetTranslation(gp_Vec(10.0, 20.0, 30.0));
  aFace.Location(TopLoc_Location(aLocationTrsf));

  const gp_GTrsf aGTrsf = makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0));
  BRepTools_GTrsfModification     aModification(aGTrsf);
  occ::handle<Poly_Triangulation> aResult;
  ASSERT_TRUE(aModification.NewTriangulation(aFace, aResult));

  TopLoc_Location aResultLocation;
  ASSERT_FALSE(BRep_Tool::Triangulation(aFace, aResultLocation).IsNull());
  for (int anIndex = 1; anIndex <= aSource->NbNodes(); ++anIndex)
  {
    gp_Pnt anExpected = aSource->Node(anIndex).Transformed(aResultLocation.Transformation());
    aGTrsf.Transforms(anExpected.ChangeCoord());
    const gp_Pnt anActual = aResult->Node(anIndex).Transformed(aResultLocation.Transformation());
    EXPECT_LT(anActual.Distance(anExpected), 1.0e-12);
  }
}

TEST(BRepTools_GTrsfModificationTest, Tolerance_TracksCurrentTransformation)
{
  BRep_Builder  aBuilder;
  TopoDS_Vertex aVertex;
  aBuilder.MakeVertex(aVertex, gp_Pnt(1.0, 2.0, 3.0), 0.1);

  BRepTools_GTrsfModification aModification{gp_GTrsf()};
  aModification.SetGTrsf(makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0)));
  gp_Pnt aPoint;
  double aTolerance = 0.0;
  ASSERT_TRUE(aModification.NewPoint(aVertex, aPoint, aTolerance));
  EXPECT_DOUBLE_EQ(aTolerance, 0.4);
  expectPointNear(aPoint, gp_Pnt(2.0, 6.0, 12.0));

  aModification.SetGTrsf(makeGTrsf(gp_Mat(1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)));
  ASSERT_TRUE(aModification.NewPoint(aVertex, aPoint, aTolerance));
  EXPECT_DOUBLE_EQ(aTolerance, 0.1);
  expectPointNear(aPoint, gp_Pnt(3.0, 2.0, 3.0));

  const double aCos = std::sqrt(0.5);
  aModification.SetGTrsf(makeGTrsf(gp_Mat(aCos, -aCos, 0.0, aCos, aCos, 0.0, 0.0, 0.0, 1.0)));
  ASSERT_TRUE(aModification.NewPoint(aVertex, aPoint, aTolerance));
  EXPECT_NEAR(aTolerance, 0.1, 1.0e-14);

  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(0.0, 0.0, 0.0), 3.0);
  aModification.SetGTrsf(gp_GTrsf(aScale));
  ASSERT_TRUE(aModification.NewPoint(aVertex, aPoint, aTolerance));
  EXPECT_DOUBLE_EQ(aTolerance, 0.3);
  expectPointNear(aPoint, gp_Pnt(3.0, 6.0, 9.0));
}

TEST(BRepTools_GTrsfModificationTest, PolygonOnTriangulation_PreservesDeflection)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  TopoDS_Edge                           anEdge;
  const occ::handle<Poly_Triangulation> aTriangulation = makeTriangulation(false);
  aBuilder.MakeFace(aFace, aTriangulation);

  NCollection_Array1<int> aNodes(1, 2);
  aNodes(1)                                         = 1;
  aNodes(2)                                         = 2;
  occ::handle<Poly_PolygonOnTriangulation> aPolygon = new Poly_PolygonOnTriangulation(aNodes);
  aPolygon->Deflection(0.25);
  aBuilder.MakeEdge(anEdge, aPolygon, aTriangulation);

  const gp_GTrsf aGTrsf = makeGTrsf(gp_Mat(2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0));
  BRepTools_GTrsfModification              aModification(aGTrsf);
  occ::handle<Poly_PolygonOnTriangulation> aResult;
  ASSERT_TRUE(aModification.NewPolygonOnTriangulation(anEdge, aFace, aResult));
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 0.25);
}

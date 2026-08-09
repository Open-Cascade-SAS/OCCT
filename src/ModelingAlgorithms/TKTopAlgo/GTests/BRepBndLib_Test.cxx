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

#include <gtest/gtest.h>

#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRep_Builder.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Circle.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon3D.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

namespace
{
void expectSameBounds(const Bnd_Box& theFirst, const Bnd_Box& theSecond)
{
  const Bnd_Box::Limits aFirst  = theFirst.Get();
  const Bnd_Box::Limits aSecond = theSecond.Get();
  EXPECT_DOUBLE_EQ(aFirst.Xmin, aSecond.Xmin);
  EXPECT_DOUBLE_EQ(aFirst.Xmax, aSecond.Xmax);
  EXPECT_DOUBLE_EQ(aFirst.Ymin, aSecond.Ymin);
  EXPECT_DOUBLE_EQ(aFirst.Ymax, aSecond.Ymax);
  EXPECT_DOUBLE_EQ(aFirst.Zmin, aSecond.Zmin);
  EXPECT_DOUBLE_EQ(aFirst.Zmax, aSecond.Zmax);
}

void expectBounds(const Bnd_Box& theBox,
                  const double   theXMin,
                  const double   theYMin,
                  const double   theZMin,
                  const double   theXMax,
                  const double   theYMax,
                  const double   theZMax)
{
  const Bnd_Box::Limits aLimits = theBox.Get();
  EXPECT_NEAR(aLimits.Xmin, theXMin, 1.e-12);
  EXPECT_NEAR(aLimits.Ymin, theYMin, 1.e-12);
  EXPECT_NEAR(aLimits.Zmin, theZMin, 1.e-12);
  EXPECT_NEAR(aLimits.Xmax, theXMax, 1.e-12);
  EXPECT_NEAR(aLimits.Ymax, theYMax, 1.e-12);
  EXPECT_NEAR(aLimits.Zmax, theZMax, 1.e-12);
}

TopoDS_Edge makeCircleEdge()
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 1.0);
  return BRepBuilderAPI_MakeEdge(aCircle).Edge();
}

TopoDS_Edge makePolygon3dEdge()
{
  NCollection_Array1<gp_Pnt> aNodes(1, 5);
  aNodes(1) = gp_Pnt(1.0, 0.0, 0.0);
  aNodes(2) = gp_Pnt(0.0, 1.0, 0.0);
  aNodes(3) = gp_Pnt(-1.0, 0.0, 0.0);
  aNodes(4) = gp_Pnt(0.0, -1.0, 0.0);
  aNodes(5) = gp_Pnt(1.0, 0.0, 0.0);

  const occ::handle<Poly_Polygon3D> aPolygon = new Poly_Polygon3D(aNodes);
  TopoDS_Edge                       anEdge;
  BRep_Builder                      aBuilder;
  aBuilder.MakeEdge(anEdge, aPolygon);
  return anEdge;
}

TopoDS_Edge thirdCylinderEdge(const TopoDS_Shape& theCylinder)
{
  TopExp_Explorer anExplorer(theCylinder, TopAbs_EDGE);
  anExplorer.Next();
  anExplorer.Next();
  return TopoDS::Edge(anExplorer.Current());
}
} // namespace

// Migrated from tests/lowalgos/bnd/bug30292. A geometric edge without a
// discrete representation must have the same bounds with and without mesh use.
TEST(BRepBndLibTest, OCC30292_GeometricEdgeWithoutDiscreteRepresentation)
{
  const TopoDS_Edge aCircleEdge = makeCircleEdge();

  Bnd_Box aWithTriangulation;
  Bnd_Box aWithoutTriangulation;
  BRepBndLib::Add(aCircleEdge, aWithTriangulation, true);
  BRepBndLib::Add(aCircleEdge, aWithoutTriangulation, false);
  expectSameBounds(aWithTriangulation, aWithoutTriangulation);
}

// The polygon path and the exact-geometry path intentionally produce different
// boxes for a meshed circular edge; both are checked against DRAW's references.
TEST(BRepBndLibTest, OCC30292_MeshedGeometricEdgeUsesRequestedRepresentation)
{
  const TopoDS_Edge        aCircleEdge = makeCircleEdge();
  BRepMesh_IncrementalMesh aMesher(aCircleEdge, 0.1);
  ASSERT_TRUE(aMesher.IsDone());

  Bnd_Box aWithTriangulation;
  Bnd_Box aWithoutTriangulation;
  BRepBndLib::Add(aCircleEdge, aWithTriangulation, true);
  BRepBndLib::Add(aCircleEdge, aWithoutTriangulation, false);

  expectBounds(aWithTriangulation,
               -1.1000001,
               -1.0927089740980542,
               -0.1000001,
               1.1000001,
               1.092708974098054,
               0.1000001);
  expectBounds(aWithoutTriangulation, -1.0000001, -1.0000001, -1.e-7, 1.0000001, 1.0000001, 1.e-7);
}

TEST(BRepBndLibTest, OCC30292_MeshedCylinderEdgeUsesPolygon3d)
{
  const TopoDS_Shape       aCylinder     = BRepPrimAPI_MakeCylinder(1.0, 1.0).Shape();
  const TopoDS_Edge        aCylinderEdge = thirdCylinderEdge(aCylinder);
  BRepMesh_IncrementalMesh aMesher(aCylinder, 0.1);
  ASSERT_TRUE(aMesher.IsDone());

  Bnd_Box aWithTriangulation;
  Bnd_Box aWithoutTriangulation;
  BRepBndLib::Add(aCylinderEdge, aWithTriangulation, true);
  BRepBndLib::Add(aCylinderEdge, aWithoutTriangulation, false);

  expectBounds(aWithTriangulation,
               -1.1000001,
               -1.0927089740980542,
               -0.1000001,
               1.1000001,
               1.092708974098054,
               0.1000001);
  expectBounds(aWithoutTriangulation, -1.0000001, -1.0000001, -1.e-7, 1.0000001, 1.0000001, 1.e-7);
}

// A polygon-only edge has no geometric curve, so the representation flag must
// not change its bounds.
TEST(BRepBndLibTest, OCC30292_Polygon3dEdgeIgnoresRepresentationFlag)
{
  const TopoDS_Edge aPolygonEdge = makePolygon3dEdge();

  Bnd_Box aWithTriangulation;
  Bnd_Box aWithoutTriangulation;
  BRepBndLib::Add(aPolygonEdge, aWithTriangulation, true);
  BRepBndLib::Add(aPolygonEdge, aWithoutTriangulation, false);
  expectSameBounds(aWithTriangulation, aWithoutTriangulation);
}

// Migrated from tests/bugs/moddata_3/bug23575. A planar BSpline edge must not
// acquire a non-zero extent in the direction orthogonal to its plane.
TEST(BRepBndLibTest, OCC23575_PlanarBSplineEdgeBounds)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(1.0, 0.0, 0.0);

  NCollection_Array1<double> aWeights(1, 4);
  aWeights.Init(1.0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities(1) = 4;
  aMultiplicities(2) = 4;

  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 3, false);
  const TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aCurve).Edge();

  Bnd_Box aBox;
  BRepBndLib::Add(anEdge, aBox);
  ASSERT_FALSE(aBox.IsVoid());

  const Bnd_Box::Limits aLimits = aBox.Get();
  // Keep the 0.1 tolerance from DRAW's checkreal commands: the curve does not
  // interpolate all of its control points (its actual Ymax is about 0.90023).
  constexpr double aTolerance = 0.1;
  EXPECT_NEAR(aLimits.Xmin, 0.0, aTolerance);
  EXPECT_NEAR(aLimits.Ymin, 0.0, aTolerance);
  EXPECT_NEAR(aLimits.Zmin, 0.0, aTolerance);
  EXPECT_NEAR(aLimits.Xmax, 1.0, aTolerance);
  EXPECT_NEAR(aLimits.Ymax, 1.0, aTolerance);
  EXPECT_NEAR(aLimits.Zmax, 0.0, aTolerance);
  EXPECT_LE(aLimits.Zmax - aLimits.Zmin, 1.e-6);
}

// Migrated from tests/bugs/moddata_2/bug6503. An infinite planar face must
// remain open in X and Y while retaining only its finite surface tolerance in Z.
TEST(BRepBndLibTest, OCC6503_InfinitePlaneBounds)
{
  const TopoDS_Face aFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0))).Face();
  ASSERT_FALSE(aFace.IsNull());

  Bnd_Box aBox;
  BRepBndLib::Add(aFace, aBox);
  ASSERT_FALSE(aBox.IsVoid());

  const Bnd_Box::Limits aLimits     = aBox.Get();
  const double          anOpenBound = Precision::Infinite() / 2.0;
  EXPECT_EQ(aLimits.Xmin, -anOpenBound);
  EXPECT_EQ(aLimits.Xmax, anOpenBound);
  EXPECT_EQ(aLimits.Ymin, -anOpenBound);
  EXPECT_EQ(aLimits.Ymax, anOpenBound);
  EXPECT_NEAR(aLimits.Zmin, -1.e-7, 1.e-12);
  EXPECT_NEAR(aLimits.Zmax, 1.e-7, 1.e-12);
}

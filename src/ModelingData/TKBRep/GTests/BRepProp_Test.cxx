// Copyright (c) 2025 OPEN CASCADE SAS
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

// Unit tests for BRepProp_Curve and BRepProp_Surface on built BRep shapes.

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepProp_Curve.hxx>
#include <BRepProp_Surface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_LIN_TOL   = Precision::PConfusion();
constexpr double THE_CURV_TOL  = 1.0e-8;
constexpr double THE_DIR_TOL   = Precision::Confusion();
constexpr double THE_POINT_TOL = Precision::Confusion();
} // namespace

// ============================================================
// BRepProp_Curve tests
// ============================================================

TEST(BRepProp_CurveTest, UninitializedState)
{
  BRepProp_Curve aProp;
  EXPECT_FALSE(aProp.IsInitialized());

  const GeomProp::TangentResult   aTan  = aProp.Tangent(0.0, THE_LIN_TOL);
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  const GeomProp::NormalResult    aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  const GeomProp::CentreResult    aCent = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);

  EXPECT_FALSE(aTan.IsDefined);
  EXPECT_FALSE(aCurv.IsDefined);
  EXPECT_FALSE(aNorm.IsDefined);
  EXPECT_FALSE(aCent.IsDefined);
}

TEST(BRepProp_CurveTest, InitializeFromEdge_Line)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 10.0);
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  BRepProp_Curve aProp;
  aProp.Initialize(anEdge);
  ASSERT_TRUE(aProp.IsInitialized());

  // Tangent on a line should be defined.
  const GeomProp::TangentResult aTan = aProp.Tangent(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(std::abs(aTan.Direction.Dot(gp_Dir(1, 0, 0))), 1.0, THE_DIR_TOL);

  // Curvature of a line should be zero.
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
}

TEST(BRepProp_CurveTest, InitializeFromEdge_Circle)
{
  constexpr double        aRadius = 5.0;
  gp_Circ                 aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  BRepBuilderAPI_MakeEdge aMakeEdge(aCirc);
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  BRepProp_Curve aProp;
  aProp.Initialize(anEdge);
  ASSERT_TRUE(aProp.IsInitialized());

  // Check curvature = 1/R at midpoint.
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / aRadius, THE_CURV_TOL);

  // Check normal is defined.
  const GeomProp::NormalResult aNorm = aProp.Normal(M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);

  // Check centre of curvature should be at origin.
  const GeomProp::CentreResult aCent = aProp.CentreOfCurvature(M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aCent.IsDefined);
  EXPECT_NEAR(aCent.Centre.Distance(gp_Pnt(0, 0, 0)), 0.0, THE_POINT_TOL);
}

TEST(BRepProp_CurveTest, InitializeFromAdaptor)
{
  constexpr double        aRadius = 3.0;
  gp_Circ                 aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  BRepBuilderAPI_MakeEdge aMakeEdge(aCirc);
  ASSERT_TRUE(aMakeEdge.IsDone());

  BRepAdaptor_Curve anAdaptor(aMakeEdge.Edge());

  BRepProp_Curve aProp;
  aProp.Initialize(anAdaptor);
  ASSERT_TRUE(aProp.IsInitialized());

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / aRadius, THE_CURV_TOL);
}

TEST(BRepProp_CurveTest, BoxEdge_Tangent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Get the first edge.
  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepProp_Curve aProp;
  aProp.Initialize(anEdge);
  ASSERT_TRUE(aProp.IsInitialized());

  double aFirst = 0.0, aLast = 0.0;
  BRep_Tool::Range(anEdge, aFirst, aLast);
  const double aMid = 0.5 * (aFirst + aLast);

  // Box edges are lines - tangent should be defined and curvature zero.
  const GeomProp::TangentResult aTan = aProp.Tangent(aMid, THE_LIN_TOL);
  EXPECT_TRUE(aTan.IsDefined);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(aMid, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
}

TEST(BRepProp_CurveTest, CylinderEdge_Curvature)
{
  constexpr double         aRadius = 4.0;
  constexpr double         aHeight = 10.0;
  BRepPrimAPI_MakeCylinder aCylMaker(aRadius, aHeight);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_TRUE(aCylMaker.IsDone());

  // Find a circular edge (not a seam line).
  for (TopExp_Explorer anExp(aCyl, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    BRepProp_Curve     aProp;
    aProp.Initialize(anEdge);
    ASSERT_TRUE(aProp.IsInitialized());

    double aFirst = 0.0, aLast = 0.0;
    BRep_Tool::Range(anEdge, aFirst, aLast);
    const double aMid = 0.5 * (aFirst + aLast);

    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aMid, THE_LIN_TOL);
    if (aCurv.IsDefined && aCurv.Value > THE_CURV_TOL)
    {
      // Circular edge: curvature = 1/R.
      EXPECT_NEAR(aCurv.Value, 1.0 / aRadius, THE_CURV_TOL);
      return;
    }
  }
  // At least one circular edge should have been found.
  FAIL() << "No circular edge found on cylinder";
}

// ============================================================
// BRepProp_Surface tests
// ============================================================

TEST(BRepProp_SurfaceTest, UninitializedState)
{
  BRepProp_Surface aProp;
  EXPECT_FALSE(aProp.IsInitialized());

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, THE_LIN_TOL);
  EXPECT_FALSE(aNorm.IsDefined);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, 0.0, THE_LIN_TOL);
  EXPECT_FALSE(aCurv.IsDefined);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(0.0, 0.0, THE_LIN_TOL);
  EXPECT_FALSE(aMG.IsDefined);
}

TEST(BRepProp_SurfaceTest, BoxFace_PlanarNormal)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  BRepProp_Surface aProp;
  aProp.Initialize(aFace);
  ASSERT_TRUE(aProp.IsInitialized());

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);

  // Plane curvatures should be zero.
  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.MinCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, THE_CURV_TOL);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(aMG.MeanCurvature, 0.0, THE_CURV_TOL);
  EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
}

TEST(BRepProp_SurfaceTest, InitializeFromAdaptor)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  BRepAdaptor_Surface anAdaptor(aFace);
  BRepProp_Surface    aProp;
  aProp.Initialize(anAdaptor);
  ASSERT_TRUE(aProp.IsInitialized());

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.5, 0.5, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
}

TEST(BRepProp_SurfaceTest, SphereFace_Curvatures)
{
  constexpr double       aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphMaker(aRadius);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();
  ASSERT_TRUE(aSphMaker.IsDone());

  // Find the spherical face.
  TopExp_Explorer anExp(aSph, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  BRepProp_Surface aProp;
  aProp.Initialize(aFace);
  ASSERT_TRUE(aProp.IsInitialized());

  // Evaluate at equator-like parameter (away from poles).
  const double aU = M_PI;
  const double aV = 0.0;

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);

  // Sphere: both principal curvatures = 1/R in absolute value.
  // Sign depends on face orientation (BRepAdaptor_Surface accounts for it).
  EXPECT_NEAR(std::abs(aCurv.MinCurvature), 1.0 / aRadius, THE_POINT_TOL);
  EXPECT_NEAR(std::abs(aCurv.MaxCurvature), 1.0 / aRadius, THE_POINT_TOL);

  const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
  ASSERT_TRUE(aMG.IsDefined);
  EXPECT_NEAR(std::abs(aMG.MeanCurvature), 1.0 / aRadius, THE_POINT_TOL);
  EXPECT_NEAR(aMG.GaussianCurvature, 1.0 / (aRadius * aRadius), THE_POINT_TOL);
}

TEST(BRepProp_SurfaceTest, CylinderFace_Curvatures)
{
  constexpr double         aRadius = 3.0;
  constexpr double         aHeight = 10.0;
  BRepPrimAPI_MakeCylinder aCylMaker(aRadius, aHeight);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_TRUE(aCylMaker.IsDone());

  // Find the cylindrical face (lateral).
  for (TopExp_Explorer anExp(aCyl, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face&  aFace = TopoDS::Face(anExp.Current());
    BRepAdaptor_Surface anAdaptor(aFace);
    if (anAdaptor.GetType() != GeomAbs_Cylinder)
    {
      continue;
    }

    BRepProp_Surface aProp;
    aProp.Initialize(aFace);
    ASSERT_TRUE(aProp.IsInitialized());

    const double aU = M_PI / 4.0;
    const double aV = aHeight / 2.0;

    const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(aU, aV, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);

    // Cylinder: one curvature = 1/R, the other = 0.
    const double aMin = std::min(std::abs(aCurv.MinCurvature), std::abs(aCurv.MaxCurvature));
    const double aMax = std::max(std::abs(aCurv.MinCurvature), std::abs(aCurv.MaxCurvature));
    EXPECT_NEAR(aMin, 0.0, THE_CURV_TOL);
    EXPECT_NEAR(aMax, 1.0 / aRadius, THE_POINT_TOL);

    const GeomProp::MeanGaussianResult aMG = aProp.MeanGaussian(aU, aV, THE_LIN_TOL);
    ASSERT_TRUE(aMG.IsDefined);
    // Gaussian curvature of cylinder = 0.
    EXPECT_NEAR(aMG.GaussianCurvature, 0.0, THE_CURV_TOL);
    return;
  }
  FAIL() << "No cylindrical face found";
}

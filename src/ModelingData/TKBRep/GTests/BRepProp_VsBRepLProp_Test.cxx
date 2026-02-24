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

// Cross-validation tests comparing BRepProp_Curve / BRepProp_Surface
// against old BRepLProp_CLProps / BRepLProp_SLProps.

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLProp_CLProps.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepProp_Curve.hxx>
#include <BRepProp_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
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
constexpr double THE_DIR_TOL   = 1.0e-6;
constexpr double THE_POINT_TOL = 1.0e-6;

// ============================================================
// Curve cross-validation helpers
// ============================================================

void compareCurveTangent(const TopoDS_Edge& theEdge, const double theParam)
{
  BRepAdaptor_Curve anAdaptor(theEdge);

  // New API
  BRepProp_Curve aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::TangentResult aNew = aNewProp.Tangent(theParam, THE_LIN_TOL);

  // Old API
  BRepLProp_CLProps anOld(anAdaptor, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New tangent undefined but old is defined at param=" << theParam;
    gp_Dir anOldDir;
    anOld.Tangent(anOldDir);
    const double aDot = aNew.Direction.Dot(anOldDir);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Tangent direction mismatch at param=" << theParam;
  }
}

void compareCurveCurvature(const TopoDS_Edge& theEdge, const double theParam)
{
  BRepAdaptor_Curve anAdaptor(theEdge);

  BRepProp_Curve aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::CurvatureResult aNew = aNewProp.Curvature(theParam, THE_LIN_TOL);

  BRepLProp_CLProps anOld(anAdaptor, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New curvature undefined at param=" << theParam;
    EXPECT_NEAR(aNew.Value, anOld.Curvature(), THE_CURV_TOL)
      << "Curvature mismatch at param=" << theParam;
  }
}

void compareCurveNormal(const TopoDS_Edge& theEdge, const double theParam)
{
  BRepAdaptor_Curve anAdaptor(theEdge);

  BRepProp_Curve aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::NormalResult aNew = aNewProp.Normal(theParam, THE_LIN_TOL);

  BRepLProp_CLProps anOld(anAdaptor, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined) << "New normal undefined but old is defined at param=" << theParam;
    gp_Dir anOldDir;
    anOld.Normal(anOldDir);
    const double aDot = aNew.Direction.Dot(anOldDir);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Normal direction mismatch at param=" << theParam;
  }
}

void compareCurveCentre(const TopoDS_Edge& theEdge, const double theParam)
{
  BRepAdaptor_Curve anAdaptor(theEdge);

  BRepProp_Curve aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::CentreResult aNew = aNewProp.CentreOfCurvature(theParam, THE_LIN_TOL);

  BRepLProp_CLProps anOld(anAdaptor, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined) << "New centre undefined at param=" << theParam;
    gp_Pnt anOldPnt;
    anOld.CentreOfCurvature(anOldPnt);
    EXPECT_NEAR(aNew.Centre.Distance(anOldPnt), 0.0, THE_POINT_TOL)
      << "Centre of curvature mismatch at param=" << theParam;
  }
}

// ============================================================
// Surface cross-validation helpers
// ============================================================

void compareSurfaceNormal(const TopoDS_Face& theFace, const double theU, const double theV)
{
  BRepAdaptor_Surface anAdaptor(theFace);

  BRepProp_Surface aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::SurfaceNormalResult aNew = aNewProp.Normal(theU, theV, THE_LIN_TOL);

  BRepLProp_SLProps anOld(anAdaptor, theU, theV, 2, THE_LIN_TOL);
  if (anOld.IsNormalDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New normal undefined but old is defined at (" << theU << ", "
                                << theV << ")";
    const gp_Dir& anOldDir = anOld.Normal();
    const double  aDot     = aNew.Direction.Dot(anOldDir);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Surface normal mismatch at (" << theU << ", " << theV << ")";
  }
}

void compareSurfaceCurvatures(const TopoDS_Face& theFace, const double theU, const double theV)
{
  BRepAdaptor_Surface anAdaptor(theFace);

  BRepProp_Surface aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::SurfaceCurvatureResult aNew = aNewProp.Curvatures(theU, theV, THE_LIN_TOL);

  BRepLProp_SLProps anOld(anAdaptor, theU, theV, 2, THE_LIN_TOL);
  if (anOld.IsCurvatureDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New curvatures undefined at (" << theU << ", " << theV << ")";
    EXPECT_NEAR(aNew.MinCurvature, anOld.MinCurvature(), THE_CURV_TOL)
      << "Min curvature mismatch at (" << theU << ", " << theV << ")";
    EXPECT_NEAR(aNew.MaxCurvature, anOld.MaxCurvature(), THE_CURV_TOL)
      << "Max curvature mismatch at (" << theU << ", " << theV << ")";
  }
}

void compareSurfaceMeanGaussian(const TopoDS_Face& theFace, const double theU, const double theV)
{
  BRepAdaptor_Surface anAdaptor(theFace);

  BRepProp_Surface aNewProp;
  aNewProp.Initialize(anAdaptor);
  const GeomProp::MeanGaussianResult aNew = aNewProp.MeanGaussian(theU, theV, THE_LIN_TOL);

  BRepLProp_SLProps anOld(anAdaptor, theU, theV, 2, THE_LIN_TOL);
  if (anOld.IsCurvatureDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New mean/gaussian undefined at (" << theU << ", " << theV
                                << ")";
    EXPECT_NEAR(aNew.MeanCurvature, anOld.MeanCurvature(), THE_CURV_TOL)
      << "Mean curvature mismatch at (" << theU << ", " << theV << ")";
    EXPECT_NEAR(aNew.GaussianCurvature, anOld.GaussianCurvature(), THE_CURV_TOL)
      << "Gaussian curvature mismatch at (" << theU << ", " << theV << ")";
  }
}
} // namespace

// ============================================================
// Curve cross-validation tests
// ============================================================

TEST(BRepProp_VsBRepLPropTest, Curve_LineEdge)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Lin(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 10.0);
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  for (double aParam = 1.0; aParam <= 9.0; aParam += 2.0)
  {
    compareCurveTangent(anEdge, aParam);
    compareCurveCurvature(anEdge, aParam);
  }
}

TEST(BRepProp_VsBRepLPropTest, Curve_CircleEdge)
{
  constexpr double        aRadius = 5.0;
  gp_Circ                 aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  BRepBuilderAPI_MakeEdge aMakeEdge(aCirc);
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge& anEdge = aMakeEdge.Edge();

  for (double aParam = 0.5; aParam < 2.0 * M_PI; aParam += 1.0)
  {
    compareCurveTangent(anEdge, aParam);
    compareCurveCurvature(anEdge, aParam);
    compareCurveNormal(anEdge, aParam);
    compareCurveCentre(anEdge, aParam);
  }
}

TEST(BRepProp_VsBRepLPropTest, Curve_BoxEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    double             aFirst = 0.0, aLast = 0.0;
    BRep_Tool::Range(anEdge, aFirst, aLast);
    const double aMid = 0.5 * (aFirst + aLast);

    compareCurveTangent(anEdge, aMid);
    compareCurveCurvature(anEdge, aMid);
  }
}

TEST(BRepProp_VsBRepLPropTest, Curve_CylinderEdges)
{
  BRepPrimAPI_MakeCylinder aCylMaker(4.0, 10.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_TRUE(aCylMaker.IsDone());

  for (TopExp_Explorer anExp(aCyl, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    double             aFirst = 0.0, aLast = 0.0;
    BRep_Tool::Range(anEdge, aFirst, aLast);
    const double aMid = 0.5 * (aFirst + aLast);

    compareCurveTangent(anEdge, aMid);
    compareCurveCurvature(anEdge, aMid);
    compareCurveNormal(anEdge, aMid);
    compareCurveCentre(anEdge, aMid);
  }
}

// ============================================================
// Surface cross-validation tests
// ============================================================

TEST(BRepProp_VsBRepLPropTest, Surface_BoxFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    compareSurfaceNormal(aFace, 0.5, 0.5);
    compareSurfaceCurvatures(aFace, 0.5, 0.5);
    compareSurfaceMeanGaussian(aFace, 0.5, 0.5);
  }
}

TEST(BRepProp_VsBRepLPropTest, Surface_SphereFace)
{
  constexpr double       aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphMaker(aRadius);
  const TopoDS_Shape&    aSph = aSphMaker.Shape();
  ASSERT_TRUE(aSphMaker.IsDone());

  TopExp_Explorer anExp(aSph, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  // Evaluate at several points away from poles.
  for (double aU = 0.5; aU < 2.0 * M_PI; aU += 1.0)
  {
    compareSurfaceNormal(aFace, aU, 0.0);
    compareSurfaceCurvatures(aFace, aU, 0.0);
    compareSurfaceMeanGaussian(aFace, aU, 0.0);
  }
}

TEST(BRepProp_VsBRepLPropTest, Surface_CylinderFace)
{
  constexpr double         aRadius = 3.0;
  constexpr double         aHeight = 10.0;
  BRepPrimAPI_MakeCylinder aCylMaker(aRadius, aHeight);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();
  ASSERT_TRUE(aCylMaker.IsDone());

  for (TopExp_Explorer anExp(aCyl, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face&  aFace = TopoDS::Face(anExp.Current());
    BRepAdaptor_Surface anAdaptor(aFace);
    if (anAdaptor.GetType() != GeomAbs_Cylinder)
    {
      continue;
    }

    for (double aU = 0.5; aU < 2.0 * M_PI; aU += 1.5)
    {
      const double aV = aHeight / 2.0;
      compareSurfaceNormal(aFace, aU, aV);
      compareSurfaceCurvatures(aFace, aU, aV);
      compareSurfaceMeanGaussian(aFace, aU, aV);
    }
    return;
  }
  FAIL() << "No cylindrical face found";
}

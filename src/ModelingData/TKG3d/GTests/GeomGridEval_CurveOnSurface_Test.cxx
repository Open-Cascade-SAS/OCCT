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

#include <gtest/gtest.h>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomGridEval_Curve.hxx>
#include <GeomGridEval_CurveOnSurface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-8;

//! Helper function to create uniform parameters.
NCollection_Array1<double> CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  NCollection_Array1<double> aParams(1, theNbPoints);
  const double               aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}

//! Create a CurveOnSurface adaptor from a 2D curve on a surface.
occ::handle<Adaptor3d_CurveOnSurface> CreateCOS(
  const occ::handle<Geom2d_Curve>& theCurve2d,
  const occ::handle<Geom_Surface>& theSurface,
  double                           theFirst,
  double                           theLast)
{
  occ::handle<Geom2dAdaptor_Curve> aC2dAdaptor =
    new Geom2dAdaptor_Curve(theCurve2d, theFirst, theLast);
  occ::handle<GeomAdaptor_Surface> aSurfAdaptor = new GeomAdaptor_Surface(theSurface);
  return new Adaptor3d_CurveOnSurface(aC2dAdaptor, aSurfAdaptor);
}

//! Create a simple B-spline surface for testing.
occ::handle<Geom_BSplineSurface> CreateSimpleBSplineSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int iU = 1; iU <= 4; ++iU)
  {
    for (int iV = 1; iV <= 4; ++iV)
    {
      double aX = (iU - 1) * 1.0;
      double aY = (iV - 1) * 1.0;
      double aZ = 0.5 * std::sin(aX) * std::cos(aY);
      aPoles.SetValue(iU, iV, gp_Pnt(aX, aY, aZ));
    }
  }

  NCollection_Array1<double> aUKnots(1, 2);
  NCollection_Array1<double> aVKnots(1, 2);
  NCollection_Array1<int>    aUMults(1, 2);
  NCollection_Array1<int>    aVMults(1, 2);
  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  aUMults.SetValue(1, 4);
  aUMults.SetValue(2, 4);
  aVMults.SetValue(1, 4);
  aVMults.SetValue(2, 4);

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
}

//! Create a simple 2D B-spline curve for testing.
occ::handle<Geom2d_BSplineCurve> CreateSimple2dBSpline()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.1, 0.1));
  aPoles.SetValue(2, gp_Pnt2d(0.3, 0.8));
  aPoles.SetValue(3, gp_Pnt2d(0.7, 0.2));
  aPoles.SetValue(4, gp_Pnt2d(0.9, 0.9));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
}

} // namespace

//==================================================================================================
// D0 Tests
//==================================================================================================

TEST(GeomGridEval_CurveOnSurfaceTest, LineOnPlane)
{
  // 2D line on XY plane
  occ::handle<Geom_Plane>  aPlane  = new Geom_Plane(gp_Pln());
  occ::handle<Geom2d_Line> aLine2d = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 1));

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aLine2d, aPlane, 0.0, 5.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 11);
  NCollection_Array1<gp_Pnt> aGrid   = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  // Verify against Adaptor3d_CurveOnSurface
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aCOS->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveOnSurfaceTest, CircleOnPlane)
{
  // 2D circle on XY plane
  occ::handle<Geom_Plane>    aPlane    = new Geom_Plane(gp_Pln());
  occ::handle<Geom2d_Circle> aCircle2d = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aCircle2d, aPlane, 0.0, 2 * M_PI);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);
  NCollection_Array1<gp_Pnt> aGrid   = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aExpected = aCOS->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveOnSurfaceTest, LineOnCylinder)
{
  // V-line on cylinder (gives a ruling line)
  occ::handle<Geom_CylindricalSurface> aCyl =
    new Geom_CylindricalSurface(gp_Ax3(), 1.0);
  // Line along V direction at u=PI/4
  occ::handle<Geom2d_Line> aLine2d =
    new Geom2d_Line(gp_Pnt2d(M_PI / 4, 0), gp_Dir2d(0, 1));

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aLine2d, aCyl, 0.0, 5.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 11);
  NCollection_Array1<gp_Pnt> aGrid   = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aCOS->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveOnSurfaceTest, LineOnSphere)
{
  // Meridian on sphere (line along V at fixed U)
  occ::handle<Geom_SphericalSurface> aSph =
    new Geom_SphericalSurface(gp_Ax3(), 2.0);
  occ::handle<Geom2d_Line> aLine2d =
    new Geom2d_Line(gp_Pnt2d(M_PI / 3, -M_PI / 4), gp_Dir2d(0, 1));

  occ::handle<Adaptor3d_CurveOnSurface> aCOS =
    CreateCOS(aLine2d, aSph, 0.0, M_PI / 2);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, M_PI / 2, 11);
  NCollection_Array1<gp_Pnt> aGrid   = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aCOS->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveOnSurfaceTest, BSplineOnBSplineSurface)
{
  occ::handle<Geom_BSplineSurface> aSurf    = CreateSimpleBSplineSurface();
  occ::handle<Geom2d_BSplineCurve> aCurve2d = CreateSimple2dBSpline();

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aCurve2d, aSurf, 0.0, 1.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);
  NCollection_Array1<gp_Pnt> aGrid   = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aCOS->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// D1 Tests
//==================================================================================================

TEST(GeomGridEval_CurveOnSurfaceTest, D1_LineOnPlane)
{
  occ::handle<Geom_Plane>  aPlane  = new Geom_Plane(gp_Pln());
  occ::handle<Geom2d_Line> aLine2d = new Geom2d_Line(gp_Pnt2d(1, 0), gp_Dir2d(0.6, 0.8));

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aLine2d, aPlane, 0.0, 3.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double>                aParams = CreateUniformParams(0.0, 3.0, 9);
  NCollection_Array1<GeomGridEval::CurveD1> aGrid   = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aCOS->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// D2 Tests
//==================================================================================================

TEST(GeomGridEval_CurveOnSurfaceTest, D2_CircleOnSphere)
{
  occ::handle<Geom_SphericalSurface> aSph =
    new Geom_SphericalSurface(gp_Ax3(), 3.0);
  // Parallel circle on sphere (line along U at fixed V)
  occ::handle<Geom2d_Line> aLine2d =
    new Geom2d_Line(gp_Pnt2d(0, M_PI / 6), gp_Dir2d(1, 0));

  occ::handle<Adaptor3d_CurveOnSurface> aCOS =
    CreateCOS(aLine2d, aSph, 0.0, 2 * M_PI);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double>                aParams = CreateUniformParams(0.0, 2 * M_PI, 13);
  NCollection_Array1<GeomGridEval::CurveD2> aGrid   = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aCOS->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// D3 Tests
//==================================================================================================

TEST(GeomGridEval_CurveOnSurfaceTest, D3_BSplineOnCylinder)
{
  occ::handle<Geom_CylindricalSurface> aCyl =
    new Geom_CylindricalSurface(gp_Ax3(), 2.0);

  // 2D B-spline curve on cylinder surface
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(M_PI / 2, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(M_PI, 2.0));
  aPoles.SetValue(4, gp_Pnt2d(3 * M_PI / 2, 1.0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  occ::handle<Geom2d_BSplineCurve> aCurve2d = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aCurve2d, aCyl, 0.0, 1.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double>                aParams = CreateUniformParams(0.0, 1.0, 11);
  NCollection_Array1<GeomGridEval::CurveD3> aGrid   = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aCOS->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// DN Tests
//==================================================================================================

TEST(GeomGridEval_CurveOnSurfaceTest, DN)
{
  occ::handle<Geom_CylindricalSurface> aCyl =
    new Geom_CylindricalSurface(gp_Ax3(), 1.5);
  occ::handle<Geom2d_Line> aLine2d =
    new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0.5));

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aLine2d, aCyl, 0.0, 3.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 3.0, 9);

  // Test DN for orders 1-3
  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);
    EXPECT_EQ(aGrid.Size(), 9);

    for (int i = 1; i <= 9; ++i)
    {
      gp_Vec aExpected = aCOS->DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Integration test: via TransformedCurve / BRepAdaptor_Curve
//==================================================================================================

TEST(GeomGridEval_CurveOnSurfaceTest, ViaTransformedCurve)
{
  // Build a face with a curved edge whose representation is curve-on-surface.
  // A cylindrical face with a non-trivial pcurve.
  occ::handle<Geom_CylindricalSurface> aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 2.0);

  // Create a 2D curve (spiral-like on cylinder parametric space)
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(M_PI / 3, 1.5));
  aPoles.SetValue(3, gp_Pnt2d(2 * M_PI / 3, 3.0));
  aPoles.SetValue(4, gp_Pnt2d(M_PI, 4.0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  occ::handle<Geom2d_BSplineCurve> aCurve2d = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  // Build edge with pcurve on the cylindrical face
  TopoDS_Face aFace;
  BRep_Builder aBuilder;
  aBuilder.MakeFace(aFace, aCyl, Precision::Confusion());

  BRepBuilderAPI_MakeEdge aMakeEdge(aCurve2d, aCyl);
  ASSERT_TRUE(aMakeEdge.IsDone());
  TopoDS_Edge anEdge = aMakeEdge.Edge();

  // Use BRepAdaptor_Curve which internally creates a TransformedCurve with COS
  BRepAdaptor_Curve aBRepAdaptor(anEdge, aFace);

  // Test via GeomGridEval_Curve unified dispatcher
  GeomGridEval_Curve anEval;
  anEval.Initialize(aBRepAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());

  NCollection_Array1<double> aParams =
    CreateUniformParams(aBRepAdaptor.FirstParameter(), aBRepAdaptor.LastParameter(), 15);

  // Verify D0
  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  for (int i = 1; i <= 15; ++i)
  {
    gp_Pnt aExpected = aBRepAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }

  // Verify D1
  NCollection_Array1<GeomGridEval::CurveD1> aGridD1 = anEval.EvaluateGridD1(aParams);
  for (int i = 1; i <= 15; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aBRepAdaptor.D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGridD1.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGridD1.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveOnSurfaceTest, D3_BSplineOnBSplineSurface)
{
  occ::handle<Geom_BSplineSurface> aSurf    = CreateSimpleBSplineSurface();
  occ::handle<Geom2d_BSplineCurve> aCurve2d = CreateSimple2dBSpline();

  occ::handle<Adaptor3d_CurveOnSurface> aCOS = CreateCOS(aCurve2d, aSurf, 0.0, 1.0);

  GeomGridEval_CurveOnSurface anEval(aCOS->GetCurve(), aCOS->GetSurface());

  NCollection_Array1<double>                aParams = CreateUniformParams(0.0, 1.0, 11);
  NCollection_Array1<GeomGridEval::CurveD3> aGrid   = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aCOS->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

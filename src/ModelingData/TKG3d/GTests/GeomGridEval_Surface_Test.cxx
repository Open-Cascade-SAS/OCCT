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

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_BSplineSurface.hxx>
#include <GeomGridEval_OtherSurface.hxx>
#include <GeomGridEval_Plane.hxx>
#include <GeomGridEval_Sphere.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

//! Helper function to create uniform parameters
TColStd_Array1OfReal CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  TColStd_Array1OfReal aParams(1, theNbPoints);
  const double         aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}

//! Helper function to create a simple B-spline surface (bilinear patch)
Handle(Geom_BSplineSurface) CreateSimpleBSplineSurface()
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1)); // Non-planar corner

  TColStd_Array1OfReal    aUKnots(1, 2);
  TColStd_Array1OfReal    aVKnots(1, 2);
  TColStd_Array1OfInteger aUMults(1, 2);
  TColStd_Array1OfInteger aVMults(1, 2);

  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  aUMults.SetValue(1, 2);
  aUMults.SetValue(2, 2);
  aVMults.SetValue(1, 2);
  aVMults.SetValue(2, 2);

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 1, 1);
}
} // namespace

//==================================================================================================
// Tests for GeomGridEval_Plane
//==================================================================================================

TEST(GeomGridEval_PlaneTest, BasicEvaluation)
{
  // XY plane at origin
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomGridEval_Plane anEval(aPlane);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 5.0, 6);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 3.0, 4);
  anEval.SetUVParams(aUParams, aVParams);

  EXPECT_EQ(anEval.NbUParams(), 6);
  EXPECT_EQ(anEval.NbVParams(), 4);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_EQ(aGrid.RowLength(), 4);
  EXPECT_EQ(aGrid.ColLength(), 6);

  // Verify that all points are in Z=0 plane
  for (int iU = 1; iU <= 6; ++iU)
  {
    for (int iV = 1; iV <= 4; ++iV)
    {
      const gp_Pnt& aPnt = aGrid.Value(iU, iV);
      EXPECT_NEAR(aPnt.Z(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aPnt.X(), aUParams.Value(iU), THE_TOLERANCE);
      EXPECT_NEAR(aPnt.Y(), aVParams.Value(iV), THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_PlaneTest, NonOriginPlane)
{
  // Plane at (1, 2, 3) with normal (0, 0, 1)
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1));

  GeomGridEval_Plane anEval(aPlane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(-1.0, 1.0, 3);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-1.0, 1.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // All points should be at Z=3
  for (int iU = 1; iU <= 3; ++iU)
  {
    for (int iV = 1; iV <= 3; ++iV)
    {
      EXPECT_NEAR(aGrid.Value(iU, iV).Z(), 3.0, THE_TOLERANCE);
    }
  }

  // Center point (u=0, v=0) should be at (1, 2, 3)
  EXPECT_NEAR(aGrid.Value(2, 2).X(), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2, 2).Y(), 2.0, THE_TOLERANCE);
}

//==================================================================================================
// Tests for GeomGridEval_Sphere
//==================================================================================================

TEST(GeomGridEval_SphereTest, BasicEvaluation)
{
  // Unit sphere at origin
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);

  GeomGridEval_Sphere anEval(aSphere);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);           // Longitude
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 5);     // Latitude
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // All points should be at distance 1 from origin
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      double aDist = aGrid.Value(iU, iV).Distance(gp_Pnt(0, 0, 0));
      EXPECT_NEAR(aDist, 1.0, THE_TOLERANCE);
    }
  }

  // North pole (v = PI/2) should be at (0, 0, 1)
  for (int iU = 1; iU <= 9; ++iU)
  {
    EXPECT_NEAR(aGrid.Value(iU, 5).X(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(iU, 5).Y(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(iU, 5).Z(), 1.0, THE_TOLERANCE);
  }

  // South pole (v = -PI/2) should be at (0, 0, -1)
  for (int iU = 1; iU <= 9; ++iU)
  {
    EXPECT_NEAR(aGrid.Value(iU, 1).X(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(iU, 1).Y(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(iU, 1).Z(), -1.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_SphereTest, NonUnitSphere)
{
  // Sphere with radius 3 at center (1, 2, 3)
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 3.0);

  GeomGridEval_Sphere anEval(aSphere);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 17);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 9);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // All points should be at distance 3 from center
  const gp_Pnt aCenter(1, 2, 3);
  for (int iU = 1; iU <= 17; ++iU)
  {
    for (int iV = 1; iV <= 9; ++iV)
    {
      double aDist = aGrid.Value(iU, iV).Distance(aCenter);
      EXPECT_NEAR(aDist, 3.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Tests for GeomGridEval_BSplineSurface
//==================================================================================================

TEST(GeomGridEval_BSplineSurfaceTest, BasicEvaluation)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aExpected = aSurf->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, CornerPoints)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);

  TColStd_Array1OfReal aUParams(1, 2);
  TColStd_Array1OfReal aVParams(1, 2);
  aUParams.SetValue(1, 0.0);
  aUParams.SetValue(2, 1.0);
  aVParams.SetValue(1, 0.0);
  aVParams.SetValue(2, 1.0);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Corner (0,0): pole (1,1) = (0, 0, 0)
  EXPECT_NEAR(aGrid.Value(1, 1).Distance(gp_Pnt(0, 0, 0)), 0.0, THE_TOLERANCE);

  // Corner (1,0): pole (2,1) = (1, 0, 0)
  EXPECT_NEAR(aGrid.Value(2, 1).Distance(gp_Pnt(1, 0, 0)), 0.0, THE_TOLERANCE);

  // Corner (0,1): pole (1,2) = (0, 1, 0)
  EXPECT_NEAR(aGrid.Value(1, 2).Distance(gp_Pnt(0, 1, 0)), 0.0, THE_TOLERANCE);

  // Corner (1,1): pole (2,2) = (1, 1, 1)
  EXPECT_NEAR(aGrid.Value(2, 2).Distance(gp_Pnt(1, 1, 1)), 0.0, THE_TOLERANCE);
}

//==================================================================================================
// Tests for GeomGridEval_OtherSurface (fallback)
//==================================================================================================

TEST(GeomGridEval_OtherSurfaceTest, CylinderFallback)
{
  // Cylinder is not directly supported, should use fallback
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aCyl);

  GeomGridEval_OtherSurface anEval(anAdaptor->ShallowCopy());
  EXPECT_FALSE(anEval.Surface().IsNull());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aExpected = aCyl->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Tests for GeomGridEval_Surface (unified dispatcher)
//==================================================================================================

TEST(GeomGridEval_SurfaceTest, PlaneDispatch)
{
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aGeomPlane);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Plane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(-5.0, 5.0, 11);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-3.0, 3.0, 7);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 11; ++iU)
  {
    for (int iV = 1; iV <= 7; ++iV)
    {
      gp_Pnt aExpected = aGeomPlane->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, SphereDispatch)
{
  Handle(Geom_SphericalSurface) aGeomSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aGeomSphere);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Sphere);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 13);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 7);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 13; ++iU)
  {
    for (int iV = 1; iV <= 7; ++iV)
    {
      gp_Pnt aExpected = aGeomSphere->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, BSplineDispatch)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aSurf);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineSurface);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 11);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 11; ++iU)
  {
    for (int iV = 1; iV <= 11; ++iV)
    {
      gp_Pnt aExpected = aSurf->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, BezierSurfaceDispatch)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 0));
  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aBezier);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BezierSurface);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = aBezier->Value(aParams.Value(i), aParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, CylinderDispatch)
{
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aCyl);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Cylinder);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aExpected = aCyl->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, TorusDispatch)
{
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 4.0, 1.0);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aTorus);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Torus);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 9; ++iV)
    {
      gp_Pnt aExpected = aTorus->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, ConeDispatch)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aCone);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Cone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aExpected = aCone->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, SurfaceOfRevolutionFallbackDispatch)
{
  // Surface of Revolution is not optimized, should use fallback
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 0, 1)); // Line at x=1 parallel to Z
  Handle(Geom_SurfaceOfRevolution) aRevSurf =
    new Geom_SurfaceOfRevolution(aLine, gp::OZ()); // Revolving around Z -> Cylinder-like

  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aRevSurf);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_SurfaceOfRevolution);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aExpected = aRevSurf->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, DirectHandleInit)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Initialize directly from Handle(Geom_Surface)
  GeomGridEval_Surface anEval;
  anEval.Initialize(aPlane);
  
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Plane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_FALSE(aGrid.IsEmpty());
  
  // Verify value
  EXPECT_NEAR(aGrid.Value(1, 1).Z(), 0.0, THE_TOLERANCE);
}

TEST(GeomGridEval_SurfaceTest, UninitializedState)
{
  GeomGridEval_Surface anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_TRUE(aGrid.IsEmpty());
}

TEST(GeomGridEval_SurfaceTest, EmptyParams)
{
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aGeomPlane);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);

  // EvaluateGrid without setting params should return empty
  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_TRUE(aGrid.IsEmpty());
}

//==================================================================================================
// Additional tests for B-spline surfaces (from BSplSLib_GridEvaluator_Test)
//==================================================================================================

namespace
{
//! Helper function to create a rational B-spline surface
Handle(Geom_BSplineSurface) CreateRationalBSplineSurface()
{
  TColgp_Array2OfPnt   aPoles(1, 3, 1, 3);
  TColStd_Array2OfReal aWeights(1, 3, 1, 3);

  // Create a curved surface patch with varying weights
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      double x = (i - 1) * 1.0;
      double y = (j - 1) * 1.0;
      double z = (i == 2 && j == 2) ? 1.0 : 0.0; // Center point elevated
      aPoles.SetValue(i, j, gp_Pnt(x, y, z));
      aWeights.SetValue(i, j, (i == 2 && j == 2) ? 2.0 : 1.0); // Center weight higher
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 2);
  TColStd_Array1OfReal    aVKnots(1, 2);
  TColStd_Array1OfInteger aUMults(1, 2);
  TColStd_Array1OfInteger aVMults(1, 2);

  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  aUMults.SetValue(1, 3);
  aUMults.SetValue(2, 3);
  aVMults.SetValue(1, 3);
  aVMults.SetValue(2, 3);

  return new Geom_BSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

//! Helper function to create a multi-span B-spline surface
Handle(Geom_BSplineSurface) CreateMultiSpanBSplineSurface()
{
  // Degree 2, multi-span surface with 1 internal knot in each direction
  // Formula: n_poles = sum(multiplicities) - degree - 1
  // U: (3 + 2 + 3) - 2 - 1 = 5 poles
  // V: (3 + 2 + 3) - 2 - 1 = 5 poles
  TColgp_Array2OfPnt aPoles(1, 5, 1, 5);

  // Create a wavy surface with 5x5 poles
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double x = (i - 1) * 1.0;
      double y = (j - 1) * 1.0;
      double z = std::sin((i - 1) * M_PI / 4.0) * std::sin((j - 1) * M_PI / 4.0);
      aPoles.SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 3);
  TColStd_Array1OfReal    aVKnots(1, 3);
  TColStd_Array1OfInteger aUMults(1, 3);
  TColStd_Array1OfInteger aVMults(1, 3);

  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 0.5);
  aUKnots.SetValue(3, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 0.5);
  aVKnots.SetValue(3, 1.0);
  aUMults.SetValue(1, 3);
  aUMults.SetValue(2, 2);
  aUMults.SetValue(3, 3);
  aVMults.SetValue(1, 3);
  aVMults.SetValue(2, 2);
  aVMults.SetValue(3, 3);

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}
} // namespace

TEST(GeomGridEval_BSplineSurfaceTest, RationalSurface)
{
  Handle(Geom_BSplineSurface) aSurf = CreateRationalBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 11);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 11; ++iU)
  {
    for (int iV = 1; iV <= 11; ++iV)
    {
      gp_Pnt aExpected = aSurf->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, MultiSpanSurface)
{
  Handle(Geom_BSplineSurface) aSurf = CreateMultiSpanBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 21);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 21);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 21; ++iU)
  {
    for (int iV = 1; iV <= 21; ++iV)
    {
      gp_Pnt aExpected = aSurf->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, HigherDegree)
{
  // Create a bicubic B-spline surface
  TColgp_Array2OfPnt aPoles(1, 4, 1, 4);

  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double x = (i - 1) * 1.0;
      double y = (j - 1) * 1.0;
      double z = std::cos((i + j - 2) * M_PI / 6.0);
      aPoles.SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 2);
  TColStd_Array1OfReal    aVKnots(1, 2);
  TColStd_Array1OfInteger aUMults(1, 2);
  TColStd_Array1OfInteger aVMults(1, 2);

  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  aUMults.SetValue(1, 4);
  aUMults.SetValue(2, 4);
  aVMults.SetValue(1, 4);
  aVMults.SetValue(2, 4);

  Handle(Geom_BSplineSurface) aSurf =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);

  GeomGridEval_BSplineSurface anEval(aSurf);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 17);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 17);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 17; ++iU)
  {
    for (int iV = 1; iV <= 17; ++iV)
    {
      gp_Pnt aExpected = aSurf->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Tests for Surface Derivative Evaluation (D1, D2)
//==================================================================================================

TEST(GeomGridEval_PlaneTest, DerivativeD1)
{
  // XY plane at origin
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomGridEval_Plane anEval(aPlane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 5.0, 6);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 3.0, 4);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

  // For a plane, D1U and D1V are constant (the X and Y directions of the plane)
  gp_Pnt aPnt;
  gp_Vec aD1URef, aD1VRef;
  aPlane->D1(0.0, 0.0, aPnt, aD1URef, aD1VRef);

  for (int iU = 1; iU <= 6; ++iU)
  {
    for (int iV = 1; iV <= 4; ++iV)
    {
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1URef).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1VRef).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_PlaneTest, DerivativeD2)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomGridEval_Plane anEval(aPlane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 5.0, 6);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 3.0, 4);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

  // For a plane, all second derivatives are zero
  for (int iU = 1; iU <= 6; ++iU)
  {
    for (int iV = 1; iV <= 4; ++iV)
    {
      EXPECT_NEAR(aGrid.Value(iU, iV).D2U.Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aGrid.Value(iU, iV).D2V.Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aGrid.Value(iU, iV).D2UV.Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SphereTest, DerivativeD1)
{
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEval_Sphere anEval(aSphere);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

  // Verify D1 against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      aSphere->D1(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SphereTest, DerivativeD2)
{
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEval_Sphere anEval(aSphere);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

  // Verify D2 against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aSphere->D2(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeD1)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface anEval(aSurf);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      aSurf->D1(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeD2)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface anEval(aSurf);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aSurf->D2(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, UnifiedDerivativeD1)
{
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aSphere);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      aSphere->D1(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceTest, UnifiedDerivativeD2)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();
  Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(aSurf);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

  // Verify against direct evaluation
  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aSurf->D2(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

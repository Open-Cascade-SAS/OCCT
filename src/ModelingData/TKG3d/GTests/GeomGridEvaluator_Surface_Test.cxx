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

#include <Geom_BSplineSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEvaluator_BSplineSurface.hxx>
#include <GeomGridEvaluator_OtherSurface.hxx>
#include <GeomGridEvaluator_Plane.hxx>
#include <GeomGridEvaluator_Sphere.hxx>
#include <GeomGridEvaluator_Surface.hxx>
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
// Tests for GeomGridEvaluator_Plane
//==================================================================================================

TEST(GeomGridEvaluator_PlaneTest, BasicEvaluation)
{
  // XY plane at origin
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomGridEvaluator_Plane anEval;
  anEval.Initialize(aPlane);
  EXPECT_TRUE(anEval.IsInitialized());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 5.0, 6);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 3.0, 4);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_PlaneTest, NonOriginPlane)
{
  // Plane at (1, 2, 3) with normal (0, 0, 1)
  gp_Pln aPlane(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1));

  GeomGridEvaluator_Plane anEval;
  anEval.Initialize(aPlane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(-1.0, 1.0, 3);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-1.0, 1.0, 3);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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
// Tests for GeomGridEvaluator_Sphere
//==================================================================================================

TEST(GeomGridEvaluator_SphereTest, BasicEvaluation)
{
  // Unit sphere at origin
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);

  GeomGridEvaluator_Sphere anEval;
  anEval.Initialize(aSphere);
  EXPECT_TRUE(anEval.IsInitialized());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);           // Longitude
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 5);     // Latitude
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_SphereTest, NonUnitSphere)
{
  // Sphere with radius 3 at center (1, 2, 3)
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 3.0);

  GeomGridEvaluator_Sphere anEval;
  anEval.Initialize(aSphere);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 17);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 9);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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
// Tests for GeomGridEvaluator_BSplineSurface
//==================================================================================================

TEST(GeomGridEvaluator_BSplineSurfaceTest, BasicEvaluation)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();

  GeomGridEvaluator_BSplineSurface anEval;
  anEval.Initialize(aSurf);
  EXPECT_TRUE(anEval.IsInitialized());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_BSplineSurfaceTest, CornerPoints)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();

  GeomGridEvaluator_BSplineSurface anEval;
  anEval.Initialize(aSurf);

  TColStd_Array1OfReal aUParams(1, 2);
  TColStd_Array1OfReal aVParams(1, 2);
  aUParams.SetValue(1, 0.0);
  aUParams.SetValue(2, 1.0);
  aVParams.SetValue(1, 0.0);
  aVParams.SetValue(2, 1.0);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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
// Tests for GeomGridEvaluator_OtherSurface (fallback)
//==================================================================================================

TEST(GeomGridEvaluator_OtherSurfaceTest, CylinderFallback)
{
  // Cylinder is not directly supported, should use fallback
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Surface anAdaptor(aCyl);

  GeomGridEvaluator_OtherSurface anEval;
  anEval.Initialize(anAdaptor.ShallowCopy());
  EXPECT_TRUE(anEval.IsInitialized());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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
// Tests for GeomGridEvaluator_Surface (unified dispatcher)
//==================================================================================================

TEST(GeomGridEvaluator_SurfaceTest, PlaneDispatch)
{
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomAdaptor_Surface anAdaptor(aGeomPlane);

  GeomGridEvaluator_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Plane);

  TColStd_Array1OfReal aUParams = CreateUniformParams(-5.0, 5.0, 11);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-3.0, 3.0, 7);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_SurfaceTest, SphereDispatch)
{
  Handle(Geom_SphericalSurface) aGeomSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Surface anAdaptor(aGeomSphere);

  GeomGridEvaluator_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Sphere);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 13);
  TColStd_Array1OfReal aVParams = CreateUniformParams(-M_PI / 2, M_PI / 2, 7);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_SurfaceTest, BSplineDispatch)
{
  Handle(Geom_BSplineSurface) aSurf = CreateSimpleBSplineSurface();
  GeomAdaptor_Surface         anAdaptor(aSurf);

  GeomGridEvaluator_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineSurface);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 11);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_SurfaceTest, CylinderFallbackDispatch)
{
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Surface anAdaptor(aCyl);

  GeomGridEvaluator_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Cylinder);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

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

TEST(GeomGridEvaluator_SurfaceTest, UninitializedState)
{
  GeomGridEvaluator_Surface anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_TRUE(aGrid.IsEmpty());
}

TEST(GeomGridEvaluator_SurfaceTest, EmptyParams)
{
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomAdaptor_Surface anAdaptor(aGeomPlane);

  GeomGridEvaluator_Surface anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);

  // EvaluateGrid without setting params should return empty
  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_TRUE(aGrid.IsEmpty());
}

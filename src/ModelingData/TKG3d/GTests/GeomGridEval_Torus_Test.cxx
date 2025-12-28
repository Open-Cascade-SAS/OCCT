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

#include <Geom_ToroidalSurface.hxx>
#include <GeomGridEval_Torus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

NCollection_Array1<double> CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  NCollection_Array1<double> aParams(1, theNbPoints);
  const double         aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}
} // namespace

//==================================================================================================
// Grid evaluation tests
//==================================================================================================

TEST(GeomGridEval_TorusTest, GridBasicEvaluation)
{
  // Torus: Major=10, Minor=2, Center(0,0,0), Z-axis
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);

  GeomGridEval_Torus anEval(aTorus);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9); // Major
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 2 * M_PI, 9); // Minor

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);
  EXPECT_EQ(aGrid.RowLength(), 9);
  EXPECT_EQ(aGrid.ColLength(), 9);

  // Verify points
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 9; ++iV)
    {
      gp_Pnt aExpected = aTorus->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_TorusTest, GridDerivativeD1)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1(aUParams, aVParams);

  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 9; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      aTorus->D1(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_TorusTest, GridDerivativeD2)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2(aUParams, aVParams);

  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 9; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aTorus->D2(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_TorusTest, GridDerivativeD3)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array2<GeomGridEval::SurfD3> aGrid = anEval.EvaluateGridD3(aUParams, aVParams);

  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 9; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      aTorus->D3(aUParams.Value(iU),
                 aVParams.Value(iV),
                 aPnt,
                 aD1U,
                 aD1V,
                 aD2U,
                 aD2V,
                 aD2UV,
                 aD3U,
                 aD3V,
                 aD3UUV,
                 aD3UVV);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D3U - aD3U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D3V - aD3V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D3UUV - aD3UUV).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D3UVV - aD3UVV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_TorusTest, GridDerivativeDN)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 2 * M_PI, 4);

  // Test D4U (4th derivative in U)
  NCollection_Array2<gp_Vec> aD4U = anEval.EvaluateGridDN(aUParams, aVParams, 4, 0);
  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 4; ++iV)
    {
      gp_Vec aExpected = aTorus->DN(aUParams.Value(iU), aVParams.Value(iV), 4, 0);
      EXPECT_NEAR((aD4U.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Points evaluation tests
//==================================================================================================

TEST(GeomGridEval_TorusTest, PointsBasicEvaluation)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  // Create arbitrary UV pairs
  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 10);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, M_PI / 4));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, M_PI / 2));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, M_PI));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 3 * M_PI / 2));
  aUVPairs.SetValue(6, gp_Pnt2d(2 * M_PI, 2 * M_PI));
  aUVPairs.SetValue(7, gp_Pnt2d(0.5, 1.0));
  aUVPairs.SetValue(8, gp_Pnt2d(1.5, 2.0));
  aUVPairs.SetValue(9, gp_Pnt2d(2.5, 3.0));
  aUVPairs.SetValue(10, gp_Pnt2d(3.5, 4.0));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);
  EXPECT_EQ(aPoints.Length(), 10);

  for (int i = 1; i <= 10; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Pnt          aExpected = aTorus->Value(aUV.X(), aUV.Y());
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_TorusTest, PointsDerivativeD1)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, M_PI / 3));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, M_PI / 2));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, M_PI));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 5 * M_PI / 4));

  NCollection_Array1<GeomGridEval::SurfD1> aResults = anEval.EvaluatePointsD1(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV = aUVPairs.Value(i);
    gp_Pnt          aPnt;
    gp_Vec          aD1U, aD1V;
    aTorus->D1(aUV.X(), aUV.Y(), aPnt, aD1U, aD1V);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_TorusTest, PointsDerivativeD2)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, M_PI / 3));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, M_PI / 2));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, M_PI));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 5 * M_PI / 4));

  NCollection_Array1<GeomGridEval::SurfD2> aResults = anEval.EvaluatePointsD2(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV = aUVPairs.Value(i);
    gp_Pnt          aPnt;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV;
    aTorus->D2(aUV.X(), aUV.Y(), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_TorusTest, PointsDerivativeD3)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, M_PI / 3));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, M_PI / 2));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, M_PI));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 5 * M_PI / 4));

  NCollection_Array1<GeomGridEval::SurfD3> aResults = anEval.EvaluatePointsD3(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV = aUVPairs.Value(i);
    gp_Pnt          aPnt;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
    aTorus->D3(aUV.X(), aUV.Y(), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3U - aD3U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3V - aD3V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3UUV - aD3UUV).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3UVV - aD3UVV).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_TorusTest, PointsDerivativeDN)
{
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, M_PI / 3));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, M_PI / 2));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, M_PI));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 5 * M_PI / 4));

  // Test D4U
  NCollection_Array1<gp_Vec> aD4U = anEval.EvaluatePointsDN(aUVPairs, 4, 0);
  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Vec          aExpected = aTorus->DN(aUV.X(), aUV.Y(), 4, 0);
    EXPECT_NEAR((aD4U.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }

  // Test mixed D2UV
  NCollection_Array1<gp_Vec> aD2UV = anEval.EvaluatePointsDN(aUVPairs, 2, 2);
  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Vec          aExpected = aTorus->DN(aUV.X(), aUV.Y(), 2, 2);
    EXPECT_NEAR((aD2UV.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_TorusTest, PointsTransformedTorus)
{
  // Torus with offset center and tilted axis
  gp_Ax3                       anAxis(gp_Pnt(5, 3, 2), gp_Dir(1, 1, 1));
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(anAxis, 8.0, 1.5);
  GeomGridEval_Torus           anEval(aTorus);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 8);
  for (int i = 1; i <= 8; ++i)
  {
    aUVPairs.SetValue(i, gp_Pnt2d((i - 1) * M_PI / 4, (i - 1) * M_PI / 3));
  }

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  for (int i = 1; i <= 8; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Pnt          aExpected = aTorus->Value(aUV.X(), aUV.Y());
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_TorusTest, PointsOnMajorCircle)
{
  // Test points on major circle (V=0)
  occ::handle<Geom_ToroidalSurface> aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  GeomGridEval_Torus anEval(aTorus);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 4);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 2, 0.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI, 0.0));
  aUVPairs.SetValue(4, gp_Pnt2d(3 * M_PI / 2, 0.0));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  // Points on major circle at V=0 should be at distance (Major+Minor) from Z axis
  const double aExpectedRadius = 10.0 + 2.0; // Major + Minor
  for (int i = 1; i <= 4; ++i)
  {
    const gp_Pnt& aPnt        = aPoints.Value(i);
    double        aRadialDist = std::sqrt(aPnt.X() * aPnt.X() + aPnt.Y() * aPnt.Y());
    EXPECT_NEAR(aRadialDist, aExpectedRadius, THE_TOLERANCE);
    EXPECT_NEAR(aPnt.Z(), 0.0, THE_TOLERANCE);
  }
}

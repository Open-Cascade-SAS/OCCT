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

#include <Geom_ConicalSurface.hxx>
#include <GeomGridEval_Cone.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

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
} // namespace

//==================================================================================================
// Grid evaluation tests
//==================================================================================================

TEST(GeomGridEval_ConeTest, GridBasicEvaluation)
{
  // Cone: SemiAngle=PI/4, Radius=1.0 at origin, Center(0,0,0), Z-axis
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);

  GeomGridEval_Cone anEval(aCone);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9); // Angle
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);      // Linear param

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);
  EXPECT_EQ(aGrid.RowLength(), 6);
  EXPECT_EQ(aGrid.ColLength(), 9);

  // Verify points
  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aExpected = aCone->Value(aUParams.Value(iU), aVParams.Value(iV));
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_ConeTest, GridDerivativeD1)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1(aUParams, aVParams);

  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      aCone->D1(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_ConeTest, GridDerivativeD2)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2(aUParams, aVParams);

  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aCone->D2(aUParams.Value(iU), aVParams.Value(iV), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      EXPECT_NEAR(aGrid.Value(iU, iV).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(iU, iV).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_ConeTest, GridDerivativeD3)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array2<GeomGridEval::SurfD3> aGrid = anEval.EvaluateGridD3(aUParams, aVParams);

  for (int iU = 1; iU <= 9; ++iU)
  {
    for (int iV = 1; iV <= 6; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      aCone->D3(aUParams.Value(iU),
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

TEST(GeomGridEval_ConeTest, GridDerivativeDN)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 4);

  // Test D4U (4th derivative in U)
  NCollection_Array2<gp_Vec> aD4U = anEval.EvaluateGridDN(aUParams, aVParams, 4, 0);
  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 4; ++iV)
    {
      gp_Vec aExpected = aCone->DN(aUParams.Value(iU), aVParams.Value(iV), 4, 0);
      EXPECT_NEAR((aD4U.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Points evaluation tests
//==================================================================================================

TEST(GeomGridEval_ConeTest, PointsBasicEvaluation)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  // Create arbitrary UV pairs
  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 10);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, 1.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, 2.0));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, 3.0));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 4.0));
  aUVPairs.SetValue(6, gp_Pnt2d(2 * M_PI, 5.0));
  aUVPairs.SetValue(7, gp_Pnt2d(0.5, 0.5));
  aUVPairs.SetValue(8, gp_Pnt2d(1.5, 1.5));
  aUVPairs.SetValue(9, gp_Pnt2d(2.5, 2.5));
  aUVPairs.SetValue(10, gp_Pnt2d(3.5, 3.5));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);
  EXPECT_EQ(aPoints.Length(), 10);

  for (int i = 1; i <= 10; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Pnt          aExpected = aCone->Value(aUV.X(), aUV.Y());
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_ConeTest, PointsDerivativeD1)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, 1.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, 2.0));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, 3.0));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 4.0));

  NCollection_Array1<GeomGridEval::SurfD1> aResults = anEval.EvaluatePointsD1(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV = aUVPairs.Value(i);
    gp_Pnt          aPnt;
    gp_Vec          aD1U, aD1V;
    aCone->D1(aUV.X(), aUV.Y(), aPnt, aD1U, aD1V);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_ConeTest, PointsDerivativeD2)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, 1.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, 2.0));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, 3.0));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 4.0));

  NCollection_Array1<GeomGridEval::SurfD2> aResults = anEval.EvaluatePointsD2(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV = aUVPairs.Value(i);
    gp_Pnt          aPnt;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV;
    aCone->D2(aUV.X(), aUV.Y(), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_ConeTest, PointsDerivativeD3)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, 1.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, 2.0));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, 3.0));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 4.0));

  NCollection_Array1<GeomGridEval::SurfD3> aResults = anEval.EvaluatePointsD3(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV = aUVPairs.Value(i);
    gp_Pnt          aPnt;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
    aCone->D3(aUV.X(), aUV.Y(), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
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

TEST(GeomGridEval_ConeTest, PointsDerivativeDN)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 4, 1.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI / 2, 2.0));
  aUVPairs.SetValue(4, gp_Pnt2d(M_PI, 3.0));
  aUVPairs.SetValue(5, gp_Pnt2d(3 * M_PI / 2, 4.0));

  // Test D4U
  NCollection_Array1<gp_Vec> aD4U = anEval.EvaluatePointsDN(aUVPairs, 4, 0);
  for (int i = 1; i <= 5; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Vec          aExpected = aCone->DN(aUV.X(), aUV.Y(), 4, 0);
    EXPECT_NEAR((aD4U.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_ConeTest, PointsTransformedCone)
{
  // Cone with offset center and tilted axis
  gp_Ax3                      anAxis(gp_Pnt(5, 3, 2), gp_Dir(1, 1, 1));
  Handle(Geom_ConicalSurface) aCone = new Geom_ConicalSurface(anAxis, M_PI / 6, 2.0);
  GeomGridEval_Cone           anEval(aCone);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 8);
  for (int i = 1; i <= 8; ++i)
  {
    aUVPairs.SetValue(i, gp_Pnt2d((i - 1) * M_PI / 4, (i - 1) * 0.5));
  }

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  for (int i = 1; i <= 8; ++i)
  {
    const gp_Pnt2d& aUV       = aUVPairs.Value(i);
    gp_Pnt          aExpected = aCone->Value(aUV.X(), aUV.Y());
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_ConeTest, PointsAtApex)
{
  // Test evaluation at cone apex (V=0 when RefRadius=0)
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 0.0);
  GeomGridEval_Cone anEval(aCone);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 4);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(M_PI / 2, 0.0));
  aUVPairs.SetValue(3, gp_Pnt2d(M_PI, 0.0));
  aUVPairs.SetValue(4, gp_Pnt2d(3 * M_PI / 2, 0.0));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  // All points at apex should be the same (origin)
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_NEAR(aPoints.Value(i).Distance(gp_Pnt(0, 0, 0)), 0.0, THE_TOLERANCE);
  }
}

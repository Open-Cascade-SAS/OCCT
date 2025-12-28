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
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

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

occ::handle<Geom_BSplineSurface> CreateSimpleBSplineSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1)); // Non-planar corner

  NCollection_Array1<double> aUKnots(1, 2);
  NCollection_Array1<double> aVKnots(1, 2);
  NCollection_Array1<int>    aUMults(1, 2);
  NCollection_Array1<int>    aVMults(1, 2);

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

occ::handle<Geom_BSplineSurface> CreateRationalBSplineSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);

  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      double x = (i - 1) * 1.0;
      double y = (j - 1) * 1.0;
      double z = (i == 2 && j == 2) ? 1.0 : 0.0;
      aPoles.SetValue(i, j, gp_Pnt(x, y, z));
      aWeights.SetValue(i, j, (i == 2 && j == 2) ? 2.0 : 1.0);
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
  aUMults.SetValue(1, 3);
  aUMults.SetValue(2, 3);
  aVMults.SetValue(1, 3);
  aVMults.SetValue(2, 3);

  return new Geom_BSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

occ::handle<Geom_BSplineSurface> CreateMultiSpanBSplineSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 5, 1, 5);

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

  NCollection_Array1<double> aUKnots(1, 3);
  NCollection_Array1<double> aVKnots(1, 3);
  NCollection_Array1<int>    aUMults(1, 3);
  NCollection_Array1<int>    aVMults(1, 3);

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

//==================================================================================================
// Basic Evaluation Tests
//==================================================================================================

TEST(GeomGridEval_BSplineSurfaceTest, BasicEvaluation)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

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
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);

  NCollection_Array1<double> aUParams(1, 2);
  NCollection_Array1<double> aVParams(1, 2);
  aUParams.SetValue(1, 0.0);
  aUParams.SetValue(2, 1.0);
  aVParams.SetValue(1, 0.0);
  aVParams.SetValue(2, 1.0);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  EXPECT_NEAR(aGrid.Value(1, 1).Distance(gp_Pnt(0, 0, 0)), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2, 1).Distance(gp_Pnt(1, 0, 0)), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(1, 2).Distance(gp_Pnt(0, 1, 0)), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2, 2).Distance(gp_Pnt(1, 1, 1)), 0.0, THE_TOLERANCE);
}

TEST(GeomGridEval_BSplineSurfaceTest, RationalSurface)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateRationalBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 11);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

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
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();

  GeomGridEval_BSplineSurface anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 21);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);

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

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);

  GeomGridEval_BSplineSurface anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 17);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 17);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

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
// Isoline Evaluation Tests
//==================================================================================================

TEST(GeomGridEval_BSplineSurfaceTest, IsolineU_CompareToGeomD0)
{
  occ::handle<Geom_BSplineSurface> aBSpline = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aBSpline);

  // U-isoline: 1 U param, multiple V params (triggers isoline path)
  NCollection_Array1<double> aUParams(1, 1);
  aUParams.SetValue(1, 0.5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 15);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  // Compare against Geom_BSplineSurface::D0
  // Note: ColLength() = number of U params (rows), RowLength() = number of V params (columns)
  const int aNbU = aGrid.ColLength();
  const int aNbV = aGrid.RowLength();
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      gp_Pnt aExpected;
      aBSpline->D0(aUParams.Value(iU), aVParams.Value(iV), aExpected);
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, IsolineV_CompareToGeomD0)
{
  occ::handle<Geom_BSplineSurface> aBSpline = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aBSpline);

  // V-isoline: multiple U params, 1 V param (triggers isoline path)
  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 15);
  NCollection_Array1<double> aVParams(1, 1);
  aVParams.SetValue(1, 0.7);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  // Compare against Geom_BSplineSurface::D0
  const int aNbU = aGrid.ColLength();
  const int aNbV = aGrid.RowLength();
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      gp_Pnt aExpected;
      aBSpline->D0(aUParams.Value(iU), aVParams.Value(iV), aExpected);
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, IsolineMultiSpan_CompareToGeomD0)
{
  occ::handle<Geom_BSplineSurface> aBSpline = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aBSpline);

  // U-isoline on multi-span surface
  NCollection_Array1<double> aUParams(1, 1);
  aUParams.SetValue(1, 0.35);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 20);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  // Compare against Geom_BSplineSurface::D0
  const int aNbU = aGrid.ColLength();
  const int aNbV = aGrid.RowLength();
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      gp_Pnt aExpected;
      aBSpline->D0(aUParams.Value(iU), aVParams.Value(iV), aExpected);
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, IsolineRational_CompareToGeomD0)
{
  occ::handle<Geom_BSplineSurface> aBSpline = CreateRationalBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aBSpline);

  // V-isoline on rational surface
  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 15);
  NCollection_Array1<double> aVParams(1, 1);
  aVParams.SetValue(1, 0.3);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  // Compare against Geom_BSplineSurface::D0
  const int aNbU = aGrid.ColLength();
  const int aNbV = aGrid.RowLength();
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      gp_Pnt aExpected;
      aBSpline->D0(aUParams.Value(iU), aVParams.Value(iV), aExpected);
      EXPECT_NEAR(aGrid.Value(iU, iV).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Derivative Tests
//==================================================================================================

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeD1)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1(aUParams, aVParams);

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
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2(aUParams, aVParams);

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

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeD3)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 11);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array2<GeomGridEval::SurfD3> aGrid = anEval.EvaluateGridD3(aUParams, aVParams);

  GeomAdaptor_Surface anAdaptor(aSurf);
  for (int iU = 1; iU <= 11; ++iU)
  {
    for (int iV = 1; iV <= 11; ++iV)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      anAdaptor.D3(aUParams.Value(iU),
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

//==================================================================================================
// DN (Arbitrary Order Derivative) Tests
//==================================================================================================

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeDN_U1V0)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aUParams, aVParams, 1, 0);

  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Vec aExpected = aSurf->DN(aUParams.Value(iU), aVParams.Value(iV), 1, 0);
      EXPECT_NEAR((aGrid.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeDN_U0V1)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aUParams, aVParams, 0, 1);

  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Vec aExpected = aSurf->DN(aUParams.Value(iU), aVParams.Value(iV), 0, 1);
      EXPECT_NEAR((aGrid.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeDN_U1V1)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aUParams, aVParams, 1, 1);

  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      gp_Vec aExpected = aSurf->DN(aUParams.Value(iU), aVParams.Value(iV), 1, 1);
      EXPECT_NEAR((aGrid.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeDN_BeyondDegree)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aUParams, aVParams, 2, 0);

  for (int iU = 1; iU <= 5; ++iU)
  {
    for (int iV = 1; iV <= 5; ++iV)
    {
      EXPECT_NEAR(aGrid.Value(iU, iV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeDN_MultiSpan)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 11);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 11);

  const int aTestCases[][2] = {{1, 0}, {0, 1}, {1, 1}, {2, 0}, {0, 2}, {2, 1}, {1, 2}};

  for (const auto& aCase : aTestCases)
  {
    const int aNU = aCase[0];
    const int aNV = aCase[1];

    NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aUParams, aVParams, aNU, aNV);

    for (int iU = 1; iU <= 11; ++iU)
    {
      for (int iV = 1; iV <= 11; ++iV)
      {
        gp_Vec aExpected = aSurf->DN(aUParams.Value(iU), aVParams.Value(iV), aNU, aNV);
        EXPECT_NEAR((aGrid.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
      }
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, DerivativeDN_RationalSurface)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateRationalBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 7);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 7);

  for (int aNU = 0; aNU <= 2; ++aNU)
  {
    for (int aNV = 0; aNV <= 2; ++aNV)
    {
      if (aNU + aNV == 0)
      {
        continue;
      }

      NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aUParams, aVParams, aNU, aNV);

      for (int iU = 1; iU <= 7; ++iU)
      {
        for (int iV = 1; iV <= 7; ++iV)
        {
          gp_Vec aExpected = aSurf->DN(aUParams.Value(iU), aVParams.Value(iV), aNU, aNV);
          EXPECT_NEAR((aGrid.Value(iU, iV) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
        }
      }
    }
  }
}

//==================================================================================================
// UV Pairs Evaluation Tests
//==================================================================================================

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_BasicEvaluation)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  // Create arbitrary UV pairs
  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(0.5, 0.25));
  aUVPairs.SetValue(3, gp_Pnt2d(1.0, 1.0));
  aUVPairs.SetValue(4, gp_Pnt2d(0.3, 0.7));
  aUVPairs.SetValue(5, gp_Pnt2d(0.8, 0.2));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);
  EXPECT_EQ(aPoints.Size(), 5);

  // Verify results against Geom_BSplineSurface::D0
  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aExpected;
    aSurf->D0(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aExpected);
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_MultiSpan)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  // Create UV pairs that span multiple knot spans
  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 10);
  aUVPairs.SetValue(1, gp_Pnt2d(0.1, 0.1));
  aUVPairs.SetValue(2, gp_Pnt2d(0.6, 0.3)); // Different U span
  aUVPairs.SetValue(3, gp_Pnt2d(0.2, 0.7)); // Different V span
  aUVPairs.SetValue(4, gp_Pnt2d(0.8, 0.9)); // Both different
  aUVPairs.SetValue(5, gp_Pnt2d(0.45, 0.45));
  aUVPairs.SetValue(6, gp_Pnt2d(0.55, 0.55));
  aUVPairs.SetValue(7, gp_Pnt2d(0.3, 0.4));
  aUVPairs.SetValue(8, gp_Pnt2d(0.7, 0.2));
  aUVPairs.SetValue(9, gp_Pnt2d(0.9, 0.1));
  aUVPairs.SetValue(10, gp_Pnt2d(0.05, 0.95));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  for (int i = 1; i <= 10; ++i)
  {
    gp_Pnt aExpected;
    aSurf->D0(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aExpected);
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_Rational)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateRationalBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 7);
  aUVPairs.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aUVPairs.SetValue(2, gp_Pnt2d(0.5, 0.5));
  aUVPairs.SetValue(3, gp_Pnt2d(1.0, 1.0));
  aUVPairs.SetValue(4, gp_Pnt2d(0.25, 0.75));
  aUVPairs.SetValue(5, gp_Pnt2d(0.75, 0.25));
  aUVPairs.SetValue(6, gp_Pnt2d(0.33, 0.67));
  aUVPairs.SetValue(7, gp_Pnt2d(0.67, 0.33));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  for (int i = 1; i <= 7; ++i)
  {
    gp_Pnt aExpected;
    aSurf->D0(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aExpected);
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_DerivativeD1)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 4);
  aUVPairs.SetValue(1, gp_Pnt2d(0.2, 0.3));
  aUVPairs.SetValue(2, gp_Pnt2d(0.5, 0.5));
  aUVPairs.SetValue(3, gp_Pnt2d(0.8, 0.1));
  aUVPairs.SetValue(4, gp_Pnt2d(0.4, 0.9));

  NCollection_Array1<GeomGridEval::SurfD1> aResults = anEval.EvaluatePointsD1(aUVPairs);

  for (int i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V;
    aSurf->D1(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aPnt, aD1U, aD1V);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_DerivativeD2)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 5);
  aUVPairs.SetValue(1, gp_Pnt2d(0.1, 0.2));
  aUVPairs.SetValue(2, gp_Pnt2d(0.6, 0.7));
  aUVPairs.SetValue(3, gp_Pnt2d(0.3, 0.8));
  aUVPairs.SetValue(4, gp_Pnt2d(0.9, 0.4));
  aUVPairs.SetValue(5, gp_Pnt2d(0.45, 0.55));

  NCollection_Array1<GeomGridEval::SurfD2> aResults = anEval.EvaluatePointsD2(aUVPairs);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
    aSurf->D2(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_DerivativeD3)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);
  GeomAdaptor_Surface              anAdaptor(aSurf);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 4);
  aUVPairs.SetValue(1, gp_Pnt2d(0.15, 0.25));
  aUVPairs.SetValue(2, gp_Pnt2d(0.65, 0.75));
  aUVPairs.SetValue(3, gp_Pnt2d(0.35, 0.85));
  aUVPairs.SetValue(4, gp_Pnt2d(0.85, 0.35));

  NCollection_Array1<GeomGridEval::SurfD3> aResults = anEval.EvaluatePointsD3(aUVPairs);

  for (int i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
    anAdaptor.D3(aUVPairs.Value(i).X(),
                 aUVPairs.Value(i).Y(),
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
    EXPECT_NEAR(aResults.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3U - aD3U).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3V - aD3V).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3UUV - aD3UUV).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aResults.Value(i).D3UVV - aD3UVV).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_DerivativeDN)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 6);
  aUVPairs.SetValue(1, gp_Pnt2d(0.1, 0.1));
  aUVPairs.SetValue(2, gp_Pnt2d(0.3, 0.5));
  aUVPairs.SetValue(3, gp_Pnt2d(0.6, 0.2));
  aUVPairs.SetValue(4, gp_Pnt2d(0.8, 0.8));
  aUVPairs.SetValue(5, gp_Pnt2d(0.4, 0.9));
  aUVPairs.SetValue(6, gp_Pnt2d(0.9, 0.3));

  const int aTestCases[][2] = {{1, 0}, {0, 1}, {1, 1}, {2, 0}, {0, 2}};

  for (const auto& aCase : aTestCases)
  {
    const int aNU = aCase[0];
    const int aNV = aCase[1];

    NCollection_Array1<gp_Vec> aResults = anEval.EvaluatePointsDN(aUVPairs, aNU, aNV);

    for (int i = 1; i <= 6; ++i)
    {
      gp_Vec aExpected = aSurf->DN(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aNU, aNV);
      EXPECT_NEAR((aResults.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_OrderPreservation)
{
  // Verify that results are returned in original input order regardless of sorting
  occ::handle<Geom_BSplineSurface> aSurf = CreateMultiSpanBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  // Create UV pairs in a specific order that will be reordered during sorting
  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 6);
  aUVPairs.SetValue(1, gp_Pnt2d(0.9, 0.1)); // High U span
  aUVPairs.SetValue(2, gp_Pnt2d(0.1, 0.9)); // Low U span
  aUVPairs.SetValue(3, gp_Pnt2d(0.5, 0.5)); // Middle
  aUVPairs.SetValue(4, gp_Pnt2d(0.2, 0.2)); // Low spans
  aUVPairs.SetValue(5, gp_Pnt2d(0.8, 0.8)); // High spans
  aUVPairs.SetValue(6, gp_Pnt2d(0.4, 0.6)); // Mixed

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  // Verify each result corresponds to its original input index
  for (int i = 1; i <= 6; ++i)
  {
    gp_Pnt aExpected;
    aSurf->D0(aUVPairs.Value(i).X(), aUVPairs.Value(i).Y(), aExpected);
    EXPECT_NEAR(aPoints.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE)
      << "Mismatch at index " << i;
  }
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_EmptyInput)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<gp_Pnt2d> aEmptyPairs;

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aEmptyPairs);
  EXPECT_TRUE(aPoints.IsEmpty());
}

TEST(GeomGridEval_BSplineSurfaceTest, UVPairs_SinglePoint)
{
  occ::handle<Geom_BSplineSurface> aSurf = CreateSimpleBSplineSurface();
  GeomGridEval_BSplineSurface      anEval(aSurf);

  NCollection_Array1<gp_Pnt2d> aUVPairs(1, 1);
  aUVPairs.SetValue(1, gp_Pnt2d(0.5, 0.5));

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluatePoints(aUVPairs);

  EXPECT_EQ(aPoints.Size(), 1);

  gp_Pnt aExpected;
  aSurf->D0(0.5, 0.5, aExpected);
  EXPECT_NEAR(aPoints.Value(1).Distance(aExpected), 0.0, THE_TOLERANCE);
}

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
#include <GeomGridEval_BezierSurface.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

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

TEST(GeomGridEval_BezierSurfaceTest, BasicEvaluation)
{
  // Simple Bezier surface 2x2 (planar)
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 0));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);

  GeomGridEval_BezierSurface anEval(aBezier);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 3);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_EQ(aGrid.RowLength(), 3);
  EXPECT_EQ(aGrid.ColLength(), 3);

  // Verify points
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Pnt aExpected = aBezier->Value(aParams.Value(i), aParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, RationalEvaluation)
{
  // Rational Bezier surface
  TColgp_Array2OfPnt   aPoles(1, 2, 1, 2);
  TColStd_Array2OfReal aWeights(1, 2, 1, 2);

  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1)); // Elevated corner

  aWeights.SetValue(1, 1, 1.0);
  aWeights.SetValue(2, 1, 1.0);
  aWeights.SetValue(1, 2, 1.0);
  aWeights.SetValue(2, 2, 2.0); // Higher weight

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface anEval(aBezier);

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

TEST(GeomGridEval_BezierSurfaceTest, DerivativeD1)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      aBezier->D1(aParams.Value(i), aParams.Value(j), aPnt, aD1U, aD1V);
      EXPECT_NEAR(aGrid.Value(i, j).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeD2)
{
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aBezier->D2(aParams.Value(i), aParams.Value(j), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      EXPECT_NEAR(aGrid.Value(i, j).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeD3)
{
  TColgp_Array2OfPnt aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
    for (int j = 1; j <= 4; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<GeomGridEval::SurfD3> aGrid = anEval.EvaluateGridD3();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      aBezier->D3(aParams.Value(i),
                  aParams.Value(j),
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
      EXPECT_NEAR(aGrid.Value(i, j).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3U - aD3U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3V - aD3V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3UUV - aD3UUV).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3UVV - aD3UVV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeDN_U1V0)
{
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(1, 0);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Vec aExpected = aBezier->DN(aParams.Value(i), aParams.Value(j), 1, 0);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeDN_U0V1)
{
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(0, 1);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Vec aExpected = aBezier->DN(aParams.Value(i), aParams.Value(j), 0, 1);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeDN_U2V0)
{
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(2, 0);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Vec aExpected = aBezier->DN(aParams.Value(i), aParams.Value(j), 2, 0);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeDN_U1V1)
{
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(1, 1);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Vec aExpected = aBezier->DN(aParams.Value(i), aParams.Value(j), 1, 1);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeDN_BeyondDegree)
{
  // Biquadratic Bezier (degree 2 in both directions)
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  // 3rd derivative in U direction (beyond degree 2) should be zero
  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(3, 0);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      EXPECT_NEAR(aGrid.Value(i, j).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeDN_RationalSurface)
{
  // Rational Bezier surface
  TColgp_Array2OfPnt   aPoles(1, 2, 1, 2);
  TColStd_Array2OfReal aWeights(1, 2, 1, 2);

  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1));

  aWeights.SetValue(1, 1, 1.0);
  aWeights.SetValue(2, 1, 1.0);
  aWeights.SetValue(1, 2, 1.0);
  aWeights.SetValue(2, 2, 2.0);

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface anEval(aBezier);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 5);
  anEval.SetUVParams(aParams, aParams);

  // Test DN(1,0), DN(0,1), and DN(1,1)
  for (int aNU = 0; aNU <= 1; ++aNU)
  {
    for (int aNV = 0; aNV <= 1; ++aNV)
    {
      if (aNU + aNV == 0)
        continue;

      NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aNU, aNV);

      for (int i = 1; i <= 5; ++i)
      {
        for (int j = 1; j <= 5; ++j)
        {
          gp_Vec aExpected = aBezier->DN(aParams.Value(i), aParams.Value(j), aNU, aNV);
          EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
        }
      }
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, IsolineU_CompareToGeomD0)
{
  // Create a non-planar Bezier surface
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
    }
  }

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  // U-isoline: 1 U param, multiple V params (triggers isoline path)
  TColStd_Array1OfReal aUParams(1, 1);
  aUParams.SetValue(1, 0.5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 10);

  anEval.SetUVParams(aUParams, aVParams);
  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  EXPECT_EQ(aGrid.RowLength(), 1);
  EXPECT_EQ(aGrid.ColLength(), 10);

  // Compare against Geom_Surface::D0
  for (int j = 1; j <= 10; ++j)
  {
    gp_Pnt aExpected;
    aBezier->D0(aUParams.Value(1), aVParams.Value(j), aExpected);
    EXPECT_NEAR(aGrid.Value(1, j).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierSurfaceTest, IsolineV_CompareToGeomD0)
{
  // Create a non-planar Bezier surface
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
    }
  }

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface anEval(aBezier);

  // V-isoline: multiple U params, 1 V param (triggers isoline path)
  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 10);
  TColStd_Array1OfReal aVParams(1, 1);
  aVParams.SetValue(1, 0.7);

  anEval.SetUVParams(aUParams, aVParams);
  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  EXPECT_EQ(aGrid.RowLength(), 10);
  EXPECT_EQ(aGrid.ColLength(), 1);

  // Compare against Geom_Surface::D0
  for (int i = 1; i <= 10; ++i)
  {
    gp_Pnt aExpected;
    aBezier->D0(aUParams.Value(i), aVParams.Value(1), aExpected);
    EXPECT_NEAR(aGrid.Value(i, 1).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierSurfaceTest, IsolineRational_CompareToGeomD0)
{
  // Rational Bezier surface
  TColgp_Array2OfPnt   aPoles(1, 3, 1, 3);
  TColStd_Array2OfReal aWeights(1, 3, 1, 3);

  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
      aWeights.SetValue(i, j, 1.0 + 0.5 * ((i - 1) + (j - 1)));
    }
  }

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface anEval(aBezier);

  // U-isoline on rational surface
  TColStd_Array1OfReal aUParams(1, 1);
  aUParams.SetValue(1, 0.3);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 15);

  anEval.SetUVParams(aUParams, aVParams);
  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  for (int j = 1; j <= 15; ++j)
  {
    gp_Pnt aExpected;
    aBezier->D0(aUParams.Value(1), aVParams.Value(j), aExpected);
    EXPECT_NEAR(aGrid.Value(1, j).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

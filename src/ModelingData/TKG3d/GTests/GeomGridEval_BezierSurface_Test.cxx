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
#include <NCollection_Array2.hxx>
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
} // namespace

TEST(GeomGridEval_BezierSurfaceTest, BasicEvaluation)
{
  // Simple Bezier surface 2x2 (planar)
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 0));

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);

  GeomGridEval_BezierSurface anEval(aBezier);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 3);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams, aParams);
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

TEST(GeomGridEval_BezierSurfaceTest, SinglePointGrid)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int aUIndex = 1; aUIndex <= 3; ++aUIndex)
  {
    for (int aVIndex = 1; aVIndex <= 3; ++aVIndex)
    {
      aPoles.SetValue(aUIndex, aVIndex, gp_Pnt(aUIndex, aVIndex, std::sin(aUIndex + aVIndex)));
    }
  }

  const occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface            anEvaluator(aSurface);
  NCollection_Array1<double>            aUParams(1);
  NCollection_Array1<double>            aVParams(1);
  aUParams.ChangeAt(0) = 0.37;
  aVParams.ChangeAt(0) = 0.63;

  const gp_Pnt               aPoint       = anEvaluator.EvaluateGrid(aUParams, aVParams).At(0);
  const GeomGridEval::SurfD1 aD1          = anEvaluator.EvaluateGridD1(aUParams, aVParams).At(0);
  const GeomGridEval::SurfD2 aD2          = anEvaluator.EvaluateGridD2(aUParams, aVParams).At(0);
  const Geom_Surface::ResD1  anExpectedD1 = aSurface->EvalD1(0.37, 0.63);
  const Geom_Surface::ResD2  anExpectedD2 = aSurface->EvalD2(0.37, 0.63);
  EXPECT_NEAR(aPoint.Distance(anExpectedD1.Point), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aD1.Point.Distance(anExpectedD1.Point), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD1.D1U - anExpectedD1.D1U).Magnitude(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD1.D1V - anExpectedD1.D1V).Magnitude(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aD2.Point.Distance(anExpectedD2.Point), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD2.D1U - anExpectedD2.D1U).Magnitude(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD2.D1V - anExpectedD2.D1V).Magnitude(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD2.D2U - anExpectedD2.D2U).Magnitude(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD2.D2V - anExpectedD2.D2V).Magnitude(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((aD2.D2UV - anExpectedD2.D2UV).Magnitude(), 0.0, THE_TOLERANCE);
}

TEST(GeomGridEval_BezierSurfaceTest, RationalEvaluation)
{
  // Rational Bezier surface
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);

  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1)); // Elevated corner

  aWeights.SetValue(1, 1, 1.0);
  aWeights.SetValue(2, 1, 1.0);
  aWeights.SetValue(1, 2, 1.0);
  aWeights.SetValue(2, 2, 2.0); // Higher weight

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams, aParams);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1));

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1(aParams, aParams);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2(aParams, aParams);

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

TEST(GeomGridEval_BezierSurfaceTest, DerivativeD2RationalLinear)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(1.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(2, 2, gp_Pnt(1.0, 1.0, 1.0));
  aWeights.SetValue(1, 1, 1.0);
  aWeights.SetValue(2, 1, 1.5);
  aWeights.SetValue(1, 2, 2.0);
  aWeights.SetValue(2, 2, 0.75);

  const occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface            anEval(aSurface);
  NCollection_Array1<double>            aParams        = CreateUniformParams(0.0, 1.0, 7);
  const NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2(aParams, aParams);
  for (size_t aUIndex = 0; aUIndex < aParams.Size(); ++aUIndex)
  {
    for (size_t aVIndex = 0; aVIndex < aParams.Size(); ++aVIndex)
    {
      const Geom_Surface::ResD2 anExpected =
        aSurface->EvalD2(aParams.At(aUIndex), aParams.At(aVIndex));
      const size_t                anIndex = aUIndex * aParams.Size() + aVIndex;
      const GeomGridEval::SurfD2& aResult = aGrid.At(anIndex);
      EXPECT_NEAR(aResult.Point.Distance(anExpected.Point), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D1U - anExpected.D1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D1V - anExpected.D1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D2U - anExpected.D2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D2V - anExpected.D2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D2UV - anExpected.D2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BezierSurfaceTest, DerivativeD3)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<GeomGridEval::SurfD3> aGrid = anEval.EvaluateGridD3(aParams, aParams);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aParams, 1, 0);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aParams, 0, 1);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aParams, 2, 0);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aParams, 1, 1);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i, j, std::sin(i + j)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  // 3rd derivative in U direction (beyond degree 2) should be zero
  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aParams, 3, 0);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);

  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 1, 0));
  aPoles.SetValue(2, 2, gp_Pnt(1, 1, 1));

  aWeights.SetValue(1, 1, 1.0);
  aWeights.SetValue(2, 1, 1.0);
  aWeights.SetValue(1, 2, 1.0);
  aWeights.SetValue(2, 2, 2.0);

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface      anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  // Test DN(1,0), DN(0,1), and DN(1,1)
  for (int aNU = 0; aNU <= 1; ++aNU)
  {
    for (int aNV = 0; aNV <= 1; ++aNV)
    {
      if (aNU + aNV == 0)
      {
        continue;
      }

      NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aParams, aNU, aNV);

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
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  // U-isoline: 1 U param, multiple V params (triggers isoline path)
  NCollection_Array1<double> aUParams(1, 1);
  aUParams.SetValue(1, 0.5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 192);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  // Note: RowLength() = V count (columns), ColLength() = U count (rows)
  EXPECT_EQ(aGrid.RowLength(), 192);
  EXPECT_EQ(aGrid.ColLength(), 1);

  // Compare against Geom_Surface::D0
  for (int j = 1; j <= 192; ++j)
  {
    gp_Pnt aExpected;
    aBezier->D0(aUParams.Value(1), aVParams.Value(j), aExpected);
    EXPECT_NEAR(aGrid.Value(1, j).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierSurfaceTest, IsolineV_CompareToGeomD0)
{
  // Create a non-planar Bezier surface
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomGridEval_BezierSurface      anEval(aBezier);

  // V-isoline: multiple U params, 1 V param (triggers isoline path)
  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 1.0, 192);
  NCollection_Array1<double> aVParams(1, 1);
  aVParams.SetValue(1, 0.7);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  // Note: RowLength() = V count (columns), ColLength() = U count (rows)
  EXPECT_EQ(aGrid.RowLength(), 1);
  EXPECT_EQ(aGrid.ColLength(), 192);

  // Compare against Geom_Surface::D0
  for (int i = 1; i <= 192; ++i)
  {
    gp_Pnt aExpected;
    aBezier->D0(aUParams.Value(i), aVParams.Value(1), aExpected);
    EXPECT_NEAR(aGrid.Value(i, 1).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierSurfaceTest, IsolineRational_CompareToGeomD0)
{
  // Rational Bezier surface
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);

  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(i - 1, j - 1, std::sin((i - 1) * 0.5 + (j - 1) * 0.5)));
      aWeights.SetValue(i, j, 1.0 + 0.5 * ((i - 1) + (j - 1)));
    }
  }

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles, aWeights);
  GeomGridEval_BezierSurface      anEval(aBezier);

  // U-isoline on rational surface
  NCollection_Array1<double> aUParams(1, 1);
  aUParams.SetValue(1, 0.3);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 1.0, 16);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int j = 1; j <= 16; ++j)
  {
    gp_Pnt aExpected;
    aBezier->D0(aUParams.Value(1), aVParams.Value(j), aExpected);
    EXPECT_NEAR(aGrid.Value(1, j).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

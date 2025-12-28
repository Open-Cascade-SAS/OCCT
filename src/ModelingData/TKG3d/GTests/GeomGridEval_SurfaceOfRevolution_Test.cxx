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

#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_Surface.hxx>
#include <GeomGridEval_SurfaceOfRevolution.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-9;

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

TEST(GeomGridEval_SurfaceOfRevolutionTest, BasicEvaluation)
{
  // Create a line parallel to Y axis at X=5 (meridian)
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  // Revolve around Z axis -> Creates a cylinder of radius 5
  gp_Ax1                                aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aLine, aRevAxis);

  GeomGridEval_SurfaceOfRevolution anEval(aRevSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 9; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = aRevSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);

      // Check cylindrical radius
      double aRadius = std::sqrt(aGrid.Value(i, j).X() * aGrid.Value(i, j).X()
                                 + aGrid.Value(i, j).Y() * aGrid.Value(i, j).Y());
      EXPECT_NEAR(aRadius, 5.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfRevolutionTest, CircleMeridian)
{
  // Create a circle in XZ plane with center at (10, 0, 0) and radius 3
  // This creates a torus when revolved
  gp_Ax2                   aCircleAx(gp_Pnt(10.0, 0.0, 0.0), gp_Dir(0, 1, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircleAx, 3.0);

  // Revolve around Z axis
  gp_Ax1                                aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aCircle, aRevAxis);

  GeomGridEval_SurfaceOfRevolution anEval(aRevSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 9; ++i)
  {
    for (int j = 1; j <= 9; ++j)
    {
      gp_Pnt aExpected = aRevSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfRevolutionTest, DerivativeD1)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1                 aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aLine, aRevAxis);

  GeomGridEval_SurfaceOfRevolution anEval(aRevSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<GeomGridEval::SurfD1> aGridD1 = anEval.EvaluateGridD1(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V;
      aRevSurf->D1(aUParams.Value(i), aVParams.Value(j), aP, aD1U, aD1V);

      const GeomGridEval::SurfD1& aData = aGridD1.Value(i, j);
      EXPECT_NEAR(aData.Point.Distance(aP), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfRevolutionTest, DerivativeD2)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1                 aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aLine, aRevAxis);

  GeomGridEval_SurfaceOfRevolution anEval(aRevSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<GeomGridEval::SurfD2> aGridD2 = anEval.EvaluateGridD2(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aRevSurf->D2(aUParams.Value(i), aVParams.Value(j), aP, aD1U, aD1V, aD2U, aD2V, aD2UV);

      const GeomGridEval::SurfD2& aData = aGridD2.Value(i, j);
      EXPECT_NEAR(aData.Point.Distance(aP), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfRevolutionTest, DerivativeD3)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1                 aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aLine, aRevAxis);

  GeomGridEval_SurfaceOfRevolution anEval(aRevSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<GeomGridEval::SurfD3> aGridD3 = anEval.EvaluateGridD3(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      aRevSurf->D3(aUParams.Value(i),
                   aVParams.Value(j),
                   aP,
                   aD1U,
                   aD1V,
                   aD2U,
                   aD2V,
                   aD2UV,
                   aD3U,
                   aD3V,
                   aD3UUV,
                   aD3UVV);

      const GeomGridEval::SurfD3& aData = aGridD3.Value(i, j);
      EXPECT_NEAR(aData.Point.Distance(aP), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D3U - aD3U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D3V - aD3V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D3UUV - aD3UUV).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D3UVV - aD3UVV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfRevolutionTest, UnifiedDispatch)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1                 aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aLine, aRevAxis);

  // Test dispatch via unified evaluator
  GeomGridEval_Surface anEval;
  anEval.Initialize(aRevSurf);

  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_SurfaceOfRevolution);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = aRevSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfRevolutionTest, AdaptorDispatch)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1                 aRevAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf = new Geom_SurfaceOfRevolution(aLine, aRevAxis);

  // Test dispatch via adaptor
  GeomAdaptor_Surface  anAdaptor(aRevSurf);
  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);

  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_SurfaceOfRevolution);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = aRevSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

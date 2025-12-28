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
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_Surface.hxx>
#include <GeomGridEval_SurfaceOfExtrusion.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
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
  const double         aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}
} // namespace

TEST(GeomGridEval_SurfaceOfExtrusionTest, BasicEvaluation)
{
  // Create a line along X axis
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));

  // Extrude along Z axis -> Creates a plane
  gp_Dir                                aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aLine, aDirection);

  GeomGridEval_SurfaceOfExtrusion anEval(anExtSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 10.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anExtSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);

      // Verify: S(u,v) = C(u) + v * Dir = (u, 0, 0) + v * (0, 0, 1) = (u, 0, v)
      EXPECT_NEAR(aGrid.Value(i, j).X(), aUParams.Value(i), THE_TOLERANCE);
      EXPECT_NEAR(aGrid.Value(i, j).Y(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aGrid.Value(i, j).Z(), aVParams.Value(j), THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, CircleBasisCurve)
{
  // Create a circle in XY plane with center at origin and radius 5
  gp_Ax2              aCircleAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircleAx, 5.0);

  // Extrude along Z axis -> Creates a cylinder
  gp_Dir                                aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aCircle, aDirection);

  GeomGridEval_SurfaceOfExtrusion anEval(anExtSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 10.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 9; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anExtSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);

      // Check cylindrical radius
      double aRadius = std::sqrt(aGrid.Value(i, j).X() * aGrid.Value(i, j).X()
                                 + aGrid.Value(i, j).Y() * aGrid.Value(i, j).Y());
      EXPECT_NEAR(aRadius, 5.0, THE_TOLERANCE);

      // Check Z coordinate equals V parameter
      EXPECT_NEAR(aGrid.Value(i, j).Z(), aVParams.Value(j), THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, DerivativeD1)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir            aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aLine, aDirection);

  GeomGridEval_SurfaceOfExtrusion anEval(anExtSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 10.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<GeomGridEval::SurfD1> aGridD1 = anEval.EvaluateGridD1(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V;
      anExtSurf->D1(aUParams.Value(i), aVParams.Value(j), aP, aD1U, aD1V);

      const GeomGridEval::SurfD1& aData = aGridD1.Value(i, j);
      EXPECT_NEAR(aData.Point.Distance(aP), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);

      // D1V should be the extrusion direction
      EXPECT_NEAR(aData.D1V.X(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aData.D1V.Y(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aData.D1V.Z(), 1.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, DerivativeD2)
{
  // Use circle for non-trivial D2U
  gp_Ax2              aCircleAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircleAx, 5.0);
  gp_Dir              aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aCircle, aDirection);

  GeomGridEval_SurfaceOfExtrusion anEval(anExtSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<GeomGridEval::SurfD2> aGridD2 = anEval.EvaluateGridD2(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      anExtSurf->D2(aUParams.Value(i), aVParams.Value(j), aP, aD1U, aD1V, aD2U, aD2V, aD2UV);

      const GeomGridEval::SurfD2& aData = aGridD2.Value(i, j);
      EXPECT_NEAR(aData.Point.Distance(aP), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aData.D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);

      // D2V and D2UV should be zero for extrusion surface
      EXPECT_NEAR(aData.D2V.Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aData.D2UV.Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, DerivativeD3)
{
  // Use circle for non-trivial D3U
  gp_Ax2              aCircleAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircleAx, 5.0);
  gp_Dir              aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aCircle, aDirection);

  GeomGridEval_SurfaceOfExtrusion anEval(anExtSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, M_PI, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<GeomGridEval::SurfD3> aGridD3 = anEval.EvaluateGridD3(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      anExtSurf->D3(aUParams.Value(i),
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

      // D3V, D3UUV, D3UVV should all be zero for extrusion surface
      EXPECT_NEAR(aData.D3V.Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aData.D3UUV.Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aData.D3UVV.Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, UnifiedDispatch)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir            aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aLine, aDirection);

  // Test dispatch via unified evaluator
  GeomGridEval_Surface anEval;
  anEval.Initialize(anExtSurf);

  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_SurfaceOfExtrusion);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 10.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anExtSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, AdaptorDispatch)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir            aDirection(0, 0, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aLine, aDirection);

  // Test dispatch via adaptor
  GeomAdaptor_Surface  anAdaptor(anExtSurf);
  GeomGridEval_Surface anEval;
  anEval.Initialize(anAdaptor);

  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_SurfaceOfExtrusion);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 10.0, 5);
  NCollection_Array1<double> aVParams = CreateUniformParams(0.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anExtSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_SurfaceOfExtrusionTest, NonAxisAlignedDirection)
{
  // Create a circle and extrude along a diagonal direction
  gp_Ax2              aCircleAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCircleAx, 3.0);

  // Extrude along (1, 1, 1) direction
  gp_Dir                                aDirection(1, 1, 1);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(aCircle, aDirection);

  GeomGridEval_SurfaceOfExtrusion anEval(anExtSurf);

  NCollection_Array1<double> aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  NCollection_Array1<double> aVParams = CreateUniformParams(-5.0, 5.0, 5);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);

  for (int i = 1; i <= 9; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anExtSurf->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
}

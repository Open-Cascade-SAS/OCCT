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

TEST(GeomGridEval_ConeTest, BasicEvaluation)
{
  // Cone: SemiAngle=PI/4, Radius=1.0 at origin, Center(0,0,0), Z-axis
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);

  GeomGridEval_Cone anEval(aCone);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9); // Angle
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);      // Linear param
  anEval.SetUVParams(aUParams, aVParams);

  EXPECT_EQ(anEval.NbUParams(), 9);
  EXPECT_EQ(anEval.NbVParams(), 6);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
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

TEST(GeomGridEval_ConeTest, DerivativeD1)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

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

TEST(GeomGridEval_ConeTest, DerivativeD2)
{
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4, 1.0);
  GeomGridEval_Cone anEval(aCone);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

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

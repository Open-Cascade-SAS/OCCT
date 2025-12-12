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

#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_OffsetSurface.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-9;

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

TEST(GeomGridEval_OffsetSurfaceTest, PlaneOffset)
{
  // Plane at Z=0
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Offset by 10.0 -> Plane at Z=10
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aPlane, 10.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 10.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 10.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anOffset->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(aGrid.Value(i, j).Z(), 10.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, CylinderOffset)
{
  // Cylinder radius 5
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Offset by 2.0 -> Cylinder radius 7 (outward)
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();

  for (int i = 1; i <= 9; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Pnt aExpected = anOffset->Value(aUParams.Value(i), aVParams.Value(j));
      EXPECT_NEAR(aGrid.Value(i, j).Distance(aExpected), 0.0, THE_TOLERANCE);

      // Check radius from Z axis
      double aDist = std::sqrt(aGrid.Value(i, j).X() * aGrid.Value(i, j).X()
                               + aGrid.Value(i, j).Y() * aGrid.Value(i, j).Y());
      EXPECT_NEAR(aDist, 7.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeD1)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aGrid = anEval.EvaluateGridD1();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V;
      anOffset->D1(aUParams.Value(i), aVParams.Value(j), aPnt, aD1U, aD1V);

      EXPECT_NEAR(aGrid.Value(i, j).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, NestedDispatch)
{
  // Test that GeomGridEval_Surface correctly dispatches to GeomGridEval_OffsetSurface
  Handle(Geom_Plane)         aPlane   = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aPlane, 5.0);

  GeomGridEval_Surface anEval;
  anEval.Initialize(anOffset);

  EXPECT_TRUE(anEval.IsInitialized());
  // Assuming GetType() returns OffsetSurface or similar identifier
  // (GeomGridEval_Surface::GetType logic might need checking, usually delegates to adaptor or
  // stored type) Since we added OffsetSurface to variant, it should be supported.

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 1.0, 3);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 1.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_EQ(aGrid.RowLength(), 3);
  EXPECT_EQ(aGrid.ColLength(), 3);

  EXPECT_NEAR(aGrid.Value(1, 1).Z(), 5.0, THE_TOLERANCE);
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeD2)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aGrid = anEval.EvaluateGridD2();

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      anOffset->D2(aUParams.Value(i), aVParams.Value(j), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);

      EXPECT_NEAR(aGrid.Value(i, j).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeD3)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<GeomGridEval::SurfD3> aGrid = anEval.EvaluateGridD3();

  GeomAdaptor_Surface anAdaptor(anOffset);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Pnt aPnt;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      anAdaptor.D3(aUParams.Value(i), aVParams.Value(j), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV,
                   aD3U, aD3V, aD3UUV, aD3UVV);

      EXPECT_NEAR(aGrid.Value(i, j).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1U - aD1U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D1V - aD1V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2U - aD2U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2V - aD2V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D2UV - aD2UV).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3U - aD3U).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3V - aD3V).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3UUV - aD3UUV).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i, j).D3UVV - aD3UVV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeDN_U1V0)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(1, 0);

  // Use Geom_OffsetSurface::DN as reference (GeomAdaptor_Surface::DN doesn't
  // properly handle offset surfaces - it returns base surface derivatives)
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Vec aExpected = anOffset->DN(aUParams.Value(i), aVParams.Value(j), 1, 0);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeDN_U0V1)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(0, 1);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Vec aExpected = anOffset->DN(aUParams.Value(i), aVParams.Value(j), 0, 1);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeDN_U1V1)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(1, 1);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Vec aExpected = anOffset->DN(aUParams.Value(i), aVParams.Value(j), 1, 1);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeDN_U2V0)
{
  // Cylinder offset
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aCyl, 2.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 2 * M_PI, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 5.0, 3);
  anEval.SetUVParams(aUParams, aVParams);

  NCollection_Array2<gp_Vec> aGrid = anEval.EvaluateGridDN(2, 0);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      gp_Vec aExpected = anOffset->DN(aUParams.Value(i), aVParams.Value(j), 2, 0);
      EXPECT_NEAR((aGrid.Value(i, j) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_OffsetSurfaceTest, DerivativeDN_PlaneOffset)
{
  // Plane offset - simpler case for testing
  Handle(Geom_Plane)         aPlane   = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aPlane, 10.0);

  GeomGridEval_OffsetSurface anEval(anOffset);

  TColStd_Array1OfReal aUParams = CreateUniformParams(0.0, 10.0, 5);
  TColStd_Array1OfReal aVParams = CreateUniformParams(0.0, 10.0, 5);
  anEval.SetUVParams(aUParams, aVParams);

  // Test DN(1,0) and DN(0,1) - use Geom_OffsetSurface::DN as reference
  NCollection_Array2<gp_Vec> aGridU = anEval.EvaluateGridDN(1, 0);
  NCollection_Array2<gp_Vec> aGridV = anEval.EvaluateGridDN(0, 1);

  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      gp_Vec aExpectedU = anOffset->DN(aUParams.Value(i), aVParams.Value(j), 1, 0);
      gp_Vec aExpectedV = anOffset->DN(aUParams.Value(i), aVParams.Value(j), 0, 1);
      EXPECT_NEAR((aGridU.Value(i, j) - aExpectedU).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGridV.Value(i, j) - aExpectedV).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }

  // For a plane offset, 2nd and higher derivatives should be zero
  NCollection_Array2<gp_Vec> aGrid2U = anEval.EvaluateGridDN(2, 0);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      EXPECT_NEAR(aGrid2U.Value(i, j).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

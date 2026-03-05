// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <PointSetLib_Equation.hxx>
#include <PointSetLib_Props.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(PointSetLib_PropsTest, EmptySet)
{
  PointSetLib_Props aProps;
  EXPECT_NEAR(aProps.Mass(), 0.0, Precision::Confusion());
}

TEST(PointSetLib_PropsTest, SinglePoint)
{
  PointSetLib_Props aProps;
  aProps.AddPoint(gp_Pnt(1.0, 2.0, 3.0));

  EXPECT_NEAR(aProps.Mass(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 3.0, Precision::Confusion());
}

TEST(PointSetLib_PropsTest, TwoPoints_Barycentre)
{
  PointSetLib_Props aProps;
  aProps.AddPoint(gp_Pnt(0.0, 0.0, 0.0));
  aProps.AddPoint(gp_Pnt(2.0, 4.0, 6.0));

  EXPECT_NEAR(aProps.Mass(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 3.0, Precision::Confusion());
}

TEST(PointSetLib_PropsTest, WeightedPoints)
{
  PointSetLib_Props aProps;
  aProps.AddPoint(gp_Pnt(0.0, 0.0, 0.0), 1.0);
  aProps.AddPoint(gp_Pnt(4.0, 0.0, 0.0), 3.0);

  EXPECT_NEAR(aProps.Mass(), 4.0, Precision::Confusion());
  // Barycentre at (3, 0, 0): (0*1 + 4*3) / 4 = 3
  EXPECT_NEAR(aProps.CentreOfMass().X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 0.0, Precision::Confusion());
}

TEST(PointSetLib_PropsTest, ArrayConstructor)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(-1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(0.0, -1.0, 0.0);

  PointSetLib_Props aProps(aPnts);
  EXPECT_NEAR(aProps.Mass(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 0.0, Precision::Confusion());
}

TEST(PointSetLib_PropsTest, StaticBarycentre)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 3);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(3.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 6.0, 0.0);

  const gp_Pnt aG = PointSetLib_Props::Barycentre(aPnts);
  EXPECT_NEAR(aG.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aG.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aG.Z(), 0.0, Precision::Confusion());
}

TEST(PointSetLib_PropsTest, MatrixOfInertia_SymmetricPoints)
{
  // Four points on X and Y axes at distance 1 from origin
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(-1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(0.0, -1.0, 0.0);

  PointSetLib_Props aProps(aPnts);
  const gp_Mat      anInertia = aProps.MatrixOfInertia();

  // Ixx = sum(yi^2 + zi^2) at centroid
  // For these points, centroid = origin, so inertia at origin = inertia at centroid
  // Ixx = 0+0+1+1 = 2, Iyy = 1+1+0+0 = 2, Izz = 1+1+1+1 = 4
  EXPECT_NEAR(anInertia.Value(1, 1), 2.0, Precision::Confusion()); // Ixx
  EXPECT_NEAR(anInertia.Value(2, 2), 2.0, Precision::Confusion()); // Iyy
  EXPECT_NEAR(anInertia.Value(3, 3), 4.0, Precision::Confusion()); // Izz
  // Off-diagonal should be zero by symmetry
  EXPECT_NEAR(anInertia.Value(1, 2), 0.0, Precision::Confusion()); // Ixy
  EXPECT_NEAR(anInertia.Value(1, 3), 0.0, Precision::Confusion()); // Ixz
  EXPECT_NEAR(anInertia.Value(2, 3), 0.0, Precision::Confusion()); // Iyz
}

// ============================================================================
// PointSetLib_Equation tests
// ============================================================================

TEST(PointSetLib_EquationTest, CoincidentPoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 3);
  aPnts(1) = gp_Pnt(1.0, 2.0, 3.0);
  aPnts(2) = gp_Pnt(1.0, 2.0, 3.0);
  aPnts(3) = gp_Pnt(1.0, 2.0, 3.0);

  PointSetLib_Equation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsPoint());

  const gp_Pnt aP = anEq.Point();
  EXPECT_NEAR(aP.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 3.0, Precision::Confusion());
}

TEST(PointSetLib_EquationTest, CollinearPoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 3);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(2.0, 0.0, 0.0);

  PointSetLib_Equation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsLinear());

  const gp_Lin aLin = anEq.Line();
  // Direction should be along X
  EXPECT_NEAR(std::abs(aLin.Direction().X()), 1.0, Precision::Confusion());
}

TEST(PointSetLib_EquationTest, CoplanarPoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(1.0, 1.0, 0.0);

  PointSetLib_Equation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsPlanar());

  const gp_Pln aPln = anEq.Plane();
  // Normal should be along Z
  EXPECT_NEAR(std::abs(aPln.Axis().Direction().Z()), 1.0, Precision::Confusion());
}

TEST(PointSetLib_EquationTest, SpacePoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(0.0, 0.0, 1.0);

  PointSetLib_Equation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsSpace());

  gp_Pnt aP;
  gp_Vec aV1, aV2, aV3;
  anEq.Box(aP, aV1, aV2, aV3);
  // Box should have non-zero extent in all 3 directions
  EXPECT_GT(aV1.Magnitude(), Precision::Confusion());
  EXPECT_GT(aV2.Magnitude(), Precision::Confusion());
  EXPECT_GT(aV3.Magnitude(), Precision::Confusion());
}

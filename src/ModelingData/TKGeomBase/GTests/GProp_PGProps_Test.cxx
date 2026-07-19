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

#include <gp_Pnt.hxx>
#include <GProp_PGProps.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(GProp_PGPropsTest, EmptySet)
{
  GProp_PGProps aProps;
  EXPECT_NEAR(aProps.Mass(), 0.0, Precision::Confusion());
}

TEST(GProp_PGPropsTest, SinglePoint)
{
  GProp_PGProps aProps;
  aProps.AddPoint(gp_Pnt(1.0, 2.0, 3.0));

  EXPECT_NEAR(aProps.Mass(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 3.0, Precision::Confusion());
}

TEST(GProp_PGPropsTest, TwoPoints_Barycentre)
{
  GProp_PGProps aProps;
  aProps.AddPoint(gp_Pnt(0.0, 0.0, 0.0));
  aProps.AddPoint(gp_Pnt(2.0, 4.0, 6.0));

  EXPECT_NEAR(aProps.Mass(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 3.0, Precision::Confusion());
}

TEST(GProp_PGPropsTest, WeightedPoints)
{
  GProp_PGProps aProps;
  aProps.AddPoint(gp_Pnt(0.0, 0.0, 0.0), 1.0);
  aProps.AddPoint(gp_Pnt(4.0, 0.0, 0.0), 3.0);

  EXPECT_NEAR(aProps.Mass(), 4.0, Precision::Confusion());
  // Weighted centroid: (0*1 + 4*3) / 4 = 3
  EXPECT_NEAR(aProps.CentreOfMass().X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 0.0, Precision::Confusion());
}

TEST(GProp_PGPropsTest, ArrayConstructor)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(-1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(0.0, -1.0, 0.0);

  GProp_PGProps aProps(aPnts);
  EXPECT_NEAR(aProps.Mass(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProps.CentreOfMass().Z(), 0.0, Precision::Confusion());
}

TEST(GProp_PGPropsTest, StaticBarycentre)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 3);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(3.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 6.0, 0.0);

  const gp_Pnt aG = GProp_PGProps::Barycentre(aPnts);
  EXPECT_NEAR(aG.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aG.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aG.Z(), 0.0, Precision::Confusion());
}

TEST(GProp_PGPropsTest, MatrixOfInertia_SymmetricPoints)
{
  // Centroid is at the origin, so origin-frame and central-frame inertia
  // matrices coincide for these symmetric points.
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(-1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(0.0, -1.0, 0.0);

  GProp_PGProps aProps(aPnts);
  const gp_Mat  anInertia = aProps.MatrixOfInertia();

  EXPECT_NEAR(anInertia.Value(1, 1), 2.0, Precision::Confusion()); // Ixx
  EXPECT_NEAR(anInertia.Value(2, 2), 2.0, Precision::Confusion()); // Iyy
  EXPECT_NEAR(anInertia.Value(3, 3), 4.0, Precision::Confusion()); // Izz
  EXPECT_NEAR(anInertia.Value(1, 2), 0.0, Precision::Confusion()); // Ixy
  EXPECT_NEAR(anInertia.Value(1, 3), 0.0, Precision::Confusion()); // Ixz
  EXPECT_NEAR(anInertia.Value(2, 3), 0.0, Precision::Confusion()); // Iyz
}

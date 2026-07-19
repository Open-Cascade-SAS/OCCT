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

#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GProp_PEquation.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(GProp_PEquationTest, CoincidentPoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 3);
  aPnts(1) = gp_Pnt(1.0, 2.0, 3.0);
  aPnts(2) = gp_Pnt(1.0, 2.0, 3.0);
  aPnts(3) = gp_Pnt(1.0, 2.0, 3.0);

  GProp_PEquation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsPoint());

  const gp_Pnt aP = anEq.Point();
  EXPECT_NEAR(aP.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aP.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aP.Z(), 3.0, Precision::Confusion());
}

TEST(GProp_PEquationTest, CollinearPoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 3);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(2.0, 0.0, 0.0);

  GProp_PEquation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsLinear());

  const gp_Lin aLin = anEq.Line();
  EXPECT_NEAR(std::abs(aLin.Direction().X()), 1.0, Precision::Confusion());
}

TEST(GProp_PEquationTest, CoplanarPoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(1.0, 1.0, 0.0);

  GProp_PEquation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsPlanar());

  const gp_Pln aPln = anEq.Plane();
  EXPECT_NEAR(std::abs(aPln.Axis().Direction().Z()), 1.0, Precision::Confusion());
}

TEST(GProp_PEquationTest, SpacePoints)
{
  NCollection_Array1<gp_Pnt> aPnts(1, 4);
  aPnts(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPnts(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPnts(3) = gp_Pnt(0.0, 1.0, 0.0);
  aPnts(4) = gp_Pnt(0.0, 0.0, 1.0);

  GProp_PEquation anEq(aPnts, 1e-6);
  EXPECT_TRUE(anEq.IsSpace());

  gp_Pnt aP;
  gp_Vec aV1, aV2, aV3;
  anEq.Box(aP, aV1, aV2, aV3);
  EXPECT_GT(aV1.Magnitude(), Precision::Confusion());
  EXPECT_GT(aV2.Magnitude(), Precision::Confusion());
  EXPECT_GT(aV3.Magnitude(), Precision::Confusion());
}

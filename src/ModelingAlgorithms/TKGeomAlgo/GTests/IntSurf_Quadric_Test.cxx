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

#include <IntSurf_Quadric.hxx>

#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

TEST(IntSurf_Quadric_Test, ConeApexGradientRemainsFinite)
{
  const gp_Cone      aCone(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
                      0.5,
                      0.0);
  const IntSurf_Quadric aQuadric(aCone);
  const gp_Pnt          anApex = aCone.Apex();

  EXPECT_NO_THROW(
    {
      const gp_Vec aGradient = aQuadric.Gradient(anApex);
      EXPECT_LE(aGradient.SquareMagnitude(), Precision::SquareConfusion());
    });

  double aDist = 1.0;
  gp_Vec aGrad(1.0, 0.0, 0.0);
  EXPECT_NO_THROW(aQuadric.ValAndGrad(anApex, aDist, aGrad));
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
  EXPECT_LE(aGrad.SquareMagnitude(), Precision::SquareConfusion());
}

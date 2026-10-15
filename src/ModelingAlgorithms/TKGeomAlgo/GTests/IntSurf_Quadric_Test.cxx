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

#include <cmath>

TEST(IntSurf_Quadric_Test, NearlyFlatConeSignedDistance)
{
  for (const double anAngle : {-1.57079432778897, 1.57079432778897})
  {
    const double          aRadius = 2.86244273186201;
    const IntSurf_Quadric aQuadric(gp_Cone(gp_Ax3(), anAngle, aRadius));
    for (const double aRadial : {10.0, 1.0e3, 1.0e9})
    {
      for (const double aSide : {-1.0, 1.0})
      {
        for (const double anOffset : {-1.0e-5, 0.0, 1.0e-5})
        {
          const double aHeight = (aSide * aRadial - aRadius) / std::tan(anAngle);
          const gp_Vec aNormal(std::cos(anAngle), 0.0, -aSide * std::sin(anAngle));
          const gp_Pnt aPoint = gp_Pnt(aRadial, 0.0, aHeight).Translated(aNormal * anOffset);
          SCOPED_TRACE(::testing::Message() << "angle=" << anAngle << " radius=" << aRadial
                                            << " side=" << aSide << " offset=" << anOffset);
          EXPECT_NEAR(aQuadric.Distance(aPoint), anOffset, 1.0e-10);
          double aDistance;
          gp_Vec aGradient;
          aQuadric.ValAndGrad(aPoint, aDistance, aGradient);
          EXPECT_NEAR(aDistance, anOffset, 1.0e-10);
          EXPECT_NEAR((aGradient - aNormal).Magnitude(), 0.0, Precision::Angular());
        }
      }
    }
  }
}

TEST(IntSurf_Quadric_Test, ConeSignedDistanceOnBothSheets)
{
  for (const double anAngle : {-0.5, 0.5})
  {
    for (const bool isDirect : {false, true})
    {
      gp_Ax3 anAxes(gp_Pnt(7.0, -3.0, 2.0), gp_Dir(1.0, 2.0, 3.0));
      if (!isDirect)
      {
        anAxes.YReverse();
      }
      const IntSurf_Quadric aQuadric(gp_Cone(anAxes, anAngle, 3.0));
      const gp_Vec aRadial = gp_Vec(anAxes.XDirection()) * 0.6 + gp_Vec(anAxes.YDirection()) * 0.8;
      for (const double aSide : {-1.0, 1.0})
      {
        const gp_Pnt aSurfacePoint = anAxes.Location().Translated(
          aRadial * 10.0 + gp_Vec(anAxes.Direction()) * ((aSide * 10.0 - 3.0) / std::tan(anAngle)));
        const gp_Vec aNormal =
          aRadial * std::cos(anAngle) - gp_Vec(anAxes.Direction()) * (aSide * std::sin(anAngle));
        for (const double anOffset : {-0.1, 0.0, 0.1})
        {
          const gp_Pnt aPoint = aSurfacePoint.Translated(aNormal * anOffset);
          EXPECT_NEAR(aQuadric.Distance(aPoint), anOffset, Precision::Confusion());
          EXPECT_NEAR((aQuadric.Gradient(aPoint) - aNormal).Magnitude(), 0.0, Precision::Angular());
          double aDistance;
          gp_Vec aGradient;
          aQuadric.ValAndGrad(aPoint, aDistance, aGradient);
          EXPECT_NEAR(aDistance, anOffset, Precision::Confusion());
          EXPECT_NEAR((aGradient - aNormal).Magnitude(), 0.0, Precision::Angular());
        }
      }
    }
  }
}

TEST(IntSurf_Quadric_Test, ConeApexGradientRemainsFinite)
{
  const gp_Cone aCone(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
                      0.5,
                      0.0);
  const IntSurf_Quadric aQuadric(aCone);
  const gp_Pnt          anApex = aCone.Apex();

  EXPECT_NO_THROW({
    const gp_Vec aGradient = aQuadric.Gradient(anApex);
    EXPECT_LE(aGradient.SquareMagnitude(), Precision::SquareConfusion());
  });

  double aDist = 1.0;
  gp_Vec aGrad(1.0, 0.0, 0.0);
  EXPECT_NO_THROW(aQuadric.ValAndGrad(anApex, aDist, aGrad));
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
  EXPECT_LE(aGrad.SquareMagnitude(), Precision::SquareConfusion());
}

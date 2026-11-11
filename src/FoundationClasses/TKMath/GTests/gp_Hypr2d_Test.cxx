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

#include <gp_Hypr2d.hxx>

#include <ElCLib.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
// Hyperbola with a rotated major axis and a left-handed local coordinate system.
gp_Hypr2d makeRotatedIndirectHyperbola()
{
  return gp_Hypr2d(gp_Ax2d(gp_Pnt2d(-2.0, 5.0), gp_Dir2d(3.0, 4.0)), 2.0, 0.5, false);
}
} // namespace

TEST(gp_Hypr2dTest, Asymptotes_AxisAlignedHyperbola_FollowDocumentedSlopes)
{
  const gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)), 3.0, 4.0);

  const gp_Ax2d anAsym1 = aHypr.Asymptote1();
  const gp_Ax2d anAsym2 = aHypr.Asymptote2();

  // Local equations are Y = (B/A)*X and Y = -(B/A)*X, so the unit directions are (A, +-B) / C.
  EXPECT_NEAR(anAsym1.Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(anAsym1.Location().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(anAsym1.Direction().X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(anAsym1.Direction().Y(), 0.8, Precision::Confusion());

  EXPECT_NEAR(anAsym2.Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(anAsym2.Location().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(anAsym2.Direction().X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(anAsym2.Direction().Y(), -0.8, Precision::Confusion());
}

TEST(gp_Hypr2dTest, Asymptotes_EqualRadii_ArePerpendicular)
{
  const gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0, 2.0);

  const gp_Ax2d anAsym1 = aHypr.Asymptote1();
  const gp_Ax2d anAsym2 = aHypr.Asymptote2();

  const double aCoord = 1.0 / std::sqrt(2.0);
  EXPECT_NEAR(anAsym1.Direction().X(), aCoord, Precision::Confusion());
  EXPECT_NEAR(anAsym1.Direction().Y(), aCoord, Precision::Confusion());
  EXPECT_NEAR(anAsym2.Direction().X(), aCoord, Precision::Confusion());
  EXPECT_NEAR(anAsym2.Direction().Y(), -aCoord, Precision::Confusion());
  EXPECT_NEAR(anAsym1.Direction().Dot(anAsym2.Direction()), 0.0, Precision::Confusion());
}

TEST(gp_Hypr2dTest, Asymptotes_RotatedIndirectHyperbola_ApproachBranch)
{
  const gp_Hypr2d aHypr = makeRotatedIndirectHyperbola();

  const gp_Lin2d anAsym1(aHypr.Asymptote1());
  const gp_Lin2d anAsym2(aHypr.Asymptote2());

  // The point (A*cosh(U), B*sinh(U)) is at distance A*B*exp(-|U|)/C from the nearest asymptote.
  // Point coordinates are about 2.2e4 here, so their rounding error is far below the tolerance.
  const double aParam    = 10.0;
  const double aFocalLen = std::sqrt(2.0 * 2.0 + 0.5 * 0.5);
  const double aDistance = 2.0 * 0.5 * std::exp(-aParam) / aFocalLen;
  const double aTol      = 1.0e-9;

  EXPECT_NEAR(anAsym1.Distance(ElCLib::Value(aParam, aHypr)), aDistance, aTol);
  EXPECT_NEAR(anAsym2.Distance(ElCLib::Value(-aParam, aHypr)), aDistance, aTol);
}

TEST(gp_Hypr2dTest, Directrices_AxisAlignedHyperbola_AreSymmetricAboutCenter)
{
  const gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)), 3.0, 4.0);

  const gp_Ax2d aDirectrix1 = aHypr.Directrix1();
  const gp_Ax2d aDirectrix2 = aHypr.Directrix2();

  // E = C/A = 5/3, so both directrices are at distance A/E = 1.8 from the center.
  EXPECT_NEAR(aDirectrix1.Location().X(), 2.8, Precision::Confusion());
  EXPECT_NEAR(aDirectrix1.Location().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aDirectrix2.Location().X(), -0.8, Precision::Confusion());
  EXPECT_NEAR(aDirectrix2.Location().Y(), 2.0, Precision::Confusion());

  EXPECT_NEAR(aDirectrix1.Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDirectrix1.Direction().Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDirectrix2.Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDirectrix2.Direction().Y(), 1.0, Precision::Confusion());
}

TEST(gp_Hypr2dTest, Directrices_RotatedIndirectHyperbola_SatisfyFocusDirectrixRatio)
{
  const gp_Hypr2d aHypr = makeRotatedIndirectHyperbola();

  const gp_Pnt2d aFocus1 = aHypr.Focus1();
  const gp_Pnt2d aFocus2 = aHypr.Focus2();
  const gp_Lin2d aDirectrix1(aHypr.Directrix1());
  const gp_Lin2d aDirectrix2(aHypr.Directrix2());
  const double   anEccentricity = aHypr.Eccentricity();

  // Each point of the hyperbola is E times farther from a focus than from the matching directrix.
  for (const double aParam : {-1.5, -0.5, 0.0, 0.75, 2.0})
  {
    const gp_Pnt2d aPoint = ElCLib::Value(aParam, aHypr);
    EXPECT_NEAR(aPoint.Distance(aFocus1),
                anEccentricity * aDirectrix1.Distance(aPoint),
                Precision::Confusion())
      << "Parameter " << aParam;
    EXPECT_NEAR(aPoint.Distance(aFocus2),
                anEccentricity * aDirectrix2.Distance(aPoint),
                Precision::Confusion())
      << "Parameter " << aParam;
  }
}

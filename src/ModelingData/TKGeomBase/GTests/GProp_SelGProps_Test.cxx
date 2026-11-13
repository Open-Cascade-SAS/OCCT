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

#include <GProp_SelGProps.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <math.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <functional>

namespace
{
//! Mass, first moment and second moment (integral of p p^T) of a patch about the origin.
struct PatchMoments
{
  double Mass = 0.0;
  gp_XYZ First;
  double Second[3][3] = {};
};

//! Gauss-Legendre integral over [theU1, theU2] x [theV1, theV2].
//! theSample returns the point at (u, v) and the area element there.
PatchMoments integratePatch(const std::function<gp_XYZ(double, double, double&)>& theSample,
                            const double                                          theU1,
                            const double                                          theU2,
                            const double                                          theV1,
                            const double                                          theV2)
{
  constexpr int aNbNodes = 16;
  math_Vector   aNodes(1, aNbNodes), aWeights(1, aNbNodes);
  math::OrderedGaussPointsAndWeights(aNbNodes, aNodes, aWeights);

  PatchMoments aRes;
  for (int i = 1; i <= aNbNodes; ++i)
  {
    for (int j = 1; j <= aNbNodes; ++j)
    {
      const double aU  = 0.5 * (theU2 - theU1) * aNodes(i) + 0.5 * (theU2 + theU1);
      const double aV  = 0.5 * (theV2 - theV1) * aNodes(j) + 0.5 * (theV2 + theV1);
      double       aDm = 0.0;
      const gp_XYZ aP  = theSample(aU, aV, aDm);
      const double aW  = 0.25 * (theU2 - theU1) * (theV2 - theV1) * aWeights(i) * aWeights(j) * aDm;
      aRes.Mass += aW;
      aRes.First += aW * aP;
      for (int a = 0; a < 3; ++a)
      {
        for (int b = 0; b < 3; ++b)
        {
          aRes.Second[a][b] += aW * aP.Coord(a + 1) * aP.Coord(b + 1);
        }
      }
    }
  }
  return aRes;
}

//! Matrix of inertia about the centre of mass from the moments about the origin.
gp_Mat inertiaAboutCentre(const PatchMoments& theMom, gp_XYZ& theCentre)
{
  theCentre = theMom.First / theMom.Mass;
  double aS[3][3];
  for (int a = 0; a < 3; ++a)
  {
    for (int b = 0; b < 3; ++b)
    {
      aS[a][b] =
        theMom.Second[a][b] - theMom.Mass * theCentre.Coord(a + 1) * theCentre.Coord(b + 1);
    }
  }
  const double aTrace = aS[0][0] + aS[1][1] + aS[2][2];
  gp_Mat       aRes;
  for (int a = 0; a < 3; ++a)
  {
    for (int b = 0; b < 3; ++b)
    {
      aRes.SetValue(a + 1, b + 1, (a == b ? aTrace : 0.0) - aS[a][b]);
    }
  }
  return aRes;
}

//! Moment of inertia about the axis through theQ along the unit vector theD.
double momentAboutAxis(const PatchMoments& theMom, const gp_XYZ& theQ, const gp_XYZ& theD)
{
  double aTrace = 0.0, aDSD = 0.0;
  for (int a = 0; a < 3; ++a)
  {
    aTrace += theMom.Second[a][a];
    for (int b = 0; b < 3; ++b)
    {
      aDSD += theD.Coord(a + 1) * theMom.Second[a][b] * theD.Coord(b + 1);
    }
  }
  const double aQd = theQ.Dot(theD);
  return (aTrace - 2.0 * theQ.Dot(theMom.First) + theMom.Mass * theQ.SquareModulus())
         - (aDSD - 2.0 * aQd * theD.Dot(theMom.First) + theMom.Mass * aQd * aQd);
}

//! Global point of the local coordinates (theX, theY, theZ) in the frame theAx.
gp_XYZ toGlobal(const gp_Ax3& theAx, const double theX, const double theY, const double theZ)
{
  return theAx.Location().XYZ() + theX * theAx.XDirection().XYZ() + theY * theAx.YDirection().XYZ()
         + theZ * theAx.Direction().XYZ();
}

struct ConeCase
{
  double SemiAngle;
  double RefRadius;
  double V1;
  double V2;
  double U1;
  double U2;
};

const double kTol = 1.0e-9;
} // namespace

TEST(GProp_SelGPropsTest, Cone_LateralArea_ClosedForm)
{
  // Area element of gp_Cone is R + v sin(a), no cos(a) factor.
  const double    aSemiAngle = M_PI / 6.0, aRadius = 5.0, aV1 = 0.0, aV2 = 10.0;
  GProp_SelGProps aProps;
  aProps.Perform(gp_Cone(gp_Ax3(gp::XOY()), aSemiAngle, aRadius), 0.0, 2.0 * M_PI, aV1, aV2);

  const double aExpected =
    2.0 * M_PI * (aV2 - aV1) * (aRadius + 0.5 * (aV1 + aV2) * std::sin(aSemiAngle));
  EXPECT_NEAR(aProps.Mass(), aExpected, kTol * aExpected);
}

TEST(GProp_SelGPropsTest, Cone_MatchesQuadrature)
{
  const double   aPi       = M_PI;
  const ConeCase aCases[]  = {{aPi / 6.0, 5.0, 0.0, 10.0, 0.0, 2.0 * aPi},
                              {aPi / 12.0, 8.0, 0.5, 6.0, 0.0, 2.0 * aPi},
                              {aPi / 3.0, 8.0, 0.5, 6.0, 0.0, 2.0 * aPi},
                              {-aPi / 6.0, 8.0, 0.5, 6.0, 0.0, 2.0 * aPi},
                              {aPi / 6.0, 5.0, 1.0, 9.0, 0.3, 2.2},
                              {-aPi / 4.0, 9.0, 0.5, 7.0, 0.3, 4.0}};
  const gp_Ax3   aFrames[] = {
    gp_Ax3(gp::XOY()),
    gp_Ax3(gp_Pnt(1.5, -2.0, 3.0), gp_Dir(1.0, 2.0, 3.0), gp_Dir(2.0, -1.0, 0.0))};
  const gp_Pnt aLoc(2.0, -1.0, 3.0);
  const gp_XYZ aAxisDir = gp_XYZ(1.0, 2.0, 2.0) / 3.0;

  for (const gp_Ax3& aFrame : aFrames)
  {
    for (const ConeCase& aCase : aCases)
    {
      const double       aSin = std::sin(aCase.SemiAngle), aCos = std::cos(aCase.SemiAngle);
      const gp_Cone      aCone(aFrame, aCase.SemiAngle, aCase.RefRadius);
      const PatchMoments aMom = integratePatch(
        [&](double theU, double theV, double& theDm) {
          const double aR = aCase.RefRadius + theV * aSin;
          theDm           = aR;
          return toGlobal(aFrame, aR * std::cos(theU), aR * std::sin(theU), theV * aCos);
        },
        aCase.U1,
        aCase.U2,
        aCase.V1,
        aCase.V2);

      GProp_SelGProps aProps;
      aProps.Perform(aCone, aCase.U1, aCase.U2, aCase.V1, aCase.V2);

      gp_XYZ       aCentre;
      const gp_Mat aInertia = inertiaAboutCentre(aMom, aCentre);
      double       aScale   = 0.0;
      for (int a = 1; a <= 3; ++a)
      {
        for (int b = 1; b <= 3; ++b)
        {
          aScale = std::max(aScale, std::abs(aInertia.Value(a, b)));
        }
      }

      EXPECT_NEAR(aProps.Mass(), aMom.Mass, kTol * aMom.Mass);
      EXPECT_NEAR(aProps.CentreOfMass().Distance(gp_Pnt(aCentre)),
                  0.0,
                  kTol * (1.0 + aCentre.Modulus()));
      const gp_Mat aMat = aProps.MatrixOfInertia();
      for (int a = 1; a <= 3; ++a)
      {
        for (int b = 1; b <= 3; ++b)
        {
          EXPECT_NEAR(aMat.Value(a, b), aInertia.Value(a, b), kTol * aScale);
        }
      }

      // The inertia about an axis through the location tests the Huyghens shift to it.
      GProp_SelGProps aLocProps(aCone, aCase.U1, aCase.U2, aCase.V1, aCase.V2, aLoc);
      const double    aExpectedAxis = momentAboutAxis(aMom, aLoc.XYZ(), aAxisDir);
      EXPECT_NEAR(aLocProps.MomentOfInertia(gp_Ax1(aLoc, gp_Dir(aAxisDir))),
                  aExpectedAxis,
                  kTol * std::abs(aExpectedAxis));
    }
  }
}

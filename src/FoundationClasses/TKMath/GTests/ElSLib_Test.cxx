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

#include <gtest/gtest.h>

#include <ElSLib.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <cmath>
#include <limits>

namespace
{
void expectVecNear(const gp_Vec& theActual, const gp_Vec& theExpected, const double theTolerance)
{
  EXPECT_NEAR(theActual.X(), theExpected.X(), theTolerance);
  EXPECT_NEAR(theActual.Y(), theExpected.Y(), theTolerance);
  EXPECT_NEAR(theActual.Z(), theExpected.Z(), theTolerance);
}
} // namespace

TEST(ElSLib_Test, TorusPreservesSmallNonzeroComponentsNearAxes)
{
  const gp_Ax3 aPosition(gp_Pnt(0.0, 0.0, 0.0),
                         gp_Dir(0.0, 0.0, 1.0),
                         gp_Dir(1.0, 0.0, 0.0));
  const double aMajorRadius = 5.0;
  const double aMinorRadius = 2.0;
  const double aHalfPi      = 0.5 * std::acos(-1.0);
  const double aULeft       = std::nextafter(aHalfPi, 0.0);
  const double aURight      = std::nextafter(aHalfPi, std::numeric_limits<double>::infinity());

  const gp_Pnt aLeft = ElSLib::TorusValue(aULeft, 0.0, aPosition, aMajorRadius, aMinorRadius);
  const gp_Pnt aRight = ElSLib::TorusValue(aURight, 0.0, aPosition, aMajorRadius, aMinorRadius);
  EXPECT_GT(aLeft.X(), 0.0);
  EXPECT_LT(aRight.X(), 0.0);

  gp_Pnt aD0;
  ElSLib::TorusD0(aURight, 0.0, aPosition, aMajorRadius, aMinorRadius, aD0);
  EXPECT_DOUBLE_EQ(aD0.X(), aRight.X());
  EXPECT_DOUBLE_EQ(aD0.Y(), aRight.Y());
  EXPECT_DOUBLE_EQ(aD0.Z(), aRight.Z());

  const double aRadius = aMajorRadius + aMinorRadius;
  const gp_Vec aDU = ElSLib::TorusDN(aURight, 0.0, aPosition, aMajorRadius, aMinorRadius, 1, 0);
  EXPECT_NE(aDU.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aDU.Y(), aRadius * std::cos(aURight));

  gp_Pnt aPoint;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  ElSLib::TorusD2(aURight,
                  0.0,
                  aPosition,
                  aMajorRadius,
                  aMinorRadius,
                  aPoint,
                  aD1U,
                  aD1V,
                  aD2U,
                  aD2V,
                  aD2UV);
  EXPECT_NE(aD2U.X(), 0.0);
  EXPECT_DOUBLE_EQ(aD2U.X(), -aRadius * std::cos(aURight));

  const double aSmallV = 1.0e-15;
  const gp_Pnt aSmallVPoint =
    ElSLib::TorusValue(0.0, aSmallV, aPosition, aMajorRadius, aMinorRadius);
  EXPECT_GT(aSmallVPoint.Z(), 0.0);
  EXPECT_DOUBLE_EQ(aSmallVPoint.Z(), aMinorRadius * std::sin(aSmallV));
}

//=================================================================================================

TEST(ElSLib_Test, TorusD3MatchesDN)
{
  const gp_Ax3 aPosition(gp_Pnt(1.0, -2.0, 3.0),
                         gp_Dir(0.0, 0.0, 1.0),
                         gp_Dir(1.0, 0.0, 0.0));
  const double aMajorRadius = 7.0;
  const double aMinorRadius = 1.25;
  const double aU           = 0.37;
  const double aV           = -0.41;

  gp_Pnt aPoint;
  gp_Vec aDU, aDV, aDUU, aDVV, aDUV, aDUUU, aDVVV, aDUUV, aDUVV;
  ElSLib::TorusD3(aU,
                  aV,
                  aPosition,
                  aMajorRadius,
                  aMinorRadius,
                  aPoint,
                  aDU,
                  aDV,
                  aDUU,
                  aDVV,
                  aDUV,
                  aDUUU,
                  aDVVV,
                  aDUUV,
                  aDUVV);

  const double aTolerance = 1.0e-14;
  expectVecNear(aDU,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 1, 0),
                aTolerance);
  expectVecNear(aDV,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 0, 1),
                aTolerance);
  expectVecNear(aDUU,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 2, 0),
                aTolerance);
  expectVecNear(aDVV,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 0, 2),
                aTolerance);
  expectVecNear(aDUV,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 1, 1),
                aTolerance);
  expectVecNear(aDUUU,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 3, 0),
                aTolerance);
  expectVecNear(aDVVV,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 0, 3),
                aTolerance);
  expectVecNear(aDUUV,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 2, 1),
                aTolerance);
  expectVecNear(aDUVV,
                ElSLib::TorusDN(aU, aV, aPosition, aMajorRadius, aMinorRadius, 1, 2),
                aTolerance);
}

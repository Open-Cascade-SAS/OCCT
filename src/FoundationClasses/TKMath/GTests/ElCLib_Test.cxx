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

#include <ElCLib.hxx>

#include <gtest/gtest.h>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>

namespace
{
// Helper function for comparing points with tolerance
void checkPointsEqual(const gp_Pnt&       theP1,
                      const gp_Pnt&       theP2,
                      const Standard_Real theTolerance = Precision::Confusion())
{
  EXPECT_NEAR(theP1.X(), theP2.X(), theTolerance);
  EXPECT_NEAR(theP1.Y(), theP2.Y(), theTolerance);
  EXPECT_NEAR(theP1.Z(), theP2.Z(), theTolerance);
}

// Helper function for comparing vectors with tolerance
void checkVectorsEqual(const gp_Vec&       theV1,
                       const gp_Vec&       theV2,
                       const Standard_Real theTolerance = Precision::Confusion())
{
  EXPECT_NEAR(theV1.X(), theV2.X(), theTolerance);
  EXPECT_NEAR(theV1.Y(), theV2.Y(), theTolerance);
  EXPECT_NEAR(theV1.Z(), theV2.Z(), theTolerance);
}

// Helper function for comparing directions with tolerance
void checkDirectorsEqual(const gp_Dir&       theD1,
                         const gp_Dir&       theD2,
                         const Standard_Real theTolerance = Precision::Confusion())
{
  EXPECT_NEAR(theD1.X(), theD2.X(), theTolerance);
  EXPECT_NEAR(theD1.Y(), theD2.Y(), theTolerance);
  EXPECT_NEAR(theD1.Z(), theD2.Z(), theTolerance);
}
} // namespace

TEST(ElClibTests, InPeriod)
{
  // Test with standard range [0, 2pi]
  const Standard_Real PI2 = 2.0 * M_PI;

  EXPECT_NEAR(ElCLib::InPeriod(0.5, 0.0, PI2), 0.5, Precision::Confusion());
  EXPECT_NEAR(ElCLib::InPeriod(PI2 + 0.5, 0.0, PI2), 0.5, Precision::Confusion());
  EXPECT_NEAR(ElCLib::InPeriod(-0.5, 0.0, PI2), PI2 - 0.5, Precision::Confusion());
  EXPECT_NEAR(ElCLib::InPeriod(-PI2 - 0.5, 0.0, PI2), PI2 - 0.5, Precision::Confusion());

  // Test with arbitrary range [1.5, 4.5]
  EXPECT_NEAR(ElCLib::InPeriod(1.7, 1.5, 4.5), 1.7, Precision::Confusion());
  EXPECT_NEAR(ElCLib::InPeriod(4.7, 1.5, 4.5), 1.7, Precision::Confusion());
  EXPECT_NEAR(ElCLib::InPeriod(7.7, 1.5, 4.5), 1.7, Precision::Confusion());
  EXPECT_NEAR(ElCLib::InPeriod(1.3, 1.5, 4.5), 4.3, Precision::Confusion());
}

TEST(ElClibTests, AdjustPeriodic)
{
  Standard_Real       U1, U2;
  const Standard_Real PI2 = 2.0 * M_PI;

  // Test with standard range [0, 2pi]
  // Case 1: U1 and U2 within range, no adjustment needed
  U1 = 0.5;
  U2 = 0.7;
  ElCLib::AdjustPeriodic(0.0, PI2, Precision::Confusion(), U1, U2);
  EXPECT_NEAR(U1, 0.5, Precision::Confusion());
  EXPECT_NEAR(U2, 0.7, Precision::Confusion());

  // Case 2: U1 within range, U2 outside range, should adjust U2 to be within period from U1
  U1 = 0.5;
  U2 = 0.5 + PI2 + 0.2;
  ElCLib::AdjustPeriodic(0.0, PI2, Precision::Confusion(), U1, U2);
  EXPECT_NEAR(U1, 0.5, Precision::Confusion());
  // U2 is adjusted to U1 + period
  EXPECT_NEAR(U2, 0.7, Precision::Confusion());

  // Case 3: Both U1 and U2 outside range but within same period
  U1 = 0.5 + PI2;
  U2 = 0.7 + PI2;
  ElCLib::AdjustPeriodic(0.0, PI2, Precision::Confusion(), U1, U2);
  EXPECT_NEAR(U1, 0.5, Precision::Confusion());
  EXPECT_NEAR(U2, 0.7, Precision::Confusion());

  // Test with negative U1
  U1 = -0.5;
  U2 = 0.7;
  ElCLib::AdjustPeriodic(0.0, PI2, Precision::Confusion(), U1, U2);
  EXPECT_NEAR(U1, PI2 - 0.5, Precision::Confusion());
  EXPECT_NEAR(U2, 0.7 + PI2, Precision::Confusion());

  // Test where U2 is very close to U1 (should add a period to U2)
  U1 = 1.0;
  U2 = 1.0 + 0.5 * Precision::Confusion(); // Very close to U1
  ElCLib::AdjustPeriodic(0.0, PI2, Precision::Confusion(), U1, U2);
  EXPECT_NEAR(U1, 1.0, Precision::Confusion());
  EXPECT_NEAR(U2, 1.0 + PI2, Precision::Confusion());
}

TEST(ElClibTests, Line3D)
{
  const gp_Pnt        aLoc(1.0, 2.0, 3.0);
  const gp_Dir        aDir(gp_Dir::D::Z);
  const gp_Lin        aLin(aLoc, aDir);
  const Standard_Real aParam = 5.0;

  // Test Value
  const gp_Pnt aPoint = ElCLib::Value(aParam, aLin);
  const gp_Pnt aExpectedPoint(1.0, 2.0, 8.0);
  checkPointsEqual(aPoint, aExpectedPoint);

  // Test D1
  gp_Pnt aPointD1;
  gp_Vec aVecD1;
  ElCLib::D1(aParam, aLin, aPointD1, aVecD1);
  checkPointsEqual(aPointD1, aExpectedPoint);
  checkVectorsEqual(aVecD1, gp_Vec(aDir));

  // Test DN
  const gp_Vec aVecDN = ElCLib::DN(aParam, aLin, 1);
  checkVectorsEqual(aVecDN, gp_Vec(aDir));
  const gp_Vec aVecDN2 = ElCLib::DN(aParam, aLin, 2);
  checkVectorsEqual(aVecDN2, gp_Vec(0.0, 0.0, 0.0));

  // Test Parameter
  const gp_Pnt        aTestPoint(1.0, 2.0, 10.0);
  const Standard_Real aCalculatedParam = ElCLib::Parameter(aLin, aTestPoint);
  EXPECT_NEAR(aCalculatedParam, 7.0, Precision::Confusion());
}

TEST(ElClibTests, Circle3D)
{
  const gp_Pnt        aLoc(0.0, 0.0, 0.0);
  const gp_Dir        aDirZ(gp_Dir::D::Z);
  const gp_Dir        aDirX(gp_Dir::D::X);
  const gp_Ax2        anAxis(aLoc, aDirZ, aDirX);
  const Standard_Real aRadius = 2.0;
  const gp_Circ       aCircle(anAxis, aRadius);
  const Standard_Real aParam = M_PI / 4.0; // 45 degrees

  // Test Value
  const gp_Pnt aPoint = ElCLib::Value(aParam, aCircle);
  const gp_Pnt aExpectedPoint(aRadius * cos(aParam), aRadius * sin(aParam), 0.0);
  checkPointsEqual(aPoint, aExpectedPoint);

  // Test D1
  gp_Pnt aPointD1;
  gp_Vec aVecD1;
  ElCLib::D1(aParam, aCircle, aPointD1, aVecD1);
  checkPointsEqual(aPointD1, aExpectedPoint);
  const gp_Vec aExpectedVecD1(-aRadius * sin(aParam), aRadius * cos(aParam), 0.0);
  checkVectorsEqual(aVecD1, aExpectedVecD1);

  // Test D2
  gp_Pnt aPointD2;
  gp_Vec aVecD2_1, aVecD2_2;
  ElCLib::D2(aParam, aCircle, aPointD2, aVecD2_1, aVecD2_2);
  checkPointsEqual(aPointD2, aExpectedPoint);
  checkVectorsEqual(aVecD2_1, aExpectedVecD1);
  const gp_Vec aExpectedVecD2_2(-aRadius * cos(aParam), -aRadius * sin(aParam), 0.0);
  checkVectorsEqual(aVecD2_2, aExpectedVecD2_2);

  // Test D3
  gp_Pnt aPointD3;
  gp_Vec aVecD3_1, aVecD3_2, aVecD3_3;
  ElCLib::D3(aParam, aCircle, aPointD3, aVecD3_1, aVecD3_2, aVecD3_3);
  checkPointsEqual(aPointD3, aExpectedPoint);
  checkVectorsEqual(aVecD3_1, aExpectedVecD1);
  checkVectorsEqual(aVecD3_2, aExpectedVecD2_2);
  const gp_Vec aExpectedVecD3_3(aRadius * sin(aParam), -aRadius * cos(aParam), 0.0);
  checkVectorsEqual(aVecD3_3, aExpectedVecD3_3);

  // Test DN
  const gp_Vec aVecDN1 = ElCLib::DN(aParam, aCircle, 1);
  checkVectorsEqual(aVecDN1, aExpectedVecD1);

  const gp_Vec aVecDN2 = ElCLib::DN(aParam, aCircle, 2);
  checkVectorsEqual(aVecDN2, aExpectedVecD2_2);

  const gp_Vec aVecDN3 = ElCLib::DN(aParam, aCircle, 3);
  checkVectorsEqual(aVecDN3, aExpectedVecD3_3);

  // Test Parameter
  const Standard_Real aCalculatedParam = ElCLib::Parameter(aCircle, aExpectedPoint);
  EXPECT_NEAR(aCalculatedParam, aParam, Precision::Confusion());
}

TEST(ElClibTests, Ellipse3D)
{
  const gp_Pnt        aLoc(0.0, 0.0, 0.0);
  const gp_Dir        aDirZ(gp_Dir::D::Z);
  const gp_Dir        aDirX(gp_Dir::D::X);
  const gp_Ax2        anAxis(aLoc, aDirZ, aDirX);
  const Standard_Real aMajorRadius = 3.0;
  const Standard_Real aMinorRadius = 2.0;
  const gp_Elips      anEllipse(anAxis, aMajorRadius, aMinorRadius);
  const Standard_Real aParam = M_PI / 4.0; // 45 degrees

  // Test Value
  const gp_Pnt aPoint = ElCLib::Value(aParam, anEllipse);
  const gp_Pnt aExpectedPoint(aMajorRadius * cos(aParam), aMinorRadius * sin(aParam), 0.0);
  checkPointsEqual(aPoint, aExpectedPoint);

  // Test D1
  gp_Pnt aPointD1;
  gp_Vec aVecD1;
  ElCLib::D1(aParam, anEllipse, aPointD1, aVecD1);
  checkPointsEqual(aPointD1, aExpectedPoint);
  const gp_Vec aExpectedVecD1(-aMajorRadius * sin(aParam), aMinorRadius * cos(aParam), 0.0);
  checkVectorsEqual(aVecD1, aExpectedVecD1);

  // Test Parameter
  const Standard_Real aCalculatedParam = ElCLib::Parameter(anEllipse, aExpectedPoint);
  EXPECT_NEAR(aCalculatedParam, aParam, Precision::Confusion());
}

TEST(ElClibTests, Hyperbola3D)
{
  const gp_Pnt        aLoc(0.0, 0.0, 0.0);
  const gp_Dir        aDirZ(gp_Dir::D::Z);
  const gp_Dir        aDirX(gp_Dir::D::X);
  const gp_Ax2        anAxis(aLoc, aDirZ, aDirX);
  const Standard_Real aMajorRadius = 3.0;
  const Standard_Real aMinorRadius = 2.0;
  const gp_Hypr       aHyperbola(anAxis, aMajorRadius, aMinorRadius);
  const Standard_Real aParam = 0.5;

  // Test Value
  const gp_Pnt aPoint = ElCLib::Value(aParam, aHyperbola);
  const gp_Pnt aExpectedPoint(aMajorRadius * cosh(aParam), aMinorRadius * sinh(aParam), 0.0);
  checkPointsEqual(aPoint, aExpectedPoint);

  // Test D1
  gp_Pnt aPointD1;
  gp_Vec aVecD1;
  ElCLib::D1(aParam, aHyperbola, aPointD1, aVecD1);
  checkPointsEqual(aPointD1, aExpectedPoint);
  const gp_Vec aExpectedVecD1(aMajorRadius * sinh(aParam), aMinorRadius * cosh(aParam), 0.0);
  checkVectorsEqual(aVecD1, aExpectedVecD1);

  // Test Parameter
  const Standard_Real aCalculatedParam = ElCLib::Parameter(aHyperbola, aExpectedPoint);
  EXPECT_NEAR(aCalculatedParam, aParam, Precision::Confusion());
}

TEST(ElClibTests, Parabola3D)
{
  const gp_Pnt        aLoc(0.0, 0.0, 0.0);
  const gp_Dir        aDirZ(gp_Dir::D::Z);
  const gp_Dir        aDirX(gp_Dir::D::X);
  const gp_Ax2        anAxis(aLoc, aDirZ, aDirX);
  const Standard_Real aFocal = 2.0;
  const gp_Parab      aParabola(anAxis, aFocal);
  const Standard_Real aParam = 3.0;

  // Test Value
  const gp_Pnt aPoint = ElCLib::Value(aParam, aParabola);
  const gp_Pnt aExpectedPoint(aParam * aParam / (4.0 * aFocal), aParam, 0.0);
  checkPointsEqual(aPoint, aExpectedPoint);

  // Test D1
  gp_Pnt aPointD1;
  gp_Vec aVecD1;
  ElCLib::D1(aParam, aParabola, aPointD1, aVecD1);
  checkPointsEqual(aPointD1, aExpectedPoint);
  const gp_Vec aExpectedVecD1(aParam / (2.0 * aFocal), 1.0, 0.0);
  checkVectorsEqual(aVecD1, aExpectedVecD1);

  // Test Parameter
  const Standard_Real aCalculatedParam = ElCLib::Parameter(aParabola, aExpectedPoint);
  EXPECT_NEAR(aCalculatedParam, aParam, Precision::Confusion());
}

TEST(ElClibTests, To3dConversion)
{
  const gp_Pnt aLoc(1.0, 2.0, 3.0);
  const gp_Dir aDirZ(gp_Dir::D::Z);
  const gp_Dir aDirX(gp_Dir::D::X);
  const gp_Ax2 anAxis(aLoc, aDirZ, aDirX);

  // Test conversion of a point
  const gp_Pnt2d aPnt2d(2.0, 3.0);
  const gp_Pnt   aPnt3d = ElCLib::To3d(anAxis, aPnt2d);
  const gp_Pnt   aExpectedPnt3d(3.0, 5.0, 3.0);
  checkPointsEqual(aPnt3d, aExpectedPnt3d);

  // Test conversion of a vector
  const gp_Vec2d aVec2d(1.0, 2.0);
  const gp_Vec   aVec3d = ElCLib::To3d(anAxis, aVec2d);
  const gp_Vec   aExpectedVec3d(1.0, 2.0, 0.0);
  checkVectorsEqual(aVec3d, aExpectedVec3d);

  // Test conversion of a direction
  const gp_Dir2d aDir2d(1.0, 2.0);
  const gp_Dir   aDir3d = ElCLib::To3d(anAxis, aDir2d);
  const gp_Dir   aExpectedDir3d(1.0 / sqrt(5.0), 2.0 / sqrt(5.0), 0.0);
  checkDirectorsEqual(aDir3d, aExpectedDir3d, Precision::Confusion());

  // Test conversion of a circle
  const gp_Pnt2d      aLoc2d(0.0, 0.0);
  const gp_Dir2d      aDir2dX(gp_Dir2d::D::X);
  const gp_Ax22d      anAxis2d(aLoc2d, aDir2dX, true);
  const Standard_Real aRadius = 2.0;
  const gp_Circ2d     aCirc2d(anAxis2d, aRadius);
  const gp_Circ       aCirc3d = ElCLib::To3d(anAxis, aCirc2d);
  EXPECT_NEAR(aCirc3d.Radius(), aRadius, Precision::Confusion());
  checkPointsEqual(aCirc3d.Location(), aLoc, Precision::Confusion());
}

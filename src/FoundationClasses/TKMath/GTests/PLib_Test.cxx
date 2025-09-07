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

#include <PLib.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>

namespace
{
// Helper function for comparing points with tolerance
void checkPointsEqual(const gp_Pnt&       thePnt1,
                      const gp_Pnt&       thePnt2,
                      const Standard_Real theTolerance = Precision::Confusion())
{
  EXPECT_NEAR(thePnt1.X(), thePnt2.X(), theTolerance) << "X coordinates differ";
  EXPECT_NEAR(thePnt1.Y(), thePnt2.Y(), theTolerance) << "Y coordinates differ";
  EXPECT_NEAR(thePnt1.Z(), thePnt2.Z(), theTolerance) << "Z coordinates differ";
}

// Helper function for comparing 2D points with tolerance
void checkPoint2dEqual(const gp_Pnt2d&     thePnt1,
                       const gp_Pnt2d&     thePnt2,
                       const Standard_Real theTolerance = Precision::Confusion())
{
  EXPECT_NEAR(thePnt1.X(), thePnt2.X(), theTolerance) << "X coordinates differ";
  EXPECT_NEAR(thePnt1.Y(), thePnt2.Y(), theTolerance) << "Y coordinates differ";
}
} // namespace

// Test class for PLib tests
class PLibTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Setup common test data
  }

  void TearDown() override
  {
    // Cleanup
  }
};

// Tests for basic utility functions
TEST_F(PLibTest, NoWeightsPointers)
{
  // Test that NoWeights() returns NULL as expected
  EXPECT_EQ(PLib::NoWeights(), static_cast<TColStd_Array1OfReal*>(nullptr));
  EXPECT_EQ(PLib::NoWeights2(), static_cast<TColStd_Array2OfReal*>(nullptr));
}

// Tests for 3D pole conversion functions
TEST_F(PLibTest, SetGetPoles3D)
{
  // Create test data
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(1.0, 2.0, 3.0);
  aPoles(2) = gp_Pnt(4.0, 5.0, 6.0);
  aPoles(3) = gp_Pnt(7.0, 8.0, 9.0);

  // Test SetPoles and GetPoles without weights
  TColStd_Array1OfReal aFP(1, 9); // 3 poles * 3 coordinates
  PLib::SetPoles(aPoles, aFP);

  TColgp_Array1OfPnt aResultPoles(1, 3);
  PLib::GetPoles(aFP, aResultPoles);

  // Verify results
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    checkPointsEqual(aPoles(i), aResultPoles(i));
  }
}

TEST_F(PLibTest, SetGetPoles3DWithWeights)
{
  // Create test data
  TColgp_Array1OfPnt aPoles(1, 2);
  aPoles(1) = gp_Pnt(1.0, 2.0, 3.0);
  aPoles(2) = gp_Pnt(4.0, 5.0, 6.0);

  TColStd_Array1OfReal aWeights(1, 2);
  aWeights(1) = 0.5;
  aWeights(2) = 2.0;

  // Test SetPoles and GetPoles with weights
  TColStd_Array1OfReal aFP(1, 8); // 2 poles * (3 coords + 1 weight)
  PLib::SetPoles(aPoles, aWeights, aFP);

  TColgp_Array1OfPnt   aResultPoles(1, 2);
  TColStd_Array1OfReal aResultWeights(1, 2);
  PLib::GetPoles(aFP, aResultPoles, aResultWeights);

  // Verify results
  for (Standard_Integer i = 1; i <= 2; i++)
  {
    checkPointsEqual(aPoles(i), aResultPoles(i));
    EXPECT_NEAR(aWeights(i), aResultWeights(i), Precision::Confusion());
  }
}

// Tests for 2D pole conversion functions
TEST_F(PLibTest, SetGetPoles2D)
{
  // Create test data
  TColgp_Array1OfPnt2d aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1.0, 2.0);
  aPoles(2) = gp_Pnt2d(3.0, 4.0);
  aPoles(3) = gp_Pnt2d(5.0, 6.0);

  // Test SetPoles and GetPoles without weights
  TColStd_Array1OfReal aFP(1, 6); // 3 poles * 2 coordinates
  PLib::SetPoles(aPoles, aFP);

  TColgp_Array1OfPnt2d aResultPoles(1, 3);
  PLib::GetPoles(aFP, aResultPoles);

  // Verify results
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    checkPoint2dEqual(aPoles(i), aResultPoles(i));
  }
}

TEST_F(PLibTest, SetGetPoles2DWithWeights)
{
  // Create test data
  TColgp_Array1OfPnt2d aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(1.0, 2.0);
  aPoles(2) = gp_Pnt2d(3.0, 4.0);

  TColStd_Array1OfReal aWeights(1, 2);
  aWeights(1) = 0.8;
  aWeights(2) = 1.5;

  // Test SetPoles and GetPoles with weights
  TColStd_Array1OfReal aFP(1, 6); // 2 poles * (2 coords + 1 weight)
  PLib::SetPoles(aPoles, aWeights, aFP);

  TColgp_Array1OfPnt2d aResultPoles(1, 2);
  TColStd_Array1OfReal aResultWeights(1, 2);
  PLib::GetPoles(aFP, aResultPoles, aResultWeights);

  // Verify results
  for (Standard_Integer i = 1; i <= 2; i++)
  {
    checkPoint2dEqual(aPoles(i), aResultPoles(i));
    EXPECT_NEAR(aWeights(i), aResultWeights(i), Precision::Confusion());
  }
}

// Test for zero weights handling (safety test)
TEST_F(PLibTest, ZeroWeightsHandling)
{
  // Create test data with zero weight - this should not crash
  TColgp_Array1OfPnt2d aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(1.0, 2.0);
  aPoles(2) = gp_Pnt2d(3.0, 4.0);

  TColStd_Array1OfReal aWeights(1, 2);
  aWeights(1) = 1.0;
  aWeights(2) = 0.0; // Zero weight - potential division by zero

  TColStd_Array1OfReal aFP(1, 6);
  PLib::SetPoles(aPoles, aWeights, aFP);

  // This test should complete without crashing
  // The behavior with zero weights may vary, but it shouldn't crash
  EXPECT_TRUE(true); // We mainly test that no crash occurs
}

// Tests for Binomial coefficient function
TEST_F(PLibTest, BinomialCoefficient)
{
  // Test known binomial coefficients
  EXPECT_NEAR(PLib::Bin(0, 0), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 0), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 5), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 1), 5.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 2), 10.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 3), 10.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(10, 3), 120.0, Precision::Confusion());

  // Test symmetry property: C(n,k) = C(n,n-k)
  for (Standard_Integer n = 1; n <= 10; n++)
  {
    for (Standard_Integer k = 0; k <= n; k++)
    {
      EXPECT_NEAR(PLib::Bin(n, k), PLib::Bin(n, n - k), Precision::Confusion())
        << "Binomial coefficient symmetry failed for C(" << n << "," << k << ")";
    }
  }
}

// Tests for polynomial evaluation
TEST_F(PLibTest, EvalPolynomial)
{
  // Test simple polynomial evaluation: f(x) = 1 + 2x + 3x^2
  const Standard_Integer aDegree     = 2;
  const Standard_Integer aDimension  = 1;
  const Standard_Integer aDerivOrder = 0;

  TColStd_Array1OfReal aCoeffs(1, 3);
  aCoeffs(1) = 1.0; // constant term
  aCoeffs(2) = 2.0; // linear term
  aCoeffs(3) = 3.0; // quadratic term

  TColStd_Array1OfReal aResults(1, 1);

  // Test at x = 0: f(0) = 1
  PLib::EvalPolynomial(0.0,
                       aDerivOrder,
                       aDegree,
                       aDimension,
                       aCoeffs.ChangeValue(1),
                       aResults.ChangeValue(1));
  EXPECT_NEAR(aResults(1), 1.0, Precision::Confusion());

  // Test at x = 1: f(1) = 1 + 2 + 3 = 6
  PLib::EvalPolynomial(1.0,
                       aDerivOrder,
                       aDegree,
                       aDimension,
                       aCoeffs.ChangeValue(1),
                       aResults.ChangeValue(1));
  EXPECT_NEAR(aResults(1), 6.0, Precision::Confusion());

  // Test at x = 2: f(2) = 1 + 4 + 12 = 17
  PLib::EvalPolynomial(2.0,
                       aDerivOrder,
                       aDegree,
                       aDimension,
                       aCoeffs.ChangeValue(1),
                       aResults.ChangeValue(1));
  EXPECT_NEAR(aResults(1), 17.0, Precision::Confusion());
}

// Tests for polynomial evaluation with derivatives
TEST_F(PLibTest, EvalPolynomialWithDerivatives)
{
  // Test polynomial f(x) = 1 + 2x + 3x^2
  // f'(x) = 2 + 6x
  // f''(x) = 6
  const Standard_Integer aDegree     = 2;
  const Standard_Integer aDimension  = 1;
  const Standard_Integer aDerivOrder = 2;

  TColStd_Array1OfReal aCoeffs(1, 3);
  aCoeffs(1) = 1.0;
  aCoeffs(2) = 2.0;
  aCoeffs(3) = 3.0;

  TColStd_Array1OfReal aResults(1, 3); // value + 1st + 2nd derivative

  // Test at x = 1
  PLib::EvalPolynomial(1.0,
                       aDerivOrder,
                       aDegree,
                       aDimension,
                       aCoeffs.ChangeValue(1),
                       aResults.ChangeValue(1));

  EXPECT_NEAR(aResults(1), 6.0, Precision::Confusion()); // f(1) = 6
  EXPECT_NEAR(aResults(2), 8.0, Precision::Confusion()); // f'(1) = 2 + 6 = 8
  EXPECT_NEAR(aResults(3), 6.0, Precision::Confusion()); // f''(1) = 6
}

// Tests for constraint order conversion functions
TEST_F(PLibTest, ConstraintOrderConversion)
{
  // Test NivConstr function
  EXPECT_EQ(PLib::NivConstr(GeomAbs_C0), 0);
  EXPECT_EQ(PLib::NivConstr(GeomAbs_C1), 1);
  EXPECT_EQ(PLib::NivConstr(GeomAbs_C2), 2);

  // Test ConstraintOrder function
  EXPECT_EQ(PLib::ConstraintOrder(0), GeomAbs_C0);
  EXPECT_EQ(PLib::ConstraintOrder(1), GeomAbs_C1);
  EXPECT_EQ(PLib::ConstraintOrder(2), GeomAbs_C2);

  // Test round-trip consistency
  for (Standard_Integer i = 0; i <= 2; i++)
  {
    GeomAbs_Shape    aShape = PLib::ConstraintOrder(i);
    Standard_Integer aLevel = PLib::NivConstr(aShape);
    EXPECT_EQ(aLevel, i) << "Round-trip conversion failed for level " << i;
  }
}

// Test for Hermite interpolation
TEST_F(PLibTest, HermiteInterpolate)
{

  const Standard_Integer aDimension  = 1;
  const Standard_Real    aFirstParam = 0.0;
  const Standard_Real    aLastParam  = 1.0;
  const Standard_Integer aFirstOrder = 1; // value + 1st derivative
  const Standard_Integer aLastOrder  = 1; // value + 1st derivative

  // Define constraints: f(0) = 0, f'(0) = 1, f(1) = 1, f'(1) = 0
  TColStd_Array2OfReal aFirstConstr(1, aDimension, 0, aFirstOrder);
  aFirstConstr(1, 0) = 0.0; // f(0) = 0
  aFirstConstr(1, 1) = 1.0; // f'(0) = 1

  TColStd_Array2OfReal aLastConstr(1, aDimension, 0, aLastOrder);
  aLastConstr(1, 0) = 1.0; // f(1) = 1
  aLastConstr(1, 1) = 0.0; // f'(1) = 0

  const Standard_Integer aCoeffCount = aFirstOrder + aLastOrder + 2;

  TColStd_Array1OfReal aCoeffs(0,
                               aCoeffCount
                                 - 1); // 0-based indexing as expected by HermiteInterpolate

  // Perform Hermite interpolation
  Standard_Boolean aResult = PLib::HermiteInterpolate(aDimension,
                                                      aFirstParam,
                                                      aLastParam,
                                                      aFirstOrder,
                                                      aLastOrder,
                                                      aFirstConstr,
                                                      aLastConstr,
                                                      aCoeffs);

  EXPECT_TRUE(aResult) << "Hermite interpolation failed";

  if (aResult)
  {

    // Verify that the resulting polynomial satisfies the constraints
    TColStd_Array1OfReal aResults(1, 2); // value + 1st derivative

    // Check at first parameter
    PLib::EvalPolynomial(aFirstParam,
                         1,
                         aCoeffCount - 1,
                         aDimension,
                         aCoeffs.ChangeValue(0),
                         aResults.ChangeValue(1));
    EXPECT_NEAR(aResults(1), 0.0, Precision::Confusion()) << "f(0) constraint not satisfied";
    EXPECT_NEAR(aResults(2), 1.0, Precision::Confusion()) << "f'(0) constraint not satisfied";

    // Check at last parameter
    PLib::EvalPolynomial(aLastParam,
                         1,
                         aCoeffCount - 1,
                         aDimension,
                         aCoeffs.ChangeValue(0),
                         aResults.ChangeValue(1));
    EXPECT_NEAR(aResults(1), 1.0, Precision::Confusion()) << "f(1) constraint not satisfied";
    EXPECT_NEAR(aResults(2), 0.0, Precision::Confusion()) << "f'(1) constraint not satisfied";
  }
}

// Edge case tests
TEST_F(PLibTest, EdgeCases)
{
  // Test with very small coefficients
  TColStd_Array1OfReal aSmallCoeffs(1, 3);
  aSmallCoeffs(1) = 1.0e-12;
  aSmallCoeffs(2) = 1.0e-12;
  aSmallCoeffs(3) = 1.0e-12;

  TColStd_Array1OfReal aResults(1, 1);

  // This should not crash even with very small coefficients
  EXPECT_NO_THROW(
    { PLib::EvalPolynomial(1.0, 0, 2, 1, aSmallCoeffs.ChangeValue(1), aResults.ChangeValue(1)); });

  // Test with large coefficients
  TColStd_Array1OfReal aLargeCoeffs(1, 3);
  aLargeCoeffs(1) = 1.0e10;
  aLargeCoeffs(2) = 1.0e10;
  aLargeCoeffs(3) = 1.0e10;

  EXPECT_NO_THROW(
    { PLib::EvalPolynomial(1.0, 0, 2, 1, aLargeCoeffs.ChangeValue(1), aResults.ChangeValue(1)); });
}

// Test for Jacobi parameter calculation
TEST_F(PLibTest, JacobiParameters)
{
  Standard_Integer aNbGaussPoints, aWorkDegree;

  // Test various constraint orders and codes
  PLib::JacobiParameters(GeomAbs_C0, 10, 1, aNbGaussPoints, aWorkDegree);
  EXPECT_GT(aNbGaussPoints, 0) << "Number of Gauss points should be positive";
  EXPECT_GT(aWorkDegree, 0) << "Work degree should be positive";

  PLib::JacobiParameters(GeomAbs_C1, 15, 2, aNbGaussPoints, aWorkDegree);
  EXPECT_GT(aNbGaussPoints, 0);
  EXPECT_GT(aWorkDegree, 0);

  PLib::JacobiParameters(GeomAbs_C2, 20, 3, aNbGaussPoints, aWorkDegree);
  EXPECT_GT(aNbGaussPoints, 0);
  EXPECT_GT(aWorkDegree, 0);
}
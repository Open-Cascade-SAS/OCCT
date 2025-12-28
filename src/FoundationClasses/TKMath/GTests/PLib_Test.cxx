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
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>

namespace
{
// Helper function for comparing points with tolerance
void checkPointsEqual(const gp_Pnt& thePnt1,
                      const gp_Pnt& thePnt2,
                      const double  theTolerance = Precision::Confusion())
{
  EXPECT_NEAR(thePnt1.X(), thePnt2.X(), theTolerance) << "X coordinates differ";
  EXPECT_NEAR(thePnt1.Y(), thePnt2.Y(), theTolerance) << "Y coordinates differ";
  EXPECT_NEAR(thePnt1.Z(), thePnt2.Z(), theTolerance) << "Z coordinates differ";
}

// Helper function for comparing 2D points with tolerance
void checkPoint2dEqual(const gp_Pnt2d& thePnt1,
                       const gp_Pnt2d& thePnt2,
                       const double    theTolerance = Precision::Confusion())
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
  EXPECT_EQ(PLib::NoWeights(), static_cast<NCollection_Array1<double>*>(nullptr));
  EXPECT_EQ(PLib::NoWeights2(), static_cast<NCollection_Array2<double>*>(nullptr));
}

// Tests for 3D pole conversion functions
TEST_F(PLibTest, SetGetPoles3D)
{
  // Create test data
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(1.0, 2.0, 3.0);
  aPoles(2) = gp_Pnt(4.0, 5.0, 6.0);
  aPoles(3) = gp_Pnt(7.0, 8.0, 9.0);

  // Test SetPoles and GetPoles without weights
  NCollection_Array1<double> aFP(1, 9); // 3 poles * 3 coordinates
  PLib::SetPoles(aPoles, aFP);

  NCollection_Array1<gp_Pnt> aResultPoles(1, 3);
  PLib::GetPoles(aFP, aResultPoles);

  // Verify results
  for (int i = 1; i <= 3; i++)
  {
    checkPointsEqual(aPoles(i), aResultPoles(i));
  }
}

TEST_F(PLibTest, SetGetPoles3DWithWeights)
{
  // Create test data
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = gp_Pnt(1.0, 2.0, 3.0);
  aPoles(2) = gp_Pnt(4.0, 5.0, 6.0);

  NCollection_Array1<double> aWeights(1, 2);
  aWeights(1) = 0.5;
  aWeights(2) = 2.0;

  // Test SetPoles and GetPoles with weights
  NCollection_Array1<double> aFP(1, 8); // 2 poles * (3 coords + 1 weight)
  PLib::SetPoles(aPoles, aWeights, aFP);

  NCollection_Array1<gp_Pnt> aResultPoles(1, 2);
  NCollection_Array1<double> aResultWeights(1, 2);
  PLib::GetPoles(aFP, aResultPoles, aResultWeights);

  // Verify results
  for (int i = 1; i <= 2; i++)
  {
    checkPointsEqual(aPoles(i), aResultPoles(i));
    EXPECT_NEAR(aWeights(i), aResultWeights(i), Precision::Confusion());
  }
}

// Tests for 2D pole conversion functions
TEST_F(PLibTest, SetGetPoles2D)
{
  // Create test data
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1.0, 2.0);
  aPoles(2) = gp_Pnt2d(3.0, 4.0);
  aPoles(3) = gp_Pnt2d(5.0, 6.0);

  // Test SetPoles and GetPoles without weights
  NCollection_Array1<double> aFP(1, 6); // 3 poles * 2 coordinates
  PLib::SetPoles(aPoles, aFP);

  NCollection_Array1<gp_Pnt2d> aResultPoles(1, 3);
  PLib::GetPoles(aFP, aResultPoles);

  // Verify results
  for (int i = 1; i <= 3; i++)
  {
    checkPoint2dEqual(aPoles(i), aResultPoles(i));
  }
}

TEST_F(PLibTest, SetGetPoles2DWithWeights)
{
  // Create test data
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(1.0, 2.0);
  aPoles(2) = gp_Pnt2d(3.0, 4.0);

  NCollection_Array1<double> aWeights(1, 2);
  aWeights(1) = 0.8;
  aWeights(2) = 1.5;

  // Test SetPoles and GetPoles with weights
  NCollection_Array1<double> aFP(1, 6); // 2 poles * (2 coords + 1 weight)
  PLib::SetPoles(aPoles, aWeights, aFP);

  NCollection_Array1<gp_Pnt2d> aResultPoles(1, 2);
  NCollection_Array1<double>   aResultWeights(1, 2);
  PLib::GetPoles(aFP, aResultPoles, aResultWeights);

  // Verify results
  for (int i = 1; i <= 2; i++)
  {
    checkPoint2dEqual(aPoles(i), aResultPoles(i));
    EXPECT_NEAR(aWeights(i), aResultWeights(i), Precision::Confusion());
  }
}

// Test for zero weights handling (safety test)
TEST_F(PLibTest, ZeroWeightsHandling)
{
  // Create test data with zero weight - this should not crash
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(1.0, 2.0);
  aPoles(2) = gp_Pnt2d(3.0, 4.0);

  NCollection_Array1<double> aWeights(1, 2);
  aWeights(1) = 1.0;
  aWeights(2) = 0.0; // Zero weight - potential division by zero

  NCollection_Array1<double> aFP(1, 6);
  PLib::SetPoles(aPoles, aWeights, aFP);

  // This test should complete without crashing
  // The behavior with zero weights may vary, but it shouldn't crash
  EXPECT_TRUE(true); // We mainly test that no crash occurs
}

// Tests for Binomial coefficient function - Basic values
TEST_F(PLibTest, BinomialCoefficient_BasicValues)
{
  // Test edge cases
  EXPECT_NEAR(PLib::Bin(0, 0), 1.0, Precision::Confusion()) << "C(0,0) should be 1";

  // Test row n=1: [1, 1]
  EXPECT_NEAR(PLib::Bin(1, 0), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(1, 1), 1.0, Precision::Confusion());

  // Test row n=5: [1, 5, 10, 10, 5, 1]
  EXPECT_NEAR(PLib::Bin(5, 0), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 1), 5.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 2), 10.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 3), 10.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 4), 5.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(5, 5), 1.0, Precision::Confusion());

  // Test row n=10: some specific values
  EXPECT_NEAR(PLib::Bin(10, 0), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(10, 1), 10.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(10, 2), 45.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(10, 3), 120.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(10, 4), 210.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(10, 5), 252.0, Precision::Confusion());
}

// Tests for Binomial coefficient - Symmetry property
TEST_F(PLibTest, BinomialCoefficient_Symmetry)
{
  // Test symmetry property: C(n,k) = C(n,n-k)
  for (int n = 0; n <= 20; n++)
  {
    for (int k = 0; k <= n; k++)
    {
      EXPECT_NEAR(PLib::Bin(n, k), PLib::Bin(n, n - k), Precision::Confusion())
        << "Binomial coefficient symmetry failed for C(" << n << "," << k << ")";
    }
  }
}

// Tests for Binomial coefficient - Pascal's triangle recurrence
TEST_F(PLibTest, BinomialCoefficient_Recurrence)
{
  // Test Pascal's triangle recurrence: C(n,k) = C(n-1,k-1) + C(n-1,k)
  for (int n = 2; n <= 20; n++)
  {
    for (int k = 1; k < n; k++)
    {
      double expected = PLib::Bin(n - 1, k - 1) + PLib::Bin(n - 1, k);
      double actual   = PLib::Bin(n, k);
      EXPECT_NEAR(actual, expected, Precision::Confusion())
        << "Pascal recurrence failed for C(" << n << "," << k << ")";
    }
  }
}

// Tests for Binomial coefficient - Known large values
TEST_F(PLibTest, BinomialCoefficient_LargeValues)
{
  // Test some larger known values
  EXPECT_NEAR(PLib::Bin(20, 10), 184756.0, Precision::Confusion()) << "C(20,10)";
  EXPECT_NEAR(PLib::Bin(15, 7), 6435.0, Precision::Confusion()) << "C(15,7)";
  EXPECT_NEAR(PLib::Bin(25, 5), 53130.0, Precision::Confusion()) << "C(25,5)";
  EXPECT_NEAR(PLib::Bin(25, 12), 5200300.0, Precision::Confusion()) << "C(25,12)";

  // Test maximum supported degree (25)
  EXPECT_NEAR(PLib::Bin(25, 0), 1.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(25, 1), 25.0, Precision::Confusion());
  EXPECT_NEAR(PLib::Bin(25, 25), 1.0, Precision::Confusion());
}

// Tests for Binomial coefficient - Edge cases with first and last columns
TEST_F(PLibTest, BinomialCoefficient_EdgeColumns)
{
  // Test first column: C(n,0) = 1 for all n
  for (int n = 0; n <= 25; n++)
  {
    EXPECT_NEAR(PLib::Bin(n, 0), 1.0, Precision::Confusion()) << "C(" << n << ",0) should be 1";
  }

  // Test diagonal: C(n,n) = 1 for all n
  for (int n = 0; n <= 25; n++)
  {
    EXPECT_NEAR(PLib::Bin(n, n), 1.0, Precision::Confusion())
      << "C(" << n << "," << n << ") should be 1";
  }

  // Test second column: C(n,1) = n for all n >= 1
  for (int n = 1; n <= 25; n++)
  {
    EXPECT_NEAR(PLib::Bin(n, 1), double(n), Precision::Confusion())
      << "C(" << n << ",1) should be " << n;
  }
}

// Tests for Binomial coefficient - Complete rows verification
TEST_F(PLibTest, BinomialCoefficient_CompleteRows)
{
  // Verify complete row n=6: [1, 6, 15, 20, 15, 6, 1]
  const double row6[] = {1.0, 6.0, 15.0, 20.0, 15.0, 6.0, 1.0};
  for (int k = 0; k <= 6; k++)
  {
    EXPECT_NEAR(PLib::Bin(6, k), row6[k], Precision::Confusion())
      << "C(6," << k << ") verification failed";
  }

  // Verify complete row n=8: [1, 8, 28, 56, 70, 56, 28, 8, 1]
  const double row8[] = {1.0, 8.0, 28.0, 56.0, 70.0, 56.0, 28.0, 8.0, 1.0};
  for (int k = 0; k <= 8; k++)
  {
    EXPECT_NEAR(PLib::Bin(8, k), row8[k], Precision::Confusion())
      << "C(8," << k << ") verification failed";
  }
}

// Tests for Binomial coefficient - Sum property
TEST_F(PLibTest, BinomialCoefficient_SumProperty)
{
  // Test sum property: Sum of C(n,k) for k=0 to n equals 2^n
  for (int n = 0; n <= 20; n++)
  {
    double sum      = 0.0;
    double expected = std::pow(2.0, n);

    for (int k = 0; k <= n; k++)
    {
      sum += PLib::Bin(n, k);
    }

    EXPECT_NEAR(sum, expected, Precision::Confusion())
      << "Sum property failed for n=" << n << " (sum should be 2^" << n << "=" << expected << ")";
  }
}

// Tests for polynomial evaluation
TEST_F(PLibTest, EvalPolynomial)
{
  // Test simple polynomial evaluation: f(x) = 1 + 2x + 3x^2
  const int aDegree     = 2;
  const int aDimension  = 1;
  const int aDerivOrder = 0;

  NCollection_Array1<double> aCoeffs(1, 3);
  aCoeffs(1) = 1.0; // constant term
  aCoeffs(2) = 2.0; // linear term
  aCoeffs(3) = 3.0; // quadratic term

  NCollection_Array1<double> aResults(1, 1);

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
  const int aDegree     = 2;
  const int aDimension  = 1;
  const int aDerivOrder = 2;

  NCollection_Array1<double> aCoeffs(1, 3);
  aCoeffs(1) = 1.0;
  aCoeffs(2) = 2.0;
  aCoeffs(3) = 3.0;

  NCollection_Array1<double> aResults(1, 3); // value + 1st + 2nd derivative

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
  for (int i = 0; i <= 2; i++)
  {
    GeomAbs_Shape aShape = PLib::ConstraintOrder(i);
    int           aLevel = PLib::NivConstr(aShape);
    EXPECT_EQ(aLevel, i) << "Round-trip conversion failed for level " << i;
  }
}

// Test for Hermite interpolation
TEST_F(PLibTest, HermiteInterpolate)
{

  const int    aDimension  = 1;
  const double aFirstParam = 0.0;
  const double aLastParam  = 1.0;
  const int    aFirstOrder = 1; // value + 1st derivative
  const int    aLastOrder  = 1; // value + 1st derivative

  // Define constraints: f(0) = 0, f'(0) = 1, f(1) = 1, f'(1) = 0
  NCollection_Array2<double> aFirstConstr(1, aDimension, 0, aFirstOrder);
  aFirstConstr(1, 0) = 0.0; // f(0) = 0
  aFirstConstr(1, 1) = 1.0; // f'(0) = 1

  NCollection_Array2<double> aLastConstr(1, aDimension, 0, aLastOrder);
  aLastConstr(1, 0) = 1.0; // f(1) = 1
  aLastConstr(1, 1) = 0.0; // f'(1) = 0

  const int aCoeffCount = aFirstOrder + aLastOrder + 2;

  NCollection_Array1<double> aCoeffs(0,
                                     aCoeffCount
                                       - 1); // 0-based indexing as expected by HermiteInterpolate

  // Perform Hermite interpolation
  bool aResult = PLib::HermiteInterpolate(aDimension,
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
    NCollection_Array1<double> aResults(1, 2); // value + 1st derivative

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
  NCollection_Array1<double> aSmallCoeffs(1, 3);
  aSmallCoeffs(1) = 1.0e-12;
  aSmallCoeffs(2) = 1.0e-12;
  aSmallCoeffs(3) = 1.0e-12;

  NCollection_Array1<double> aResults(1, 1);

  // This should not crash even with very small coefficients
  EXPECT_NO_THROW(
    { PLib::EvalPolynomial(1.0, 0, 2, 1, aSmallCoeffs.ChangeValue(1), aResults.ChangeValue(1)); });

  // Test with large coefficients
  NCollection_Array1<double> aLargeCoeffs(1, 3);
  aLargeCoeffs(1) = 1.0e10;
  aLargeCoeffs(2) = 1.0e10;
  aLargeCoeffs(3) = 1.0e10;

  EXPECT_NO_THROW(
    { PLib::EvalPolynomial(1.0, 0, 2, 1, aLargeCoeffs.ChangeValue(1), aResults.ChangeValue(1)); });
}

// Test for Jacobi parameter calculation
TEST_F(PLibTest, JacobiParameters)
{
  int aNbGaussPoints, aWorkDegree;

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
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

#include <PLib_JacobiPolynomial.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>

namespace
{

} // namespace

// Test fixture for PLib_JacobiPolynomial tests
class PLibJacobiPolynomialTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Common test setup
  }

  void TearDown() override
  {
    // Cleanup
  }
};

// Test constructor and basic properties
TEST_F(PLibJacobiPolynomialTest, ConstructorAndBasicProperties)
{
  // Test with different constraint orders
  PLib_JacobiPolynomial aJacC0(10, GeomAbs_C0);
  PLib_JacobiPolynomial aJacC1(15, GeomAbs_C1);
  PLib_JacobiPolynomial aJacC2(20, GeomAbs_C2);

  // Test WorkDegree property
  EXPECT_EQ(aJacC0.WorkDegree(), 10);
  EXPECT_EQ(aJacC1.WorkDegree(), 15);
  EXPECT_EQ(aJacC2.WorkDegree(), 20);

  // Test NivConstr property
  EXPECT_EQ(aJacC0.NivConstr(), 0);
  EXPECT_EQ(aJacC1.NivConstr(), 1);
  EXPECT_EQ(aJacC2.NivConstr(), 2);
}

// Test constructor with edge cases
TEST_F(PLibJacobiPolynomialTest, ConstructorEdgeCases)
{
  // Test minimum valid WorkDegree for each constraint order
  // For C0: WorkDegree >= 2 to get myDegree >= 0
  PLib_JacobiPolynomial aJacMinC0(2, GeomAbs_C0);
  EXPECT_EQ(aJacMinC0.WorkDegree(), 2);

  // For C1: WorkDegree >= 4 to get myDegree >= 0
  PLib_JacobiPolynomial aJacMinC1(4, GeomAbs_C1);
  EXPECT_EQ(aJacMinC1.WorkDegree(), 4);

  // For C2: WorkDegree >= 6 to get myDegree >= 0
  PLib_JacobiPolynomial aJacMinC2(6, GeomAbs_C2);
  EXPECT_EQ(aJacMinC2.WorkDegree(), 6);

  // Test reasonable maximum WorkDegree
  PLib_JacobiPolynomial aJacMax(30, GeomAbs_C0);
  EXPECT_EQ(aJacMax.WorkDegree(), 30);

  // Test reasonable high degrees
  PLib_JacobiPolynomial aJacHigh(25, GeomAbs_C0);
  EXPECT_GT(aJacHigh.WorkDegree(), 20) << "High degree should be supported";
}

// Test Gauss integration points
TEST_F(PLibJacobiPolynomialTest, GaussIntegrationPoints)
{
  PLib_JacobiPolynomial aJac(10, GeomAbs_C0);

  // Test various numbers of Gauss points (only valid values supported by OCCT)
  std::vector<Standard_Integer> aGaussNumbers = {8, 10, 15, 20, 25, 30, 40, 50, 61};

  for (Standard_Integer aNbGauss : aGaussNumbers)
  {
    if (aNbGauss > aJac.WorkDegree())
    {
      TColStd_Array1OfReal aPoints(0, aNbGauss / 2);

      EXPECT_NO_THROW({ aJac.Points(aNbGauss, aPoints); })
        << "Points calculation failed for " << aNbGauss << " Gauss points";

      // Verify points are in valid range and ordered
      for (Standard_Integer i = aPoints.Lower(); i <= aPoints.Upper(); i++)
      {
        if (i == 0 && aNbGauss % 2 == 0)
        {
          // For even number of Gauss points, TabPoints(0) is set to UNDEFINED (-999)
          EXPECT_EQ(aPoints(i), -999) << "TabPoints(0) should be UNDEFINED for even NbGaussPoints";
        }
        else if (i == 0 && aNbGauss % 2 == 1)
        {
          // For odd number of Gauss points, TabPoints(0) should be 0
          EXPECT_EQ(aPoints(i), 0.0) << "TabPoints(0) should be 0 for odd NbGaussPoints";
        }
        else
        {
          // Other points should be positive and <= 1
          EXPECT_GT(aPoints(i), 0.0) << "Gauss point should be positive";
          EXPECT_LE(aPoints(i), 1.0) << "Gauss point should be <= 1";

          if (i > aPoints.Lower() && i > 0)
          {
            EXPECT_GE(aPoints(i), aPoints(i - 1)) << "Gauss points should be ordered";
          }
        }
      }
    }
  }
}

// Test Gauss integration weights
TEST_F(PLibJacobiPolynomialTest, GaussIntegrationWeights)
{
  PLib_JacobiPolynomial aJac(8, GeomAbs_C1);

  Standard_Integer     aNbGauss = 15; // Must be > degree for valid computation
  TColStd_Array2OfReal aWeights(0, aNbGauss / 2, 0, aJac.WorkDegree());

  aJac.Weights(aNbGauss, aWeights);

  // Basic sanity checks on weights - the array is 2D with specific bounds
  EXPECT_EQ(aWeights.LowerRow(), 0) << "Lower row should be 0";
  EXPECT_EQ(aWeights.UpperRow(), aNbGauss / 2) << "Upper row mismatch";
  EXPECT_EQ(aWeights.LowerCol(), 0) << "Lower col should be 0";
  EXPECT_EQ(aWeights.UpperCol(), aJac.WorkDegree()) << "Upper col should match work degree";

  for (Standard_Integer i = aWeights.LowerRow(); i <= aWeights.UpperRow(); i++)
  {
    for (Standard_Integer j = aWeights.LowerCol(); j <= aWeights.UpperCol(); j++)
    {
      EXPECT_FALSE(Precision::IsInfinite(aWeights(i, j)))
        << "Weight should be finite at (" << i << "," << j << ")";
    }
  }
}

// Test MaxValue computation
TEST_F(PLibJacobiPolynomialTest, MaxValue)
{
  PLib_JacobiPolynomial aJac(10, GeomAbs_C0);

  Standard_Integer aTabSize = aJac.WorkDegree() - 2 * (aJac.NivConstr() + 1);
  if (aTabSize > 0)
  {
    TColStd_Array1OfReal aTabMax(0, aTabSize);

    aJac.MaxValue(aTabMax);

    // Verify all max values are positive (they represent maximum absolute values)
    for (Standard_Integer i = aTabMax.Lower(); i <= aTabMax.Upper(); i++)
    {
      EXPECT_GT(aTabMax(i), 0.0) << "Max value should be positive at index " << i;
      EXPECT_FALSE(Precision::IsInfinite(aTabMax(i)))
        << "Max value should be finite at index " << i;
    }
  }
}

// Test basis function evaluation D0
TEST_F(PLibJacobiPolynomialTest, BasisFunctionD0)
{
  PLib_JacobiPolynomial aJac(6, GeomAbs_C0);

  // Calculate actual number of basis functions
  Standard_Integer aDegree = aJac.WorkDegree() - 2 * (aJac.NivConstr() + 1);

  TColStd_Array1OfReal aBasisValue(0, aDegree);

  // Test at various parameter values
  std::vector<Standard_Real> aTestParams = {-1.0, -0.5, 0.0, 0.5, 1.0};

  for (Standard_Real aU : aTestParams)
  {
    aJac.D0(aU, aBasisValue);

    // Basic sanity checks
    for (Standard_Integer i = aBasisValue.Lower(); i <= aBasisValue.Upper(); i++)
    {
      EXPECT_FALSE(Precision::IsInfinite(aBasisValue(i)))
        << "Basis value should be finite at index " << i << ", U=" << aU;
    }
  }
}

// Test basis function evaluation with derivatives
TEST_F(PLibJacobiPolynomialTest, BasisFunctionDerivatives)
{
  PLib_JacobiPolynomial aJac(8, GeomAbs_C1);

  // Calculate actual number of basis functions (same as MaxValue test pattern)
  Standard_Integer aDegree = aJac.WorkDegree() - 2 * (aJac.NivConstr() + 1);

  TColStd_Array1OfReal aBasisValue(0, aDegree);
  TColStd_Array1OfReal aBasisD1(0, aDegree);
  TColStd_Array1OfReal aBasisD2(0, aDegree);
  TColStd_Array1OfReal aBasisD3(0, aDegree);

  Standard_Real aU = 0.5; // Test at middle point

  // Test D1, D2, D3 evaluations
  aJac.D1(aU, aBasisValue, aBasisD1);
  aJac.D2(aU, aBasisValue, aBasisD1, aBasisD2);
  aJac.D3(aU, aBasisValue, aBasisD1, aBasisD2, aBasisD3);

  // Verify all values are finite
  for (Standard_Integer i = aBasisValue.Lower(); i <= aBasisValue.Upper(); i++)
  {
    EXPECT_FALSE(Precision::IsInfinite(aBasisValue(i))) << "Basis value should be finite at " << i;
    EXPECT_FALSE(Precision::IsInfinite(aBasisD1(i)))
      << "First derivative should be finite at " << i;
    EXPECT_FALSE(Precision::IsInfinite(aBasisD2(i)))
      << "Second derivative should be finite at " << i;
    EXPECT_FALSE(Precision::IsInfinite(aBasisD3(i)))
      << "Third derivative should be finite at " << i;
  }
}

// Test coefficient conversion
TEST_F(PLibJacobiPolynomialTest, CoefficientConversion)
{
  const Standard_Integer aWorkDegree = 6; // Use smaller degree that works well with ToCoefficients
  PLib_JacobiPolynomial  aJac(aWorkDegree, GeomAbs_C0);

  const Standard_Integer aDimension = 1;
  const Standard_Integer aDegree =
    aJac.WorkDegree() - 2 * (aJac.NivConstr() + 1); // Use computational degree

  // Create test Jacobi coefficients with proper size
  // ToCoefficients expects arrays sized based on the degree and dimension
  // Analysis shows we need arrays that can handle the indexing patterns in the method
  Standard_Integer aJacSize   = (aDegree + 1) * aDimension;
  Standard_Integer aCoeffSize = (aDegree + 1) * aDimension;

  // Use 0-based arrays to match the ToCoefficients indexing expectations
  TColStd_Array1OfReal aJacCoeff(0, aJacSize - 1);
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = std::sin(i * 0.1); // Some test values
  }

  TColStd_Array1OfReal aCoefficients(0, aCoeffSize - 1);

  aJac.ToCoefficients(aDimension, aDegree, aJacCoeff, aCoefficients);

  // Verify output is finite
  for (Standard_Integer i = aCoefficients.Lower(); i <= aCoefficients.Upper(); i++)
  {
    EXPECT_FALSE(Precision::IsInfinite(aCoefficients(i)))
      << "Converted coefficient should be finite at index " << i;
  }
}

// Test degree reduction
TEST_F(PLibJacobiPolynomialTest, DegreeReduction)
{
  PLib_JacobiPolynomial aJac(10, GeomAbs_C0);

  const Standard_Integer aDimension = 1;
  const Standard_Integer aMaxDegree = 8;
  const Standard_Real    aTol       = 1e-6;

  // Create test coefficients - must be sized for full WorkDegree
  const Standard_Integer aWorkDegree = aJac.WorkDegree();
  TColStd_Array1OfReal   aCoeff(1, (aWorkDegree + 1) * aDimension);
  for (Standard_Integer i = aCoeff.Lower(); i <= aCoeff.Upper(); i++)
  {
    aCoeff(i) = 1.0 / (i + 1); // Decreasing coefficients to allow reduction
  }

  Standard_Integer aNewDegree = -1;
  Standard_Real    aMaxError  = -1.0;

  aJac.ReduceDegree(aDimension, aMaxDegree, aTol, aCoeff.ChangeValue(1), aNewDegree, aMaxError);

  // Verify results are reasonable
  EXPECT_LE(aNewDegree, aMaxDegree) << "New degree should not exceed max degree";
  EXPECT_GE(aNewDegree, 0) << "New degree should be non-negative";
  EXPECT_GE(aMaxError, 0.0) << "Max error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aMaxError)) << "Max error should be finite";
}

// Test error estimation
TEST_F(PLibJacobiPolynomialTest, ErrorEstimation)
{
  PLib_JacobiPolynomial aJac(8, GeomAbs_C1);

  const Standard_Integer aDimension = 1;

  // Create test coefficients
  TColStd_Array1OfReal aCoeff(1, 10 * aDimension);
  for (Standard_Integer i = aCoeff.Lower(); i <= aCoeff.Upper(); i++)
  {
    aCoeff(i) = 1.0 / (i + 1);
  }

  Standard_Integer aNewDegree = 6; // Reduced from original

  // Test MaxError
  Standard_Real aMaxErr = aJac.MaxError(aDimension, aCoeff.ChangeValue(1), aNewDegree);

  EXPECT_GE(aMaxErr, 0.0) << "Max error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aMaxErr)) << "Max error should be finite";

  // Test AverageError
  Standard_Real aAvgErr = aJac.AverageError(aDimension, aCoeff.ChangeValue(1), aNewDegree);

  EXPECT_GE(aAvgErr, 0.0) << "Average error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aAvgErr)) << "Average error should be finite";

  // Average error should typically be <= max error
  EXPECT_LE(aAvgErr, aMaxErr + Precision::Confusion())
    << "Average error should not exceed max error significantly";
}

// Performance and stress tests
TEST_F(PLibJacobiPolynomialTest, StressTests)
{
  // Test with maximum degree
  PLib_JacobiPolynomial aJacMax(30, GeomAbs_C2);

  // Calculate actual number of basis functions
  Standard_Integer aDegree = aJacMax.WorkDegree() - 2 * (aJacMax.NivConstr() + 1);

  // Test that basic operations work with high degrees
  TColStd_Array1OfReal aBasisValue(0, aDegree);

  aJacMax.D0(0.0, aBasisValue);
  aJacMax.D0(0.5, aBasisValue);
  aJacMax.D0(1.0, aBasisValue);

  // Test with extreme parameter values
  std::vector<Standard_Real> aExtremeParams = {-0.99999, -1e-10, 1e-10, 0.99999};

  for (Standard_Real aU : aExtremeParams)
  {
    aJacMax.D0(aU, aBasisValue);
    // Verify basis values are finite
    for (Standard_Integer i = aBasisValue.Lower(); i <= aBasisValue.Upper(); i++)
    {
      EXPECT_FALSE(Precision::IsInfinite(aBasisValue(i))) << "Basis value should be finite";
    }
  }
}

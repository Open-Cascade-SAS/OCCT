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

#include <PLib_HermitJacobi.hxx>
#include <PLib_JacobiPolynomial.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>

// Test fixture for PLib_HermitJacobi tests
class PLibHermitJacobiTest : public ::testing::Test
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
TEST_F(PLibHermitJacobiTest, ConstructorAndBasicProperties)
{
  // Test with different constraint orders
  PLib_HermitJacobi aHermC0(10, GeomAbs_C0);
  PLib_HermitJacobi aHermC1(15, GeomAbs_C1);
  PLib_HermitJacobi aHermC2(20, GeomAbs_C2);

  // Test WorkDegree property
  EXPECT_EQ(aHermC0.WorkDegree(), 10);
  EXPECT_EQ(aHermC1.WorkDegree(), 15);
  EXPECT_EQ(aHermC2.WorkDegree(), 20);

  // Test NivConstr property
  EXPECT_EQ(aHermC0.NivConstr(), 0);
  EXPECT_EQ(aHermC1.NivConstr(), 1);
  EXPECT_EQ(aHermC2.NivConstr(), 2);
}

// Test basis function evaluation D0
TEST_F(PLibHermitJacobiTest, BasisFunctionD0)
{
  PLib_HermitJacobi aHerm(6, GeomAbs_C0);

  TColStd_Array1OfReal aBasisValue(0, aHerm.WorkDegree());

  // Test at various parameter values
  std::vector<Standard_Real> aTestParams = {-1.0, -0.5, 0.0, 0.5, 1.0};

  for (Standard_Real aU : aTestParams)
  {
    EXPECT_NO_THROW({ aHerm.D0(aU, aBasisValue); }) << "D0 evaluation failed at U=" << aU;

    // Basic sanity checks
    for (Standard_Integer i = aBasisValue.Lower(); i <= aBasisValue.Upper(); i++)
    {
      EXPECT_FALSE(Precision::IsInfinite(aBasisValue(i)))
        << "Basis value should be finite at index " << i << ", U=" << aU;
    }
  }
}

// Test basis function evaluation with derivatives
TEST_F(PLibHermitJacobiTest, BasisFunctionDerivatives)
{
  PLib_HermitJacobi aHerm(8, GeomAbs_C1);

  TColStd_Array1OfReal aBasisValue(0, aHerm.WorkDegree());
  TColStd_Array1OfReal aBasisD1(0, aHerm.WorkDegree());
  TColStd_Array1OfReal aBasisD2(0, aHerm.WorkDegree());
  TColStd_Array1OfReal aBasisD3(0, aHerm.WorkDegree());

  Standard_Real aU = 0.5; // Test at middle point

  // Test D1
  EXPECT_NO_THROW({ aHerm.D1(aU, aBasisValue, aBasisD1); }) << "D1 evaluation failed";

  // Test D2
  EXPECT_NO_THROW({ aHerm.D2(aU, aBasisValue, aBasisD1, aBasisD2); }) << "D2 evaluation failed";

  // Test D3
  EXPECT_NO_THROW({ aHerm.D3(aU, aBasisValue, aBasisD1, aBasisD2, aBasisD3); })
    << "D3 evaluation failed";

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
TEST_F(PLibHermitJacobiTest, CoefficientConversion)
{
  const Standard_Integer aWorkDegree = 6; // Use smaller degree that works well with ToCoefficients
  PLib_HermitJacobi      aHerm(aWorkDegree, GeomAbs_C0);

  const Standard_Integer aDimension = 1;
  const Standard_Integer aDegree =
    aHerm.WorkDegree() - 2 * (aHerm.NivConstr() + 1); // Use computational degree

  // Create test HermitJacobi coefficients with proper size
  // ToCoefficients expects arrays sized based on the degree and dimension
  Standard_Integer aHermJacSize = (aDegree + 1) * aDimension;
  Standard_Integer aCoeffSize   = (aDegree + 1) * aDimension;

  // Use 0-based arrays to match the ToCoefficients indexing expectations
  TColStd_Array1OfReal aHermJacCoeff(0, aHermJacSize - 1);
  for (Standard_Integer i = aHermJacCoeff.Lower(); i <= aHermJacCoeff.Upper(); i++)
  {
    aHermJacCoeff(i) = std::sin(i * 0.3); // Some test values
  }

  TColStd_Array1OfReal aCoefficients(0, aCoeffSize - 1);

  EXPECT_NO_THROW({ aHerm.ToCoefficients(aDimension, aDegree, aHermJacCoeff, aCoefficients); })
    << "Coefficient conversion failed";

  // Verify output is finite
  for (Standard_Integer i = aCoefficients.Lower(); i <= aCoefficients.Upper(); i++)
  {
    EXPECT_FALSE(Precision::IsInfinite(aCoefficients(i)))
      << "Converted coefficient should be finite at index " << i;
  }
}

// Test degree reduction
TEST_F(PLibHermitJacobiTest, DegreeReduction)
{
  PLib_HermitJacobi aHerm(10, GeomAbs_C0);

  const Standard_Integer aDimension = 1;
  const Standard_Integer aMaxDegree = 8;
  const Standard_Real    aTol       = 1e-6;

  // Create test coefficients - must be sized for full WorkDegree
  const Standard_Integer aWorkDegree = aHerm.WorkDegree();
  TColStd_Array1OfReal   aCoeff(1, (aWorkDegree + 1) * aDimension);
  for (Standard_Integer i = aCoeff.Lower(); i <= aCoeff.Upper(); i++)
  {
    aCoeff(i) = 1.0 / (i + 1); // Decreasing coefficients to allow reduction
  }

  Standard_Integer aNewDegree = -1;
  Standard_Real    aMaxError  = -1.0;

  EXPECT_NO_THROW({
    aHerm.ReduceDegree(aDimension, aMaxDegree, aTol, aCoeff.ChangeValue(1), aNewDegree, aMaxError);
  }) << "Degree reduction failed";

  // Verify results are reasonable
  EXPECT_LE(aNewDegree, aMaxDegree) << "New degree should not exceed max degree";
  EXPECT_GE(aNewDegree, 0) << "New degree should be non-negative";
  EXPECT_GE(aMaxError, 0.0) << "Max error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aMaxError)) << "Max error should be finite";
}

// Test error estimation
TEST_F(PLibHermitJacobiTest, ErrorEstimation)
{
  PLib_HermitJacobi aHerm(8, GeomAbs_C1);

  const Standard_Integer aDimension = 1;

  // Create test coefficients
  TColStd_Array1OfReal aCoeff(1, 10 * aDimension);
  for (Standard_Integer i = aCoeff.Lower(); i <= aCoeff.Upper(); i++)
  {
    aCoeff(i) = 1.0 / (i + 1);
  }

  Standard_Integer aNewDegree = 6; // Reduced from original

  // Test MaxError
  Standard_Real aMaxErr = -1.0;
  EXPECT_NO_THROW({ aMaxErr = aHerm.MaxError(aDimension, aCoeff.ChangeValue(1), aNewDegree); })
    << "MaxError calculation failed";

  EXPECT_GE(aMaxErr, 0.0) << "Max error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aMaxErr)) << "Max error should be finite";

  // Test AverageError
  Standard_Real aAvgErr = -1.0;
  EXPECT_NO_THROW({ aAvgErr = aHerm.AverageError(aDimension, aCoeff.ChangeValue(1), aNewDegree); })
    << "AverageError calculation failed";

  EXPECT_GE(aAvgErr, 0.0) << "Average error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aAvgErr)) << "Average error should be finite";

  // Average error should typically be <= max error
  EXPECT_LE(aAvgErr, aMaxErr + Precision::Confusion())
    << "Average error should not exceed max error significantly";
}

// Test extreme parameter values
TEST_F(PLibHermitJacobiTest, ExtremeParameterValues)
{
  PLib_HermitJacobi aHerm(10, GeomAbs_C2);

  TColStd_Array1OfReal aBasisValue(0, aHerm.WorkDegree());

  // Test with boundary values
  std::vector<Standard_Real> aExtremeParams = {-0.99999, -1e-12, 1e-12, 0.99999};

  for (Standard_Real aU : aExtremeParams)
  {
    EXPECT_NO_THROW({ aHerm.D0(aU, aBasisValue); })
      << "Extreme parameter U=" << aU << " should not crash";

    // Check that results are finite
    for (Standard_Integer i = aBasisValue.Lower(); i <= aBasisValue.Upper(); i++)
    {
      EXPECT_FALSE(Precision::IsInfinite(aBasisValue(i)))
        << "Basis value should be finite at extreme parameter U=" << aU;
    }
  }
}

// Test consistency between different derivative orders
TEST_F(PLibHermitJacobiTest, DerivativeConsistency)
{
  PLib_HermitJacobi aHerm(6, GeomAbs_C2);

  TColStd_Array1OfReal aBasisValue1(0, aHerm.WorkDegree());
  TColStd_Array1OfReal aBasisD1_1(0, aHerm.WorkDegree());

  TColStd_Array1OfReal aBasisValue2(0, aHerm.WorkDegree());
  TColStd_Array1OfReal aBasisD1_2(0, aHerm.WorkDegree());
  TColStd_Array1OfReal aBasisD2_2(0, aHerm.WorkDegree());

  Standard_Real aU = 0.3;

  // Get values from D1 and D2 calls
  aHerm.D1(aU, aBasisValue1, aBasisD1_1);
  aHerm.D2(aU, aBasisValue2, aBasisD1_2, aBasisD2_2);

  // Values and first derivatives should be consistent
  for (Standard_Integer i = aBasisValue1.Lower(); i <= aBasisValue1.Upper(); i++)
  {
    EXPECT_NEAR(aBasisValue1(i), aBasisValue2(i), Precision::Confusion())
      << "Function values should be consistent between D1 and D2 calls at index " << i;
    EXPECT_NEAR(aBasisD1_1(i), aBasisD1_2(i), Precision::Confusion())
      << "First derivatives should be consistent between D1 and D2 calls at index " << i;
  }
}

// Performance test with maximum degree
TEST_F(PLibHermitJacobiTest, PerformanceTest)
{
  PLib_HermitJacobi aHermMax(30, GeomAbs_C2);

  TColStd_Array1OfReal aBasisValue(0, aHermMax.WorkDegree());
  TColStd_Array1OfReal aBasisD1(0, aHermMax.WorkDegree());
  TColStd_Array1OfReal aBasisD2(0, aHermMax.WorkDegree());
  TColStd_Array1OfReal aBasisD3(0, aHermMax.WorkDegree());

  // Test that operations complete in reasonable time
  std::vector<Standard_Real> aTestParams = {-0.8, -0.5, 0.0, 0.5, 0.8};

  for (Standard_Real aU : aTestParams)
  {
    EXPECT_NO_THROW({
      aHermMax.D0(aU, aBasisValue);
      aHermMax.D1(aU, aBasisValue, aBasisD1);
      aHermMax.D2(aU, aBasisValue, aBasisD1, aBasisD2);
      aHermMax.D3(aU, aBasisValue, aBasisD1, aBasisD2, aBasisD3);
    }) << "High degree operations should complete without crashing at U="
       << aU;
  }
}

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

#include <PLib_DoubleJacobiPolynomial.hxx>
#include <PLib_JacobiPolynomial.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>

// Test fixture for PLib_DoubleJacobiPolynomial tests
class PLibDoubleJacobiPolynomialTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create Jacobi polynomials for testing
    myJacobiU = new PLib_JacobiPolynomial(10, GeomAbs_C0);
    myJacobiV = new PLib_JacobiPolynomial(8, GeomAbs_C1);
  }

  void TearDown() override
  {
    myJacobiU.Nullify();
    myJacobiV.Nullify();
  }

  Handle(PLib_JacobiPolynomial) myJacobiU;
  Handle(PLib_JacobiPolynomial) myJacobiV;

  // Helper to create test coefficients
  void createTestCoefficients(TColStd_Array1OfReal& theCoeffs,
                              Standard_Integer      theDimension,
                              Standard_Integer      theDegreeU,
                              Standard_Integer      theDegreeV)
  {
    Standard_Integer anIndex = theCoeffs.Lower(); // Start from array lower bound
    for (Standard_Integer j = 0; j <= theDegreeV; j++)
    {
      for (Standard_Integer i = 0; i <= theDegreeU; i++)
      {
        for (Standard_Integer d = 1; d <= theDimension; d++)
        {
          theCoeffs(anIndex) = Sin(anIndex * 0.1 + i * 0.2 + j * 0.3);
          anIndex++;
        }
      }
    }
  }
};

// Test default constructor
TEST_F(PLibDoubleJacobiPolynomialTest, DefaultConstructor)
{
  PLib_DoubleJacobiPolynomial aDoubleJac;

  // After default construction, accessors should return null handles
  EXPECT_TRUE(aDoubleJac.U().IsNull()) << "U polynomial should be null after default construction";
  EXPECT_TRUE(aDoubleJac.V().IsNull()) << "V polynomial should be null after default construction";
  EXPECT_TRUE(aDoubleJac.TabMaxU().IsNull()) << "TabMaxU should be null after default construction";
  EXPECT_TRUE(aDoubleJac.TabMaxV().IsNull()) << "TabMaxV should be null after default construction";
}

// Test parameterized constructor
TEST_F(PLibDoubleJacobiPolynomialTest, ParameterizedConstructor)
{
  ASSERT_FALSE(myJacobiU.IsNull()) << "JacobiU should not be null";
  ASSERT_FALSE(myJacobiV.IsNull()) << "JacobiV should not be null";

  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  // After construction with parameters, accessors should return valid handles
  EXPECT_FALSE(aDoubleJac.U().IsNull()) << "U polynomial should not be null";
  EXPECT_FALSE(aDoubleJac.V().IsNull()) << "V polynomial should not be null";
  EXPECT_FALSE(aDoubleJac.TabMaxU().IsNull()) << "TabMaxU should not be null";
  EXPECT_FALSE(aDoubleJac.TabMaxV().IsNull()) << "TabMaxV should not be null";

  // Test that the returned handles are correct
  EXPECT_EQ(aDoubleJac.U().get(), myJacobiU.get()) << "U polynomial handle should match";
  EXPECT_EQ(aDoubleJac.V().get(), myJacobiV.get()) << "V polynomial handle should match";
}

// Test MaxErrorU calculation
TEST_F(PLibDoubleJacobiPolynomialTest, MaxErrorU)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  const Standard_Integer aDimension = 2;
  const Standard_Integer aDegreeU   = 6;
  const Standard_Integer aDegreeV   = 5;
  const Standard_Integer aDJacCoeff = (aDegreeU + 1) * aDimension;

  // JacCoeff array must be sized based on WorkDegrees
  const Standard_Integer aWorkDegreeU = myJacobiU->WorkDegree();
  const Standard_Integer aWorkDegreeV = myJacobiV->WorkDegree();
  const Standard_Integer aCoeffCount  = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;

  TColStd_Array1OfReal aJacCoeff(0, aCoeffCount - 1);
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = Sin(i * 0.1);
  }

  EXPECT_NO_THROW({
    Standard_Real aMaxErrU =
      aDoubleJac.MaxErrorU(aDimension, aDegreeU, aDegreeV, aDJacCoeff, aJacCoeff);
    EXPECT_GE(aMaxErrU, 0.0) << "MaxErrorU should be non-negative";
    EXPECT_FALSE(Precision::IsInfinite(aMaxErrU)) << "MaxErrorU should be finite";
  }) << "MaxErrorU calculation failed";
}

// Test MaxErrorV calculation
TEST_F(PLibDoubleJacobiPolynomialTest, MaxErrorV)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  const Standard_Integer aDimension = 2;
  const Standard_Integer aDegreeU   = 6;
  const Standard_Integer aDegreeV   = 5;
  const Standard_Integer aDJacCoeff = (aDegreeU + 1) * aDimension;

  // JacCoeff array must be sized based on WorkDegrees
  const Standard_Integer aWorkDegreeU = myJacobiU->WorkDegree();
  const Standard_Integer aWorkDegreeV = myJacobiV->WorkDegree();
  const Standard_Integer aCoeffCount  = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;

  TColStd_Array1OfReal aJacCoeff(0, aCoeffCount - 1);
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = Sin(i * 0.1);
  }

  EXPECT_NO_THROW({
    Standard_Real aMaxErrV =
      aDoubleJac.MaxErrorV(aDimension, aDegreeU, aDegreeV, aDJacCoeff, aJacCoeff);
    EXPECT_GE(aMaxErrV, 0.0) << "MaxErrorV should be non-negative";
    EXPECT_FALSE(Precision::IsInfinite(aMaxErrV)) << "MaxErrorV should be finite";
  }) << "MaxErrorV calculation failed";
}

// Test general MaxError calculation
TEST_F(PLibDoubleJacobiPolynomialTest, MaxError)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  const Standard_Integer aDimension  = 1;
  const Standard_Integer aMinDegreeU = 2, aMaxDegreeU = 5;
  const Standard_Integer aMinDegreeV = 4, aMaxDegreeV = 7; // MinDegreeV must be >= MinV=4
  const Standard_Integer aDJacCoeff = (aMaxDegreeU + 1) * aDimension;
  const Standard_Real    anError    = 1e-6;

  // JacCoeff array must be sized based on WorkDegrees
  const Standard_Integer aWorkDegreeU = myJacobiU->WorkDegree();
  const Standard_Integer aWorkDegreeV = myJacobiV->WorkDegree();
  const Standard_Integer aCoeffCount  = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;

  TColStd_Array1OfReal aJacCoeff(0, aCoeffCount - 1);
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = Sin(i * 0.1);
  }

  EXPECT_NO_THROW({
    Standard_Real aMaxErr = aDoubleJac.MaxError(aDimension,
                                                aMinDegreeU,
                                                aMaxDegreeU,
                                                aMinDegreeV,
                                                aMaxDegreeV,
                                                aDJacCoeff,
                                                aJacCoeff,
                                                anError);
    EXPECT_GE(aMaxErr, 0.0) << "MaxError should be non-negative";
    EXPECT_FALSE(Precision::IsInfinite(aMaxErr)) << "MaxError should be finite";
  }) << "MaxError calculation failed";
}

// Test degree reduction
TEST_F(PLibDoubleJacobiPolynomialTest, ReduceDegree)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  const Standard_Integer aDimension  = 1;
  const Standard_Integer aMinDegreeU = 2, aMaxDegreeU = 6; // Must be >= MinU=2
  const Standard_Integer aMinDegreeV = 4, aMaxDegreeV = 7; // Must be >= MinV=4  
  const Standard_Integer aDJacCoeff  = (aMaxDegreeU + 1) * aDimension;
  const Standard_Real    anEpmsCut   = 1e-8;

  // JacCoeff array must be sized based on WorkDegrees and use 0-based indexing
  const Standard_Integer aWorkDegreeU = myJacobiU->WorkDegree();
  const Standard_Integer aWorkDegreeV = myJacobiV->WorkDegree();
  const Standard_Integer aCoeffCount = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;

  TColStd_Array1OfReal aJacCoeff(0, aCoeffCount - 1);
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = 1.0 / (i + 10); // Small decreasing values
  }

  Standard_Real    aMaxError   = -1.0;
  Standard_Integer aNewDegreeU = -1, aNewDegreeV = -1;

  EXPECT_NO_THROW({
    aDoubleJac.ReduceDegree(aDimension,
                            aMinDegreeU,
                            aMaxDegreeU,
                            aMinDegreeV,
                            aMaxDegreeV,
                            aDJacCoeff,
                            aJacCoeff,
                            anEpmsCut,
                            aMaxError,
                            aNewDegreeU,
                            aNewDegreeV);
  }) << "ReduceDegree failed";

  // Verify results are reasonable
  EXPECT_LE(aNewDegreeU, aMaxDegreeU) << "New U degree should not exceed max";
  EXPECT_GE(aNewDegreeU, aMinDegreeU) << "New U degree should be at least min";
  EXPECT_LE(aNewDegreeV, aMaxDegreeV) << "New V degree should not exceed max";
  EXPECT_GE(aNewDegreeV, aMinDegreeV) << "New V degree should be at least min";
  EXPECT_GE(aMaxError, 0.0) << "Max error should be non-negative";
  EXPECT_FALSE(Precision::IsInfinite(aMaxError)) << "Max error should be finite";
}

// Test average error calculation
TEST_F(PLibDoubleJacobiPolynomialTest, AverageError)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  const Standard_Integer aDimension  = 2;
  const Standard_Integer aDegreeU    = 4;
  const Standard_Integer aDegreeV    = 3;
  const Standard_Integer aDJacCoeff  = 0; // For 0-based arrays, start offset should be 0

  // JacCoeff array must be sized based on WorkDegrees and use 0-based indexing
  const Standard_Integer aWorkDegreeU = myJacobiU->WorkDegree();
  const Standard_Integer aWorkDegreeV = myJacobiV->WorkDegree();
  const Standard_Integer aCoeffCount = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;

  TColStd_Array1OfReal aJacCoeff(0, aCoeffCount - 1);
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = Sin(i * 0.1);
  }

  EXPECT_NO_THROW({
    Standard_Real aAvgErr =
      aDoubleJac.AverageError(aDimension, aDegreeU, aDegreeV, aDJacCoeff, aJacCoeff);
    EXPECT_GE(aAvgErr, 0.0) << "Average error should be non-negative";
    EXPECT_FALSE(Precision::IsInfinite(aAvgErr)) << "Average error should be finite";
  }) << "AverageError calculation failed";
}

// Test coefficient conversion to canonical base
TEST_F(PLibDoubleJacobiPolynomialTest, CoefficientConversion)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  const Standard_Integer aDimension = 3;
  const Standard_Integer aDegreeU   = 3;
  const Standard_Integer aDegreeV   = 2;

  // JacCoeff array must be sized based on WorkDegrees, not input degrees
  const Standard_Integer aWorkDegreeU  = myJacobiU->WorkDegree();
  const Standard_Integer aWorkDegreeV  = myJacobiV->WorkDegree();
  const Standard_Integer aJacCoeffSize = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;

  // JacCoeff uses 0-based indexing as per the implementation
  TColStd_Array1OfReal aJacCoeff(0, aJacCoeffSize - 1);
  // Initialize with test data - use simple pattern for valid coefficients
  for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
  {
    aJacCoeff(i) = Sin(i * 0.1);
  }

  // Output coefficients array is sized based on actual degrees
  const Standard_Integer aCoeffCount = (aDegreeU + 1) * (aDegreeV + 1) * aDimension;
  TColStd_Array1OfReal   aCoefficients(0, aCoeffCount - 1);

  EXPECT_NO_THROW({
    aDoubleJac.WDoubleJacobiToCoefficients(aDimension,
                                           aDegreeU,
                                           aDegreeV,
                                           aJacCoeff,
                                           aCoefficients);
  }) << "Coefficient conversion failed";

  // Verify output coefficients are finite
  for (Standard_Integer i = aCoefficients.Lower(); i <= aCoefficients.Upper(); i++)
  {
    EXPECT_FALSE(Precision::IsInfinite(aCoefficients(i)))
      << "Converted coefficient should be finite at index " << i;
  }
}


// Test with mismatched degrees
TEST_F(PLibDoubleJacobiPolynomialTest, MismatchedDegrees)
{
  Handle(PLib_JacobiPolynomial) aJacU_Low  = new PLib_JacobiPolynomial(5, GeomAbs_C0);
  Handle(PLib_JacobiPolynomial) aJacV_High = new PLib_JacobiPolynomial(20, GeomAbs_C1);

  EXPECT_NO_THROW({
    PLib_DoubleJacobiPolynomial aDoubleJac(aJacU_Low, aJacV_High);

    // Should handle mismatched degrees gracefully
    EXPECT_FALSE(aDoubleJac.U().IsNull());
    EXPECT_FALSE(aDoubleJac.V().IsNull());
  }) << "Constructor should handle mismatched degrees";
}

// Test accessor consistency
TEST_F(PLibDoubleJacobiPolynomialTest, AccessorConsistency)
{
  PLib_DoubleJacobiPolynomial aDoubleJac(myJacobiU, myJacobiV);

  // Test that accessors return consistent results
  Handle(PLib_JacobiPolynomial) aReturnedU = aDoubleJac.U();
  Handle(PLib_JacobiPolynomial) aReturnedV = aDoubleJac.V();
  Handle(TColStd_HArray1OfReal) aTabMaxU   = aDoubleJac.TabMaxU();
  Handle(TColStd_HArray1OfReal) aTabMaxV   = aDoubleJac.TabMaxV();

  // Multiple calls should return same handles
  EXPECT_EQ(aReturnedU.get(), aDoubleJac.U().get()) << "U accessor should be consistent";
  EXPECT_EQ(aReturnedV.get(), aDoubleJac.V().get()) << "V accessor should be consistent";
  EXPECT_EQ(aTabMaxU.get(), aDoubleJac.TabMaxU().get()) << "TabMaxU accessor should be consistent";
  EXPECT_EQ(aTabMaxV.get(), aDoubleJac.TabMaxV().get()) << "TabMaxV accessor should be consistent";

  // Verify TabMax arrays are properly sized and contain valid data
  if (!aTabMaxU.IsNull())
  {
    const TColStd_Array1OfReal& anArrU = aTabMaxU->Array1();
    for (Standard_Integer i = anArrU.Lower(); i <= anArrU.Upper(); i++)
    {
      EXPECT_GT(anArrU(i), 0.0) << "TabMaxU values should be positive";
      EXPECT_FALSE(Precision::IsInfinite(anArrU(i))) << "TabMaxU values should be finite";
    }
  }

  if (!aTabMaxV.IsNull())
  {
    const TColStd_Array1OfReal& anArrV = aTabMaxV->Array1();
    for (Standard_Integer i = anArrV.Lower(); i <= anArrV.Upper(); i++)
    {
      EXPECT_GT(anArrV(i), 0.0) << "TabMaxV values should be positive";
      EXPECT_FALSE(Precision::IsInfinite(anArrV(i))) << "TabMaxV values should be finite";
    }
  }
}

// Stress test with large degrees
TEST_F(PLibDoubleJacobiPolynomialTest, StressTest)
{
  Handle(PLib_JacobiPolynomial) aJacU_Large = new PLib_JacobiPolynomial(25, GeomAbs_C1);
  Handle(PLib_JacobiPolynomial) aJacV_Large = new PLib_JacobiPolynomial(20, GeomAbs_C2);

  EXPECT_NO_THROW({
    PLib_DoubleJacobiPolynomial aDoubleJac(aJacU_Large, aJacV_Large);

    // Test basic operations don't crash with large degrees
    const Standard_Integer aDimension  = 1;
    const Standard_Integer aDegreeU    = 15;
    const Standard_Integer aDegreeV    = 12;
    
    // Array must be sized based on WorkDegrees for proper method operation
    const Standard_Integer aWorkDegreeU = aJacU_Large->WorkDegree();
    const Standard_Integer aWorkDegreeV = aJacV_Large->WorkDegree();
    const Standard_Integer aCoeffCount = (aWorkDegreeU + 1) * (aWorkDegreeV + 1) * aDimension;
    const Standard_Integer aDJacCoeff  = 0; // For 0-based arrays

    TColStd_Array1OfReal aJacCoeff(0, aCoeffCount - 1);
    for (Standard_Integer i = aJacCoeff.Lower(); i <= aJacCoeff.Upper(); i++)
    {
      aJacCoeff(i) = Sin(i * 0.1);
    }

    Standard_Real aMaxErrU =
      aDoubleJac.MaxErrorU(aDimension, aDegreeU, aDegreeV, aDJacCoeff, aJacCoeff);
    Standard_Real aMaxErrV =
      aDoubleJac.MaxErrorV(aDimension, aDegreeU, aDegreeV, aDJacCoeff, aJacCoeff);
    Standard_Real aAvgErr =
      aDoubleJac.AverageError(aDimension, aDegreeU, aDegreeV, aDJacCoeff, aJacCoeff);

    EXPECT_GE(aMaxErrU, 0.0) << "MaxErrorU should be non-negative";
    EXPECT_GE(aMaxErrV, 0.0) << "MaxErrorV should be non-negative";
    EXPECT_GE(aAvgErr, 0.0) << "AverageError should be non-negative";
  }) << "Large degree operations should complete without crashing";
}
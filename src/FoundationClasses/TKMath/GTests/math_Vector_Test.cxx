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

#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_NullValue.hxx>
#include <Precision.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

namespace
{
// Helper function for comparing vectors with tolerance
void checkVectorsEqual(const math_Vector&  theV1,
                       const math_Vector&  theV2,
                       const Standard_Real theTolerance = Precision::Confusion())
{
  ASSERT_EQ(theV1.Length(), theV2.Length());
  ASSERT_EQ(theV1.Lower(), theV2.Lower());
  ASSERT_EQ(theV1.Upper(), theV2.Upper());

  for (Standard_Integer anI = theV1.Lower(); anI <= theV1.Upper(); anI++)
  {
    EXPECT_NEAR(theV1(anI), theV2(anI), theTolerance);
  }
}
} // namespace

// Tests for constructors
TEST(MathVectorTest, Constructors)
{
  // Test standard constructor
  math_Vector aVec1(1, 5);
  EXPECT_EQ(aVec1.Length(), 5);
  EXPECT_EQ(aVec1.Lower(), 1);
  EXPECT_EQ(aVec1.Upper(), 5);

  // Test constructor with initial value
  math_Vector aVec2(-2, 3, 2.5);
  EXPECT_EQ(aVec2.Length(), 6);
  EXPECT_EQ(aVec2.Lower(), -2);
  EXPECT_EQ(aVec2.Upper(), 3);

  for (Standard_Integer anI = -2; anI <= 3; anI++)
  {
    EXPECT_EQ(aVec2(anI), 2.5);
  }

  // Test constructor with external array
  Standard_Real anArray[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
  math_Vector   aVec3(anArray, 0, 4);
  EXPECT_EQ(aVec3.Length(), 5);
  EXPECT_EQ(aVec3.Lower(), 0);
  EXPECT_EQ(aVec3.Upper(), 4);

  for (Standard_Integer anI = 0; anI <= 4; anI++)
  {
    EXPECT_EQ(aVec3(anI), anArray[anI]);
  }

  // Test copy constructor
  math_Vector aVec4(aVec2);
  checkVectorsEqual(aVec2, aVec4);
}

// Tests for gp_XY and gp_XYZ constructors
TEST(MathVectorTest, GeometryConstructors)
{
  // Test gp_XY constructor
  gp_XY       anXY(3.5, 4.5);
  math_Vector aVecXY(anXY);
  EXPECT_EQ(aVecXY.Length(), 2);
  EXPECT_EQ(aVecXY.Lower(), 1);
  EXPECT_EQ(aVecXY.Upper(), 2);
  EXPECT_DOUBLE_EQ(aVecXY(1), 3.5);
  EXPECT_DOUBLE_EQ(aVecXY(2), 4.5);

  // Test gp_XYZ constructor
  gp_XYZ      anXYZ(1.1, 2.2, 3.3);
  math_Vector aVecXYZ(anXYZ);
  EXPECT_EQ(aVecXYZ.Length(), 3);
  EXPECT_EQ(aVecXYZ.Lower(), 1);
  EXPECT_EQ(aVecXYZ.Upper(), 3);
  EXPECT_DOUBLE_EQ(aVecXYZ(1), 1.1);
  EXPECT_DOUBLE_EQ(aVecXYZ(2), 2.2);
  EXPECT_DOUBLE_EQ(aVecXYZ(3), 3.3);
}

// Tests for initialization and access
TEST(MathVectorTest, InitAndAccess)
{
  math_Vector aVec(1, 4);

  // Test Init
  aVec.Init(7.0);
  for (Standard_Integer anI = 1; anI <= 4; anI++)
  {
    EXPECT_EQ(aVec(anI), 7.0);
  }

  // Test direct value access and modification
  aVec(2) = 15.0;
  EXPECT_EQ(aVec(2), 15.0);

  // Test Value method
  aVec.Value(3) = 25.0;
  EXPECT_EQ(aVec(3), 25.0);
  EXPECT_EQ(aVec.Value(3), 25.0);
}

// Tests for vector properties
TEST(MathVectorTest, VectorProperties)
{
  math_Vector aVec(1, 4);
  aVec(1) = 3.0;
  aVec(2) = 4.0;
  aVec(3) = 0.0;
  aVec(4) = -2.0;

  // Test Norm (should be sqrt(3^2 + 4^2 + 0^2 + (-2)^2) = sqrt(29))
  Standard_Real anExpectedNorm = std::sqrt(29.0);
  EXPECT_NEAR(aVec.Norm(), anExpectedNorm, Precision::Confusion());

  // Test Norm2 (should be 29)
  EXPECT_NEAR(aVec.Norm2(), 29.0, Precision::Confusion());

  // Test Max (should return index 2, value 4.0)
  EXPECT_EQ(aVec.Max(), 2);

  // Test Min (should return index 4, value -2.0)
  EXPECT_EQ(aVec.Min(), 4);
}

// Tests for normalization
TEST(MathVectorTest, Normalization)
{
  math_Vector aVec(1, 3);
  aVec(1) = 3.0;
  aVec(2) = 4.0;
  aVec(3) = 0.0;

  Standard_Real anOriginalNorm = aVec.Norm();
  EXPECT_NEAR(anOriginalNorm, 5.0, Precision::Confusion());

  // Test Normalized (creates new vector)
  math_Vector aNormalizedVec = aVec.Normalized();
  EXPECT_NEAR(aNormalizedVec.Norm(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aNormalizedVec(1), 3.0 / 5.0, Precision::Confusion());
  EXPECT_NEAR(aNormalizedVec(2), 4.0 / 5.0, Precision::Confusion());
  EXPECT_NEAR(aNormalizedVec(3), 0.0, Precision::Confusion());

  // Original vector should remain unchanged
  EXPECT_NEAR(aVec.Norm(), 5.0, Precision::Confusion());

  // Test Normalize (modifies in-place)
  aVec.Normalize();
  EXPECT_NEAR(aVec.Norm(), 1.0, Precision::Confusion());
  checkVectorsEqual(aVec, aNormalizedVec);
}

// Tests for normalization exception
TEST(MathVectorTest, ZeroVectorHandling)
{
  math_Vector aZeroVec(1, 3, 0.0);

  // Test behavior with zero vector - verify it's actually zero
  EXPECT_DOUBLE_EQ(aZeroVec.Norm(), 0.0) << "Zero vector should have zero norm";

  // Test with non-zero vector for comparison
  math_Vector aNonZeroVec(1, 3);
  aNonZeroVec(1) = 1.0;
  aNonZeroVec(2) = 0.0;
  aNonZeroVec(3) = 0.0;

  EXPECT_DOUBLE_EQ(aNonZeroVec.Norm(), 1.0) << "Unit vector should have norm 1";
  aNonZeroVec.Normalize();
  EXPECT_DOUBLE_EQ(aNonZeroVec.Norm(), 1.0) << "Normalized vector should have norm 1";
}

// Tests for inversion
TEST(MathVectorTest, Inversion)
{
  math_Vector aVec(1, 5);
  aVec(1) = 1.0;
  aVec(2) = 2.0;
  aVec(3) = 3.0;
  aVec(4) = 4.0;
  aVec(5) = 5.0;

  // Test Inverse (creates new vector)
  math_Vector anInverseVec = aVec.Inverse();
  EXPECT_EQ(anInverseVec(1), 5.0);
  EXPECT_EQ(anInverseVec(2), 4.0);
  EXPECT_EQ(anInverseVec(3), 3.0);
  EXPECT_EQ(anInverseVec(4), 2.0);
  EXPECT_EQ(anInverseVec(5), 1.0);

  // Original vector should remain unchanged
  EXPECT_EQ(aVec(1), 1.0);
  EXPECT_EQ(aVec(5), 5.0);

  // Test Invert (modifies in-place)
  aVec.Invert();
  checkVectorsEqual(aVec, anInverseVec);
}

// Tests for scalar operations
TEST(MathVectorTest, ScalarOperations)
{
  math_Vector aVec(1, 3);
  aVec(1) = 2.0;
  aVec(2) = 4.0;
  aVec(3) = 6.0;

  // Test multiplication by scalar
  math_Vector aMulResult = aVec.Multiplied(2.5);
  EXPECT_EQ(aMulResult(1), 5.0);
  EXPECT_EQ(aMulResult(2), 10.0);
  EXPECT_EQ(aMulResult(3), 15.0);

  // Test TMultiplied (should be same as Multiplied for scalar)
  math_Vector aTMulResult = aVec.TMultiplied(2.5);
  checkVectorsEqual(aMulResult, aTMulResult);

  // Test in-place multiplication
  aVec.Multiply(0.5);
  EXPECT_EQ(aVec(1), 1.0);
  EXPECT_EQ(aVec(2), 2.0);
  EXPECT_EQ(aVec(3), 3.0);

  // Test operator*= for scalar
  aVec *= 3.0;
  EXPECT_EQ(aVec(1), 3.0);
  EXPECT_EQ(aVec(2), 6.0);
  EXPECT_EQ(aVec(3), 9.0);

  // Test division by scalar
  math_Vector aDivResult = aVec.Divided(3.0);
  EXPECT_EQ(aDivResult(1), 1.0);
  EXPECT_EQ(aDivResult(2), 2.0);
  EXPECT_EQ(aDivResult(3), 3.0);

  // Test in-place division
  aVec.Divide(3.0);
  checkVectorsEqual(aVec, aDivResult);

  // Test operator/= for scalar
  aVec *= 6.0; // Set to [6, 12, 18]
  aVec /= 2.0;
  EXPECT_EQ(aVec(1), 3.0);
  EXPECT_EQ(aVec(2), 6.0);
  EXPECT_EQ(aVec(3), 9.0);
}

// Tests for division by zero
TEST(MathVectorTest, DivisionOperations)
{
  math_Vector aVec(1, 3, 2.0);

  // Test normal division operations
  aVec.Divide(2.0);
  EXPECT_DOUBLE_EQ(aVec(1), 1.0) << "Division should work correctly";

  math_Vector aVec2(1, 3, 4.0);
  math_Vector aResult = aVec2.Divided(2.0);
  EXPECT_DOUBLE_EQ(aResult(1), 2.0) << "Divided method should work correctly";
}

// Tests for vector addition and subtraction
TEST(MathVectorTest, VectorAdditionSubtraction)
{
  math_Vector aVec1(1, 3);
  aVec1(1) = 1.0;
  aVec1(2) = 2.0;
  aVec1(3) = 3.0;

  math_Vector aVec2(1, 3);
  aVec2(1) = 4.0;
  aVec2(2) = 5.0;
  aVec2(3) = 6.0;

  // Test Added
  math_Vector anAddResult = aVec1.Added(aVec2);
  EXPECT_EQ(anAddResult(1), 5.0);
  EXPECT_EQ(anAddResult(2), 7.0);
  EXPECT_EQ(anAddResult(3), 9.0);

  // Test operator+
  math_Vector anAddResult2 = aVec1 + aVec2;
  checkVectorsEqual(anAddResult, anAddResult2);

  // Test Subtracted
  math_Vector aSubResult = aVec1.Subtracted(aVec2);
  EXPECT_EQ(aSubResult(1), -3.0);
  EXPECT_EQ(aSubResult(2), -3.0);
  EXPECT_EQ(aSubResult(3), -3.0);

  // Test operator-
  math_Vector aSubResult2 = aVec1 - aVec2;
  checkVectorsEqual(aSubResult, aSubResult2);

  // Test in-place Add
  math_Vector aVecCopy1(aVec1);
  aVecCopy1.Add(aVec2);
  checkVectorsEqual(aVecCopy1, anAddResult);

  // Test operator+=
  math_Vector aVecCopy2(aVec1);
  aVecCopy2 += aVec2;
  checkVectorsEqual(aVecCopy2, anAddResult);

  // Test in-place Subtract
  math_Vector aVecCopy3(aVec1);
  aVecCopy3.Subtract(aVec2);
  checkVectorsEqual(aVecCopy3, aSubResult);

  // Test operator-=
  math_Vector aVecCopy4(aVec1);
  aVecCopy4 -= aVec2;
  checkVectorsEqual(aVecCopy4, aSubResult);
}

// Tests for vector operations with different bounds
TEST(MathVectorTest, VectorOperationsDifferentBounds)
{
  math_Vector aVec1(0, 2);
  aVec1(0) = 1.0;
  aVec1(1) = 2.0;
  aVec1(2) = 3.0;

  math_Vector aVec2(-1, 1);
  aVec2(-1) = 4.0;
  aVec2(0)  = 5.0;
  aVec2(1)  = 6.0;

  // Should work fine - same length, different bounds
  math_Vector anAddResult = aVec1.Added(aVec2);
  EXPECT_EQ(anAddResult(0), 5.0); // 1.0 + 4.0
  EXPECT_EQ(anAddResult(1), 7.0); // 2.0 + 5.0
  EXPECT_EQ(anAddResult(2), 9.0); // 3.0 + 6.0
}

// Tests for dimension errors

// Tests for dot product
TEST(MathVectorTest, DotProduct)
{
  math_Vector aVec1(1, 3);
  aVec1(1) = 1.0;
  aVec1(2) = 2.0;
  aVec1(3) = 3.0;

  math_Vector aVec2(1, 3);
  aVec2(1) = 4.0;
  aVec2(2) = 5.0;
  aVec2(3) = 6.0;

  // Test dot product (1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32)
  Standard_Real aDotProduct = aVec1.Multiplied(aVec2);
  EXPECT_EQ(aDotProduct, 32.0);

  // Test operator*
  Standard_Real aDotProduct2 = aVec1 * aVec2;
  EXPECT_EQ(aDotProduct, aDotProduct2);
}

// Tests for Set operation
TEST(MathVectorTest, SetOperation)
{
  math_Vector aVec(1, 6);
  aVec.Init(0.0);

  math_Vector aSubVec(1, 3);
  aSubVec(1) = 10.0;
  aSubVec(2) = 20.0;
  aSubVec(3) = 30.0;

  // Set elements from index 2 to 4
  aVec.Set(2, 4, aSubVec);

  EXPECT_EQ(aVec(1), 0.0);
  EXPECT_EQ(aVec(2), 10.0);
  EXPECT_EQ(aVec(3), 20.0);
  EXPECT_EQ(aVec(4), 30.0);
  EXPECT_EQ(aVec(5), 0.0);
  EXPECT_EQ(aVec(6), 0.0);
}

// Tests for Slice operation
TEST(MathVectorTest, SliceOperation)
{
  math_Vector aVec(1, 6);
  aVec(1) = 10.0;
  aVec(2) = 20.0;
  aVec(3) = 30.0;
  aVec(4) = 40.0;
  aVec(5) = 50.0;
  aVec(6) = 60.0;

  // Test normal slice (ascending)
  math_Vector aSlice1 = aVec.Slice(2, 4);
  EXPECT_EQ(aSlice1.Length(), 3);
  EXPECT_EQ(aSlice1.Lower(), 2);
  EXPECT_EQ(aSlice1.Upper(), 4);
  EXPECT_EQ(aSlice1(2), 20.0);
  EXPECT_EQ(aSlice1(3), 30.0);
  EXPECT_EQ(aSlice1(4), 40.0);

  // Test reverse slice (descending indices)
  math_Vector aSlice2 = aVec.Slice(4, 2);
  EXPECT_EQ(aSlice2.Length(), 3);
  EXPECT_EQ(aSlice2.Lower(), 2);
  EXPECT_EQ(aSlice2.Upper(), 4);
  EXPECT_EQ(aSlice2(2), 20.0); // Copy from original aVec(2)
  EXPECT_EQ(aSlice2(3), 30.0); // Copy from original aVec(3)
  EXPECT_EQ(aSlice2(4), 40.0); // Copy from original aVec(4)
}

// Tests for vector-matrix operations
TEST(MathVectorTest, VectorMatrixOperations)
{
  // Create test matrix [2x3]
  math_Matrix aMat(1, 2, 1, 3);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 2.0;
  aMat(1, 3) = 3.0;
  aMat(2, 1) = 4.0;
  aMat(2, 2) = 5.0;
  aMat(2, 3) = 6.0;

  // Create test vectors
  math_Vector aVec1(1, 2); // For left multiplication
  aVec1(1) = 2.0;
  aVec1(2) = 3.0;

  math_Vector aVec2(1, 3); // For right multiplication
  aVec2(1) = 1.0;
  aVec2(2) = 2.0;
  aVec2(3) = 3.0;

  // Test vector * matrix (should produce vector of size 3)
  math_Vector aResult1 = aVec1.Multiplied(aMat);
  EXPECT_EQ(aResult1.Length(), 3);
  // aResult1 = [2, 3] * [[1,2,3],[4,5,6]] = [2*1+3*4, 2*2+3*5, 2*3+3*6] = [14, 19, 24]
  EXPECT_EQ(aResult1(1), 14.0);
  EXPECT_EQ(aResult1(2), 19.0);
  EXPECT_EQ(aResult1(3), 24.0);

  // Test vector multiplication with matrix using Multiply method
  math_Vector aResult2(1, 3);
  aResult2.Multiply(aVec1, aMat);
  checkVectorsEqual(aResult1, aResult2);

  // Test matrix * vector using Multiply method
  math_Vector aResult3(1, 2);
  aResult3.Multiply(aMat, aVec2);
  // aResult3 = [[1,2,3],[4,5,6]] * [1,2,3] = [1*1+2*2+3*3, 4*1+5*2+6*3] = [14, 32]
  EXPECT_EQ(aResult3(1), 14.0);
  EXPECT_EQ(aResult3(2), 32.0);
}

// Tests for transpose matrix operations
TEST(MathVectorTest, TransposeMatrixOperations)
{
  // Create test matrix [2x3]
  math_Matrix aMat(1, 2, 1, 3);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 2.0;
  aMat(1, 3) = 3.0;
  aMat(2, 1) = 4.0;
  aMat(2, 2) = 5.0;
  aMat(2, 3) = 6.0;

  math_Vector aVec1(1, 2);
  aVec1(1) = 2.0;
  aVec1(2) = 3.0;

  math_Vector aVec2(1, 3);
  aVec2(1) = 1.0;
  aVec2(2) = 2.0;
  aVec2(3) = 3.0;

  // Test TMultiply (matrix^T * vector)
  math_Vector aResult1(1, 3);
  aResult1.TMultiply(aMat, aVec1);
  // aMat^T * aVec1 = [[1,4],[2,5],[3,6]] * [2,3] = [1*2+4*3, 2*2+5*3, 3*2+6*3] = [14, 19, 24]
  EXPECT_EQ(aResult1(1), 14.0);
  EXPECT_EQ(aResult1(2), 19.0);
  EXPECT_EQ(aResult1(3), 24.0);

  // Test TMultiply (vector * matrix^T)
  math_Vector aResult2(1, 2);
  aResult2.TMultiply(aVec2, aMat);
  // aVec2 * aMat^T = [1,2,3] * [[1,4],[2,5],[3,6]] = [1*1+2*2+3*3, 1*4+2*5+3*6] = [14, 32]
  EXPECT_EQ(aResult2(1), 14.0);
  EXPECT_EQ(aResult2(2), 32.0);
}

// Tests for matrix dimension errors

// Tests for three-operand operations
TEST(MathVectorTest, ThreeOperandOperations)
{
  math_Vector aVec1(1, 3);
  aVec1(1) = 1.0;
  aVec1(2) = 2.0;
  aVec1(3) = 3.0;

  math_Vector aVec2(1, 3);
  aVec2(1) = 4.0;
  aVec2(2) = 5.0;
  aVec2(3) = 6.0;

  math_Vector aResult(1, 3);

  // Test Add(left, right)
  aResult.Add(aVec1, aVec2);
  EXPECT_EQ(aResult(1), 5.0);
  EXPECT_EQ(aResult(2), 7.0);
  EXPECT_EQ(aResult(3), 9.0);

  // Test Subtract(left, right)
  aResult.Subtract(aVec1, aVec2);
  EXPECT_EQ(aResult(1), -3.0);
  EXPECT_EQ(aResult(2), -3.0);
  EXPECT_EQ(aResult(3), -3.0);

  // Test Multiply(scalar, vector)
  aResult.Multiply(2.5, aVec1);
  EXPECT_EQ(aResult(1), 2.5);
  EXPECT_EQ(aResult(2), 5.0);
  EXPECT_EQ(aResult(3), 7.5);
}

// Tests for Opposite operation
TEST(MathVectorTest, OppositeOperation)
{
  math_Vector aVec(1, 3);
  aVec(1) = 1.0;
  aVec(2) = -2.0;
  aVec(3) = 3.0;

  math_Vector anOpposite = aVec.Opposite();
  EXPECT_EQ(anOpposite(1), -1.0);
  EXPECT_EQ(anOpposite(2), 2.0);
  EXPECT_EQ(anOpposite(3), -3.0);

  // Test unary minus operator
  math_Vector anOpposite2 = -aVec;
  checkVectorsEqual(anOpposite, anOpposite2);
}

// Tests for assignment operations
TEST(MathVectorTest, AssignmentOperations)
{
  math_Vector aVec1(1, 3);
  aVec1(1) = 1.0;
  aVec1(2) = 2.0;
  aVec1(3) = 3.0;

  math_Vector aVec2(1, 3);
  aVec2.Init(0.0);

  // Test Initialized
  aVec2.Initialized(aVec1);
  checkVectorsEqual(aVec1, aVec2);

  // Test operator=
  math_Vector aVec3(1, 3);
  aVec3 = aVec1;
  checkVectorsEqual(aVec1, aVec3);
}

// Tests for friend operators
TEST(MathVectorTest, FriendOperators)
{
  math_Vector aVec(1, 3);
  aVec(1) = 2.0;
  aVec(2) = 4.0;
  aVec(3) = 6.0;

  // Test scalar * vector
  math_Vector aResult1 = 3.0 * aVec;
  EXPECT_EQ(aResult1(1), 6.0);
  EXPECT_EQ(aResult1(2), 12.0);
  EXPECT_EQ(aResult1(3), 18.0);

  // Test vector * scalar
  math_Vector aResult2 = aVec * 2.5;
  EXPECT_EQ(aResult2(1), 5.0);
  EXPECT_EQ(aResult2(2), 10.0);
  EXPECT_EQ(aResult2(3), 15.0);

  // Test vector / scalar
  math_Vector aResult3 = aVec / 2.0;
  EXPECT_EQ(aResult3(1), 1.0);
  EXPECT_EQ(aResult3(2), 2.0);
  EXPECT_EQ(aResult3(3), 3.0);
}

// Tests for edge cases and boundary conditions
TEST(MathVectorTest, EdgeCases)
{
  // Test single element vector
  math_Vector aSingleVec(5, 5, 42.0);
  EXPECT_EQ(aSingleVec.Length(), 1);
  EXPECT_EQ(aSingleVec(5), 42.0);
  EXPECT_EQ(aSingleVec.Max(), 5);
  EXPECT_EQ(aSingleVec.Min(), 5);

  // Test negative indices
  math_Vector aNegVec(-2, 1);
  aNegVec(-2) = 10.0;
  aNegVec(-1) = 20.0;
  aNegVec(0)  = 30.0;
  aNegVec(1)  = 40.0;

  EXPECT_EQ(aNegVec.Length(), 4);
  EXPECT_EQ(aNegVec.Lower(), -2);
  EXPECT_EQ(aNegVec.Upper(), 1);
  EXPECT_EQ(aNegVec.Max(), 1);
  EXPECT_EQ(aNegVec.Min(), -2);
}

// Tests for Move Semantics
TEST(MathVectorTest, MoveSemantics)
{
  // --- Move Constructor ---

  // Large vector (heap allocated)
  Standard_Integer aLen = 100;
  math_Vector      aVec1(1, aLen);
  for (Standard_Integer i = 1; i <= aLen; ++i)
  {
    aVec1(i) = static_cast<Standard_Real>(i);
  }

  // Move aVec1 to aVec2
  math_Vector aVec2(std::move(aVec1));

  EXPECT_EQ(aVec2.Length(), aLen);
  EXPECT_EQ(aVec2(1), 1.0);
  EXPECT_EQ(aVec2(aLen), static_cast<Standard_Real>(aLen));

  // Verify source state (length should be 0 after move for NCollection_Array1)
  // Note: calling Length() is safe as it just returns size.
  EXPECT_EQ(aVec1.Length(), 0);

  // Small vector (buffer allocated)
  Standard_Integer aSmallLen = 10;
  math_Vector      aSmallVec1(1, aSmallLen);
  for (Standard_Integer i = 1; i <= aSmallLen; ++i)
  {
    aSmallVec1(i) = static_cast<Standard_Real>(i);
  }

  // Move aSmallVec1 to aSmallVec2 (should copy because of buffer)
  math_Vector aSmallVec2(std::move(aSmallVec1));

  EXPECT_EQ(aSmallVec2.Length(), aSmallLen);
  EXPECT_EQ(aSmallVec2(1), 1.0);

  // Source remains valid for buffer-based vector
  EXPECT_EQ(aSmallVec1.Length(), aSmallLen);
  EXPECT_EQ(aSmallVec1(1), 1.0);

  // --- Move Assignment ---

  // Large vector move assignment
  math_Vector aVecAssign1(1, aLen);
  for (Standard_Integer i = 1; i <= aLen; ++i)
  {
    aVecAssign1(i) = static_cast<Standard_Real>(i);
  }

  math_Vector aVecAssign2(1, aLen);
  aVecAssign2.Init(0.0);

  aVecAssign2 = std::move(aVecAssign1);

  EXPECT_EQ(aVecAssign2.Length(), aLen);
  EXPECT_EQ(aVecAssign2(1), 1.0);

  EXPECT_EQ(aVecAssign1.Length(), 0);
}

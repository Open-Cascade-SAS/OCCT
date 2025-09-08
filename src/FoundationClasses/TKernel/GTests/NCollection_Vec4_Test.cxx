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

#include <NCollection_Vec4.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Mat4.hxx>

#include <gtest/gtest.h>
#include <cmath>

// Test fixture for NCollection_Vec4 tests
class NCollection_Vec4Test : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NCollection_Vec4Test, BasicConstruction)
{
  // Test default constructor
  NCollection_Vec4<float> aVec1;
  EXPECT_EQ(0.0f, aVec1.x());
  EXPECT_EQ(0.0f, aVec1.y());
  EXPECT_EQ(0.0f, aVec1.z());
  EXPECT_EQ(0.0f, aVec1.w());

  // Test parameterized constructor
  NCollection_Vec4<float> aVec2(1.0f, 2.0f, 3.0f, 4.0f);
  EXPECT_EQ(1.0f, aVec2.x());
  EXPECT_EQ(2.0f, aVec2.y());
  EXPECT_EQ(3.0f, aVec2.z());
  EXPECT_EQ(4.0f, aVec2.w());
}

TEST_F(NCollection_Vec4Test, XyzMethod)
{
  NCollection_Vec4<float> aVec(2.0f, 3.0f, 4.0f, 5.0f);
  NCollection_Vec3<float> aVec3 = aVec.xyz();

  EXPECT_EQ(2.0f, aVec3.x());
  EXPECT_EQ(3.0f, aVec3.y());
  EXPECT_EQ(4.0f, aVec3.z());
}

TEST_F(NCollection_Vec4Test, MatrixMultiplicationAndTransformation)
{
  // Test matrix multiplication and xyz() method with translation
  NCollection_Mat4<float> aMatrix;
  aMatrix.Translate(NCollection_Vec3<float>(4.0f, 3.0f, 1.0f));

  // Create a cube of points (-1,-1,-1) to (1,1,1)
  NCollection_Vec4<float> aPoints1[8];
  for (int aX = 0; aX < 2; ++aX)
  {
    for (int aY = 0; aY < 2; ++aY)
    {
      for (int aZ = 0; aZ < 2; ++aZ)
      {
        aPoints1[aX * 2 * 2 + aY * 2 + aZ] = NCollection_Vec4<float>(-1.0f + 2.0f * float(aX),
                                                                     -1.0f + 2.0f * float(aY),
                                                                     -1.0f + 2.0f * float(aZ),
                                                                     1.0f);
      }
    }
  }

  // Apply transformation and convert to Vec3
  NCollection_Vec3<float> aPoints2[8];
  for (int aPntIdx = 0; aPntIdx < 8; ++aPntIdx)
  {
    aPoints1[aPntIdx] = aMatrix * aPoints1[aPntIdx];
    aPoints2[aPntIdx] = aPoints1[aPntIdx].xyz() / aPoints1[aPntIdx].w();
  }

  // Check that the transformation worked correctly
  // The last point should be (1,1,1) transformed by translation (4,3,1) -> (5,4,2)
  // SquareModulus of (5,4,2) = 25 + 16 + 4 = 45
  float aSquareModulus = aPoints2[7].SquareModulus();
  EXPECT_NEAR(45.0f, aSquareModulus, 0.001f);

  // Check that all points were translated correctly
  // Point (0,0,0) -> translate by (4,3,1) -> (4,3,1)
  // Point index 0 corresponds to (-1,-1,-1) -> (3,2,0)
  float aExpectedSquareMod0 = 3.0f * 3.0f + 2.0f * 2.0f + 0.0f * 0.0f; // 9 + 4 + 0 = 13
  EXPECT_NEAR(aExpectedSquareMod0, aPoints2[0].SquareModulus(), 0.001f);
}

TEST_F(NCollection_Vec4Test, ComponentAccess)
{
  NCollection_Vec4<float> aVec(1.5f, 2.5f, 3.5f, 4.5f);

  // Test read access
  EXPECT_EQ(1.5f, aVec.x());
  EXPECT_EQ(2.5f, aVec.y());
  EXPECT_EQ(3.5f, aVec.z());
  EXPECT_EQ(4.5f, aVec.w());

  // Test write access
  aVec.x() = 10.0f;
  aVec.y() = 20.0f;
  aVec.z() = 30.0f;
  aVec.w() = 40.0f;

  EXPECT_EQ(10.0f, aVec.x());
  EXPECT_EQ(20.0f, aVec.y());
  EXPECT_EQ(30.0f, aVec.z());
  EXPECT_EQ(40.0f, aVec.w());
}

TEST_F(NCollection_Vec4Test, HomogeneousCoordinateDivision)
{
  // Test perspective division using w component
  NCollection_Vec4<float> aVec(8.0f, 12.0f, 16.0f, 4.0f);
  NCollection_Vec3<float> aResult = aVec.xyz() / aVec.w();

  EXPECT_EQ(2.0f, aResult.x()); // 8/4 = 2
  EXPECT_EQ(3.0f, aResult.y()); // 12/4 = 3
  EXPECT_EQ(4.0f, aResult.z()); // 16/4 = 4
}
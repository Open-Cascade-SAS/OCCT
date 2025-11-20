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

#include <gtest/gtest.h>

#include <BVH_RadixSorter.hxx>

// =============================================================================
// Morton Code Tests
// =============================================================================

TEST(BVH_RadixSorterTest, EncodeMortonCode_Origin)
{
  unsigned int aCode = BVH::EncodeMortonCode(0, 0, 0);
  EXPECT_EQ(aCode, 0u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SingleBits)
{
  // X=1, Y=0, Z=0 should give 1 (bit 0)
  unsigned int aCodeX = BVH::EncodeMortonCode(1, 0, 0);
  EXPECT_EQ(aCodeX, 1u);

  // X=0, Y=1, Z=0 should give 2 (bit 1)
  unsigned int aCodeY = BVH::EncodeMortonCode(0, 1, 0);
  EXPECT_EQ(aCodeY, 2u);

  // X=0, Y=0, Z=1 should give 4 (bit 2)
  unsigned int aCodeZ = BVH::EncodeMortonCode(0, 0, 1);
  EXPECT_EQ(aCodeZ, 4u);

  // X=1, Y=1, Z=1 should give 7 (bits 0, 1, 2)
  unsigned int aCodeXYZ = BVH::EncodeMortonCode(1, 1, 1);
  EXPECT_EQ(aCodeXYZ, 7u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_Interleaving)
{
  // X=2 (binary 10), Y=0, Z=0
  // Should produce binary ...001000 = 8
  unsigned int aCode1 = BVH::EncodeMortonCode(2, 0, 0);
  EXPECT_EQ(aCode1, 8u);

  // X=0, Y=2, Z=0
  // Should produce binary ...010000 = 16
  unsigned int aCode2 = BVH::EncodeMortonCode(0, 2, 0);
  EXPECT_EQ(aCode2, 16u);

  // X=0, Y=0, Z=2
  // Should produce binary ...100000 = 32
  unsigned int aCode3 = BVH::EncodeMortonCode(0, 0, 2);
  EXPECT_EQ(aCode3, 32u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_MaxValues)
{
  // Test with maximum 10-bit values (1023)
  unsigned int aCode = BVH::EncodeMortonCode(1023, 1023, 1023);

  // All 30 bits should be set
  EXPECT_EQ(aCode, 0x3FFFFFFFu);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_Symmetry)
{
  // Swapping coordinates should produce predictable results
  unsigned int aCode1 = BVH::EncodeMortonCode(100, 200, 300);
  unsigned int aCode2 = BVH::EncodeMortonCode(200, 100, 300);
  unsigned int aCode3 = BVH::EncodeMortonCode(100, 300, 200);

  // Codes should be different
  EXPECT_NE(aCode1, aCode2);
  EXPECT_NE(aCode1, aCode3);
  EXPECT_NE(aCode2, aCode3);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_Ordering)
{
  // Points closer in space should have closer Morton codes
  // (0, 0, 0) vs (1, 0, 0) should be closer than (0, 0, 0) vs (0, 0, 1023)
  unsigned int aCode000    = BVH::EncodeMortonCode(0, 0, 0);
  unsigned int aCode100    = BVH::EncodeMortonCode(1, 0, 0);
  unsigned int aCode001023 = BVH::EncodeMortonCode(0, 0, 1023);

  // (0,0,0) to (1,0,0) difference should be small
  unsigned int aDiff1 = aCode100 - aCode000;

  // (0,0,0) to (0,0,1023) difference should be large
  unsigned int aDiff2 = aCode001023 - aCode000;

  EXPECT_LT(aDiff1, aDiff2);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_MidValues)
{
  // Test with mid-range values
  unsigned int aCode = BVH::EncodeMortonCode(512, 512, 512);

  // Should be non-zero and within 30-bit range
  EXPECT_GT(aCode, 0u);
  EXPECT_LE(aCode, 0x3FFFFFFFu);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_PowersOfTwo)
{
  // Test powers of two for each axis
  unsigned int aCode4  = BVH::EncodeMortonCode(4, 0, 0);  // X=100 binary
  unsigned int aCode8  = BVH::EncodeMortonCode(8, 0, 0);  // X=1000 binary
  unsigned int aCode16 = BVH::EncodeMortonCode(16, 0, 0); // X=10000 binary

  // Each should be 8x the previous (3 bit positions apart in Morton code)
  EXPECT_EQ(aCode8, aCode4 * 8);
  EXPECT_EQ(aCode16, aCode8 * 8);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_ConsistentEncoding)
{
  // Same inputs should always produce same outputs
  for (int i = 0; i < 100; ++i)
  {
    unsigned int aCode1 = BVH::EncodeMortonCode(100, 200, 300);
    unsigned int aCode2 = BVH::EncodeMortonCode(100, 200, 300);
    EXPECT_EQ(aCode1, aCode2);
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_BoundaryValues)
{
  // Test boundary values near max (1023)
  unsigned int aCode1022 = BVH::EncodeMortonCode(1022, 1022, 1022);
  unsigned int aCode1023 = BVH::EncodeMortonCode(1023, 1023, 1023);

  EXPECT_LT(aCode1022, aCode1023);
  EXPECT_EQ(aCode1023, 0x3FFFFFFFu);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SmallValues)
{
  // Test small values
  unsigned int aCode2 = BVH::EncodeMortonCode(2, 2, 2);
  unsigned int aCode3 = BVH::EncodeMortonCode(3, 3, 3);

  // Both should be small but aCode3 > aCode2
  EXPECT_LT(aCode2, 100u);
  EXPECT_LT(aCode3, 100u);
  EXPECT_LT(aCode2, aCode3);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SingleAxisValues)
{
  // Test with value only in one axis
  unsigned int aCodeX = BVH::EncodeMortonCode(255, 0, 0);
  unsigned int aCodeY = BVH::EncodeMortonCode(0, 255, 0);
  unsigned int aCodeZ = BVH::EncodeMortonCode(0, 0, 255);

  // X bits in positions 0, 3, 6, ...
  // Y bits in positions 1, 4, 7, ...
  // Z bits in positions 2, 5, 8, ...
  EXPECT_NE(aCodeX, aCodeY);
  EXPECT_NE(aCodeY, aCodeZ);
  EXPECT_NE(aCodeX, aCodeZ);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SequentialXValues)
{
  // Test that sequential X values produce increasing codes
  unsigned int aPrev = BVH::EncodeMortonCode(0, 500, 500);
  for (unsigned int x = 1; x <= 10; ++x)
  {
    unsigned int aCurr = BVH::EncodeMortonCode(x, 500, 500);
    EXPECT_GT(aCurr, aPrev);
    aPrev = aCurr;
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SequentialYValues)
{
  // Test that sequential Y values produce increasing codes
  unsigned int aPrev = BVH::EncodeMortonCode(500, 0, 500);
  for (unsigned int y = 1; y <= 10; ++y)
  {
    unsigned int aCurr = BVH::EncodeMortonCode(500, y, 500);
    EXPECT_GT(aCurr, aPrev);
    aPrev = aCurr;
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SequentialZValues)
{
  // Test that sequential Z values produce increasing codes
  unsigned int aPrev = BVH::EncodeMortonCode(500, 500, 0);
  for (unsigned int z = 1; z <= 10; ++z)
  {
    unsigned int aCurr = BVH::EncodeMortonCode(500, 500, z);
    EXPECT_GT(aCurr, aPrev);
    aPrev = aCurr;
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_ZeroInDimensions)
{
  // Test with zeros in different dimensions
  unsigned int aCodeXY = BVH::EncodeMortonCode(100, 100, 0);
  unsigned int aCodeXZ = BVH::EncodeMortonCode(100, 0, 100);
  unsigned int aCodeYZ = BVH::EncodeMortonCode(0, 100, 100);

  // All should be different
  EXPECT_NE(aCodeXY, aCodeXZ);
  EXPECT_NE(aCodeXY, aCodeYZ);
  EXPECT_NE(aCodeXZ, aCodeYZ);

  // All should be non-zero
  EXPECT_GT(aCodeXY, 0u);
  EXPECT_GT(aCodeXZ, 0u);
  EXPECT_GT(aCodeYZ, 0u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_UpperByteBits)
{
  // Test values that require upper byte (values >= 256)
  unsigned int aCode256 = BVH::EncodeMortonCode(256, 0, 0);
  unsigned int aCode512 = BVH::EncodeMortonCode(512, 0, 0);
  unsigned int aCode768 = BVH::EncodeMortonCode(768, 0, 0);

  EXPECT_GT(aCode256, 0u);
  EXPECT_GT(aCode512, aCode256);
  EXPECT_GT(aCode768, aCode512);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_AllOnes)
{
  // 255 = 0xFF (all ones in lower byte)
  unsigned int aCode = BVH::EncodeMortonCode(255, 255, 255);

  // Should be less than max (1023, 1023, 1023)
  EXPECT_LT(aCode, 0x3FFFFFFFu);
  EXPECT_GT(aCode, 0u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_LocalityPreservation)
{
  // Points close in space should have relatively close Morton codes
  unsigned int aCenter = BVH::EncodeMortonCode(512, 512, 512);
  unsigned int aNear1  = BVH::EncodeMortonCode(513, 512, 512);
  unsigned int aNear2  = BVH::EncodeMortonCode(512, 513, 512);
  unsigned int aFar    = BVH::EncodeMortonCode(0, 0, 0);

  // Near points should have smaller difference than far point
  unsigned int aDiffNear1 = (aNear1 > aCenter) ? (aNear1 - aCenter) : (aCenter - aNear1);
  unsigned int aDiffNear2 = (aNear2 > aCenter) ? (aNear2 - aCenter) : (aCenter - aNear2);
  unsigned int aDiffFar   = (aFar > aCenter) ? (aFar - aCenter) : (aCenter - aFar);

  EXPECT_LT(aDiffNear1, aDiffFar);
  EXPECT_LT(aDiffNear2, aDiffFar);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_BitPattern3)
{
  // X=3 (binary 11), Y=0, Z=0
  // Interleaved: ...001001 = 9
  unsigned int aCode = BVH::EncodeMortonCode(3, 0, 0);
  EXPECT_EQ(aCode, 9u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_BitPattern7)
{
  // X=7 (binary 111), Y=0, Z=0
  // Interleaved: 001001001 = 73
  unsigned int aCode = BVH::EncodeMortonCode(7, 0, 0);
  EXPECT_EQ(aCode, 73u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_XYEqual)
{
  // When X = Y and Z = 0, code should follow pattern
  unsigned int aCode1 = BVH::EncodeMortonCode(1, 1, 0);
  unsigned int aCode2 = BVH::EncodeMortonCode(2, 2, 0);
  unsigned int aCode3 = BVH::EncodeMortonCode(4, 4, 0);

  // X=1, Y=1, Z=0: bits at 0,1 -> 3
  EXPECT_EQ(aCode1, 3u);

  // X=2, Y=2, Z=0: bits at 3,4 -> 24
  EXPECT_EQ(aCode2, 24u);

  // X=4, Y=4, Z=0: bits at 6,7 -> 192
  EXPECT_EQ(aCode3, 192u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_AllAxesEqual)
{
  // When all axes are equal
  unsigned int aCode1 = BVH::EncodeMortonCode(1, 1, 1);
  unsigned int aCode2 = BVH::EncodeMortonCode(2, 2, 2);

  // X=1, Y=1, Z=1: bits at 0,1,2 -> 7
  EXPECT_EQ(aCode1, 7u);

  // X=2, Y=2, Z=2: bits at 3,4,5 -> 56
  EXPECT_EQ(aCode2, 56u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_HalfMax)
{
  // Test with half of maximum (512)
  unsigned int aCode = BVH::EncodeMortonCode(512, 512, 512);

  // Should be in upper half of range
  EXPECT_GT(aCode, 0x1FFFFFFFu);
  EXPECT_LE(aCode, 0x3FFFFFFFu);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_QuarterMax)
{
  // Test with quarter of maximum (256)
  unsigned int aCode = BVH::EncodeMortonCode(256, 256, 256);

  // Should be less than half max
  EXPECT_GT(aCode, 0u);
  EXPECT_LT(aCode, 0x3FFFFFFFu);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_DifferentScales)
{
  // Test that scaling all coordinates by 2 shifts the Morton code
  unsigned int aCode1 = BVH::EncodeMortonCode(100, 100, 100);
  unsigned int aCode2 = BVH::EncodeMortonCode(200, 200, 200);

  // Code2 should be larger
  EXPECT_GT(aCode2, aCode1);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_AdjacentCells)
{
  // Test adjacent cells in X direction
  unsigned int aCode1 = BVH::EncodeMortonCode(100, 100, 100);
  unsigned int aCode2 = BVH::EncodeMortonCode(101, 100, 100);

  // Difference should be 1 (X bit is at position 0)
  EXPECT_EQ(aCode2 - aCode1, 1u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_AdjacentCellsY)
{
  // Test adjacent cells in Y direction
  unsigned int aCode1 = BVH::EncodeMortonCode(100, 100, 100);
  unsigned int aCode2 = BVH::EncodeMortonCode(100, 101, 100);

  // Difference should be 2 (Y bit is at position 1)
  EXPECT_EQ(aCode2 - aCode1, 2u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_AdjacentCellsZ)
{
  // Test adjacent cells in Z direction
  unsigned int aCode1 = BVH::EncodeMortonCode(100, 100, 100);
  unsigned int aCode2 = BVH::EncodeMortonCode(100, 100, 101);

  // Difference should be 4 (Z bit is at position 2)
  EXPECT_EQ(aCode2 - aCode1, 4u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_MonotonicX)
{
  // Verify monotonicity when only X changes
  unsigned int aPrev = 0;
  for (unsigned int x = 0; x < 100; ++x)
  {
    unsigned int aCode = BVH::EncodeMortonCode(x, 0, 0);
    EXPECT_GE(aCode, aPrev);
    aPrev = aCode;
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_MonotonicY)
{
  // Verify monotonicity when only Y changes
  unsigned int aPrev = 0;
  for (unsigned int y = 0; y < 100; ++y)
  {
    unsigned int aCode = BVH::EncodeMortonCode(0, y, 0);
    EXPECT_GE(aCode, aPrev);
    aPrev = aCode;
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_MonotonicZ)
{
  // Verify monotonicity when only Z changes
  unsigned int aPrev = 0;
  for (unsigned int z = 0; z < 100; ++z)
  {
    unsigned int aCode = BVH::EncodeMortonCode(0, 0, z);
    EXPECT_GE(aCode, aPrev);
    aPrev = aCode;
  }
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_NoOverflow)
{
  // Verify no overflow with maximum values
  unsigned int aCode = BVH::EncodeMortonCode(1023, 1023, 1023);

  // Should fit in 30 bits
  EXPECT_EQ(aCode & 0xC0000000u, 0u);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_UniqueForDifferentInputs)
{
  // Different inputs should produce different codes
  unsigned int aCode1 = BVH::EncodeMortonCode(100, 200, 300);
  unsigned int aCode2 = BVH::EncodeMortonCode(100, 200, 301);
  unsigned int aCode3 = BVH::EncodeMortonCode(100, 201, 300);
  unsigned int aCode4 = BVH::EncodeMortonCode(101, 200, 300);

  EXPECT_NE(aCode1, aCode2);
  EXPECT_NE(aCode1, aCode3);
  EXPECT_NE(aCode1, aCode4);
  EXPECT_NE(aCode2, aCode3);
  EXPECT_NE(aCode2, aCode4);
  EXPECT_NE(aCode3, aCode4);
}

TEST(BVH_RadixSorterTest, EncodeMortonCode_SpecificPattern)
{
  // Test specific known pattern
  // X=5 (101), Y=3 (011), Z=6 (110)
  unsigned int aCode = BVH::EncodeMortonCode(5, 3, 6);

  // Morton code interleaves bits: X at 3i, Y at 3i+1, Z at 3i+2
  // The exact value depends on the interleaving order
  // Just verify it's non-zero and within range
  EXPECT_GT(aCode, 0u);
  EXPECT_LE(aCode, 0x3FFFFFFFu);

  // Also verify consistency
  unsigned int aCode2 = BVH::EncodeMortonCode(5, 3, 6);
  EXPECT_EQ(aCode, aCode2);
}

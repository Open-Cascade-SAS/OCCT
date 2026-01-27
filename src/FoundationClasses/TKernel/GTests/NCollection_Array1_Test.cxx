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

#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <vector>

TEST(NCollection_Array1Test, DefaultConstructor)
{
  // Default constructor should not compile as it's explicitly deleted
  // NCollection_Array1<int> anArray;
  // Instead we need to use the parameterized constructor
  NCollection_Array1<int> anArray(1, 10);

  EXPECT_EQ(10, anArray.Length());
  EXPECT_EQ(1, anArray.Lower());
  EXPECT_EQ(10, anArray.Upper());
}

TEST(NCollection_Array1Test, ConstructorWithBounds)
{
  // Test constructor with explicit bounds
  NCollection_Array1<int> anArray(1, 5);
  EXPECT_EQ(5, anArray.Size());
  EXPECT_EQ(1, anArray.Lower());
  EXPECT_EQ(5, anArray.Upper());
}

TEST(NCollection_Array1Test, ConstructorWithNegativeBounds)
{
  // Test constructor with negative bounds
  NCollection_Array1<int> anArray(-3, 2);
  EXPECT_EQ(6, anArray.Size());
  EXPECT_EQ(-3, anArray.Lower());
  EXPECT_EQ(2, anArray.Upper());
}

TEST(NCollection_Array1Test, AssignmentValue)
{
  NCollection_Array1<int> anArray(1, 5);

  // Initialize array
  for (int i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    anArray(i) = i * 10;
  }

  // Verify values
  for (int i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    EXPECT_EQ(i * 10, anArray(i));
  }

  // Test Value vs operator()
  for (int i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    EXPECT_EQ(anArray.Value(i), anArray(i));
  }
}

TEST(NCollection_Array1Test, CopyConstructor)
{
  NCollection_Array1<int> anArray1(1, 5);

  // Initialize array
  for (int i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    anArray1(i) = i * 10;
  }

  // Copy construct
  NCollection_Array1<int> anArray2(anArray1);

  // Verify copy
  EXPECT_EQ(anArray1.Length(), anArray2.Length());
  EXPECT_EQ(anArray1.Lower(), anArray2.Lower());
  EXPECT_EQ(anArray1.Upper(), anArray2.Upper());

  for (int i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    EXPECT_EQ(anArray1(i), anArray2(i));
  }

  // Modify original to ensure deep copy
  anArray1(3) = 999;
  EXPECT_NE(anArray1(3), anArray2(3));
}

TEST(NCollection_Array1Test, ValueAccess)
{
  NCollection_Array1<int> anArray(1, 5);

  // Set values
  for (int i = 1; i <= 5; i++)
  {
    anArray.SetValue(i, i * 10);
  }

  // Check values
  for (int i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray.Value(i));
    EXPECT_EQ(i * 10, anArray(i)); // Using operator()
  }
}

TEST(NCollection_Array1Test, ChangeValueAccess)
{
  NCollection_Array1<int> anArray(1, 5);

  // Set values using ChangeValue
  for (int i = 1; i <= 5; i++)
  {
    anArray.ChangeValue(i) = i * 10;
  }

  // Check values
  for (int i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray(i));
  }

  // Modify values through references
  for (int i = 1; i <= 5; i++)
  {
    anArray.ChangeValue(i) += 5;
  }

  // Check modified values
  for (int i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10 + 5, anArray(i));
  }
}

TEST(NCollection_Array1Test, AssignmentOperator)
{
  NCollection_Array1<int> anArray1(1, 5);

  // Initialize array
  for (int i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    anArray1(i) = i * 10;
  }

  // Test assignment
  NCollection_Array1<int> anArray2(11, 15);
  anArray2 = anArray1;
  anArray2.Resize(1, 5, true); // Resize to match anArray1

  // Verify assignment result
  EXPECT_EQ(anArray1.Length(), anArray2.Length());
  EXPECT_EQ(anArray1.Lower(), anArray2.Lower());
  EXPECT_EQ(anArray1.Upper(), anArray2.Upper());

  for (int i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    EXPECT_EQ(anArray1(i), anArray2(i));
  }
}

TEST(NCollection_Array1Test, Move)
{
  NCollection_Array1<int> anArray1(1, 5);

  // Initialize array
  for (int i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    anArray1(i) = i * 10;
  }

  // Test Move method
  NCollection_Array1<int> anArray2(11, 15);
  anArray2.Move(anArray1);
  anArray2.Resize(1, 5, true); // Resize to match anArray1

  // Verify move result
  EXPECT_EQ(5, anArray2.Length());
  EXPECT_EQ(1, anArray2.Lower());
  EXPECT_EQ(5, anArray2.Upper());

  // Original array is not keep referecing the same data
  EXPECT_EQ(anArray1.Length(), 0);
  EXPECT_EQ(anArray1.Lower(), 1);
}

TEST(NCollection_Array1Test, Init)
{
  NCollection_Array1<int> anArray(1, 5);

  // Test Init method
  anArray.Init(42);

  // Verify all values initialized
  for (int i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    EXPECT_EQ(42, anArray(i));
  }
}

TEST(NCollection_Array1Test, SetValue)
{
  NCollection_Array1<int> anArray(1, 5);

  // Test SetValue method
  anArray.SetValue(3, 123);

  // Verify value set
  EXPECT_EQ(123, anArray(3));
}

TEST(NCollection_Array1Test, FirstLast)
{
  NCollection_Array1<int> anArray(5, 10);

  anArray(5)  = 55;
  anArray(10) = 1010;

  // Test First and Last methods
  EXPECT_EQ(55, anArray.First());
  EXPECT_EQ(1010, anArray.Last());

  // Change First and Last
  anArray.ChangeFirst() = 555;
  anArray.ChangeLast()  = 10101;

  EXPECT_EQ(555, anArray.First());
  EXPECT_EQ(10101, anArray.Last());
}

TEST(NCollection_Array1Test, STLIteration)
{
  NCollection_Array1<int> anArray(1, 5);
  for (int i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    anArray(i) = i * 10;
  }

  // Test STL-style iteration
  int index = 1;
  for (const auto& val : anArray)
  {
    EXPECT_EQ(index * 10, val);
    index++;
  }
}

TEST(NCollection_Array1Test, Resize)
{
  NCollection_Array1<int> anArray(1, 5);
  for (int i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    anArray(i) = i * 10;
  }

  // Test Resize method - increasing size
  anArray.Resize(1, 10, true);

  // Check new size
  EXPECT_EQ(10, anArray.Length());
  EXPECT_EQ(1, anArray.Lower());
  EXPECT_EQ(10, anArray.Upper());

  // Verify original data preserved
  for (int i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray(i));
  }

  // Test Resize method - decreasing size
  NCollection_Array1<int> anArray2(1, 10);
  for (int i = anArray2.Lower(); i <= anArray2.Upper(); i++)
  {
    anArray2(i) = i * 10;
  }

  anArray2.Resize(1, 5, true);

  // Check new size
  EXPECT_EQ(5, anArray2.Length());
  EXPECT_EQ(1, anArray2.Lower());
  EXPECT_EQ(5, anArray2.Upper());

  // Verify original data preserved
  for (int i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray2(i));
  }
}

TEST(NCollection_Array1Test, ChangeValue)
{
  NCollection_Array1<int> anArray(1, 5);
  anArray.Init(42);

  // Test ChangeValue method
  anArray.ChangeValue(3) = 123;

  // Verify value changed
  EXPECT_EQ(123, anArray(3));

  // Verify other values unchanged
  EXPECT_EQ(42, anArray(1));
  EXPECT_EQ(42, anArray(2));
  EXPECT_EQ(42, anArray(4));
  EXPECT_EQ(42, anArray(5));
}

TEST(NCollection_Array1Test, IteratorAccess)
{
  NCollection_Array1<int> anArray(1, 5);
  for (int i = 1; i <= 5; i++)
  {
    anArray(i) = i * 10;
  }

  // Test iteration using STL-compatible iterators
  int index = 1;
  for (auto it = anArray.begin(); it != anArray.end(); ++it, ++index)
  {
    EXPECT_EQ(index * 10, *it);
  }

  // Test range-based for loop
  index = 1;
  for (const auto& value : anArray)
  {
    EXPECT_EQ(index * 10, value);
    index++;
  }
}

TEST(NCollection_Array1Test, STLAlgorithmCompatibility_MinMax)
{
  const int               size = 100;
  NCollection_Array1<int> anArray(1, size);
  std::vector<int>        aVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 1; anIdx <= size; ++anIdx)
  {
    int aVal       = aDistribution(aGenerator);
    anArray(anIdx) = aVal;
    aVector.push_back(aVal);
  }

  auto aMinOCCT = std::min_element(anArray.begin(), anArray.end());
  auto aMinStd  = std::min_element(aVector.begin(), aVector.end());

  auto aMaxOCCT = std::max_element(anArray.begin(), anArray.end());
  auto aMaxStd  = std::max_element(aVector.begin(), aVector.end());

  EXPECT_EQ(*aMinOCCT, *aMinStd);
  EXPECT_EQ(*aMaxOCCT, *aMaxStd);
}

TEST(NCollection_Array1Test, STLAlgorithmCompatibility_Replace)
{
  const int               size = 100;
  NCollection_Array1<int> anArray(1, size);
  std::vector<int>        aVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 1; anIdx <= size; ++anIdx)
  {
    int aVal       = aDistribution(aGenerator);
    anArray(anIdx) = aVal;
    aVector.push_back(aVal);
  }

  int aTargetValue = aVector.back();
  int aNewValue    = -1;

  std::replace(anArray.begin(), anArray.end(), aTargetValue, aNewValue);
  std::replace(aVector.begin(), aVector.end(), aTargetValue, aNewValue);

  EXPECT_TRUE(std::equal(anArray.begin(), anArray.end(), aVector.begin()));
}

TEST(NCollection_Array1Test, STLAlgorithmCompatibility_Sort)
{
  const int               size = 100;
  NCollection_Array1<int> anArray(1, size);
  std::vector<int>        aVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 1; anIdx <= size; ++anIdx)
  {
    int aVal       = aDistribution(aGenerator);
    anArray(anIdx) = aVal;
    aVector.push_back(aVal);
  }

  std::sort(anArray.begin(), anArray.end());
  std::sort(aVector.begin(), aVector.end());

  EXPECT_TRUE(std::equal(anArray.begin(), anArray.end(), aVector.begin()));
}

// Helper struct for testing in-place construction with multiple arguments
struct Array1MultiArgType
{
  int    myA;
  double myB;
  Array1MultiArgType()
      : myA(0),
        myB(0.0)
  {
  }
  Array1MultiArgType(int theA, double theB)
      : myA(theA),
        myB(theB)
  {
  }
};

TEST(NCollection_Array1Test, EmplaceValue)
{
  NCollection_Array1<Array1MultiArgType> anArray(1, 5);

  // Test EmplaceValue with multiple constructor arguments
  Array1MultiArgType& aRef1 = anArray.EmplaceValue(1, 42, 3.14);
  EXPECT_EQ(42, aRef1.myA);
  EXPECT_NEAR(3.14, aRef1.myB, 1e-10);

  Array1MultiArgType& aRef2 = anArray.EmplaceValue(3, 100, 2.71);
  EXPECT_EQ(100, aRef2.myA);
  EXPECT_NEAR(2.71, aRef2.myB, 1e-10);

  // Verify the values are in the array
  EXPECT_EQ(42, anArray(1).myA);
  EXPECT_EQ(100, anArray(3).myA);

  // Verify other elements are default-constructed
  EXPECT_EQ(0, anArray(2).myA);
  EXPECT_EQ(0, anArray(4).myA);
  EXPECT_EQ(0, anArray(5).myA);
}

TEST(NCollection_Array1Test, EmplaceValue_ReplacesExisting)
{
  NCollection_Array1<Array1MultiArgType> anArray(1, 3);

  // Set initial values
  anArray.EmplaceValue(1, 10, 1.0);
  anArray.EmplaceValue(2, 20, 2.0);
  anArray.EmplaceValue(3, 30, 3.0);

  // Replace value at index 2
  Array1MultiArgType& aRef = anArray.EmplaceValue(2, 200, 20.0);
  EXPECT_EQ(200, aRef.myA);
  EXPECT_NEAR(20.0, aRef.myB, 1e-10);

  // Verify other values unchanged
  EXPECT_EQ(10, anArray(1).myA);
  EXPECT_EQ(200, anArray(2).myA);
  EXPECT_EQ(30, anArray(3).myA);
}

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

#include <NCollection_OccAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Assert.hxx>

#include <gtest/gtest.h>
#include <list>
#include <vector>

// Test fixture for NCollection_OccAllocator tests
class NCollection_OccAllocatorTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NCollection_OccAllocatorTest, AllocatorTypeTraits)
{
  // Test type definitions and static assertions (compile-time checks)
  typedef Handle(Standard_Transient)           anElemType;
  typedef NCollection_OccAllocator<anElemType> anAllocatorType;

  // These would fail to compile if the types were wrong
  EXPECT_EQ(sizeof(anAllocatorType::value_type), sizeof(anElemType));
  EXPECT_EQ(sizeof(anAllocatorType::pointer), sizeof(void*));
  EXPECT_EQ(sizeof(anAllocatorType::const_pointer), sizeof(void*));
  EXPECT_EQ(sizeof(anAllocatorType::size_type), sizeof(size_t));
  EXPECT_EQ(sizeof(anAllocatorType::difference_type), sizeof(ptrdiff_t));

  // Test reference types
  anElemType                       aDummy;
  anAllocatorType::reference       aRef      = aDummy;
  anAllocatorType::const_reference aConstRef = aDummy;

  // Basic checks that references work
  EXPECT_EQ(&aRef, &aDummy);
  EXPECT_EQ(&aConstRef, &aDummy);

  // Test rebind functionality
  typedef int                                             anOtherElemType;
  typedef anAllocatorType::rebind<anOtherElemType>::other anOtherAllocatorType;
  EXPECT_EQ(sizeof(anOtherAllocatorType::value_type), sizeof(anOtherElemType));
}

TEST_F(NCollection_OccAllocatorTest, STLContainerIntegration)
{
  // Create incremental allocator outside the scope of objects it will manage
  Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();

  {
    // Test with std::list using typed allocator
    NCollection_OccAllocator<int>                 anSAlloc(anIncAlloc);
    std::list<int, NCollection_OccAllocator<int>> aList(anSAlloc);

    aList.push_back(2);
    EXPECT_EQ(1, aList.size());
    EXPECT_EQ(2, aList.front());

    // Test with std::vector using type cast
    NCollection_OccAllocator<char>                  aCAlloc;
    std::vector<int, NCollection_OccAllocator<int>> aVector(aCAlloc);

    aVector.push_back(1);
    EXPECT_EQ(1, aVector.size());
    EXPECT_EQ(1, aVector[0]);

    // Test using void-specialization allocator
    NCollection_OccAllocator<void*>                 aVAlloc;
    std::vector<int, NCollection_OccAllocator<int>> aVector2(aVAlloc);

    aVector2.resize(10);
    aVector2.push_back(-1);
    EXPECT_EQ(11, aVector2.size());
    EXPECT_EQ(-1, aVector2.back());

    // Test equality of allocators
    EXPECT_NE(anSAlloc, aCAlloc); // Different underlying allocators

    NCollection_OccAllocator<int> anIAlloc(anIncAlloc);
    EXPECT_EQ(anSAlloc, anIAlloc); // Same underlying allocator
  }
}

TEST_F(NCollection_OccAllocatorTest, DefaultAllocator)
{
  // Test default allocator behavior
  NCollection_OccAllocator<int>                   aDefaultAlloc;
  std::vector<int, NCollection_OccAllocator<int>> aVector(aDefaultAlloc);

  // Fill with some data
  for (int i = 0; i < 100; ++i)
  {
    aVector.push_back(i);
  }

  EXPECT_EQ(100, aVector.size());

  // Verify data integrity
  for (size_t i = 0; i < aVector.size(); ++i)
  {
    EXPECT_EQ(static_cast<int>(i), aVector[i]);
  }
}

TEST_F(NCollection_OccAllocatorTest, AllocatorComparison)
{
  Handle(NCollection_IncAllocator) anIncAlloc1 = new NCollection_IncAllocator();
  Handle(NCollection_IncAllocator) anIncAlloc2 = new NCollection_IncAllocator();

  NCollection_OccAllocator<int> aAlloc1(anIncAlloc1);
  NCollection_OccAllocator<int> aAlloc2(anIncAlloc1); // Same underlying allocator
  NCollection_OccAllocator<int> aAlloc3(anIncAlloc2); // Different underlying allocator
  NCollection_OccAllocator<int> aAlloc4;              // Default allocator

  // Test equality
  EXPECT_EQ(aAlloc1, aAlloc2); // Same underlying allocator
  EXPECT_NE(aAlloc1, aAlloc3); // Different underlying allocator
  EXPECT_NE(aAlloc1, aAlloc4); // One uses IncAllocator, one uses default

  // Test inequality
  EXPECT_FALSE(aAlloc1 != aAlloc2); // Should be equal
  EXPECT_TRUE(aAlloc1 != aAlloc3);  // Should be different
}

TEST_F(NCollection_OccAllocatorTest, CrossTypeCompatibility)
{
  Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();

  // Test allocators for different types but same underlying allocator
  NCollection_OccAllocator<int>    anIntAlloc(anIncAlloc);
  NCollection_OccAllocator<double> aDoubleAlloc(anIncAlloc);
  NCollection_OccAllocator<char>   aCharAlloc(anIncAlloc);

  // They should be considered equal despite different value types
  // (This tests the allocator's rebind and comparison logic)
  EXPECT_EQ(anIntAlloc, NCollection_OccAllocator<int>(anIncAlloc));

  // Create containers with different allocators pointing to same IncAllocator
  std::vector<int, NCollection_OccAllocator<int>>       anIntVector(anIntAlloc);
  std::vector<double, NCollection_OccAllocator<double>> aDoubleVector(aDoubleAlloc);

  anIntVector.push_back(42);
  aDoubleVector.push_back(3.14);

  EXPECT_EQ(1, anIntVector.size());
  EXPECT_EQ(1, aDoubleVector.size());
  EXPECT_EQ(42, anIntVector[0]);
  EXPECT_DOUBLE_EQ(3.14, aDoubleVector[0]);
}
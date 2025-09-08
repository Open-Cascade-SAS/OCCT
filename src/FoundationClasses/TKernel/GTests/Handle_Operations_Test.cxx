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

#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_HeapAllocator.hxx>

#include <gtest/gtest.h>

// Test classes for Handle operations
class TestBase : public Standard_Transient
{
public:
  TestBase() {}

  virtual ~TestBase() {}
  DEFINE_STANDARD_RTTI_INLINE(TestBase, Standard_Transient)
};
DEFINE_STANDARD_HANDLE(TestBase, Standard_Transient)

class TestDerived : public TestBase
{
public:
  TestDerived() {}

  virtual ~TestDerived() {}
  DEFINE_STANDARD_RTTI_INLINE(TestDerived, TestBase)
};
DEFINE_STANDARD_HANDLE(TestDerived, TestBase)

class TestOther : public Standard_Transient
{
public:
  TestOther() {}

  virtual ~TestOther() {}
  DEFINE_STANDARD_RTTI_INLINE(TestOther, Standard_Transient)
};
DEFINE_STANDARD_HANDLE(TestOther, Standard_Transient)

// Test fixture for Handle operations tests
class HandleOperationsTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(HandleOperationsTest, BasicHandleOperations)
{
  // Test basic handle creation and null checking
  Handle(TestDerived) aDerived = new TestDerived();
  EXPECT_FALSE(aDerived.IsNull());

  // Test various casting operations
  const Handle(TestDerived)& aConstDerived = aDerived;
  const Handle(TestBase)&    aConstBase    = aDerived;
  (void)aConstBase; // Avoid unused variable warning

  TestDerived*       aDerivedPtr      = aDerived.get();
  const TestDerived* aConstDerivedPtr = aDerived.get();
  EXPECT_NE(nullptr, aDerivedPtr);
  EXPECT_NE(nullptr, aConstDerivedPtr);
  EXPECT_EQ(aDerivedPtr, aConstDerivedPtr);

  // Test reference access
  TestDerived&       aDerivedRef      = *aDerived;
  const TestDerived& aConstDerivedRef = *aConstDerived;
  EXPECT_EQ(&aDerivedRef, aDerivedPtr);
  EXPECT_EQ(&aConstDerivedRef, aConstDerivedPtr);

  // Test handle assignment to base type
  Handle(TestBase) aBase = aDerived;
  EXPECT_FALSE(aBase.IsNull());
  EXPECT_EQ(aBase.get(), aDerived.get());
}

TEST_F(HandleOperationsTest, HandleDownCast)
{
  // Test DownCast functionality
  Handle(TestDerived) aDerived = new TestDerived();
  Handle(TestBase)    aBase    = aDerived;

  Handle(TestDerived) aDownCastDerived = Handle(TestDerived)::DownCast(aBase);
  EXPECT_FALSE(aDownCastDerived.IsNull());
  EXPECT_EQ(aDownCastDerived.get(), aDerived.get());

  // Test failed downcast (different type)
  Handle(TestOther)          anOther         = new TestOther();
  Handle(Standard_Transient) aTransient      = anOther;
  Handle(TestDerived)        aFailedDownCast = Handle(TestDerived)::DownCast(aTransient);
  EXPECT_TRUE(aFailedDownCast.IsNull());
}

TEST_F(HandleOperationsTest, HandleComparisons)
{
  Handle(TestDerived) aDerived  = new TestDerived();
  Handle(TestDerived) aDerived2 = aDerived;          // Same object
  Handle(TestDerived) aDerived3 = new TestDerived(); // Different object
  Handle(TestBase)    aBase     = aDerived;

  // Test equality operators
  EXPECT_TRUE(aDerived == aDerived);   // Self equality
  EXPECT_TRUE(aDerived == aDerived2);  // Same object through different handles
  EXPECT_TRUE(aDerived == aBase);      // Handle to base type
  EXPECT_FALSE(aDerived == aDerived3); // Different objects

  // Test with pointers
  TestDerived*       aDerivedPtr      = aDerived.get();
  const TestDerived* aConstDerivedPtr = aDerived.get();
  EXPECT_TRUE(aDerived == aDerivedPtr);
  EXPECT_TRUE(aDerivedPtr == aDerived);
  EXPECT_TRUE(aDerived == aConstDerivedPtr);
  EXPECT_TRUE(aConstDerivedPtr == aDerived);

  // Test inequality
  EXPECT_FALSE(aDerived != aDerived2); // Should be equal
  EXPECT_TRUE(aDerived != aDerived3);  // Should be different

  // Test boolean conversion
  EXPECT_TRUE(static_cast<bool>(aDerived));
  Handle(TestDerived) aNullDerived;
  EXPECT_FALSE(static_cast<bool>(aNullDerived));
}

TEST_F(HandleOperationsTest, AllocatorHandleCasting)
{
  // Test Handle casting with allocator hierarchy
  Handle(NCollection_BaseAllocator) aBasePtr = new NCollection_IncAllocator();
  EXPECT_FALSE(aBasePtr.IsNull());

  // Test successful downcast to IncAllocator
  Handle(NCollection_IncAllocator) anIncAlloc =
    Handle(NCollection_IncAllocator)::DownCast(aBasePtr);
  EXPECT_FALSE(anIncAlloc.IsNull());

  // Test upcast back to BaseAllocator
  Handle(NCollection_BaseAllocator) aBaseAlloc = anIncAlloc;
  EXPECT_FALSE(aBaseAlloc.IsNull());
  EXPECT_EQ(aBaseAlloc.get(), aBasePtr.get());

  // Test failed downcast to HeapAllocator
  Handle(NCollection_HeapAllocator) aHeapAlloc =
    Handle(NCollection_HeapAllocator)::DownCast(aBasePtr);
  EXPECT_TRUE(aHeapAlloc.IsNull());
}

TEST_F(HandleOperationsTest, HandleCopyAndAssignment)
{
  Handle(TestDerived) aDerived1 = new TestDerived();

  // Test copy constructor
  Handle(TestDerived) aDerived2(aDerived1);
  EXPECT_FALSE(aDerived2.IsNull());
  EXPECT_EQ(aDerived1.get(), aDerived2.get());

  // Test assignment operator
  Handle(TestDerived) aDerived3;
  EXPECT_TRUE(aDerived3.IsNull());

  aDerived3 = aDerived1;
  EXPECT_FALSE(aDerived3.IsNull());
  EXPECT_EQ(aDerived1.get(), aDerived3.get());

  // Test self-assignment (suppress warning)
#include <Standard_WarningsDisable.hxx>
  aDerived1 = aDerived1;
#include <Standard_WarningsRestore.hxx>
  EXPECT_FALSE(aDerived1.IsNull());

  // Test assignment to null
  aDerived3 = Handle(TestDerived)();
  EXPECT_TRUE(aDerived3.IsNull());
}

TEST_F(HandleOperationsTest, HandleWithDifferentTypes)
{
  Handle(TestDerived) aDerived = new TestDerived();
  Handle(TestOther)   anOther  = new TestOther();

  // Note: Direct comparison between handles of different types is not allowed by the type system
  // This is actually a good thing as it prevents type errors at compile time

  // Test casting both to common base type
  Handle(Standard_Transient) aDerivedTransient = aDerived;
  Handle(Standard_Transient) anOtherTransient  = anOther;

  // These should be different objects even when cast to base type
  EXPECT_FALSE(aDerivedTransient == anOtherTransient);
  EXPECT_TRUE(aDerivedTransient != anOtherTransient);

  // But each should equal itself when cast
  EXPECT_TRUE(aDerivedTransient == aDerived);
  EXPECT_TRUE(anOtherTransient == anOther);

  // Test that each maintains its identity through base type casting
  EXPECT_EQ(aDerivedTransient.get(), aDerived.get());
  EXPECT_EQ(anOtherTransient.get(), anOther.get());
}

TEST_F(HandleOperationsTest, HandleNullOperations)
{
  Handle(TestDerived) aNullHandle;
  Handle(TestDerived) anotherNullHandle;
  Handle(TestDerived) aValidHandle = new TestDerived();

  // Test null handle properties
  EXPECT_TRUE(aNullHandle.IsNull());
  EXPECT_EQ(nullptr, aNullHandle.get());

  // Test null handle comparisons
  EXPECT_TRUE(aNullHandle == anotherNullHandle); // Both null
  EXPECT_FALSE(aNullHandle == aValidHandle);     // Null vs valid
  EXPECT_FALSE(aValidHandle == aNullHandle);     // Valid vs null

  EXPECT_FALSE(aNullHandle != anotherNullHandle); // Both null
  EXPECT_TRUE(aNullHandle != aValidHandle);       // Null vs valid
  EXPECT_TRUE(aValidHandle != aNullHandle);       // Valid vs null

  // Test boolean conversion
  EXPECT_FALSE(static_cast<bool>(aNullHandle));
  EXPECT_TRUE(static_cast<bool>(aValidHandle));
}
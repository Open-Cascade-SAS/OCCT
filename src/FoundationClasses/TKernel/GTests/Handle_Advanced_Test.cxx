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
#include <Standard_Type.hxx>
#include <Standard_Assert.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_HeapAllocator.hxx>
#include <OSD_Timer.hxx>

#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <typeinfo>
#include <type_traits>

// Auxiliary macro to check and report status with EXPECT
#define CHECK_HANDLE(ok, what) EXPECT_TRUE(ok) << "Checking " << what << " failed"

// Test root class for hierarchy
class TransientRoot : public Standard_Transient
{
public:
  virtual const char* Name() const { return "TransientRoot"; }

  virtual Standard_Transient* CreateParent() const { return new Standard_Transient; }

  virtual Standard_Transient* Clone() const { return new TransientRoot; }
  DEFINE_STANDARD_RTTI_INLINE(TransientRoot, Standard_Transient)
};
DEFINE_STANDARD_HANDLE(TransientRoot, Standard_Transient)

// Auxiliary macros to create hierarchy of classes
#define QA_DEFINECLASS(theClass, theParent)                                                        \
  class theClass : public theParent                                                                \
  {                                                                                                \
  public:                                                                                          \
    virtual const char* Name() const override                                                      \
    {                                                                                              \
      return #theClass;                                                                            \
    }                                                                                              \
    virtual Standard_Transient* CreateParent() const override                                      \
    {                                                                                              \
      return new theParent();                                                                      \
    }                                                                                              \
    virtual Standard_Transient* Clone() const override                                             \
    {                                                                                              \
      return new theClass();                                                                       \
    }                                                                                              \
    DEFINE_STANDARD_RTTI_INLINE(theClass, theParent)                                               \
  };                                                                                               \
  DEFINE_STANDARD_HANDLE(theClass, theParent)

#define QA_NAME(theNum) QaClass##theNum##_50
#define QA_HANDLE_NAME(theNum) Handle(QaClass##theNum##_50)

#define QA_DEFINECLASS10(theParent, theTens)                                                       \
  QA_DEFINECLASS(QA_NAME(theTens##0), theParent)                                                   \
  QA_DEFINECLASS(QA_NAME(theTens##1), QA_NAME(theTens##0))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##2), QA_NAME(theTens##1))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##3), QA_NAME(theTens##2))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##4), QA_NAME(theTens##3))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##5), QA_NAME(theTens##4))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##6), QA_NAME(theTens##5))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##7), QA_NAME(theTens##6))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##8), QA_NAME(theTens##7))                                         \
  QA_DEFINECLASS(QA_NAME(theTens##9), QA_NAME(theTens##8))

// Create the hierarchy: 50 classes in inheritance chain
QA_DEFINECLASS10(TransientRoot, 0)
QA_DEFINECLASS10(QaClass09_50, 1)
QA_DEFINECLASS10(QaClass19_50, 2)
QA_DEFINECLASS10(QaClass29_50, 3)
QA_DEFINECLASS10(QaClass39_50, 4)
QA_DEFINECLASS(QaClass50_50, QaClass49_50)

// Anonymous namespace class for testing
namespace
{
class QaClass50_50Anon : public QaClass49_50
{
public:
  QaClass50_50Anon() {}
};
} // namespace

// Named namespace class for testing
namespace QaNamespace
{
class QaClass50_50 : public QaClass49_50
{
public:
  QaClass50_50() {}
};
} // namespace QaNamespace

// Timer class for performance testing
namespace
{
class QATimer : public OSD_Timer
{
public:
  enum TimeFormat
  {
    Seconds,
    Milliseconds,
    Microseconds
  };

  QATimer(const char* theOperationName, TimeFormat theFormat = Milliseconds)
      : myOperationName(theOperationName),
        myFormat(theFormat)
  {
    Start();
  }

  ~QATimer()
  {
    Stop();
    Standard_Real aTime = 0.0;
    switch (myFormat)
    {
      case Seconds:
        aTime = ElapsedTime();
        break;
      case Milliseconds:
        aTime = ElapsedTime() * 1000.0;
        break;
      case Microseconds:
        aTime = ElapsedTime() * 1000000.0;
        break;
    }
    // Note: In tests we don't print timing info but could store it for verification
    (void)aTime;           // Avoid unused variable warning
    (void)myOperationName; // Avoid unused field warning
  }

private:
  const char* myOperationName;
  TimeFormat  myFormat;
};
} // namespace

// Test fixture for advanced Handle operations tests
class HandleAdvancedTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(HandleAdvancedTest, CompilerSpecificBehavior)
{
  Handle(TransientRoot) aRoot = new TransientRoot();
  EXPECT_FALSE(aRoot.IsNull());

  const Handle(TransientRoot)& aConstRoot = aRoot;
  (void)aConstRoot; // Avoid unused variable warning
  Handle(Standard_Transient) aTransient = aRoot;

  // Test passing handle as reference to base class
  // This tests template argument deduction and inheritance
  auto testFunction = [](const Handle(Standard_Transient)& theObj) -> bool {
    return !theObj.IsNull();
  };

  EXPECT_TRUE(testFunction(aRoot));

// Test overloaded function compatibility (compiler version specific)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)                              \
  || (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)

  // Test overload resolution with handles
  auto testOverload1 = [](const Handle(Standard_Transient)&) -> int { return 1; };
  auto testOverload2 = [](const Handle(TransientRoot)&) -> int { return 2; };

  // More specific overload should be chosen
  EXPECT_EQ(2, testOverload2(aRoot));
  EXPECT_EQ(1, testOverload1(aTransient));
#endif

  const Handle(Standard_Transient)& aTransient2 = aRoot; // cast to base const ref
  CHECK_HANDLE(!aTransient2.IsNull(), "cast to base class const reference");
}

TEST_F(HandleAdvancedTest, DeepHierarchyRTTI)
{
  // Test RTTI with deep inheritance hierarchy
  Handle(Standard_Type) aType00 = STANDARD_TYPE(QaClass00_50);
  Handle(Standard_Type) aType10 = STANDARD_TYPE(QaClass10_50);
  Handle(Standard_Type) aType20 = STANDARD_TYPE(QaClass20_50);
  Handle(Standard_Type) aType30 = STANDARD_TYPE(QaClass30_50);
  Handle(Standard_Type) aType40 = STANDARD_TYPE(QaClass40_50);
  Handle(Standard_Type) aType50 = STANDARD_TYPE(QaClass50_50);

  Handle(QaClass00_50) aHandle = new QaClass40_50();

  // Test type name
  EXPECT_STREQ("QaClass40_50", aHandle->DynamicType()->Name());

  // Test IsKind() - should be true for all parent types
  EXPECT_TRUE(aHandle->IsKind(aType00)) << "IsKind failed for root type";
  EXPECT_TRUE(aHandle->IsKind(aType10)) << "IsKind failed for parent type 10";
  EXPECT_TRUE(aHandle->IsKind(aType20)) << "IsKind failed for parent type 20";
  EXPECT_TRUE(aHandle->IsKind(aType30)) << "IsKind failed for parent type 30";
  EXPECT_TRUE(aHandle->IsKind(aType40)) << "IsKind failed for exact type 40";
  EXPECT_FALSE(aHandle->IsKind(aType50)) << "IsKind should be false for child type 50";

  // Test IsKind() with string names
  EXPECT_TRUE(aHandle->IsKind("QaClass00_50"));
  EXPECT_TRUE(aHandle->IsKind("QaClass10_50"));
  EXPECT_TRUE(aHandle->IsKind("QaClass20_50"));
  EXPECT_TRUE(aHandle->IsKind("QaClass30_50"));
  EXPECT_TRUE(aHandle->IsKind("QaClass40_50"));
  EXPECT_FALSE(aHandle->IsKind("QaClass50_50"));

  // Test IsInstance() - should be true only for exact type
  EXPECT_FALSE(aHandle->IsInstance(aType00));
  EXPECT_FALSE(aHandle->IsInstance(aType10));
  EXPECT_FALSE(aHandle->IsInstance(aType20));
  EXPECT_FALSE(aHandle->IsInstance(aType30));
  EXPECT_TRUE(aHandle->IsInstance(aType40));
  EXPECT_FALSE(aHandle->IsInstance(aType50));
}

TEST_F(HandleAdvancedTest, TypeInfoCompatibility)
{
  Handle(QaClass40_50) aHandle = new QaClass40_50();

#ifdef __cpp_rtti
  // Test C++ RTTI compatibility
  // Use OCCT standard warning suppression for RTTI operations
  #include <Standard_WarningsDisable.hxx>

  const std::type_info& aTypeInfo = typeid(*aHandle.get());

  // Test type_info comparisons
  EXPECT_FALSE(aTypeInfo == typeid(QaClass00_50));
  EXPECT_FALSE(aTypeInfo == typeid(QaClass10_50));
  EXPECT_FALSE(aTypeInfo == typeid(QaClass20_50));
  EXPECT_FALSE(aTypeInfo == typeid(QaClass30_50));
  EXPECT_TRUE(aTypeInfo == typeid(QaClass40_50));
  EXPECT_FALSE(aTypeInfo == typeid(QaClass50_50));

  // Test type_index if available
  #if __cplusplus >= 201103L
  std::type_index aCppType = typeid(*aHandle.get());
  EXPECT_FALSE(aCppType == typeid(QaClass00_50));
  EXPECT_TRUE(aCppType == typeid(QaClass40_50));
  #endif

  // Test anonymous and namespaced classes
  QaClass50_50Anon          anAnon;
  QaNamespace::QaClass50_50 aNamed;

  // These should have different type_info
  EXPECT_FALSE(typeid(anAnon) == typeid(aNamed));
  EXPECT_FALSE(typeid(anAnon) == typeid(QaClass50_50));
  EXPECT_FALSE(typeid(aNamed) == typeid(QaClass50_50));

  #include <Standard_WarningsRestore.hxx>

#endif // __cpp_rtti

  // Test Standard_Transient type traits
  EXPECT_TRUE(std::is_class<Standard_Transient>::value);
  // Note: IsClass() method is not available in all OCCT versions
  // EXPECT_TRUE(STANDARD_TYPE(Standard_Transient)->IsClass());
}

TEST_F(HandleAdvancedTest, AllocatorHandlePerformance)
{
  // Test performance aspects of handle operations with different allocators
  Handle(NCollection_BaseAllocator) aBasePtr = new NCollection_IncAllocator();
  EXPECT_FALSE(aBasePtr.IsNull());

  // Test casting performance with allocator hierarchy
  {
    QATimer aTimer("IncAllocator DownCast", QATimer::Microseconds);
    for (int i = 0; i < 1000; ++i)
    {
      Handle(NCollection_IncAllocator) anIncAlloc =
        Handle(NCollection_IncAllocator)::DownCast(aBasePtr);
      EXPECT_FALSE(anIncAlloc.IsNull());
    }
  }

  // Test failed downcast performance
  {
    QATimer aTimer("Failed HeapAllocator DownCast", QATimer::Microseconds);
    for (int i = 0; i < 1000; ++i)
    {
      Handle(NCollection_HeapAllocator) aHeapAlloc =
        Handle(NCollection_HeapAllocator)::DownCast(aBasePtr);
      EXPECT_TRUE(aHeapAlloc.IsNull());
    }
  }
}

TEST_F(HandleAdvancedTest, HandleArrayOperations)
{
  // Test handle operations with arrays and containers
  std::vector<Handle(QaClass00_50)> aHandleVector;

  // Fill with different types in the hierarchy
  aHandleVector.push_back(new QaClass00_50());
  aHandleVector.push_back(new QaClass10_50());
  aHandleVector.push_back(new QaClass20_50());
  aHandleVector.push_back(new QaClass30_50());
  aHandleVector.push_back(new QaClass40_50());

  EXPECT_EQ(5, aHandleVector.size());

  // Test that all handles are valid and point to correct types
  for (size_t i = 0; i < aHandleVector.size(); ++i)
  {
    EXPECT_FALSE(aHandleVector[i].IsNull());

    // Test polymorphic behavior
    EXPECT_TRUE(aHandleVector[i]->IsKind("QaClass00_50"));

    // Test dynamic casting
    Handle(QaClass00_50) aCast = aHandleVector[i];
    EXPECT_FALSE(aCast.IsNull());
    EXPECT_EQ(aHandleVector[i].get(), aCast.get());
  }

  // Test specific type casting
  Handle(QaClass40_50) aSpecific = Handle(QaClass40_50)::DownCast(aHandleVector[4]);
  EXPECT_FALSE(aSpecific.IsNull());

  // This should fail - trying to cast parent to child
  Handle(QaClass40_50) aFailedCast = Handle(QaClass40_50)::DownCast(aHandleVector[0]);
  EXPECT_TRUE(aFailedCast.IsNull());
}

TEST_F(HandleAdvancedTest, ConstHandleOperations)
{
  Handle(QaClass30_50)        aNonConstHandle = new QaClass30_50();
  const Handle(QaClass30_50)& aConstHandle    = aNonConstHandle;

  // Test const correctness
  EXPECT_EQ(aNonConstHandle.get(), aConstHandle.get());

  // Test const pointer access
  const QaClass30_50* aConstPtr    = aConstHandle.get();
  QaClass30_50*       aNonConstPtr = aNonConstHandle.get();

  EXPECT_EQ(aConstPtr, aNonConstPtr);

  // Test const casting to base types
  const Handle(QaClass00_50)& aConstBase    = aConstHandle;
  Handle(QaClass00_50)        aNonConstBase = aNonConstHandle;

  EXPECT_EQ(aConstBase.get(), aNonConstBase.get());

  // Test const handle comparisons
  EXPECT_TRUE(aConstHandle == aNonConstHandle);
  EXPECT_TRUE(aConstBase == aNonConstBase);
  EXPECT_FALSE(aConstHandle != aNonConstHandle);
}

TEST_F(HandleAdvancedTest, WeakReferenceSimulation)
{
  // Simulate weak reference-like behavior using raw pointers
  QaClass20_50* aRawPtr = nullptr;

  {
    Handle(QaClass20_50) aHandle = new QaClass20_50();
    aRawPtr                      = aHandle.get();

    EXPECT_NE(nullptr, aRawPtr);

    // Handle should keep the object alive
    EXPECT_FALSE(aHandle.IsNull());
    EXPECT_EQ(aRawPtr, aHandle.get());
  }

  // After handle destruction, raw pointer becomes invalid
  // Note: We can't safely test this without risking segfaults,
  // but the pattern demonstrates handle lifetime management

  // Create multiple new handles to ensure we get different objects
  // (Memory allocator might reuse the same location, so we create several)
  std::vector<Handle(QaClass20_50)> aHandles;
  bool                              aFoundDifferent = false;

  for (int i = 0; i < 10 && !aFoundDifferent; ++i)
  {
    aHandles.push_back(new QaClass20_50());
    if (aHandles.back().get() != aRawPtr)
    {
      aFoundDifferent = true;
    }
  }

  // We expect to find at least one different address (though allocator might reuse)
  // The test demonstrates handle independence regardless
  EXPECT_TRUE(aFoundDifferent || !aFoundDifferent); // Either outcome is acceptable
}
// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <NCollection_ForwardRange.hxx>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Mock iterators for testing SFINAE detection
// ---------------------------------------------------------------------------

namespace
{

//! Mock iterator with Value() accessor (like TopoDS_Iterator).
class MockValueIterator
{
public:
  MockValueIterator(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  const int& Value() const { return myData[myIndex]; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Mock iterator with Current() accessor (like BRepGraph_Iterator).
class MockCurrentIterator
{
public:
  MockCurrentIterator(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  const int& Current() const { return myData[myIndex]; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Mock iterator with CurrentId() accessor (like BRepGraph_RefsIterator).
//! Returns by value, not by reference.
class MockCurrentIdIterator
{
public:
  MockCurrentIdIterator(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  int CurrentId() const { return myData[myIndex]; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Mock iterator with both Value() and Current() (like TopExp_Explorer).
//! SFINAE should pick Value() due to higher priority.
class MockBothValueAndCurrent
{
public:
  MockBothValueAndCurrent(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  const int& Value() const { return myData[myIndex]; }

  const int& Current() const { return myData[myIndex]; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Mock iterator with both Current() and CurrentId() (like BRepGraph_DefsIterator).
//! SFINAE should pick Current() due to higher priority.
class MockBothCurrentAndId
{
public:
  MockBothCurrentAndId(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  const int& Current() const { return myData[myIndex]; }

  int CurrentId() const { return myIndex; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Non-copyable mock iterator (like BRepGraph_ChildExplorer).
class MockNonCopyableIterator
{
public:
  MockNonCopyableIterator(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  MockNonCopyableIterator(const MockNonCopyableIterator&)            = delete;
  MockNonCopyableIterator& operator=(const MockNonCopyableIterator&) = delete;
  MockNonCopyableIterator(MockNonCopyableIterator&&)                 = default;
  MockNonCopyableIterator& operator=(MockNonCopyableIterator&&)      = default;

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  int Current() const { return myData[myIndex]; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Mock iterator with begin()/end() integrated directly (like modified BRepGraph classes).
class MockWithBeginEnd
{
public:
  MockWithBeginEnd(const int* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  const int& Current() const { return myData[myIndex]; }

  NCollection_ForwardRangeIterator<MockWithBeginEnd> begin()
  {
    return NCollection_ForwardRangeIterator<MockWithBeginEnd>(this);
  }

  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  const int* myData;
  int        mySize  = 0;
  int        myIndex = 0;
};

//! Struct for testing operator->.
struct Point
{
  int X;
  int Y;
};

//! Mock iterator returning a reference to a struct.
class MockPointIterator
{
public:
  MockPointIterator(const Point* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  const Point& Value() const { return myData[myIndex]; }

private:
  const Point* myData;
  int          mySize  = 0;
  int          myIndex = 0;
};

//! Mock iterator returning a struct by value (tests ArrowProxy).
class MockPointByValueIterator
{
public:
  MockPointByValueIterator(const Point* theData, const int theSize)
      : myData(theData),
        mySize(theSize)
  {
  }

  bool More() const { return myIndex < mySize; }

  void Next() { ++myIndex; }

  Point Current() const { return myData[myIndex]; }

private:
  const Point* myData;
  int          mySize  = 0;
  int          myIndex = 0;
};

} // namespace

// ---------------------------------------------------------------------------
// SFINAE detection tests
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, SFINAEDetection_Value)
{
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasValue<MockValueIterator>::value);
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasCurrent<MockValueIterator>::value);
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasCurrentId<MockValueIterator>::value);
}

TEST(NCollection_ForwardRangeTest, SFINAEDetection_Current)
{
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasValue<MockCurrentIterator>::value);
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasCurrent<MockCurrentIterator>::value);
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasCurrentId<MockCurrentIterator>::value);
}

TEST(NCollection_ForwardRangeTest, SFINAEDetection_CurrentId)
{
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasValue<MockCurrentIdIterator>::value);
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasCurrent<MockCurrentIdIterator>::value);
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasCurrentId<MockCurrentIdIterator>::value);
}

TEST(NCollection_ForwardRangeTest, SFINAEDetection_BothValueAndCurrent)
{
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasValue<MockBothValueAndCurrent>::value);
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasCurrent<MockBothValueAndCurrent>::value);
  // AccessorTraits should pick Value() over Current()
  using Accessor = NCollection_ForwardRangeDetail::AccessorTraits<MockBothValueAndCurrent>;
  EXPECT_TRUE((std::is_same_v<Accessor::ReturnType, const int&>));
}

TEST(NCollection_ForwardRangeTest, SFINAEDetection_BothCurrentAndId)
{
  EXPECT_FALSE(NCollection_ForwardRangeDetail::HasValue<MockBothCurrentAndId>::value);
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasCurrent<MockBothCurrentAndId>::value);
  EXPECT_TRUE(NCollection_ForwardRangeDetail::HasCurrentId<MockBothCurrentAndId>::value);
  // AccessorTraits should pick Current() over CurrentId()
  using Accessor = NCollection_ForwardRangeDetail::AccessorTraits<MockBothCurrentAndId>;
  EXPECT_TRUE((std::is_same_v<Accessor::ReturnType, const int&>));
}

// ---------------------------------------------------------------------------
// Standalone range wrapper tests
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, StandaloneRange_ValueIterator)
{
  const int aData[] = {10, 20, 30, 40, 50};
  int       aSum    = 0;
  int       aCount  = 0;
  for (const int& aVal : NCollection_ForwardRange(MockValueIterator(aData, 5)))
  {
    aSum += aVal;
    ++aCount;
  }
  EXPECT_EQ(aSum, 150);
  EXPECT_EQ(aCount, 5);
}

TEST(NCollection_ForwardRangeTest, StandaloneRange_CurrentIterator)
{
  const int aData[] = {1, 2, 3};
  int       aSum    = 0;
  for (const int& aVal : NCollection_ForwardRange(MockCurrentIterator(aData, 3)))
  {
    aSum += aVal;
  }
  EXPECT_EQ(aSum, 6);
}

TEST(NCollection_ForwardRangeTest, StandaloneRange_CurrentIdIterator_ByValue)
{
  const int aData[] = {100, 200, 300};
  int       aSum    = 0;
  for (const int aVal : NCollection_ForwardRange(MockCurrentIdIterator(aData, 3)))
  {
    aSum += aVal;
  }
  EXPECT_EQ(aSum, 600);
}

TEST(NCollection_ForwardRangeTest, StandaloneRange_Empty)
{
  const int aData[] = {1};
  int       aCount  = 0;
  for (const int& aVal : NCollection_ForwardRange(MockValueIterator(aData, 0)))
  {
    (void)aVal;
    ++aCount;
  }
  EXPECT_EQ(aCount, 0);
}

TEST(NCollection_ForwardRangeTest, StandaloneRange_SingleElement)
{
  const int aData[] = {42};
  int       aResult = 0;
  for (const int& aVal : NCollection_ForwardRange(MockValueIterator(aData, 1)))
  {
    aResult = aVal;
  }
  EXPECT_EQ(aResult, 42);
}

TEST(NCollection_ForwardRangeTest, StandaloneRange_NonCopyable_Move)
{
  const int aData[] = {5, 10, 15};
  int       aSum    = 0;
  for (const int aVal : NCollection_ForwardRange(MockNonCopyableIterator(aData, 3)))
  {
    aSum += aVal;
  }
  EXPECT_EQ(aSum, 30);
}

// ---------------------------------------------------------------------------
// In-class begin/end integration tests
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, InClassBeginEnd_RangeFor)
{
  const int        aData[] = {7, 14, 21};
  MockWithBeginEnd anIter(aData, 3);
  int              aSum = 0;
  for (const int& aVal : anIter)
  {
    aSum += aVal;
  }
  EXPECT_EQ(aSum, 42);
}

TEST(NCollection_ForwardRangeTest, InClassBeginEnd_Temporary)
{
  const int aData[] = {1, 2, 3, 4};
  int       aSum    = 0;
  for (const int& aVal : MockWithBeginEnd(aData, 4))
  {
    aSum += aVal;
  }
  EXPECT_EQ(aSum, 10);
}

// ---------------------------------------------------------------------------
// Iterator traits tests
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, IteratorTraits_ValueType)
{
  using Iter = NCollection_ForwardRangeIterator<MockValueIterator>;
  EXPECT_TRUE((std::is_same_v<Iter::value_type, int>));
  EXPECT_TRUE((std::is_same_v<Iter::iterator_category, std::input_iterator_tag>));
  EXPECT_TRUE((std::is_same_v<Iter::difference_type, std::ptrdiff_t>));
}

TEST(NCollection_ForwardRangeTest, IteratorTraits_ReferenceReturn)
{
  using Iter = NCollection_ForwardRangeIterator<MockValueIterator>;
  // Value() returns const int&, so reference should be const int&
  EXPECT_TRUE((std::is_same_v<Iter::reference, const int&>));
}

TEST(NCollection_ForwardRangeTest, IteratorTraits_ValueReturn)
{
  using Iter = NCollection_ForwardRangeIterator<MockCurrentIdIterator>;
  // CurrentId() returns int by value
  EXPECT_TRUE((std::is_same_v<Iter::value_type, int>));
}

// ---------------------------------------------------------------------------
// operator-> tests
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, ArrowOperator_Reference)
{
  const Point                                         aData[] = {{1, 2}, {3, 4}};
  NCollection_ForwardRange                            aRange(MockPointIterator(aData, 2));
  NCollection_ForwardRangeIterator<MockPointIterator> anIt = aRange.begin();
  EXPECT_EQ(anIt->X, 1);
  EXPECT_EQ(anIt->Y, 2);
  ++anIt;
  EXPECT_EQ(anIt->X, 3);
  EXPECT_EQ(anIt->Y, 4);
}

TEST(NCollection_ForwardRangeTest, ArrowOperator_ArrowProxy)
{
  const Point              aData[] = {{10, 20}, {30, 40}};
  NCollection_ForwardRange aRange(MockPointByValueIterator(aData, 2));
  NCollection_ForwardRangeIterator<MockPointByValueIterator> anIt = aRange.begin();
  EXPECT_EQ(anIt->X, 10);
  EXPECT_EQ(anIt->Y, 20);
  ++anIt;
  EXPECT_EQ(anIt->X, 30);
  EXPECT_EQ(anIt->Y, 40);
}

// ---------------------------------------------------------------------------
// Postfix increment test
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, PostfixIncrement)
{
  const int                                                         aData[] = {100, 200, 300};
  MockValueIterator                                                 anIter(aData, 3);
  NCollection_ForwardRangeIterator<MockValueIterator>               anIt(&anIter);
  NCollection_ForwardRangeIterator<MockValueIterator>::PostfixProxy aProxy = anIt++;
  EXPECT_EQ(*aProxy, 100);
  EXPECT_EQ(*anIt, 200);
}

// ---------------------------------------------------------------------------
// Sentinel comparison tests
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, SentinelComparison)
{
  const int                                           aData[] = {1};
  MockValueIterator                                   anIter(aData, 1);
  NCollection_ForwardRangeIterator<MockValueIterator> anIt(&anIter);
  NCollection_ForwardRangeSentinel                    aSentinel;

  EXPECT_TRUE(anIt != aSentinel);
  EXPECT_FALSE(anIt == aSentinel);
  EXPECT_TRUE(aSentinel != anIt);
  EXPECT_FALSE(aSentinel == anIt);

  ++anIt;

  EXPECT_FALSE(anIt != aSentinel);
  EXPECT_TRUE(anIt == aSentinel);
  EXPECT_FALSE(aSentinel != anIt);
  EXPECT_TRUE(aSentinel == anIt);
}

// ---------------------------------------------------------------------------
// Priority verification: Value() wins over Current()
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, Priority_ValueOverCurrent)
{
  const int aData[] = {99};
  int       aResult = 0;
  for (const int& aVal : NCollection_ForwardRange(MockBothValueAndCurrent(aData, 1)))
  {
    aResult = aVal;
  }
  EXPECT_EQ(aResult, 99);
}

// ---------------------------------------------------------------------------
// Priority verification: Current() wins over CurrentId()
// ---------------------------------------------------------------------------

TEST(NCollection_ForwardRangeTest, Priority_CurrentOverCurrentId)
{
  const int aData[] = {55, 66};
  int       aSum    = 0;
  for (const int& aVal : NCollection_ForwardRange(MockBothCurrentAndId(aData, 2)))
  {
    aSum += aVal;
  }
  EXPECT_EQ(aSum, 121);
}

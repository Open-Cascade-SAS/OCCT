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

#include <NCollection_BitDynamicArray.hxx>
#include <NCollection_PagedArray.hxx>
#include <NCollection_PagedDataMap.hxx>
#include <NCollection_PersistentRadixMap.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <cstdint>
#include <iterator>
#include <limits>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

namespace
{
struct PagedTestValue
{
  std::shared_ptr<int> Resource;
};

struct NonDefaultPagedValue
{
  explicit NonDefaultPagedValue(const size_t theNumber)
      : Number(theNumber)
  {
  }

  NonDefaultPagedValue(const NonDefaultPagedValue&) = default;
  NonDefaultPagedValue& operator=(const NonDefaultPagedValue&) = delete;

  size_t Number;
};

struct ThrowingPagedValue
{
  explicit ThrowingPagedValue(const size_t theNumber = 0, const bool theToThrow = false)
      : Number(theNumber),
        ToThrow(theToThrow)
  {
  }

  ThrowingPagedValue(const ThrowingPagedValue&) = default;

  ThrowingPagedValue& operator=(const ThrowingPagedValue& theOther)
  {
    if (theOther.ToThrow)
    {
      throw Standard_Failure("ThrowingPagedValue assignment");
    }
    Number  = theOther.Number;
    ToThrow = theOther.ToThrow;
    return *this;
  }

  ThrowingPagedValue& operator=(ThrowingPagedValue&& theOther)
  {
    return operator=(static_cast<const ThrowingPagedValue&>(theOther));
  }

  size_t Number;
  bool   ToThrow;
};

struct StatefulPagedHasher
{
  explicit StatefulPagedHasher(const size_t theSeed)
      : Seed(theSeed)
  {
  }

  StatefulPagedHasher(const StatefulPagedHasher&) = default;
  StatefulPagedHasher& operator=(const StatefulPagedHasher&) = delete;

  size_t operator()(const size_t theKey) const noexcept { return theKey ^ Seed; }

  bool operator()(const size_t theLeft, const size_t theRight) const noexcept
  {
    return theLeft == theRight;
  }

  size_t Seed;
};

struct PersistentRadixTestHasher
{
  using ChildHash = std::pair<uint8_t, uint64_t>;

  static uint64_t EmptyHash() { return 1469598103934665603ULL; }

  static uint64_t LeafHash(const uint64_t theKey, const uint32_t theValue)
  {
    return (theKey * 1099511628211ULL) ^ theValue;
  }

  static uint64_t BranchHash(const size_t     theDepth,
                             const ChildHash* theChildren,
                             const size_t     theNbChildren)
  {
    uint64_t aHash = EmptyHash() ^ static_cast<uint64_t>(theDepth);
    for (size_t anIndex = 0; anIndex < theNbChildren; ++anIndex)
    {
      const auto& [aSlot, aChildHash] = theChildren[anIndex];
      aHash = (aHash ^ aSlot) * 1099511628211ULL;
      aHash = (aHash ^ aChildHash) * 1099511628211ULL;
    }
    return aHash;
  }
};

using PersistentRadixTestMap =
  NCollection_PersistentRadixMap<uint64_t, uint32_t, uint64_t, PersistentRadixTestHasher>;

struct NonDefaultRadixValue
{
  explicit NonDefaultRadixValue(const uint32_t theNumber)
      : Number(theNumber)
  {
  }

  bool operator==(const NonDefaultRadixValue& theOther) const { return Number == theOther.Number; }

  uint32_t Number;
};

struct NonDefaultRadixHasher
{
  using ChildHash = PersistentRadixTestHasher::ChildHash;

  static uint64_t EmptyHash() { return PersistentRadixTestHasher::EmptyHash(); }

  static uint64_t LeafHash(const uint64_t theKey, const NonDefaultRadixValue& theValue)
  {
    return PersistentRadixTestHasher::LeafHash(theKey, theValue.Number);
  }

  static uint64_t BranchHash(const size_t     theDepth,
                             const ChildHash* theChildren,
                             const size_t     theNbChildren)
  {
    return PersistentRadixTestHasher::BranchHash(theDepth, theChildren, theNbChildren);
  }
};

using NonDefaultRadixMap =
  NCollection_PersistentRadixMap<uint64_t, NonDefaultRadixValue, uint64_t, NonDefaultRadixHasher>;

static_assert(
  std::is_constructible<NCollection_PagedArray<int>::Iterator,
                        const NCollection_PagedArray<int>&>::value,
  "legacy paged-array iterator must accept a const container");
static_assert(
  std::is_constructible<NCollection_PagedDataMap<int, int>::Iterator,
                        const NCollection_PagedDataMap<int, int>&>::value,
  "legacy paged-map iterator must accept a const container");
} // namespace

TEST(NCollection_PersistentRadixMapTest, BuildUpdatesAndSnapshotsAreCanonical)
{
  NCollection_LinearVector<PersistentRadixTestMap::Entry> anEntries;
  anEntries.Append({0xFF00000000000001ULL, 30});
  anEntries.Append({1, 10});
  anEntries.Append({0x0100000000000001ULL, 20});

  PersistentRadixTestMap aBuilt;
  ASSERT_TRUE(PersistentRadixTestMap::Build(anEntries, aBuilt));
  const PersistentRadixTestMap anInserted = PersistentRadixTestMap()
                                              .Insert(1, 10)
                                              .Insert(0x0100000000000001ULL, 20)
                                              .Insert(0xFF00000000000001ULL, 30);
  ASSERT_EQ(aBuilt.Size(), 3u);
  EXPECT_EQ(aBuilt.RootHash(), anInserted.RootHash());
  EXPECT_TRUE(aBuilt.Contains(1));
  EXPECT_FALSE(aBuilt.Contains(2));
  ASSERT_NE(aBuilt.Find(1), nullptr);
  EXPECT_EQ(*aBuilt.Find(1), 10u);

  const PersistentRadixTestMap anUnchanged = aBuilt.Insert(1, 10);
  EXPECT_EQ(anUnchanged.RootIdentity(), aBuilt.RootIdentity());

  const PersistentRadixTestMap aChanged = aBuilt.Insert(1, 11);
  EXPECT_NE(aChanged.RootIdentity(), aBuilt.RootIdentity());
  EXPECT_NE(aChanged.RootHash(), aBuilt.RootHash());
  EXPECT_EQ(aChanged.Size(), aBuilt.Size());

  const PersistentRadixTestMap aRemoved = aChanged.Remove(0x0100000000000001ULL);
  EXPECT_EQ(aRemoved.Size(), 2u);
  EXPECT_FALSE(aRemoved.Contains(0x0100000000000001ULL));
  EXPECT_TRUE(aBuilt.Contains(0x0100000000000001ULL));
}

TEST(NCollection_PersistentRadixMapTest, BuildRejectsDuplicateKeys)
{
  NCollection_LinearVector<PersistentRadixTestMap::Entry> anEntries;
  anEntries.Append({7, 1});
  anEntries.Append({7, 2});
  PersistentRadixTestMap aMap  = PersistentRadixTestMap().Insert(1, 42);
  const void*            aRoot = aMap.RootIdentity();
  EXPECT_FALSE(PersistentRadixTestMap::Build(anEntries, aMap));
  EXPECT_EQ(aMap.RootIdentity(), aRoot);
  EXPECT_EQ(*aMap.Find(1), 42u);
}

TEST(NCollection_PersistentRadixMapTest, ValueDoesNotNeedDefaultConstructor)
{
  const NonDefaultRadixMap aMap = NonDefaultRadixMap().Insert(17, NonDefaultRadixValue(42));
  ASSERT_NE(aMap.Find(17), nullptr);
  EXPECT_EQ(aMap.Find(17)->Number, 42u);
}

TEST(NCollection_PersistentRadixMapTest, IteratorsFollowCanonicalKeyOrder)
{
  const PersistentRadixTestMap aMap = PersistentRadixTestMap()
                                        .Insert(0xFF00000000000001ULL, 30)
                                        .Insert(7, 10)
                                        .Insert(0x0100000000000001ULL, 20);

  std::vector<uint64_t> aKeys;
  uint32_t              aValueSum = 0;
  for (const auto [aKey, aValue] : aMap.Items())
  {
    aKeys.push_back(aKey);
    aValueSum += aValue;
  }
  ASSERT_EQ(aKeys.size(), 3u);
  EXPECT_EQ(aKeys[0], 7u);
  EXPECT_EQ(aKeys[1], 0x0100000000000001ULL);
  EXPECT_EQ(aKeys[2], 0xFF00000000000001ULL);
  EXPECT_EQ(aValueSum, 60u);

  uint32_t aRangeSum = 0;
  for (const uint32_t aValue : aMap)
  {
    aRangeSum += aValue;
  }
  EXPECT_EQ(aRangeSum, aValueSum);
  EXPECT_EQ(std::distance(aMap.cbegin(), aMap.cend()), 3);

  PersistentRadixTestMap::Iterator anIterator(aMap);
  ASSERT_TRUE(anIterator.More());
  EXPECT_EQ(anIterator.Key(), 7u);
  anIterator.Next();
  EXPECT_EQ(anIterator.Value(), 20u);
  anIterator.Reset();
  EXPECT_EQ(anIterator.Key(), 7u);

  PersistentRadixTestMap::Iterator anEnd;
  EXPECT_THROW((void)anEnd.Key(), Standard_NoSuchObject);
  EXPECT_THROW((void)anEnd.Value(), Standard_NoSuchObject);
}

TEST(NCollection_PersistentRadixMapTest, RemovalCollapsesBranchToSingleChild)
{
  const PersistentRadixTestMap aMap = PersistentRadixTestMap().Insert(1, 10).Insert(2, 20);
  const PersistentRadixTestMap aReduced  = aMap.Remove(2);
  const PersistentRadixTestMap aExpected = PersistentRadixTestMap().Insert(1, 10);

  EXPECT_EQ(aReduced.Size(), 1u);
  EXPECT_EQ(aReduced.RootHash(), aExpected.RootHash());
  ASSERT_NE(aReduced.Find(1), nullptr);
  EXPECT_EQ(*aReduced.Find(1), 10u);
  EXPECT_EQ(std::distance(aReduced.begin(), aReduced.end()), 1);
}

TEST(NCollection_PagedArrayTest, CopyOnWriteDetachesOnlyModifiedSnapshot)
{
  NCollection_PagedArray<int> aBase(4);
  for (int anIndex = 0; anIndex < 12; ++anIndex)
  {
    aBase.Append(anIndex);
  }

  NCollection_PagedArray<int> aChanged = aBase;
  aChanged.ChangeValue(5)              = 50;
  aChanged.Resize(14, 7);

  EXPECT_EQ(aBase.Size(), 12u);
  EXPECT_EQ(aBase.Value(5), 5);
  EXPECT_EQ(aChanged.Value(5), 50);
  EXPECT_EQ(aChanged.Value(12), 7);
  EXPECT_EQ(aChanged.Value(13), 7);
}

TEST(NCollection_PagedArrayTest, SharedSnapshotSupportsConcurrentReadsAndIndependentUpdates)
{
  NCollection_PagedArray<int> aValues(8);
  for (int anIndex = 0; anIndex < 128; ++anIndex)
  {
    aValues.Append(anIndex);
  }
  const NCollection_PagedArray<int> aBase = aValues;

  std::atomic<bool>        isValid{true};
  std::vector<std::thread> aThreads;
  for (int aThread = 0; aThread < 8; ++aThread)
  {
    aThreads.emplace_back([&aBase, &isValid, aThread]() {
      NCollection_PagedArray<int> aChanged               = aBase;
      aChanged.ChangeValue(static_cast<size_t>(aThread)) = -aThread - 1;
      for (int anIndex = 0; anIndex < 128; ++anIndex)
      {
        if (aBase.Value(static_cast<size_t>(anIndex)) != anIndex)
        {
          isValid.store(false, std::memory_order_relaxed);
        }
      }
      if (aChanged.Value(static_cast<size_t>(aThread)) != -aThread - 1)
      {
        isValid.store(false, std::memory_order_relaxed);
      }
    });
  }
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }
  EXPECT_TRUE(isValid.load(std::memory_order_relaxed));
}

TEST(NCollection_PagedArrayTest, ShrinkAndExtensionRestoreDefaultValues)
{
  NCollection_PagedArray<int> aValues(4);
  aValues.Resize(4, 9);
  aValues.Resize(1);

  EXPECT_EQ(aValues.Appended(), 0);

  aValues.Resize(1);
  aValues.SetValue(3, 7);
  EXPECT_EQ(aValues.Value(1), 0);
  EXPECT_EQ(aValues.Value(2), 0);
  EXPECT_EQ(aValues.Value(3), 7);
}

TEST(NCollection_PagedArrayTest, ShrinkReleasesRetainedPageValues)
{
  NCollection_PagedArray<PagedTestValue> aValues(4);
  aValues.Resize(3);
  aValues.ChangeValue(2).Resource = std::make_shared<int>(42);
  std::weak_ptr<int> aResource    = aValues.Value(2).Resource;

  aValues.Resize(1);
  EXPECT_TRUE(aResource.expired());

  aValues.Resize(3);
  EXPECT_FALSE(aValues.Value(1).Resource);
  EXPECT_FALSE(aValues.Value(2).Resource);
}

TEST(NCollection_PagedArrayTest, ShrinkDetachesSharedTailPage)
{
  NCollection_PagedArray<PagedTestValue> aBase(4);
  aBase.Resize(3);
  aBase.ChangeValue(2).Resource = std::make_shared<int>(42);

  NCollection_PagedArray<PagedTestValue> aChanged = aBase;
  aChanged.Resize(1);
  aChanged.Resize(3);

  ASSERT_TRUE(aBase.Value(2).Resource);
  EXPECT_EQ(*aBase.Value(2).Resource, 42);
  EXPECT_FALSE(aChanged.Value(2).Resource);
}

TEST(NCollection_PagedArrayTest, SparseSetInitializesEveryGap)
{
  NCollection_PagedArray<int> aValues(3);
  aValues.SetValue(10, 42);

  ASSERT_EQ(aValues.Size(), 11u);
  for (size_t anIndex = 0; anIndex < 10; ++anIndex)
  {
    EXPECT_EQ(aValues.Value(anIndex), 0) << "index " << anIndex;
  }
  EXPECT_EQ(aValues.Value(10), 42);
}

TEST(NCollection_PagedArrayTest, FailedGrowthKeepsVisibleRangeConsistent)
{
  NCollection_PagedArray<ThrowingPagedValue> aValues(2);
  aValues.Append(ThrowingPagedValue(1));

  EXPECT_THROW(aValues.Append(ThrowingPagedValue(2, true)), Standard_Failure);
  ASSERT_EQ(aValues.Size(), 1u);
  EXPECT_EQ(aValues.First().Number, 1u);

  EXPECT_THROW(aValues.Resize(4, ThrowingPagedValue(3, true)), Standard_Failure);
  ASSERT_EQ(aValues.Size(), 1u);
  EXPECT_EQ(aValues.First().Number, 1u);
}

TEST(NCollection_PagedArrayTest, RejectsUnrepresentableSetIndex)
{
  NCollection_PagedArray<size_t> aValues;
  EXPECT_THROW(aValues.SetValue(std::numeric_limits<size_t>::max(), 1), Standard_OutOfRange);
  EXPECT_TRUE(aValues.IsEmpty());
}

TEST(NCollection_PagedArrayTest, SupportsStandardAndLegacyIteration)
{
  NCollection_PagedArray<int> aValues(3);
  for (int aValue = 1; aValue <= 7; ++aValue)
  {
    aValues.Append(aValue);
  }

  for (int& aValue : aValues)
  {
    aValue *= 2;
  }
  const NCollection_PagedArray<int>& aConstValues = aValues;
  EXPECT_EQ(aConstValues.end() - aConstValues.begin(), 7);
  EXPECT_EQ(aConstValues.cend() - aConstValues.cbegin(), 7);
  EXPECT_EQ(aConstValues.begin()[3], 8);
  EXPECT_EQ(aConstValues.First(), 2);
  EXPECT_EQ(aConstValues.Last(), 14);

  NCollection_PagedArray<int>::iterator       aMutableIterator = aValues.begin();
  NCollection_PagedArray<int>::const_iterator aConstStlIterator;
  aConstStlIterator = aMutableIterator;
  EXPECT_EQ(*aConstStlIterator, *aMutableIterator);
  *aMutableIterator = 3;

  NCollection_PagedArray<int>::Iterator anIterator(aConstValues);
  ASSERT_TRUE(anIterator.More());
  aValues.Append(16);
  int aVisited = 0;
  for (; anIterator.More(); anIterator.Next())
  {
    ++aVisited;
  }
  EXPECT_EQ(aVisited, 7);
  EXPECT_EQ(aValues.First(), 3);
}

TEST(NCollection_BitDynamicArrayTest, TailMaskAndCopyOnWriteAreStable)
{
  NCollection_BitDynamicArray aBase;
  aBase.Resize(70);
  aBase.SetAll();
  EXPECT_EQ(aBase.Size(), 70u);
  EXPECT_EQ(aBase.Block(0), ~uint64_t(0));
  EXPECT_EQ(aBase.Block(1), uint64_t(0x3f));

  NCollection_BitDynamicArray aChanged = aBase;
  aChanged.Clear(65);
  aChanged.Resize(66);

  EXPECT_TRUE(aBase.Test(65));
  EXPECT_FALSE(aChanged.Test(65));
  EXPECT_EQ(aChanged.Block(1), uint64_t(0x1));
}

TEST(NCollection_PagedDataMapTest, SnapshotsKeepIndependentBucketsAndValues)
{
  NCollection_PagedDataMap<int, int> aBase;
  for (int aKey = 0; aKey < 100; ++aKey)
  {
    aBase.Bind(aKey, aKey * 2);
  }

  NCollection_PagedDataMap<int, int> aChanged = aBase;
  ASSERT_NE(aChanged.ChangeSeek(42), nullptr);
  *aChanged.ChangeSeek(42) = 7;
  EXPECT_TRUE(aChanged.UnBind(10));
  aChanged.Bind(200, 400);

  ASSERT_NE(aBase.Seek(42), nullptr);
  EXPECT_EQ(*aBase.Seek(42), 84);
  EXPECT_TRUE(aBase.IsBound(10));
  EXPECT_FALSE(aBase.IsBound(200));
  EXPECT_EQ(*aChanged.Seek(42), 7);
}

TEST(NCollection_PagedDataMapTest, DuplicateBindReturnsFalseAndKeepsValue)
{
  NCollection_PagedDataMap<int, int> aMap;
  EXPECT_TRUE(aMap.Bind(7, 70));
  EXPECT_FALSE(aMap.Bind(7, 71));
  ASSERT_NE(aMap.Seek(7), nullptr);
  EXPECT_EQ(*aMap.Seek(7), 70);
  EXPECT_EQ(aMap.Size(), 1u);
}

TEST(NCollection_PagedDataMapTest, ValueDoesNotNeedDefaultConstructor)
{
  NCollection_PagedDataMap<size_t, NonDefaultPagedValue> aMap;
  for (size_t aKey = 0; aKey < 100; ++aKey)
  {
    EXPECT_TRUE(aMap.Bind(aKey, NonDefaultPagedValue(aKey * 2)));
  }
  EXPECT_TRUE(aMap.UnBind(17));
  EXPECT_TRUE(aMap.Bind(117, NonDefaultPagedValue(234)));
  aMap.Reserve(1000);

  ASSERT_NE(aMap.Seek(42), nullptr);
  EXPECT_EQ(aMap.Seek(42)->Number, 84u);
  ASSERT_NE(aMap.Seek(117), nullptr);
  EXPECT_EQ(aMap.Seek(117)->Number, 234u);
}

TEST(NCollection_PagedDataMapTest, UnBindDestroysDetachedEntry)
{
  NCollection_PagedDataMap<size_t, std::shared_ptr<int>> aMap;
  std::shared_ptr<int>                                   aResource = std::make_shared<int>(42);
  std::weak_ptr<int>                                     aWeakResource(aResource);
  EXPECT_TRUE(aMap.Bind(1, aResource));
  aResource.reset();

  NCollection_PagedDataMap<size_t, std::shared_ptr<int>> aSnapshot = aMap;
  EXPECT_TRUE(aMap.UnBind(1));
  EXPECT_FALSE(aWeakResource.expired());

  EXPECT_TRUE(aSnapshot.UnBind(1));
  EXPECT_TRUE(aWeakResource.expired());
}

TEST(NCollection_PagedDataMapTest, ClearFollowsCapacityRetentionConvention)
{
  NCollection_PagedDataMap<size_t, size_t> aMap;
  aMap.Reserve(100);
  const size_t aReservedCapacity = aMap.Capacity();
  EXPECT_GT(aReservedCapacity, 100u);
  EXPECT_TRUE(aMap.Bind(1, 10));

  aMap.Clear();
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Capacity(), aReservedCapacity);
  EXPECT_TRUE(aMap.Bind(2, 20));

  aMap.Clear(true);
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Capacity(), 8u);
}

TEST(NCollection_PagedDataMapTest, RehashPreservesStatefulHasher)
{
  NCollection_PagedDataMap<size_t, size_t, StatefulPagedHasher> aMap(StatefulPagedHasher(17));
  for (size_t aKey = 0; aKey < 100; ++aKey)
  {
    EXPECT_TRUE(aMap.Bind(aKey, aKey * 3));
  }
  aMap.Reserve(1000);

  EXPECT_EQ(aMap.GetHasher().Seed, 17u);
  for (size_t aKey = 0; aKey < 100; ++aKey)
  {
    ASSERT_NE(aMap.Seek(aKey), nullptr);
    EXPECT_EQ(*aMap.Seek(aKey), aKey * 3);
  }
}

TEST(NCollection_PagedDataMapTest, SupportsStandardLegacyAndItemIteration)
{
  NCollection_PagedDataMap<int, int> aMap;
  aMap.Bind(3, 30);
  aMap.Bind(7, 70);
  aMap.Bind(11, 110);

  int aValueSum = 0;
  for (int& aValue : aMap)
  {
    aValue += 1;
    aValueSum += aValue;
  }
  EXPECT_EQ(aValueSum, 213);

  int aKeySum = 0;
  for (auto [aKey, aValue] : aMap.Items())
  {
    aKeySum += aKey;
    aValue += 1;
  }
  EXPECT_EQ(aKeySum, 21);

  const NCollection_PagedDataMap<int, int>& aConstMap = aMap;
  EXPECT_EQ(std::distance(aConstMap.cbegin(), aConstMap.cend()), 3);
  NCollection_PagedDataMap<int, int>::iterator       aMutableIterator = aMap.begin();
  NCollection_PagedDataMap<int, int>::const_iterator aConstIterator;
  aConstIterator = aMutableIterator;
  EXPECT_EQ(aConstIterator.Key(), aMutableIterator.Key());
  EXPECT_EQ(*aConstIterator, *aMutableIterator);
  int aConstSum = 0;
  for (const auto [aKey, aValue] : aConstMap.Items())
  {
    aConstSum += aKey + aValue;
  }
  EXPECT_EQ(aConstSum, 237);
  *aMutableIterator += 10;

  NCollection_PagedDataMap<int, int>::Iterator anIterator(aConstMap);
  ASSERT_TRUE(anIterator.More());
  const int aFirstKey = anIterator.Key();
  anIterator.Next();
  anIterator.Reset();
  EXPECT_EQ(anIterator.Key(), aFirstKey);

  NCollection_PagedDataMap<int, int>::Iterator anEnd;
  EXPECT_THROW((void)anEnd.Key(), Standard_NoSuchObject);
  EXPECT_THROW((void)anEnd.Value(), Standard_NoSuchObject);
}

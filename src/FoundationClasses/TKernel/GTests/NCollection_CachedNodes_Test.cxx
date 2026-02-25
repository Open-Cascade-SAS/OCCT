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

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DoubleMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_OrderedDataMap.hxx>
#include <NCollection_OrderedMap.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Transient.hxx>

#include <gtest/gtest.h>

namespace
{

//! A counting allocator that tracks the number of Allocate and Free calls.
class CountingAllocator : public NCollection_BaseAllocator
{
public:
  CountingAllocator()
      : myAllocCount(0),
        myFreeCount(0)
  {
  }

  void* Allocate(const size_t theSize) override
  {
    ++myAllocCount;
    return NCollection_BaseAllocator::CommonBaseAllocator()->Allocate(theSize);
  }

  void Free(void* theAddress) override
  {
    ++myFreeCount;
    NCollection_BaseAllocator::CommonBaseAllocator()->Free(theAddress);
  }

  int AllocCount() const { return myAllocCount; }

  int FreeCount() const { return myFreeCount; }

  void ResetCounts()
  {
    myAllocCount = 0;
    myFreeCount  = 0;
  }

  DEFINE_STANDARD_RTTI_INLINE(CountingAllocator, NCollection_BaseAllocator)

private:
  int myAllocCount;
  int myFreeCount;
};

} // namespace

// =======================================================================
// DataMap: UnBind + Bind reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, DataMap_UnBind_Bind_ReusesNode)
{
  Handle(CountingAllocator)     anAlloc = new CountingAllocator();
  NCollection_DataMap<int, int> aMap(1, anAlloc);

  aMap.Bind(1, 100);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // UnBind caches the node - no free
  aMap.UnBind(1);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Bind should reuse the cached node - no alloc
  aMap.Bind(2, 200);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Find(2), 200);
}

// =======================================================================
// DataMap: Multiple UnBind + Bind cycles reuse cached nodes
// =======================================================================
TEST(NCollection_CachedNodesTest, DataMap_MultipleUnBind_ReusesNodes)
{
  Handle(CountingAllocator)     anAlloc = new CountingAllocator();
  NCollection_DataMap<int, int> aMap(1, anAlloc);

  // Populate 10 entries
  for (int i = 0; i < 10; ++i)
  {
    aMap.Bind(i, i * 10);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 10);
  anAlloc->ResetCounts();

  // UnBind all 10 - nodes cached, no frees
  for (int i = 0; i < 10; ++i)
  {
    aMap.UnBind(i);
  }
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Rebind 10 new entries - all from cache
  for (int i = 100; i < 110; ++i)
  {
    aMap.Bind(i, i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Extent(), 10);

  // Verify values
  for (int i = 100; i < 110; ++i)
  {
    EXPECT_EQ(aMap.Find(i), i);
  }
}

// =======================================================================
// DataMap: Clear(true) frees cached nodes from prior UnBind
// =======================================================================
TEST(NCollection_CachedNodesTest, DataMap_ClearTrue_FreesCachedFromUnBind)
{
  Handle(CountingAllocator)     anAlloc = new CountingAllocator();
  NCollection_DataMap<int, int> aMap(1, anAlloc);

  aMap.Bind(1, 10);
  aMap.Bind(2, 20);
  EXPECT_EQ(anAlloc->AllocCount(), 2);

  // UnBind one - cached
  aMap.UnBind(1);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Clear(true) should free the active node (key=2) + the cached node (key=1)
  aMap.Clear(true);
  EXPECT_EQ(anAlloc->FreeCount(), 2);
}

// =======================================================================
// Map: Remove + Add reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, Map_Remove_Add_ReusesNode)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  NCollection_Map<int>      aMap(1, anAlloc);

  aMap.Add(42);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // Remove caches the node - no free
  aMap.Remove(42);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Add should reuse the cached node - no alloc
  aMap.Add(99);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_TRUE(aMap.Contains(99));
}

// =======================================================================
// List: RemoveFirst + Append reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, List_RemoveFirst_Append_ReusesNode)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  NCollection_List<int>     aList(anAlloc);

  aList.Append(10);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // RemoveFirst caches the node - no free
  aList.RemoveFirst();
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Append should reuse the cached node - no alloc
  aList.Append(20);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aList.First(), 20);
}

// =======================================================================
// List: Multiple Remove via iterator + repopulate reuses cached nodes
// =======================================================================
TEST(NCollection_CachedNodesTest, List_RemoveIterator_ReusesNodes)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  NCollection_List<int>     aList(anAlloc);

  for (int i = 0; i < 50; ++i)
  {
    aList.Append(i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 50);
  anAlloc->ResetCounts();

  // Remove all via iterator - nodes cached, no frees
  for (NCollection_List<int>::Iterator anIt(aList); anIt.More();)
  {
    aList.Remove(anIt);
  }
  EXPECT_TRUE(aList.IsEmpty());
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Repopulate - should reuse all 50 cached nodes
  for (int i = 0; i < 50; ++i)
  {
    aList.Append(i * 2);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aList.Extent(), 50);
}

// =======================================================================
// Sequence: Remove + Append reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, Sequence_Remove_Append_ReusesNode)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  NCollection_Sequence<int> aSeq(anAlloc);

  aSeq.Append(7);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // Remove caches the node - no free
  aSeq.Remove(1);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Append should reuse the cached node - no alloc
  aSeq.Append(14);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aSeq.First(), 14);
}

// =======================================================================
// Sequence: Multiple Remove + repopulate reuses cached nodes
// =======================================================================
TEST(NCollection_CachedNodesTest, Sequence_MultipleRemove_ReusesNodes)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  NCollection_Sequence<int> aSeq(anAlloc);

  for (int i = 1; i <= 30; ++i)
  {
    aSeq.Append(i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 30);
  anAlloc->ResetCounts();

  // Remove all one by one (always index 1) - nodes cached
  for (int i = 0; i < 30; ++i)
  {
    aSeq.Remove(1);
  }
  EXPECT_TRUE(aSeq.IsEmpty());
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Repopulate - should reuse all 30 cached nodes
  for (int i = 1; i <= 30; ++i)
  {
    aSeq.Append(i * 3);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aSeq.Length(), 30);

  for (int i = 1; i <= 30; ++i)
  {
    EXPECT_EQ(aSeq(i), i * 3);
  }
}

// =======================================================================
// DataMap: Move constructor transfers cache from UnBind
// =======================================================================
TEST(NCollection_CachedNodesTest, DataMap_MoveConstructor_TransfersCache)
{
  Handle(CountingAllocator)     anAlloc = new CountingAllocator();
  NCollection_DataMap<int, int> aMap(1, anAlloc);

  for (int i = 0; i < 10; ++i)
  {
    aMap.Bind(i, i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 10);

  // UnBind all to build cache
  for (int i = 0; i < 10; ++i)
  {
    aMap.UnBind(i);
  }
  EXPECT_EQ(anAlloc->FreeCount(), 0);
  anAlloc->ResetCounts();

  // Move construct - cache should transfer
  NCollection_DataMap<int, int> aMap2(std::move(aMap));

  // Repopulate aMap2 - should reuse all 10 cached nodes
  for (int i = 0; i < 10; ++i)
  {
    aMap2.Bind(i, i * 5);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap2.Extent(), 10);
}

// =======================================================================
// DataMap: Exchange swaps cache
// =======================================================================
TEST(NCollection_CachedNodesTest, DataMap_Exchange_SwapsCache)
{
  Handle(CountingAllocator) anAlloc1 = new CountingAllocator();
  Handle(CountingAllocator) anAlloc2 = new CountingAllocator();

  NCollection_DataMap<int, int> aMap1(1, anAlloc1);
  NCollection_DataMap<int, int> aMap2(1, anAlloc2);

  // Populate and cache 5 nodes via UnBind in aMap1
  for (int i = 0; i < 5; ++i)
  {
    aMap1.Bind(i, i);
  }
  EXPECT_EQ(anAlloc1->AllocCount(), 5);
  for (int i = 0; i < 5; ++i)
  {
    aMap1.UnBind(i);
  }
  EXPECT_EQ(anAlloc1->FreeCount(), 0);

  // Populate aMap2 (no cache)
  aMap2.Bind(100, 100);
  EXPECT_EQ(anAlloc2->AllocCount(), 1);

  // Exchange - aMap2 gets the 5-node cache from aMap1
  aMap1.Exchange(aMap2);

  anAlloc1->ResetCounts();
  for (int i = 0; i < 5; ++i)
  {
    aMap2.Bind(i, i * 2);
  }
  EXPECT_EQ(anAlloc1->AllocCount(), 0);
  EXPECT_EQ(aMap2.Extent(), 5);
}

// =======================================================================
// List: Move constructor transfers cache from RemoveFirst
// =======================================================================
TEST(NCollection_CachedNodesTest, List_MoveConstructor_TransfersCache)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  NCollection_List<int>     aList(anAlloc);

  for (int i = 0; i < 10; ++i)
  {
    aList.Append(i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 10);

  // RemoveFirst all to build cache
  for (int i = 0; i < 10; ++i)
  {
    aList.RemoveFirst();
  }
  EXPECT_EQ(anAlloc->FreeCount(), 0);
  anAlloc->ResetCounts();

  // Move construct - cache should transfer
  NCollection_List<int> aList2(std::move(aList));

  // Repopulate - should reuse all 10 cached nodes
  for (int i = 0; i < 10; ++i)
  {
    aList2.Append(i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aList2.Extent(), 10);
}

// =======================================================================
// DoubleMap: UnBind1 + Bind reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, DoubleMap_UnBind1_Bind_ReusesNode)
{
  Handle(CountingAllocator)       anAlloc = new CountingAllocator();
  NCollection_DoubleMap<int, int> aMap(1, anAlloc);

  aMap.Bind(1, 100);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // UnBind1 caches the node - no free
  aMap.UnBind1(1);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Bind should reuse the cached node - no alloc
  aMap.Bind(2, 200);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Find1(2), 200);
  EXPECT_EQ(aMap.Find2(200), 2);
}

// =======================================================================
// DoubleMap: UnBind2 + Bind reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, DoubleMap_UnBind2_Bind_ReusesNode)
{
  Handle(CountingAllocator)       anAlloc = new CountingAllocator();
  NCollection_DoubleMap<int, int> aMap(1, anAlloc);

  aMap.Bind(1, 100);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // UnBind2 caches the node - no free
  aMap.UnBind2(100);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Bind should reuse the cached node - no alloc
  aMap.Bind(2, 200);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Find1(2), 200);
}

// =======================================================================
// IndexedMap: RemoveLast + Add reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, IndexedMap_RemoveLast_Add_ReusesNode)
{
  Handle(CountingAllocator)   anAlloc = new CountingAllocator();
  NCollection_IndexedMap<int> aMap(1, anAlloc);

  aMap.Add(42);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // RemoveLast caches the node - no free
  aMap.RemoveLast();
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Add should reuse the cached node - no alloc
  aMap.Add(99);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_TRUE(aMap.Contains(99));
  EXPECT_EQ(aMap.FindKey(1), 99);
}

// =======================================================================
// IndexedMap: RemoveFromIndex + Add reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, IndexedMap_RemoveFromIndex_Add_ReusesNode)
{
  Handle(CountingAllocator)   anAlloc = new CountingAllocator();
  NCollection_IndexedMap<int> aMap(1, anAlloc);

  for (int i = 0; i < 5; ++i)
  {
    aMap.Add(i * 10);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 5);
  anAlloc->ResetCounts();

  // RemoveFromIndex caches the node - no free
  aMap.RemoveFromIndex(3);
  EXPECT_EQ(anAlloc->FreeCount(), 0);
  EXPECT_EQ(aMap.Extent(), 4);

  // Add should reuse the cached node - no alloc
  aMap.Add(999);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_TRUE(aMap.Contains(999));
}

// =======================================================================
// IndexedDataMap: RemoveLast + Add reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, IndexedDataMap_RemoveLast_Add_ReusesNode)
{
  Handle(CountingAllocator)            anAlloc = new CountingAllocator();
  NCollection_IndexedDataMap<int, int> aMap(1, anAlloc);

  aMap.Add(42, 420);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // RemoveLast caches the node - no free
  aMap.RemoveLast();
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Add should reuse the cached node - no alloc
  aMap.Add(99, 990);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_TRUE(aMap.Contains(99));
  EXPECT_EQ(aMap.FindFromKey(99), 990);
}

// =======================================================================
// IndexedDataMap: RemoveKey + Add reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, IndexedDataMap_RemoveKey_Add_ReusesNode)
{
  Handle(CountingAllocator)            anAlloc = new CountingAllocator();
  NCollection_IndexedDataMap<int, int> aMap(1, anAlloc);

  for (int i = 0; i < 5; ++i)
  {
    aMap.Add(i, i * 100);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 5);
  anAlloc->ResetCounts();

  // RemoveKey caches the node - no free
  aMap.RemoveKey(2);
  EXPECT_EQ(anAlloc->FreeCount(), 0);
  EXPECT_EQ(aMap.Extent(), 4);

  // Add should reuse the cached node - no alloc
  aMap.Add(99, 9900);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_TRUE(aMap.Contains(99));
  EXPECT_EQ(aMap.FindFromKey(99), 9900);
}

// =======================================================================
// OrderedMap: Remove + Add reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, OrderedMap_Remove_Add_ReusesNode)
{
  Handle(CountingAllocator)   anAlloc = new CountingAllocator();
  NCollection_OrderedMap<int> aMap(1, anAlloc);

  aMap.Add(42);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // Remove caches the node - no free
  aMap.Remove(42);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Add should reuse the cached node - no alloc
  aMap.Add(99);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_TRUE(aMap.Contains(99));
}

// =======================================================================
// OrderedMap: Multiple Remove + Add reuses cached nodes
// =======================================================================
TEST(NCollection_CachedNodesTest, OrderedMap_MultipleRemove_ReusesNodes)
{
  Handle(CountingAllocator)   anAlloc = new CountingAllocator();
  NCollection_OrderedMap<int> aMap(1, anAlloc);

  for (int i = 0; i < 10; ++i)
  {
    aMap.Add(i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 10);
  anAlloc->ResetCounts();

  // Remove all - nodes cached, no frees
  for (int i = 0; i < 10; ++i)
  {
    aMap.Remove(i);
  }
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Re-add - should reuse all 10 cached nodes
  for (int i = 100; i < 110; ++i)
  {
    aMap.Add(i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Extent(), 10);
}

// =======================================================================
// OrderedDataMap: UnBind + Bind reuses cached node
// =======================================================================
TEST(NCollection_CachedNodesTest, OrderedDataMap_UnBind_Bind_ReusesNode)
{
  Handle(CountingAllocator)            anAlloc = new CountingAllocator();
  NCollection_OrderedDataMap<int, int> aMap(1, anAlloc);

  aMap.Bind(1, 100);
  EXPECT_EQ(anAlloc->AllocCount(), 1);
  anAlloc->ResetCounts();

  // UnBind caches the node - no free
  aMap.UnBind(1);
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Bind should reuse the cached node - no alloc
  aMap.Bind(2, 200);
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Find(2), 200);
}

// =======================================================================
// OrderedDataMap: Multiple UnBind + Bind reuses cached nodes
// =======================================================================
TEST(NCollection_CachedNodesTest, OrderedDataMap_MultipleUnBind_ReusesNodes)
{
  Handle(CountingAllocator)            anAlloc = new CountingAllocator();
  NCollection_OrderedDataMap<int, int> aMap(1, anAlloc);

  for (int i = 0; i < 10; ++i)
  {
    aMap.Bind(i, i * 10);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 10);
  anAlloc->ResetCounts();

  // UnBind all - nodes cached, no frees
  for (int i = 0; i < 10; ++i)
  {
    aMap.UnBind(i);
  }
  EXPECT_EQ(anAlloc->FreeCount(), 0);

  // Rebind - should reuse all 10 cached nodes
  for (int i = 100; i < 110; ++i)
  {
    aMap.Bind(i, i);
  }
  EXPECT_EQ(anAlloc->AllocCount(), 0);
  EXPECT_EQ(aMap.Extent(), 10);

  // Verify values
  for (int i = 100; i < 110; ++i)
  {
    EXPECT_EQ(aMap.Find(i), i);
  }
}

// =======================================================================
// Destructor frees all cached nodes from UnBind
// =======================================================================
TEST(NCollection_CachedNodesTest, Destructor_FreesAllCachedNodes)
{
  Handle(CountingAllocator) anAlloc = new CountingAllocator();
  {
    NCollection_DataMap<int, int> aMap(1, anAlloc);
    for (int i = 0; i < 20; ++i)
    {
      aMap.Bind(i, i);
    }
    EXPECT_EQ(anAlloc->AllocCount(), 20);

    // UnBind all to build cache of 20 nodes
    for (int i = 0; i < 20; ++i)
    {
      aMap.UnBind(i);
    }
    EXPECT_EQ(anAlloc->FreeCount(), 0);
    // Destructor should free all 20 cached nodes
  }
  EXPECT_EQ(anAlloc->FreeCount(), 20);
}

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

#include <BRepGraph_Cache.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_ProgramError.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace
{

class TestCache : public BRepGraph_Cache
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TestCache, BRepGraph_Cache)

  TestCache(const Standard_GUID& theID, const TCollection_AsciiString& theName)
      : myID(theID),
        myName(theName)
  {
  }

  const Standard_GUID& ID() const override { return myID; }

  const TCollection_AsciiString& Name() const override { return myName; }

  bool Attached() const noexcept { return IsAttached(); }

private:
  Standard_GUID           myID;
  TCollection_AsciiString myName;
};

class EntryTestCache : public BRepGraph_Cache
{
public:
  DEFINE_STANDARD_RTTI_INLINE(EntryTestCache, BRepGraph_Cache)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("f1f2f3f4-1234-4321-8888-010203040506");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("EntryTestCache");
    return THE_NAME;
  }

  bool Attached() const noexcept { return IsAttached(); }

  bool SetNodeOwn(const BRepGraph_NodeId theNode, const int theValue)
  {
    NodeValue anEntry;
    if (!anEntry.BindOwnGen(*this, theNode))
    {
      return false;
    }
    anEntry.Value = theValue;
    set(myNodeOwnValues, theNode, anEntry);
    return true;
  }

  bool GetNodeOwn(const BRepGraph_NodeId theNode, int& theValue) const
  {
    const NodeValue* anEntry = myNodeOwnValues.Seek(theNode);
    if (anEntry == nullptr || !anEntry->IsFreshOwn(*this, theNode))
    {
      return false;
    }
    theValue = anEntry->Value;
    return true;
  }

  bool SetNodeSubtree(const BRepGraph_NodeId theNode, const int theValue)
  {
    NodeValue anEntry;
    if (!anEntry.BindSubtreeGen(*this, theNode))
    {
      return false;
    }
    anEntry.Value = theValue;
    set(myNodeSubtreeValues, theNode, anEntry);
    return true;
  }

  bool GetNodeSubtree(const BRepGraph_NodeId theNode, int& theValue) const
  {
    const NodeValue* anEntry = myNodeSubtreeValues.Seek(theNode);
    if (anEntry == nullptr || !anEntry->IsFreshSubtree(*this, theNode))
    {
      return false;
    }
    theValue = anEntry->Value;
    return true;
  }

  bool SetRefOwn(const BRepGraph_RefId theRef, const int theValue)
  {
    RefValue anEntry;
    if (!anEntry.BindOwnGen(*this, theRef))
    {
      return false;
    }
    anEntry.Value = theValue;
    set(myRefOwnValues, theRef, anEntry);
    return true;
  }

  bool GetRefOwn(const BRepGraph_RefId theRef, int& theValue) const
  {
    const RefValue* anEntry = myRefOwnValues.Seek(theRef);
    if (anEntry == nullptr || !anEntry->IsFreshOwn(*this, theRef))
    {
      return false;
    }
    theValue = anEntry->Value;
    return true;
  }

  bool SetItemOwn(const BRepGraph_ItemId theItem, const int theValue)
  {
    ItemValue anEntry;
    if (!anEntry.BindOwnGen(*this, theItem))
    {
      return false;
    }
    anEntry.Value = theValue;
    set(myItemOwnValues, theItem, anEntry);
    return true;
  }

  bool GetItemOwn(const BRepGraph_ItemId theItem, int& theValue) const
  {
    const ItemValue* anEntry = myItemOwnValues.Seek(theItem);
    if (anEntry == nullptr || !anEntry->IsFreshOwn(*this, theItem))
    {
      return false;
    }
    theValue = anEntry->Value;
    return true;
  }

  bool ResolveChildForTest(const BRepGraph_RefId theRef, BRepGraph_NodeId& theNode) const
  {
    return ResolveActiveRefChild(theRef, theNode);
  }

  void Clear() noexcept override
  {
    ++myClearCount;
    myNodeOwnValues.Clear();
    myNodeSubtreeValues.Clear();
    myRefOwnValues.Clear();
    myItemOwnValues.Clear();
  }

  int ClearCount() const noexcept { return myClearCount; }

  int NbValues() const noexcept
  {
    return static_cast<int>(myNodeOwnValues.Size() + myNodeSubtreeValues.Size()
                            + myRefOwnValues.Size() + myItemOwnValues.Size());
  }

private:
  struct NodeValue : NodeEntry
  {
    int Value = 0;
  };

  struct RefValue : RefEntry
  {
    int Value = 0;
  };

  struct ItemValue : ItemEntry
  {
    int Value = 0;
  };

  template <typename KeyT, typename ValueT>
  static void set(NCollection_DataMap<KeyT, ValueT>& theMap,
                  const KeyT                         theKey,
                  const ValueT&                      theValue)
  {
    if (theMap.IsBound(theKey))
    {
      theMap.ChangeFind(theKey) = theValue;
    }
    else
    {
      theMap.Bind(theKey, theValue);
    }
  }

  NCollection_DataMap<BRepGraph_NodeId, NodeValue> myNodeOwnValues;
  NCollection_DataMap<BRepGraph_NodeId, NodeValue> myNodeSubtreeValues;
  NCollection_DataMap<BRepGraph_RefId, RefValue>   myRefOwnValues;
  NCollection_DataMap<BRepGraph_ItemId, ItemValue> myItemOwnValues;
  int                                              myClearCount = 0;
};

class BlockingEnsureCache : public BRepGraph_Cache
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BlockingEnsureCache, BRepGraph_Cache)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("f2f3f4f5-2345-5432-9999-020304050607");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("BlockingEnsureCache");
    return THE_NAME;
  }

  bool Attached() const noexcept { return IsAttached(); }

  static void StartBlocking() noexcept
  {
    THE_ENTERED.store(false, std::memory_order_release);
    THE_RELEASE.store(false, std::memory_order_release);
    THE_BLOCK.store(true, std::memory_order_release);
  }

  static void ReleaseBlocking() noexcept
  {
    THE_RELEASE.store(true, std::memory_order_release);
    THE_BLOCK.store(false, std::memory_order_release);
  }

  static bool IsEntered() noexcept { return THE_ENTERED.load(std::memory_order_acquire); }

protected:
  void OnAttached() noexcept override
  {
    if (!THE_BLOCK.load(std::memory_order_acquire))
    {
      return;
    }

    THE_ENTERED.store(true, std::memory_order_release);
    while (!THE_RELEASE.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
  }

private:
  inline static std::atomic<bool> THE_BLOCK{false};
  inline static std::atomic<bool> THE_ENTERED{false};
  inline static std::atomic<bool> THE_RELEASE{false};
};

class LifecycleTestCache : public BRepGraph_Cache
{
public:
  DEFINE_STANDARD_RTTI_INLINE(LifecycleTestCache, BRepGraph_Cache)

  LifecycleTestCache(const Standard_GUID& theID, BRepGraph_CacheRegistry* theRegistry)
      : myID(theID),
        myRegistry(theRegistry)
  {
  }

  const Standard_GUID& ID() const override { return myID; }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("LifecycleTestCache");
    return THE_NAME;
  }

  bool Attached() const noexcept { return IsAttached(); }
  int  AttachedCount() const noexcept { return myAttachedCount; }
  int  DetachedCount() const noexcept { return myDetachedCount; }
  int  ClearCount() const noexcept { return myClearCount; }
  const BRepGraph* CallbackGraph() const noexcept { return myCallbackGraph; }
  bool RegisteredCacheWasAttached() const noexcept { return myRegisteredCacheWasAttached; }
  void SetReRegisterOnDetach(const bool theValue) noexcept { myToReRegister = theValue; }
  void SetCacheToRegisterOnDetach(const occ::handle<LifecycleTestCache>& theCache)
  {
    myCacheToRegister = theCache;
  }

  void Clear() noexcept override { ++myClearCount; }

protected:
  void OnAttached() noexcept override
  {
    ++myAttachedCount;
    myCallbackGraph = AttachedGraph();
    if (myRegistry != nullptr)
    {
      [[maybe_unused]] const uint32_t aCacheCount = myRegistry->NbCaches();
    }
  }

  void OnDetached() noexcept override
  {
    ++myDetachedCount;
    myCallbackGraph = AttachedGraph();
    if (myRegistry != nullptr)
    {
      [[maybe_unused]] const uint32_t aCacheCount = myRegistry->NbCaches();
      if (myToReRegister)
      {
        myRegistry->RegisterCache(this);
      }
      if (!myCacheToRegister.IsNull())
      {
        myRegistry->RegisterCache(myCacheToRegister);
        myRegisteredCacheWasAttached = myCacheToRegister->Attached();
      }
    }
  }

private:
  Standard_GUID            myID;
  BRepGraph_CacheRegistry* myRegistry      = nullptr;
  const BRepGraph*         myCallbackGraph = nullptr;
  int                      myAttachedCount = 0;
  int                      myDetachedCount = 0;
  int                      myClearCount    = 0;
  bool                     myToReRegister = false;
  bool                     myRegisteredCacheWasAttached = false;
  occ::handle<LifecycleTestCache> myCacheToRegister;
};

static BRepGraph makeBoxGraph()
{
  BRepGraph                                            aGraph;
  BRepPrimAPI_MakeBox                                  aBoxMaker(10.0, 20.0, 30.0);
  [[maybe_unused]] const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(aBoxMaker.Shape());
  return aGraph;
}

} // namespace

TEST(BRepGraph_CacheRegistryTest, Register_SameGUID_SameSlot)
{
  const Standard_GUID                aGUID("a1b2c3d4-1111-2222-3333-444455556666");
  const occ::handle<TestCache> aKind1 = new TestCache(aGUID, "SameGUID");
  const occ::handle<TestCache> aKind2 = new TestCache(aGUID, "SameGUID");
  BRepGraph_CacheRegistry            aRegistry;
  const uint32_t                     aSlot1 = aRegistry.RegisterCache(aKind1);
  const uint32_t                     aSlot2 = aRegistry.RegisterCache(aKind2);
  EXPECT_EQ(aSlot1, aSlot2);
  EXPECT_FALSE(aKind2->Attached());
}

TEST(BRepGraph_CacheRegistryTest, Register_DifferentGUID_DifferentSlot)
{
  const occ::handle<BRepGraph_Cache> aKind1 =
    new TestCache(Standard_GUID("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0001"), "Kind1");
  const occ::handle<BRepGraph_Cache> aKind2 =
    new TestCache(Standard_GUID("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0002"), "Kind2");
  BRepGraph_CacheRegistry aRegistry;
  const uint32_t          aSlot1 = aRegistry.RegisterCache(aKind1);
  const uint32_t          aSlot2 = aRegistry.RegisterCache(aKind2);
  EXPECT_NE(aSlot1, aSlot2);
}

TEST(BRepGraph_CacheRegistryTest, Register_AttachedCallbackCanQueryRegistryAndRunsOnce)
{
  BRepGraph aGraph;
  BRepGraph_CacheRegistry& aRegistry = aGraph.CacheRegistry();
  const occ::handle<LifecycleTestCache> aCache = new LifecycleTestCache(
    Standard_GUID("ab46f199-ee43-4dbf-a80d-c82e39a6b65a"), &aRegistry);

  aRegistry.RegisterCache(aCache);

  EXPECT_TRUE(aCache->Attached());
  EXPECT_EQ(aCache->AttachedCount(), 1);
  EXPECT_EQ(aCache->CallbackGraph(), &aGraph);

  aRegistry.RegisterCache(aCache);
  EXPECT_EQ(aCache->AttachedCount(), 1);
}

TEST(BRepGraph_CacheRegistryTest, Register_DetachedNotifiesOnlyAfterAttach)
{
  BRepGraph aGraph;
  BRepGraph_CacheRegistry aRegistry;
  const occ::handle<LifecycleTestCache> aCache = new LifecycleTestCache(
    Standard_GUID("46650490-84af-4915-bf3a-01da3bfd9c46"), &aRegistry);

  aRegistry.RegisterCache(aCache);
  EXPECT_EQ(aCache->AttachedCount(), 0);
  EXPECT_FALSE(aCache->Attached());

  aRegistry.Attach(aGraph);
  EXPECT_EQ(aCache->AttachedCount(), 1);
  EXPECT_EQ(aCache->CallbackGraph(), &aGraph);
}

TEST(BRepGraph_CacheRegistryTest, Detach_CallbackCanQueryRegistryAndIsIdempotent)
{
  BRepGraph aGraph;
  BRepGraph_CacheRegistry aRegistry(aGraph);
  const occ::handle<LifecycleTestCache> aCache = new LifecycleTestCache(
    Standard_GUID("16aaadcc-87e4-44cb-a6ea-2cf8db56af1b"), &aRegistry);
  aRegistry.RegisterCache(aCache);

  aRegistry.Detach();
  aRegistry.Detach();

  EXPECT_FALSE(aCache->Attached());
  EXPECT_EQ(aCache->AttachedCount(), 1);
  EXPECT_EQ(aCache->DetachedCount(), 1);
  EXPECT_EQ(aCache->ClearCount(), 1);
  EXPECT_EQ(aCache->CallbackGraph(), &aGraph);
}

TEST(BRepGraph_CacheRegistryTest, Detach_ReentrantRegistrationRemainsDetached)
{
  BRepGraph aGraph;
  BRepGraph_CacheRegistry aRegistry(aGraph);
  const occ::handle<LifecycleTestCache> aFirst = new LifecycleTestCache(
    Standard_GUID("2a803c5f-3f37-46af-9731-aa5d1fce39e5"), &aRegistry);
  const occ::handle<LifecycleTestCache> aSecond = new LifecycleTestCache(
    Standard_GUID("ca24a971-65cf-437a-85f4-197261c8cffd"), &aRegistry);
  aFirst->SetCacheToRegisterOnDetach(aSecond);
  aRegistry.RegisterCache(aFirst);

  aRegistry.Detach();

  EXPECT_EQ(aRegistry.FindCache(aSecond->ID()), aSecond);
  EXPECT_FALSE(aFirst->Attached());
  EXPECT_FALSE(aSecond->Attached());
  EXPECT_EQ(aFirst->DetachedCount(), 1);
  EXPECT_FALSE(aFirst->RegisteredCacheWasAttached());
  EXPECT_EQ(aSecond->AttachedCount(), 0);
  EXPECT_EQ(aSecond->DetachedCount(), 0);
}

TEST(BRepGraph_CacheRegistryTest, Register_ReplacementTransitionsEachCacheOnce)
{
  BRepGraph aGraph;
  BRepGraph_CacheRegistry& aRegistry = aGraph.CacheRegistry();
  const Standard_GUID aGUID("312bb54d-7910-45f1-8ba7-ae412fae84bd");
  const occ::handle<LifecycleTestCache> aFirst = new LifecycleTestCache(aGUID, &aRegistry);
  const occ::handle<LifecycleTestCache> aSecond = new LifecycleTestCache(aGUID, &aRegistry);

  aRegistry.RegisterCache(aFirst);
  aRegistry.RegisterCache(aSecond);

  EXPECT_EQ(aFirst->AttachedCount(), 1);
  EXPECT_EQ(aFirst->DetachedCount(), 1);
  EXPECT_FALSE(aFirst->Attached());
  EXPECT_EQ(aSecond->AttachedCount(), 1);
  EXPECT_EQ(aSecond->DetachedCount(), 0);
  EXPECT_TRUE(aSecond->Attached());
}

TEST(BRepGraph_CacheRegistryTest, GraphDestructionNotifiesLifecycleExactlyOnce)
{
  occ::handle<LifecycleTestCache> aCache;
  {
    BRepGraph aGraph;
    BRepGraph_CacheRegistry& aRegistry = aGraph.CacheRegistry();
    aCache = new LifecycleTestCache(
      Standard_GUID("45ee2fbf-c9a4-4ad8-88d4-79ab0ee78de7"), &aRegistry);
    aRegistry.RegisterCache(aCache);
    ASSERT_EQ(aCache->AttachedCount(), 1);
  }

  EXPECT_EQ(aCache->DetachedCount(), 1);
  EXPECT_EQ(aCache->ClearCount(), 1);
  EXPECT_FALSE(aCache->Attached());
}

TEST(BRepGraph_CacheRegistryTest, UnregisterAllowsReentrantRegistrationFromDetachCallback)
{
  BRepGraph aGraph;
  BRepGraph_CacheRegistry& aRegistry = aGraph.CacheRegistry();
  const occ::handle<LifecycleTestCache> aCache = new LifecycleTestCache(
    Standard_GUID("8f9674ae-8fdb-48b3-8535-090f8fce1d93"), &aRegistry);
  aRegistry.RegisterCache(aCache);
  aCache->SetReRegisterOnDetach(true);

  aRegistry.UnregisterCache(aCache->ID());

  EXPECT_EQ(aRegistry.FindCache(aCache->ID()), aCache);
  EXPECT_TRUE(aCache->Attached());
  EXPECT_EQ(aCache->AttachedCount(), 2);
  EXPECT_EQ(aCache->DetachedCount(), 1);
}

TEST(BRepGraph_CacheRegistryTest, FindSlot_ByGUID_ReturnsCorrectSlot)
{
  const occ::handle<BRepGraph_Cache> aKind =
    new TestCache(Standard_GUID("c1c2c3c4-1111-2222-3333-aabbccddeeff"), "FindByGUID");
  BRepGraph_CacheRegistry aRegistry;
  const uint32_t          aExpectedSlot = aRegistry.RegisterCache(aKind);

  uint32_t aFoundSlot = 0;
  ASSERT_TRUE(aRegistry.FindSlot(aKind->ID(), aFoundSlot));
  EXPECT_EQ(aFoundSlot, aExpectedSlot);
}

TEST(BRepGraph_CacheRegistryTest, FindCache_BySlot_ReturnsCorrectDescriptor)
{
  const occ::handle<BRepGraph_Cache> aKind =
    new TestCache(Standard_GUID("d1d2d3d4-5555-6666-7777-888899990000"), "FindBySlot");
  BRepGraph_CacheRegistry aRegistry;
  const uint32_t          aSlot = aRegistry.RegisterCache(aKind);

  const occ::handle<BRepGraph_Cache> aFound = aRegistry.Cache(aSlot);
  ASSERT_FALSE(aFound.IsNull());
  EXPECT_TRUE(aFound->ID() == aKind->ID());
  EXPECT_TRUE(aFound->Name().IsEqual("FindBySlot"));
}

TEST(BRepGraph_CacheRegistryTest, Cache_InvalidSlot_ReturnsNull)
{
  BRepGraph_CacheRegistry aRegistry;
  EXPECT_TRUE(aRegistry.Cache(0).IsNull());
}

TEST(BRepGraph_CacheRegistryTest, Ensure_IsConcurrentCreationSafe)
{
  constexpr int THE_NB_THREADS    = 8;
  constexpr int THE_NB_ITERATIONS = 64;

  BRepGraph aGraph;
  std::atomic<int>  aReadyCount{0};
  std::atomic<bool> toStart{false};
  std::atomic<int>  aFailureCount{0};
  std::vector<occ::handle<EntryTestCache>> aResults(static_cast<size_t>(THE_NB_THREADS));
  std::vector<std::thread>                 aThreads;
  aThreads.reserve(static_cast<size_t>(THE_NB_THREADS));

  for (int aThreadIdx = 0; aThreadIdx < THE_NB_THREADS; ++aThreadIdx)
  {
    aThreads.emplace_back([&, aThreadIdx]() {
      aReadyCount.fetch_add(1);
      while (!toStart.load())
      {
        std::this_thread::yield();
      }

      for (int anIteration = 0; anIteration < THE_NB_ITERATIONS; ++anIteration)
      {
        occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();
        if (aCache.IsNull())
        {
          aFailureCount.fetch_add(1);
          continue;
        }
        if (!aCache->Attached())
        {
          aFailureCount.fetch_add(1);
        }
        if (anIteration == 0)
        {
          aResults[static_cast<size_t>(aThreadIdx)] = aCache;
        }
        else if (aResults[static_cast<size_t>(aThreadIdx)] != aCache)
        {
          aFailureCount.fetch_add(1);
        }
      }
    });
  }

  while (aReadyCount.load() != THE_NB_THREADS)
  {
    std::this_thread::yield();
  }
  toStart.store(true);

  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }

  EXPECT_EQ(aFailureCount.load(), 0);
  ASSERT_FALSE(aResults[0].IsNull());
  for (const occ::handle<EntryTestCache>& aCache : aResults)
  {
    EXPECT_EQ(aCache, aResults[0]);
  }
  EXPECT_EQ(aGraph.CacheRegistry().NbCaches(), 1u);
  uint32_t aSlot = 42u;
  ASSERT_TRUE(aGraph.CacheRegistry().FindSlot(EntryTestCache::GetID(), aSlot));
  EXPECT_EQ(aSlot, 0u);
}

TEST(BRepGraph_CacheRegistryTest, Ensure_ConcurrentLookupSeesPublishedAttachment)
{
  BRepGraph aGraph;
  BlockingEnsureCache::StartBlocking();

  std::thread aCreator([&]() {
    [[maybe_unused]] const occ::handle<BlockingEnsureCache> aCache =
      aGraph.CacheRegistry().Ensure<BlockingEnsureCache>();
  });

  const auto anAttachDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  while (!BlockingEnsureCache::IsEntered()
         && std::chrono::steady_clock::now() < anAttachDeadline)
  {
    std::this_thread::yield();
  }

  std::promise<void> aLookupStarted;
  std::future<void>  aLookupStartedFuture = aLookupStarted.get_future();
  std::promise<bool> aLookupResult;
  std::future<bool>  aLookupResultFuture = aLookupResult.get_future();
  std::thread        aLookup([&]() {
    aLookupStarted.set_value();
    bool anIsAttached = false;
    try
    {
      const occ::handle<BlockingEnsureCache> aCache =
        aGraph.CacheRegistry().Ensure<BlockingEnsureCache>();
      anIsAttached = aCache->Attached();
    }
    catch (...)
    {
      anIsAttached = false;
    }
    aLookupResult.set_value(anIsAttached);
  });

  aLookupStartedFuture.wait();
  const bool isLookupComplete =
    aLookupResultFuture.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready;

  BlockingEnsureCache::ReleaseBlocking();
  aCreator.join();
  aLookup.join();

  EXPECT_TRUE(BlockingEnsureCache::IsEntered());
  EXPECT_TRUE(isLookupComplete);
  EXPECT_TRUE(aLookupResultFuture.get());
}

TEST(BRepGraph_CacheRegistryTest, SameCache_CannotUseDifferentRegistries)
{
  const occ::handle<BRepGraph_Cache> aShared =
    new TestCache(Standard_GUID("e1e2e3e4-1111-2222-3333-888899990001"), "Shared");
  const occ::handle<BRepGraph_Cache> aPrefix =
    new TestCache(Standard_GUID("e1e2e3e4-1111-2222-3333-888899990002"), "Prefix");

  BRepGraph_CacheRegistry         aRegistry1;
  BRepGraph_CacheRegistry         aRegistry2;
  const uint32_t                  aSlot1      = aRegistry1.RegisterCache(aShared);
  [[maybe_unused]] const uint32_t aPrefixSlot = aRegistry2.RegisterCache(aPrefix);

  EXPECT_EQ(aSlot1, 0u);
  EXPECT_THROW(aRegistry2.RegisterCache(aShared), Standard_ProgramError);
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistry_AttachDetachPreservesHandleOwnership)
{
  BRepGraph aGraph = makeBoxGraph();
  BRepGraph_CacheRegistry aRegistry;
  const occ::handle<EntryTestCache> aCache = new EntryTestCache;

  EXPECT_FALSE(aRegistry.IsAttached());
  aRegistry.Attach(aGraph);
  EXPECT_TRUE(aRegistry.IsAttached());
  EXPECT_TRUE(aRegistry.IsFor(aGraph));
  aRegistry.RegisterCache(aCache);
  ASSERT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 7));

  aRegistry.Detach();
  EXPECT_FALSE(aRegistry.IsAttached());
  EXPECT_FALSE(aCache->Attached());
  EXPECT_EQ(aCache->NbValues(), 0);
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistriesSameGraphHaveIndependentOwnership)
{
  BRepGraph aGraph = makeBoxGraph();
  BRepGraph_CacheRegistry aFirstRegistry(aGraph);
  BRepGraph_CacheRegistry aSecondRegistry;
  aSecondRegistry.Attach(aGraph);
  const occ::handle<EntryTestCache> aFirstCache = new EntryTestCache;
  const occ::handle<EntryTestCache> aSecondCache = new EntryTestCache;

  aFirstRegistry.RegisterCache(aFirstCache);
  aSecondRegistry.RegisterCache(aSecondCache);
  EXPECT_TRUE(aFirstCache->Attached());
  EXPECT_TRUE(aSecondCache->Attached());
  EXPECT_THROW(aSecondRegistry.RegisterCache(aFirstCache), Standard_ProgramError);
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistry_ReattachRebindsRegisteredCaches)
{
  BRepGraph aGraph = makeBoxGraph();
  BRepGraph_CacheRegistry aRegistry;
  const occ::handle<EntryTestCache> aCache = new EntryTestCache;

  aRegistry.Attach(aGraph);
  aRegistry.RegisterCache(aCache);
  ASSERT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 3));

  aRegistry.Detach();
  EXPECT_FALSE(aCache->Attached());
  EXPECT_EQ(aCache->NbValues(), 0);

  aRegistry.Attach(aGraph);
  EXPECT_TRUE(aCache->Attached());
  EXPECT_TRUE(aRegistry.IsFor(aGraph));
  EXPECT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 4));
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistry_GraphClearClearsRegisteredCaches)
{
  BRepGraph aGraph = makeBoxGraph();
  BRepGraph_CacheRegistry aRegistry(aGraph);
  const occ::handle<EntryTestCache> aCache = new EntryTestCache;
  aRegistry.RegisterCache(aCache);
  ASSERT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 8));

  aGraph.Clear();

  EXPECT_TRUE(aRegistry.IsFor(aGraph));
  EXPECT_EQ(aCache->NbValues(), 0);
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistry_GraphDestructionDetachesRegistry)
{
  BRepGraph_CacheRegistry aRegistry;
  occ::handle<EntryTestCache> aCache;
  {
    BRepGraph aGraph = makeBoxGraph();
    aRegistry.Attach(aGraph);
    aCache = new EntryTestCache;
    aRegistry.RegisterCache(aCache);
    ASSERT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 9));
  }

  EXPECT_FALSE(aRegistry.IsAttached());
  EXPECT_FALSE(aCache->Attached());
  EXPECT_EQ(aCache->NbValues(), 0);
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistry_GraphMoveRebindsRegistry)
{
  BRepGraph_CacheRegistry aRegistry;
  BRepGraph aGraph = makeBoxGraph();
  aRegistry.Attach(aGraph);
  const occ::handle<EntryTestCache> aCache = new EntryTestCache;
  aRegistry.RegisterCache(aCache);

  BRepGraph aMoved(std::move(aGraph));

  EXPECT_TRUE(aRegistry.IsFor(aMoved));
  EXPECT_EQ(aRegistry.AttachedGraph(), &aMoved);
  EXPECT_TRUE(aCache->Attached());
  EXPECT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 10));
}

TEST(BRepGraph_CacheRegistryTest, ExternalRegistry_MoveAssignmentDetachesDestinationGraph)
{
  BRepGraph aDestinationGraph = makeBoxGraph();
  BRepGraph aSourceGraph      = makeBoxGraph();
  BRepGraph_CacheRegistry aDestinationRegistry(aDestinationGraph);
  BRepGraph_CacheRegistry aSourceRegistry(aSourceGraph);

  const occ::handle<EntryTestCache> aDestinationCache = new EntryTestCache;
  const occ::handle<EntryTestCache> aSourceCache      = new EntryTestCache;
  aDestinationRegistry.RegisterCache(aDestinationCache);
  aSourceRegistry.RegisterCache(aSourceCache);
  ASSERT_TRUE(aSourceCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 11));

  aDestinationRegistry = std::move(aSourceRegistry);

  EXPECT_FALSE(aDestinationCache->Attached());
  EXPECT_TRUE(aDestinationRegistry.IsFor(aSourceGraph));
  EXPECT_FALSE(aDestinationRegistry.IsFor(aDestinationGraph));

  aDestinationGraph.Clear();
  EXPECT_EQ(aSourceCache->NbValues(), 1);
}

TEST(BRepGraph_CacheRegistryTest, Ensure_UsesEachRegistryLocalSlot)
{
  BRepGraph aPrefixedGraph;
  const occ::handle<BRepGraph_Cache> aPrefix =
    new TestCache(Standard_GUID("bf6aa7e4-3c38-43dd-98ef-623134d80001"), "Prefix");
  ASSERT_EQ(aPrefixedGraph.CacheRegistry().RegisterCache(aPrefix), 0u);

  const occ::handle<EntryTestCache> aPrefixedCache =
    aPrefixedGraph.CacheRegistry().Ensure<EntryTestCache>();
  ASSERT_FALSE(aPrefixedCache.IsNull());
  uint32_t aPrefixedSlot = 0;
  ASSERT_TRUE(aPrefixedGraph.CacheRegistry().FindSlot(EntryTestCache::GetID(), aPrefixedSlot));
  EXPECT_EQ(aPrefixedSlot, 1u);

  BRepGraph aPlainGraph;
  const occ::handle<EntryTestCache> aPlainCache =
    aPlainGraph.CacheRegistry().Ensure<EntryTestCache>();
  ASSERT_FALSE(aPlainCache.IsNull());
  uint32_t aPlainSlot = 42u;
  ASSERT_TRUE(aPlainGraph.CacheRegistry().FindSlot(EntryTestCache::GetID(), aPlainSlot));
  EXPECT_EQ(aPlainSlot, 0u);

  EXPECT_EQ(aPlainGraph.CacheRegistry().Find<EntryTestCache>(), aPlainCache);
  EXPECT_EQ(aPrefixedGraph.CacheRegistry().Ensure<EntryTestCache>(), aPrefixedCache);
}

TEST(BRepGraph_CacheRegistryTest, Find_FollowsSlotCompactionAfterUnregister)
{
  BRepGraph aGraph;
  const occ::handle<BRepGraph_Cache> aPrefix =
    new TestCache(Standard_GUID("bf6aa7e4-3c38-43dd-98ef-623134d80002"), "Prefix");
  ASSERT_EQ(aGraph.CacheRegistry().RegisterCache(aPrefix), 0u);

  const occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();
  ASSERT_FALSE(aCache.IsNull());
  uint32_t aSlot = 0;
  ASSERT_TRUE(aGraph.CacheRegistry().FindSlot(EntryTestCache::GetID(), aSlot));
  ASSERT_EQ(aSlot, 1u);

  aGraph.CacheRegistry().UnregisterCache(aPrefix->ID());

  ASSERT_TRUE(aGraph.CacheRegistry().FindSlot(EntryTestCache::GetID(), aSlot));
  EXPECT_EQ(aSlot, 0u);
  EXPECT_EQ(aGraph.CacheRegistry().Find<EntryTestCache>(), aCache);
  EXPECT_EQ(aGraph.CacheRegistry().Ensure<EntryTestCache>(), aCache);
}

TEST(BRepGraph_CacheRegistryTest, Unregister_CompactsSlotsAndUpdatesIterator)
{
  BRepGraph                          aGraph = makeBoxGraph();
  const occ::handle<BRepGraph_Cache> aFirst =
    new TestCache(Standard_GUID("c8f2fd46-7c54-42a1-a73e-83c6ae2c7401"), "First");
  const occ::handle<BRepGraph_Cache> aSecond =
    new TestCache(Standard_GUID("ac165657-f0b6-4e51-9a1b-ef9d33c12096"), "Second");

  const uint32_t aFirstSlot  = aGraph.CacheRegistry().RegisterCache(aFirst);
  const uint32_t aSecondSlot = aGraph.CacheRegistry().RegisterCache(aSecond);
  ASSERT_EQ(aFirstSlot, 0u);
  ASSERT_EQ(aSecondSlot, 1u);
  ASSERT_EQ(aGraph.CacheRegistry().NbCaches(), 2u);

  aGraph.CacheRegistry().UnregisterCache(aFirst->ID());

  EXPECT_EQ(aGraph.CacheRegistry().NbCaches(), 1u);
  uint32_t aFoundSlot = 0;
  ASSERT_TRUE(aGraph.CacheRegistry().FindSlot(aSecond->ID(), aFoundSlot));
  EXPECT_EQ(aFoundSlot, 0u);
  uint32_t aIterCount = 0;
  for (const occ::handle<BRepGraph_Cache> aCache : aGraph.CacheRegistry().CacheIter())
  {
    ASSERT_FALSE(aCache.IsNull());
    EXPECT_TRUE(aCache->ID() == aSecond->ID());
    ++aIterCount;
  }
  EXPECT_EQ(aIterCount, 1u);
}

TEST(BRepGraph_CacheRegistryTest, ResolveActiveRefChild_RejectsInvalidAndRemovedRefs)
{
  BRepGraph                   aGraph = makeBoxGraph();
  occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();

  const BRepGraph_SolidId aSolid = BRepGraph_SolidId::Start();
  ASSERT_FALSE(aGraph.Topo().Solids().Relations(aSolid).ShellRefIds.IsEmpty());
  const BRepGraph_ShellRefId aShellRef =
    aGraph.Topo().Solids().Relations(aSolid).ShellRefIds.First();

  BRepGraph_NodeId aNode;
  ASSERT_TRUE(aCache->ResolveChildForTest(BRepGraph_RefId(aShellRef), aNode));
  EXPECT_EQ(aNode.NodeKind, BRepGraph_NodeId::Kind::Shell);

  EXPECT_FALSE(aCache->ResolveChildForTest(BRepGraph_RefId::Invalid(), aNode));
  EXPECT_FALSE(
    aCache->ResolveChildForTest(BRepGraph_RefId(BRepGraph_RefId::Kind::Shell, 999999), aNode));

  aGraph.Editor().Gen().RemoveRef(aShellRef);
  EXPECT_FALSE(aCache->ResolveChildForTest(BRepGraph_RefId(aShellRef), aNode));
}

TEST(BRepGraph_CacheRegistryTest, NodeOwnEntry_MissesAfterOwnGenerationChange)
{
  BRepGraph                   aGraph = makeBoxGraph();
  occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();

  const BRepGraph_NodeId anEdge(BRepGraph_EdgeId::Start());
  ASSERT_TRUE(aCache->SetNodeOwn(anEdge, 42));

  int aValue = 0;
  EXPECT_TRUE(aCache->GetNodeOwn(anEdge, aValue));
  EXPECT_EQ(aValue, 42);

  aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start()).MarkDirty();

  EXPECT_FALSE(aCache->GetNodeOwn(anEdge, aValue));
}

TEST(BRepGraph_CacheRegistryTest, NodeSubtreeEntry_MissesAfterChildGenerationChange)
{
  BRepGraph                   aGraph = makeBoxGraph();
  occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();

  const BRepGraph_NodeId aWire(BRepGraph_WireId::Start());
  ASSERT_TRUE(aCache->SetNodeSubtree(aWire, 77));

  int aValue = 0;
  EXPECT_TRUE(aCache->GetNodeSubtree(aWire, aValue));
  EXPECT_EQ(aValue, 77);

  aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start()).MarkDirty();

  EXPECT_FALSE(aCache->GetNodeSubtree(aWire, aValue));
}

TEST(BRepGraph_CacheRegistryTest, RefOwnEntry_MissesAfterRefGenerationChange)
{
  BRepGraph                   aGraph = makeBoxGraph();
  occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();

  const BRepGraph_ProductId aProductId = BRepGraph_ProductId::Start();
  ASSERT_FALSE(aGraph.Topo().Products().Relations(aProductId).OccurrenceRefIds.IsEmpty());
  const BRepGraph_OccurrenceRefId anOccRef =
    aGraph.Topo().Products().Relations(aProductId).OccurrenceRefIds.First();
  ASSERT_TRUE(aCache->SetRefOwn(BRepGraph_RefId(anOccRef), 99));

  int aValue = 0;
  EXPECT_TRUE(aCache->GetRefOwn(BRepGraph_RefId(anOccRef), aValue));
  EXPECT_EQ(aValue, 99);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  ASSERT_TRUE(BRepGraph_Transform::MoveRef(aGraph, anOccRef, aTrsf));

  EXPECT_FALSE(aCache->GetRefOwn(BRepGraph_RefId(anOccRef), aValue));
}

TEST(BRepGraph_CacheRegistryTest, ItemOwnEntry_WorksForNodeAndRef)
{
  BRepGraph                   aGraph = makeBoxGraph();
  occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();

  const BRepGraph_ItemId aFaceItem(BRepGraph_FaceId::Start());
  ASSERT_TRUE(aCache->SetItemOwn(aFaceItem, 11));

  const BRepGraph_SolidId aSolid = BRepGraph_SolidId::Start();
  ASSERT_FALSE(aGraph.Topo().Solids().Relations(aSolid).ShellRefIds.IsEmpty());
  const BRepGraph_ShellRefId aShellRef =
    aGraph.Topo().Solids().Relations(aSolid).ShellRefIds.First();
  const BRepGraph_ItemId aRefItem(aShellRef);
  ASSERT_TRUE(aCache->SetItemOwn(aRefItem, 22));

  int aValue = 0;
  EXPECT_TRUE(aCache->GetItemOwn(aFaceItem, aValue));
  EXPECT_EQ(aValue, 11);
  EXPECT_TRUE(aCache->GetItemOwn(aRefItem, aValue));
  EXPECT_EQ(aValue, 22);
}

TEST(BRepGraph_CacheRegistryTest, Unregister_DetachesAndClearsRepresentation)
{
  BRepGraph                   aGraph = makeBoxGraph();
  occ::handle<EntryTestCache> aCache = aGraph.CacheRegistry().Ensure<EntryTestCache>();
  ASSERT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 1));
  ASSERT_EQ(aCache->NbValues(), 1);

  aGraph.CacheRegistry().UnregisterCache(EntryTestCache::GetID());

  EXPECT_FALSE(aCache->Attached());
  EXPECT_EQ(aCache->NbValues(), 0);
  EXPECT_GE(aCache->ClearCount(), 1);

  int aValue = 0;
  EXPECT_FALSE(aCache->GetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), aValue));
}

TEST(BRepGraph_CacheRegistryTest, GraphDestruction_DetachesAndClearsExternalCacheHandle)
{
  occ::handle<EntryTestCache> aCache;
  {
    BRepGraph aGraph = makeBoxGraph();
    aCache           = aGraph.CacheRegistry().Ensure<EntryTestCache>();
    ASSERT_TRUE(aCache->SetNodeOwn(BRepGraph_NodeId(BRepGraph_EdgeId::Start()), 5));
    ASSERT_TRUE(aCache->Attached());
    ASSERT_EQ(aCache->NbValues(), 1);
  }

  ASSERT_FALSE(aCache.IsNull());
  EXPECT_FALSE(aCache->Attached());
  EXPECT_EQ(aCache->NbValues(), 0);
  EXPECT_GE(aCache->ClearCount(), 1);
}

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
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <tuple>

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
  const occ::handle<BRepGraph_Cache> aKind1 = new TestCache(aGUID, "SameGUID");
  const occ::handle<BRepGraph_Cache> aKind2 = new TestCache(aGUID, "SameGUID");
  BRepGraph_CacheRegistry            aRegistry;
  const uint32_t                     aSlot1 = aRegistry.RegisterCache(aKind1);
  const uint32_t                     aSlot2 = aRegistry.RegisterCache(aKind2);
  EXPECT_EQ(aSlot1, aSlot2);
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

TEST(BRepGraph_CacheRegistryTest, SameCache_CanUseDifferentGraphLocalSlots)
{
  const occ::handle<BRepGraph_Cache> aShared =
    new TestCache(Standard_GUID("e1e2e3e4-1111-2222-3333-888899990001"), "Shared");
  const occ::handle<BRepGraph_Cache> aPrefix =
    new TestCache(Standard_GUID("e1e2e3e4-1111-2222-3333-888899990002"), "Prefix");

  BRepGraph_CacheRegistry         aRegistry1;
  BRepGraph_CacheRegistry         aRegistry2;
  const uint32_t                  aSlot1      = aRegistry1.RegisterCache(aShared);
  [[maybe_unused]] const uint32_t aPrefixSlot = aRegistry2.RegisterCache(aPrefix);
  const uint32_t                  aSlot2      = aRegistry2.RegisterCache(aShared);

  EXPECT_EQ(aSlot1, 0u);
  EXPECT_EQ(aSlot2, 1u);
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

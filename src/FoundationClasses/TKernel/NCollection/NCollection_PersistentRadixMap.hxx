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

#ifndef _NCollection_PersistentRadixMap_HeaderFile
#define _NCollection_PersistentRadixMap_HeaderFile

#include <NCollection_LinearVector.hxx>
#include <NCollection_ItemsView.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_NoSuchObject.hxx>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

//! Immutable fixed-depth radix-256 map with structural sharing.
//!
//! The hash policy owns canonical encoding. It must provide EmptyHash(),
//! LeafHash(key, value), and BranchHash(depth, ordered slot/hash pairs, count).
//! Values must be equality-comparable. Container mechanics retain them for
//! lookup but never interpret or serialize domain values.
template <typename TheKeyType, typename TheValueType, typename TheHashType, typename TheHashPolicy>
class NCollection_PersistentRadixMap
{
  static_assert(std::is_integral_v<TheKeyType> && std::is_unsigned_v<TheKeyType>,
                "persistent radix keys must be unsigned integers");

public:
  DEFINE_STANDARD_ALLOC

  using key_type        = TheKeyType;
  using mapped_type     = TheValueType;
  using value_type      = TheValueType;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using const_reference = const TheValueType&;
  using const_pointer   = const TheValueType*;

  struct Entry
  {
    TheKeyType   Key;
    TheValueType Value;
  };

  NCollection_PersistentRadixMap()
      : myRootHash(TheHashPolicy::EmptyHash())
  {
  }

  //! Build a canonical map in one pass. Return false and leave the result unchanged on duplicates.
  [[nodiscard]] static bool Build(NCollection_LinearVector<Entry> theEntries,
                                  NCollection_PersistentRadixMap& theResult)
  {
    if (theEntries.IsEmpty())
    {
      theResult = NCollection_PersistentRadixMap();
      return true;
    }
    std::sort(
      theEntries.begin(),
      theEntries.end(),
      [](const Entry& theLeft, const Entry& theRight) { return theLeft.Key < theRight.Key; });
    for (size_t anIndex = 1; anIndex < theEntries.Size(); ++anIndex)
    {
      if (theEntries.Value(anIndex - 1).Key == theEntries.Value(anIndex).Key)
      {
        return false;
      }
    }
    const NodePtr aRoot = build(theEntries, 0, theEntries.Size(), 0);
    theResult           = NCollection_PersistentRadixMap(aRoot, aRoot->Hash);
    return true;
  }

  [[nodiscard]] NCollection_PersistentRadixMap Insert(const TheKeyType    theKey,
                                                      const TheValueType& theValue) const
  {
    const NodePtr aRoot = insert(myRoot, theKey, theValue, 0);
    return aRoot == myRoot ? *this : NCollection_PersistentRadixMap(aRoot, aRoot->Hash);
  }

  [[nodiscard]] NCollection_PersistentRadixMap Remove(const TheKeyType theKey) const
  {
    const NodePtr aRoot = remove(myRoot, theKey, 0);
    if (aRoot == myRoot)
    {
      return *this;
    }
    return NCollection_PersistentRadixMap(aRoot,
                                          aRoot == nullptr ? TheHashPolicy::EmptyHash()
                                                           : aRoot->Hash);
  }

  [[nodiscard]] bool Contains(const TheKeyType theKey) const { return Find(theKey) != nullptr; }

  //! Return the retained value for a key, or null when the key is absent.
  [[nodiscard]] const TheValueType* Find(const TheKeyType theKey) const
  {
    return find(myRoot, theKey, 0);
  }

  [[nodiscard]] size_t Size() const noexcept { return myRoot == nullptr ? 0 : myRoot->Count; }

  [[nodiscard]] const TheHashType& RootHash() const noexcept { return myRootHash; }

  [[nodiscard]] bool IsEmpty() const noexcept { return myRoot == nullptr; }

  //! Exact identity of the retained root, useful for collision-free subtree fast paths.
  [[nodiscard]] const void* RootIdentity() const noexcept { return myRoot.get(); }

private:
  static constexpr size_t THE_KEY_BYTES = sizeof(TheKeyType);

  struct Node;
  using NodePtr = std::shared_ptr<const Node>;

  struct Child
  {
    uint8_t Slot;
    NodePtr NodeValue;
  };

  using Children     = NCollection_LinearVector<Child>;
  using HashChildren = NCollection_LinearVector<std::pair<uint8_t, TheHashType>>;

  struct Node
  {
    struct LeafData
    {
      TheKeyType   Key;
      TheValueType Value;
    };

    using ContentType = std::variant<LeafData, Child, Children>;

    Node(const TheKeyType theKey, const TheValueType& theValue, const TheHashType& theHash)
        : Hash(theHash),
          Count(1),
          Content(std::in_place_type<LeafData>, LeafData{theKey, theValue})
    {
    }

    Node(Children&& theChildren, const size_t theDepth)
        : Hash(hashChildren(theChildren, theDepth)),
          Count(countChildren(theChildren)),
          Content(std::in_place_type<Children>, std::move(theChildren))
    {
    }

    Node(Child&& theChild, const size_t theDepth)
        : Hash(hashChild(theChild, theDepth)),
          Count(theChild.NodeValue->Count),
          Content(std::in_place_type<Child>, std::move(theChild))
    {
    }

    [[nodiscard]] bool IsLeaf() const noexcept { return std::holds_alternative<LeafData>(Content); }

    [[nodiscard]] const LeafData& Leaf() const { return std::get<LeafData>(Content); }

    [[nodiscard]] size_t NbChildren() const noexcept
    {
      if (IsLeaf())
      {
        return 0;
      }
      return std::holds_alternative<Child>(Content) ? 1 : std::get<Children>(Content).Size();
    }

    [[nodiscard]] const Child& ChildAt(const size_t theIndex) const
    {
      return std::holds_alternative<Child>(Content) ? std::get<Child>(Content)
                                                     : std::get<Children>(Content).Value(theIndex);
    }

    [[nodiscard]] size_t ChildIndex(const uint8_t theSlot) const
    {
      if (std::holds_alternative<Child>(Content))
      {
        return std::get<Child>(Content).Slot < theSlot ? 1 : 0;
      }
      const Children& aChildren = std::get<Children>(Content);
      const auto anIt = std::lower_bound(
        aChildren.begin(),
        aChildren.end(),
        theSlot,
        [](const Child& theChild, const uint8_t theValue) { return theChild.Slot < theValue; });
      return static_cast<size_t>(anIt - aChildren.begin());
    }

    [[nodiscard]] Children CopyChildren() const
    {
      if (std::holds_alternative<Child>(Content))
      {
        Children aChildren(1);
        aChildren.Append(std::get<Child>(Content));
        return aChildren;
      }
      return std::get<Children>(Content);
    }

    static TheHashType hashChildren(const Children& theChildren, const size_t theDepth)
    {
      HashChildren aHashes(theChildren.Size());
      for (const Child& aChild : theChildren)
      {
        aHashes.Append({aChild.Slot, aChild.NodeValue->Hash});
      }
      return TheHashPolicy::BranchHash(theDepth, aHashes.Data(), aHashes.Size());
    }

    static TheHashType hashChild(const Child& theChild, const size_t theDepth)
    {
      const std::pair<uint8_t, TheHashType> aHash(theChild.Slot, theChild.NodeValue->Hash);
      return TheHashPolicy::BranchHash(theDepth, &aHash, 1);
    }

    static size_t countChildren(const Children& theChildren)
    {
      size_t aCount = 0;
      for (const Child& aChild : theChildren)
      {
        aCount += aChild.NodeValue->Count;
      }
      return aCount;
    }

    TheHashType Hash;
    size_t      Count;
    ContentType Content;
  };

public:
  class const_iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = TheValueType;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const TheValueType*;
    using reference         = const TheValueType&;

    const_iterator() noexcept = default;

    explicit const_iterator(const NCollection_PersistentRadixMap& theMap) noexcept
        : myRoot(theMap.myRoot)
    {
      initialize();
    }

    reference operator*() const noexcept { return myLeaf->Leaf().Value; }

    pointer operator->() const noexcept { return &myLeaf->Leaf().Value; }

    [[nodiscard]] const TheKeyType& Key() const noexcept { return myLeaf->Leaf().Key; }

    [[nodiscard]] reference Value() const noexcept { return operator*(); }

    [[nodiscard]] bool More() const noexcept { return myLeaf != nullptr; }

    const_iterator& operator++() noexcept
    {
      advance();
      return *this;
    }

    const_iterator operator++(int) noexcept
    {
      const_iterator anOld(*this);
      ++(*this);
      return anOld;
    }

    bool operator==(const const_iterator& theOther) const noexcept
    {
      return (!More() && !theOther.More())
             || (myRoot == theOther.myRoot && myLeaf == theOther.myLeaf);
    }

    bool operator!=(const const_iterator& theOther) const noexcept { return !(*this == theOther); }

  private:
    void initialize() noexcept
    {
      if (myRoot == nullptr)
      {
        return;
      }
      const Node* aNode = myRoot.get();
      for (size_t aDepth = 0; aDepth < THE_KEY_BYTES; ++aDepth)
      {
        myBranches[aDepth] = aNode;
        myIndices[aDepth]  = 0;
        if (aNode->NbChildren() == 0)
        {
          myRoot.reset();
          return;
        }
        aNode = aNode->ChildAt(0).NodeValue.get();
      }
      myLeaf = aNode->IsLeaf() ? aNode : nullptr;
      if (myLeaf == nullptr)
      {
        myRoot.reset();
      }
    }

    void advance() noexcept
    {
      if (myLeaf == nullptr)
      {
        return;
      }
      for (size_t aDepth = THE_KEY_BYTES; aDepth-- > 0;)
      {
        const Node*  aBranch = myBranches[aDepth];
        const size_t aNext   = myIndices[aDepth] + 1;
        if (aNext >= aBranch->NbChildren())
        {
          continue;
        }

        myIndices[aDepth] = aNext;
        const Node* aNode = aBranch->ChildAt(aNext).NodeValue.get();
        for (size_t aChildDepth = aDepth + 1; aChildDepth < THE_KEY_BYTES; ++aChildDepth)
        {
          myBranches[aChildDepth] = aNode;
          myIndices[aChildDepth]  = 0;
          aNode                   = aNode->ChildAt(0).NodeValue.get();
        }
        myLeaf = aNode;
        return;
      }
      myLeaf = nullptr;
      myRoot.reset();
    }

    NodePtr                                myRoot;
    std::array<const Node*, THE_KEY_BYTES> myBranches{};
    std::array<size_t, THE_KEY_BYTES>      myIndices{};
    const Node*                            myLeaf = nullptr;
  };

  using iterator = const_iterator;

  class Iterator : public const_iterator
  {
  public:
    Iterator() noexcept = default;

    explicit Iterator(const NCollection_PersistentRadixMap& theMap) noexcept
        : const_iterator(theMap),
          myMap(&theMap)
    {
    }

    bool More() const noexcept { return const_iterator::More(); }

    void Next() noexcept { ++(*this); }

    const TheKeyType& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_PersistentRadixMap::Iterator::Key");
      return const_iterator::Key();
    }

    const TheValueType& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_PersistentRadixMap::Iterator::Value");
      return const_iterator::Value();
    }

    void Initialize(const NCollection_PersistentRadixMap& theMap) noexcept
    {
      *this = Iterator(theMap);
    }

    void Reset() noexcept { *this = myMap == nullptr ? Iterator() : Iterator(*myMap); }

    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return const_iterator::operator==(static_cast<const const_iterator&>(theOther));
    }

  private:
    const NCollection_PersistentRadixMap* myMap = nullptr;
  };

  iterator begin() noexcept { return iterator(*this); }

  const_iterator begin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(); }

  const_iterator end() const noexcept { return const_iterator(); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  const_iterator cend() const noexcept { return const_iterator(); }

  using ConstKeyValueRef = NCollection_ItemsView::KeyValueRef<TheKeyType, TheValueType, true>;

private:
  struct ConstItemsExtractor
  {
    template <class TheIterator>
    static ConstKeyValueRef Extract(const TheIterator& theIterator)
    {
      return {theIterator.Key(), *theIterator};
    }
  };

public:
  using ConstItemsView = NCollection_ItemsView::
    View<NCollection_PersistentRadixMap, ConstKeyValueRef, ConstItemsExtractor, true>;

  ConstItemsView Items() { return ConstItemsView(*this); }

  ConstItemsView Items() const { return ConstItemsView(*this); }

private:
  NCollection_PersistentRadixMap(const NodePtr& theRoot, const TheHashType& theRootHash)
      : myRoot(theRoot),
        myRootHash(theRootHash)
  {
  }

  static uint8_t keySlot(const TheKeyType theKey, const size_t theDepth)
  {
    return static_cast<uint8_t>(theKey >> ((THE_KEY_BYTES - theDepth - 1) * 8));
  }

  static NodePtr insert(const NodePtr&      theNode,
                        const TheKeyType    theKey,
                        const TheValueType& theValue,
                        const size_t        theDepth)
  {
    if (theDepth == THE_KEY_BYTES)
    {
      const TheHashType aHash = TheHashPolicy::LeafHash(theKey, theValue);
      if (theNode != nullptr && theNode->IsLeaf() && theNode->Leaf().Key == theKey
          && theNode->Leaf().Value == theValue)
      {
        return theNode;
      }
      return std::make_shared<const Node>(theKey, theValue, aHash);
    }

    const uint8_t aSlot       = keySlot(theKey, theDepth);
    const size_t  aChildIndex = theNode == nullptr ? 0 : theNode->ChildIndex(aSlot);
    const bool hasChild = theNode != nullptr && aChildIndex < theNode->NbChildren()
                          && theNode->ChildAt(aChildIndex).Slot == aSlot;
    const NodePtr anOldChild =
      hasChild ? theNode->ChildAt(aChildIndex).NodeValue : NodePtr();
    const NodePtr aNewChild = insert(anOldChild, theKey, theValue, theDepth + 1);
    if (aNewChild == anOldChild)
    {
      return theNode;
    }

    if (theNode == nullptr || (hasChild && theNode->NbChildren() == 1))
    {
      return std::make_shared<const Node>(Child{aSlot, aNewChild}, theDepth);
    }

    Children aChildren = theNode->CopyChildren();
    if (hasChild)
    {
      aChildren.ChangeValue(aChildIndex).NodeValue = aNewChild;
    }
    else
    {
      aChildren.InsertBefore(aChildIndex, {aSlot, aNewChild});
    }
    return std::make_shared<const Node>(std::move(aChildren), theDepth);
  }

  static NodePtr build(const NCollection_LinearVector<Entry>& theEntries,
                       const size_t                           theFirst,
                       const size_t                           theLast,
                       const size_t                           theDepth)
  {
    if (theDepth == THE_KEY_BYTES)
    {
      const Entry& anEntry = theEntries.Value(theFirst);
      return std::make_shared<const Node>(anEntry.Key,
                                          anEntry.Value,
                                          TheHashPolicy::LeafHash(anEntry.Key, anEntry.Value));
    }


    const uint8_t aFirstSlot = keySlot(theEntries.Value(theFirst).Key, theDepth);
    if (aFirstSlot == keySlot(theEntries.Value(theLast - 1).Key, theDepth))
    {
      return std::make_shared<const Node>(
        Child{aFirstSlot, build(theEntries, theFirst, theLast, theDepth + 1)},
        theDepth);
    }
    Children aChildren(std::min(theLast - theFirst, size_t(256)));
    size_t   aFirst = theFirst;
    while (aFirst < theLast)
    {
      const uint8_t aSlot = keySlot(theEntries.Value(aFirst).Key, theDepth);
      size_t        aNext = aFirst + 1;
      while (aNext < theLast && keySlot(theEntries.Value(aNext).Key, theDepth) == aSlot)
      {
        ++aNext;
      }
      aChildren.Append({aSlot, build(theEntries, aFirst, aNext, theDepth + 1)});
      aFirst = aNext;
    }
    return std::make_shared<const Node>(std::move(aChildren), theDepth);
  }

  static NodePtr remove(const NodePtr& theNode, const TheKeyType theKey, const size_t theDepth)
  {
    if (theNode == nullptr)
    {
      return theNode;
    }
    if (theDepth == THE_KEY_BYTES)
    {
      return theNode->IsLeaf() && theNode->Leaf().Key == theKey ? NodePtr() : theNode;
    }
    const uint8_t aSlot   = keySlot(theKey, theDepth);
    const size_t  anIndex = theNode->ChildIndex(aSlot);
    if (anIndex == theNode->NbChildren() || theNode->ChildAt(anIndex).Slot != aSlot)
    {
      return theNode;
    }
    const NodePtr& anOldChild = theNode->ChildAt(anIndex).NodeValue;
    const NodePtr  aNewChild  = remove(anOldChild, theKey, theDepth + 1);
    if (aNewChild == anOldChild)
    {
      return theNode;
    }
    Children aChildren = theNode->CopyChildren();
    if (aNewChild != nullptr)
    {
      if (theNode->NbChildren() == 1)
      {
        return std::make_shared<const Node>(Child{aSlot, aNewChild}, theDepth);
      }
      aChildren.ChangeValue(anIndex).NodeValue = aNewChild;
      return std::make_shared<const Node>(std::move(aChildren), theDepth);
    }
    aChildren.Erase(anIndex);
    if (aChildren.IsEmpty())
    {
      return NodePtr();
    }
    if (aChildren.Size() == 1)
    {
      return std::make_shared<const Node>(std::move(aChildren.ChangeFirst()), theDepth);
    }
    return std::make_shared<const Node>(std::move(aChildren), theDepth);
  }

  static const TheValueType* find(const NodePtr&   theNode,
                                  const TheKeyType theKey,
                                  const size_t     theDepth)
  {
    if (theNode == nullptr)
    {
      return nullptr;
    }
    if (theDepth == THE_KEY_BYTES)
    {
      return theNode->IsLeaf() && theNode->Leaf().Key == theKey ? &theNode->Leaf().Value : nullptr;
    }
    const uint8_t aSlot   = keySlot(theKey, theDepth);
    const size_t  anIndex = theNode->ChildIndex(aSlot);
    return anIndex < theNode->NbChildren() && theNode->ChildAt(anIndex).Slot == aSlot
             ? find(theNode->ChildAt(anIndex).NodeValue, theKey, theDepth + 1)
             : nullptr;
  }

  NodePtr     myRoot;
  TheHashType myRootHash;
};

#endif // _NCollection_PersistentRadixMap_HeaderFile

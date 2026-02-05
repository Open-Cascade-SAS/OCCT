// Created on: 2002-04-24
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef NCollection_IndexedDataMap_HeaderFile
#define NCollection_IndexedDataMap_HeaderFile

#include <NCollection_BaseMap.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_NoSuchObject.hxx>
#include <NCollection_StlIterator.hxx>
#include <NCollection_DefaultHasher.hxx>
#include <NCollection_ItemsView.hxx>

#include <Standard_OutOfRange.hxx>
#include <type_traits>
#include <utility>

/**
 * Purpose:     An indexed map is used  to store keys and to  bind
 *              an index to them.  Each  new key stored in the map
 *              gets an index.  Index are  incremented as keys are
 *              stored in the map. A key can be found by the index
 *              and an index by the key.  No  key but the last can
 *              be  removed so the  indices   are in the range 1..
 *              Extent.  An Item is stored with each key.
 *
 *              This   class is   similar  to  IndexedMap     from
 *              NCollection  with the Item as  a new feature. Note
 *              the important difference on  the operator  ().  In
 *              the IndexedMap this operator returns  the Key.  In
 *              the IndexedDataMap this operator returns the Item.
 *
 *              See  the  class   Map   from NCollection   for   a
 *              discussion about the number of buckets.
 */

template <class TheKeyType, class TheItemType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_IndexedDataMap : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;
  typedef Hasher      hasher;

private:
  //!    Adaptation of the ListNode to the INDEXEDDatamap
  class IndexedDataMapNode : public NCollection_ListNode
  {
  public:
    //! Constructor with 'Next'
    IndexedDataMapNode(const TheKeyType&     theKey1,
                       const int             theIndex,
                       const TheItemType&    theItem,
                       NCollection_ListNode* theNext1)
        : NCollection_ListNode(theNext1),
          myKey1(theKey1),
          myIndex(theIndex),
          myValue(theItem)
    {
    }

    //! Constructor with 'Next'
    IndexedDataMapNode(TheKeyType&&          theKey1,
                       const int             theIndex,
                       const TheItemType&    theItem,
                       NCollection_ListNode* theNext1)
        : NCollection_ListNode(theNext1),
          myKey1(std::forward<TheKeyType>(theKey1)),
          myIndex(theIndex),
          myValue(theItem)
    {
    }

    //! Constructor with 'Next'
    IndexedDataMapNode(const TheKeyType&     theKey1,
                       const int             theIndex,
                       TheItemType&&         theItem,
                       NCollection_ListNode* theNext1)
        : NCollection_ListNode(theNext1),
          myKey1(theKey1),
          myIndex(theIndex),
          myValue(std::forward<TheItemType>(theItem))
    {
    }

    //! Constructor with 'Next'
    IndexedDataMapNode(TheKeyType&&          theKey1,
                       const int             theIndex,
                       TheItemType&&         theItem,
                       NCollection_ListNode* theNext1)
        : NCollection_ListNode(theNext1),
          myKey1(std::forward<TheKeyType>(theKey1)),
          myIndex(theIndex),
          myValue(std::forward<TheItemType>(theItem))
    {
    }

    //! Constructor with in-place value construction
    template <typename K, typename... Args>
    IndexedDataMapNode(K&&       theKey1,
                       const int theIndex,
                       std::in_place_t,
                       NCollection_ListNode* theNext1,
                       Args&&... theArgs)
        : NCollection_ListNode(theNext1),
          myKey1(std::forward<K>(theKey1)),
          myIndex(theIndex),
          myValue(std::forward<Args>(theArgs)...)
    {
    }

    //! Key1
    TheKeyType& Key1() noexcept { return myKey1; }

    //! Index
    int& Index() noexcept { return myIndex; }

    //! Constant value access
    const TheItemType& Value() const noexcept { return myValue; }

    //! Variable value access
    TheItemType& ChangeValue() noexcept { return myValue; }

    //! Static deleter to be passed to BaseList
    static void delNode(NCollection_ListNode*                   theNode,
                        occ::handle<NCollection_BaseAllocator>& theAl) noexcept
    {
      ((IndexedDataMapNode*)theNode)->~IndexedDataMapNode();
      theAl->Free(theNode);
    }

  private:
    TheKeyType  myKey1;
    int         myIndex;
    TheItemType myValue;
  };

public:
  //!   Implementation of the Iterator interface.
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator()
        : myMap(nullptr),
          myIndex(0)
    {
    }

    //! Constructor
    Iterator(const NCollection_IndexedDataMap& theMap)
        : myMap((NCollection_IndexedDataMap*)&theMap),
          myIndex(1)
    {
    }

    //! Query if the end of collection is reached by iterator
    bool More() const noexcept { return (myMap != nullptr) && (myIndex <= myMap->Extent()); }

    //! Make a step along the collection
    void Next() noexcept { ++myIndex; }

    //! Value access
    const TheItemType& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedDataMap::Iterator::Value");
      return myMap->FindFromIndex(myIndex);
    }

    //! ChangeValue access
    TheItemType& ChangeValue() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedDataMap::Iterator::ChangeValue");
      return myMap->ChangeFromIndex(myIndex);
    }

    //! Key
    const TheKeyType& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedDataMap::Iterator::Key");
      return myMap->FindKey(myIndex);
    }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return myMap == theOther.myMap && myIndex == theOther.myIndex;
    }

    //! Returns current index (1-based).
    int Index() const noexcept { return myIndex; }

  private:
    NCollection_IndexedDataMap* myMap;   //!< Pointer to current node
    int                         myIndex; //!< Current index
  };

  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, true>
    const_iterator;

  //! Returns an iterator pointing to the first element in the map.
  iterator begin() const noexcept { return Iterator(*this); }

  //! Returns an iterator referring to the past-the-end element in the map.
  iterator end() const noexcept { return Iterator(); }

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const noexcept { return Iterator(*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const noexcept { return Iterator(); }

public:
  // **************** Key-value pair iteration support for structured bindings

  //! Key-value pair reference for structured binding support.
  //! Enables: for (auto [key, value] : map.Items())
  using KeyValueRef = NCollection_ItemsView::KeyValueRef<TheKeyType, TheItemType, false>;

  //! Const key-value pair reference for structured binding support.
  using ConstKeyValueRef = NCollection_ItemsView::KeyValueRef<TheKeyType, TheItemType, true>;

  //! Key-value-index tuple reference for structured binding support.
  //! Enables: for (auto [key, value, index] : map.IndexedItems())
  using KeyValueIndexRef = NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheItemType, false>;

  //! Const key-value-index tuple reference for structured binding support.
  using ConstKeyValueIndexRef =
    NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheItemType, true>;

private:
  //! Extractor for mutable key-value pairs
  struct ItemsExtractor
  {
    static KeyValueRef Extract(const Iterator& theIter)
    {
      return {theIter.Key(), theIter.ChangeValue()};
    }
  };

  //! Extractor for const key-value pairs
  struct ConstItemsExtractor
  {
    static ConstKeyValueRef Extract(const Iterator& theIter)
    {
      return {theIter.Key(), theIter.Value()};
    }
  };

  //! Extractor for mutable key-value-index tuples
  struct IndexedItemsExtractor
  {
    static KeyValueIndexRef Extract(const Iterator& theIter)
    {
      return {theIter.Key(), theIter.ChangeValue(), theIter.Index()};
    }
  };

  //! Extractor for const key-value-index tuples
  struct ConstIndexedItemsExtractor
  {
    static ConstKeyValueIndexRef Extract(const Iterator& theIter)
    {
      return {theIter.Key(), theIter.Value(), theIter.Index()};
    }
  };

public:
  //! View class for key-value pair iteration (mutable).
  using ItemsView =
    NCollection_ItemsView::View<NCollection_IndexedDataMap, KeyValueRef, ItemsExtractor, false>;

  //! View class for key-value pair iteration (const).
  using ConstItemsView = NCollection_ItemsView::
    View<NCollection_IndexedDataMap, ConstKeyValueRef, ConstItemsExtractor, true>;

  //! View class for key-value-index tuple iteration (mutable).
  using IndexedItemsView = NCollection_ItemsView::
    View<NCollection_IndexedDataMap, KeyValueIndexRef, IndexedItemsExtractor, false>;

  //! View class for key-value-index tuple iteration (const).
  using ConstIndexedItemsView = NCollection_ItemsView::
    View<NCollection_IndexedDataMap, ConstKeyValueIndexRef, ConstIndexedItemsExtractor, true>;

  //! Returns a view for key-value pair iteration.
  //! Usage: for (auto [aKey, aValue] : aMap.Items())
  ItemsView Items() { return ItemsView(*this); }

  //! Returns a const view for key-value pair iteration.
  //! Usage: for (const auto& [aKey, aValue] : aMap.Items())
  ConstItemsView Items() const { return ConstItemsView(*this); }

  //! Returns a view for key-value-index tuple iteration.
  //! Usage: for (auto [aKey, aValue, anIndex] : aMap.IndexedItems())
  IndexedItemsView IndexedItems() { return IndexedItemsView(*this); }

  //! Returns a const view for key-value-index tuple iteration.
  //! Usage: for (const auto& [aKey, aValue, anIndex] : aMap.IndexedItems())
  ConstIndexedItemsView IndexedItems() const { return ConstIndexedItemsView(*this); }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_IndexedDataMap()
      : NCollection_BaseMap(1, true, occ::handle<NCollection_BaseAllocator>())
  {
  }

  //! Constructor
  explicit NCollection_IndexedDataMap(
    const int                                     theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator)
  {
  }

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial number of buckets
  //! @param theAllocator custom memory allocator
  explicit NCollection_IndexedDataMap(
    const Hasher&                                 theHasher,
    const int                                     theNbBuckets = 1,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator),
        myHasher(theHasher)
  {
  }

  //! Constructor with custom hasher (move).
  //! @param theHasher custom hasher instance (moved)
  //! @param theNbBuckets initial number of buckets
  //! @param theAllocator custom memory allocator
  explicit NCollection_IndexedDataMap(
    Hasher&&                                      theHasher,
    const int                                     theNbBuckets = 1,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator),
        myHasher(std::move(theHasher))
  {
  }

  //! Copy constructor
  NCollection_IndexedDataMap(const NCollection_IndexedDataMap& theOther)
      : NCollection_BaseMap(theOther.NbBuckets(), true, theOther.myAllocator),
        myHasher(theOther.myHasher)
  {
    *this = theOther;
  }

  //! Move constructor
  NCollection_IndexedDataMap(NCollection_IndexedDataMap&& theOther) noexcept
      : NCollection_BaseMap(std::forward<NCollection_BaseMap>(theOther)),
        myHasher(std::move(theOther.myHasher))
  {
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_IndexedDataMap& theOther) noexcept
  {
    this->exchangeMapsData(theOther);
    std::swap(myHasher, theOther.myHasher);
  }

  //! Returns const reference to the hasher.
  const Hasher& GetHasher() const noexcept { return myHasher; }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_IndexedDataMap& Assign(const NCollection_IndexedDataMap& theOther)
  {
    if (this == &theOther)
      return *this;

    Clear();
    int anExt = theOther.Extent();
    if (anExt)
    {
      ReSize(anExt - 1); // mySize is same after resize
      for (int anIndexIter = 1; anIndexIter <= anExt; ++anIndexIter)
      {
        const TheKeyType&   aKey1  = theOther.FindKey(anIndexIter);
        const TheItemType&  anItem = theOther.FindFromIndex(anIndexIter);
        const size_t        iK1    = HashCode(aKey1, NbBuckets());
        IndexedDataMapNode* pNode =
          new (this->myAllocator) IndexedDataMapNode(aKey1, anIndexIter, anItem, myData1[iK1]);
        myData1[iK1]             = pNode;
        myData2[anIndexIter - 1] = pNode;
        Increment();
      }
    }
    return *this;
  }

  //! Assignment operator
  NCollection_IndexedDataMap& operator=(const NCollection_IndexedDataMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move operator
  NCollection_IndexedDataMap& operator=(NCollection_IndexedDataMap&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    return *this;
  }

  //! ReSize
  void ReSize(const int N)
  {
    NCollection_ListNode** ppNewData1 = nullptr;
    NCollection_ListNode** ppNewData2 = nullptr;
    int                    newBuck;
    if (BeginResize(N, newBuck, ppNewData1, ppNewData2))
    {
      if (myData1)
      {
        for (int aBucketIter = 0; aBucketIter <= NbBuckets(); ++aBucketIter)
        {
          if (myData1[aBucketIter])
          {
            IndexedDataMapNode* p = (IndexedDataMapNode*)myData1[aBucketIter];
            while (p)
            {
              const size_t        iK1 = HashCode(p->Key1(), newBuck);
              IndexedDataMapNode* q   = (IndexedDataMapNode*)p->Next();
              p->Next()               = ppNewData1[iK1];
              ppNewData1[iK1]         = p;
              p                       = q;
            }
          }
        }
      }
      EndResize(N,
                newBuck,
                ppNewData1,
                (NCollection_ListNode**)
                  Standard::Reallocate(myData2, (newBuck + 1) * sizeof(NCollection_ListNode*)));
    }
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  int Add(const TheKeyType& theKey1, const TheItemType& theItem)
  {
    return addImpl(theKey1, theItem);
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  int Add(TheKeyType&& theKey1, const TheItemType& theItem)
  {
    return addImpl(std::move(theKey1), theItem);
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  int Add(const TheKeyType& theKey1, TheItemType&& theItem)
  {
    return addImpl(theKey1, std::move(theItem));
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  int Add(TheKeyType&& theKey1, TheItemType&& theItem)
  {
    return addImpl(std::move(theKey1), std::move(theItem));
  }

  //! TryBound binds Item to Key only if Key is not yet bound.
  //! @param theKey1 key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return reference to existing or newly bound Item
  TheItemType& TryBound(const TheKeyType& theKey1, const TheItemType& theItem)
  {
    return bindImpl(theKey1, theItem, std::true_type{}, std::true_type{});
  }

  //! TryBound binds Item to Key only if Key is not yet bound.
  TheItemType& TryBound(TheKeyType&& theKey1, const TheItemType& theItem)
  {
    return bindImpl(std::move(theKey1), theItem, std::true_type{}, std::true_type{});
  }

  //! TryBound binds Item to Key only if Key is not yet bound.
  TheItemType& TryBound(const TheKeyType& theKey1, TheItemType&& theItem)
  {
    return bindImpl(theKey1, std::move(theItem), std::true_type{}, std::true_type{});
  }

  //! TryBound binds Item to Key only if Key is not yet bound.
  TheItemType& TryBound(TheKeyType&& theKey1, TheItemType&& theItem)
  {
    return bindImpl(std::move(theKey1), std::move(theItem), std::true_type{}, std::true_type{});
  }

  //! Bind binds Item to Key in map; overwrites value if Key already exists.
  //! @param theKey1 key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return true if Key was not bound already
  bool Bind(const TheKeyType& theKey1, const TheItemType& theItem)
  {
    return bindImpl(theKey1, theItem, std::false_type{}, std::false_type{});
  }

  //! Bind binds Item to Key in map; overwrites value if Key already exists.
  bool Bind(TheKeyType&& theKey1, const TheItemType& theItem)
  {
    return bindImpl(std::move(theKey1), theItem, std::false_type{}, std::false_type{});
  }

  //! Bind binds Item to Key in map; overwrites value if Key already exists.
  bool Bind(const TheKeyType& theKey1, TheItemType&& theItem)
  {
    return bindImpl(theKey1, std::move(theItem), std::false_type{}, std::false_type{});
  }

  //! Bind binds Item to Key in map; overwrites value if Key already exists.
  bool Bind(TheKeyType&& theKey1, TheItemType&& theItem)
  {
    return bindImpl(std::move(theKey1), std::move(theItem), std::false_type{}, std::false_type{});
  }

  //! Bound binds Item to Key in map; overwrites value if Key already exists.
  //! @param theKey1 key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return pointer to modifiable Item
  TheItemType* Bound(const TheKeyType& theKey1, const TheItemType& theItem)
  {
    return &bindImpl(theKey1, theItem, std::false_type{}, std::true_type{});
  }

  //! Bound binds Item to Key in map; overwrites value if Key already exists.
  TheItemType* Bound(TheKeyType&& theKey1, const TheItemType& theItem)
  {
    return &bindImpl(std::move(theKey1), theItem, std::false_type{}, std::true_type{});
  }

  //! Bound binds Item to Key in map; overwrites value if Key already exists.
  TheItemType* Bound(const TheKeyType& theKey1, TheItemType&& theItem)
  {
    return &bindImpl(theKey1, std::move(theItem), std::false_type{}, std::true_type{});
  }

  //! Bound binds Item to Key in map; overwrites value if Key already exists.
  TheItemType* Bound(TheKeyType&& theKey1, TheItemType&& theItem)
  {
    return &bindImpl(std::move(theKey1), std::move(theItem), std::false_type{}, std::true_type{});
  }

  //! Emplace constructs value in-place; if key exists, overwrites value.
  //! @param theKey1 key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return index of the key (new or existing)
  template <typename K, typename... Args>
  int Emplace(K&& theKey1, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey1),
                       std::false_type{},
                       std::false_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! Emplaced constructs value in-place; if key exists, destroys and reconstructs value.
  //! @param theKey1 key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing reconstructed or newly added)
  template <typename K, typename... Args>
  TheItemType& Emplaced(K&& theKey1, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey1),
                       std::false_type{},
                       std::true_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! TryEmplace constructs value in-place only if key not already bound.
  //! @param theKey1 key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return index of the key (new or existing)
  template <typename K, typename... Args>
  int TryEmplace(K&& theKey1, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey1),
                       std::true_type{},
                       std::false_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! TryEmplaced constructs value in-place only if key not already bound.
  //! @param theKey1 key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing or newly added)
  template <typename K, typename... Args>
  TheItemType& TryEmplaced(K&& theKey1, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey1),
                       std::true_type{},
                       std::true_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! Contains
  bool Contains(const TheKeyType& theKey1) const
  {
    IndexedDataMapNode* aNode;
    return static_cast<bool>(lookup(theKey1, aNode));
  }

  //! Substitute
  void Substitute(const int theIndex, const TheKeyType& theKey1, const TheItemType& theItem)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::Substitute : "
                                 "Index is out of range");

    // check if theKey1 is not already in the map
    size_t              aHash;
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode, aHash))
    {
      if (aNode->Index() != theIndex)
      {
        throw Standard_DomainError("NCollection_IndexedDataMap::Substitute : "
                                   "Attempt to substitute existing key");
      }
      aNode->Key1()        = theKey1;
      aNode->ChangeValue() = theItem;
      return;
    }

    // Find the node for the index I
    aNode = (IndexedDataMapNode*)myData2[theIndex - 1];

    // remove the old key
    const size_t        iK = HashCode(aNode->Key1(), NbBuckets());
    IndexedDataMapNode* q  = (IndexedDataMapNode*)myData1[iK];
    if (q == aNode)
      myData1[iK] = (IndexedDataMapNode*)aNode->Next();
    else
    {
      while (q->Next() != aNode)
        q = (IndexedDataMapNode*)q->Next();
      q->Next() = aNode->Next();
    }

    // update the node
    aNode->Key1()        = theKey1;
    aNode->ChangeValue() = theItem;
    aNode->Next()        = myData1[aHash];
    myData1[aHash]       = aNode;
  }

  //! Swaps two elements with the given indices.
  void Swap(const int theIndex1, const int theIndex2)
  {
    Standard_OutOfRange_Raise_if(theIndex1 < 1 || theIndex1 > Extent() || theIndex2 < 1
                                   || theIndex2 > Extent(),
                                 "NCollection_IndexedDataMap::Swap");

    if (theIndex1 == theIndex2)
    {
      return;
    }

    IndexedDataMapNode* aP1 = (IndexedDataMapNode*)myData2[theIndex1 - 1];
    IndexedDataMapNode* aP2 = (IndexedDataMapNode*)myData2[theIndex2 - 1];
    std::swap(aP1->Index(), aP2->Index());
    myData2[theIndex2 - 1] = aP1;
    myData2[theIndex1 - 1] = aP2;
  }

  //! RemoveLast
  void RemoveLast()
  {
    const int aLastIndex = Extent();
    Standard_OutOfRange_Raise_if(aLastIndex == 0, "NCollection_IndexedDataMap::RemoveLast");

    // Find the node for the last index and remove it
    IndexedDataMapNode* p   = (IndexedDataMapNode*)myData2[aLastIndex - 1];
    myData2[aLastIndex - 1] = nullptr;

    // remove the key
    const size_t        iK1 = HashCode(p->Key1(), NbBuckets());
    IndexedDataMapNode* q   = (IndexedDataMapNode*)myData1[iK1];
    if (q == p)
      myData1[iK1] = (IndexedDataMapNode*)p->Next();
    else
    {
      while (q->Next() != p)
        q = (IndexedDataMapNode*)q->Next();
      q->Next() = p->Next();
    }
    p->~IndexedDataMapNode();
    this->myAllocator->Free(p);
    Decrement();
  }

  //! Remove the key of the given index.
  //! Caution! The index of the last key can be changed.
  void RemoveFromIndex(const int theIndex)
  {
    const int aLastInd = Extent();
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > aLastInd,
                                 "NCollection_IndexedDataMap::Remove");
    if (theIndex != aLastInd)
    {
      Swap(theIndex, aLastInd);
    }
    RemoveLast();
  }

  //! Remove the given key.
  //! Caution! The index of the last key can be changed.
  void RemoveKey(const TheKeyType& theKey1)
  {
    int anIndToRemove = FindIndex(theKey1);
    if (anIndToRemove > 0)
    {
      RemoveFromIndex(anIndToRemove);
    }
  }

  //! FindKey
  const TheKeyType& FindKey(const int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::FindKey");
    IndexedDataMapNode* aNode = (IndexedDataMapNode*)myData2[theIndex - 1];
    return aNode->Key1();
  }

  //! FindFromIndex
  const TheItemType& FindFromIndex(const int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::FindFromIndex");
    IndexedDataMapNode* aNode = (IndexedDataMapNode*)myData2[theIndex - 1];
    return aNode->Value();
  }

  //! operator ()
  const TheItemType& operator()(const int theIndex) const { return FindFromIndex(theIndex); }

  //! ChangeFromIndex
  TheItemType& ChangeFromIndex(const int theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::ChangeFromIndex");
    IndexedDataMapNode* aNode = (IndexedDataMapNode*)myData2[theIndex - 1];
    return aNode->ChangeValue();
  }

  //! operator ()
  TheItemType& operator()(const int theIndex) { return ChangeFromIndex(theIndex); }

  //! FindIndex
  int FindIndex(const TheKeyType& theKey1) const
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->Index();
    }
    return 0;
  }

  //! FindFromKey
  const TheItemType& FindFromKey(const TheKeyType& theKey1) const
  {
    Standard_NoSuchObject_Raise_if(IsEmpty(), "NCollection_IndexedDataMap::FindFromKey");
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->Value();
    }
    throw Standard_NoSuchObject("NCollection_IndexedDataMap::FindFromKey");
  }

  //! ChangeFromKey
  TheItemType& ChangeFromKey(const TheKeyType& theKey1)
  {
    Standard_NoSuchObject_Raise_if(IsEmpty(), "NCollection_IndexedDataMap::ChangeFromKey");
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->ChangeValue();
    }
    throw Standard_NoSuchObject("NCollection_IndexedDataMap::ChangeFromKey");
  }

  //! Seek returns pointer to Item by Key. Returns
  //! NULL if Key was not found.
  const TheItemType* Seek(const TheKeyType& theKey1) const
  {
    return const_cast<NCollection_IndexedDataMap*>(this)->ChangeSeek(theKey1);
  }

  //! ChangeSeek returns modifiable pointer to Item by Key. Returns
  //! NULL if Key was not found.
  TheItemType* ChangeSeek(const TheKeyType& theKey1)
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return &aNode->ChangeValue();
    }
    return nullptr;
  }

  //! Find value for key with copying.
  //! @return true if key was found
  bool FindFromKey(const TheKeyType& theKey1, TheItemType& theValue) const
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      theValue = aNode->Value();
      return true;
    }
    return false;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const bool doReleaseMemory = false)
  {
    clearNodes<IndexedDataMapNode>(doReleaseMemory);
  }

  //! Clear data and reset allocator
  void Clear(const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    Clear(theAllocator != this->myAllocator);
    this->myAllocator =
      (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator());
  }

  //! Destructor
  ~NCollection_IndexedDataMap() { Clear(true); }

  //! Size
  int Size() const noexcept { return Extent(); }

protected:
  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, IndexedDataMapNode*& theNode, size_t& theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return false; // Not found
    for (theNode = (IndexedDataMapNode*)myData1[theHash]; theNode;
         theNode = (IndexedDataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key1(), theKey))
        return true;
    }
    return false; // Not found
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, IndexedDataMapNode*& theNode) const
  {
    if (IsEmpty())
      return false; // Not found
    for (theNode = (IndexedDataMapNode*)myData1[HashCode(theKey, NbBuckets())]; theNode;
         theNode = (IndexedDataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key1(), theKey))
      {
        return true;
      }
    }
    return false; // Not found
  }

  bool IsEqual(const TheKeyType& theKey1, const TheKeyType& theKey2) const
  {
    return myHasher(theKey1, theKey2);
  }

  size_t HashCode(const TheKeyType& theKey, const int theUpperBound) const
  {
    return myHasher(theKey) % theUpperBound + 1;
  }

  //! Implementation helper for Add (returns index).
  //! @tparam K forwarding reference type for key
  //! @tparam V forwarding reference type for value
  //! @param theKey1 key to add
  //! @param theItem item to bind
  //! @return index of the key (new or existing)
  template <typename K, typename V>
  int addImpl(K&& theKey1, V&& theItem)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const int aNewIndex    = Extent() + 1;
    aNode                  = new (this->myAllocator) IndexedDataMapNode(std::forward<K>(theKey1),
                                                       aNewIndex,
                                                       std::forward<V>(theItem),
                                                       myData1[aHash]);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    Increment();
    return aNewIndex;
  }

  //! Implementation helper for Emplace/TryEmplace operations.
  //! @tparam K forwarding reference type for key
  //! @tparam IsTry if true, does not modify existing; if false, overwrites
  //! @tparam ReturnRef if true, returns reference; if false, returns int (index)
  //! @param theKey1 key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return int (index) or TheItemType& depending on ReturnRef
  template <typename K, bool IsTry, bool ReturnRef, typename... Args>
  auto emplaceImpl(K&& theKey1,
                   std::bool_constant<IsTry>,
                   std::bool_constant<ReturnRef>,
                   Args&&... theArgs) -> std::conditional_t<ReturnRef, TheItemType&, int>
  {
    if (Resizable())
      ReSize(Extent());
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      if constexpr (!IsTry)
        aNode->ChangeValue() = TheItemType(std::forward<Args>(theArgs)...);
      if constexpr (ReturnRef)
        return aNode->ChangeValue();
      else
        return aNode->Index();
    }
    const int aNewIndex    = Extent() + 1;
    aNode                  = new (this->myAllocator) IndexedDataMapNode(std::forward<K>(theKey1),
                                                       aNewIndex,
                                                       std::in_place,
                                                       myData1[aHash],
                                                       std::forward<Args>(theArgs)...);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    Increment();
    if constexpr (ReturnRef)
      return aNode->ChangeValue();
    else
      return aNewIndex;
  }

  //! Implementation helper for Bind/TryBind/Bound/TryBound operations.
  //! @tparam K forwarding reference type for key
  //! @tparam V forwarding reference type for value
  //! @tparam IsTry if true, does not modify existing; if false, overwrites
  //! @tparam ReturnRef if true, returns reference; if false, returns bool
  //! @return bool or TheItemType& depending on ReturnRef
  template <typename K, typename V, bool IsTry, bool ReturnRef>
  auto bindImpl(K&& theKey1, V&& theItem, std::bool_constant<IsTry>, std::bool_constant<ReturnRef>)
    -> std::conditional_t<ReturnRef, TheItemType&, bool>
  {
    if (Resizable())
      ReSize(Extent());
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      if constexpr (!IsTry)
        aNode->ChangeValue() = std::forward<V>(theItem);
      if constexpr (ReturnRef)
        return aNode->ChangeValue();
      else
        return false;
    }
    const int aNewIndex    = Extent() + 1;
    aNode                  = new (this->myAllocator) IndexedDataMapNode(std::forward<K>(theKey1),
                                                       aNewIndex,
                                                       std::forward<V>(theItem),
                                                       myData1[aHash]);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    Increment();
    if constexpr (ReturnRef)
      return aNode->ChangeValue();
    else
      return true;
  }

protected:
  Hasher myHasher;
};

#endif

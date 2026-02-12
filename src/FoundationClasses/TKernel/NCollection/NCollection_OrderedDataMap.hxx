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

#ifndef NCollection_OrderedDataMap_HeaderFile
#define NCollection_OrderedDataMap_HeaderFile

#include <NCollection_BaseMap.hxx>
#include <NCollection_DefaultHasher.hxx>
#include <NCollection_ItemsView.hxx>
#include <NCollection_StlIterator.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_NoSuchObject.hxx>

#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

//! @brief Hash map that preserves insertion order.
//!
//! NCollection_OrderedDataMap is an alternative to NCollection_DataMap that
//! maintains a doubly-linked list threaded through the hash nodes, so
//! iteration always follows the order in which key-value pairs were inserted.
//!
//! Key features:
//! - O(1) hash lookup (IsBound, Find, Seek, Contained)
//! - O(1) insertion at tail (Bind, Emplace)
//! - O(1) removal with linked-list unlink (UnBind)
//! - O(1) access to first/last inserted pairs (First, Last, FirstValue, LastValue)
//! - Deterministic iteration order across platforms
//! - Structured binding support via Items() view
//!
//! Best suited for:
//! - Key-value maps that require stable iteration order
//! - Serialization-friendly maps (deterministic output)
//! - Property registries where definition order matters
//! - Cases where NCollection_IndexedDataMap overhead is unnecessary
//!
//! Compared to NCollection_IndexedDataMap:
//! - UnBind is O(1) instead of O(n) (no swap-and-shrink on dense array)
//! - No integer index access (use IndexedDataMap if indices are needed)
//!
//! The OrderedDataMap can be seen as an extended array where the Keys are
//! the indices. For this reason the operator () is defined to fetch an
//! Item from a Key.
//!
//! The number of buckets is managed automatically and grows when the number
//! of keys exceeds the bucket count.
//!
//! @note This class is NOT thread-safe. External synchronization is required
//!       for concurrent access from multiple threads.
//!
//! @tparam TheKeyType  Type of keys
//! @tparam TheItemType Type of values
//! @tparam Hasher      Hash and equality functor (default: NCollection_DefaultHasher)
template <class TheKeyType, class TheItemType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_OrderedDataMap : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

public:
  //! Adaptation of the TListNode to the ordered data map notations.
  //! Extends the hash-chain node with insertion-order linked list pointers.
  class OrderedDataMapNode : public NCollection_TListNode<TheItemType>
  {
  public:
    //! Constructor with copy key and copy item
    OrderedDataMapNode(const TheKeyType&     theKey,
                       const TheItemType&    theItem,
                       NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(theItem, theNext),
          myOrderPrev(nullptr),
          myOrderNext(nullptr),
          myKey(theKey)
    {
    }

    //! Constructor with copy key and move item
    OrderedDataMapNode(const TheKeyType&     theKey,
                       TheItemType&&         theItem,
                       NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(std::forward<TheItemType>(theItem), theNext),
          myOrderPrev(nullptr),
          myOrderNext(nullptr),
          myKey(theKey)
    {
    }

    //! Constructor with move key and copy item
    OrderedDataMapNode(TheKeyType&&          theKey,
                       const TheItemType&    theItem,
                       NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(theItem, theNext),
          myOrderPrev(nullptr),
          myOrderNext(nullptr),
          myKey(std::forward<TheKeyType>(theKey))
    {
    }

    //! Constructor with move key and move item
    OrderedDataMapNode(TheKeyType&& theKey, TheItemType&& theItem, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(std::forward<TheItemType>(theItem), theNext),
          myOrderPrev(nullptr),
          myOrderNext(nullptr),
          myKey(std::forward<TheKeyType>(theKey))
    {
    }

    //! Constructor with in-place value construction
    template <typename K, typename... Args>
    OrderedDataMapNode(K&& theKey,
                       std::in_place_t,
                       NCollection_ListNode* theNext,
                       Args&&... theArgs)
        : NCollection_TListNode<TheItemType>(std::in_place,
                                             theNext,
                                             std::forward<Args>(theArgs)...),
          myOrderPrev(nullptr),
          myOrderNext(nullptr),
          myKey(std::forward<K>(theKey))
    {
    }

    //! Key
    const TheKeyType& Key() const noexcept { return myKey; }

    //! Static deleter to be passed to BaseMap
    static void delNode(NCollection_ListNode*                   theNode,
                        occ::handle<NCollection_BaseAllocator>& theAl) noexcept
    {
      ((OrderedDataMapNode*)theNode)->~OrderedDataMapNode();
      theAl->Free(theNode);
    }

    OrderedDataMapNode* myOrderPrev; //!< Previous node in insertion order
    OrderedDataMapNode* myOrderNext; //!< Next node in insertion order

  private:
    TheKeyType myKey;
  };

public:
  //! Implementation of the Iterator interface.
  //! Iterates in insertion order by walking the doubly-linked list.
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator() noexcept
        : myNode(nullptr)
    {
    }

    //! Constructor
    Iterator(const NCollection_OrderedDataMap& theMap) noexcept
        : myNode(theMap.myFirst)
    {
    }

    //! Query if the end of collection is reached by iterator
    bool More() const noexcept { return myNode != nullptr; }

    //! Make a step along the collection (in insertion order)
    void Next() noexcept
    {
      if (myNode)
        myNode = myNode->myOrderNext;
    }

    //! Value inquiry
    const TheItemType& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_OrderedDataMap::Iterator::Value");
      return myNode->Value();
    }

    //! Value change access
    TheItemType& ChangeValue() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_OrderedDataMap::Iterator::ChangeValue");
      return myNode->ChangeValue();
    }

    //! Key
    const TheKeyType& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_OrderedDataMap::Iterator::Key");
      return myNode->Key();
    }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept { return myNode == theOther.myNode; }

    //! Initialize
    void Initialize(const NCollection_OrderedDataMap& theMap) noexcept { myNode = theMap.myFirst; }

    //! Reset
    void Reset() noexcept { myNode = nullptr; }

  private:
    OrderedDataMapNode* myNode; //!< Current node in insertion-order list
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

public:
  //! View class for key-value pair iteration (mutable).
  using ItemsView =
    NCollection_ItemsView::View<NCollection_OrderedDataMap, KeyValueRef, ItemsExtractor, false>;

  //! View class for key-value pair iteration (const).
  using ConstItemsView = NCollection_ItemsView::
    View<NCollection_OrderedDataMap, ConstKeyValueRef, ConstItemsExtractor, true>;

  //! Returns a view for key-value pair iteration.
  //! Usage: for (auto [aKey, aValue] : aMap.Items())
  ItemsView Items() { return ItemsView(*this); }

  //! Returns a const view for key-value pair iteration.
  //! Usage: for (const auto& [aKey, aValue] : aMap.Items())
  ConstItemsView Items() const { return ConstItemsView(*this); }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty Constructor.
  NCollection_OrderedDataMap()
      : NCollection_BaseMap(1, true, occ::handle<NCollection_BaseAllocator>()),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor
  explicit NCollection_OrderedDataMap(
    const int                                     theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial number of buckets
  //! @param theAllocator custom memory allocator
  explicit NCollection_OrderedDataMap(
    const Hasher&                                 theHasher,
    const int                                     theNbBuckets = 1,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator),
        myHasher(theHasher),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor with custom hasher (move).
  //! @param theHasher custom hasher instance (moved)
  //! @param theNbBuckets initial number of buckets
  //! @param theAllocator custom memory allocator
  explicit NCollection_OrderedDataMap(
    Hasher&&                                      theHasher,
    const int                                     theNbBuckets = 1,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator),
        myHasher(std::move(theHasher)),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Copy constructor
  NCollection_OrderedDataMap(const NCollection_OrderedDataMap& theOther)
      : NCollection_BaseMap(theOther.NbBuckets(), true, theOther.myAllocator),
        myHasher(theOther.myHasher),
        myFirst(nullptr),
        myLast(nullptr)
  {
    const int anExt = theOther.Extent();
    if (anExt <= 0)
      return;
    ReSize(anExt - 1);
    for (Iterator anIter(theOther); anIter.More(); anIter.Next())
      Bind(anIter.Key(), anIter.Value());
  }

  //! Move constructor
  NCollection_OrderedDataMap(NCollection_OrderedDataMap&& theOther) noexcept
      : NCollection_BaseMap(std::forward<NCollection_BaseMap>(theOther)),
        myHasher(std::move(theOther.myHasher)),
        myFirst(theOther.myFirst),
        myLast(theOther.myLast)
  {
    theOther.myFirst = nullptr;
    theOther.myLast  = nullptr;
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_OrderedDataMap& theOther) noexcept
  {
    this->exchangeMapsData(theOther);
    std::swap(myFirst, theOther.myFirst);
    std::swap(myLast, theOther.myLast);
    std::swap(myHasher, theOther.myHasher);
  }

  //! Returns const reference to the hasher.
  const Hasher& GetHasher() const noexcept { return myHasher; }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_OrderedDataMap& Assign(const NCollection_OrderedDataMap& theOther)
  {
    if (this == &theOther)
      return *this;

    Clear();
    int anExt = theOther.Extent();
    if (anExt)
    {
      ReSize(anExt - 1);
      Iterator anIter(theOther);
      for (; anIter.More(); anIter.Next())
        Bind(anIter.Key(), anIter.Value());
    }
    return *this;
  }

  //! Assignment operator
  NCollection_OrderedDataMap& operator=(const NCollection_OrderedDataMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move operator
  NCollection_OrderedDataMap& operator=(NCollection_OrderedDataMap&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    std::swap(myFirst, theOther.myFirst);
    std::swap(myLast, theOther.myLast);
    return *this;
  }

  //! ReSize
  void ReSize(const int N)
  {
    NCollection_ListNode** newdata = nullptr;
    NCollection_ListNode** dummy   = nullptr;
    int                    newBuck;
    if (BeginResize(N, newBuck, newdata, dummy))
    {
      if (myData1)
      {
        OrderedDataMapNode** olddata = (OrderedDataMapNode**)myData1;
        OrderedDataMapNode * p, *q;
        for (int i = 0; i <= NbBuckets(); i++)
        {
          if (olddata[i])
          {
            p = olddata[i];
            while (p)
            {
              const size_t k = HashCode(p->Key(), newBuck);
              q              = (OrderedDataMapNode*)p->Next();
              p->Next()      = newdata[k];
              newdata[k]     = p;
              p              = q;
            }
          }
        }
      }
      EndResize(N, newBuck, newdata, dummy);
    }
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return true if Key was not bound already
  bool Bind(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(theKey, std::false_type{}, std::false_type{}, theItem);
  }

  //! Bind binds Item to Key in map.
  bool Bind(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(std::move(theKey), std::false_type{}, std::false_type{}, theItem);
  }

  //! Bind binds Item to Key in map.
  bool Bind(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(theKey, std::false_type{}, std::false_type{}, std::move(theItem));
  }

  //! Bind binds Item to Key in map.
  bool Bind(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(std::move(theKey), std::false_type{}, std::false_type{}, std::move(theItem));
  }

  //! Bound binds Item to Key in map.
  //! @return pointer to modifiable Item
  TheItemType* Bound(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return &emplaceImpl(theKey, std::false_type{}, std::true_type{}, theItem);
  }

  //! Bound binds Item to Key in map.
  TheItemType* Bound(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return &emplaceImpl(std::move(theKey), std::false_type{}, std::true_type{}, theItem);
  }

  //! Bound binds Item to Key in map.
  TheItemType* Bound(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return &emplaceImpl(theKey, std::false_type{}, std::true_type{}, std::move(theItem));
  }

  //! Bound binds Item to Key in map.
  TheItemType* Bound(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return &emplaceImpl(std::move(theKey), std::false_type{}, std::true_type{}, std::move(theItem));
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  //! @return true if Key was newly bound, false if Key already existed (no replacement)
  bool TryBind(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::false_type{}, theItem);
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  bool TryBind(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::false_type{}, theItem);
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  bool TryBind(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::false_type{}, std::move(theItem));
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  bool TryBind(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::false_type{}, std::move(theItem));
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  //! @return reference to existing or newly bound Item
  TheItemType& TryBound(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::true_type{}, theItem);
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  TheItemType& TryBound(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::true_type{}, theItem);
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  TheItemType& TryBound(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::true_type{}, std::move(theItem));
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  TheItemType& TryBound(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::true_type{}, std::move(theItem));
  }

  //! Emplace constructs value in-place; if key exists, destroys and reconstructs value.
  //! @param theKey  key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return true if key was newly added, false if key already existed (and value was
  //! reconstructed)
  template <typename K, typename... Args>
  bool Emplace(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::false_type{},
                       std::false_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! Emplaced constructs value in-place; if key exists, destroys and reconstructs value.
  //! @param theKey  key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing reconstructed or newly added)
  template <typename K, typename... Args>
  TheItemType& Emplaced(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::false_type{},
                       std::true_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! TryEmplace constructs value in-place only if key not already bound.
  //! @param theKey  key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return true if key was newly added, false if key already existed
  template <typename K, typename... Args>
  bool TryEmplace(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::true_type{},
                       std::false_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! TryEmplaced constructs value in-place only if key not already bound.
  //! @param theKey  key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing or newly added)
  template <typename K, typename... Args>
  TheItemType& TryEmplaced(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::true_type{},
                       std::true_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! IsBound
  bool IsBound(const TheKeyType& theKey) const
  {
    OrderedDataMapNode* p;
    return lookup(theKey, p);
  }

  //! Contained returns optional pair of const references to key and value.
  //! Returns std::nullopt if the key is not found.
  std::optional<
    std::pair<std::reference_wrapper<const TheKeyType>, std::reference_wrapper<const TheItemType>>>
    Contained(const TheKeyType& theKey) const
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return std::nullopt;
    return std::make_pair(std::cref(p->Key()), std::cref(p->Value()));
  }

  //! Contained returns optional pair of const key reference and mutable value reference.
  //! Returns std::nullopt if the key is not found.
  std::optional<
    std::pair<std::reference_wrapper<const TheKeyType>, std::reference_wrapper<TheItemType>>>
    Contained(const TheKeyType& theKey)
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return std::nullopt;
    return std::make_pair(std::cref(p->Key()), std::ref(p->ChangeValue()));
  }

  //! UnBind removes Item Key pair from map
  bool UnBind(const TheKeyType& theKey)
  {
    if (IsEmpty())
      return false;
    OrderedDataMapNode** data = (OrderedDataMapNode**)myData1;
    const size_t         k    = HashCode(theKey, NbBuckets());
    OrderedDataMapNode*  p    = data[k];
    OrderedDataMapNode*  q    = nullptr;
    while (p)
    {
      if (IsEqual(p->Key(), theKey))
      {
        Decrement();
        if (q)
          q->Next() = p->Next();
        else
          data[k] = (OrderedDataMapNode*)p->Next();
        unlinkFromList(p);
        p->~OrderedDataMapNode();
        this->myAllocator->Free(p);
        return true;
      }
      q = p;
      p = (OrderedDataMapNode*)p->Next();
    }
    return false;
  }

  //! Seek returns pointer to Item by Key. Returns
  //! NULL if Key was not bound.
  const TheItemType* Seek(const TheKeyType& theKey) const
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return nullptr;
    return &p->Value();
  }

  //! Find returns the Item for Key. Raises if Key was not bound
  const TheItemType& Find(const TheKeyType& theKey) const
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::Find");
    return p->Value();
  }

  //! Find Item for key with copying.
  //! @return true if key was found
  bool Find(const TheKeyType& theKey, TheItemType& theValue) const
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return false;

    theValue = p->Value();
    return true;
  }

  //! operator ()
  const TheItemType& operator()(const TheKeyType& theKey) const { return Find(theKey); }

  //! ChangeSeek returns modifiable pointer to Item by Key. Returns
  //! NULL if Key was not bound.
  TheItemType* ChangeSeek(const TheKeyType& theKey)
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return nullptr;
    return &p->ChangeValue();
  }

  //! ChangeFind returns mofifiable Item by Key. Raises if Key was not bound
  TheItemType& ChangeFind(const TheKeyType& theKey)
  {
    OrderedDataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::Find");
    return p->ChangeValue();
  }

  //! operator ()
  TheItemType& operator()(const TheKeyType& theKey) { return ChangeFind(theKey); }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const bool doReleaseMemory = false)
  {
    Destroy(OrderedDataMapNode::delNode, doReleaseMemory);
    myFirst = nullptr;
    myLast  = nullptr;
  }

  //! Clear data and reset allocator
  void Clear(const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    Clear(theAllocator != this->myAllocator);
    this->myAllocator =
      (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator());
  }

  //! Destructor
  ~NCollection_OrderedDataMap() override { Clear(true); }

  //! Size
  int Size() const noexcept { return Extent(); }

  //! Returns the first key in insertion order.
  //! @throws Standard_NoSuchObject if map is empty
  const TheKeyType& First() const
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::First");
    return myFirst->Key();
  }

  //! Returns the last key in insertion order.
  //! @throws Standard_NoSuchObject if map is empty
  const TheKeyType& Last() const
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::Last");
    return myLast->Key();
  }

  //! Returns the first value in insertion order.
  //! @throws Standard_NoSuchObject if map is empty
  const TheItemType& FirstValue() const
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::FirstValue");
    return myFirst->Value();
  }

  //! Returns the last value in insertion order.
  //! @throws Standard_NoSuchObject if map is empty
  const TheItemType& LastValue() const
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::LastValue");
    return myLast->Value();
  }

  //! Returns modifiable first value in insertion order.
  //! @throws Standard_NoSuchObject if map is empty
  TheItemType& ChangeFirstValue()
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::ChangeFirstValue");
    return myFirst->ChangeValue();
  }

  //! Returns modifiable last value in insertion order.
  //! @throws Standard_NoSuchObject if map is empty
  TheItemType& ChangeLastValue()
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedDataMap::ChangeLastValue");
    return myLast->ChangeValue();
  }

protected:
  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, OrderedDataMapNode*& theNode) const
  {
    if (IsEmpty())
      return false;
    for (theNode = (OrderedDataMapNode*)myData1[HashCode(theKey, NbBuckets())]; theNode;
         theNode = (OrderedDataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey))
        return true;
    }
    return false;
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, OrderedDataMapNode*& theNode, size_t& theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return false;
    for (theNode = (OrderedDataMapNode*)myData1[theHash]; theNode;
         theNode = (OrderedDataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey))
      {
        return true;
      }
    }
    return false;
  }

  bool IsEqual(const TheKeyType& theKey1, const TheKeyType& theKey2) const
  {
    return myHasher(theKey1, theKey2);
  }

  size_t HashCode(const TheKeyType& theKey, const int theUpperBound) const
  {
    return myHasher(theKey) % theUpperBound + 1;
  }

  //! Append a node to the tail of the insertion-order linked list.
  void appendToList(OrderedDataMapNode* theNode)
  {
    theNode->myOrderPrev = myLast;
    theNode->myOrderNext = nullptr;
    if (myLast)
      myLast->myOrderNext = theNode;
    else
      myFirst = theNode;
    myLast = theNode;
  }

  //! Unlink a node from the insertion-order linked list.
  void unlinkFromList(OrderedDataMapNode* theNode)
  {
    OrderedDataMapNode* aPrev = theNode->myOrderPrev;
    OrderedDataMapNode* aNext = theNode->myOrderNext;
    if (aPrev)
      aPrev->myOrderNext = aNext;
    else
      myFirst = aNext;
    if (aNext)
      aNext->myOrderPrev = aPrev;
    else
      myLast = aPrev;
  }

  //! Implementation helper for Bind/TryBind/Bound/TryBound/Emplace/TryEmplace/Emplaced/TryEmplaced.
  //! @tparam K forwarding reference type for key
  //! @tparam IsTry if true, does not overwrite existing; if false, destroys and reconstructs
  //! @tparam ReturnRef if true, returns reference; if false, returns bool
  //! @param theKey  key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return bool or TheItemType& depending on ReturnRef
  template <typename K, bool IsTry, bool ReturnRef, typename... Args>
  auto emplaceImpl(K&& theKey,
                   std::bool_constant<IsTry>,
                   std::bool_constant<ReturnRef>,
                   Args&&... theArgs) -> std::conditional_t<ReturnRef, TheItemType&, bool>
  {
    if (Resizable())
      ReSize(Extent());
    size_t              aHash;
    OrderedDataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      if constexpr (!IsTry)
      {
        aNode->ChangeValue() = TheItemType(std::forward<Args>(theArgs)...);
      }
      if constexpr (ReturnRef)
        return aNode->ChangeValue();
      else
        return false;
    }
    OrderedDataMapNode** data = (OrderedDataMapNode**)myData1;
    data[aHash]               = new (this->myAllocator) OrderedDataMapNode(std::forward<K>(theKey),
                                                             std::in_place,
                                                             data[aHash],
                                                             std::forward<Args>(theArgs)...);
    appendToList(data[aHash]);
    Increment();
    if constexpr (ReturnRef)
      return data[aHash]->ChangeValue();
    else
      return true;
  }

private:
  Hasher              myHasher;
  OrderedDataMapNode* myFirst; //!< Head of insertion-order linked list
  OrderedDataMapNode* myLast;  //!< Tail of insertion-order linked list
};

#endif

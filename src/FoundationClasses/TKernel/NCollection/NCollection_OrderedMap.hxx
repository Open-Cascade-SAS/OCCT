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

#ifndef NCollection_OrderedMap_HeaderFile
#define NCollection_OrderedMap_HeaderFile

#include <NCollection_BaseMap.hxx>
#include <NCollection_DefaultHasher.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

#include <cstddef>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

//! @brief Hash set that preserves insertion order.
//!
//! NCollection_OrderedMap is an alternative to NCollection_Map that maintains
//! a doubly-linked list threaded through the hash nodes, so iteration always
//! follows the order in which keys were inserted.
//!
//! Key features:
//! - O(1) hash lookup (Contains, Contained)
//! - O(1) insertion at tail (Add, Emplace)
//! - O(1) removal with linked-list unlink (Remove)
//! - O(1) access to first/last inserted keys (First, Last)
//! - Deterministic iteration order across platforms
//!
//! Best suited for:
//! - Membership sets that require stable iteration order
//! - Serialization-friendly key sets (deterministic output)
//! - Cases where NCollection_IndexedMap overhead is unnecessary
//!
//! Compared to NCollection_IndexedMap:
//! - Remove is O(1) instead of O(n) (no swap-and-shrink on dense array)
//! - No integer index access (use IndexedMap if indices are needed)
//!
//! The number of buckets is managed automatically and grows when the number
//! of keys exceeds the bucket count.
//!
//! @note This class is NOT thread-safe. External synchronization is required
//!       for concurrent access from multiple threads.
//!
//! @tparam TheKeyType Type of keys
//! @tparam Hasher     Hash and equality functor (default: NCollection_DefaultHasher)
template <class TheKeyType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_OrderedMap : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;
  typedef TheKeyType value_type;
  typedef Hasher     hasher;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = const TheKeyType&;
  using const_reference = const TheKeyType&;
  using pointer         = const TheKeyType*;
  using const_pointer   = const TheKeyType*;

public:
  //! Adaptation of the TListNode to the ordered map notations.
  //! Extends the hash-chain node with insertion-order linked list pointers.
  class OrderedMapNode : public NCollection_TListNode<TheKeyType>
  {
  public:
    //! Constructor with copy key
    OrderedMapNode(const TheKeyType& theKey, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheKeyType>(theKey, theNext),
          myOrderPrev(nullptr),
          myOrderNext(nullptr)
    {
    }

    //! Constructor with move key
    OrderedMapNode(TheKeyType&& theKey, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheKeyType>(std::forward<TheKeyType>(theKey), theNext),
          myOrderPrev(nullptr),
          myOrderNext(nullptr)
    {
    }

    //! Constructor with in-place key construction
    template <typename... Args>
    OrderedMapNode(std::in_place_t, NCollection_ListNode* theNext, Args&&... theArgs)
        : NCollection_TListNode<TheKeyType>(std::in_place, theNext, std::forward<Args>(theArgs)...),
          myOrderPrev(nullptr),
          myOrderNext(nullptr)
    {
    }

    //! Key
    const TheKeyType& Key() noexcept { return this->Value(); }

    const TheKeyType& Key() const noexcept { return this->Value(); }

    //! Next node in insertion order.
    OrderedMapNode* OrderNext() noexcept { return myOrderNext; }

    const OrderedMapNode* OrderNext() const noexcept { return myOrderNext; }

    //! Static deleter to be passed to BaseMap
    static void delNode(NCollection_ListNode*                   theNode,
                        occ::handle<NCollection_BaseAllocator>& theAl) noexcept
    {
      ((OrderedMapNode*)theNode)->~OrderedMapNode();
      theAl->Free(theNode);
    }

    OrderedMapNode* myOrderPrev; //!< Previous node in insertion order
    OrderedMapNode* myOrderNext; //!< Next node in insertion order
  };

  using iterator = NCollection_BaseMap::
    BasicIterator<NCollection_OrderedMap, OrderedMapNode, TheKeyType, TheKeyType, true, true, true>;
  using const_iterator = iterator;

public:
  //! Legacy OCCT cursor backed by the standard iterator state.
  class Iterator : public const_iterator
  {
  public:
    Iterator() noexcept = default;

    explicit Iterator(const NCollection_OrderedMap& theMap) noexcept
        : const_iterator(static_cast<const OrderedMapNode*>(theMap.myFirst)),
          myMap(&theMap)
    {
    }

    bool More() const noexcept { return !const_iterator::operator==(const_iterator()); }

    void Next() noexcept { ++(*this); }

    const TheKeyType& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_OrderedMap::Iterator::Value");
      return const_iterator::operator*();
    }

    const TheKeyType& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_OrderedMap::Iterator::Key");
      return const_iterator::operator*();
    }

    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return const_iterator::operator==(static_cast<const const_iterator&>(theOther));
    }

    void Initialize(const NCollection_OrderedMap& theMap) noexcept { *this = Iterator(theMap); }

    void Reset() noexcept { *this = myMap != nullptr ? Iterator(*myMap) : Iterator(); }

  private:
    const NCollection_OrderedMap* myMap = nullptr;
  };

  //! Returns an iterator pointing to the first element in the map.
  iterator begin() noexcept { return iterator(*this); }

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator begin() const noexcept { return const_iterator(*this); }

  //! Returns an iterator referring to the past-the-end element in the map.
  iterator end() noexcept { return iterator(); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator end() const noexcept { return const_iterator(); }

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const noexcept { return const_iterator(); }

  //! Returns the first node in insertion order for the shared iterator state.
  OrderedMapNode* FirstNode() noexcept { return myFirst; }

  //! Returns the first node in insertion order for the shared iterator state.
  const OrderedMapNode* FirstNode() const noexcept { return myFirst; }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_OrderedMap()
      : NCollection_BaseMap(1, occ::handle<NCollection_BaseAllocator>()),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor
  explicit NCollection_OrderedMap(
    const size_t                                  theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, theAllocator),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor (legacy int-taking).
  explicit NCollection_OrderedMap(
    const int                                     theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_OrderedMap(NCollection_BaseMap::nbBucketsFromInt(theNbBuckets), theAllocator)
  {
  }

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial number of buckets
  //! @param theAllocator custom memory allocator
  explicit NCollection_OrderedMap(
    const Hasher&                                 theHasher,
    const size_t                                  theNbBuckets = 1,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, theAllocator),
        myHasher(theHasher),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor with custom hasher (copy, legacy int-taking).
  explicit NCollection_OrderedMap(
    const Hasher&                                 theHasher,
    const int                                     theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_OrderedMap(theHasher,
                               NCollection_BaseMap::nbBucketsFromInt(theNbBuckets),
                               theAllocator)
  {
  }

  //! Constructor with custom hasher (move).
  //! @param theHasher custom hasher instance (moved)
  //! @param theNbBuckets initial number of buckets
  //! @param theAllocator custom memory allocator
  explicit NCollection_OrderedMap(
    Hasher&&                                      theHasher,
    const size_t                                  theNbBuckets = 1,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, theAllocator),
        myHasher(std::move(theHasher)),
        myFirst(nullptr),
        myLast(nullptr)
  {
  }

  //! Constructor with custom hasher (move, legacy int-taking).
  explicit NCollection_OrderedMap(
    Hasher&&                                      theHasher,
    const int                                     theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_OrderedMap(std::move(theHasher),
                               NCollection_BaseMap::nbBucketsFromInt(theNbBuckets),
                               theAllocator)
  {
  }

  //! Copy constructor
  NCollection_OrderedMap(const NCollection_OrderedMap& theOther)
      : NCollection_BaseMap(theOther.NbBuckets(), theOther.myAllocator),
        myHasher(theOther.myHasher),
        myFirst(nullptr),
        myLast(nullptr)
  {
    const int anExt = theOther.Extent();
    if (anExt <= 0)
      return;
    ReSize(anExt - 1);
    for (Iterator anIter(theOther); anIter.More(); anIter.Next())
      Add(anIter.Key());
  }

  //! Move constructor
  NCollection_OrderedMap(NCollection_OrderedMap&& theOther) noexcept(
    std::is_nothrow_move_constructible<Hasher>::value)
      : NCollection_BaseMap(theOther.NbBuckets(), theOther.myAllocator),
        myHasher(std::move(theOther.myHasher)),
        myFirst(nullptr),
        myLast(nullptr)
  {
    exchangeMapsData(theOther);
    std::swap(myFirst, theOther.myFirst);
    std::swap(myLast, theOther.myLast);
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_OrderedMap& theOther) noexcept(
    noexcept(std::swap(std::declval<Hasher&>(), std::declval<Hasher&>())))
  {
    std::swap(myHasher, theOther.myHasher);
    std::swap(myFirst, theOther.myFirst);
    std::swap(myLast, theOther.myLast);
    this->exchangeMapsData(theOther);
  }

  //! Returns const reference to the hasher.
  const Hasher& GetHasher() const noexcept { return myHasher; }

  //! Assign.
  //! This method does not change the internal allocator.
  NCollection_OrderedMap& Assign(const NCollection_OrderedMap& theOther)
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
        Add(anIter.Key());
    }
    return *this;
  }

  //! Assign operator
  NCollection_OrderedMap& operator=(const NCollection_OrderedMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move operator
  NCollection_OrderedMap& operator=(NCollection_OrderedMap&& theOther) noexcept(
    noexcept(std::swap(std::declval<Hasher&>(), std::declval<Hasher&>())))
  {
    if (this == &theOther)
      return *this;
    std::swap(myHasher, theOther.myHasher);
    exchangeMapsData(theOther);
    std::swap(myFirst, theOther.myFirst);
    std::swap(myLast, theOther.myLast);
    return *this;
  }

  //! ReSize
  void ReSize(const size_t N)
  {
    NCollection_ListNode** newdata = nullptr;
    size_t                 newBuck;
    if (beginResize(N, newBuck, newdata))
    {
      if (myData1)
      {
        OrderedMapNode** olddata = (OrderedMapNode**)myData1;
        OrderedMapNode * p, *q;
        for (size_t i = 0; i <= NbBuckets(); ++i)
        {
          if (olddata[i])
          {
            p = olddata[i];
            while (p)
            {
              const size_t k = hashCode(p->Key(), newBuck);
              q              = (OrderedMapNode*)p->Next();
              p->Next()      = newdata[k];
              newdata[k]     = p;
              p              = q;
            }
          }
        }
      }
      endResize(N, newBuck, newdata);
    }
  }

  void ReSize(const int N)
  {
    Standard_OutOfRange_Raise_if(N < 0, "NCollection_OrderedMap::ReSize: negative size");
    ReSize(static_cast<size_t>(N));
  }

  //! Add
  bool Add(const TheKeyType& theKey) { return addImpl(theKey, std::false_type{}); }

  //! Add
  bool Add(TheKeyType&& theKey) { return addImpl(std::move(theKey), std::false_type{}); }

  //! Added: add a new key if not yet in the map, and return
  //! reference to either newly added or previously existing object
  const TheKeyType& Added(const TheKeyType& theKey) { return addImpl(theKey, std::true_type{}); }

  //! Added: add a new key if not yet in the map, and return
  //! reference to either newly added or previously existing object
  const TheKeyType& Added(TheKeyType&& theKey)
  {
    return addImpl(std::move(theKey), std::true_type{});
  }

  //! Emplace constructs key in-place; if key exists, destroys and reconstructs.
  //! @param theArgs arguments forwarded to key constructor
  //! @return true if key was newly added, false if key already existed (and was reconstructed)
  template <typename... Args>
  bool Emplace(Args&&... theArgs)
  {
    return emplaceImpl(std::false_type{}, std::false_type{}, std::forward<Args>(theArgs)...);
  }

  //! Emplaced constructs key in-place; if key exists, destroys and reconstructs.
  //! @param theArgs arguments forwarded to key constructor
  //! @return const reference to the key in the map
  template <typename... Args>
  const TheKeyType& Emplaced(Args&&... theArgs)
  {
    return emplaceImpl(std::false_type{}, std::true_type{}, std::forward<Args>(theArgs)...);
  }

  //! TryEmplace constructs key in-place only if not already present.
  //! @param theArgs arguments forwarded to key constructor
  //! @return true if key was newly added, false if key already existed
  template <typename... Args>
  bool TryEmplace(Args&&... theArgs)
  {
    return emplaceImpl(std::true_type{}, std::false_type{}, std::forward<Args>(theArgs)...);
  }

  //! TryEmplaced constructs key in-place only if not already present.
  //! @param theArgs arguments forwarded to key constructor
  //! @return const reference to the key (existing or newly added)
  template <typename... Args>
  const TheKeyType& TryEmplaced(Args&&... theArgs)
  {
    return emplaceImpl(std::true_type{}, std::true_type{}, std::forward<Args>(theArgs)...);
  }

  //! Contains
  bool Contains(const TheKeyType& theKey) const
  {
    OrderedMapNode* p;
    return lookup(theKey, p);
  }

  //! Contained returns optional const reference to the key in the map.
  //! Returns std::nullopt if the key is not found.
  std::optional<std::reference_wrapper<const TheKeyType>> Contained(const TheKeyType& theKey) const
  {
    OrderedMapNode* p;
    if (!lookup(theKey, p))
      return std::nullopt;
    return std::cref(p->Key());
  }

  //! Remove
  bool Remove(const TheKeyType& K)
  {
    if (IsEmpty())
      return false;
    OrderedMapNode** data = (OrderedMapNode**)myData1;
    const size_t     k    = hashCode(K, NbBuckets());
    OrderedMapNode*  p    = data[k];
    OrderedMapNode*  q    = nullptr;
    while (p)
    {
      if (isEqual(p->Key(), K))
      {
        decrement();
        if (q)
          q->Next() = p->Next();
        else
          data[k] = (OrderedMapNode*)p->Next();
        unlinkFromList(p);
        p->~OrderedMapNode();
        this->myAllocator->Free(p);
        return true;
      }
      q = p;
      p = (OrderedMapNode*)p->Next();
    }
    return false;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const bool doReleaseMemory = false)
  {
    destroy(OrderedMapNode::delNode, doReleaseMemory);
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
  ~NCollection_OrderedMap() { Clear(true); }

  //! Returns the first key in insertion order.
  //! @return const reference to the first key
  //! @throws Standard_NoSuchObject if map is empty
  const TheKeyType& First() const
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedMap::First");
    return myFirst->Value();
  }

  //! Returns the last key in insertion order.
  //! @return const reference to the last key
  //! @throws Standard_NoSuchObject if map is empty
  const TheKeyType& Last() const
  {
    if (IsEmpty())
      throw Standard_NoSuchObject("NCollection_OrderedMap::Last");
    return myLast->Value();
  }

protected:
  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, OrderedMapNode*& theNode, size_t& theHash) const
  {
    theHash = hashCode(theKey, NbBuckets());
    if (IsEmpty())
      return false;
    for (theNode = (OrderedMapNode*)myData1[theHash]; theNode;
         theNode = (OrderedMapNode*)theNode->Next())
    {
      if (isEqual(theNode->Key(), theKey))
        return true;
    }
    return false;
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, OrderedMapNode*& theNode) const
  {
    if (IsEmpty())
      return false;
    for (theNode = (OrderedMapNode*)myData1[hashCode(theKey, NbBuckets())]; theNode;
         theNode = (OrderedMapNode*)theNode->Next())
    {
      if (isEqual(theNode->Key(), theKey))
      {
        return true;
      }
    }
    return false;
  }

  bool isEqual(const TheKeyType& theKey1, const TheKeyType& theKey2) const
  {
    return myHasher(theKey1, theKey2);
  }

  size_t hashCode(const TheKeyType& theKey, const size_t theUpperBound) const
  {
    return myHasher(theKey) % theUpperBound + 1;
  }

  //! Append a node to the tail of the insertion-order linked list.
  void appendToList(OrderedMapNode* theNode)
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
  void unlinkFromList(OrderedMapNode* theNode)
  {
    OrderedMapNode* aPrev = theNode->myOrderPrev;
    OrderedMapNode* aNext = theNode->myOrderNext;
    if (aPrev)
      aPrev->myOrderNext = aNext;
    else
      myFirst = aNext;
    if (aNext)
      aNext->myOrderPrev = aPrev;
    else
      myLast = aPrev;
  }

  //! Implementation helper for Add/Added.
  //! @tparam K forwarding reference type for key
  //! @tparam ReturnRef if true, returns const reference to key; if false, returns bool
  //! @param theKey key to add
  //! @return bool (Add) or const TheKeyType& (Added)
  template <typename K, bool ReturnRef>
  auto addImpl(K&& theKey, std::bool_constant<ReturnRef>)
    -> std::conditional_t<ReturnRef, const TheKeyType&, bool>
  {
    if (resizable())
      ReSize(Extent());
    OrderedMapNode* aNode;
    size_t          aHash;
    if (lookup(theKey, aNode, aHash))
    {
      if constexpr (ReturnRef)
        return aNode->Key();
      else
        return false;
    }
    OrderedMapNode** data = (OrderedMapNode**)myData1;
    data[aHash] = new (this->myAllocator) OrderedMapNode(std::forward<K>(theKey), data[aHash]);
    appendToList(data[aHash]);
    increment();
    if constexpr (ReturnRef)
      return data[aHash]->Key();
    else
      return true;
  }

  //! Implementation helper for Emplace/TryEmplace/Emplaced/TryEmplaced.
  //! @tparam IsTry if true, does not modify existing; if false, destroys and reconstructs
  //! @tparam ReturnRef if true, returns const reference to key; if false, returns bool
  //! @param theArgs arguments forwarded to key constructor
  //! @return bool or const TheKeyType& depending on ReturnRef
  template <bool IsTry, bool ReturnRef, typename... Args>
  auto emplaceImpl(std::bool_constant<IsTry>, std::bool_constant<ReturnRef>, Args&&... theArgs)
    -> std::conditional_t<ReturnRef, const TheKeyType&, bool>
  {
    if (resizable())
      ReSize(Extent());
    TheKeyType      aTempKey(std::forward<Args>(theArgs)...);
    OrderedMapNode* aNode;
    size_t          aHash;
    if (lookup(aTempKey, aNode, aHash))
    {
      if constexpr (!IsTry)
      {
        aNode->ChangeValue().~TheKeyType();
        new (&aNode->ChangeValue()) TheKeyType(std::move(aTempKey));
      }
      if constexpr (ReturnRef)
        return aNode->Key();
      else
        return false;
    }
    OrderedMapNode** data = (OrderedMapNode**)myData1;
    data[aHash] = new (this->myAllocator) OrderedMapNode(std::move(aTempKey), data[aHash]);
    appendToList(data[aHash]);
    increment();
    if constexpr (ReturnRef)
      return data[aHash]->Key();
    else
      return true;
  }

protected:
  Hasher          myHasher;
  OrderedMapNode* myFirst; //!< Head of insertion-order linked list
  OrderedMapNode* myLast;  //!< Tail of insertion-order linked list
};

#endif

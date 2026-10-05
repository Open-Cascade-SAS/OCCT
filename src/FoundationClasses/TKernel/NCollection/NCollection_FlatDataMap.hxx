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

#ifndef NCollection_FlatDataMap_HeaderFile
#define NCollection_FlatDataMap_HeaderFile

#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <NCollection_DefaultHasher.hxx>
#include <NCollection_ItemsView.hxx>

#include <cstddef>
#include <functional>
#include <iterator>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

/**
 * @brief High-performance hash map using open addressing with Robin Hood hashing.
 *
 * NCollection_FlatDataMap is an alternative to NCollection_DataMap that provides
 * better cache locality and reduced memory allocation overhead by storing all
 * key-value pairs inline in a contiguous array.
 *
 * Key features:
 * - Open addressing with linear probing (better cache locality)
 * - Robin Hood hashing (reduces probe sequence variance)
 * - Power-of-2 sizing for fast modulo operations
 * - No per-element allocations
 *
 * Typical faster usage patterns:
 * - POD or small key/value types
 * - Performance-critical code paths
 * - Lookup-heavy workloads
 * - Full traversal / iteration-heavy workloads
 * - Stable-size maps with Reserve() called once before bulk Bind()
 *
 * Container-specific implementation notes:
 * - UnBind() keeps probe clusters consistent using backward-shift compaction.
 *
 * Relative to NCollection_DataMap:
 * - Bind()/UnBind() can be faster in many workloads thanks to contiguous storage and
 *   no per-element node allocation.
 * - Iteration is often faster due to contiguous slot scanning and reduced pointer chasing.
 *
 * Limitations:
 * - Keys and values must be movable
 * - Higher memory usage at low load factors
 * - Iteration order is not insertion order
 * - Probe distance grows with collisions (bounded by table capacity)
 *
 * @note This class is NOT thread-safe. External synchronization is required
 *       for concurrent access from multiple threads.
 *
 * @tparam TheKeyType   Type of keys
 * @tparam TheItemType  Type of values
 * @tparam Hasher       Hash and equality functor (default: NCollection_DefaultHasher)
 */
template <class TheKeyType, class TheItemType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_FlatDataMap
{
public:
  //! STL-compliant type alias for key type
  using key_type = TheKeyType;

  //! STL-compliant type alias for value type
  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;

private:
  //! Internal slot structure holding key, value, and metadata.
  //! Key and item storage is uninitialized until state becomes Used.
#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4324) // structure was padded due to alignment specifier
#endif
  struct Slot
  {
    size_t myHash; //!< Cached hash code
    //! Distance from ideal bucket plus one; 0 means Empty, otherwise Used.
    size_t myProbeDistancePlus1;
    alignas(TheKeyType) char myKeyStorage[sizeof(TheKeyType)];
    alignas(TheItemType) char myItemStorage[sizeof(TheItemType)];

    Slot() noexcept
        : myHash(0),
          myProbeDistancePlus1(0)
    {
    }

    TheKeyType& Key() noexcept { return *reinterpret_cast<TheKeyType*>(myKeyStorage); }

    const TheKeyType& Key() const noexcept
    {
      return *reinterpret_cast<const TheKeyType*>(myKeyStorage);
    }

    TheItemType& Item() noexcept { return *reinterpret_cast<TheItemType*>(myItemStorage); }

    const TheItemType& Item() const noexcept
    {
      return *reinterpret_cast<const TheItemType*>(myItemStorage);
    }

    bool IsEmpty() const noexcept { return myProbeDistancePlus1 == 0; }

    bool IsUsed() const noexcept { return myProbeDistancePlus1 != 0; }

    size_t ProbeDistance() const noexcept { return myProbeDistancePlus1 - 1; }

    void SetProbeDistance(const size_t theProbeDistance) noexcept
    {
      myProbeDistancePlus1 = theProbeDistance + 1;
    }

    void SetEmpty() noexcept { myProbeDistancePlus1 = 0; }
  };
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

  static void destroyTable(Slot* theSlots, const size_t theCapacity) noexcept
  {
    if (theSlots == nullptr)
    {
      return;
    }
    for (size_t anIndex = 0; anIndex < theCapacity; ++anIndex)
    {
      if (theSlots[anIndex].IsUsed())
      {
        theSlots[anIndex].Key().~TheKeyType();
        theSlots[anIndex].Item().~TheItemType();
        theSlots[anIndex].SetEmpty();
      }
      theSlots[anIndex].~Slot();
    }
    freeBytes(theSlots);
  }

  template <bool IsConstant>
  class iterator_impl
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = TheItemType;
    using difference_type   = std::ptrdiff_t;
    using pointer   = typename std::conditional<IsConstant, const TheItemType*, TheItemType*>::type;
    using reference = typename std::conditional<IsConstant, const TheItemType&, TheItemType&>::type;
    using slot_pointer   = typename std::conditional<IsConstant, const Slot*, Slot*>::type;
    using container_type = typename std::
      conditional<IsConstant, const NCollection_FlatDataMap, NCollection_FlatDataMap>::type;

    iterator_impl() noexcept = default;

    explicit iterator_impl(container_type& theContainer) noexcept
        : mySlots(theContainer.slotData()),
          myCapacity(theContainer.myCapacity)
    {
      skipEmptySlots();
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    iterator_impl(const iterator_impl<false>& theOther) noexcept
        : mySlots(theOther.mySlots),
          myCapacity(theOther.myCapacity),
          myIndex(theOther.myIndex)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    iterator_impl& operator=(const iterator_impl<false>& theOther) noexcept
    {
      mySlots    = theOther.mySlots;
      myCapacity = theOther.myCapacity;
      myIndex    = theOther.myIndex;
      return *this;
    }

    reference operator*() const noexcept { return mySlots[myIndex].Item(); }

    pointer operator->() const noexcept { return &operator*(); }

    const TheKeyType& Key() const noexcept { return mySlots[myIndex].Key(); }

    bool More() const noexcept { return myIndex < myCapacity; }

    iterator_impl& operator++() noexcept
    {
      ++myIndex;
      skipEmptySlots();
      return *this;
    }

    iterator_impl operator++(int) noexcept
    {
      iterator_impl anOld(*this);
      ++(*this);
      return anOld;
    }

    template <bool theOtherIsConstant>
    bool operator==(const iterator_impl<theOtherIsConstant>& theOther) const noexcept
    {
      return (!More() && !theOther.More())
             || (mySlots == theOther.mySlots && myIndex == theOther.myIndex);
    }

    template <bool theOtherIsConstant>
    bool operator!=(const iterator_impl<theOtherIsConstant>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

  private:
    template <bool>
    friend class iterator_impl;

    void skipEmptySlots() noexcept
    {
      while (myIndex < myCapacity && !mySlots[myIndex].IsUsed())
      {
        ++myIndex;
      }
    }

    slot_pointer mySlots    = nullptr;
    size_t       myCapacity = 0;
    size_t       myIndex    = 0;
  };

public:
  // **************** Iterator interface ****************

  using iterator       = iterator_impl<false>;
  using const_iterator = iterator_impl<true>;

  //! Legacy OCCT cursor backed by the standard iterator state.
  class Iterator : public iterator
  {
  public:
    Iterator() noexcept = default;

    //! Constructor from map
    Iterator(NCollection_FlatDataMap& theMap) noexcept
        : iterator(theMap)
    {
    }

    //! Legacy cursor construction from a const map.
    Iterator(const NCollection_FlatDataMap& theMap) noexcept
        : iterator(const_cast<NCollection_FlatDataMap&>(theMap))
    {
    }

    //! Check if there are more elements
    bool More() const noexcept { return iterator::More(); }

    //! Move to next element
    void Next() noexcept { ++(*this); }

    //! Get current key
    const TheKeyType& Key() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::Key");
      return iterator::Key();
    }

    //! Get current value (const)
    const TheItemType& Value() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::Value");
      return iterator::operator*();
    }

    //! Get current value (mutable)
    TheItemType& ChangeValue() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::ChangeValue");
      return iterator::operator*();
    }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return iterator::operator==(static_cast<const iterator&>(theOther));
    }
  };

public:
  // **************** Constructors and destructor ****************

  //! Default constructor
  NCollection_FlatDataMap() = default;

  //! Constructor with initial capacity hint
  //! @param theNbBuckets initial capacity (will be rounded up to power of 2)
  explicit NCollection_FlatDataMap(const size_t theNbBuckets)
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial capacity hint
  explicit NCollection_FlatDataMap(const Hasher& theHasher, const size_t theNbBuckets = 0)
      : myHasher(theHasher)
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Constructor with custom hasher (move).
  //! @param theHasher custom hasher instance (moved)
  //! @param theNbBuckets initial capacity hint
  explicit NCollection_FlatDataMap(Hasher&& theHasher, const size_t theNbBuckets = 0)
      : myHasher(std::move(theHasher))
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Copy constructor
  NCollection_FlatDataMap(const NCollection_FlatDataMap& theOther)
      : myHasher(theOther.myHasher)
  {
    if (theOther.mySize > 0)
    {
      // Allocate same capacity as the source (not through reserve which may change capacity)
      mySlots = static_cast<Slot*>(allocateBytes(theOther.myCapacity * sizeof(Slot)));
      for (size_t i = 0; i < theOther.myCapacity; ++i)
      {
        new (&slotData()[i]) Slot();
        if (theOther.slotData()[i].IsUsed())
        {
          new (&slotData()[i].Key()) TheKeyType(theOther.slotData()[i].Key());
          new (&slotData()[i].Item()) TheItemType(theOther.slotData()[i].Item());
          slotData()[i].myHash               = theOther.slotData()[i].myHash;
          slotData()[i].myProbeDistancePlus1 = theOther.slotData()[i].myProbeDistancePlus1;
        }
      }
      myCapacity = theOther.myCapacity;
      mySize     = theOther.mySize;
    }
  }

  //! Move constructor
  NCollection_FlatDataMap(NCollection_FlatDataMap&& theOther) noexcept(
    std::is_nothrow_move_constructible<Hasher>::value)
      : myHasher(std::move(theOther.myHasher))
  {
    exchangeStorage(theOther);
  }

  //! Destructor
  ~NCollection_FlatDataMap() { Clear(true); }

  //! Copy assignment
  NCollection_FlatDataMap& operator=(const NCollection_FlatDataMap& theOther)
  {
    if (this != &theOther)
    {
      Clear(true);
      myHasher = theOther.myHasher;
      if (theOther.mySize > 0)
      {
        // Allocate same capacity as the source (not through reserve which may change capacity)
        mySlots = static_cast<Slot*>(allocateBytes(theOther.myCapacity * sizeof(Slot)));
        for (size_t i = 0; i < theOther.myCapacity; ++i)
        {
          new (&slotData()[i]) Slot();
          if (theOther.slotData()[i].IsUsed())
          {
            new (&slotData()[i].Key()) TheKeyType(theOther.slotData()[i].Key());
            new (&slotData()[i].Item()) TheItemType(theOther.slotData()[i].Item());
            slotData()[i].myHash               = theOther.slotData()[i].myHash;
            slotData()[i].myProbeDistancePlus1 = theOther.slotData()[i].myProbeDistancePlus1;
          }
        }
        myCapacity = theOther.myCapacity;
        mySize     = theOther.mySize;
      }
    }
    return *this;
  }

  //! Move assignment
  NCollection_FlatDataMap& operator=(NCollection_FlatDataMap&& theOther) noexcept(
    std::is_nothrow_move_assignable<Hasher>::value)
  {
    if (this != &theOther)
    {
      Clear(true);
      exchangeStorage(theOther);
      myHasher = std::move(theOther.myHasher);
    }
    return *this;
  }

public:
  // **************** Query methods ****************

  //! Returns number of elements.
  size_t Size() const noexcept { return mySize; }

  //! Returns number of elements (legacy int-returning API, convention shared with BaseMap).
  int Extent() const noexcept { return static_cast<int>(mySize); }

  //! Returns true if map is empty
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Returns current capacity
  size_t Capacity() const noexcept { return myCapacity; }

  //! Check if key exists
  bool IsBound(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return false;
    size_t anIndex = 0;
    return findSlotIndex(theKey, anIndex);
  }

  //! Contained returns optional pair of const references to key and value.
  //! Returns std::nullopt if the key is not found.
  std::optional<
    std::pair<std::reference_wrapper<const TheKeyType>, std::reference_wrapper<const TheItemType>>>
    Contained(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return std::nullopt;
    size_t aIdx = 0;
    if (!findSlotIndex(theKey, aIdx))
      return std::nullopt;
    return std::make_pair(std::cref(slotData()[aIdx].Key()), std::cref(slotData()[aIdx].Item()));
  }

  //! Contained returns optional pair of const key reference and mutable value reference.
  //! Returns std::nullopt if the key is not found.
  std::optional<
    std::pair<std::reference_wrapper<const TheKeyType>, std::reference_wrapper<TheItemType>>>
    Contained(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return std::nullopt;
    size_t aIdx = 0;
    if (!findSlotIndex(theKey, aIdx))
      return std::nullopt;
    return std::make_pair(std::cref(slotData()[aIdx].Key()), std::ref(slotData()[aIdx].Item()));
  }

  //! Find value by key, returns nullptr if not found
  const TheItemType* Seek(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return nullptr;
    size_t aFoundIndex = 0;
    if (findSlotIndex(theKey, aFoundIndex))
    {
      return &slotData()[aFoundIndex].Item();
    }
    return nullptr;
  }

  //! Find value by key (mutable), returns nullptr if not found
  TheItemType* ChangeSeek(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return nullptr;
    size_t aFoundIndex = 0;
    if (findSlotIndex(theKey, aFoundIndex))
    {
      return &slotData()[aFoundIndex].Item();
    }
    return nullptr;
  }

  //! Find value by key, throws if not found
  const TheItemType& Find(const TheKeyType& theKey) const
  {
    const TheItemType* aPtr = Seek(theKey);
    if (aPtr == nullptr)
    {
      throw Standard_NoSuchObject("NCollection_FlatDataMap::Find");
    }
    return *aPtr;
  }

  //! Find value by key (mutable), throws if not found
  TheItemType& ChangeFind(const TheKeyType& theKey)
  {
    TheItemType* aPtr = ChangeSeek(theKey);
    if (aPtr == nullptr)
    {
      throw Standard_NoSuchObject("NCollection_FlatDataMap::ChangeFind");
    }
    return *aPtr;
  }

  //! Operator() for const access
  const TheItemType& operator()(const TheKeyType& theKey) const { return Find(theKey); }

  //! Operator() for mutable access
  TheItemType& operator()(const TheKeyType& theKey) { return ChangeFind(theKey); }

public:
  // **************** Modification methods ****************

  //! Bind key to value
  //! @return true if key was newly added, false if existing key was updated
  bool Bind(const TheKeyType& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return insertImpl(theKey, theItem);
  }

  //! Bind key to value (move semantics for value)
  bool Bind(const TheKeyType& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return insertImpl(theKey, std::forward<TheItemType>(theItem));
  }

  //! Bind key to value (move semantics for key)
  bool Bind(TheKeyType&& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return insertImpl(std::forward<TheKeyType>(theKey), theItem);
  }

  //! Bind key to value (move semantics for both)
  bool Bind(TheKeyType&& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return insertImpl(std::forward<TheKeyType>(theKey), std::forward<TheItemType>(theItem));
  }

  //! TryBind binds key to value only if key is not yet bound.
  //! @param theKey key to add
  //! @param theItem item to bind if key is not yet bound
  //! @return true if key was newly added, false if key already existed
  bool TryBind(const TheKeyType& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return tryInsertImpl(theKey, theItem);
  }

  //! TryBind binds key to value only if key is not yet bound.
  bool TryBind(const TheKeyType& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return tryInsertImpl(theKey, std::move(theItem));
  }

  //! TryBind binds key to value only if key is not yet bound.
  bool TryBind(TheKeyType&& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return tryInsertImpl(std::move(theKey), theItem);
  }

  //! TryBind binds key to value only if key is not yet bound.
  bool TryBind(TheKeyType&& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return tryInsertImpl(std::move(theKey), std::move(theItem));
  }

  //! Bound binds key to value and returns reference to the value.
  //! @param theKey key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return reference to the value in the map
  TheItemType& Bound(const TheKeyType& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return insertRefImpl(theKey, theItem, std::false_type{});
  }

  //! Bound binds key to value and returns reference to the value.
  TheItemType& Bound(const TheKeyType& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return insertRefImpl(theKey, std::move(theItem), std::false_type{});
  }

  //! Bound binds key to value and returns reference to the value.
  TheItemType& Bound(TheKeyType&& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return insertRefImpl(std::move(theKey), theItem, std::false_type{});
  }

  //! Bound binds key to value and returns reference to the value.
  TheItemType& Bound(TheKeyType&& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return insertRefImpl(std::move(theKey), std::move(theItem), std::false_type{});
  }

  //! TryBound binds key to value only if key is not yet bound.
  //! @param theKey key to add
  //! @param theItem item to bind if key is not yet bound
  //! @return reference to existing or newly bound value
  TheItemType& TryBound(const TheKeyType& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return insertRefImpl(theKey, theItem, std::true_type{});
  }

  //! TryBound binds key to value only if key is not yet bound.
  TheItemType& TryBound(const TheKeyType& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return insertRefImpl(theKey, std::move(theItem), std::true_type{});
  }

  //! TryBound binds key to value only if key is not yet bound.
  TheItemType& TryBound(TheKeyType&& theKey, const TheItemType& theItem)
  {
    ensureCapacity();
    return insertRefImpl(std::move(theKey), theItem, std::true_type{});
  }

  //! TryBound binds key to value only if key is not yet bound.
  TheItemType& TryBound(TheKeyType&& theKey, TheItemType&& theItem)
  {
    ensureCapacity();
    return insertRefImpl(std::move(theKey), std::move(theItem), std::true_type{});
  }

  //! Emplace constructs value in-place; if key exists, updates the value.
  //! @param theKey key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return true if key was newly added, false if existing key was updated
  template <typename K, typename... Args>
  bool Emplace(K&& theKey, Args&&... theArgs)
  {
    ensureCapacity();
    return emplaceImpl(std::forward<K>(theKey), std::false_type{}, std::forward<Args>(theArgs)...);
  }

  //! Emplaced constructs value in-place; if key exists, updates the value.
  //! @param theKey key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing updated or newly added)
  template <typename K, typename... Args>
  TheItemType& Emplaced(K&& theKey, Args&&... theArgs)
  {
    ensureCapacity();
    return emplacedImpl(std::forward<K>(theKey), std::false_type{}, std::forward<Args>(theArgs)...);
  }

  //! TryEmplace constructs value in-place only if key not already bound.
  //! @param theKey key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return true if key was newly added, false if key already existed
  template <typename K, typename... Args>
  bool TryEmplace(K&& theKey, Args&&... theArgs)
  {
    ensureCapacity();
    return emplaceImpl(std::forward<K>(theKey), std::true_type{}, std::forward<Args>(theArgs)...);
  }

  //! TryEmplaced constructs value in-place only if key not already bound.
  //! @param theKey key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing or newly added)
  template <typename K, typename... Args>
  TheItemType& TryEmplaced(K&& theKey, Args&&... theArgs)
  {
    ensureCapacity();
    return emplacedImpl(std::forward<K>(theKey), std::true_type{}, std::forward<Args>(theArgs)...);
  }

  //! Remove key from map
  //! @return true if key was found and removed
  bool UnBind(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return false;

    size_t aFoundIndex = 0;
    if (!findSlotIndex(theKey, aFoundIndex))
    {
      return false;
    }

    const size_t aIndex = aFoundIndex;

    slotData()[aIndex].Key().~TheKeyType();
    slotData()[aIndex].Item().~TheItemType();
    slotData()[aIndex].SetEmpty();
    --mySize;

    backwardShiftDelete(aIndex);

    return true;
  }

  //! Clear all elements
  //! @param doReleaseMemory if true, free the internal buffer
  void Clear(bool doReleaseMemory = false)
  {
    if (slotData() != nullptr)
    {
      for (size_t i = 0; i < myCapacity; ++i)
      {
        if (slotData()[i].IsUsed())
        {
          slotData()[i].Key().~TheKeyType();
          slotData()[i].Item().~TheItemType();
          slotData()[i].SetEmpty();
        }
      }
      mySize = 0;

      if (doReleaseMemory)
      {
        releaseStorage(true);
      }
    }
  }

  //! Exchange content with another map
  void Exchange(NCollection_FlatDataMap& theOther) noexcept(
    noexcept(std::swap(std::declval<Hasher&>(), std::declval<Hasher&>())))
  {
    exchangeStorage(theOther);
    std::swap(myHasher, theOther.myHasher);
  }

  //! Returns const reference to the hasher.
  const Hasher& GetHasher() const noexcept { return myHasher; }

  //! Reserve capacity for at least theN elements
  void reserve(size_t theN)
  {
    const size_t aNewCapacity = capacityFor(theN);
    if (aNewCapacity > myCapacity)
    {
      rehash(aNewCapacity);
    }
  }

  //! Reserve capacity for at least theN elements
  void Reserve(const size_t theN) { reserve(theN); }

public:
  // **************** Iterator access ****************

  //! Returns iterator to first element
  iterator begin() noexcept { return iterator(*this); }

  const_iterator begin() const noexcept { return const_iterator(*this); }

  //! Returns iterator past the end
  iterator end() noexcept { return iterator(); }

  const_iterator end() const noexcept { return const_iterator(); }

  //! Returns iterator to first element
  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  //! Returns iterator past the end
  const_iterator cend() const noexcept { return const_iterator(); }

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
    template <class TheIterator>
    static KeyValueRef Extract(const TheIterator& theIter)
    {
      return {theIter.Key(), *theIter};
    }
  };

  //! Extractor for const key-value pairs
  struct ConstItemsExtractor
  {
    template <class TheIterator>
    static ConstKeyValueRef Extract(const TheIterator& theIter)
    {
      return {theIter.Key(), *theIter};
    }
  };

public:
  //! View class for key-value pair iteration (mutable).
  using ItemsView =
    NCollection_ItemsView::View<NCollection_FlatDataMap, KeyValueRef, ItemsExtractor, false>;

  //! View class for key-value pair iteration (const).
  using ConstItemsView = NCollection_ItemsView::
    View<NCollection_FlatDataMap, ConstKeyValueRef, ConstItemsExtractor, true>;

  //! Returns a view for key-value pair iteration.
  //! Usage: for (auto [aKey, aValue] : aMap.Items())
  ItemsView Items() { return ItemsView(*this); }

  //! Returns a const view for key-value pair iteration.
  //! Usage: for (const auto& [aKey, aValue] : aMap.Items())
  ConstItemsView Items() const { return ConstItemsView(*this); }

private:
  // **************** Internal implementation ****************

  static size_t capacityFor(const size_t theElementCount)
  {
    constexpr size_t THE_DEFAULT_CAPACITY     = 8;
    constexpr size_t THE_MAX_LOAD_NUMERATOR   = 13;
    constexpr size_t THE_MAX_LOAD_DENOMINATOR = 16;

    if (theElementCount == 0)
    {
      return THE_DEFAULT_CAPACITY;
    }

    size_t aCapacity = (theElementCount * THE_MAX_LOAD_DENOMINATOR + THE_MAX_LOAD_NUMERATOR - 1)
                       / THE_MAX_LOAD_NUMERATOR;
    if (aCapacity < THE_DEFAULT_CAPACITY)
    {
      aCapacity = THE_DEFAULT_CAPACITY;
    }

    --aCapacity;
    aCapacity |= aCapacity >> 1;
    aCapacity |= aCapacity >> 2;
    aCapacity |= aCapacity >> 4;
    aCapacity |= aCapacity >> 8;
    aCapacity |= aCapacity >> 16;
    if constexpr (sizeof(size_t) > 4)
    {
      aCapacity |= aCapacity >> 32;
    }
    return aCapacity + 1;
  }

  size_t nextCapacity() const
  {
    constexpr size_t THE_DEFAULT_CAPACITY = 8;
    if (myCapacity == 0)
    {
      return THE_DEFAULT_CAPACITY;
    }

    return myCapacity * 2;
  }

  bool needsGrowth() const { return myCapacity == 0 || capacityFor(mySize + 1) > myCapacity; }

  static void* allocateBytes(const size_t theBytes) { return Standard::Allocate(theBytes); }

  static void freeBytes(void* theAddress) noexcept { Standard::Free(theAddress); }

  void releaseStorage(const bool doReleaseMemory) noexcept
  {
    if (doReleaseMemory)
    {
      Standard::Free(mySlots);
      mySlots    = nullptr;
      myCapacity = 0;
    }
  }

  void exchangeStorage(NCollection_FlatDataMap& theOther) noexcept
  {
    std::swap(mySlots, theOther.mySlots);
    std::swap(myCapacity, theOther.myCapacity);
    std::swap(mySize, theOther.mySize);
  }

  //! Get next power of 2 >= n
  //! Ensure there's room for at least one more element
  void ensureCapacity()
  {
    // Grow at ~81.25% load factor.
    if (needsGrowth())
    {
      rehash(nextCapacity());
    }
  }

  //! Rehash to new capacity
  void rehash(size_t theNewCapacity)
  {
    Slot*        aOldSlots    = slotData();
    const size_t aOldCapacity = myCapacity;

    mySlots = static_cast<Slot*>(allocateBytes(theNewCapacity * sizeof(Slot)));
    for (size_t i = 0; i < theNewCapacity; ++i)
    {
      new (&slotData()[i]) Slot();
    }
    myCapacity = theNewCapacity;
    mySize     = 0;

    if (aOldSlots != nullptr)
    {
      for (size_t i = 0; i < aOldCapacity; ++i)
      {
        if (aOldSlots[i].IsUsed())
        {
          insertRehashedImpl(std::move_if_noexcept(aOldSlots[i].Key()),
                             std::move_if_noexcept(aOldSlots[i].Item()),
                             aOldSlots[i].myHash);
          aOldSlots[i].Key().~TheKeyType();
          aOldSlots[i].Item().~TheItemType();
          aOldSlots[i].SetEmpty();
        }
      }
      for (size_t i = 0; i < aOldCapacity; ++i)
      {
        aOldSlots[i].~Slot();
      }
      freeBytes(aOldSlots);
    }
  }

  //! Find slot containing key.
  //! @param theKey key to find
  //! @param[out] theIndex found index
  //! @return true if key was found
  bool findSlotIndex(const TheKeyType& theKey, size_t& theIndex) const
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    size_t       aProbe = 0;

    while (true)
    {
      const Slot& aSlot = slotData()[aIndex];

      if (aSlot.IsEmpty())
      {
        return false;
      }

      if (aProbe > aSlot.ProbeDistance())
      {
        return false;
      }

      if (aSlot.myHash == aHash && myHasher(aSlot.Key(), theKey))
      {
        theIndex = aIndex;
        return true;
      }
      aIndex = (aIndex + 1) & aMask;
      ++aProbe;
    }
  }

  template <typename K, typename V, bool CheckExisting, bool UpdateExisting>
  bool insertRehashedImpl(K&&          theKey,
                          V&&          theItem,
                          const size_t theHash,
                          std::bool_constant<CheckExisting>,
                          std::bool_constant<UpdateExisting>,
                          size_t* theInsertedIndex = nullptr)
  {
    const size_t aMask             = myCapacity - 1;
    size_t       aIndex            = theHash & aMask;
    size_t       aProbe            = 0;
    size_t       anInsertedIndex   = 0;
    bool         aHasInsertedIndex = false;

    TheKeyType  aKeyToInsert  = std::forward<K>(theKey);
    TheItemType aItemToInsert = std::forward<V>(theItem);
    size_t      aHashToInsert = theHash;

    while (true)
    {
      Slot& aSlot = slotData()[aIndex];
      if (aSlot.IsEmpty())
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::move(aItemToInsert));
        aSlot.myHash = aHashToInsert;
        aSlot.SetProbeDistance(aProbe);
        ++mySize;
        if (theInsertedIndex != nullptr)
        {
          *theInsertedIndex = aHasInsertedIndex ? anInsertedIndex : aIndex;
        }
        return true;
      }

      if constexpr (CheckExisting)
      {
        if (aSlot.myHash == aHashToInsert && myHasher(aSlot.Key(), aKeyToInsert))
        {
          if constexpr (UpdateExisting)
          {
            aSlot.Item() = std::move(aItemToInsert);
          }
          if (theInsertedIndex != nullptr)
          {
            *theInsertedIndex = aIndex;
          }
          return false;
        }
      }

      if (aProbe > aSlot.ProbeDistance())
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp = aProbe;
        aProbe            = aSlot.ProbeDistance();
        aSlot.SetProbeDistance(aTmp);
        if (!aHasInsertedIndex)
        {
          anInsertedIndex   = aIndex;
          aHasInsertedIndex = true;
        }
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
  }

  template <typename K, typename V>
  void insertRehashedImpl(K&& theKey, V&& theItem, const size_t theHash)
  {
    (void)insertRehashedImpl(std::forward<K>(theKey),
                             std::forward<V>(theItem),
                             theHash,
                             std::false_type{},
                             std::false_type{});
  }

  template <typename K, typename V>
  bool insertImpl(K&& theKey, V&& theItem)
  {
    const size_t aHash = myHasher(theKey);
    return insertRehashedImpl(std::forward<K>(theKey),
                              std::forward<V>(theItem),
                              aHash,
                              std::true_type{},
                              std::true_type{});
  }

  template <typename K, typename V>
  bool tryInsertImpl(K&& theKey, V&& theItem)
  {
    const size_t aHash = myHasher(theKey);
    return insertRehashedImpl(std::forward<K>(theKey),
                              std::forward<V>(theItem),
                              aHash,
                              std::true_type{},
                              std::false_type{});
  }

  template <typename K, typename V, bool IsTry>
  TheItemType& insertRefImpl(K&& theKey, V&& theItem, std::bool_constant<IsTry>)
  {
    const size_t aHash  = myHasher(theKey);
    size_t       aIndex = 0;
    if constexpr (IsTry)
    {
      (void)insertRehashedImpl(std::forward<K>(theKey),
                               std::forward<V>(theItem),
                               aHash,
                               std::true_type{},
                               std::false_type{},
                               &aIndex);
    }
    else
    {
      (void)insertRehashedImpl(std::forward<K>(theKey),
                               std::forward<V>(theItem),
                               aHash,
                               std::true_type{},
                               std::true_type{},
                               &aIndex);
    }
    return slotData()[aIndex].Item();
  }

  template <typename K, bool IsTry, typename... Args>
  bool emplaceImpl(K&& theKey, std::bool_constant<IsTry>, Args&&... theArgs)
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    size_t       aProbe = 0;

    TheKeyType aKeyToInsert  = std::forward<K>(theKey);
    size_t     aHashToInsert = aHash;

    while (true)
    {
      Slot& aSlot = slotData()[aIndex];

      if (aSlot.IsEmpty())
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::forward<Args>(theArgs)...);
        aSlot.myHash = aHashToInsert;
        aSlot.SetProbeDistance(aProbe);
        ++mySize;
        return true;
      }

      if (aSlot.myHash == aHashToInsert && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Item() = TheItemType(std::forward<Args>(theArgs)...);
        return false;
      }

      if (aProbe > aSlot.ProbeDistance())
      {
        TheItemType aItemToInsert(std::forward<Args>(theArgs)...);

        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp = aProbe;
        aProbe            = aSlot.ProbeDistance();
        aSlot.SetProbeDistance(aTmp);

        ++aProbe;
        aIndex = (aIndex + 1) & aMask;

        while (true)
        {
          Slot& aSlot2 = slotData()[aIndex];

          if (aSlot2.IsEmpty())
          {
            new (&aSlot2.Key()) TheKeyType(std::move(aKeyToInsert));
            new (&aSlot2.Item()) TheItemType(std::move(aItemToInsert));
            aSlot2.myHash = aHashToInsert;
            aSlot2.SetProbeDistance(aProbe);
            ++mySize;
            return true;
          }

          if (aProbe > aSlot2.ProbeDistance())
          {
            std::swap(aKeyToInsert, aSlot2.Key());
            std::swap(aItemToInsert, aSlot2.Item());
            std::swap(aHashToInsert, aSlot2.myHash);
            const size_t aTmp2 = aProbe;
            aProbe             = aSlot2.ProbeDistance();
            aSlot2.SetProbeDistance(aTmp2);
          }

          ++aProbe;
          aIndex = (aIndex + 1) & aMask;
        }
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
  }

  template <typename K, bool IsTry, typename... Args>
  TheItemType& emplacedImpl(K&& theKey, std::bool_constant<IsTry>, Args&&... theArgs)
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    size_t       aProbe = 0;

    TheKeyType aKeyToInsert  = std::forward<K>(theKey);
    size_t     aHashToInsert = aHash;

    while (true)
    {
      Slot& aSlot = slotData()[aIndex];

      if (aSlot.IsEmpty())
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::forward<Args>(theArgs)...);
        aSlot.myHash = aHashToInsert;
        aSlot.SetProbeDistance(aProbe);
        ++mySize;
        return aSlot.Item();
      }

      if (aSlot.myHash == aHashToInsert && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Item() = TheItemType(std::forward<Args>(theArgs)...);
        return aSlot.Item();
      }

      if (aProbe > aSlot.ProbeDistance())
      {
        TheItemType aItemToInsert(std::forward<Args>(theArgs)...);

        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp = aProbe;
        aProbe            = aSlot.ProbeDistance();
        aSlot.SetProbeDistance(aTmp);

        TheItemType& aResult = aSlot.Item();

        ++aProbe;
        aIndex = (aIndex + 1) & aMask;

        while (true)
        {
          Slot& aSlot2 = slotData()[aIndex];

          if (aSlot2.IsEmpty())
          {
            new (&aSlot2.Key()) TheKeyType(std::move(aKeyToInsert));
            new (&aSlot2.Item()) TheItemType(std::move(aItemToInsert));
            aSlot2.myHash = aHashToInsert;
            aSlot2.SetProbeDistance(aProbe);
            ++mySize;
            return aResult;
          }

          if (aProbe > aSlot2.ProbeDistance())
          {
            std::swap(aKeyToInsert, aSlot2.Key());
            std::swap(aItemToInsert, aSlot2.Item());
            std::swap(aHashToInsert, aSlot2.myHash);
            const size_t aTmp2 = aProbe;
            aProbe             = aSlot2.ProbeDistance();
            aSlot2.SetProbeDistance(aTmp2);
          }

          ++aProbe;
          aIndex = (aIndex + 1) & aMask;
        }
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
  }

  void backwardShiftDelete(size_t theIndex)
  {
    const size_t aMask    = myCapacity - 1;
    size_t       aCurrent = theIndex;
    size_t       aNext    = (aCurrent + 1) & aMask;

    while (slotData()[aNext].IsUsed() && slotData()[aNext].ProbeDistance() > 0)
    {
      new (&slotData()[aCurrent].Key()) TheKeyType(std::move(slotData()[aNext].Key()));
      new (&slotData()[aCurrent].Item()) TheItemType(std::move(slotData()[aNext].Item()));
      slotData()[aCurrent].myHash = slotData()[aNext].myHash;
      slotData()[aCurrent].SetProbeDistance(slotData()[aNext].ProbeDistance() - 1);

      slotData()[aNext].Key().~TheKeyType();
      slotData()[aNext].Item().~TheItemType();

      aCurrent = aNext;
      aNext    = (aNext + 1) & aMask;
    }

    slotData()[aCurrent].SetEmpty();
  }

private:
  Slot* slotData() noexcept { return static_cast<Slot*>(mySlots); }

  const Slot* slotData() const noexcept { return static_cast<const Slot*>(mySlots); }

  void*  mySlots    = nullptr;
  size_t myCapacity = 0;
  size_t mySize     = 0;
  Hasher myHasher; //!< Hash and equality functor
};

#endif // NCollection_FlatDataMap_HeaderFile

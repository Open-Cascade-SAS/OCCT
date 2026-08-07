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

#ifndef NCollection_FlatMap_HeaderFile
#define NCollection_FlatMap_HeaderFile

#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_DefaultHasher.hxx>

#include <cstddef>
#include <functional>
#include <iterator>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

/**
 * @brief High-performance hash set using open addressing with Robin Hood hashing.
 *
 * NCollection_FlatMap is an alternative to NCollection_Map that provides
 * better cache locality and reduced memory allocation overhead by storing all
 * keys inline in a contiguous array.
 *
 * Key features:
 * - Open addressing with linear probing (better cache locality)
 * - Robin Hood hashing (reduces probe sequence variance)
 * - Power-of-2 sizing for fast modulo operations
 * - No per-element allocations
 *
 * Typical faster usage patterns:
 * - POD or small key types
 * - Performance-critical code paths
 * - Lookup-heavy workloads (Contains()/Seek())
 * - Full traversal / iteration-heavy workloads
 * - Stable-size maps with Reserve() called once before bulk insert
 *
 * Container-specific implementation notes:
 * - Remove() keeps probe clusters consistent using backward-shift compaction.
 *
 * Relative to NCollection_Map:
 * - Add()/Remove() can be faster in many workloads thanks to contiguous storage and
 *   no per-element node allocation.
 * - Iteration is often faster due to contiguous slot scanning and reduced pointer chasing.
 *
 * Limitations:
 * - Keys must be movable
 * - Higher memory usage at low load factors
 * - Iteration order is not insertion order
 * - Probe distance grows with collisions (bounded by table capacity)
 *
 * @note This class is NOT thread-safe. External synchronization is required
 *       for concurrent access from multiple threads.
 *
 * @tparam TheKeyType Type of keys
 * @tparam Hasher     Hash and equality functor (default: NCollection_DefaultHasher)
 */
template <class TheKeyType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_FlatMap
{
public:
  //! STL-compliant type alias for key type
  using key_type = TheKeyType;
  using value_type = TheKeyType;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = const TheKeyType&;
  using const_reference = const TheKeyType&;
  using pointer = const TheKeyType*;
  using const_pointer = const TheKeyType*;

private:
  //! Internal slot structure holding key and metadata.
  //! Key storage is uninitialized until state becomes Used.
  struct Slot
  {
    alignas(TheKeyType) char myKeyStorage[sizeof(TheKeyType)]; //!< Uninitialized key storage
    size_t myHash;                                             //!< Cached hash code
    //! Distance from ideal bucket plus one; 0 means Empty, otherwise Used.
    size_t myProbeDistancePlus1;

    Slot() noexcept
        : myHash(0),
          myProbeDistancePlus1(0)
    {
      // Key is NOT constructed - myKeyStorage is uninitialized
    }

    //! Access the key (only valid when IsUsed() == true)
    TheKeyType& Key() noexcept { return *reinterpret_cast<TheKeyType*>(myKeyStorage); }

    const TheKeyType& Key() const noexcept
    {
      return *reinterpret_cast<const TheKeyType*>(myKeyStorage);
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
    using value_type        = TheKeyType;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const TheKeyType*;
    using reference         = const TheKeyType&;
    using slot_pointer = typename std::conditional<IsConstant, const Slot*, Slot*>::type;
    using container_type = typename std::conditional<IsConstant,
                                                     const NCollection_FlatMap,
                                                     NCollection_FlatMap>::type;

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

    reference operator*() const noexcept { return mySlots[myIndex].Key(); }
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

  using iterator = iterator_impl<true>;
  using const_iterator = iterator;

  //! Legacy OCCT cursor backed by the standard iterator state.
  class Iterator : public const_iterator
  {
  public:
    Iterator() noexcept = default;

    //! Constructor from map
    Iterator(const NCollection_FlatMap& theMap) noexcept
        : const_iterator(theMap)
    {
    }

    //! Check if there are more elements
    bool More() const noexcept { return const_iterator::More(); }

    //! Move to next element
    void Next() noexcept { ++(*this); }

    //! Get current key
    const TheKeyType& Key() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatMap::Iterator::Key");
      return const_iterator::Key();
    }

    //! Get current value (alias for Key for compatibility)
    const TheKeyType& Value() const { return Key(); }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return const_iterator::operator==(static_cast<const const_iterator&>(theOther));
    }
  };

public:
  // **************** Constructors and destructor ****************

  //! Default constructor
  NCollection_FlatMap() = default;

  //! Constructor with initial capacity hint
  explicit NCollection_FlatMap(const size_t theNbBuckets)
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial capacity hint
  explicit NCollection_FlatMap(const Hasher& theHasher, const size_t theNbBuckets = 0)
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
  explicit NCollection_FlatMap(Hasher&& theHasher, const size_t theNbBuckets = 0)
      : myHasher(std::move(theHasher))
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Copy constructor
  NCollection_FlatMap(const NCollection_FlatMap& theOther)
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
          slotData()[i].myHash               = theOther.slotData()[i].myHash;
          slotData()[i].myProbeDistancePlus1 =
            theOther.slotData()[i].myProbeDistancePlus1;
        }
      }
      myCapacity = theOther.myCapacity;
      mySize = theOther.mySize;
    }
  }

  //! Move constructor
  NCollection_FlatMap(NCollection_FlatMap&& theOther) noexcept(
    std::is_nothrow_move_constructible<Hasher>::value)
      : myHasher(std::move(theOther.myHasher))
  {
    exchangeStorage(theOther);
  }

  //! Destructor
  ~NCollection_FlatMap() { Clear(true); }

  //! Copy assignment
  NCollection_FlatMap& operator=(const NCollection_FlatMap& theOther)
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
            slotData()[i].myHash               = theOther.slotData()[i].myHash;
            slotData()[i].myProbeDistancePlus1 =
              theOther.slotData()[i].myProbeDistancePlus1;
          }
        }
        myCapacity = theOther.myCapacity;
        mySize = theOther.mySize;
      }
    }
    return *this;
  }

  //! Move assignment
  NCollection_FlatMap& operator=(NCollection_FlatMap&& theOther) noexcept(
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

  //! Returns true if map is empty
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Returns current capacity
  size_t Capacity() const noexcept { return myCapacity; }

  //! Check if key exists
  bool Contains(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return false;
    size_t anIndex = 0;
    return findSlotIndex(theKey, anIndex);
  }

  //! Contained returns optional const reference to the key in the map.
  //! Returns std::nullopt if the key is not found.
  std::optional<std::reference_wrapper<const TheKeyType>> Contained(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return std::nullopt;
    size_t aIdx = 0;
    if (!findSlotIndex(theKey, aIdx))
      return std::nullopt;
    return std::cref(slotData()[aIdx].Key());
  }

  //! Seek returns pointer to key in map. Returns NULL if not found.
  const TheKeyType* Seek(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return nullptr;
    size_t aIdx = 0;
    if (!findSlotIndex(theKey, aIdx))
      return nullptr;
    return &slotData()[aIdx].Key();
  }

  //! ChangeSeek returns modifiable pointer to key in map. Returns NULL if not found.
  TheKeyType* ChangeSeek(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return nullptr;
    size_t aIdx = 0;
    if (!findSlotIndex(theKey, aIdx))
      return nullptr;
    return &slotData()[aIdx].Key();
  }

public:
  // **************** Modification methods ****************

  //! Add key to set
  //! @return true if key was newly added, false if already present
  bool Add(const TheKeyType& theKey)
  {
    ensureCapacity();
    return insertImpl(theKey);
  }

  //! Add key to set (move semantics)
  bool Add(TheKeyType&& theKey)
  {
    ensureCapacity();
    return insertImpl(std::forward<TheKeyType>(theKey));
  }

  //! Added: add a new key if not yet in the map, and return
  //! reference to either newly added or previously existing key.
  //! @param theKey key to add
  //! @return const reference to the key in the map
  const TheKeyType& Added(const TheKeyType& theKey)
  {
    ensureCapacity();
    return insertRefImpl(theKey, std::false_type{});
  }

  //! Added: add a new key if not yet in the map, and return
  //! reference to either newly added or previously existing key.
  //! @param theKey key to add
  //! @return const reference to the key in the map
  const TheKeyType& Added(TheKeyType&& theKey)
  {
    ensureCapacity();
    return insertRefImpl(std::move(theKey), std::false_type{});
  }

  //! Emplace constructs key in-place; if key exists, overwrites.
  //! @param theArgs arguments forwarded to key constructor
  //! @return true if key was newly added, false if key already existed
  template <typename... Args>
  bool Emplace(Args&&... theArgs)
  {
    ensureCapacity();
    TheKeyType aTempKey(std::forward<Args>(theArgs)...);
    return emplaceImpl(std::move(aTempKey), std::false_type{}, std::false_type{});
  }

  //! Emplaced constructs key in-place; if key exists, overwrites.
  //! @param theArgs arguments forwarded to key constructor
  //! @return const reference to the key in the map
  template <typename... Args>
  const TheKeyType& Emplaced(Args&&... theArgs)
  {
    ensureCapacity();
    TheKeyType aTempKey(std::forward<Args>(theArgs)...);
    return emplaceImpl(std::move(aTempKey), std::false_type{}, std::true_type{});
  }

  //! TryEmplace constructs key in-place only if not already present.
  //! @param theArgs arguments forwarded to key constructor
  //! @return true if key was newly added, false if key already existed
  template <typename... Args>
  bool TryEmplace(Args&&... theArgs)
  {
    ensureCapacity();
    TheKeyType aTempKey(std::forward<Args>(theArgs)...);
    return emplaceImpl(std::move(aTempKey), std::true_type{}, std::false_type{});
  }

  //! TryEmplaced constructs key in-place only if not already present.
  //! @param theArgs arguments forwarded to key constructor
  //! @return const reference to the key (existing or newly added)
  template <typename... Args>
  const TheKeyType& TryEmplaced(Args&&... theArgs)
  {
    ensureCapacity();
    TheKeyType aTempKey(std::forward<Args>(theArgs)...);
    return emplaceImpl(std::move(aTempKey), std::true_type{}, std::true_type{});
  }

  //! Remove key from set
  //! @return true if key was found and removed
  bool Remove(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return false;

    size_t aFoundIndex = 0;
    if (!findSlotIndex(theKey, aFoundIndex))
    {
      return false;
    }

    const size_t aIndex = aFoundIndex;

    // Destroy key
    slotData()[aIndex].Key().~TheKeyType();
    slotData()[aIndex].SetEmpty();
    --mySize;

    // Backward shift delete
    backwardShiftDelete(aIndex);

    return true;
  }

  //! Clear all elements
  void Clear(bool doReleaseMemory = false)
  {
    if (slotData() != nullptr)
    {
      for (size_t i = 0; i < myCapacity; ++i)
      {
        if (slotData()[i].IsUsed())
        {
          slotData()[i].Key().~TheKeyType();
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
  void Exchange(NCollection_FlatMap& theOther) noexcept(
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

  iterator begin() noexcept { return iterator(*this); }

  const_iterator begin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(); }

  const_iterator end() const noexcept { return const_iterator(); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  const_iterator cend() const noexcept { return const_iterator(); }

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

    size_t aCapacity = (theElementCount * THE_MAX_LOAD_DENOMINATOR
                        + THE_MAX_LOAD_NUMERATOR - 1)
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

  bool needsGrowth() const
  {
    return myCapacity == 0 || capacityFor(mySize + 1) > myCapacity;
  }

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

  void exchangeStorage(NCollection_FlatMap& theOther) noexcept
  {
    std::swap(mySlots, theOther.mySlots);
    std::swap(myCapacity, theOther.myCapacity);
    std::swap(mySize, theOther.mySize);
  }

  void ensureCapacity()
  {
    // Grow at ~81.25% load factor.
    if (needsGrowth())
    {
      rehash(nextCapacity());
    }
  }

  void rehash(size_t theNewCapacity)
  {
    Slot* aOldSlots = slotData();
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
          insertRehashedImpl(std::move_if_noexcept(aOldSlots[i].Key()), aOldSlots[i].myHash);
          aOldSlots[i].Key().~TheKeyType();
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

  template <typename K, bool CheckExisting>
  bool insertRehashedImpl(K&&          theKey,
                          const size_t theHash,
                          std::bool_constant<CheckExisting>,
                          size_t* theInsertedIndex = nullptr)
  {
    const size_t aMask             = myCapacity - 1;
    size_t       aIndex            = theHash & aMask;
    size_t       aProbe            = 0;
    size_t       anInsertedIndex   = 0;
    bool         aHasInsertedIndex = false;

    TheKeyType aKeyToInsert  = std::forward<K>(theKey);
    size_t     aHashToInsert = theHash;

    while (true)
    {
      Slot& aSlot = slotData()[aIndex];
      if (aSlot.IsEmpty())
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
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

  template <typename K>
  void insertRehashedImpl(K&& theKey, const size_t theHash)
  {
    (void)insertRehashedImpl(std::forward<K>(theKey), theHash, std::false_type{});
  }

  template <typename K>
  bool insertImpl(K&& theKey)
  {
    const size_t aHash = myHasher(theKey);
    return insertRehashedImpl(std::forward<K>(theKey), aHash, std::true_type{});
  }

  //! Insert key and return reference to it (for Added method)
  //! @tparam IsTry if true, does not modify existing (not used for key-only map)
  template <typename K, bool IsTry>
  const TheKeyType& insertRefImpl(K&& theKey, std::bool_constant<IsTry>)
  {
    const size_t aHash  = myHasher(theKey);
    size_t       aIndex = 0;
    (void)insertRehashedImpl(std::forward<K>(theKey), aHash, std::true_type{}, &aIndex);
    return slotData()[aIndex].Key();
  }

  //! Implementation helper for Emplace/Emplaced.
  //! @tparam IsTry if true, does not modify existing; if false, overwrites
  //! @tparam ReturnRef if true, returns reference; if false, returns bool
  template <bool IsTry, bool ReturnRef>
  auto emplaceImpl(TheKeyType&& theKey, std::bool_constant<IsTry>, std::bool_constant<ReturnRef>)
    -> std::conditional_t<ReturnRef, const TheKeyType&, bool>
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    size_t       aProbe = 0;

    TheKeyType aKeyToInsert  = std::move(theKey);
    size_t     aHashToInsert = aHash;

    while (true)
    {
      Slot& aSlot = slotData()[aIndex];

      if (aSlot.IsEmpty())
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        aSlot.myHash = aHashToInsert;
        aSlot.SetProbeDistance(aProbe);
        ++mySize;
        if constexpr (ReturnRef)
          return aSlot.Key();
        else
          return true;
      }

      if (aSlot.myHash == aHashToInsert && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Key() = std::move(aKeyToInsert);
        if constexpr (ReturnRef)
          return aSlot.Key();
        else
          return false;
      }

      if (aProbe > aSlot.ProbeDistance())
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp = aProbe;
        aProbe            = aSlot.ProbeDistance();
        aSlot.SetProbeDistance(aTmp);
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
      // Construct key at aCurrent (which was destroyed or never had a key)
      new (&slotData()[aCurrent].Key()) TheKeyType(std::move(slotData()[aNext].Key()));
      slotData()[aCurrent].myHash = slotData()[aNext].myHash;
      slotData()[aCurrent].SetProbeDistance(slotData()[aNext].ProbeDistance() - 1);

      // Destroy the moved-from key at aNext
      slotData()[aNext].Key().~TheKeyType();

      aCurrent = aNext;
      aNext    = (aNext + 1) & aMask;
    }

    // Mark final slot as Empty (removes tombstone; either original deleted slot or last
    // shifted-from slot)
    slotData()[aCurrent].SetEmpty();
  }

private:
  Slot* slotData() noexcept
  {
    return static_cast<Slot*>(mySlots);
  }

  const Slot* slotData() const noexcept
  {
    return static_cast<const Slot*>(mySlots);
  }

  void*  mySlots    = nullptr;
  size_t myCapacity = 0;
  size_t mySize     = 0;
  Hasher myHasher;
};

#endif // NCollection_FlatMap_HeaderFile

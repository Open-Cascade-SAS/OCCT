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

#include <functional>
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
 * Best suited for:
 * - POD or small key types
 * - Performance-critical code paths
 * - Membership testing workloads
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

private:
  //! Default initial capacity (must be power of 2)
  static constexpr size_t THE_DEFAULT_CAPACITY = 8;

  //! Slot state enumeration for hash table entries.
  //! Uses Robin Hood hashing with backward shift deletion.
  enum class SlotState : uint8_t
  {
    Empty,   //!< Slot has never been used; search can stop here
    Deleted, //!< Slot was used but element was removed; search must continue past this
    Used     //!< Slot contains a valid element
  };

  //! Internal slot structure holding key and metadata.
  //! Key storage is uninitialized until state becomes Used.
  struct Slot
  {
    alignas(TheKeyType) char myKeyStorage[sizeof(TheKeyType)]; //!< Uninitialized key storage
    size_t    myHash;                                          //!< Cached hash code
    size_t    myProbeDistance; //!< Distance from ideal bucket (for Robin Hood)
    SlotState myState;         //!< Current state of this slot

    Slot() noexcept
        : myHash(0),
          myProbeDistance(0),
          myState(SlotState::Empty)
    {
      // Key is NOT constructed - myKeyStorage is uninitialized
    }

    //! Access the key (only valid when myState == Used)
    TheKeyType& Key() noexcept { return *reinterpret_cast<TheKeyType*>(myKeyStorage); }

    const TheKeyType& Key() const noexcept
    {
      return *reinterpret_cast<const TheKeyType*>(myKeyStorage);
    }
  };

public:
  // **************** Iterator interface ****************

  //! Forward iterator for NCollection_FlatMap
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator() noexcept
        : mySlots(nullptr),
          myCapacity(0),
          myIndex(0)
    {
    }

    //! Constructor from map
    Iterator(const NCollection_FlatMap& theMap) noexcept
        : mySlots(theMap.mySlots),
          myCapacity(theMap.myCapacity),
          myIndex(0)
    {
      // Find first used slot
      while (myIndex < myCapacity && mySlots[myIndex].myState != SlotState::Used)
      {
        ++myIndex;
      }
    }

    //! Check if there are more elements
    bool More() const noexcept { return myIndex < myCapacity; }

    //! Move to next element
    void Next() noexcept
    {
      ++myIndex;
      while (myIndex < myCapacity && mySlots[myIndex].myState != SlotState::Used)
      {
        ++myIndex;
      }
    }

    //! Get current key
    const TheKeyType& Key() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatMap::Iterator::Key");
      return mySlots[myIndex].Key();
    }

    //! Get current value (alias for Key for compatibility)
    const TheKeyType& Value() const { return Key(); }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return mySlots == theOther.mySlots && myIndex == theOther.myIndex;
    }

  private:
    const Slot* mySlots;
    size_t      myCapacity;
    size_t      myIndex;
  };

public:
  // **************** Constructors and destructor ****************

  //! Default constructor
  NCollection_FlatMap()
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0)
  {
  }

  //! Constructor with initial capacity hint
  explicit NCollection_FlatMap(const int theNbBuckets)
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0)
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial capacity hint
  explicit NCollection_FlatMap(const Hasher& theHasher, const int theNbBuckets = 0)
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0),
        myHasher(theHasher)
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Constructor with custom hasher (move).
  //! @param theHasher custom hasher instance (moved)
  //! @param theNbBuckets initial capacity hint
  explicit NCollection_FlatMap(Hasher&& theHasher, const int theNbBuckets = 0)
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0),
        myHasher(std::move(theHasher))
  {
    if (theNbBuckets > 0)
    {
      reserve(static_cast<size_t>(theNbBuckets));
    }
  }

  //! Copy constructor
  NCollection_FlatMap(const NCollection_FlatMap& theOther)
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0),
        myHasher(theOther.myHasher)
  {
    if (theOther.mySize > 0)
    {
      // Allocate same capacity as the source (not through reserve which may change capacity)
      mySlots = static_cast<Slot*>(Standard::Allocate(theOther.myCapacity * sizeof(Slot)));
      for (size_t i = 0; i < theOther.myCapacity; ++i)
      {
        new (&mySlots[i]) Slot();
      }
      myCapacity = theOther.myCapacity;

      for (size_t i = 0; i < theOther.myCapacity; ++i)
      {
        if (theOther.mySlots[i].myState == SlotState::Used)
        {
          new (&mySlots[i].Key()) TheKeyType(theOther.mySlots[i].Key());
          mySlots[i].myHash          = theOther.mySlots[i].myHash;
          mySlots[i].myProbeDistance = theOther.mySlots[i].myProbeDistance;
          mySlots[i].myState         = SlotState::Used;
        }
      }
      mySize = theOther.mySize;
    }
  }

  //! Move constructor
  NCollection_FlatMap(NCollection_FlatMap&& theOther) noexcept
      : mySlots(theOther.mySlots),
        myCapacity(theOther.myCapacity),
        mySize(theOther.mySize),
        myHasher(std::move(theOther.myHasher))
  {
    theOther.mySlots    = nullptr;
    theOther.myCapacity = 0;
    theOther.mySize     = 0;
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
        mySlots = static_cast<Slot*>(Standard::Allocate(theOther.myCapacity * sizeof(Slot)));
        for (size_t i = 0; i < theOther.myCapacity; ++i)
        {
          new (&mySlots[i]) Slot();
        }
        myCapacity = theOther.myCapacity;

        for (size_t i = 0; i < theOther.myCapacity; ++i)
        {
          if (theOther.mySlots[i].myState == SlotState::Used)
          {
            new (&mySlots[i].Key()) TheKeyType(theOther.mySlots[i].Key());
            mySlots[i].myHash          = theOther.mySlots[i].myHash;
            mySlots[i].myProbeDistance = theOther.mySlots[i].myProbeDistance;
            mySlots[i].myState         = SlotState::Used;
          }
        }
        mySize = theOther.mySize;
      }
    }
    return *this;
  }

  //! Move assignment
  NCollection_FlatMap& operator=(NCollection_FlatMap&& theOther) noexcept
  {
    if (this != &theOther)
    {
      Clear(true);
      mySlots             = theOther.mySlots;
      myCapacity          = theOther.myCapacity;
      mySize              = theOther.mySize;
      myHasher            = std::move(theOther.myHasher);
      theOther.mySlots    = nullptr;
      theOther.myCapacity = 0;
      theOther.mySize     = 0;
    }
    return *this;
  }

public:
  // **************** Query methods ****************

  //! Returns number of elements
  int Size() const noexcept { return static_cast<int>(mySize); }

  //! Returns number of elements
  int Extent() const noexcept { return static_cast<int>(mySize); }

  //! Returns true if map is empty
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Returns current capacity
  size_t Capacity() const noexcept { return myCapacity; }

  //! Check if key exists
  bool Contains(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return false;
    return findSlot(theKey).has_value();
  }

  //! Contained returns optional const reference to the key in the map.
  //! Returns std::nullopt if the key is not found.
  std::optional<std::reference_wrapper<const TheKeyType>> Contained(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return std::nullopt;
    const std::optional<size_t> aIdx = findSlot(theKey);
    if (!aIdx.has_value())
      return std::nullopt;
    return std::cref(mySlots[*aIdx].Key());
  }

  //! Seek returns pointer to key in map. Returns NULL if not found.
  const TheKeyType* Seek(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return nullptr;
    const std::optional<size_t> aIdx = findSlot(theKey);
    if (!aIdx.has_value())
      return nullptr;
    return &mySlots[*aIdx].Key();
  }

  //! ChangeSeek returns modifiable pointer to key in map. Returns NULL if not found.
  TheKeyType* ChangeSeek(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return nullptr;
    const std::optional<size_t> aIdx = findSlot(theKey);
    if (!aIdx.has_value())
      return nullptr;
    return &mySlots[*aIdx].Key();
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

    const std::optional<size_t> aFoundIndex = findSlot(theKey);
    if (!aFoundIndex.has_value())
    {
      return false;
    }

    const size_t aIndex = *aFoundIndex;

    // Destroy key
    mySlots[aIndex].Key().~TheKeyType();
    mySlots[aIndex].myState = SlotState::Deleted;
    --mySize;

    // Backward shift delete
    backwardShiftDelete(aIndex);

    return true;
  }

  //! Clear all elements
  void Clear(bool doReleaseMemory = false)
  {
    if (mySlots != nullptr)
    {
      for (size_t i = 0; i < myCapacity; ++i)
      {
        if (mySlots[i].myState == SlotState::Used)
        {
          mySlots[i].Key().~TheKeyType();
          mySlots[i].myState = SlotState::Empty;
        }
        else if (mySlots[i].myState == SlotState::Deleted)
        {
          mySlots[i].myState = SlotState::Empty;
        }
      }
      mySize = 0;

      if (doReleaseMemory)
      {
        Standard::Free(mySlots);
        mySlots    = nullptr;
        myCapacity = 0;
      }
    }
  }

  //! Exchange content with another map
  void Exchange(NCollection_FlatMap& theOther) noexcept
  {
    std::swap(mySlots, theOther.mySlots);
    std::swap(myCapacity, theOther.myCapacity);
    std::swap(mySize, theOther.mySize);
    std::swap(myHasher, theOther.myHasher);
  }

  //! Returns const reference to the hasher.
  const Hasher& GetHasher() const noexcept { return myHasher; }

  //! Reserve capacity for at least theN elements
  void reserve(size_t theN)
  {
    size_t aNewCapacity = nextPowerOf2(theN + theN / 8);
    if (aNewCapacity > myCapacity)
    {
      rehash(aNewCapacity);
    }
  }

public:
  // **************** Iterator access ****************

  Iterator begin() const noexcept { return Iterator(*this); }

  Iterator end() const noexcept { return Iterator(); }

  Iterator cbegin() const noexcept { return Iterator(*this); }

  Iterator cend() const noexcept { return Iterator(); }

private:
  // **************** Internal implementation ****************

  static size_t nextPowerOf2(size_t n) noexcept
  {
    if (n == 0)
      return THE_DEFAULT_CAPACITY;
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if constexpr (sizeof(size_t) > 4)
    {
      n |= n >> 32;
    }
    return n + 1;
  }

  void ensureCapacity()
  {
    // Grow at ~87.5% load factor
    if (myCapacity == 0 || (mySize + 1) * 8 > myCapacity * 7)
    {
      size_t aNewCapacity = myCapacity == 0 ? THE_DEFAULT_CAPACITY : myCapacity * 2;
      rehash(aNewCapacity);
    }
  }

  void rehash(size_t theNewCapacity)
  {
    Slot*  aOldSlots    = mySlots;
    size_t aOldCapacity = myCapacity;

    mySlots = static_cast<Slot*>(Standard::Allocate(theNewCapacity * sizeof(Slot)));
    for (size_t i = 0; i < theNewCapacity; ++i)
    {
      new (&mySlots[i]) Slot();
    }
    myCapacity = theNewCapacity;
    mySize     = 0;

    if (aOldSlots != nullptr)
    {
      for (size_t i = 0; i < aOldCapacity; ++i)
      {
        if (aOldSlots[i].myState == SlotState::Used)
        {
          insertImpl(std::move(aOldSlots[i].Key()));
          aOldSlots[i].Key().~TheKeyType();
        }
      }
      Standard::Free(aOldSlots);
    }
  }

  //! Find slot containing key.
  //! @param theKey key to find
  //! @return index of found slot, or std::nullopt if not found
  std::optional<size_t> findSlot(const TheKeyType& theKey) const
  {
    const size_t aHash     = myHasher(theKey);
    const size_t aMask     = myCapacity - 1;
    size_t       aIndex    = aHash & aMask;
    size_t       aProbe    = 0;
    const size_t aMaxProbe = myCapacity;

    while (aProbe < aMaxProbe)
    {
      const Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SlotState::Empty)
      {
        return std::nullopt;
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHash
          && myHasher(aSlot.Key(), theKey))
      {
        return aIndex;
      }

      // Robin Hood optimization: if current probe > slot's probe, key can't exist further
      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        return std::nullopt;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
    return std::nullopt;
  }

  template <typename K>
  bool insertImpl(K&& theKey)
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    size_t       aProbe = 0;

    TheKeyType aKeyToInsert  = std::forward<K>(theKey);
    size_t     aHashToInsert = aHash;

    while (true)
    {
      Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return true;
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        return false; // Already exists
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp     = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
  }

  //! Insert key and return reference to it (for Added method)
  //! @tparam IsTry if true, does not modify existing (not used for key-only map)
  template <typename K, bool IsTry>
  const TheKeyType& insertRefImpl(K&& theKey, std::bool_constant<IsTry>)
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    size_t       aProbe = 0;

    TheKeyType aKeyToInsert  = std::forward<K>(theKey);
    size_t     aHashToInsert = aHash;
    size_t     aFoundIndex   = SIZE_MAX;

    while (true)
    {
      Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return aSlot.Key();
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        return aSlot.Key(); // Already exists
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        // Track where the original key ends up after swaps
        if (aFoundIndex == SIZE_MAX)
        {
          aFoundIndex = aIndex;
        }
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp     = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
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
      Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        if constexpr (ReturnRef)
          return aSlot.Key();
        else
          return true;
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Key() = std::move(aKeyToInsert);
        if constexpr (ReturnRef)
          return aSlot.Key();
        else
          return false;
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aHashToInsert, aSlot.myHash);
        const size_t aTmp     = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
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

    while (mySlots[aNext].myState == SlotState::Used && mySlots[aNext].myProbeDistance > 0)
    {
      // Construct key at aCurrent (which was destroyed or never had a key)
      new (&mySlots[aCurrent].Key()) TheKeyType(std::move(mySlots[aNext].Key()));
      mySlots[aCurrent].myHash          = mySlots[aNext].myHash;
      mySlots[aCurrent].myProbeDistance = mySlots[aNext].myProbeDistance - 1;
      mySlots[aCurrent].myState         = SlotState::Used;

      // Destroy the moved-from key at aNext
      mySlots[aNext].Key().~TheKeyType();

      aCurrent = aNext;
      aNext    = (aNext + 1) & aMask;
    }

    // Mark final slot as Empty (removes tombstone; either original deleted slot or last
    // shifted-from slot)
    mySlots[aCurrent].myState = SlotState::Empty;
  }

private:
  Slot*  mySlots;
  size_t myCapacity;
  size_t mySize;
  Hasher myHasher;
};

#endif // NCollection_FlatMap_HeaderFile

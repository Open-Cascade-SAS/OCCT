// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <new>
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
 * - Maximum probe distance is 250 (sufficient for normal hash distributions)
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
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;

private:
  //! Slot state markers
  static constexpr uint8_t SLOT_EMPTY   = 0;
  static constexpr uint8_t SLOT_DELETED = 1;
  static constexpr uint8_t SLOT_USED    = 2;

  //! Internal slot structure holding key and metadata
  struct Slot
  {
    TheKeyType myKey;
    size_t     myHash;          //!< Cached hash code
    uint8_t    myProbeDistance; //!< Distance from ideal bucket (for Robin Hood)
    uint8_t    myState;         //!< SLOT_EMPTY, SLOT_DELETED, or SLOT_USED

    Slot() noexcept(std::is_nothrow_default_constructible<TheKeyType>::value)
        : myKey(),
          myHash(0),
          myProbeDistance(0),
          myState(SLOT_EMPTY)
    {
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
      while (myIndex < myCapacity && mySlots[myIndex].myState != SLOT_USED)
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
      while (myIndex < myCapacity && mySlots[myIndex].myState != SLOT_USED)
      {
        ++myIndex;
      }
    }

    //! Get current key
    const TheKeyType& Key() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatMap::Iterator::Key");
      return mySlots[myIndex].myKey;
    }

    //! Get current value (alias for Key for compatibility)
    const TheKeyType& Value() const { return Key(); }

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

  //! Copy constructor
  NCollection_FlatMap(const NCollection_FlatMap& theOther)
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0),
        myHasher(theOther.myHasher)
  {
    if (theOther.mySize > 0)
    {
      reserve(theOther.myCapacity);
      for (size_t i = 0; i < theOther.myCapacity; ++i)
      {
        if (theOther.mySlots[i].myState == SLOT_USED)
        {
          new (&mySlots[i].myKey) TheKeyType(theOther.mySlots[i].myKey);
          mySlots[i].myHash          = theOther.mySlots[i].myHash;
          mySlots[i].myProbeDistance = theOther.mySlots[i].myProbeDistance;
          mySlots[i].myState         = SLOT_USED;
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
      if (theOther.mySize > 0)
      {
        reserve(theOther.myCapacity);
        for (size_t i = 0; i < theOther.myCapacity; ++i)
        {
          if (theOther.mySlots[i].myState == SLOT_USED)
          {
            new (&mySlots[i].myKey) TheKeyType(theOther.mySlots[i].myKey);
            mySlots[i].myHash          = theOther.mySlots[i].myHash;
            mySlots[i].myProbeDistance = theOther.mySlots[i].myProbeDistance;
            mySlots[i].myState         = SLOT_USED;
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
    size_t aIndex;
    return findSlot(theKey, aIndex);
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

  //! Remove key from set
  //! @return true if key was found and removed
  bool Remove(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return false;

    size_t aIndex;
    if (!findSlot(theKey, aIndex))
    {
      return false;
    }

    // Destroy key
    mySlots[aIndex].myKey.~TheKeyType();
    mySlots[aIndex].myState = SLOT_DELETED;
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
        if (mySlots[i].myState == SLOT_USED)
        {
          mySlots[i].myKey.~TheKeyType();
          mySlots[i].myState = SLOT_EMPTY;
        }
        else if (mySlots[i].myState == SLOT_DELETED)
        {
          mySlots[i].myState = SLOT_EMPTY;
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
      return 8;
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
    if (myCapacity == 0 || (mySize + 1) * 8 > myCapacity * 7)
    {
      size_t aNewCapacity = myCapacity == 0 ? 8 : myCapacity * 2;
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
        if (aOldSlots[i].myState == SLOT_USED)
        {
          insertImpl(std::move(aOldSlots[i].myKey));
          aOldSlots[i].myKey.~TheKeyType();
        }
      }
      Standard::Free(aOldSlots);
    }
  }

  bool findSlot(const TheKeyType& theKey, size_t& theIndex) const
  {
    const size_t aHash     = myHasher(theKey);
    const size_t aMask     = myCapacity - 1;
    size_t       aIndex    = aHash & aMask;
    uint8_t      aProbe    = 0;
    const size_t aMaxProbe = myCapacity;

    while (aProbe < aMaxProbe)
    {
      const Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SLOT_EMPTY)
      {
        return false;
      }

      if (aSlot.myState == SLOT_USED && aSlot.myHash == aHash && myHasher(aSlot.myKey, theKey))
      {
        theIndex = aIndex;
        return true;
      }

      if (aSlot.myState == SLOT_USED && aProbe > aSlot.myProbeDistance)
      {
        return false;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
    return false;
  }

  template <typename K>
  bool insertImpl(K&& theKey)
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    uint8_t      aProbe = 0;

    TheKeyType aKeyToInsert  = std::forward<K>(theKey);
    size_t     aHashToInsert = aHash;

    while (true)
    {
      Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SLOT_EMPTY || aSlot.myState == SLOT_DELETED)
      {
        new (&aSlot.myKey) TheKeyType(std::move(aKeyToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SLOT_USED;
        ++mySize;
        return true;
      }

      if (aSlot.myState == SLOT_USED && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.myKey, aKeyToInsert))
      {
        return false; // Already exists
      }

      if (aSlot.myState == SLOT_USED && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.myKey);
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      if (aProbe > 250)
      {
        throw Standard_OutOfRange("NCollection_FlatMap: excessive probe length");
      }
    }
  }

  void backwardShiftDelete(size_t theIndex)
  {
    const size_t aMask    = myCapacity - 1;
    size_t       aCurrent = theIndex;
    size_t       aNext    = (aCurrent + 1) & aMask;

    while (mySlots[aNext].myState == SLOT_USED && mySlots[aNext].myProbeDistance > 0)
    {
      mySlots[aCurrent].myKey           = std::move(mySlots[aNext].myKey);
      mySlots[aCurrent].myHash          = mySlots[aNext].myHash;
      mySlots[aCurrent].myProbeDistance = mySlots[aNext].myProbeDistance - 1;
      mySlots[aCurrent].myState         = SLOT_USED;

      mySlots[aNext].myState = SLOT_DELETED;

      aCurrent = aNext;
      aNext    = (aNext + 1) & aMask;
    }

    mySlots[aCurrent].myState = SLOT_EMPTY;
  }

private:
  Slot*  mySlots;
  size_t myCapacity;
  size_t mySize;
  Hasher myHasher;
};

#endif // NCollection_FlatMap_HeaderFile

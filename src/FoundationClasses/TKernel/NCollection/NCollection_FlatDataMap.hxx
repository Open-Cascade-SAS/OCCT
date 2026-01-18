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

#ifndef NCollection_FlatDataMap_HeaderFile
#define NCollection_FlatDataMap_HeaderFile

#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <NCollection_DefaultHasher.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <cstring>
#include <utility>
#include <new>

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
 * Best suited for:
 * - POD or small key/value types
 * - Performance-critical code paths
 * - Lookup-heavy workloads
 *
 * Limitations:
 * - Keys and values must be movable
 * - Higher memory usage at low load factors
 * - Iteration order is not insertion order
 *
 * @tparam TheKeyType   Type of keys
 * @tparam TheItemType  Type of values
 * @tparam Hasher       Hash and equality functor (default: NCollection_DefaultHasher)
 */
template <class TheKeyType, class TheItemType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_FlatDataMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;

  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

private:
  //! Slot state markers
  static constexpr uint8_t SLOT_EMPTY   = 0;
  static constexpr uint8_t SLOT_DELETED = 1;
  static constexpr uint8_t SLOT_USED    = 2;

  //! Internal slot structure holding key, value, and metadata
  struct Slot
  {
    TheKeyType  myKey;
    TheItemType myItem;
    size_t      myHash;          //!< Cached hash code
    uint8_t     myProbeDistance; //!< Distance from ideal bucket (for Robin Hood)
    uint8_t     myState;         //!< SLOT_EMPTY, SLOT_DELETED, or SLOT_USED

    Slot() noexcept
        : myHash(0),
          myProbeDistance(0),
          myState(SLOT_EMPTY)
    {
    }
  };

public:
  // **************** Iterator interface ****************

  //! Forward iterator for NCollection_FlatDataMap
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
    Iterator(const NCollection_FlatDataMap& theMap) noexcept
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
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::Key");
      return mySlots[myIndex].myKey;
    }

    //! Get current value (const)
    const TheItemType& Value() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::Value");
      return mySlots[myIndex].myItem;
    }

    //! Get current value (mutable)
    TheItemType& ChangeValue() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::ChangeValue");
      return const_cast<Slot*>(mySlots)[myIndex].myItem;
    }

  private:
    const Slot* mySlots;
    size_t      myCapacity;
    size_t      myIndex;
  };

public:
  // **************** Constructors and destructor ****************

  //! Default constructor
  NCollection_FlatDataMap()
      : mySlots(nullptr),
        myCapacity(0),
        mySize(0)
  {
  }

  //! Constructor with initial capacity hint
  //! @param theNbBuckets initial capacity (will be rounded up to power of 2)
  explicit NCollection_FlatDataMap(const int theNbBuckets)
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
  NCollection_FlatDataMap(const NCollection_FlatDataMap& theOther)
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
          // Copy key and value using copy constructor
          new (&mySlots[i].myKey) TheKeyType(theOther.mySlots[i].myKey);
          new (&mySlots[i].myItem) TheItemType(theOther.mySlots[i].myItem);
          mySlots[i].myHash          = theOther.mySlots[i].myHash;
          mySlots[i].myProbeDistance = theOther.mySlots[i].myProbeDistance;
          mySlots[i].myState         = SLOT_USED;
        }
      }
      mySize = theOther.mySize;
    }
  }

  //! Move constructor
  NCollection_FlatDataMap(NCollection_FlatDataMap&& theOther) noexcept
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
  ~NCollection_FlatDataMap() { Clear(true); }

  //! Copy assignment
  NCollection_FlatDataMap& operator=(const NCollection_FlatDataMap& theOther)
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
            new (&mySlots[i].myItem) TheItemType(theOther.mySlots[i].myItem);
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
  NCollection_FlatDataMap& operator=(NCollection_FlatDataMap&& theOther) noexcept
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
  bool IsBound(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return false;
    size_t aIndex;
    return findSlot(theKey, aIndex);
  }

  //! Find value by key, returns nullptr if not found
  const TheItemType* Seek(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return nullptr;
    size_t aIndex;
    if (findSlot(theKey, aIndex))
    {
      return &mySlots[aIndex].myItem;
    }
    return nullptr;
  }

  //! Find value by key (mutable), returns nullptr if not found
  TheItemType* ChangeSeek(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return nullptr;
    size_t aIndex;
    if (findSlot(theKey, aIndex))
    {
      return &mySlots[aIndex].myItem;
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

  //! Remove key from map
  //! @return true if key was found and removed
  bool UnBind(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return false;

    size_t aIndex;
    if (!findSlot(theKey, aIndex))
    {
      return false;
    }

    // Destroy key and value
    mySlots[aIndex].myKey.~TheKeyType();
    mySlots[aIndex].myItem.~TheItemType();
    mySlots[aIndex].myState = SLOT_DELETED;
    --mySize;

    // Backward shift delete to maintain Robin Hood invariant
    backwardShiftDelete(aIndex);

    return true;
  }

  //! Clear all elements
  //! @param doReleaseMemory if true, free the internal buffer
  void Clear(bool doReleaseMemory = false)
  {
    if (mySlots != nullptr)
    {
      // Destroy all used slots
      for (size_t i = 0; i < myCapacity; ++i)
      {
        if (mySlots[i].myState == SLOT_USED)
        {
          mySlots[i].myKey.~TheKeyType();
          mySlots[i].myItem.~TheItemType();
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
  void Exchange(NCollection_FlatDataMap& theOther) noexcept
  {
    std::swap(mySlots, theOther.mySlots);
    std::swap(myCapacity, theOther.myCapacity);
    std::swap(mySize, theOther.mySize);
    std::swap(myHasher, theOther.myHasher);
  }

  //! Reserve capacity for at least theN elements
  void reserve(size_t theN)
  {
    size_t aNewCapacity = nextPowerOf2(theN + theN / 8); // ~87.5% load factor target
    if (aNewCapacity > myCapacity)
    {
      rehash(aNewCapacity);
    }
  }

public:
  // **************** Iterator access ****************

  //! Returns iterator to first element
  Iterator begin() const noexcept { return Iterator(*this); }

  //! Returns iterator past the end
  Iterator end() const noexcept { return Iterator(); }

  //! Returns iterator to first element
  Iterator cbegin() const noexcept { return Iterator(*this); }

  //! Returns iterator past the end
  Iterator cend() const noexcept { return Iterator(); }

private:
  // **************** Internal implementation ****************

  //! Get next power of 2 >= n
  static size_t nextPowerOf2(size_t n) noexcept
  {
    if (n == 0)
      return 8; // Minimum capacity
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

  //! Ensure there's room for at least one more element
  void ensureCapacity()
  {
    // Grow at ~87.5% load factor
    if (myCapacity == 0 || (mySize + 1) * 8 > myCapacity * 7)
    {
      size_t aNewCapacity = myCapacity == 0 ? 8 : myCapacity * 2;
      rehash(aNewCapacity);
    }
  }

  //! Rehash to new capacity
  void rehash(size_t theNewCapacity)
  {
    Slot*  aOldSlots    = mySlots;
    size_t aOldCapacity = myCapacity;

    // Allocate new buffer
    mySlots = static_cast<Slot*>(Standard::Allocate(theNewCapacity * sizeof(Slot)));
    for (size_t i = 0; i < theNewCapacity; ++i)
    {
      new (&mySlots[i]) Slot();
    }
    myCapacity = theNewCapacity;
    mySize     = 0;

    // Reinsert all elements from old buffer
    if (aOldSlots != nullptr)
    {
      for (size_t i = 0; i < aOldCapacity; ++i)
      {
        if (aOldSlots[i].myState == SLOT_USED)
        {
          insertImpl(std::move(aOldSlots[i].myKey), std::move(aOldSlots[i].myItem));
          aOldSlots[i].myKey.~TheKeyType();
          aOldSlots[i].myItem.~TheItemType();
        }
      }
      Standard::Free(aOldSlots);
    }
  }

  //! Find slot containing key
  //! @param theKey key to find
  //! @param theIndex [out] index of found slot
  //! @return true if found
  bool findSlot(const TheKeyType& theKey, size_t& theIndex) const
  {
    const size_t aHash   = myHasher(theKey);
    const size_t aMask   = myCapacity - 1;
    size_t       aIndex  = aHash & aMask;
    uint8_t      aProbe  = 0;
    const size_t aMaxProbe = myCapacity;

    while (aProbe < aMaxProbe)
    {
      const Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SLOT_EMPTY)
      {
        return false; // Key not found
      }

      if (aSlot.myState == SLOT_USED && aSlot.myHash == aHash && myHasher(aSlot.myKey, theKey))
      {
        theIndex = aIndex;
        return true;
      }

      // Robin Hood optimization: if current probe > slot's probe, key can't exist further
      if (aSlot.myState == SLOT_USED && aProbe > aSlot.myProbeDistance)
      {
        return false;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
    return false;
  }

  //! Insert key-value pair
  template <typename K, typename V>
  bool insertImpl(K&& theKey, V&& theItem)
  {
    const size_t aHash  = myHasher(theKey);
    const size_t aMask  = myCapacity - 1;
    size_t       aIndex = aHash & aMask;
    uint8_t      aProbe = 0;

    TheKeyType  aKeyToInsert  = std::forward<K>(theKey);
    TheItemType aItemToInsert = std::forward<V>(theItem);
    size_t      aHashToInsert = aHash;

    while (true)
    {
      Slot& aSlot = mySlots[aIndex];

      if (aSlot.myState == SLOT_EMPTY || aSlot.myState == SLOT_DELETED)
      {
        // Found empty slot - insert here
        new (&aSlot.myKey) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.myItem) TheItemType(std::move(aItemToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SLOT_USED;
        ++mySize;
        return true;
      }

      if (aSlot.myState == SLOT_USED && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.myKey, aKeyToInsert))
      {
        // Key exists - update value
        aSlot.myItem = std::move(aItemToInsert);
        return false;
      }

      // Robin Hood: if our probe distance > current slot's, swap and continue
      if (aSlot.myState == SLOT_USED && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.myKey);
        std::swap(aItemToInsert, aSlot.myItem);
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      // Safety check - should never happen with proper load factor
      if (aProbe > 250)
      {
        throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
      }
    }
  }

  //! Backward shift delete to maintain Robin Hood invariant
  void backwardShiftDelete(size_t theIndex)
  {
    const size_t aMask    = myCapacity - 1;
    size_t       aCurrent = theIndex;
    size_t       aNext    = (aCurrent + 1) & aMask;

    while (mySlots[aNext].myState == SLOT_USED && mySlots[aNext].myProbeDistance > 0)
    {
      // Move next slot backward
      mySlots[aCurrent].myKey           = std::move(mySlots[aNext].myKey);
      mySlots[aCurrent].myItem          = std::move(mySlots[aNext].myItem);
      mySlots[aCurrent].myHash          = mySlots[aNext].myHash;
      mySlots[aCurrent].myProbeDistance = mySlots[aNext].myProbeDistance - 1;
      mySlots[aCurrent].myState         = SLOT_USED;

      mySlots[aNext].myState = SLOT_DELETED;

      aCurrent = aNext;
      aNext    = (aNext + 1) & aMask;
    }

    // Final slot becomes empty (not deleted)
    mySlots[aCurrent].myState = SLOT_EMPTY;
  }

private:
  Slot*  mySlots;    //!< Array of slots
  size_t myCapacity; //!< Total number of slots (always power of 2)
  size_t mySize;     //!< Number of used slots
  Hasher myHasher;   //!< Hash and equality functor
};

#endif // NCollection_FlatDataMap_HeaderFile

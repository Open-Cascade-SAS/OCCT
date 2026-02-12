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

#include <functional>
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
 * Best suited for:
 * - POD or small key/value types
 * - Performance-critical code paths
 * - Lookup-heavy workloads
 *
 * Limitations:
 * - Keys and values must be movable
 * - Higher memory usage at low load factors
 * - Iteration order is not insertion order
 * - Maximum probe distance is 250 (sufficient for normal hash distributions)
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
  using value_type = TheItemType;

private:
  //! Default initial capacity (must be power of 2)
  static constexpr size_t THE_DEFAULT_CAPACITY = 8;

  //! Maximum allowed probe distance before throwing an exception.
  //! This limit is sufficient for normal hash distributions with proper load factors.
  static constexpr uint8_t THE_MAX_PROBE_DISTANCE = 250;

  //! Slot state enumeration for hash table entries.
  //! Uses Robin Hood hashing with backward shift deletion.
  enum class SlotState : uint8_t
  {
    Empty,   //!< Slot has never been used; search can stop here
    Deleted, //!< Slot was used but element was removed; search must continue past this
    Used     //!< Slot contains a valid element
  };

  //! Internal slot structure holding key, value, and metadata.
  //! Key and item storage is uninitialized until state becomes Used.
#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4324) // structure was padded due to alignment specifier
#endif
  struct Slot
  {
    alignas(TheKeyType) char myKeyStorage[sizeof(TheKeyType)];
    alignas(TheItemType) char myItemStorage[sizeof(TheItemType)];
    size_t    myHash;          //!< Cached hash code
    uint8_t   myProbeDistance; //!< Distance from ideal bucket (for Robin Hood)
    SlotState myState;         //!< Current state of this slot

    Slot() noexcept
        : myHash(0),
          myProbeDistance(0),
          myState(SlotState::Empty)
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
  };
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

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
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::Key");
      return mySlots[myIndex].Key();
    }

    //! Get current value (const)
    const TheItemType& Value() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::Value");
      return mySlots[myIndex].Item();
    }

    //! Get current value (mutable)
    TheItemType& ChangeValue() const
    {
      Standard_OutOfRange_Raise_if(!More(), "NCollection_FlatDataMap::Iterator::ChangeValue");
      return const_cast<Slot*>(mySlots)[myIndex].Item();
    }

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

  //! Constructor with custom hasher (copy).
  //! @param theHasher custom hasher instance
  //! @param theNbBuckets initial capacity hint
  explicit NCollection_FlatDataMap(const Hasher& theHasher, const int theNbBuckets = 0)
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
  explicit NCollection_FlatDataMap(Hasher&& theHasher, const int theNbBuckets = 0)
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
  NCollection_FlatDataMap(const NCollection_FlatDataMap& theOther)
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
          new (&mySlots[i].Item()) TheItemType(theOther.mySlots[i].Item());
          mySlots[i].myHash          = theOther.mySlots[i].myHash;
          mySlots[i].myProbeDistance = theOther.mySlots[i].myProbeDistance;
          mySlots[i].myState         = SlotState::Used;
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
            new (&mySlots[i].Item()) TheItemType(theOther.mySlots[i].Item());
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
    return findSlot(theKey).has_value();
  }

  //! Contained returns optional pair of const references to key and value.
  //! Returns std::nullopt if the key is not found.
  std::optional<
    std::pair<std::reference_wrapper<const TheKeyType>, std::reference_wrapper<const TheItemType>>>
    Contained(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return std::nullopt;
    const std::optional<size_t> aIdx = findSlot(theKey);
    if (!aIdx.has_value())
      return std::nullopt;
    return std::make_pair(std::cref(mySlots[*aIdx].Key()), std::cref(mySlots[*aIdx].Item()));
  }

  //! Contained returns optional pair of const key reference and mutable value reference.
  //! Returns std::nullopt if the key is not found.
  std::optional<
    std::pair<std::reference_wrapper<const TheKeyType>, std::reference_wrapper<TheItemType>>>
    Contained(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return std::nullopt;
    const std::optional<size_t> aIdx = findSlot(theKey);
    if (!aIdx.has_value())
      return std::nullopt;
    return std::make_pair(std::cref(mySlots[*aIdx].Key()), std::ref(mySlots[*aIdx].Item()));
  }

  //! Find value by key, returns nullptr if not found
  const TheItemType* Seek(const TheKeyType& theKey) const
  {
    if (mySize == 0)
      return nullptr;
    const std::optional<size_t> aFoundIndex = findSlot(theKey);
    if (aFoundIndex.has_value())
    {
      return &mySlots[*aFoundIndex].Item();
    }
    return nullptr;
  }

  //! Find value by key (mutable), returns nullptr if not found
  TheItemType* ChangeSeek(const TheKeyType& theKey)
  {
    if (mySize == 0)
      return nullptr;
    const std::optional<size_t> aFoundIndex = findSlot(theKey);
    if (aFoundIndex.has_value())
    {
      return &mySlots[*aFoundIndex].Item();
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

    const std::optional<size_t> aFoundIndex = findSlot(theKey);
    if (!aFoundIndex.has_value())
    {
      return false;
    }

    const size_t aIndex = *aFoundIndex;

    mySlots[aIndex].Key().~TheKeyType();
    mySlots[aIndex].Item().~TheItemType();
    mySlots[aIndex].myState = SlotState::Deleted;
    --mySize;

    backwardShiftDelete(aIndex);

    return true;
  }

  //! Clear all elements
  //! @param doReleaseMemory if true, free the internal buffer
  void Clear(bool doReleaseMemory = false)
  {
    if (mySlots != nullptr)
    {
      for (size_t i = 0; i < myCapacity; ++i)
      {
        if (mySlots[i].myState == SlotState::Used)
        {
          mySlots[i].Key().~TheKeyType();
          mySlots[i].Item().~TheItemType();
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
  void Exchange(NCollection_FlatDataMap& theOther) noexcept
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

  //! Get next power of 2 >= n
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

  //! Ensure there's room for at least one more element
  void ensureCapacity()
  {
    // Grow at ~87.5% load factor
    if (myCapacity == 0 || (mySize + 1) * 8 > myCapacity * 7)
    {
      size_t aNewCapacity = myCapacity == 0 ? THE_DEFAULT_CAPACITY : myCapacity * 2;
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

    if (aOldSlots != nullptr)
    {
      for (size_t i = 0; i < aOldCapacity; ++i)
      {
        if (aOldSlots[i].myState == SlotState::Used)
        {
          insertImpl(std::move(aOldSlots[i].Key()), std::move(aOldSlots[i].Item()));
          aOldSlots[i].Key().~TheKeyType();
          aOldSlots[i].Item().~TheItemType();
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
    uint8_t      aProbe    = 0;
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

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        return std::nullopt;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;
    }
    return std::nullopt;
  }

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

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::move(aItemToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return true;
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        aSlot.Item() = std::move(aItemToInsert);
        return false;
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      if (aProbe > THE_MAX_PROBE_DISTANCE)
      {
        throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
      }
    }
  }

  template <typename K, typename V>
  bool tryInsertImpl(K&& theKey, V&& theItem)
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

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::move(aItemToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return true;
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        return false;
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      if (aProbe > THE_MAX_PROBE_DISTANCE)
      {
        throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
      }
    }
  }

  template <typename K, typename V, bool IsTry>
  TheItemType& insertRefImpl(K&& theKey, V&& theItem, std::bool_constant<IsTry>)
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

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::move(aItemToInsert));
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return aSlot.Item();
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Item() = std::move(aItemToInsert);
        return aSlot.Item();
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      if (aProbe > THE_MAX_PROBE_DISTANCE)
      {
        throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
      }
    }
  }

  template <typename K, bool IsTry, typename... Args>
  bool emplaceImpl(K&& theKey, std::bool_constant<IsTry>, Args&&... theArgs)
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

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::forward<Args>(theArgs)...);
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return true;
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Item() = TheItemType(std::forward<Args>(theArgs)...);
        return false;
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        TheItemType aItemToInsert(std::forward<Args>(theArgs)...);

        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;

        ++aProbe;
        aIndex = (aIndex + 1) & aMask;

        while (true)
        {
          Slot& aSlot2 = mySlots[aIndex];

          if (aSlot2.myState == SlotState::Empty || aSlot2.myState == SlotState::Deleted)
          {
            new (&aSlot2.Key()) TheKeyType(std::move(aKeyToInsert));
            new (&aSlot2.Item()) TheItemType(std::move(aItemToInsert));
            aSlot2.myHash          = aHashToInsert;
            aSlot2.myProbeDistance = aProbe;
            aSlot2.myState         = SlotState::Used;
            ++mySize;
            return true;
          }

          if (aSlot2.myState == SlotState::Used && aProbe > aSlot2.myProbeDistance)
          {
            std::swap(aKeyToInsert, aSlot2.Key());
            std::swap(aItemToInsert, aSlot2.Item());
            std::swap(aHashToInsert, aSlot2.myHash);
            uint8_t aTmp2          = aProbe;
            aProbe                 = aSlot2.myProbeDistance;
            aSlot2.myProbeDistance = aTmp2;
          }

          ++aProbe;
          aIndex = (aIndex + 1) & aMask;

          if (aProbe > THE_MAX_PROBE_DISTANCE)
          {
            throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
          }
        }
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      if (aProbe > THE_MAX_PROBE_DISTANCE)
      {
        throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
      }
    }
  }

  template <typename K, bool IsTry, typename... Args>
  TheItemType& emplacedImpl(K&& theKey, std::bool_constant<IsTry>, Args&&... theArgs)
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

      if (aSlot.myState == SlotState::Empty || aSlot.myState == SlotState::Deleted)
      {
        new (&aSlot.Key()) TheKeyType(std::move(aKeyToInsert));
        new (&aSlot.Item()) TheItemType(std::forward<Args>(theArgs)...);
        aSlot.myHash          = aHashToInsert;
        aSlot.myProbeDistance = aProbe;
        aSlot.myState         = SlotState::Used;
        ++mySize;
        return aSlot.Item();
      }

      if (aSlot.myState == SlotState::Used && aSlot.myHash == aHashToInsert
          && myHasher(aSlot.Key(), aKeyToInsert))
      {
        if constexpr (!IsTry)
          aSlot.Item() = TheItemType(std::forward<Args>(theArgs)...);
        return aSlot.Item();
      }

      if (aSlot.myState == SlotState::Used && aProbe > aSlot.myProbeDistance)
      {
        TheItemType aItemToInsert(std::forward<Args>(theArgs)...);

        std::swap(aKeyToInsert, aSlot.Key());
        std::swap(aItemToInsert, aSlot.Item());
        std::swap(aHashToInsert, aSlot.myHash);
        uint8_t aTmp          = aProbe;
        aProbe                = aSlot.myProbeDistance;
        aSlot.myProbeDistance = aTmp;

        TheItemType& aResult = aSlot.Item();

        ++aProbe;
        aIndex = (aIndex + 1) & aMask;

        while (true)
        {
          Slot& aSlot2 = mySlots[aIndex];

          if (aSlot2.myState == SlotState::Empty || aSlot2.myState == SlotState::Deleted)
          {
            new (&aSlot2.Key()) TheKeyType(std::move(aKeyToInsert));
            new (&aSlot2.Item()) TheItemType(std::move(aItemToInsert));
            aSlot2.myHash          = aHashToInsert;
            aSlot2.myProbeDistance = aProbe;
            aSlot2.myState         = SlotState::Used;
            ++mySize;
            return aResult;
          }

          if (aSlot2.myState == SlotState::Used && aProbe > aSlot2.myProbeDistance)
          {
            std::swap(aKeyToInsert, aSlot2.Key());
            std::swap(aItemToInsert, aSlot2.Item());
            std::swap(aHashToInsert, aSlot2.myHash);
            uint8_t aTmp2          = aProbe;
            aProbe                 = aSlot2.myProbeDistance;
            aSlot2.myProbeDistance = aTmp2;
          }

          ++aProbe;
          aIndex = (aIndex + 1) & aMask;

          if (aProbe > THE_MAX_PROBE_DISTANCE)
          {
            throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
          }
        }
      }

      ++aProbe;
      aIndex = (aIndex + 1) & aMask;

      if (aProbe > THE_MAX_PROBE_DISTANCE)
      {
        throw Standard_OutOfRange("NCollection_FlatDataMap: excessive probe length");
      }
    }
  }

  void backwardShiftDelete(size_t theIndex)
  {
    const size_t aMask    = myCapacity - 1;
    size_t       aCurrent = theIndex;
    size_t       aNext    = (aCurrent + 1) & aMask;

    while (mySlots[aNext].myState == SlotState::Used && mySlots[aNext].myProbeDistance > 0)
    {
      new (&mySlots[aCurrent].Key()) TheKeyType(std::move(mySlots[aNext].Key()));
      new (&mySlots[aCurrent].Item()) TheItemType(std::move(mySlots[aNext].Item()));
      mySlots[aCurrent].myHash          = mySlots[aNext].myHash;
      mySlots[aCurrent].myProbeDistance = mySlots[aNext].myProbeDistance - 1;
      mySlots[aCurrent].myState         = SlotState::Used;

      mySlots[aNext].Key().~TheKeyType();
      mySlots[aNext].Item().~TheItemType();

      aCurrent = aNext;
      aNext    = (aNext + 1) & aMask;
    }

    mySlots[aCurrent].myState = SlotState::Empty;
  }

private:
  Slot*  mySlots;    //!< Array of slots
  size_t myCapacity; //!< Total number of slots (always power of 2)
  size_t mySize;     //!< Number of used slots
  Hasher myHasher;   //!< Hash and equality functor
};

#endif // NCollection_FlatDataMap_HeaderFile

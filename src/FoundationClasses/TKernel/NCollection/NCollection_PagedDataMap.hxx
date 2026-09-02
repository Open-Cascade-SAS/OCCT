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

#ifndef _NCollection_PagedDataMap_HeaderFile
#define _NCollection_PagedDataMap_HeaderFile

#include <NCollection_PagedArray.hxx>

#include <NCollection_DefaultHasher.hxx>
#include <NCollection_ItemsView.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

//! Page-granular copy-on-write hash index.
//!
//! The bucket table uses open addressing. A copied map shares bucket pages;
//! insertion, replacement, and removal clone only pages containing modified
//! buckets. Capacity growth performs an explicit rehash and is amortized.
//! Retained copies are safe for concurrent const access and independently
//! synchronized updates. Concurrent mutation of the same map object requires
//! external synchronization.
template <typename TheKey,
          typename TheValue,
          typename TheHasher = NCollection_DefaultHasher<TheKey>>
class NCollection_PagedDataMap
{
public:
  DEFINE_STANDARD_ALLOC

  using key_type        = TheKey;
  using value_type      = TheValue;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = TheValue&;
  using const_reference = const TheValue&;
  using pointer         = TheValue*;
  using const_pointer   = const TheValue*;

private:
  template <bool IsConstant>
  class BasicIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = TheValue;
    using difference_type   = std::ptrdiff_t;
    using pointer           = std::conditional_t<IsConstant, const TheValue*, TheValue*>;
    using reference         = std::conditional_t<IsConstant, const TheValue&, TheValue&>;
    using map_pointer =
      std::conditional_t<IsConstant, const NCollection_PagedDataMap*, NCollection_PagedDataMap*>;

    BasicIterator() noexcept = default;

    template <bool B = IsConstant, std::enable_if_t<!B>* = nullptr>
    explicit BasicIterator(NCollection_PagedDataMap& theMap) noexcept
        : myMap(&theMap)
    {
      skipEmpty();
    }

    template <bool B = IsConstant, std::enable_if_t<B>* = nullptr>
    explicit BasicIterator(const NCollection_PagedDataMap& theMap) noexcept
        : myMap(&theMap)
    {
      skipEmpty();
    }

    template <bool B = IsConstant, std::enable_if_t<B>* = nullptr>
    BasicIterator(const BasicIterator<false>& theOther) noexcept
        : myMap(theOther.myMap),
          myIndex(theOther.myIndex)
    {
    }

    template <bool B = IsConstant, std::enable_if_t<B>* = nullptr>
    BasicIterator& operator=(const BasicIterator<false>& theOther) noexcept
    {
      myMap   = theOther.myMap;
      myIndex = theOther.myIndex;
      return *this;
    }

    reference operator*() const
    {
      if constexpr (IsConstant)
      {
        return myMap->myBuckets.Value(myIndex).Value();
      }
      else
      {
        return myMap->myBuckets.ChangeValue(myIndex).ChangeValue();
      }
    }

    pointer operator->() const { return &operator*(); }

    [[nodiscard]] bool More() const noexcept
    {
      return myMap != nullptr && myIndex < myMap->myBuckets.Size();
    }

    BasicIterator& operator++()
    {
      ++myIndex;
      skipEmpty();
      return *this;
    }

    BasicIterator operator++(int)
    {
      BasicIterator anOld(*this);
      ++(*this);
      return anOld;
    }

    [[nodiscard]] const TheKey& Key() const { return myMap->myBuckets.Value(myIndex).Key(); }

    [[nodiscard]] const TheValue& Value() const { return myMap->myBuckets.Value(myIndex).Value(); }

    template <bool B = IsConstant, std::enable_if_t<!B>* = nullptr>
    [[nodiscard]] TheValue& ChangeValue() const
    {
      return myMap->myBuckets.ChangeValue(myIndex).ChangeValue();
    }

    template <bool TheOtherIsConstant>
    bool operator==(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return (!More() && !theOther.More())
             || (myMap == theOther.myMap && myIndex == theOther.myIndex);
    }

    template <bool TheOtherIsConstant>
    bool operator!=(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

  private:
    template <bool>
    friend class BasicIterator;

    void skipEmpty() noexcept
    {
      while (myMap != nullptr && myIndex < myMap->myBuckets.Size()
             && !myMap->myBuckets.Value(myIndex).IsOccupied())
      {
        ++myIndex;
      }
    }

    map_pointer myMap   = nullptr;
    size_t      myIndex = 0;
  };

public:
  using iterator       = BasicIterator<false>;
  using const_iterator = BasicIterator<true>;

  class Iterator : public const_iterator
  {
  public:
    Iterator() noexcept = default;

    explicit Iterator(const NCollection_PagedDataMap& theMap) noexcept
        : const_iterator(theMap),
          myMap(&theMap)
    {
    }

    bool More() const noexcept { return const_iterator::More(); }

    void Next() noexcept { ++(*this); }

    const TheKey& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_PagedDataMap::Iterator::Key");
      return const_iterator::Key();
    }

    const TheValue& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_PagedDataMap::Iterator::Value");
      return const_iterator::Value();
    }

    void Initialize(const NCollection_PagedDataMap& theMap) noexcept { *this = Iterator(theMap); }

    void Reset() noexcept { *this = myMap == nullptr ? Iterator() : Iterator(*myMap); }

    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return const_iterator::operator==(static_cast<const const_iterator&>(theOther));
    }

  private:
    const NCollection_PagedDataMap* myMap = nullptr;
  };

  iterator begin() noexcept { return iterator(*this); }

  const_iterator begin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(); }

  const_iterator end() const noexcept { return const_iterator(); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  const_iterator cend() const noexcept { return const_iterator(); }

  using KeyValueRef      = NCollection_ItemsView::KeyValueRef<TheKey, TheValue, false>;
  using ConstKeyValueRef = NCollection_ItemsView::KeyValueRef<TheKey, TheValue, true>;

private:
  struct ItemsExtractor
  {
    template <class TheIterator>
    static KeyValueRef Extract(const TheIterator& theIterator)
    {
      return {theIterator.Key(), *theIterator};
    }
  };

  struct ConstItemsExtractor
  {
    template <class TheIterator>
    static ConstKeyValueRef Extract(const TheIterator& theIterator)
    {
      return {theIterator.Key(), *theIterator};
    }
  };

public:
  using ItemsView =
    NCollection_ItemsView::View<NCollection_PagedDataMap, KeyValueRef, ItemsExtractor, false>;
  using ConstItemsView = NCollection_ItemsView::
    View<NCollection_PagedDataMap, ConstKeyValueRef, ConstItemsExtractor, true>;

  ItemsView Items() { return ItemsView(*this); }

  ConstItemsView Items() const { return ConstItemsView(*this); }

  explicit NCollection_PagedDataMap(const size_t theInitialCapacity = 1)
  {
    initialize(capacityFor(theInitialCapacity));
  }

  NCollection_PagedDataMap(const TheHasher& theHasher, const size_t theInitialCapacity = 1)
      : myHasher(theHasher)
  {
    initialize(capacityFor(theInitialCapacity));
  }

  NCollection_PagedDataMap(TheHasher&& theHasher, const size_t theInitialCapacity = 1)
      : myHasher(std::move(theHasher))
  {
    initialize(capacityFor(theInitialCapacity));
  }
  [[nodiscard]] size_t Size() const noexcept { return mySize; }

  [[nodiscard]] bool IsEmpty() const noexcept { return mySize == 0; }

  [[nodiscard]] size_t Capacity() const noexcept { return myBuckets.Size(); }

  [[nodiscard]] const TheHasher& GetHasher() const noexcept { return myHasher; }

  [[nodiscard]] const TheValue* Seek(const TheKey& theKey) const
  {
    const size_t anIndex = findIndex(theKey);
    return anIndex == THE_NOT_FOUND ? nullptr : &myBuckets.Value(anIndex).Value();
  }

  [[nodiscard]] TheValue* ChangeSeek(const TheKey& theKey)
  {
    const size_t anIndex = findIndex(theKey);
    return anIndex == THE_NOT_FOUND ? nullptr : &myBuckets.ChangeValue(anIndex).ChangeValue();
  }

  [[nodiscard]] bool IsBound(const TheKey& theKey) const
  {
    return findIndex(theKey) != THE_NOT_FOUND;
  }

  //! Bind a new key and return false without modifying the map if it is already bound.
  bool Bind(const TheKey& theKey, const TheValue& theValue)
  {
    if (IsBound(theKey))
    {
      return false;
    }
    ensureInsertCapacity();
    bindWithoutResize(theKey, theValue);
    return true;
  }

  //! Bind a new key and return false without modifying the map if it is already bound.
  bool Bind(const TheKey& theKey, TheValue&& theValue)
  {
    if (IsBound(theKey))
    {
      return false;
    }
    ensureInsertCapacity();
    bindWithoutResize(theKey, std::move(theValue));
    return true;
  }

  TheValue& TryBound(const TheKey& theKey, const TheValue& theValue)
  {
    const size_t anExisting = findIndex(theKey);
    if (anExisting != THE_NOT_FOUND)
    {
      return myBuckets.ChangeValue(anExisting).ChangeValue();
    }
    ensureInsertCapacity();
    return bindWithoutResize(theKey, theValue);
  }

  bool UnBind(const TheKey& theKey)
  {
    const size_t anIndex = findIndex(theKey);
    if (anIndex == THE_NOT_FOUND)
    {
      return false;
    }
    Bucket& aBucket = myBuckets.ChangeValue(anIndex);
    aBucket.Remove();
    --mySize;
    ++myRemoved;
    return true;
  }

  void Reserve(const size_t theCapacity)
  {
    const size_t aRequired = capacityForEntries(theCapacity);
    if (aRequired > myBuckets.Size())
    {
      rehash(aRequired);
    }
  }

  //! Remove all entries, retaining capacity unless memory release is requested.
  void Clear(const bool theToReleaseMemory = false)
  {
    if (mySize == 0 && myRemoved == 0
        && (!theToReleaseMemory || myBuckets.Size() == THE_MIN_CAPACITY))
    {
      return;
    }
    initialize(theToReleaseMemory ? THE_MIN_CAPACITY : myBuckets.Size());
  }

private:
  enum class BucketState : uint8_t
  {
    Empty,
    Occupied,
    Removed
  };

  struct Bucket
  {
    using KeyValue = std::pair<TheKey, TheValue>;

    Bucket() noexcept = default;

    Bucket(const Bucket& theOther)
    {
      if (theOther.IsOccupied())
      {
        new (myStorage) KeyValue(theOther.keyValue());
        myState = BucketState::Occupied;
      }
      else
      {
        myState = theOther.myState;
      }
    }

    ~Bucket() { destroy(); }

    Bucket& operator=(const Bucket& theOther)
    {
      if (this != &theOther)
      {
        assign(theOther);
      }
      return *this;
    }

    [[nodiscard]] bool IsEmpty() const noexcept { return myState == BucketState::Empty; }

    [[nodiscard]] bool IsOccupied() const noexcept { return myState == BucketState::Occupied; }

    [[nodiscard]] bool IsRemoved() const noexcept { return myState == BucketState::Removed; }

    [[nodiscard]] const TheKey& Key() const noexcept { return keyValue().first; }

    [[nodiscard]] const TheValue& Value() const noexcept { return keyValue().second; }

    [[nodiscard]] TheValue& ChangeValue() noexcept { return changeKeyValue().second; }

    template <typename TheStoredValue>
    TheValue& Bind(const TheKey& theKey, TheStoredValue&& theValue)
    {
      new (myStorage) KeyValue(theKey, std::forward<TheStoredValue>(theValue));
      myState = BucketState::Occupied;
      return changeKeyValue().second;
    }

    void Remove() noexcept
    {
      destroy();
      myState = BucketState::Removed;
    }

  private:
    [[nodiscard]] const KeyValue& keyValue() const noexcept
    {
      return *std::launder(reinterpret_cast<const KeyValue*>(myStorage));
    }

    [[nodiscard]] KeyValue& changeKeyValue() noexcept
    {
      return *std::launder(reinterpret_cast<KeyValue*>(myStorage));
    }

    void destroy() noexcept
    {
      if (IsOccupied())
      {
        changeKeyValue().~KeyValue();
      }
    }

    void assign(const Bucket& theOther)
    {
      destroy();
      myState = BucketState::Empty;
      if (theOther.IsOccupied())
      {
        new (myStorage) KeyValue(theOther.keyValue());
        myState = BucketState::Occupied;
      }
      else
      {
        myState = theOther.myState;
      }
    }

    BucketState                     myState = BucketState::Empty;
    alignas(KeyValue) unsigned char myStorage[sizeof(KeyValue)];
  };

  static constexpr size_t THE_MIN_CAPACITY = 8;
  static constexpr size_t THE_PAGE_SIZE    = 64;
  static constexpr size_t THE_NOT_FOUND    = std::numeric_limits<size_t>::max();
  static constexpr size_t THE_MAX_CAPACITY = size_t(1)
                                                 << (std::numeric_limits<size_t>::digits - 1);

  static size_t loadLimit(const size_t theCapacity, const size_t theTenths) noexcept
  {
    return (theCapacity / 10) * theTenths + ((theCapacity % 10) * theTenths) / 10;
  }

  static size_t capacityFor(size_t theCapacity)
  {
    size_t aCapacity = THE_MIN_CAPACITY;
    while (aCapacity < theCapacity)
    {
      Standard_OutOfRange_Raise_if(aCapacity == THE_MAX_CAPACITY,
                                   "NCollection_PagedDataMap capacity is too large");
      aCapacity <<= 1;
    }
    return aCapacity;
  }

  static size_t capacityForEntries(const size_t theSize)
  {
    size_t aCapacity = THE_MIN_CAPACITY;
    while (loadLimit(aCapacity, 7) < theSize)
    {
      Standard_OutOfRange_Raise_if(aCapacity == THE_MAX_CAPACITY,
                                   "NCollection_PagedDataMap size is too large");
      aCapacity <<= 1;
    }
    return aCapacity;
  }

  void initialize(const size_t theCapacity)
  {
    NCollection_PagedArray<Bucket> aBuckets(THE_PAGE_SIZE);
    aBuckets.Resize(theCapacity);
    myBuckets = std::move(aBuckets);
    mySize    = 0;
    myRemoved = 0;
  }

  [[nodiscard]] size_t findIndex(const TheKey& theKey) const
  {
    const size_t aMask   = myBuckets.Size() - 1;
    size_t       anIndex = myHasher(theKey) & aMask;
    for (size_t aProbe = 0; aProbe < myBuckets.Size(); ++aProbe)
    {
      const Bucket& aBucket = myBuckets.Value(anIndex);
      if (aBucket.IsEmpty())
      {
        return THE_NOT_FOUND;
      }
      if (aBucket.IsOccupied() && myHasher(aBucket.Key(), theKey))
      {
        return anIndex;
      }
      anIndex = (anIndex + 1) & aMask;
    }
    return THE_NOT_FOUND;
  }

  void ensureInsertCapacity()
  {
    const size_t aCapacity = myBuckets.Size();
    if (mySize + myRemoved + 1 > loadLimit(aCapacity, 7))
    {
      if (mySize > loadLimit(aCapacity, 6))
      {
        Standard_OutOfRange_Raise_if(aCapacity == THE_MAX_CAPACITY,
                                     "NCollection_PagedDataMap size is too large");
        rehash(aCapacity << 1);
      }
      else
      {
        rehash(aCapacity);
      }
    }
  }

  template <typename TheStoredValue>
  TheValue& bindWithoutResize(const TheKey& theKey, TheStoredValue&& theValue)
  {
    const size_t aMask         = myBuckets.Size() - 1;
    size_t       anIndex       = myHasher(theKey) & aMask;
    size_t       aRemovedIndex = THE_NOT_FOUND;
    for (;;)
    {
      const Bucket& aReadBucket = myBuckets.Value(anIndex);
      if (aReadBucket.IsRemoved() && aRemovedIndex == THE_NOT_FOUND)
      {
        aRemovedIndex = anIndex;
      }
      else if (aReadBucket.IsEmpty())
      {
        const size_t aTarget = aRemovedIndex == THE_NOT_FOUND ? anIndex : aRemovedIndex;
        Bucket&      aBucket = myBuckets.ChangeValue(aTarget);
        TheValue&    aValue  = aBucket.Bind(theKey, std::forward<TheStoredValue>(theValue));
        ++mySize;
        if (aRemovedIndex != THE_NOT_FOUND)
        {
          --myRemoved;
        }
        return aValue;
      }
      anIndex = (anIndex + 1) & aMask;
    }
  }

  void rehash(const size_t theCapacity)
  {
    NCollection_PagedDataMap aRehashed(myHasher, capacityFor(theCapacity));
    for (size_t anIndex = 0; anIndex < myBuckets.Size(); ++anIndex)
    {
      const Bucket& aBucket = myBuckets.Value(anIndex);
      if (aBucket.IsOccupied())
      {
        aRehashed.bindWithoutResize(aBucket.Key(), aBucket.Value());
      }
    }
    myBuckets = std::move(aRehashed.myBuckets);
    mySize    = aRehashed.mySize;
    myRemoved = 0;
  }

  NCollection_PagedArray<Bucket> myBuckets{THE_PAGE_SIZE};
  size_t                         mySize    = 0;
  size_t                         myRemoved = 0;
  TheHasher                      myHasher;
};

#endif // _NCollection_PagedDataMap_HeaderFile

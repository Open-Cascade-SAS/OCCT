// Copyright (c) 2002-2023 OPEN CASCADE SAS
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

#ifndef NCollection_DynamicArray_HeaderFile
#define NCollection_DynamicArray_HeaderFile

#include <NCollection_Allocator.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_DefineAlloc.hxx>
#include <NCollection_Iterator.hxx>
#include <NCollection_OccAllocator.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_IndexedIterator.hxx>

#include <cstring>
#include <locale>
#include <type_traits>
#include <vector>

//! Class NCollection_DynamicArray (dynamic array of objects)
//!
//! The array's indices always start at 0.
//!
//! The Vector is always created with 0 length. It can be enlarged by two means:
//!  1. Calling the method Append (val) - then "val" is added to the end of the
//!     vector (the vector length is incremented)
//!  2. Calling the method SetValue (i, val) - if "i" is greater than or equal
//!     to the current length of the vector, the vector is enlarged to accomo-
//!     date this index
//!
//! The methods Append and SetValue return a non-const reference to the copied
//! object inside the vector. This reference is guaranteed to be valid until
//! the vector is destroyed. It can be used to access the vector member directly
//! or to pass its address to other data structures.
//!
//! The vector iterator remembers the length of the vector at the moment of the
//! creation or initialisation of the iterator. Therefore the iteration begins
//! at index 0 and stops at the index equal to (remembered_length-1). It is OK
//! to enlarge the vector during the iteration.
template <class TheItemType>
class NCollection_DynamicArray
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;

public:
  typedef NCollection_OccAllocator<TheItemType>  allocator_type;
  typedef NCollection_LinearVector<TheItemType*> vector;

public:
  // Define various type aliases for convenience
  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = size_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;

public:
  using iterator       = NCollection_IndexedIterator<std::random_access_iterator_tag,
                                                     NCollection_DynamicArray,
                                                     value_type,
                                                     false>;
  using const_iterator = NCollection_IndexedIterator<std::random_access_iterator_tag,
                                                     NCollection_DynamicArray,
                                                     value_type,
                                                     true>;
  using Iterator       = NCollection_Iterator<NCollection_DynamicArray<TheItemType>>;

public:
  const_iterator begin() const noexcept { return const_iterator(*this); }

  iterator begin() noexcept { return iterator(*this); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(myUsedSize, *this); }

  const_iterator end() const noexcept { return const_iterator(myUsedSize, *this); }

  const_iterator cend() const noexcept { return const_iterator(myUsedSize, *this); }

public: //! @name public methods
  NCollection_DynamicArray(const size_t theIncrement)
      : myAlloc(),
        myInternalSize(roundUpPow2(theIncrement)),
        myBlockShift(log2Pow2(myInternalSize)),
        myBlockMask(myInternalSize - 1),
        myUsedSize(0)
  {
  }

  NCollection_DynamicArray(const int theIncrement = 256)
      : NCollection_DynamicArray(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement))
  {
  }

  // Constructor taking an allocator
  explicit NCollection_DynamicArray(const size_t                                  theIncrement,
                                    const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myAlloc(allocator_type(theAllocator)),
        myInternalSize(roundUpPow2(theIncrement)),
        myBlockShift(log2Pow2(myInternalSize)),
        myBlockMask(myInternalSize - 1),
        myUsedSize(0)
  {
  }

  explicit NCollection_DynamicArray(const int                                     theIncrement,
                                    const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : NCollection_DynamicArray(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement),
                                 theAllocator)
  {
  }

  // Constructor taking an allocator
  explicit NCollection_DynamicArray(const size_t theIncrement, const allocator_type& theAllocator)
      : myAlloc(theAllocator),
        myInternalSize(roundUpPow2(theIncrement)),
        myBlockShift(log2Pow2(myInternalSize)),
        myBlockMask(myInternalSize - 1),
        myUsedSize(0)
  {
  }

  explicit NCollection_DynamicArray(const int theIncrement, const allocator_type& theAllocator)
      : NCollection_DynamicArray(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement),
                                 theAllocator)
  {
  }

  //! Copy constructor
  NCollection_DynamicArray(const NCollection_DynamicArray& theOther)
      : myContainer(theOther.myContainer),
        myAlloc(theOther.myAlloc),
        myInternalSize(theOther.myInternalSize),
        myBlockShift(theOther.myBlockShift),
        myBlockMask(theOther.myBlockMask),
        myUsedSize(theOther.myUsedSize)
  {
    copyDate();
  }

  NCollection_DynamicArray(NCollection_DynamicArray&& theOther) noexcept
      : myContainer(std::move(theOther.myContainer)),
        myAlloc(theOther.myAlloc),
        myInternalSize(theOther.myInternalSize),
        myBlockShift(theOther.myBlockShift),
        myBlockMask(theOther.myBlockMask),
        myUsedSize(theOther.myUsedSize)
  {
    theOther.myUsedSize = 0;
  }

  ~NCollection_DynamicArray() { Clear(true); }

  //! Total number of items in the vector.
  size_t Size() const noexcept { return myUsedSize; }

  //! Total number of items (legacy int-returning API).
  int Length() const noexcept { return static_cast<int>(myUsedSize); }

  //! Method for consistency with other collections.
  //! @return Lower bound (inclusive) for iteration.
  int Lower() const noexcept { return 0; }

  //! Method for consistency with other collections.
  //! @return Upper bound (inclusive) for iteration.
  int Upper() const noexcept { return static_cast<int>(myUsedSize) - 1; }

  //! Empty query
  bool IsEmpty() const noexcept { return myUsedSize == 0; }

  //! Assignment to the collection of the same type
  NCollection_DynamicArray& Assign(const NCollection_DynamicArray& theOther,
                                   const bool                      theOwnAllocator = true)
  {
    if (&theOther == this)
    {
      return *this;
    }
    if (!theOwnAllocator)
    {
      Clear(myAlloc != theOther.myAlloc);
      myAlloc = theOther.myAlloc;
    }
    else
    {
      Clear(false);
    }
    myContainer    = theOther.myContainer;
    myInternalSize = theOther.myInternalSize;
    myBlockShift   = theOther.myBlockShift;
    myBlockMask    = theOther.myBlockMask;
    myUsedSize     = theOther.myUsedSize;
    copyDate();
    return *this;
  }

  NCollection_DynamicArray& Assign(NCollection_DynamicArray&& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    Clear(true);
    myContainer         = std::move(theOther.myContainer);
    myAlloc             = theOther.myAlloc;
    myInternalSize      = theOther.myInternalSize;
    myBlockShift        = theOther.myBlockShift;
    myBlockMask         = theOther.myBlockMask;
    myUsedSize          = theOther.myUsedSize;
    theOther.myUsedSize = 0;
    return *this;
  }

  //! Assignment operator
  NCollection_DynamicArray& operator=(const NCollection_DynamicArray& theOther)
  {
    return Assign(theOther, false);
  }

  //! Assignment operator
  NCollection_DynamicArray& operator=(NCollection_DynamicArray&& theOther) noexcept
  {
    return Assign(std::forward<NCollection_DynamicArray>(theOther));
  }

  //! Append
  reference Append(const TheItemType& theValue)
  {
    if (myUsedSize >= availableSize())
    {
      expandArray();
    }
    pointer aPnt = &at(myUsedSize++);
    myAlloc.construct(aPnt, theValue);
    return *aPnt;
  }

  //! Append
  reference Append(TheItemType&& theValue)
  {
    if (myUsedSize >= availableSize())
    {
      expandArray();
    }
    pointer aPnt = &at(myUsedSize++);
    myAlloc.construct(aPnt, std::forward<TheItemType>(theValue));
    return *aPnt;
  }

  //! Insert a value after the element at theIndex, shifting subsequent elements right.
  //! @param theIndex index after which to insert (must be in [0, Size()-1])
  //! @param theValue value to insert
  //! @return reference to the inserted element
  reference InsertAfter(const size_t theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex + 1; --i)
      at(i) = std::move(at(i - 1));
    at(theIndex + 1) = theValue;
    return at(theIndex + 1);
  }

  //! Insert a value after the element at theIndex (move version).
  reference InsertAfter(const size_t theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex + 1; --i)
      at(i) = std::move(at(i - 1));
    at(theIndex + 1) = std::forward<TheItemType>(theValue);
    return at(theIndex + 1);
  }

  reference InsertAfter(const int theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    return InsertAfter(static_cast<size_t>(theIndex), theValue);
  }

  reference InsertAfter(const int theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    return InsertAfter(static_cast<size_t>(theIndex), std::forward<TheItemType>(theValue));
  }

  //! Insert a value before the element at theIndex, shifting it and subsequent elements right.
  //! @param theIndex index before which to insert (must be in [0, Size()-1])
  //! @param theValue value to insert
  //! @return reference to the inserted element
  reference InsertBefore(const size_t theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex; --i)
      at(i) = std::move(at(i - 1));
    at(theIndex) = theValue;
    return at(theIndex);
  }

  //! Insert a value before the element at theIndex (move version).
  reference InsertBefore(const size_t theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex; --i)
      at(i) = std::move(at(i - 1));
    at(theIndex) = std::forward<TheItemType>(theValue);
    return at(theIndex);
  }

  reference InsertBefore(const int theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    return InsertBefore(static_cast<size_t>(theIndex), theValue);
  }

  reference InsertBefore(const int theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    return InsertBefore(static_cast<size_t>(theIndex), std::forward<TheItemType>(theValue));
  }

  void EraseLast()
  {
    if (myUsedSize == 0)
    {
      return;
    }
    if constexpr (!std::is_trivially_destructible_v<TheItemType>)
    {
      TheItemType* aLastElem = &ChangeLast();
      myAlloc.destroy(aLastElem);
    }
    myUsedSize--;
  }

  //! Appends an empty value and returns the reference to it
  reference Appended()
  {
    if (myUsedSize >= availableSize())
    {
      expandArray();
    }
    pointer aPnt = &at(myUsedSize++);
    myAlloc.construct(aPnt);
    return *aPnt;
  }

  //! Emplace one item at the end, constructing it in-place
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceAppend(Args&&... theArgs)
  {
    if (myUsedSize >= availableSize())
    {
      expandArray();
    }
    pointer aPnt = &at(myUsedSize++);
    myAlloc.construct(aPnt, std::forward<Args>(theArgs)...);
    return *aPnt;
  }

  //! Emplace value at the specified index, constructing it in-place
  //! If the index is beyond current size, default-constructs intermediate elements
  //! @param theIndex index at which to emplace the value
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceValue(const size_t theIndex, Args&&... theArgs)
  {
    ensureStorageForIndex(theIndex);
    const bool isExisting = theIndex < myUsedSize;
    if (!isExisting)
    {
      for (; myUsedSize < theIndex; myUsedSize++)
      {
        pointer aPnt = &at(myUsedSize);
        myAlloc.construct(aPnt);
      }
      myUsedSize++;
    }
    pointer aPnt = &at(theIndex);
    if (isExisting)
    {
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        myAlloc.destroy(aPnt);
      }
    }
    myAlloc.construct(aPnt, std::forward<Args>(theArgs)...);
    return *aPnt;
  }

  template <typename... Args>
  reference EmplaceValue(const int theIndex, Args&&... theArgs)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::EmplaceValue: index out of range");
    return EmplaceValue(static_cast<size_t>(theIndex), std::forward<Args>(theArgs)...);
  }

  //! Operator() - query the const value
  const_reference operator()(const size_t theIndex) const noexcept { return at(theIndex); }

  const_reference operator()(const int theIndex) const noexcept
  {
    return at(static_cast<size_t>(theIndex));
  }

  //! Operator[] - query the const value
  const_reference operator[](const size_t theIndex) const noexcept { return at(theIndex); }

  const_reference operator[](const int theIndex) const noexcept
  {
    return at(static_cast<size_t>(theIndex));
  }

  const_reference Value(const size_t theIndex) const noexcept { return at(theIndex); }

  const_reference Value(const int theIndex) const noexcept
  {
    return at(static_cast<size_t>(theIndex));
  }

  //! @return first element
  const_reference First() const noexcept { return getArray()[0][0]; }

  //! @return first element
  reference ChangeFirst() noexcept { return getArray()[0][0]; }

  //! @return last element
  const_reference Last() const noexcept { return at(myUsedSize - 1); }

  //! @return last element
  reference ChangeLast() noexcept { return at(myUsedSize - 1); }

  //! Operator() - query the value
  reference operator()(const size_t theIndex) noexcept { return at(theIndex); }

  reference operator()(const int theIndex) noexcept { return at(static_cast<size_t>(theIndex)); }

  //! Operator[] - query the value
  reference operator[](const size_t theIndex) noexcept { return at(theIndex); }

  reference operator[](const int theIndex) noexcept { return at(static_cast<size_t>(theIndex)); }

  reference ChangeValue(const size_t theIndex) noexcept { return at(theIndex); }

  reference ChangeValue(const int theIndex) noexcept { return at(static_cast<size_t>(theIndex)); }

  //! SetValue () - set or append a value
  reference SetValue(const size_t theIndex, const TheItemType& theValue)
  {
    ensureStorageForIndex(theIndex);
    const bool isExisting = theIndex < myUsedSize;
    if (!isExisting)
    {
      for (; myUsedSize < theIndex; myUsedSize++)
      {
        pointer aPnt = &at(myUsedSize);
        myAlloc.construct(aPnt);
      }
      myUsedSize++;
    }
    pointer aPnt = &at(theIndex);
    if (isExisting)
    {
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        myAlloc.destroy(aPnt);
      }
    }
    myAlloc.construct(aPnt, theValue);
    return *aPnt;
  }

  //! SetValue () - set or append a value
  reference SetValue(const size_t theIndex, TheItemType&& theValue)
  {
    ensureStorageForIndex(theIndex);
    const bool isExisting = theIndex < myUsedSize;
    if (!isExisting)
    {
      for (; myUsedSize < theIndex; myUsedSize++)
      {
        pointer aPnt = &at(myUsedSize);
        myAlloc.construct(aPnt);
      }
      myUsedSize++;
    }
    pointer aPnt = &at(theIndex);
    if (isExisting)
    {
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        myAlloc.destroy(aPnt);
      }
    }
    myAlloc.construct(aPnt, std::forward<TheItemType>(theValue));
    return *aPnt;
  }

  reference SetValue(const int theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::SetValue: index out of range");
    return SetValue(static_cast<size_t>(theIndex), theValue);
  }

  reference SetValue(const int theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::SetValue: index out of range");
    return SetValue(static_cast<size_t>(theIndex), std::forward<TheItemType>(theValue));
  }

  void Clear(const bool theReleaseMemory = false)
  {
    for (size_t aBlockInd = 0; aBlockInd < myContainer.Size(); aBlockInd++)
    {
      TheItemType* aCurStart = getArray()[aBlockInd];
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        const size_t aBlockStart = aBlockInd * myInternalSize;
        const size_t aCount      = (myUsedSize > aBlockStart + myInternalSize)
                                     ? myInternalSize
                                     : (myUsedSize > aBlockStart ? myUsedSize - aBlockStart : 0);
        for (size_t anElemInd = 0; anElemInd < aCount; anElemInd++)
        {
          aCurStart[anElemInd].~TheItemType();
        }
      }
      if (theReleaseMemory)
        myAlloc.deallocate(aCurStart, myInternalSize);
    }
    if (theReleaseMemory)
      myContainer.Clear(theReleaseMemory);
    myUsedSize = 0;
  }

  void SetIncrement(const size_t theIncrement) noexcept
  {
    if (myUsedSize != 0)
    {
      return;
    }
    myInternalSize = roundUpPow2(theIncrement);
    myBlockShift   = log2Pow2(myInternalSize);
    myBlockMask    = myInternalSize - 1;
  }

  void SetIncrement(const int theIncrement) noexcept
  {
    SetIncrement(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement));
  }

  friend iterator;
  friend const_iterator;

protected:
  size_t availableSize() const noexcept
  {
    return static_cast<size_t>(myContainer.Size()) << myBlockShift;
  }

  //! Ensure storage blocks exist to access theIndex.
  void ensureStorageForIndex(const size_t theIndex)
  {
    const size_t aRequiredBlocks = (theIndex >> myBlockShift) + 1;
    ensureBlockCount(aRequiredBlocks);
  }

  //! Ensure at least theBlockCount blocks are allocated in myContainer.
  void ensureBlockCount(const size_t theBlockCount)
  {
    if (theBlockCount > myContainer.Capacity())
    {
      myContainer.Reserve(theBlockCount);
    }
    while (myContainer.Size() < theBlockCount)
    {
      expandArray();
    }
  }

  TheItemType* expandArray()
  {
    TheItemType* aNewBlock = myAlloc.allocate(myInternalSize);
    myContainer.Append(aNewBlock);
    return aNewBlock;
  }

  reference at(const size_t theInd) noexcept
  {
    return getArray()[theInd >> myBlockShift][theInd & myBlockMask];
  }

  const_reference at(const size_t theInd) const noexcept
  {
    return getArray()[theInd >> myBlockShift][theInd & myBlockMask];
  }

  void copyDate()
  {
    size_t aUsedSize = 0;
    for (size_t aBlockInd = 0; aBlockInd < myContainer.Size(); aBlockInd++)
    {
      TheItemType* aCurStart = getArray()[aBlockInd];
      TheItemType* aNewBlock = myAlloc.allocate(myInternalSize);
      if constexpr (std::is_trivially_copyable_v<TheItemType>)
      {
        const size_t aCount =
          (myUsedSize - aUsedSize < myInternalSize) ? myUsedSize - aUsedSize : myInternalSize;
        std::memcpy(aNewBlock, aCurStart, aCount * sizeof(TheItemType));
        aUsedSize += aCount;
      }
      else
      {
        for (size_t anElemInd = 0; anElemInd < myInternalSize && aUsedSize < myUsedSize;
             anElemInd++, aUsedSize++)
        {
          pointer aPnt = &aNewBlock[anElemInd];
          myAlloc.construct(aPnt, aCurStart[anElemInd]);
        }
      }
      getArray()[aBlockInd] = aNewBlock;
    }
  }

  //! Wrapper to extract array of block pointers.
  TheItemType** getArray() noexcept { return myContainer.IsEmpty() ? nullptr : &myContainer[0]; }

  //! Wrapper to extract array of block pointers (const overload).
  TheItemType* const* getArray() const noexcept
  {
    return myContainer.IsEmpty() ? nullptr : &myContainer[0];
  }

  //! Round up to the nearest power of 2 (returns theValue if already power of 2).
  //! Works correctly for both 32-bit and 64-bit size_t.
  static constexpr size_t roundUpPow2(const size_t theValue) noexcept
  {
    size_t v = (theValue < 1 ? 1 : theValue);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    if constexpr (sizeof(size_t) > 4)
    {
      v |= v >> 32;
    }
    v++;
    return v;
  }

  //! Compute log2 of a power-of-2 value.
  static constexpr size_t log2Pow2(const size_t theValue) noexcept
  {
    size_t aShift = 0;
    size_t v      = theValue;
    while (v > 1)
    {
      v >>= 1;
      ++aShift;
    }
    return aShift;
  }

protected:
  vector         myContainer;
  allocator_type myAlloc;
  size_t         myInternalSize;
  size_t         myBlockShift; //!< log2(myInternalSize) for fast index-to-block mapping
  size_t         myBlockMask;  //!< myInternalSize - 1 for fast index-within-block mapping
  size_t         myUsedSize;
};

#endif // NCollection_DynamicArray_HeaderFile

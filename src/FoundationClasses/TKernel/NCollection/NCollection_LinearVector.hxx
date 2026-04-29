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

#ifndef NCollection_LinearVector_HeaderFile
#define NCollection_LinearVector_HeaderFile

#include <NCollection_Allocator.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>

//! Contiguous dynamic array using a flat memory buffer.
//!
//! Unlike NCollection_DynamicArray which uses segmented block storage,
//! this container stores all elements in a single contiguous allocation,
//! providing O(1) element access with a single pointer dereference.
//!
//! For trivially copyable types, growth uses Standard::Reallocate which
//! can extend the buffer in-place without copying elements. For non-trivial
//! types, growth allocates a new buffer and move-constructs elements.
//!
//! Indices are always 0-based.
//!
//! @warning Any operation that may grow the buffer - Append, Appended,
//!          EmplaceAppend, SetValue past end, Resize, Reserve, InsertBefore,
//!          InsertAfter, copy/move assignment - invalidates all iterators,
//!          references, and raw pointers into the vector whenever it
//!          actually reallocates. Erase/EraseLast also invalidate references
//!          at or beyond the removed position.
template <typename TheItemType>
class NCollection_LinearVector
{
public:
  using value_type      = TheItemType;
  using size_type       = size_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;
  using iterator        = TheItemType*;
  using const_iterator  = const TheItemType*;
  using allocator_type  = NCollection_Allocator<TheItemType>;

public:
  //! Empty constructor.
  NCollection_LinearVector() noexcept = default;

  //! Constructor with pre-allocated capacity.
  //! Unlike std::vector(n), this constructor does not create elements.
  //! Use Resize() or NCollection_LinearVector(theSize, theValue) to construct items.
  //! @param[in] theCapacity number of elements to pre-allocate
  explicit NCollection_LinearVector(const size_t theCapacity)
  {
    if (theCapacity > 0)
    {
      myData     = myAlloc.allocate(theCapacity);
      myCapacity = theCapacity;
    }
  }

  //! Constructor creating theSize elements initialized to theValue.
  //! Equivalent to std::vector(n, val).
  //! @param[in] theSize   number of elements to construct
  //! @param[in] theValue  value to initialize each element with
  NCollection_LinearVector(const size_t theSize, const TheItemType& theValue)
  {
    Resize(theSize, theValue);
  }

  //! Copy constructor.
  NCollection_LinearVector(const NCollection_LinearVector& theOther)
  {
    if (theOther.mySize > 0)
    {
      myData     = myAlloc.allocate(theOther.mySize);
      myCapacity = theOther.mySize;
      mySize     = theOther.mySize;
      if constexpr (std::is_trivially_copyable_v<TheItemType>)
      {
        std::memcpy(myData, theOther.myData, mySize * sizeof(TheItemType));
      }
      else
      {
        for (size_t i = 0; i < mySize; ++i)
        {
          myAlloc.construct(myData + i, theOther.myData[i]);
        }
      }
    }
  }

  //! Move constructor.
  NCollection_LinearVector(NCollection_LinearVector&& theOther) noexcept
      : myData(theOther.myData),
        mySize(theOther.mySize),
        myCapacity(theOther.myCapacity)
  {
    theOther.myData     = nullptr;
    theOther.mySize     = 0;
    theOther.myCapacity = 0;
  }

  //! Destructor.
  ~NCollection_LinearVector() { Clear(true); }

  //! Copy assignment.
  NCollection_LinearVector& operator=(const NCollection_LinearVector& theOther)
  {
    if (this != &theOther)
    {
      if (theOther.mySize > myCapacity)
      {
        NCollection_LinearVector aTmp(theOther);
        *this = std::move(aTmp);
        return *this;
      }

      if constexpr (std::is_trivially_copyable_v<TheItemType>)
      {
        if (theOther.mySize > 0)
        {
          std::memcpy(myData, theOther.myData, theOther.mySize * sizeof(TheItemType));
        }
      }
      else
      {
        const size_t aCommonSize = std::min(mySize, theOther.mySize);
        for (size_t i = 0; i < aCommonSize; ++i)
        {
          myData[i] = theOther.myData[i];
        }
        for (size_t i = aCommonSize; i < theOther.mySize; ++i)
        {
          myAlloc.construct(myData + i, theOther.myData[i]);
        }
        destroyRange(theOther.mySize, mySize);
      }
      mySize = theOther.mySize;
    }
    return *this;
  }

  //! Move assignment.
  NCollection_LinearVector& operator=(NCollection_LinearVector&& theOther) noexcept
  {
    if (this != &theOther)
    {
      Clear(true);
      myData              = theOther.myData;
      mySize              = theOther.mySize;
      myCapacity          = theOther.myCapacity;
      theOther.myData     = nullptr;
      theOther.mySize     = 0;
      theOther.myCapacity = 0;
    }
    return *this;
  }

  //! @return raw data pointer.
  TheItemType* Data() noexcept { return myData; }

  //! @return raw data pointer.
  const TheItemType* Data() const noexcept { return myData; }

  //! @return true if the vector has allocated storage.
  bool HasData() const noexcept { return myData != nullptr; }

  //! @return true if the vector contains no elements.
  bool Empty() const noexcept { return IsEmpty(); }

  //! @return current max supported size.
  static constexpr size_t MaxSize() noexcept { return std::numeric_limits<size_t>::max(); }

  //! @return number of elements.
  size_t Size() const noexcept { return mySize; }

  //! @return true if the vector contains no elements.
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! @return current allocated capacity.
  size_t Capacity() const noexcept { return myCapacity; }

  //! Pre-allocate memory for at least theCapacity elements without changing size.
  //! @param[in] theCapacity minimum capacity to ensure
  void Reserve(const size_t theCapacity)
  {
    if (theCapacity > myCapacity)
    {
      grow(theCapacity);
    }
  }

  //! Change the number of elements.
  //! If theSize > Size(), new elements are default-constructed.
  //! If theSize < Size(), excess elements are destroyed.
  //! @param[in] theSize new number of elements
  void Resize(const size_t theSize) { Resize(theSize, TheItemType()); }

  //! Change the number of elements, filling new slots with theValue.
  //! If theSize > Size(), new elements are copy-constructed from theValue.
  //! If theSize < Size(), excess elements are destroyed.
  //! @param[in] theSize  new number of elements
  //! @param[in] theValue value to fill new elements with
  void Resize(const size_t theSize, const TheItemType& theValue)
  {
    if (theSize > mySize)
    {
      if (theSize > myCapacity)
      {
        grow(theSize);
      }
      for (size_t i = mySize; i < theSize; ++i)
      {
        myAlloc.construct(myData + i, theValue);
      }
    }
    else if (theSize < mySize)
    {
      destroyRange(theSize, mySize);
    }
    mySize = theSize;
  }

  //! @return const reference to element at theIndex.
  //! @param[in] theIndex element index (0-based)
  const TheItemType& Value(const size_t theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_LinearVector::Value");
    return myData[theIndex];
  }

  //! @return mutable reference to element at theIndex.
  //! @param[in] theIndex element index (0-based)
  TheItemType& ChangeValue(const size_t theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_LinearVector::ChangeValue");
    return myData[theIndex];
  }

  //! @return const reference to element at theIndex.
  const TheItemType& operator()(const size_t theIndex) const { return myData[theIndex]; }

  //! @return mutable reference to element at theIndex.
  TheItemType& operator()(const size_t theIndex) { return myData[theIndex]; }

  //! @return const reference to element at theIndex.
  const TheItemType& operator[](const size_t theIndex) const { return myData[theIndex]; }

  //! @return mutable reference to element at theIndex.
  TheItemType& operator[](const size_t theIndex) { return myData[theIndex]; }

  //! @return const reference to the first element.
  const TheItemType& First() const
  {
    Standard_OutOfRange_Raise_if(mySize == 0, "NCollection_LinearVector::First");
    return myData[0];
  }

  //! @return mutable reference to the first element.
  TheItemType& ChangeFirst()
  {
    Standard_OutOfRange_Raise_if(mySize == 0, "NCollection_LinearVector::ChangeFirst");
    return myData[0];
  }

  //! @return const reference to the last element.
  const TheItemType& Last() const
  {
    Standard_OutOfRange_Raise_if(mySize == 0, "NCollection_LinearVector::Last");
    return myData[mySize - 1];
  }

  //! @return mutable reference to the last element.
  TheItemType& ChangeLast()
  {
    Standard_OutOfRange_Raise_if(mySize == 0, "NCollection_LinearVector::ChangeLast");
    return myData[mySize - 1];
  }

  //! Append a copy of theValue to the end.
  //! @param[in] theValue element to append
  //! @return reference to the appended element
  TheItemType& Append(const TheItemType& theValue)
  {
    if (mySize == myCapacity)
    {
      grow(mySize + 1);
    }
    myAlloc.construct(myData + mySize, theValue);
    return myData[mySize++];
  }

  //! Append theValue by move to the end.
  //! @param[in] theValue element to move-append
  //! @return reference to the appended element
  TheItemType& Append(TheItemType&& theValue)
  {
    if (mySize == myCapacity)
    {
      grow(mySize + 1);
    }
    myAlloc.construct(myData + mySize, std::move(theValue));
    return myData[mySize++];
  }

  //! Append a default-constructed element.
  //! @return reference to the appended element
  TheItemType& Appended()
  {
    if (mySize == myCapacity)
    {
      grow(mySize + 1);
    }
    myAlloc.construct(myData + mySize, TheItemType());
    return myData[mySize++];
  }

  //! Append an element constructed in-place with the given arguments.
  //! @param[in] theArgs constructor arguments
  //! @return reference to the appended element
  template <class... Args>
  TheItemType& EmplaceAppend(Args&&... theArgs)
  {
    if (mySize == myCapacity)
    {
      grow(mySize + 1);
    }
    myAlloc.construct(myData + mySize, std::forward<Args>(theArgs)...);
    return myData[mySize++];
  }

  //! Set value at theIndex. If theIndex >= Size(), the vector is extended.
  //! @param[in] theIndex element index (0-based)
  //! @param[in] theValue value to set
  //! @return reference to the element
  TheItemType& SetValue(const size_t theIndex, const TheItemType& theValue)
  {
    if (theIndex >= mySize)
    {
      Resize(theIndex + 1);
    }
    myData[theIndex] = theValue;
    return myData[theIndex];
  }

  //! Set value at theIndex by move. If theIndex >= Size(), the vector is extended.
  //! @param[in] theIndex element index (0-based)
  //! @param[in] theValue value to set
  //! @return reference to the element
  TheItemType& SetValue(const size_t theIndex, TheItemType&& theValue)
  {
    if (theIndex >= mySize)
    {
      Resize(theIndex + 1);
    }
    myData[theIndex] = std::move(theValue);
    return myData[theIndex];
  }

  //! Insert theValue before theIndex, shifting elements right.
  //! @param[in] theIndex insertion position (0-based)
  //! @param[in] theValue element to insert
  void InsertBefore(const size_t theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex > mySize, "NCollection_LinearVector::InsertBefore");
    if (mySize == myCapacity)
    {
      grow(mySize + 1);
    }
    if (theIndex < mySize)
    {
      shiftRight(theIndex, 1);
    }
    myAlloc.construct(myData + theIndex, theValue);
    ++mySize;
  }

  //! Insert theValue after theIndex, shifting elements right.
  //! @param[in] theIndex position after which to insert (0-based)
  //! @param[in] theValue element to insert
  void InsertAfter(const size_t theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_LinearVector::InsertAfter");
    InsertBefore(theIndex + 1, theValue);
  }

  //! Insert theValue before theIndex, shifting elements right.
  //! @param[in] theIndex insertion position (0-based)
  //! @param[in] theValue element to move-insert
  void InsertBefore(const size_t theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex > mySize, "NCollection_LinearVector::InsertBefore");
    if (mySize == myCapacity)
    {
      grow(mySize + 1);
    }
    if (theIndex < mySize)
    {
      shiftRight(theIndex, 1);
    }
    myAlloc.construct(myData + theIndex, std::move(theValue));
    ++mySize;
  }

  //! Insert theValue after theIndex, shifting elements right.
  //! @param[in] theIndex position after which to insert (0-based)
  //! @param[in] theValue element to move-insert
  void InsertAfter(const size_t theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_LinearVector::InsertAfter");
    InsertBefore(theIndex + 1, std::move(theValue));
  }

  //! Remove the last element.
  void EraseLast()
  {
    if (mySize > 0)
    {
      --mySize;
      destroyRange(mySize, mySize + 1);
    }
  }

  //! Remove element at theIndex, shifting subsequent elements left.
  //! @param[in] theIndex element index (0-based)
  void Erase(const size_t theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_LinearVector::Erase");
    // Shift first (move-assign into still-live slots), then destroy the
    // vacated tail slot.  Destroying before shifting would leave
    // myData[theIndex] uninitialized; a non-trivial move-assignment (e.g.
    // TCollection_AsciiString's, which inspects its current buffer before
    // freeing) would then read garbage from the destructed slot and crash.
    if (theIndex + 1 < mySize)
    {
      shiftLeft(theIndex, theIndex + 1, mySize);
    }
    destroyRange(mySize - 1, mySize);
    --mySize;
  }

  //! Remove elements in range [theFrom, theTo), shifting subsequent elements left.
  //! @param[in] theFrom start index (inclusive, 0-based)
  //! @param[in] theTo   end index (exclusive, 0-based)
  void Erase(const size_t theFrom, const size_t theTo)
  {
    Standard_OutOfRange_Raise_if(theTo > mySize || theFrom >= theTo,
                                 "NCollection_LinearVector::Erase");
    const size_t aCount = theTo - theFrom;
    // Shift first (move-assign over still-live slots), then destroy the
    // vacated tail - see Erase(size_t) above for the rationale.
    if (theTo < mySize)
    {
      shiftLeft(theFrom, theTo, mySize);
    }
    destroyRange(mySize - aCount, mySize);
    mySize -= aCount;
  }

  //! Remove all elements.
  //! @param[in] theReleaseMemory if true, deallocate the buffer
  void Clear(const bool theReleaseMemory = false)
  {
    destroyRange(0, mySize);
    mySize = 0;
    if (theReleaseMemory && myData != nullptr)
    {
      myAlloc.deallocate(myData, myCapacity);
      myData     = nullptr;
      myCapacity = 0;
    }
  }

  //! @return iterator to the first element.
  iterator begin() noexcept { return myData; }

  //! @return iterator past the last element.
  iterator end() noexcept { return myData + mySize; }

  //! @return const iterator to the first element.
  const_iterator begin() const noexcept { return myData; }

  //! @return const iterator past the last element.
  const_iterator end() const noexcept { return myData + mySize; }

  //! @return const iterator to the first element.
  const_iterator cbegin() const noexcept { return myData; }

  //! @return const iterator past the last element.
  const_iterator cend() const noexcept { return myData + mySize; }

private:
  //! Grow the buffer to accommodate at least theMinCapacity elements.
  void grow(const size_t theMinCapacity)
  {
    Standard_OutOfMemory_Raise_if(theMinCapacity > MaxSize(), "NCollection_LinearVector::grow");
    size_t aNewCap = myCapacity > 0 ? myCapacity * 2 : 8;
    if (myCapacity > MaxSize() / 2)
    {
      aNewCap = MaxSize();
    }
    if (aNewCap < theMinCapacity)
    {
      aNewCap = theMinCapacity;
    }
    if constexpr (std::is_trivially_copyable_v<TheItemType>)
    {
      myData = myAlloc.reallocate(myData, aNewCap);
    }
    else
    {
      TheItemType* aNewData = myAlloc.allocate(aNewCap);
      for (size_t i = 0; i < mySize; ++i)
      {
        myAlloc.construct(aNewData + i, std::move(myData[i]));
        myData[i].~TheItemType();
      }
      if (myData != nullptr)
      {
        myAlloc.deallocate(myData, myCapacity);
      }
      myData = aNewData;
    }
    myCapacity = aNewCap;
  }

  //! Destroy elements in range [theFrom, theTo).
  void destroyRange(const size_t theFrom, const size_t theTo)
  {
    if constexpr (!std::is_trivially_destructible_v<TheItemType>)
    {
      for (size_t i = theFrom; i < theTo; ++i)
      {
        myData[i].~TheItemType();
      }
    }
  }

  //! Shift elements right starting at theIndex by theCount positions.
  //! Caller must ensure capacity is sufficient. Does NOT construct at theIndex.
  void shiftRight(const size_t theIndex, const size_t theCount)
  {
    if constexpr (std::is_trivially_copyable_v<TheItemType>)
    {
      std::memmove(myData + theIndex + theCount,
                   myData + theIndex,
                   (mySize - theIndex) * sizeof(TheItemType));
    }
    else
    {
      // Move-construct last element into uninitialized space
      for (size_t i = mySize; i-- > theIndex;)
      {
        myAlloc.construct(myData + i + theCount, std::move(myData[i]));
        myData[i].~TheItemType();
      }
    }
  }

  //! Shift live elements [theSrcFrom, theSrcTo) left so they start at theDstFrom.
  //! Requires theDstFrom < theSrcFrom and assumes both source and destination
  //! slots currently hold live, constructed objects (move-assignment is used).
  //! The caller is responsible for destroying the vacated tail - see Erase().
  void shiftLeft(const size_t theDstFrom, const size_t theSrcFrom, const size_t theSrcTo)
  {
    const size_t aCount = theSrcTo - theSrcFrom;
    if constexpr (std::is_trivially_copyable_v<TheItemType>)
    {
      std::memmove(myData + theDstFrom, myData + theSrcFrom, aCount * sizeof(TheItemType));
    }
    else
    {
      for (size_t i = 0; i < aCount; ++i)
      {
        myData[theDstFrom + i] = std::move(myData[theSrcFrom + i]);
      }
    }
  }

private:
  allocator_type myAlloc;
  TheItemType*   myData     = nullptr;
  size_t         mySize     = 0;
  size_t         myCapacity = 0;
};

#endif // NCollection_LinearVector_HeaderFile

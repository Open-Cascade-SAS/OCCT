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

#ifndef NCollection_Array1_HeaderFile
#define NCollection_Array1_HeaderFile

#include <Standard.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Assert.hxx>

#include <NCollection_DefineAlloc.hxx>
#include <NCollection_Iterator.hxx>
#include <NCollection_Allocator.hxx>
#include <StdFail_NotDone.hxx>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

//! The class NCollection_Array1 represents unidimensional arrays of fixed size known at run time.
//! The range of the index is user defined.
//! An array1 can be constructed with a "C array".
//! This functionality is useful to call methods expecting an Array1.
//! It allows to carry the bounds inside the arrays.
//!
//! Examples:
//! @code
//! Item tab[100]; // an example with a C array
//! NCollection_Array1<Item> ttab (tab[0], 1, 100);
//!
//! NCollection_Array1<Item> tttab (ttab(10), 10, 20); // a slice of ttab
//! @endcode
//! If you want to reindex an array from 1 to Length do:
//! @code
//! NCollection_Array1<Item> tab1 (tab (tab.Lower()), 1, tab.Length());
//! @endcode
//! Warning: Programs client of such a class must be independent of the range of the first element.
//! Then, a C++ for loop must be written like this
//! @code
//! for (i = A.Lower(); i <= A.Upper(); i++)
//! @endcode
//!
//! Zero-based (size_t) construction mode:
//! Use NCollection_Array1(size_t theSize) or NCollection_Array1(pointer, size_t) to create
//! a zero-based array (Lower()==0). In this mode At()/ChangeAt() and STL iterators are the
//! preferred access path - they address elements directly without any offset subtraction.
//! Buffer-reuse variants do NOT own the memory and will not free it on destruction.
//! @code
//! int aBuffer[100];
//! NCollection_Array1<int> aZero(100);      // allocates, lower=0
//! NCollection_Array1<int> aWrap(aBuffer, 100); // wraps aBuffer, lower=0, not owner
//! for (size_t i = 0; i < aWrap.Size(); ++i)
//!   aWrap.At(i) = static_cast<int>(i);
//! @endcode
template <class TheItemType>
class NCollection_Array1
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;

public:
  typedef NCollection_Allocator<TheItemType> allocator_type;

public:
  // Define various type aliases for convenience
  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;

  using iterator       = pointer;
  using const_iterator = const_pointer;
  using Iterator       = NCollection_Iterator<NCollection_Array1<TheItemType>, false>;
  using ConstIterator  = NCollection_Iterator<NCollection_Array1<TheItemType>, true>;

public:
  //! Return the flat storage pointer.
  pointer Data() noexcept { return data(); }

  //! Return the flat storage pointer.
  const_pointer Data() const noexcept { return data(); }

  const_iterator begin() const noexcept { return data(); }

  iterator begin() noexcept { return data(); }

  const_iterator cbegin() const noexcept { return data(); }

  iterator end() noexcept { return mySize == 0 ? data() : data() + mySize; }

  const_iterator end() const noexcept { return mySize == 0 ? data() : data() + mySize; }

  const_iterator cend() const noexcept { return mySize == 0 ? data() : data() + mySize; }

public:
  //! Return the number of elements in the array.
  size_t Size() const noexcept { return mySize; }

  //! Return the number of elements as the legacy integer length.
  int Length() const noexcept { return static_cast<int>(mySize); }

  //! Return TRUE when the array is empty.
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Return the inclusive lower bound.
  int Lower() const noexcept { return myLowerBound; }

  //! Return the inclusive upper bound.
  int Upper() const noexcept
  {
    return myLowerBound + static_cast<int>(mySize) - 1;
  }

  //! Return TRUE when this array owns its storage.
  bool IsDeletable() const noexcept { return myIsOwner; }

  //! Compute the number of elements in an inclusive integer range.
  static size_t RangeSize(const int theLower, const int theUpper)
  {
    return static_cast<size_t>(theUpper - theLower + 1);
  }

public:
  // Constructors
  NCollection_Array1() noexcept = default;

  explicit NCollection_Array1(const int theLower, const int theUpper)
      : myLowerBound(theLower),
        mySize(RangeSize(theLower, theUpper)),
        myPointer(nullptr),
        myIsOwner(false)
  {
    allocateAndConstruct();
  }

  explicit NCollection_Array1(const_reference theBegin,
                              const int       theLower,
                              const int       theUpper,
                              const bool      theUseBuffer = true)
      : myLowerBound(theLower),
        mySize(RangeSize(theLower, theUpper)),
        myPointer(nullptr),
        myIsOwner(false)
  {
    if (theUseBuffer)
    {
      setStorage(const_cast<pointer>(&theBegin), mySize, myLowerBound, false);
      return;
    }
    allocateAndConstruct();
  }

  //! Zero-based constructor from first element reference.
  //! When theUseBuffer is true, wraps contiguous storage starting at theBegin.
  //! Otherwise allocates own storage of theSize elements.
  explicit NCollection_Array1(const_reference theBegin,
                              const size_t    theSize,
                              const bool      theUseBuffer)
      : myLowerBound(0),
        mySize(theSize),
        myPointer(nullptr),
        myIsOwner(false)
  {
    if (theUseBuffer)
    {
      setStorage(const_cast<pointer>(&theBegin), mySize, myLowerBound, false);
      return;
    }
    allocateAndConstruct();
  }

  //! Zero-based constructor: allocates theSize elements with lower bound 0.
  //! Use At()/ChangeAt() or STL iterators for optimal access (no offset subtraction).
  explicit NCollection_Array1(const size_t theSize)
      : myLowerBound(0),
        mySize(theSize),
        myPointer(nullptr),
        myIsOwner(false)
  {
    allocateAndConstruct();
  }

  //! Zero-based buffer-reuse constructor: wraps an existing C array of theSize elements.
  //! The array does NOT own the buffer and will NOT free it on destruction.
  //! Use At()/ChangeAt() or STL iterators for optimal access (no offset subtraction).
  explicit NCollection_Array1(pointer theBegin, const size_t theSize)
      : myLowerBound(0),
        mySize(theSize),
        myPointer(theBegin),
        myIsOwner(false)
  {
  }

  //! Copy constructor
  NCollection_Array1(const NCollection_Array1& theOther)
      : myLowerBound(theOther.myLowerBound),
        mySize(theOther.mySize),
        myPointer(nullptr),
        myIsOwner(false)
  {
    pointer aPointer = allocateRaw();
    if (aPointer == nullptr)
    {
      return;
    }
    copyConstruct(static_cast<const_pointer>(theOther.myPointer), mySize);
  }

  //! Move constructor
  NCollection_Array1(NCollection_Array1&& theOther) noexcept
      : myLowerBound(theOther.myLowerBound),
        mySize(theOther.mySize),
        myPointer(theOther.myPointer),
        myIsOwner(theOther.myIsOwner)
  {
    theOther.myLowerBound = 1;
    theOther.mySize       = 0;
    theOther.myPointer    = nullptr;
    theOther.myIsOwner    = false;
  }

  virtual ~NCollection_Array1()
  {
    if (myIsOwner)
    {
      destroy(data(), 0, mySize);
    }
    releaseStorage();
  }

  //! Initialise the items with theValue
  void Init(const_reference theValue)
  {
    for (size_t anIter = 0; anIter < mySize; anIter++)
    {
      data()[anIter] = theValue;
    }
  }

  //! Replaces this array by a copy of theOther array.
  //! Bounds and length are copied from theOther.
  //! When this array wraps an external (non-owned) buffer:
  //!  - if theOther has the same length, values are copied in place into the
  //!    external buffer and ownership is unchanged;
  //!  - if theOther has a different length, this array detaches from the
  //!    external buffer and allocates a fresh owned buffer.
  //! Use CopyValues() to preserve this array's bounds.
  NCollection_Array1& Assign(const NCollection_Array1& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    assign(static_cast<const_pointer>(theOther.myPointer), theOther.mySize, theOther.myLowerBound);
    return *this;
  }

  //! Copies values from theOther array without changing this array bounds.
  //! This array should be pre-allocated and have the same length as theOther;
  //! otherwise exception Standard_DimensionMismatch is thrown.
  NCollection_Array1& CopyValues(const NCollection_Array1& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    Standard_DimensionMismatch_Raise_if(mySize != theOther.mySize,
                                        "NCollection_Array1::CopyValues");
    const size_t aCommonSize = (std::min)(mySize, theOther.mySize);
    copyAssign(data(), static_cast<const_pointer>(theOther.myPointer), aCommonSize);
    return *this;
  }

  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will keep pointer to the memory buffer and
  //! range, but it will not free the buffer on destruction.
  NCollection_Array1& Move(NCollection_Array1&& theOther) noexcept
  {
    if (&theOther == this)
    {
      return *this;
    }
    if (myIsOwner)
    {
      destroy(data(), 0, mySize);
    }
    releaseStorage();
    myLowerBound = theOther.myLowerBound;
    mySize       = theOther.mySize;
    myPointer    = theOther.myPointer;
    myIsOwner    = theOther.myIsOwner;

    theOther.myLowerBound = 1;
    theOther.mySize       = 0;
    theOther.myPointer    = nullptr;
    theOther.myIsOwner    = false;
    return *this;
  }

  NCollection_Array1& Move(NCollection_Array1& theOther) noexcept
  {
    return Move(std::move(theOther));
  }

  //! Assignment operator; @sa Assign()
  NCollection_Array1& operator=(const NCollection_Array1& theOther) { return Assign(theOther); }

  //! Move assignment operator; @sa Move()
  NCollection_Array1& operator=(NCollection_Array1&& theOther) noexcept
  {
    return Move(std::forward<NCollection_Array1>(theOther));
  }

  //! @return first element
  const_reference First() const noexcept { return data()[0]; }

  //! @return first element
  reference ChangeFirst() noexcept { return data()[0]; }

  //! @return last element
  const_reference Last() const noexcept { return data()[mySize - 1]; }

  //! @return last element
  reference ChangeLast() noexcept { return data()[mySize - 1]; }

  //! Constant value access
  const_reference Value(const int theIndex) const
  {
    return data()[offsetOf(theIndex, "NCollection_Array1::Value")];
  }

  //! operator() - alias to Value
  const_reference operator()(const int theIndex) const { return Value(theIndex); }

  //! operator[] - alias to Value
  const_reference operator[](const int theIndex) const { return Value(theIndex); }

  //! Variable value access
  reference ChangeValue(const int theIndex)
  {
    return data()[offsetOf(theIndex, "NCollection_Array1::ChangeValue")];
  }

  //! operator() - alias to ChangeValue
  reference operator()(const int theIndex) { return ChangeValue(theIndex); }

  //! operator[] - alias to ChangeValue
  reference operator[](const int theIndex) { return ChangeValue(theIndex); }

  //! 0-based checked access independent of Lower()/Upper().
  //! @param[in] theIndex 0-based index in [0, Size()-1]
  const_reference At(const size_t theIndex) const { return at(theIndex); }

  //! 0-based checked mutable access independent of Lower()/Upper().
  //! @param[in] theIndex 0-based index in [0, Size()-1]
  reference ChangeAt(const size_t theIndex) { return at(theIndex); }

  //! Set value
  void SetValue(const int theIndex, const value_type& theItem)
  {
    data()[offsetOf(theIndex, "NCollection_Array1::SetValue")] = theItem;
  }

  //! Set value
  void SetValue(const int theIndex, value_type&& theItem)
  {
    data()[offsetOf(theIndex, "NCollection_Array1::SetValue")] =
      std::forward<value_type>(theItem);
  }

  //! Emplace value at the specified index, constructing it in-place
  //! @param theIndex index at which to emplace the value
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceValue(const int theIndex, Args&&... theArgs)
  {
    const size_t aPos = offsetOf(theIndex, "NCollection_Array1::EmplaceValue");
    data()[aPos] = value_type(std::forward<Args>(theArgs)...);
    return data()[aPos];
  }

  //! Changes the lowest bound. Do not move data
  void UpdateLowerBound(const int theLower) noexcept { myLowerBound = theLower; }

  //! Changes the upper bound. Do not move data
  void UpdateUpperBound(const int theUpper) noexcept
  {
    myLowerBound = myLowerBound - Upper() + theUpper;
  }

  //! Resizes the array to specified bounds.
  //! No re-allocation will be done if length of array does not change,
  //! but existing values will not be discarded if theToCopyData set to FALSE.
  //! @param theLower new lower bound of array
  //! @param theUpper new upper bound of array
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(const int theLower, const int theUpper, const bool theToCopyData)
  {
    resizeImpl(RangeSize(theLower, theUpper), theLower, theToCopyData);
  }

  //! Resizes the array to theSize elements, keeping the lower bound unchanged.
  //! @param theSize new number of elements
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(const size_t theSize, const bool theToCopyData)
  {
    resizeImpl(theSize, myLowerBound, theToCopyData);
  }

protected:
  //! Check an integer index and return its zero-based offset.
  size_t offsetOf(const int theIndex, const char* theMessage) const
  {
    const std::ptrdiff_t anOffset = static_cast<std::ptrdiff_t>(theIndex)
                                    - static_cast<std::ptrdiff_t>(myLowerBound);
    Standard_OutOfRange_Raise_if(anOffset < 0 || static_cast<size_t>(anOffset) >= mySize,
                                 theMessage);
    return static_cast<size_t>(anOffset);
  }

  //! Allocate raw storage for the array.
  static void* allocate(const size_t theSize, const size_t theItemSize)
  {
    return Standard::AllocateOptimal(theSize * theItemSize);
  }

  //! Free raw storage.
  static void deallocate(void* thePointer) noexcept { Standard::Free(thePointer); }

  //! Replace raw storage state.
  void setStorage(void*       thePointer,
                  const size_t theSize,
                  const int   theLowerBound,
                  const bool  theIsOwner) noexcept
  {
    myPointer    = thePointer;
    mySize       = theSize;
    myLowerBound = theLowerBound;
    myIsOwner    = theIsOwner;
  }

  //! Release owned storage and reset the array state.
  void releaseStorage() noexcept
  {
    if (myIsOwner && myPointer != nullptr)
    {
      deallocate(myPointer);
    }
    myLowerBound = 1;
    mySize       = 0;
    myPointer    = nullptr;
    myIsOwner    = false;
  }

  //! Core resize implementation used by all public Resize() overloads.
  //! @param theNewSize new number of elements
  //! @param theNewLower new lower bound value to store
  //! @param theToCopyData whether to preserve existing elements
  void resizeImpl(const size_t theNewSize, const int theNewLower, const bool theToCopyData)
  {
    if (theNewSize == mySize)
    {
      myLowerBound = theNewLower;
      return;
    }
    const pointer aPrevPtr  = data();
    const size_t  aPrevSize = mySize;

    if constexpr (std::is_trivially_copyable_v<TheItemType>)
    {
      if (myIsOwner && theToCopyData)
      {
        if (theNewSize == 0)
        {
          releaseStorage();
          setStorage(nullptr, 0, theNewLower, false);
          return;
        }
        myPointer = Standard::Reallocate(myPointer, theNewSize * sizeof(TheItemType));
        mySize       = theNewSize;
        myLowerBound = theNewLower;
        myIsOwner    = true;
        construct(aPrevSize, theNewSize);
        return;
      }
    }

    pointer       aNewPtr   = nullptr;
    if (theNewSize != 0)
    {
      aNewPtr = static_cast<pointer>(allocate(theNewSize, sizeof(TheItemType)));
      if (theToCopyData)
      {
        const size_t aMinSize = (std::min)(theNewSize, aPrevSize);
        copyConstruct(aNewPtr, aPrevPtr, aMinSize);
        construct(aMinSize, theNewSize, aNewPtr);
      }
      else
      {
        construct(0, theNewSize, aNewPtr);
      }
    }

    if (myIsOwner)
    {
      destroy(aPrevPtr, 0, aPrevSize);
    }
    releaseStorage();
    setStorage(aNewPtr, theNewSize, theNewLower, aNewPtr != nullptr);
  }

protected:
  const_reference at(const size_t theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_Array1::at");
    return data()[theIndex];
  }

  reference at(const size_t theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_Array1::at");
    return data()[theIndex];
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_trivially_default_constructible<U>::value, void>::type construct(
    const size_t,
    const size_t)
  {
    // Do nothing for the existing storage.
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_trivially_default_constructible<U>::value, void>::type construct(
    const size_t theFrom,
    const size_t theTo)
  {
    construct(theFrom, theTo, data());
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_trivially_default_constructible<U>::value, void>::type construct(
    const size_t,
    const size_t,
    pointer)
  {
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_trivially_default_constructible<U>::value, void>::type construct(
    const size_t theFrom,
    const size_t theTo,
    pointer       thePointer)
  {
    for (size_t anInd = theFrom; anInd < theTo; anInd++)
    {
      ::new (static_cast<void*>(thePointer + anInd)) TheItemType();
    }
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_trivially_destructible<U>::value, void>::type destroy(
    pointer,
    const size_t,
    const size_t)
  {
    // Do nothing
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_trivially_destructible<U>::value, void>::type destroy(
    pointer      theWhat,
    const size_t theFrom,
    const size_t theTo)
  {
    for (size_t anInd = theFrom; anInd < theTo; anInd++)
    {
      (theWhat + anInd)->~TheItemType();
    }
  }

  void assign(const const_pointer theFrom, const size_t theSize, const int theLower)
  {
    if (theSize == mySize)
    {
      copyAssign(data(), theFrom, theSize);
      myLowerBound = theLower;
      return;
    }
    pointer aNewPointer = nullptr;
    if (theSize != 0)
    {
      aNewPointer = static_cast<pointer>(allocate(theSize, sizeof(TheItemType)));
      copyConstruct(aNewPointer, theFrom, theSize);
    }

    if (myIsOwner)
    {
      destroy(data(), 0, mySize);
    }
    releaseStorage();
    setStorage(aNewPointer, theSize, theLower, aNewPointer != nullptr);
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type copyAssign(
    pointer       theTarget,
    const_pointer theFrom,
    const size_t  theCount)
  {
    if (theCount != 0)
    {
      std::memmove(theTarget, theFrom, theCount * sizeof(TheItemType));
    }
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_trivially_copyable<U>::value, void>::type copyAssign(
    pointer       theTarget,
    const_pointer theFrom,
    const size_t  theCount)
  {
    for (size_t anInd = 0; anInd < theCount; anInd++)
    {
      theTarget[anInd] = theFrom[anInd];
    }
  }

  void copyConstruct(const_pointer theFrom, const size_t theCount)
  {
    copyConstruct(data(), theFrom, theCount);
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type copyConstruct(
    pointer       theTarget,
    const_pointer theFrom,
    const size_t  theCount)
  {
    std::uninitialized_copy_n(theFrom, theCount, theTarget);
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_trivially_copyable<U>::value, void>::type copyConstruct(
    pointer       theTarget,
    const_pointer theFrom,
    const size_t  theCount)
  {
    for (size_t anInd = 0; anInd < theCount; anInd++)
    {
      ::new (static_cast<void*>(theTarget + anInd)) TheItemType(theFrom[anInd]);
    }
  }

private:
  pointer data() noexcept { return static_cast<pointer>(myPointer); }
  const_pointer data() const noexcept { return static_cast<const_pointer>(myPointer); }

  pointer allocateRaw()
  {
    if (mySize == 0)
    {
      return nullptr;
    }
    pointer aPointer = static_cast<pointer>(allocate(mySize, sizeof(TheItemType)));
    myPointer        = aPointer;
    myIsOwner        = true;
    return aPointer;
  }

  void allocateAndConstruct()
  {
    pointer aPointer = allocateRaw();
    if (aPointer == nullptr)
    {
      return;
    }
    construct(0, mySize);
  }

private:
  int     myLowerBound = 1;
  size_t  mySize       = 0;
  void*   myPointer    = nullptr;
  bool    myIsOwner    = false;
};

#endif

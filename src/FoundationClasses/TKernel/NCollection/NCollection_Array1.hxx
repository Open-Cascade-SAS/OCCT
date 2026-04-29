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

#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_DefineAlloc.hxx>
#include <NCollection_Iterator.hxx>
#include <NCollection_Allocator.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_IndexedIterator.hxx>

#include <algorithm>

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
  using difference_type = size_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;

  using iterator       = NCollection_IndexedIterator<std::random_access_iterator_tag,
                                                     NCollection_Array1,
                                                     value_type,
                                                     false>;
  using const_iterator = NCollection_IndexedIterator<std::random_access_iterator_tag,
                                                     NCollection_Array1,
                                                     value_type,
                                                     true>;
  using Iterator       = NCollection_Iterator<NCollection_Array1<TheItemType>>;

public:
  const_iterator begin() const noexcept { return const_iterator(*this); }

  iterator begin() noexcept { return iterator(*this); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(mySize, *this); }

  const_iterator end() const noexcept { return const_iterator(mySize, *this); }

  const_iterator cend() const noexcept { return const_iterator(mySize, *this); }

public:
  // Constructors
  NCollection_Array1() noexcept
      : myLowerBound(1),
        mySize(0)
  {
  }

  explicit NCollection_Array1(const int theLower, const int theUpper)
      : myLowerBound(theLower),
        mySize(theUpper - theLower + 1)
  {
    if (mySize == 0)
    {
      return;
    }
    myPointer = myAllocator.allocate(mySize);
    myIsOwner = true;
    construct(0, mySize);
  }

  explicit NCollection_Array1(const_reference theBegin,
                              const int       theLower,
                              const int       theUpper,
                              const bool      theUseBuffer = true)
      : myLowerBound(theLower),
        mySize(theUpper - theLower + 1),
        myPointer(theUseBuffer ? const_cast<pointer>(&theBegin) : nullptr),
        myIsOwner(!theUseBuffer)
  {
    if (!myIsOwner)
    {
      return;
    }
    myPointer = myAllocator.allocate(mySize);
    myIsOwner = true;
    construct(0, mySize);
  }

  //! Zero-based constructor: allocates theSize elements with lower bound 0.
  //! Use At()/ChangeAt() or STL iterators for optimal access (no offset subtraction).
  explicit NCollection_Array1(const size_t theSize)
      : myLowerBound(0),
        mySize(theSize)
  {
    if (mySize == 0)
    {
      return;
    }
    myPointer = myAllocator.allocate(mySize);
    myIsOwner = true;
    construct(0, mySize);
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
        mySize(theOther.mySize)
  {
    if (mySize == 0)
    {
      return;
    }
    myPointer = myAllocator.allocate(mySize);
    myIsOwner = true;
    copyConstruct(theOther.myPointer, mySize);
  }

  //! Move constructor
  NCollection_Array1(NCollection_Array1&& theOther) noexcept
      : myLowerBound(theOther.myLowerBound),
        mySize(theOther.mySize),
        myPointer(theOther.myPointer),
        myIsOwner(theOther.myIsOwner)
  {
    theOther.myIsOwner    = false;
    theOther.myPointer    = nullptr;
    theOther.mySize       = 0;
    theOther.myLowerBound = 1;
  }

  virtual ~NCollection_Array1()
  {
    if (!myIsOwner)
    {
      return;
    }
    destroy(myPointer, 0, mySize);
    myAllocator.deallocate(myPointer, mySize);
  }

  //! Initialise the items with theValue
  void Init(const_reference theValue)
  {
    for (size_t anIter = 0; anIter < mySize; anIter++)
    {
      myPointer[anIter] = theValue;
    }
  }

  //! Size query.
  size_t Size() const noexcept { return mySize; }

  //! Length query (legacy int-returning API).
  int Length() const noexcept { return static_cast<int>(mySize); }

  //! Return TRUE if array has zero length.
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Lower bound
  int Lower() const noexcept { return myLowerBound; }

  //! Upper bound
  int Upper() const noexcept { return myLowerBound + static_cast<int>(mySize) - 1; }

  //! Copies data of theOther array to this.
  //! This array should be pre-allocated and have the same length as theOther;
  //! otherwise exception Standard_DimensionMismatch is thrown.
  NCollection_Array1& Assign(const NCollection_Array1& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    Standard_DimensionMismatch_Raise_if(mySize != theOther.mySize, "NCollection_Array1::operator=");
    for (size_t anInd = 0; anInd < mySize; anInd++)
    {
      myPointer[anInd] = theOther.myPointer[anInd];
    }
    // Current implementation disable changing bounds by assigning
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
      destroy(myPointer, 0, mySize);
      myAllocator.deallocate(myPointer, mySize);
    }
    myLowerBound          = theOther.myLowerBound;
    mySize                = theOther.mySize;
    myPointer             = theOther.myPointer;
    myIsOwner             = theOther.myIsOwner;
    theOther.myIsOwner    = false;
    theOther.myPointer    = nullptr;
    theOther.mySize       = 0;
    theOther.myLowerBound = 1;
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
  const_reference First() const noexcept { return myPointer[0]; }

  //! @return first element
  reference ChangeFirst() noexcept { return myPointer[0]; }

  //! @return last element
  const_reference Last() const noexcept { return myPointer[mySize - 1]; }

  //! @return last element
  reference ChangeLast() noexcept { return myPointer[mySize - 1]; }

  //! Constant value access
  const_reference Value(const int theIndex) const
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::Value");
    return myPointer[aPos];
  }

  //! operator() - alias to Value
  const_reference operator()(const int theIndex) const { return Value(theIndex); }

  //! operator[] - alias to Value
  const_reference operator[](const int theIndex) const { return Value(theIndex); }

  //! Variable value access
  reference ChangeValue(const int theIndex)
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::ChangeValue");
    return myPointer[aPos];
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
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::SetValue");
    myPointer[aPos] = theItem;
  }

  //! Set value
  void SetValue(const int theIndex, value_type&& theItem)
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::SetValue");
    myPointer[aPos] = std::forward<value_type>(theItem);
  }

  //! Emplace value at the specified index, constructing it in-place
  //! @param theIndex index at which to emplace the value
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceValue(const int theIndex, Args&&... theArgs)
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::EmplaceValue");
    myPointer[aPos] = value_type(std::forward<Args>(theArgs)...);
    return myPointer[aPos];
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
    Standard_RangeError_Raise_if(theUpper < theLower, "NCollection_Array1::Resize");
    resizeImpl(static_cast<size_t>(theUpper - theLower + 1), theLower, theToCopyData);
  }

  //! Resizes the array to theSize elements, keeping the lower bound unchanged.
  //! @param theSize new number of elements
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(const size_t theSize, const bool theToCopyData)
  {
    resizeImpl(theSize, myLowerBound, theToCopyData);
  }

  bool IsDeletable() const noexcept { return myIsOwner; }

  friend iterator;
  friend const_iterator;

protected:
  //! Core resize implementation used by all public Resize() overloads.
  //! @param theNewSize new number of elements
  //! @param theNewLower new lower bound value to store
  //! @param theToCopyData whether to preserve existing elements
  void resizeImpl(const size_t theNewSize, const int theNewLower, const bool theToCopyData)
  {
    const pointer aPrevPtr = myPointer;
    if (theNewSize == mySize)
    {
      myLowerBound = theNewLower;
      return;
    }
    if (myIsOwner)
    {
      if (theToCopyData)
        destroy(myPointer, theNewSize, mySize);
      else
        destroy(myPointer, 0, mySize);
    }
    myLowerBound = theNewLower;
    if (theToCopyData)
    {
      const size_t aMinSize = (std::min)(theNewSize, mySize);
      if (myIsOwner)
      {
        myPointer = myAllocator.reallocate(myPointer, theNewSize);
      }
      else
      {
        myPointer = myAllocator.allocate(theNewSize);
        copyConstruct(aPrevPtr, aMinSize);
      }
      construct(mySize < theNewSize ? mySize : theNewSize, theNewSize);
    }
    else
    {
      if (myIsOwner)
        myAllocator.deallocate(aPrevPtr, mySize);
      myPointer = myAllocator.allocate(theNewSize);
      construct(0, theNewSize);
    }
    mySize    = theNewSize;
    myIsOwner = true;
  }

protected:
  const_reference at(const size_t theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_Array1::at");
    return myPointer[theIndex];
  }

  reference at(const size_t theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize, "NCollection_Array1::at");
    return myPointer[theIndex];
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_trivially_default_constructible<U>::value, void>::type construct(
    const size_t,
    const size_t)
  {
    // Do nothing
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_trivially_default_constructible<U>::value, void>::type construct(
    const size_t theFrom,
    const size_t theTo)
  {
    for (size_t anInd = theFrom; anInd < theTo; anInd++)
    {
      myAllocator.construct(myPointer + anInd);
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
      myAllocator.destroy(theWhat + anInd);
    }
  }

  void copyConstruct(const pointer theFrom, const size_t theCount)
  {
    for (size_t anInd = 0; anInd < theCount; anInd++)
    {
      myAllocator.construct(myPointer + anInd, theFrom[anInd]);
    }
  }

  // ---------- PROTECTED FIELDS -----------
  int            myLowerBound;
  size_t         mySize;
  pointer        myPointer = nullptr;
  bool           myIsOwner = false;
  allocator_type myAllocator;
};

#endif

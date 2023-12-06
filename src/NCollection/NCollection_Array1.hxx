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
#include <array>
#include <vector>

//! The class NCollection_Array1 represents unidimensional arrays of fixed size known at run time.
//! The range of the index is user defined.
//! An array1 can be constructed with a "C array".
//! This functionality is useful to call methods expecting an Array1.
//! It allows to carry the bounds inside the arrays.
//!
//! Examples:
//! @code
//! Item tab[100]; //  an example with a C array
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
  using value_type = TheItemType;
  using size_type = size_t;
  using difference_type = size_t;
  using pointer = TheItemType*;
  using const_pointer = const TheItemType*;
  using reference = TheItemType&;
  using const_reference = const TheItemType&;

  using iterator = NCollection_IndexedIterator<std::random_access_iterator_tag, NCollection_Array1, value_type, false>;
  using const_iterator = NCollection_IndexedIterator<std::random_access_iterator_tag, NCollection_Array1, value_type, true>;
  using Iterator = NCollection_Iterator<NCollection_Array1<TheItemType>>;

public:

  const_iterator begin() const
  {
    return const_iterator(*this);
  }

  iterator begin()
  {
    return iterator(*this);
  }

  const_iterator cbegin() const
  {
    return const_iterator(*this);
  }

  iterator end()
  {
    return iterator(mySize, *this);
  }

  const_iterator end() const
  {
    return const_iterator(mySize, *this);
  }

  const_iterator cend() const
  {
    return const_iterator(mySize, *this);
  }

public:
  // Constructors
  NCollection_Array1() :
    myLowerBound(1),
    mySize(0)
  {}

  explicit NCollection_Array1(const Standard_Integer theLower,
                              const Standard_Integer theUpper) :
    myLowerBound(theLower),
    mySize(theUpper - theLower + 1)
  {
    if (mySize == 0)
    {
      return;
    }
    myPointer = myAllocator.allocate(mySize);
    myIsOwner = true;
    construct();
  }

  explicit NCollection_Array1(const allocator_type& theAlloc,
                              const Standard_Integer theLower,
                              const Standard_Integer theUpper) :
    myLowerBound(theLower),
    mySize(theUpper - theLower + 1),
    myPointer(nullptr),
    myIsOwner(false),
    myAllocator(theAlloc)
  {
    if (mySize == 0)
    {
      return;
    }
    myPointer = myAllocator.allocate(mySize);
    myIsOwner = true;
    construct();
  }

  explicit NCollection_Array1(const_reference theBegin,
                              const Standard_Integer theLower,
                              const Standard_Integer theUpper,
                              const bool theUseBuffer = true) :
    myLowerBound(theLower),
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
    construct();
  }

  //! Copy constructor 
  NCollection_Array1(const NCollection_Array1& theOther) :
    myLowerBound(theOther.myLowerBound),
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
  NCollection_Array1(NCollection_Array1&& theOther) noexcept :
    myLowerBound(theOther.myLowerBound),
    mySize(theOther.mySize),
    myPointer(theOther.myPointer),
    myIsOwner(theOther.myIsOwner)
  {
    theOther.myIsOwner = false;
  }

  virtual ~NCollection_Array1()
  {
    if (!myIsOwner)
    {
      return;
    }
    destroy();
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

  //! Size query
  Standard_Integer Size() const
  {
    return Length();
  }

  //! Length query (the same)
  Standard_Integer Length() const
  {
    return static_cast<Standard_Integer>(mySize);
  }

  //! Return TRUE if array has zero length.
  Standard_Boolean IsEmpty() const
  {
    return mySize == 0;
  }

  //! Lower bound
  Standard_Integer Lower() const
  {
    return myLowerBound;
  }

  //! Upper bound
  Standard_Integer Upper() const
  {
    return myLowerBound + static_cast<int>(mySize) - 1;
  }

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
      destroy();
      myAllocator.deallocate(myPointer, mySize);
    }
    myLowerBound = theOther.myLowerBound;
    mySize = theOther.mySize;
    myPointer = theOther.myPointer;
    myIsOwner = theOther.myIsOwner;
    theOther.myIsOwner = false;
    return *this;
  }

  NCollection_Array1& Move(NCollection_Array1& theOther)
  {
    return Move(std::move(theOther));
  }

  //! Assignment operator; @sa Assign()
  NCollection_Array1& operator= (const NCollection_Array1& theOther)
  {
    return Assign(theOther);
  }

  //! Move assignment operator; @sa Move()
  NCollection_Array1& operator= (NCollection_Array1&& theOther) noexcept
  {
    return Move(std::forward<NCollection_Array1>(theOther));
  }

  //! @return first element
  const_reference First() const
  {
    return myPointer[0];
  }

  //! @return first element
  reference ChangeFirst()
  {
    return myPointer[0];
  }

  //! @return last element
  const_reference Last() const
  {
    return myPointer[mySize - 1];
  }

  //! @return last element
  reference ChangeLast()
  {
    return myPointer[mySize - 1];
  }

  //! Constant value access
  const_reference Value(const Standard_Integer theIndex) const
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::Value");
    return myPointer[aPos];
  }

  //! operator() - alias to Value
  const_reference operator() (const Standard_Integer theIndex) const
  {
    return Value(theIndex);
  }

  //! operator[] - alias to Value
  const_reference operator[] (const Standard_Integer theIndex) const
  {
    return Value(theIndex);
  }

  //! Variable value access
  reference ChangeValue(const Standard_Integer theIndex)
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::ChangeValue");
    return myPointer[aPos];
  }

  //! operator() - alias to ChangeValue
  reference operator() (const Standard_Integer theIndex)
  {
    return ChangeValue(theIndex);
  }

  //! operator[] - alias to ChangeValue
  reference operator[] (const Standard_Integer theIndex)
  {
    return ChangeValue(theIndex);
  }

  //! Set value 
  void SetValue(const Standard_Integer theIndex,
                const value_type& theItem)
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::SetValue");
    myPointer[aPos] = theItem;
  }

  //! Set value 
  void SetValue(const Standard_Integer theIndex,
                value_type&& theItem)
  {
    const size_t aPos = theIndex - myLowerBound;
    Standard_OutOfRange_Raise_if(aPos >= mySize, "NCollection_Array1::SetValue");
    myPointer[aPos] = std::forward<value_type>(theItem);
  }

  //! Changes the lowest bound. Do not move data
  void UpdateLowerBound(const Standard_Integer theLower)
  {
    myLowerBound = theLower;
  }

  //! Changes the upper bound. Do not move data
  void UpdateUpperBound(const Standard_Integer theUpper)
  {
    myLowerBound = myLowerBound - Upper() + theUpper;
  }

  //! Resizes the array to specified bounds.
  //! No re-allocation will be done if length of array does not change,
  //! but existing values will not be discarded if theToCopyData set to FALSE.
  //! @param theLower new lower bound of array
  //! @param theUpper new upper bound of array
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(const Standard_Integer theLower,
              const Standard_Integer theUpper,
              const Standard_Boolean theToCopyData)
  {
    Standard_RangeError_Raise_if(theUpper < theLower, "NCollection_Array1::Resize");
    const size_t aNewSize = static_cast<size_t>(theUpper - theLower + 1);
    const size_t anOldSize = mySize;
    pointer aPrevContPnt = myPointer;
    if (aNewSize == anOldSize)
    {
      myLowerBound = theLower;
      return;
    }
    if (myIsOwner)
    {
      if (theToCopyData)
        destroy(myPointer, aNewSize, mySize);
      else
        destroy();
    }
    myLowerBound = theLower;
    mySize = aNewSize;
    if (theToCopyData)
    {
      const size_t aMinSize = std::min<size_t>(aNewSize, anOldSize);
      if (myIsOwner)
      {
        myPointer = myAllocator.reallocate(myPointer, aNewSize);
      }
      else
      {
        myPointer = myAllocator.allocate(aNewSize);
        copyConstruct(aPrevContPnt, aMinSize);
      }
      construct(anOldSize, aNewSize);
    }
    else
    {
      if (myIsOwner)
        myAllocator.deallocate(aPrevContPnt, mySize);
      myPointer = myAllocator.allocate(aNewSize);
      construct();
    }
    myIsOwner = true;
  }

  bool IsDeletable() const
  {
    return myIsOwner;
  }

  friend iterator;
  friend const_iterator;

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

protected:

  template <typename U = TheItemType>
  typename std::enable_if<std::is_arithmetic<U>::value, void>::type construct()
  {
    // Do nothing
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_arithmetic<U>::value, void>::type construct()
  {
    for (size_t anInd = 0; anInd < mySize; anInd++)
    {
      myAllocator.construct(myPointer + anInd);
    }
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_arithmetic<U>::value, void>::type construct(const size_t,
                                                                              const size_t)
  {
    // Do nothing
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_arithmetic<U>::value, void>::type construct(const size_t theFrom,
                                                                               const size_t theTo)
  {
    for (size_t anInd = theFrom; anInd < theTo; anInd++)
    {
      myAllocator.construct(myPointer + anInd);
    }
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_arithmetic<U>::value, void>::type destroy()
  {
    // Do nothing
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_arithmetic<U>::value, void>::type destroy()
  {
    for (size_t anInd = 0; anInd < mySize; anInd++)
    {
      myAllocator.destroy(myPointer + anInd);
    }
  }

  template <typename U = TheItemType>
  typename std::enable_if<std::is_arithmetic<U>::value, void>::type destroy(pointer,
                                                                            const size_t,
                                                                            const size_t)
  {
    // Do nothing
  }

  template <typename U = TheItemType>
  typename std::enable_if<!std::is_arithmetic<U>::value, void>::type destroy(pointer theWhat,
                                                                             const size_t theFrom,
                                                                             const size_t theTo)
  {
    for (size_t anInd = theFrom; anInd < theTo; anInd++)
    {
      myAllocator.destroy(theWhat + anInd);
    }
  }

  void copyConstruct(const pointer theFrom,
                     const size_t theCount)
  {
    for (size_t anInd = 0; anInd < theCount; anInd++)
    {
      myAllocator.construct(myPointer + anInd, theFrom[anInd]);
    }
  }

  // ---------- PROTECTED FIELDS -----------
  Standard_Integer myLowerBound;
  size_t mySize;
  pointer myPointer = nullptr;
  bool myIsOwner = false;
  allocator_type myAllocator;
};

#endif

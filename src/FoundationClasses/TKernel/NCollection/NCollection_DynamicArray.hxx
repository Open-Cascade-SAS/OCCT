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

#include "NCollection_Allocator.hxx"
#include "NCollection_BasePointerVector.hxx"
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_BasePointerVector.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <NCollection_Allocator.hxx>
#include <NCollection_Iterator.hxx>
#include <NCollection_OccAllocator.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_IndexedIterator.hxx>

#include <locale>
#include <vector>

//! Class NCollection_DynamicArray (dynamic array of objects)
//!
//! The array's indices always start at 0.
//!
//! The Vector is always created with 0 length. It can be enlarged by two means:
//!  1. Calling the method Append (val) - then "val" is added to the end of the
//!     vector (the vector length is incremented)
//!  2. Calling the method SetValue (i, val)  - if "i" is greater than or equal
//!     to the current length of the vector,  the vector is enlarged to accomo-
//!     date this index
//!
//! The methods Append and SetValue return  a non-const reference  to the copied
//! object  inside  the vector.  This reference  is guaranteed to be valid until
//! the vector is destroyed. It can be used to access the vector member directly
//! or to pass its address to other data structures.
//!
//! The vector iterator remembers the length of the vector  at the moment of the
//! creation or initialisation of the iterator.   Therefore the iteration begins
//! at index 0  and stops at the index equal to (remembered_length-1).  It is OK
//! to enlarge the vector during the iteration.
template <class TheItemType>
class NCollection_DynamicArray
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;

public:
  typedef NCollection_OccAllocator<TheItemType> allocator_type;
  typedef NCollection_BasePointerVector         vector;

public:
  // Define various type aliases for convenience
  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = size_t;
  using pointer         = TheItemType*;
  using const_pointer   = TheItemType&;
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
  const_iterator begin() const { return const_iterator(*this); }

  iterator begin() { return iterator(*this); }

  const_iterator cbegin() const { return const_iterator(*this); }

  iterator end() { return iterator(myUsedSize, *this); }

  const_iterator end() const { return const_iterator(myUsedSize, *this); }

  const_iterator cend() const { return const_iterator(myUsedSize, *this); }

public: //! @name public methods
  NCollection_DynamicArray(const Standard_Integer theIncrement = 256)
      : myContainer(),
        myAlloc(),
        myInternalSize(theIncrement),
        myUsedSize(0)
  {
  }

  // Constructor taking an allocator
  explicit NCollection_DynamicArray(const Standard_Integer                   theIncrement,
                                    const Handle(NCollection_BaseAllocator)& theAllocator)
      : myContainer(),
        myAlloc(allocator_type(theAllocator)),
        myInternalSize(theIncrement),
        myUsedSize(0)
  {
  }

  // Constructor taking an allocator
  explicit NCollection_DynamicArray(const Standard_Integer theIncrement,
                                    const allocator_type&  theAllocator)
      : myContainer(),
        myAlloc(theAllocator),
        myInternalSize(theIncrement),
        myUsedSize(0)
  {
  }

  //! Copy constructor
  NCollection_DynamicArray(const NCollection_DynamicArray& theOther)
      : myContainer(theOther.myContainer),
        myAlloc(theOther.myAlloc),
        myInternalSize(theOther.myInternalSize),
        myUsedSize(theOther.myUsedSize)
  {
    copyDate();
  }

  NCollection_DynamicArray(NCollection_DynamicArray&& theOther) noexcept
      : myContainer(std::move(theOther.myContainer)),
        myAlloc(theOther.myAlloc),
        myInternalSize(theOther.myInternalSize),
        myUsedSize(theOther.myUsedSize)
  {
    theOther.myUsedSize = 0;
  }

  ~NCollection_DynamicArray() { Clear(true); }

  //! Total number of items
  Standard_Integer Length() const { return static_cast<int>(myUsedSize); }

  //! Total number of items in the vector
  Standard_Integer Size() const { return Length(); }

  //! Method for consistency with other collections.
  //! @return Lower bound (inclusive) for iteration.
  Standard_Integer Lower() const { return 0; }

  //! Method for consistency with other collections.
  //! @return Upper bound (inclusive) for iteration.
  Standard_Integer Upper() const { return Length() - 1; }

  //! Empty query
  Standard_Boolean IsEmpty() const { return myUsedSize == 0; }

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

  void EraseLast()
  {
    if (myUsedSize == 0)
    {
      return;
    }
    TheItemType* aLastElem = &ChangeLast();
    myAlloc.destroy(aLastElem);
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

  //! Operator() - query the const value
  const_reference operator()(const Standard_Integer theIndex) const { return Value(theIndex); }

  //! Operator[] - query the const value
  const_reference operator[](const Standard_Integer theIndex) const { return Value(theIndex); }

  //! Operator[] - query the const value
  const_reference operator[](const size_t theIndex) const { return at(theIndex); }

  const_reference Value(const Standard_Integer theIndex) const
  {
    return at(static_cast<int>(theIndex));
  }

  //! @return first element
  const_reference First() const { return getArray()[0][0]; }

  //! @return first element
  reference ChangeFirst() { return getArray()[0][0]; }

  //! @return last element
  const_reference Last() const { return at(myUsedSize - 1); }

  //! @return last element
  reference ChangeLast() { return at(myUsedSize - 1); }

  //! Operator() - query the value
  reference operator()(const Standard_Integer theIndex) { return ChangeValue(theIndex); }

  //! Operator[] - query the value
  reference operator[](const Standard_Integer theIndex) { return ChangeValue(theIndex); }

  //! Operator[] - query the value
  reference operator[](const size_t theIndex) { return at(theIndex); }

  reference ChangeValue(const Standard_Integer theIndex) { return at(static_cast<int>(theIndex)); }

  //! SetValue () - set or append a value
  reference SetValue(const Standard_Integer theIndex, const TheItemType& theValue)
  {
    const size_t aBlockInd = static_cast<size_t>(theIndex / myInternalSize);
    const size_t anIndex   = static_cast<size_t>(theIndex);
    for (size_t aInd = myContainer.Size(); aInd <= aBlockInd; aInd++)
    {
      expandArray();
    }
    if (myUsedSize <= anIndex)
    {
      for (; myUsedSize < anIndex; myUsedSize++)
      {
        pointer aPnt = &at(myUsedSize);
        myAlloc.construct(aPnt);
      }
      myUsedSize++;
    }
    pointer aPnt = &at(anIndex);
    myAlloc.construct(aPnt, theValue);
    return *aPnt;
  }

  //! SetValue () - set or append a value
  reference SetValue(const Standard_Integer theIndex, TheItemType&& theValue)
  {
    const size_t aBlockInd = static_cast<size_t>(theIndex / myInternalSize);
    const size_t anIndex   = static_cast<size_t>(theIndex);
    for (size_t aInd = myContainer.Size(); aInd <= aBlockInd; aInd++)
    {
      expandArray();
    }
    if (myUsedSize <= anIndex)
    {
      for (; myUsedSize < anIndex; myUsedSize++)
      {
        pointer aPnt = &at(myUsedSize);
        myAlloc.construct(aPnt);
      }
      myUsedSize++;
    }
    pointer aPnt = &at(anIndex);
    myAlloc.construct(aPnt, std::forward<TheItemType>(theValue));
    return *aPnt;
  }

  void Clear(const bool theReleaseMemory = false)
  {
    size_t aUsedSize = 0;
    for (size_t aBlockInd = 0; aBlockInd < myContainer.Size(); aBlockInd++)
    {
      TheItemType* aCurStart = getArray()[aBlockInd];
      for (size_t anElemInd = 0; anElemInd < myInternalSize && aUsedSize < myUsedSize;
           anElemInd++, aUsedSize++)
      {
        aCurStart[anElemInd].~TheItemType();
      }
      if (theReleaseMemory)
        myAlloc.deallocate(aCurStart, myInternalSize);
    }
    if (theReleaseMemory)
      myContainer.Clear(theReleaseMemory);
    myUsedSize = 0;
  }

  void SetIncrement(const Standard_Integer theIncrement)
  {
    if (myUsedSize != 0)
    {
      return;
    }
    myInternalSize = static_cast<size_t>(theIncrement);
  }

  friend iterator;
  friend const_iterator;

protected:
  size_t availableSize() const { return myContainer.Size() * myInternalSize; }

  TheItemType* expandArray()
  {
    TheItemType* aNewBlock = myAlloc.allocate(myInternalSize);
    myContainer.Append(aNewBlock);
    return aNewBlock;
  }

  reference at(const size_t theInd)
  {
    return getArray()[theInd / myInternalSize][theInd % myInternalSize];
  }

  const_reference at(const size_t theInd) const
  {
    return getArray()[theInd / myInternalSize][theInd % myInternalSize];
  }

  void copyDate()
  {
    size_t aUsedSize = 0;
    for (size_t aBlockInd = 0; aBlockInd < myContainer.Size(); aBlockInd++)
    {
      TheItemType* aCurStart = getArray()[aBlockInd];
      TheItemType* aNewBlock = myAlloc.allocate(myInternalSize);
      for (size_t anElemInd = 0; anElemInd < myInternalSize && aUsedSize < myUsedSize;
           anElemInd++, aUsedSize++)
      {
        pointer aPnt = &aNewBlock[anElemInd];
        myAlloc.construct(aPnt, aCurStart[anElemInd]);
      }
      getArray()[aBlockInd] = aNewBlock;
    }
  }

  //! Wrapper to extract array
  TheItemType** getArray() const { return (TheItemType**)myContainer.GetArray(); }

protected:
  vector         myContainer;
  allocator_type myAlloc;
  size_t         myInternalSize;
  size_t         myUsedSize;
};

#endif // NCollection_DynamicArray_HeaderFile

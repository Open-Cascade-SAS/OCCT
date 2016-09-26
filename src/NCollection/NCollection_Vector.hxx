// Created on: 2002-04-23
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2013 OPEN CASCADE SAS
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

#ifndef NCollection_Vector_HeaderFile
#define NCollection_Vector_HeaderFile

#include <NCollection_BaseVector.hxx>
#include <NCollection_StlIterator.hxx>

//! Class NCollection_Vector (dynamic array of objects)
//!
//! This class is similar to NCollection_Array1  though the indices always start
//! at 0 (in Array1 the first index must be specified)
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
class NCollection_Vector : public NCollection_BaseVector
{
public:
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

public:

  //! Nested class Iterator
  class Iterator : public NCollection_BaseVector::Iterator
  {
  public:

    //! Empty constructor - for later Init
    Iterator() {}

    //! Constructor with initialisation
    Iterator (const NCollection_Vector& theVector, Standard_Boolean theToEnd = Standard_False)
    : NCollection_BaseVector::Iterator (theVector, theToEnd) {}

    //! Copy constructor
    Iterator (const Iterator& theOther)
    : NCollection_BaseVector::Iterator (theOther) {}

    //! Initialisation
    void Init (const NCollection_Vector& theVector)
    {
      initV (theVector);
    }

    //! Assignment
    Iterator& operator= (const Iterator& theOther)
    {
      copyV (theOther);
      return *this;
    }

    //! Check end
    Standard_Boolean More() const
    {
      return moreV();
    }

    //! Increment operator.
    void Next()
    {
      nextV();
    }

    //! Decrement operator.
    void Previous()
    {
      prevV();
    }

    //! Offset operator.
    void Offset (ptrdiff_t theOffset)
    {
      offsetV (static_cast<int>(theOffset));
    }

    //! Difference operator.
    ptrdiff_t Differ (const Iterator& theOther) const
    {
      return differV (theOther);
    }

    //! Constant value access
    const TheItemType& Value() const
    {
      return ((const TheItemType* )curBlockV()->DataPtr)[myCurIndex];
    }

    //! Variable value access
    TheItemType& ChangeValue() const
    {
      return ((TheItemType* )curBlockV()->DataPtr)[myCurIndex];
    }

    //! Performs comparison of two iterators.
    Standard_Boolean IsEqual (const Iterator& theOther) const
    {
      return myVector    == theOther.myVector
          && myCurIndex  == theOther.myCurIndex
          && myEndIndex  == theOther.myEndIndex  
          && myICurBlock == theOther.myICurBlock
          && myIEndBlock == theOther.myIEndBlock;
    }
  };

  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::random_access_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::random_access_iterator_tag, Iterator, TheItemType, true> const_iterator;

  //! Returns an iterator pointing to the first element in the vector.
  iterator begin() const { return Iterator (*this, false); }

  //! Returns an iterator referring to the past-the-end element in the vector.
  iterator end() const { return Iterator (*this, true); }

  //! Returns a const iterator pointing to the first element in the vector.
  const_iterator cbegin() const { return Iterator (*this, false); }

  //! Returns a const iterator referring to the past-the-end element in the vector.
  const_iterator cend() const { return Iterator (*this, true); }

public: //! @name public methods

  //! Constructor
  explicit NCollection_Vector (const Standard_Integer theIncrement              = 256,
                               const Handle(NCollection_BaseAllocator)& theAlloc = NULL) :
    NCollection_BaseVector (theAlloc, initMemBlocks, sizeof(TheItemType), theIncrement)
  {}

  //! Copy constructor
  NCollection_Vector (const NCollection_Vector& theOther) :
    NCollection_BaseVector (theOther.myAllocator, initMemBlocks, theOther)
  {
    copyData (theOther);
  }

  //! Destructor
  virtual ~NCollection_Vector()
  {
    for (Standard_Integer anItemIter = 0; anItemIter < myCapacity; ++anItemIter)
    {
      initMemBlocks (*this, myData[anItemIter], 0, 0);
    }
    this->myAllocator->Free (myData);
  }

  //! Total number of items
  Standard_Integer Length() const
  {
    return myLength;
  }

  //! Total number of items in the vector
  Standard_Integer Size() const
  {
    return myLength;
  }

  //! Method for consistency with other collections.
  //! @return Lower bound (inclusive) for iteration.
  Standard_Integer Lower() const
  {
    return 0;
  }

  //! Method for consistency with other collections.
  //! @return Upper bound (inclusive) for iteration.
  Standard_Integer Upper() const
  {
    return myLength - 1;
  }

  //! Empty query
  Standard_Boolean IsEmpty() const
  {
    return (myLength == 0);
  }

  //! Assignment to the collection of the same type
  inline void Assign (const NCollection_Vector& theOther,
                      const Standard_Boolean theOwnAllocator = Standard_True);

  //! Assignment operator
  NCollection_Vector& operator= (const NCollection_Vector& theOther)
  {
    Assign (theOther, Standard_False);
    return *this;
  }

  //! Append
  TheItemType& Append (const TheItemType& theValue)
  {
    TheItemType& anAppended = *(TheItemType* )expandV (myLength);
    anAppended = theValue;
    return anAppended;
  }

  //! Operator() - query the const value
  const TheItemType& operator() (const Standard_Integer theIndex) const
  {
    return Value (theIndex);
  }

  const TheItemType& Value (const Standard_Integer theIndex) const
  {
    return *(const TheItemType* )findV (theIndex);
  }

  //! @return first element
  const TheItemType& First() const
  {
    return *(const TheItemType* )findV (Lower());
  }

  //! @return first element
  TheItemType& ChangeFirst()
  {
    return *(TheItemType* )findV (Lower());
  }

  //! @return last element
  const TheItemType& Last() const
  {
    return *(const TheItemType* )findV (Upper());
  }

  //! @return last element
  TheItemType& ChangeLast()
  {
    return *(TheItemType* )findV (Upper());
  }

  //! Operator() - query the value
  TheItemType& operator() (const Standard_Integer theIndex)
  {
    return ChangeValue (theIndex);
  }

  TheItemType& ChangeValue (const Standard_Integer theIndex)
  {
    return *(TheItemType* )findV (theIndex);
  }

  //! SetValue () - set or append a value
  TheItemType& SetValue (const Standard_Integer theIndex,
                         const TheItemType&     theValue)
  {
    Standard_OutOfRange_Raise_if (theIndex < 0, "NCollection_Vector::SetValue");
    TheItemType* const aVecValue = (TheItemType* )(theIndex < myLength ? findV (theIndex) : expandV (theIndex));
    *aVecValue = theValue;
    return *aVecValue;
  }

private: //! @name private methods

  void copyData (const NCollection_Vector& theOther)
  {
    Standard_Integer iBlock = 0;
    /*NCollection_Vector::*/Iterator anIter (theOther);
    for (Standard_Integer aLength = 0; aLength < myLength; aLength += myIncrement)
    {
      MemBlock& aBlock = myData[iBlock];
      initMemBlocks (*this, aBlock, aLength, myIncrement);
      Standard_Integer anItemIter = 0;
      for (; anItemIter < myIncrement; ++anItemIter)
      {
        if (!anIter.More())
        {
          break;
        }

        ((TheItemType* )aBlock.DataPtr)[anItemIter] = anIter.Value();
        anIter.Next();
      }
      aBlock.Length = anItemIter;
      iBlock++;
    }
  }

  //! Method to initialize memory block content
  static void initMemBlocks (NCollection_BaseVector&           theVector,
                             NCollection_BaseVector::MemBlock& theBlock,
                             const Standard_Integer            theFirst,
                             const Standard_Integer            theSize)
  {
    NCollection_Vector& aSelf = static_cast<NCollection_Vector&> (theVector);
    Handle(NCollection_BaseAllocator)& anAllocator = aSelf.myAllocator;

    // release current content
    if (theBlock.DataPtr != NULL)
    {
      for (Standard_Integer anItemIter = 0; anItemIter < theBlock.Size; ++anItemIter)
      {
        ((TheItemType* )theBlock.DataPtr)[anItemIter].~TheItemType();
      }
      anAllocator->Free (theBlock.DataPtr);
      theBlock.DataPtr = NULL;
    }

    // allocate new content if requested
    if (theSize > 0)
    {
      theBlock.DataPtr = anAllocator->Allocate (theSize * sizeof(TheItemType));
      for (Standard_Integer anItemIter = 0; anItemIter < theSize; ++anItemIter)
      {
        new (&((TheItemType* )theBlock.DataPtr)[anItemIter]) TheItemType;
      }
    }
    theBlock.FirstIndex = theFirst;
    theBlock.Size       = theSize;
    theBlock.Length     = 0;
  }

  friend class Iterator;

};

//! Assignment to the collection of the same type
template <class TheItemType> inline
void NCollection_Vector<TheItemType>::Assign (const NCollection_Vector& theOther,
                                              const Standard_Boolean    theOwnAllocator)
{
  if (this == &theOther)
  {
    return;
  }

  // destroy current data using current allocator
  for (Standard_Integer anItemIter = 0; anItemIter < myCapacity; ++anItemIter)
  {
    initMemBlocks (*this, myData[anItemIter], 0, 0);
  }
  this->myAllocator->Free (myData);

  // allocate memory blocks with new allocator
  if (!theOwnAllocator)
  {
    this->myAllocator = theOther.myAllocator;
  }
  myIncrement = theOther.myIncrement;
  myLength    = theOther.myLength;
  myNBlocks   = (myLength == 0) ? 0 : (1 + (myLength - 1)/myIncrement);
  myCapacity  = GetCapacity (myIncrement) + myLength / myIncrement;
  myData      = allocMemBlocks (myCapacity);

  // copy data
  copyData (theOther);
}

#endif // NCollection_Vector_HeaderFile

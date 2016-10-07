// Created on: 2002-04-24
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

#ifndef NCollection_BaseVector_HeaderFile
#define NCollection_BaseVector_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

#include <stddef.h>

// this value defines the number of blocks that are reserved
// when the capacity of vector is increased
inline Standard_Integer GetCapacity (const Standard_Integer theIncrement)
{
  return Max(theIncrement/8, 1);
}

//! Class NCollection_BaseVector - base for NCollection_Vector template
class NCollection_BaseVector
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

protected:

  // Auxiliary structure for memory blocks
  struct MemBlock
  {

  public:

    //! @param theIndex    Item index in the block
    //! @param theItemSize Element size in bytes
    //! @return the address of specified item in this memory block
    void* findV (const Standard_Integer theIndex,
                 const size_t           theItemSize) const
    {
      return (char* )DataPtr + size_t(theIndex) * theItemSize;
    }

  public:

    void*            DataPtr;    //!< block of elements
    Standard_Integer FirstIndex; //!< index of the first element (among all memory blocks in collection)
    Standard_Integer Length;
    Standard_Integer Size;

  };

  //! Base class for Iterator implementation
  class Iterator
  {
  protected:
    Iterator()
    : myICurBlock (0),
      myIEndBlock (0),
      myCurIndex  (0),
      myEndIndex  (0) {}

    Iterator (const NCollection_BaseVector& theVector, Standard_Boolean theToEnd = Standard_False)
    {
      initV (theVector, theToEnd);
    }

    Iterator (const Iterator& theVector)
    {
      copyV (theVector);
    }

    Standard_EXPORT void initV (const NCollection_BaseVector& theVector, Standard_Boolean theToEnd = Standard_False);

    Standard_EXPORT void copyV (const Iterator&);

    Standard_Boolean moreV() const
    {
      return (myICurBlock < myIEndBlock || myCurIndex < myEndIndex);
    }

    void nextV()
    {
      if (++myCurIndex >= myVector->myData[myICurBlock].Length
       && myICurBlock < myIEndBlock)
      {
        ++myICurBlock;
        myCurIndex = 0;
      }
    }

    void prevV()
    {
      if (--myCurIndex < 0 && myICurBlock > 0)
      {
        --myICurBlock;
        myCurIndex = myVector->myData[myICurBlock].Length - 1;
      }
    }

    void offsetV (Standard_Integer theOffset)
    {
      const Standard_Integer anIndex = myCurIndex + myICurBlock * myVector->myIncrement + theOffset;
      myICurBlock = anIndex / myVector->myIncrement;
      myCurIndex = anIndex % myVector->myIncrement;
      if (myICurBlock > myIEndBlock)
      {
        // make sure that iterator produced by Offset()
        // is equal to the end() iterator
        --myICurBlock;
        myCurIndex += myVector->myIncrement;
      }
    }

    Standard_Integer differV (const Iterator& theOther) const
    {
      return (myCurIndex - theOther.myCurIndex) + (myICurBlock - theOther.myICurBlock) * myVector->myIncrement;
    }

    const MemBlock* curBlockV() const
    {
      return &myVector->myData[myICurBlock];
    }

    const NCollection_BaseVector* myVector;    //!< the Master vector
    Standard_Integer              myICurBlock; //!< # of the current block
    Standard_Integer              myIEndBlock;
    Standard_Integer              myCurIndex;  //!< Index in the current block
    Standard_Integer              myEndIndex;
  };

protected: //! @name Block initializer

  typedef void (*initMemBlocks_t) (NCollection_BaseVector& theVector,
                                   MemBlock&               theBlock,
                                   const Standard_Integer  theFirst,
                                   const Standard_Integer  theSize);

  //! Allocate memory for array of memory blocks.
  //! @param theCapacity   Number of memory blocks in array
  //! @param theSource     Original array of memory blocks, will be automatically deallocated
  //! @param theSourceSize Number of memory blocks in original array
  Standard_EXPORT MemBlock* allocMemBlocks (const Standard_Integer theCapacity,
                                            MemBlock*              theSource     = NULL,
                                            const Standard_Integer theSourceSize = 0);

protected: //! @name protected methods

  //! Empty constructor
  NCollection_BaseVector (const Handle(NCollection_BaseAllocator)& theAllocator,
                          initMemBlocks_t                    theInitBlocks,
                          const size_t                       theSize,
                          const Standard_Integer             theInc)
  : myItemSize   (theSize),
    myIncrement  (theInc),
    myLength     (0),
    myCapacity   (GetCapacity (myIncrement)),
    myNBlocks    (0),
    myInitBlocks (theInitBlocks)
  {
    myAllocator = (theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAllocator);
    myData = allocMemBlocks (myCapacity);
  }

  //! Copy constructor
  NCollection_BaseVector (const Handle(NCollection_BaseAllocator)& theAllocator,
                          initMemBlocks_t                    theInitBlocks,
                          const NCollection_BaseVector&      theOther)
  : myItemSize   (theOther.myItemSize),
    myIncrement  (theOther.myIncrement),
    myLength     (theOther.myLength),
    myCapacity   (GetCapacity(myIncrement) + theOther.myLength / theOther.myIncrement),
    myNBlocks    (theOther.myLength == 0 ? 0 : 1 + (theOther.myLength - 1)/theOther.myIncrement),
    myInitBlocks (theInitBlocks)
  {
    myAllocator = (theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAllocator);
    myData = allocMemBlocks (myCapacity);
  }

  //! Destructor
  virtual ~NCollection_BaseVector() {}

  //! @return pointer to memory where to put the new item
  Standard_EXPORT void* expandV (const Standard_Integer theIndex);

  //! Locate the memory holding the desired value
  inline void* findV (const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if (theIndex < 0 || theIndex >= myLength,
                                  "NCollection_BaseVector::findV");
    const Standard_Integer aBlock = theIndex / myIncrement;
    return myData[aBlock].findV (theIndex - aBlock * myIncrement, myItemSize);
  }

public: //! @name public API

  //! Empty the vector of its objects
  Standard_EXPORT void Clear();
  // to set the size of increment dynamically
  void SetIncrement(const Standard_Integer aIncrement) {
    if (aIncrement > 0) {
      if (!myIncrement) {
        myIncrement=aIncrement;
      }
    }
  }

  //! Returns attached allocator
  const Handle(NCollection_BaseAllocator)& Allocator() const
  {
    return myAllocator;
  }

protected: //! @name Protected fields

  Handle(NCollection_BaseAllocator) myAllocator;
  size_t           myItemSize;
  Standard_Integer myIncrement;
  Standard_Integer myLength;
  Standard_Integer myCapacity;
  Standard_Integer myNBlocks;
  MemBlock*        myData;
  initMemBlocks_t  myInitBlocks;

protected:

  friend class Iterator;
};

#endif // NCollection_BaseVector_HeaderFile

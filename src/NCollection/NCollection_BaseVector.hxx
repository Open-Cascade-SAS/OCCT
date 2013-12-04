// Created on: 2002-04-24
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef NCollection_BaseVector_HeaderFile
#define NCollection_BaseVector_HeaderFile

#include <Standard_TypeDef.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <stddef.h>

#include <Standard_OutOfRange.hxx>

#if defined(_MSC_VER)
  #pragma warning(push, 1)
  #pragma warning(disable:4355)
#endif

// this value defines the number of blocks that are reserved
// when the capacity of vector is increased
inline Standard_Integer GetCapacity (const Standard_Integer theIncrement)
{
  return Max(theIncrement/8, 1);
}

//! Class NCollection_BaseVector - base for NCollection_Vector template
class NCollection_BaseVector
{

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

    Iterator (const NCollection_BaseVector& theVector)
    {
      initV (theVector);
    }

    Iterator (const Iterator& theVector)
    {
      copyV (theVector);
    }

    Standard_EXPORT void initV (const NCollection_BaseVector& theVector);

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
  //! @param theAllocator  Memory allocator to use
  //! @param theCapacity   Number of memory blocks in array
  //! @param theSource     Original array of memory blocks, will be automatically deallocated
  //! @param theSourceSize Number of memory blocks in original array
  Standard_EXPORT static MemBlock* allocMemBlocks (Handle(NCollection_BaseAllocator)& theAllocator,
                                                   const Standard_Integer             theCapacity,
                                                   MemBlock*                          theSource     = NULL,
                                                   const Standard_Integer             theSourceSize = 0);

protected: //! @name protected methods

  //! Empty constructor
  NCollection_BaseVector (Handle(NCollection_BaseAllocator)& theAllocator,
                          initMemBlocks_t                    theInitBlocks,
                          const size_t                       theSize,
                          const Standard_Integer             theInc)
  : myItemSize   (theSize),
    myIncrement  (theInc),
    myLength     (0),
    myCapacity   (GetCapacity (myIncrement)),
    myNBlocks    (0),
    myData       (allocMemBlocks (theAllocator, myCapacity)),
    myInitBlocks (theInitBlocks) {}

  //! Copy constructor
  NCollection_BaseVector (Handle(NCollection_BaseAllocator)& theAllocator,
                          initMemBlocks_t                    theInitBlocks,
                          const NCollection_BaseVector&      theOther)
  : myItemSize   (theOther.myItemSize),
    myIncrement  (theOther.myIncrement),
    myLength     (theOther.myLength),
    myCapacity   (GetCapacity(myIncrement) + theOther.myLength / theOther.myIncrement),
    myNBlocks    (1 + (theOther.myLength - 1)/theOther.myIncrement),
    myData       (allocMemBlocks (theAllocator, myCapacity)),
    myInitBlocks (theInitBlocks) {}

  //! @return pointer to memory where to put the new item
  Standard_EXPORT void* expandV (Handle(NCollection_BaseAllocator)& theAllocator,
                                 const Standard_Integer             theIndex);

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

protected: //! @name Private fields

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

#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

#endif // NCollection_BaseVector_HeaderFile

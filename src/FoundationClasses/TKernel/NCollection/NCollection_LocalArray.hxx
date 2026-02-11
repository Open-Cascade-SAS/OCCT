// Created on: 2009-09-23
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _NCollection_LocalArray_HeaderFile
#define _NCollection_LocalArray_HeaderFile

#include <Standard.hxx>
#include <Standard_TypeDef.hxx>

#include <cstring>
#include <algorithm>

//! Auxiliary class optimizing creation of array buffer
//! (using stack allocation for small arrays).
template <class theItem, int MAX_ARRAY_SIZE = 1024>
class NCollection_LocalArray
{
public:
  explicit NCollection_LocalArray(const size_t theSize)
      : myPtr(myBuffer),
        mySize(0)
  {
    Allocate(theSize);
  }

  NCollection_LocalArray() noexcept
      : myPtr(myBuffer),
        mySize(0)
  {
  }

  ~NCollection_LocalArray() { Deallocate(); }

  void Allocate(const size_t theSize) { Reallocate(theSize, false); }

  //! Reallocate the array to a new size.
  //! @param[in] theNewSize new number of elements
  //! @param[in] theToCopy  if true, existing elements are copied to the new buffer
  void Reallocate(const size_t theNewSize, bool theToCopy = true)
  {
    if (theNewSize <= mySize)
    {
      // Shrinking - just update the logical size, keep existing allocation
      mySize = theNewSize;
      return;
    }

    const bool   isOnHeap  = (myPtr != myBuffer);
    const size_t aNewBytes = theNewSize * sizeof(theItem);

    if (theNewSize <= static_cast<size_t>(MAX_ARRAY_SIZE))
    {
      // New size fits in stack buffer
      if (isOnHeap)
      {
        if (theToCopy && mySize > 0)
        {
          std::memcpy(myBuffer, myPtr, std::min(mySize, theNewSize) * sizeof(theItem));
        }
        Standard::Free(myPtr);
        myPtr = myBuffer;
      }
      mySize = theNewSize;
      return;
    }

    if (isOnHeap)
    {
      // Already on heap - use Standard::Reallocate (preserves content when growing)
      if (theToCopy)
      {
        myPtr = (theItem*)Standard::Reallocate(myPtr, aNewBytes);
      }
      else
      {
        Standard::Free(myPtr);
        myPtr = (theItem*)Standard::Allocate(aNewBytes);
      }
    }
    else
    {
      // Stack to heap transition
      myPtr = (theItem*)Standard::Allocate(aNewBytes);
      if (theToCopy && mySize > 0)
      {
        std::memcpy(myPtr, myBuffer, std::min(mySize, theNewSize) * sizeof(theItem));
      }
    }
    mySize = theNewSize;
  }

  size_t Size() const noexcept { return mySize; }

  operator theItem*() const noexcept { return myPtr; }

  NCollection_LocalArray(NCollection_LocalArray&& theOther) noexcept
      : myPtr(myBuffer),
        mySize(theOther.mySize)
  {
    if (theOther.myPtr == theOther.myBuffer)
    {
      std::memcpy(myBuffer, theOther.myBuffer, mySize * sizeof(theItem));
    }
    else
    {
      myPtr          = theOther.myPtr;
      theOther.myPtr = theOther.myBuffer;
    }
    theOther.mySize = 0;
  }

  NCollection_LocalArray& operator=(NCollection_LocalArray&& theOther) noexcept
  {
    if (this != &theOther)
    {
      mySize = theOther.mySize;
      if (theOther.myPtr == theOther.myBuffer)
      {
        // Source on stack: copy data to our buffer
        Deallocate();
        myPtr = myBuffer;
        std::memcpy(myBuffer, theOther.myBuffer, mySize * sizeof(theItem));
      }
      else if (myPtr != myBuffer)
      {
        // Both on heap: swap pointers, theOther frees our old allocation on destruction
        theItem* anOldPtr = myPtr;
        myPtr             = theOther.myPtr;
        theOther.myPtr    = anOldPtr;
      }
      else
      {
        // this on stack, theOther on heap: take the pointer
        myPtr          = theOther.myPtr;
        theOther.myPtr = theOther.myBuffer;
      }
      theOther.mySize = 0;
    }
    return *this;
  }

  NCollection_LocalArray(const NCollection_LocalArray&)            = delete;
  NCollection_LocalArray& operator=(const NCollection_LocalArray&) = delete;

protected:
  void Deallocate()
  {
    if (myPtr != myBuffer)
      Standard::Free(myPtr);
  }

protected:
  theItem  myBuffer[MAX_ARRAY_SIZE];
  theItem* myPtr;
  size_t   mySize;
};

#endif // _NCollection_LocalArray_HeaderFile

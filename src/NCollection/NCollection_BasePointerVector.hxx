// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef NCollection_BasePointerVector_HeaderFile
#define NCollection_BasePointerVector_HeaderFile

#include <Standard.hxx>

#include <NCollection_Allocator.hxx>
#include <NCollection_DefineAlloc.hxx>

//! Simplified class for vector of pointers of void.
//! Offers basic functionality to scalable inserts,
//! resizes and erasing last.
//!
//! Control of processing values of pointers out-of-scope
//! and should be controlled externally.
//! Especially, copy operation should post-process elements of pointers to make deep copy.
class NCollection_BasePointerVector 
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

public:

  //! Default constructor
  NCollection_BasePointerVector() {}

  //! Copy data from another vector
  Standard_EXPORT NCollection_BasePointerVector(const NCollection_BasePointerVector& theOther);

  //! Move data from another vector
  Standard_EXPORT NCollection_BasePointerVector(NCollection_BasePointerVector&& theOther) noexcept;

  //! Destroy container
  ~NCollection_BasePointerVector() { clear(); }

  //! Checks for an empty status
  bool IsEmpty() const { return mySize == 0; }

  //! Gets used size
  size_t Size() const { return mySize; }

  //! Gets available capacity
  size_t Capacity() const { return myCapacity; }

  //! Erases last element, decrements size.
  void RemoveLast() { mySize--; }

  //! Resets the size
  void Clear(const bool theReleaseMemory = false)
  {
    if (theReleaseMemory)
      clear(); 
    mySize = 0;
  }

public:

  //! Gets array, can be null
  void** GetArray() const { return myArray; }

  //! Gets value by index, no acess validation
  void* Value (const size_t theInd) const { return myArray[theInd]; }

public:

  //! Inserts new element at the end, increase size,
  //! if capacity is not enough, call resize.
  Standard_EXPORT void Append (const void* thePnt);

  //! Updates value of existed element,
  //! If index more then size, increase size of container,
  //! in this case capacity can be updated.
  Standard_EXPORT void SetValue (const size_t theInd, const void* thePnt);

  //! Copy vector
  Standard_EXPORT NCollection_BasePointerVector& operator= (const NCollection_BasePointerVector& theOther);

  //! Move vector
  Standard_EXPORT NCollection_BasePointerVector& operator= (NCollection_BasePointerVector&& theOther) noexcept;

private:

  //! Deallocate array
  Standard_EXPORT void clear();

private:
  size_t mySize = 0; //!< Used length of vector
  size_t myCapacity = 0; //!< Allocated vector size
  void** myArray = nullptr; //! Array of pointers
  NCollection_Allocator<void*> myAllocator;
};

#endif

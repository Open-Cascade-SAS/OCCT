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

#include <NCollection_BasePointerVector.hxx>

#include <TCollection.hxx>
#include <cstring>

//=======================================================================
//function : NCollection_BasePointerVector
//purpose  :
//=======================================================================
NCollection_BasePointerVector::NCollection_BasePointerVector(const NCollection_BasePointerVector& theOther) :
  mySize(theOther.mySize),
  myCapacity(theOther.myCapacity)
{
  if (myCapacity > 0)
  {
    myArray = myAllocator.allocate(myCapacity);
    memcpy(myArray, theOther.myArray, Size() * sizeof(void*));
  }
}

//=======================================================================
//function : NCollection_BasePointerVector
//purpose  :
//=======================================================================
NCollection_BasePointerVector::NCollection_BasePointerVector(NCollection_BasePointerVector&& theOther) noexcept :
  mySize(std::move(theOther.mySize)),
  myCapacity(std::move(theOther.myCapacity)),
  myArray(std::move(theOther.myArray))
{
  theOther.myCapacity = 0;
  theOther.mySize = 0;
  theOther.myArray = nullptr;
}

//=======================================================================
//function : Append
//purpose  :
//=======================================================================
void NCollection_BasePointerVector::Append (const void* thePnt)
{
  if (mySize == myCapacity)
  {
    if (myCapacity == 0)
    {
      myCapacity = 8; // the most optimal initial value
    }
    else
    {
      myCapacity <<= 1;
    }
    myArray = myAllocator.reallocate(myArray, myCapacity);
  }
  myArray[mySize++] = (void*)thePnt;
}

//=======================================================================
//function : SetValue
//purpose  :
//=======================================================================
void NCollection_BasePointerVector::SetValue (const size_t theInd, const void* thePnt)
{
  if (theInd >= myCapacity)
  {
    if (myCapacity == 0)
    {
      myCapacity = 8; // the most optimal initial value
    }
    while (myCapacity < theInd)
    {
      myCapacity <<= 1;
    }
    myArray = myAllocator.reallocate(myArray, myCapacity);
    memset(myArray + mySize, 0, (theInd - mySize) * sizeof(void**));
    mySize = theInd;
  }
  myArray[theInd] = (void*)thePnt;
}

//=======================================================================
//function : clear
//purpose  :
//=======================================================================
void NCollection_BasePointerVector::clear()
{
  if (myCapacity > 0)
  {
    myAllocator.deallocate(myArray, myCapacity);
  }
  myArray = nullptr;
  myCapacity = 0;
}

//=======================================================================
//function : operator=
//purpose  :
//=======================================================================
NCollection_BasePointerVector& NCollection_BasePointerVector::operator=(const NCollection_BasePointerVector& theOther)
{
  if (this == &theOther)
  {
    return *this;
  }
  mySize = theOther.mySize;
  if (myCapacity < theOther.myCapacity)
  {
    clear();
    myCapacity = theOther.myCapacity;
    myArray = myAllocator.allocate(myCapacity);
  }
  memcpy(myArray, theOther.myArray, mySize * sizeof(void*));
  return *this;
}

//=======================================================================
//function : operator=
//purpose  :
//=======================================================================
NCollection_BasePointerVector& NCollection_BasePointerVector::operator=(NCollection_BasePointerVector&& theOther) noexcept
{
  if (this == &theOther)
  {
    return *this;
  }
  clear();
  mySize = std::move(theOther.mySize);
  myCapacity = std::move(theOther.myCapacity);
  myArray = std::move(theOther.myArray);
  theOther.myCapacity = 0;
  theOther.mySize = 0;
  theOther.myArray = nullptr;
  return *this;
}
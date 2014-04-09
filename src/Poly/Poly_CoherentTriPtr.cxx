// Created on: 2007-12-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <Poly_CoherentTriPtr.hxx>

//=======================================================================
//function : Iterator::Next
//purpose  :
//=======================================================================

void Poly_CoherentTriPtr::Iterator::Next ()
{
  if (myCurrent)
  {
    myCurrent = myCurrent->myNext;
    if (myCurrent == myFirst)
      myCurrent = 0L;
  }
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void Poly_CoherentTriPtr::Append
                        (const Poly_CoherentTriangle *           pTri,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
{
  Handle(NCollection_BaseAllocator) anAlloc = theAlloc;
  if (theAlloc.IsNull())
    anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  Poly_CoherentTriPtr * aNewPtr = new (anAlloc) Poly_CoherentTriPtr(* pTri);
  aNewPtr->myNext = myNext;
  myNext->myPrevious = aNewPtr;
  aNewPtr->myPrevious = this;
  myNext = aNewPtr;
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================

void Poly_CoherentTriPtr::Prepend
                        (const Poly_CoherentTriangle *           pTri,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
{
  Handle(NCollection_BaseAllocator) anAlloc = theAlloc;
  if (theAlloc.IsNull())
    anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  Poly_CoherentTriPtr * aNewPtr = new (anAlloc) Poly_CoherentTriPtr(* pTri);
  aNewPtr->myPrevious = myPrevious;
  myPrevious->myNext = aNewPtr;
  aNewPtr->myNext = this;
  myPrevious = aNewPtr;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void Poly_CoherentTriPtr::Remove
                        (Poly_CoherentTriPtr *                   thePtr,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
{
  Handle(NCollection_BaseAllocator) anAlloc = theAlloc;
  if (theAlloc.IsNull())
    anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  if (thePtr->myNext && thePtr->myPrevious) {
    thePtr->myPrevious->myNext = thePtr->myNext;
    thePtr->myNext->myPrevious = thePtr->myPrevious;
    thePtr->myNext = thePtr;
    thePtr->myPrevious = thePtr;
  }
  anAlloc->Free(thePtr);
}

//=======================================================================
//function : RemoveList
//purpose  : 
//=======================================================================

void Poly_CoherentTriPtr::RemoveList
                        (Poly_CoherentTriPtr *                   thePtr,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
{
  Handle(NCollection_BaseAllocator) anAlloc = theAlloc;
  if (theAlloc.IsNull())
    anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  Poly_CoherentTriPtr * aPtr = thePtr;
  do {
    if (aPtr == 0L)
      break;
    Poly_CoherentTriPtr * aLostPtr = aPtr;
    aPtr = aPtr->myNext;
    anAlloc->Free(aLostPtr);
  } while (aPtr != thePtr);
}

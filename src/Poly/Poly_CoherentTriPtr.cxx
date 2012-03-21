// Created on: 2007-12-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
                         const Handle_NCollection_BaseAllocator& theAlloc)
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
                         const Handle_NCollection_BaseAllocator& theAlloc)
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
                         const Handle_NCollection_BaseAllocator& theAlloc)
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
                         const Handle_NCollection_BaseAllocator& theAlloc)
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

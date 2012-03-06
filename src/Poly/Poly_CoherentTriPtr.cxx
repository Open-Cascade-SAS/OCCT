// File:      Poly_CoherentTriPtr.cxx
// Created:   08.12.07 12:33
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


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

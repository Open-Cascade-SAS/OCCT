// File:      NCollection_HeapAllocator.cxx
// Created:   30.12.09 00:38
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2009


#include <NCollection_HeapAllocator.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_Mutex.hxx>

IMPLEMENT_STANDARD_HANDLE (NCollection_HeapAllocator, NCollection_BaseAllocator)
IMPLEMENT_STANDARD_RTTIEXT(NCollection_HeapAllocator, NCollection_BaseAllocator)

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

void * NCollection_HeapAllocator::Allocate (const Standard_Size theSize)
{
  // the size is rounded up to word size.
  const Standard_Size aRoundSize = (theSize + 3) & ~0x3;
  void * pResult = malloc(aRoundSize);
  if (!pResult) {
    char buf[128];
    sprintf (buf, "Failed to allocate %d bytes in global dynamic heap",theSize);
    Standard_OutOfMemory::Raise(&buf[0]);
  }
  return pResult;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void NCollection_HeapAllocator::Free (void * anAddress)
{
  if (anAddress) free(anAddress);
}

//=======================================================================
//function : GlobalHeapAllocator
//purpose  : 
//=======================================================================

const Handle_NCollection_HeapAllocator&
       NCollection_HeapAllocator::GlobalHeapAllocator()
{ 
  static Handle(NCollection_HeapAllocator) pAllocator;
  if (pAllocator.IsNull()) {
    static Standard_Mutex theMutex;
    Standard_Mutex::Sentry aSentry (theMutex);
    if (pAllocator.IsNull()) {
      pAllocator = new NCollection_HeapAllocator;
    }
  }
  return pAllocator;
}

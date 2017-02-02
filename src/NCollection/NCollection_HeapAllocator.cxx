// Created on: 2009-12-30
// Created by: Alexander GRIGORIEV
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

#include <NCollection_HeapAllocator.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_Mutex.hxx>


IMPLEMENT_STANDARD_RTTIEXT(NCollection_HeapAllocator,NCollection_BaseAllocator)

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

void * NCollection_HeapAllocator::Allocate (const Standard_Size theSize)
{
  // the size is rounded up to word size.
  const Standard_Size aRoundSize = (theSize + 3) & ~0x3;
  void* aResult = malloc (aRoundSize);
  if (aResult == NULL)
  {
    char aBuffer[96];
    Sprintf (aBuffer, "Failed to allocate %" PRIuPTR " bytes in global dynamic heap", theSize);
    throw Standard_OutOfMemory(aBuffer);
  }
  return aResult;
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

const Handle(NCollection_HeapAllocator)&
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

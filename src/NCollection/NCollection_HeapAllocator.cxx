// Created on: 2009-12-30
// Created by: Alexander GRIGORIEV
// Copyright (c) 2009-2012 OPEN CASCADE SAS
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

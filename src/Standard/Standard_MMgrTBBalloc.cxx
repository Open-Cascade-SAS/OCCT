// Created on: 2010-03-15
// Created by: Sergey KUUL
// Copyright (c) 2010-2012 OPEN CASCADE SAS
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


#include <Standard_MMgrTBBalloc.hxx>
#include <Standard_OutOfMemory.hxx>

// NOTE: replaced by more correct check
//#if defined(WNT) || defined(LIN)
//#define HAVE_TBB 1
//#endif

// paralleling with Intel TBB
#ifdef HAVE_TBB
#include <tbb/scalable_allocator.h>
using namespace tbb;
#else
#define scalable_malloc malloc
#define scalable_calloc calloc
#define scalable_realloc realloc
#define scalable_free free
#endif

//=======================================================================
//function : Standard_MMgrTBBalloc
//purpose  : 
//=======================================================================

Standard_MMgrTBBalloc::Standard_MMgrTBBalloc(const Standard_Boolean aClear)
{
  myClear = aClear;
}

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrTBBalloc::Allocate(const Standard_Size aSize)
{
  // the size is rounded up to 4 since some OCC classes
  // (e.g. TCollection_AsciiString) assume memory to be double word-aligned
  const Standard_Size aRoundSize = (aSize + 3) & ~0x3;
  // we use ?: operator instead of if() since it is faster :-)
  Standard_Address aPtr = ( myClear ? scalable_calloc(aRoundSize, sizeof(char)) :
                                      scalable_malloc(aRoundSize) );
  if ( ! aPtr )
    Standard_OutOfMemory::Raise("Standard_MMgrTBBalloc::Allocate(): malloc failed");
  return aPtr;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void Standard_MMgrTBBalloc::Free(Standard_Address& aStorage)
{
  scalable_free(aStorage);
  aStorage=NULL;
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrTBBalloc::Reallocate(Standard_Address&   aStorage,
					      const Standard_Size newSize)
{
  // the size is rounded up to 4 since some OCC classes
  // (e.g. TCollection_AsciiString) assume memory to be double word-aligned
  const Standard_Size aRoundSize = (newSize + 3) & ~0x3;
  Standard_Address newStorage = (Standard_Address)scalable_realloc(aStorage, aRoundSize);
  if ( ! newStorage )
    Standard_OutOfMemory::Raise("Standard_MMgrTBBalloc::Reallocate(): realloc failed");
  // Note that it is not possible to ensure that additional memory
  // allocated by realloc will be cleared (so as to satisfy myClear mode);
  // in order to do that we would need using memset...
  aStorage = NULL;
  return newStorage;
}

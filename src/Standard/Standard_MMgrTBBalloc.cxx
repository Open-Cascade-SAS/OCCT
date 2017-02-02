// Created on: 2010-03-15
// Created by: Sergey KUUL
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#include <Standard_MMgrTBBalloc.hxx>
#include <Standard_OutOfMemory.hxx>

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
    throw Standard_OutOfMemory("Standard_MMgrTBBalloc::Allocate(): malloc failed");
  return aPtr;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void Standard_MMgrTBBalloc::Free (Standard_Address theStorage)
{
  scalable_free (theStorage);
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrTBBalloc::Reallocate (Standard_Address theStorage,
					            const Standard_Size theSize)
{
  // the size is rounded up to 4 since some OCC classes
  // (e.g. TCollection_AsciiString) assume memory to be double word-aligned
  const Standard_Size aRoundSize = (theSize + 3) & ~0x3;
  Standard_Address newStorage = (Standard_Address)scalable_realloc(theStorage, aRoundSize);
  if ( ! newStorage )
    throw Standard_OutOfMemory("Standard_MMgrTBBalloc::Reallocate(): realloc failed");
  // Note that it is not possible to ensure that additional memory
  // allocated by realloc will be cleared (so as to satisfy myClear mode);
  // in order to do that we would need using memset...
  return newStorage;
}

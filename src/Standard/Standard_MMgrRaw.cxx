// Created on: 2005-03-15
// Created by: Peter KURNEV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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


#include <Standard_MMgrRaw.hxx>
#include <Standard_OutOfMemory.hxx>
#include <stdlib.h>

//=======================================================================
//function : Standard_MMgrRaw
//purpose  : 
//=======================================================================

Standard_MMgrRaw::Standard_MMgrRaw(const Standard_Boolean aClear)
{
  myClear = aClear;
}

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrRaw::Allocate(const Standard_Size aSize)
{
  // the size is rounded up to 4 since some OCC classes
  // (e.g. TCollection_AsciiString) assume memory to be double word-aligned
  const Standard_Size aRoundSize = (aSize + 3) & ~0x3;
  // we use ?: operator instead of if() since it is faster :-)
  Standard_Address aPtr = ( myClear ? calloc(aRoundSize, sizeof(char)) :
                                      malloc(aRoundSize) );
  if ( ! aPtr )
    Standard_OutOfMemory::Raise("Standard_MMgrRaw::Allocate(): malloc failed");
  return aPtr;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void Standard_MMgrRaw::Free(Standard_Address& aStorage)
{
  free(aStorage);
  aStorage=NULL;
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrRaw::Reallocate(Standard_Address&   aStorage,
					      const Standard_Size newSize)
{
  // the size is rounded up to 4 since some OCC classes
  // (e.g. TCollection_AsciiString) assume memory to be double word-aligned
  const Standard_Size aRoundSize = (newSize + 3) & ~0x3;
  Standard_Address newStorage = (Standard_Address)realloc(aStorage, aRoundSize);
  if ( ! newStorage )
    Standard_OutOfMemory::Raise("Standard_MMgrRaw::Reallocate(): realloc failed");
  // Note that it is not possible to ensure that additional memory
  // allocated by realloc will be cleared (so as to satisfy myClear mode);
  // in order to do that we would need using memset...
  aStorage = NULL;
  return newStorage;
}

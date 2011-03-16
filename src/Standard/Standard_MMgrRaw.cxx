// File:	Standard_MMgrRaw.cxx
// Created:	Tue Mar 15 12:09:38 2005
// Author:	Peter KURNEV
//		<pkv@irinox>

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

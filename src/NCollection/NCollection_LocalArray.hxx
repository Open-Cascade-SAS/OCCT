// Created on: 2009-09-23
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

#ifndef _NCollection_LocalArray_HeaderFile
#define _NCollection_LocalArray_HeaderFile

#include <Standard.hxx>
#include <Standard_TypeDef.hxx>

//! Auxiliary class optimizing creation of array buffer 
//! (using stack allocation for small arrays).
template<class theItem> class NCollection_LocalArray
{
public:

  // 1K * sizeof (theItem)
  static const size_t MAX_ARRAY_SIZE = 1024;

  NCollection_LocalArray (const size_t theSize)
  : myPtr (myBuffer)
  {
    Allocate(theSize);
  }

  NCollection_LocalArray ()
  : myPtr (myBuffer) {}

  ~NCollection_LocalArray()
  {
    Deallocate();
  }

  void Allocate (const size_t theSize)
  {
    Deallocate();
    if (theSize > MAX_ARRAY_SIZE)
      myPtr = (theItem*)Standard::Allocate (theSize * sizeof(theItem));
    else
      myPtr = myBuffer;
  }

  operator theItem*() const
  {
    return myPtr;
  }

private:

  NCollection_LocalArray (const NCollection_LocalArray& );
  NCollection_LocalArray& operator= (const NCollection_LocalArray& );

protected:

  void Deallocate()
  {
    if (myPtr != myBuffer)
      Standard::Free (myPtr);
  }

protected:

  theItem  myBuffer[MAX_ARRAY_SIZE];
  theItem* myPtr;

};

#endif // _NCollection_LocalArray_HeaderFile

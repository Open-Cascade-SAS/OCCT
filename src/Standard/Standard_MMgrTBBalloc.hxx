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

#ifndef _Standard_MMgrTBBalloc_HeaderFile
#define _Standard_MMgrTBBalloc_HeaderFile

#include <Standard_MMgrRoot.hxx>

//!
//! Implementation of OCC memory manager which uses Intel TBB
//! scalable allocator.
//!
//! On configurations where TBB is not available standard RTL functions 
//! malloc() / free() are used.

class Standard_MMgrTBBalloc : public Standard_MMgrRoot
{
 public:
  //! Constructor; if aClear is True, the memory will be nullified
  //! upon allocation.
  Standard_EXPORT Standard_MMgrTBBalloc(const Standard_Boolean aClear=Standard_False);

  //! Allocate aSize bytes 
  Standard_EXPORT virtual Standard_Address Allocate(const Standard_Size aSize);
  
  //! Reallocate aPtr to the size aSize. 
  //! The new pointer is returned.
  Standard_EXPORT virtual Standard_Address Reallocate (Standard_Address thePtr,
						       const Standard_Size theSize);
  
  //! Free allocated memory
  Standard_EXPORT virtual void Free (Standard_Address thePtr);

 protected:
  Standard_Boolean myClear; //! Option to nullify allocated memory
};

#endif

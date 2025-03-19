// Created on: 2005-03-15
// Created by: Peter KURNEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Standard_MMgrRoot_HeaderFile
#define _Standard_MMgrRoot_HeaderFile

#include <Standard_TypeDef.hxx>

/**
 * Root class for Open CASCADE mmemory managers.
 * Defines only abstract interface functions.
 */

class Standard_MMgrRoot
{
public:
  //! Virtual destructor; required for correct inheritance
  Standard_EXPORT virtual ~Standard_MMgrRoot();

  //! Allocate specified number of bytes.
  //! The actually allocated space should be rounded up to
  //! double word size (4 bytes), as this is expected by implementation
  //! of some classes in OCC (e.g. TCollection_AsciiString)
  Standard_EXPORT virtual Standard_Address Allocate(const Standard_Size theSize) = 0;

  //! Reallocate previously allocated memory to contain at least theSize bytes.
  //! In case of success, new pointer is returned.
  Standard_EXPORT virtual Standard_Address Reallocate(Standard_Address    thePtr,
                                                      const Standard_Size theSize) = 0;

  //! Frees previously allocated memory at specified address.
  Standard_EXPORT virtual void Free(Standard_Address thePtr) = 0;

  //! Purge internally cached unused memory blocks (if any)
  //! by releasing them to the operating system.
  //! Must return non-zero if some memory has been actually released,
  //! or zero otherwise.
  //!
  //! If option isDestroyed is True, this means that memory
  //! manager is not expected to be used any more; note however
  //! that in general case it is still possible to have calls to that
  //! instance of memory manager after this (e.g. to free memory
  //! of static objects in OCC). Thus this option should
  //! command the memory manager to release any cached memory
  //! to the system and not cache any more, but still remain operable...
  //!
  //! Default implementation does nothing and returns 0.
  Standard_EXPORT virtual Standard_Integer Purge(Standard_Boolean isDestroyed = Standard_False);
};

#endif

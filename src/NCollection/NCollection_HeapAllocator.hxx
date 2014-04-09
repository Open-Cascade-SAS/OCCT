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

#ifndef NCollection_HeapAllocator_HeaderFile
#define NCollection_HeapAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>

class Handle(NCollection_HeapAllocator);

/**
 * Allocator that uses the global dynamic heap (malloc / free). 
 */

class NCollection_HeapAllocator : public NCollection_BaseAllocator
{
 public:
  // ---------- PUBLIC METHODS ----------
  Standard_EXPORT virtual void* Allocate (const Standard_Size theSize);
  Standard_EXPORT virtual void  Free     (void * anAddress);

  Standard_EXPORT static const Handle(NCollection_HeapAllocator)&
                                GlobalHeapAllocator();

 protected:
  //! Constructor - prohibited
  NCollection_HeapAllocator(void) {}

 private:
  //! Copy constructor - prohibited
  NCollection_HeapAllocator(const NCollection_HeapAllocator&);

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NCollection_HeapAllocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NCollection_HeapAllocator, NCollection_BaseAllocator)

#endif

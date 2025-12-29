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

/**
 * Allocator that uses the global dynamic heap (malloc / free).
 */

class NCollection_HeapAllocator : public NCollection_BaseAllocator
{
public:
  // ---------- PUBLIC METHODS ----------
  Standard_EXPORT void* Allocate(const size_t theSize) override;

  void* AllocateOptimal(const size_t theSize) override { return Allocate(theSize); }

  Standard_EXPORT void Free(void* anAddress) override;

  Standard_EXPORT static const occ::handle<NCollection_HeapAllocator>& GlobalHeapAllocator();

protected:
  //! Constructor - prohibited
  NCollection_HeapAllocator() {}

private:
  //! Copy constructor - prohibited
  NCollection_HeapAllocator(const NCollection_HeapAllocator&) = delete;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(NCollection_HeapAllocator, NCollection_BaseAllocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
#endif

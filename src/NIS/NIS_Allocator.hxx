// Created on: 2010-10-22
// Created by: Alexander GRIGORIEV
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

#ifndef NIS_Allocator_HeaderFile
#define NIS_Allocator_HeaderFile

#include <NCollection_IncAllocator.hxx>

/**
 * Subclass of Incremental Allocator. It is aware of the total
 * allocated and released memory. Used in NIS_Interactive context as
 * private allocator that manages all memory used by interactive objects.
 */

class NIS_Allocator : public NCollection_IncAllocator
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Constructor.
   */
  Standard_EXPORT NIS_Allocator (const size_t theBlockSize = 24600);

  /**
   * Query the total number of allocated bytes
   */
  inline Standard_Size  NAllocated      () const
  {
    return myNAllocated;
  }

  /**
   * Query the total number of released bytes
   */
  inline Standard_Size  NFreed          () const
  {
    return myNFreed;
  }

  /**
   * Set both counters to zero. Should be called with method Reset of the base
   * class NCollection_IncAlocator.
   */
  Standard_EXPORT void ResetCounters   ();

  /**
   * Allocate memory with given size. Returns NULL on failure
   */
  Standard_EXPORT virtual void* Allocate        (const size_t size);

  /*
   * Free a previously allocated memory. Does nothing but count released bytes.
   */
  Standard_EXPORT virtual void  Free            (void *anAddress);

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Size         myNAllocated;
  Standard_Size         myNFreed;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Allocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Allocator, NCollection_IncAllocator)


#endif

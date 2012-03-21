// Created on: 2010-10-22
// Created by: Alexander GRIGORIEV
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

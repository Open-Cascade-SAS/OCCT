// Created on: 2002-04-12
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef NCollection_IncAllocator_HeaderFile
#define NCollection_IncAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>

/**
 *  Class NCollection_IncAllocator - incremental memory  allocator. This class
 *  allocates  memory  on  request  returning  the  pointer  to  an  allocated
 *  block. This memory is never returned  to the system until the allocator is
 *  destroyed.
 *  
 *  By comparison with  the standard new() and malloc()  calls, this method is
 *  faster and consumes very small additional memory to maintain the heap.
 *  
 *  All pointers  returned by Allocate() are  aligned to the size  of the data
 *  type "aligned_t". To  modify the size of memory  blocks requested from the
 *  OS,  use the parameter  of the  constructor (measured  in bytes);  if this
 *  parameter is  smaller than  25 bytes on  32bit or  49 bytes on  64bit, the
 *  block size will be the default 24 kbytes
 */
class NCollection_IncAllocator : public NCollection_BaseAllocator
{
 public:
  // The type defining the alignement of allocated objects
  typedef void * aligned_t;

  // ---------- PUBLIC METHODS ----------

  //! Constructor
  Standard_EXPORT NCollection_IncAllocator (const size_t theBlockSize = DefaultBlockSize);

  //! Allocate memory with given size. Returns NULL on failure
  Standard_EXPORT virtual void* Allocate        (const size_t size) Standard_OVERRIDE;

  //! Free a previously allocated memory. Does nothing
  Standard_EXPORT virtual void  Free            (void *anAddress) Standard_OVERRIDE;

  //! Diagnostic method, returns the total allocated size
  Standard_EXPORT size_t        GetMemSize      () const;

  //! Destructor (calls Clean() internally)
  Standard_EXPORT ~NCollection_IncAllocator     ();

  //! Reallocation: it is always allowed but is only efficient with the
  //! last allocated item
  Standard_EXPORT void *        Reallocate      (void * anAddress,
                                                 const size_t oldSize,
                                                 const size_t newSize);

  //! Re-initialize the allocator so that the next Allocate call should
  //! start allocating in the very begining as though the allocator is just
  //! constructed. Warning: make sure that all previously allocated data are
  //! no more used in your code!
  //! @param doReleaseMem
  //!   True - release all previously allocated memory, False - preserve it
  //!   for future allocations.
  Standard_EXPORT void          Reset           (const Standard_Boolean
                                                 doReleaseMem=Standard_True);

  static const size_t DefaultBlockSize = 24600;

 protected:
  struct         IBlock;

  //! Flush all previously allocated data. All pointers returned by
  //! Allocate() become invalid -- be very careful with this
  Standard_EXPORT void  Clean                   ();

  //! Allocate a new block and return a pointer to it
  //! ** only for internal usage **
  void *                allocateNewBlock        (const size_t cSize);

 private:
  // Prohibited methods
  NCollection_IncAllocator (const NCollection_IncAllocator&);
  NCollection_IncAllocator& operator = (const NCollection_IncAllocator&);

 protected:
  // ----- PROTECTED CLASS IBlock -------
  struct IBlock {
    aligned_t * allocateInBlock (const size_t cSize)
    {
      aligned_t * aResult = p_free_space;
      p_free_space += cSize;
      return aResult;
    }
    aligned_t     * p_free_space;
    aligned_t     * p_end_block;
    struct IBlock * p_next;
  };
 protected:
  // --------- PROTECTED FIELDS ---------
  IBlock        * myFirstBlock;
  size_t        mySize;
  size_t        myMemSize;

 public:
// Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(NCollection_IncAllocator,NCollection_BaseAllocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NCollection_IncAllocator, NCollection_BaseAllocator)

#endif

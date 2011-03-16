// File:      NCollection_HeapAllocator.hxx
// Created:   30.12.09 00:33
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2009


#ifndef NCollection_HeapAllocator_HeaderFile
#define NCollection_HeapAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>

class Handle_NCollection_HeapAllocator;

/**
 * Allocator that uses the global dynamic heap (malloc / free). 
 */

class NCollection_HeapAllocator : public NCollection_BaseAllocator
{
 public:
  // ---------- PUBLIC METHODS ----------
  Standard_EXPORT virtual void* Allocate (const Standard_Size theSize);
  Standard_EXPORT virtual void  Free     (void * anAddress);

  Standard_EXPORT static const Handle_NCollection_HeapAllocator&
                                GlobalHeapAllocator();

 protected:
  //! Constructor - prohibited
  NCollection_HeapAllocator(void) {};

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

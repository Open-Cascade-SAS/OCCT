// Created on: 2009-12-30
// Created by: Alexander GRIGORIEV
// Copyright (c) 2009-2012 OPEN CASCADE SAS
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

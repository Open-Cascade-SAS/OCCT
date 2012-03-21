// Created on: 2002-04-12
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Purpose:     Basic class for memory allocation wizards.
//              Defines  the  interface  for devising  different  allocators
//              firstly to be used  by collections of NCollection, though it
//              it is not  deferred. It allocates/frees  the memory  through
//              Standard procedures, thus it is  unnecessary (and  sometimes
//              injurious) to have  more than one such  allocator.  To avoid
//              creation  of multiple  objects the  constructors  were  maid
//              inaccessible.  To  create the  BaseAllocator use  the method
//              CommonBaseAllocator.
//              Note that this object is managed by Handle.

#ifndef NCollection_BaseAllocator_HeaderFile
#define NCollection_BaseAllocator_HeaderFile

#include <MMgt_TShared.hxx>
#include <Standard_DefineHandle.hxx>
#include <NCollection_TypeDef.hxx>

class Handle(NCollection_BaseAllocator);

/**
* Purpose:     Basic class for memory allocation wizards.
*              Defines  the  interface  for devising  different  allocators
*              firstly to be used  by collections of NCollection, though it
*              it is not  deferred. It allocates/frees  the memory  through
*              Standard procedures, thus it is  unnecessary (and  sometimes
*              injurious) to have  more than one such  allocator.  To avoid
*              creation  of multiple  objects the  constructors  were  maid
*              inaccessible.  To  create the  BaseAllocator use  the method
*              CommonBaseAllocator.
*              Note that this object is managed by Handle.
*/              
class NCollection_BaseAllocator : public MMgt_TShared
{
 public:
  // ---------- PUBLIC METHODS ------------
  Standard_EXPORT virtual void* Allocate (const size_t size);
  Standard_EXPORT virtual void  Free     (void * anAddress);
  
  //! CommonBaseAllocator
  //! This method is designed to have the only one BaseAllocator (to avoid
  //! useless copying of collections). However one can use operator new to
  //! create more BaseAllocators, but it is injurious.
  Standard_EXPORT static const Handle(NCollection_BaseAllocator)&
    CommonBaseAllocator(void);

  //! Callback function to register alloc/free calls
  Standard_EXPORT static void StandardCallBack
                    (const Standard_Boolean theIsAlloc,
                     const Standard_Address theStorage,
                     const Standard_Size theRoundSize,
                     const Standard_Size theSize);

  //! Prints memory usage statistics cumulated by StandardCallBack
  Standard_EXPORT static void PrintMemUsageStatistics();

 protected:
  //! Constructor - prohibited
  NCollection_BaseAllocator(void) {};

 private:
  //! Copy constructor - prohibited
  NCollection_BaseAllocator(const NCollection_BaseAllocator&);

 public:
  // ---------- CasCade RunTime Type Information
  DEFINE_STANDARD_RTTI(NCollection_BaseAllocator)
};

DEFINE_STANDARD_HANDLE(NCollection_BaseAllocator,MMgt_TShared)

#endif

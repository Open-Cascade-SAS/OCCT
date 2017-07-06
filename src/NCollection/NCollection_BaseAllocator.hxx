// Created on: 2002-04-12
// Created by: Alexander KARTOMIN (akm)
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

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <NCollection_TypeDef.hxx>


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
class NCollection_BaseAllocator : public Standard_Transient
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
  NCollection_BaseAllocator(void) {}

 private:
  //! Copy constructor - prohibited
  NCollection_BaseAllocator(const NCollection_BaseAllocator&);

 public:
  // ---------- CasCade RunTime Type Information
  DEFINE_STANDARD_RTTIEXT(NCollection_BaseAllocator,Standard_Transient)
};

DEFINE_STANDARD_HANDLE(NCollection_BaseAllocator,Standard_Transient)

#endif

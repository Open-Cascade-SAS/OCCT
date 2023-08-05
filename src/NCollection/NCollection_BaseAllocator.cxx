/// Copyright (c) 2002-2023 OPEN CASCADE SAS
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

#include <NCollection_BaseAllocator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(NCollection_BaseAllocator,Standard_Transient)

//=======================================================================
//function : Allocate
//purpose  : Standard allocation
//=======================================================================
void* NCollection_BaseAllocator::Allocate(const size_t theSize)
{
  return Standard::Allocate(theSize);
}

//=======================================================================
//function : AllocateOptimal
//purpose  : Optimal allocation
//=======================================================================
void* NCollection_BaseAllocator::AllocateOptimal(const size_t theSize)
{
  return Standard::AllocateOptimal(theSize);
}

//=======================================================================
//function : Free
//purpose  : Standard deallocation
//=======================================================================
void  NCollection_BaseAllocator::Free(void* theAddress)
{
  Standard::Free(theAddress);
}

//=======================================================================
//function : CommonBaseAllocator
//purpose  : Creates the only one BaseAllocator
//=======================================================================
const Handle(NCollection_BaseAllocator)& 
       NCollection_BaseAllocator::CommonBaseAllocator(void)
{ 
  static Handle(NCollection_BaseAllocator) THE_SINGLETON_ALLOC =
    new NCollection_BaseAllocator;
  return THE_SINGLETON_ALLOC;
}

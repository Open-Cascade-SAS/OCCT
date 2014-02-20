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

#include <NIS_Allocator.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_Allocator, NCollection_IncAllocator)
IMPLEMENT_STANDARD_RTTIEXT (NIS_Allocator, NCollection_IncAllocator)

//=======================================================================
//function : NIS_Allocator
//purpose  : 
//=======================================================================

NIS_Allocator::NIS_Allocator (const size_t theBlockSize)
  : NCollection_IncAllocator  (theBlockSize),
    myNAllocated              (0),
    myNFreed                  (0)
{}

//=======================================================================
//function : ResetCounters
//purpose  : 
//=======================================================================

void NIS_Allocator::ResetCounters ()
{
  myNAllocated = 0;
  myNFreed = 0;
}

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

void* NIS_Allocator::Allocate (const size_t size)
{
  size_t* pResult = reinterpret_cast<size_t*>
    (NCollection_IncAllocator::Allocate(size + sizeof(size_t)));
  pResult[0] = size;
  myNAllocated += size;
  return &pResult[1];
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void NIS_Allocator::Free (void *anAddress)
{
  if (anAddress) {
    size_t* pAddress = reinterpret_cast<size_t*>(anAddress) - 1;
    myNFreed += pAddress[0];
    NCollection_IncAllocator::Free(pAddress);
  }
}

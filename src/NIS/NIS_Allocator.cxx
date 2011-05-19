// File:      NIS_Allocator.cpp
// Created:   22.10.10 17:35
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2010

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

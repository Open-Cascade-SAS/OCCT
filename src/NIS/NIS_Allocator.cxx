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

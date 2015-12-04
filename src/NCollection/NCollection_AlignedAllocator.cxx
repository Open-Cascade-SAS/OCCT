// Created on: 2014-03-31
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <NCollection_AlignedAllocator.hxx>
#include <NCollection_Buffer.hxx>




IMPLEMENT_STANDARD_RTTIEXT(NCollection_AlignedAllocator,NCollection_BaseAllocator)

//=======================================================================
//function : NCollection_AlignedAllocator()
//purpose  : Constructor
//=======================================================================
NCollection_AlignedAllocator::NCollection_AlignedAllocator (const size_t theAlignment)
: myAlignment (theAlignment)
{
  //
}

//=======================================================================
//function : Allocate
//purpose  : allocate a memory
//=======================================================================
void* NCollection_AlignedAllocator::Allocate (const size_t theSize)
{
  return Standard::AllocateAligned (theSize, myAlignment);
}

//=======================================================================
//function : Free
//purpose  :
//=======================================================================
void NCollection_AlignedAllocator::Free (void* thePtr)
{
  Standard::FreeAligned (thePtr);
}

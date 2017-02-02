// Created on: 22.07.11
// Created by: Kirill GAVRILOV
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

#include <NCollection_WinHeapAllocator.hxx>
#include <Standard_OutOfMemory.hxx>

IMPLEMENT_STANDARD_RTTIEXT(NCollection_WinHeapAllocator,NCollection_BaseAllocator)

#if(defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
#endif

//=======================================================================
//function : NCollection_WinHeapAllocator
//purpose  : Main constructor
//=======================================================================
NCollection_WinHeapAllocator::NCollection_WinHeapAllocator
                                        (const size_t theInitSizeBytes)
: NCollection_BaseAllocator(),
#if(defined(_WIN32) || defined(__WIN32__))
  myHeapH (HeapCreate (0, theInitSizeBytes, 0)),
#endif
  myToZeroMemory (Standard_False)
{
#if defined(_WIN32) && (_WIN32_WINNT >= 0x0501)
  // activate LHF to improve small size allocations
  ULONG aHeapInfo = 2;
  HeapSetInformation (myHeapH, HeapCompatibilityInformation,
                      &aHeapInfo, sizeof(aHeapInfo));
#else
  (void )theInitSizeBytes;
#endif
}

//=======================================================================
//function : ~NCollection_WinHeapAllocator
//purpose  : Destructor
//=======================================================================
NCollection_WinHeapAllocator::~NCollection_WinHeapAllocator()
{
#if(defined(_WIN32) || defined(__WIN32__))
  HeapDestroy (myHeapH);
#endif
}


//=======================================================================
//function : Allocate
//purpose  :
//=======================================================================
void* NCollection_WinHeapAllocator::Allocate (const Standard_Size theSize)
{
  // the size is rounded up to word size.
  const Standard_Size aRoundSize = (theSize + 3) & ~0x3;
#if(defined(_WIN32) || defined(__WIN32__))
  void* aResult = HeapAlloc (myHeapH, myToZeroMemory ? HEAP_ZERO_MEMORY : 0,
                             aRoundSize);
#else
  void* aResult = malloc (aRoundSize);
#endif
  if (aResult == NULL)
  {
    char aBuf[128];
    Sprintf (aBuf, "Failed to allocate %" PRIuPTR " bytes in local dynamic heap", theSize);
    throw Standard_OutOfMemory(aBuf);
  }
  return aResult;
}

//=======================================================================
//function : Free
//purpose  :
//=======================================================================
void NCollection_WinHeapAllocator::Free (void* theAddress)
{
  if (theAddress != NULL)
  {
  #if(defined(_WIN32) || defined(__WIN32__))
    HeapFree (myHeapH, 0, theAddress);
  #else
    free (theAddress);
  #endif
  }
}

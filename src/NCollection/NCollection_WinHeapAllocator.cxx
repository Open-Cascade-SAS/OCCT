// File:      NCollection_WinHeapAllocator.cxx
// Created:   22.07.11
// Author:    Kirill GAVRILOV
// Copyright: Open Cascade 2011

#include <NCollection_WinHeapAllocator.hxx>
#include <Standard_OutOfMemory.hxx>

#if(defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
#endif

IMPLEMENT_STANDARD_HANDLE (NCollection_WinHeapAllocator,
                           NCollection_BaseAllocator)
IMPLEMENT_STANDARD_RTTIEXT(NCollection_WinHeapAllocator,
                           NCollection_BaseAllocator)

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
#if(defined(_WIN32) || defined(__WIN32__))
  // activate LHF to improve small size allocations
  ULONG aHeapInfo = 2;
  HeapSetInformation (myHeapH, HeapCompatibilityInformation,
                      &aHeapInfo, sizeof(aHeapInfo));
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
    Sprintf (aBuf, "Failed to allocate "PRIuPTR" bytes in local dynamic heap", theSize);
    Standard_OutOfMemory::Raise (aBuf);
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

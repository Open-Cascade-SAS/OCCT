// Created on: 2005-03-15
// Created by: Peter KURNEV
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard.hxx>
#include <Standard_MMgrOpt.hxx>
#include <Standard_MMgrRaw.hxx>
#include <Standard_MMgrTBBalloc.hxx>
#include <Standard_Assert.hxx>

#include <stdlib.h>
#if(defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
  #include <malloc.h>
  #include <locale.h>
#endif

#if defined(_MSC_VER) || defined(__ANDROID__) || defined(__QNX__)
  #include <malloc.h>
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && (defined(__i386) || defined(__x86_64)))
  #include <mm_malloc.h>
#else
  extern "C" int posix_memalign (void** thePtr, size_t theAlign, size_t theSize);
#endif

// There is no support for environment variables in UWP
// OSD_Environment could not be used here because of cyclic dependency
#ifdef OCCT_UWP
#define getenv(x) NULL
#endif

#ifndef OCCT_MMGT_OPT_DEFAULT
#define OCCT_MMGT_OPT_DEFAULT 0
#endif

//=======================================================================
//class    : Standard_MMgrFactory 
//purpose  : Container for pointer to memory manager;
//           used to construct appropriate memory manager according
//           to environment settings, and to ensure destruction upon exit
//=======================================================================
class Standard_MMgrFactory
{
public:
  static Standard_MMgrRoot* GetMMgr();
  ~Standard_MMgrFactory();

private:
  Standard_MMgrFactory();
  Standard_MMgrFactory (const Standard_MMgrFactory&);
  Standard_MMgrFactory& operator= (const Standard_MMgrFactory&);

private:
  Standard_MMgrRoot* myFMMgr;
};

//=======================================================================
//function : Standard_MMgrFactory
//purpose  : Check environment variables and create appropriate memory manager
//=======================================================================

Standard_MMgrFactory::Standard_MMgrFactory()
: myFMMgr (NULL)
{
/*#if defined(_MSC_VER) && (_MSC_VER > 1400)
  // Turn ON thread-safe C locale globally to avoid side effects by setlocale() calls between threads.
  // After this call all following _configthreadlocale() will be ignored assuming
  // Notice that this is MSVCRT feature - on POSIX systems xlocale API (uselocale instead of setlocale)
  // should be used explicitly to ensure thread-safety!

  // This is not well documented call because _ENABLE_PER_THREAD_LOCALE_GLOBAL flag is defined but not implemented for some reason.
  // -1 will set global locale flag to force _ENABLE_PER_THREAD_LOCALE_GLOBAL + _ENABLE_PER_THREAD_LOCALE_NEW behaviour
  // although there NO way to turn it off again and following calls will have no effect (locale will be changed only for current thread).
  _configthreadlocale (-1);
#endif*/

  // Check basic assumption.
  // If assertion happens, then OCCT should be corrected for compatibility with such CPU architecture.
  Standard_STATIC_ASSERT(sizeof(Standard_Utf8Char)  == 1);
  Standard_STATIC_ASSERT(sizeof(short) == 2);
  Standard_STATIC_ASSERT(sizeof(Standard_Utf16Char) == 2);
  Standard_STATIC_ASSERT(sizeof(Standard_Utf32Char) == 4);
#ifdef _WIN32
  Standard_STATIC_ASSERT(sizeof(Standard_WideChar) == sizeof(Standard_Utf16Char));
#endif

  char* aVar;
  aVar = getenv ("MMGT_OPT");
  Standard_Integer anAllocId   = (aVar ?  atoi (aVar): OCCT_MMGT_OPT_DEFAULT);

#if defined(_WIN32) && !defined(_WIN64) && !defined(__MINGW32__)
  static const DWORD _SSE2_FEATURE_BIT(0x04000000);
  if ( anAllocId == 2 )
  {
    // CR25396: Check if SSE2 instructions are supported, if not then use MMgrRaw
    // instead of MMgrTBBalloc. It is to avoid runtime crash when running on a 
    // CPU that supports SSE but does not support SSE2 (some modifications of
    // AMD Sempron).
    DWORD volatile dwFeature;
    _asm
    {
      push eax
      push ebx
      push ecx
      push edx

      // get the CPU feature bits
      mov eax, 1
      cpuid
      mov dwFeature, edx

      pop edx
      pop ecx
      pop ebx
      pop eax
    }
    if ((dwFeature & _SSE2_FEATURE_BIT) == 0)
      anAllocId = 0;
  }
#endif

  aVar = getenv ("MMGT_CLEAR");
  Standard_Boolean toClear     = (aVar ? (atoi (aVar) != 0) : Standard_True);

  // on Windows (actual for XP and 2000) activate low fragmentation heap
  // for CRT heap in order to get best performance.
  // Environment variable MMGT_LFH can be used to switch off this action (if set to 0)
#if defined(_MSC_VER)
  aVar = getenv ("MMGT_LFH");
  if ( aVar == NULL || atoi (aVar) != 0 )
  {
    ULONG aHeapInfo = 2;
    HANDLE aCRTHeap = (HANDLE)_get_heap_handle();
    HeapSetInformation (aCRTHeap, HeapCompatibilityInformation, &aHeapInfo, sizeof(aHeapInfo));
  }
#endif

  switch (anAllocId)
  {
    case 1:  // OCCT optimized memory allocator
    {
      aVar = getenv ("MMGT_MMAP");
      Standard_Boolean bMMap       = (aVar ? (atoi (aVar) != 0) : Standard_True);
      aVar = getenv ("MMGT_CELLSIZE");
      Standard_Integer aCellSize   = (aVar ?  atoi (aVar) : 200);
      aVar = getenv ("MMGT_NBPAGES");
      Standard_Integer aNbPages    = (aVar ?  atoi (aVar) : 1000);
      aVar = getenv ("MMGT_THRESHOLD");
      Standard_Integer aThreshold  = (aVar ?  atoi (aVar) : 40000);
      myFMMgr = new Standard_MMgrOpt (toClear, bMMap, aCellSize, aNbPages, aThreshold);
      break;
    }
    case 2:  // TBB memory allocator
      myFMMgr = new Standard_MMgrTBBalloc (toClear);
      break;
    case 0:
    default: // system default memory allocator
      myFMMgr = new Standard_MMgrRaw (toClear);
  }
}

//=======================================================================
//function : ~Standard_MMgrFactory
//purpose  : 
//=======================================================================

Standard_MMgrFactory::~Standard_MMgrFactory()
{
  if (  myFMMgr )
    myFMMgr->Purge(Standard_True);
}

//=======================================================================
// function: GetMMgr
//
// This static function has a purpose to wrap static holder for memory 
// manager instance. 
//
// Wrapping holder inside a function is needed to ensure that it will
// be initialized not later than the first call to memory manager (that
// would be impossible to guarantee if holder was static variable on 
// global or file scope, because memory manager may be called from 
// constructors of other static objects).
//
// Note that at the same time we could not guarantee that the holder 
// object is destroyed after last call to memory manager, since that 
// last call may be from static Handle() object which has been initialized
// dynamically during program execution rather than in its constructor.
//
// Therefore holder currently does not call destructor of the memory manager 
// but only its method Purge() with Standard_True.
//
// To free the memory completely, we probably could use compiler-specific 
// pragmas (such as '#pragma fini' on SUN Solaris and '#pragma init_seg' on 
// WNT MSVC++) to put destructing function in code segment that is called
// after destructors of other (even static) objects. However, this is not 
// done by the moment since it is compiler-dependent and there is no guarantee 
// thatsome other object calling memory manager is not placed also in that segment...
//
// Note that C runtime function atexit() could not help in this problem 
// since its behaviour is the same as for destructors of static objects 
// (see ISO 14882:1998 "Programming languages -- C++" 3.6.3)
//
// The correct approach to deal with the problem would be to have memory manager 
// to properly control its memory allocation and caching free blocks so 
// as to release all memory as soon as it is returned to it, and probably
// even delete itself if all memory it manages has been released and 
// last call to method Purge() was with True.
//
// Note that one possible method to control memory allocations could
// be counting calls to Allocate() and Free()...
//
//=======================================================================
Standard_MMgrRoot* Standard_MMgrFactory::GetMMgr()
{
  static Standard_MMgrFactory aFactory;
  return aFactory.myFMMgr;
}

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

Standard_Address Standard::Allocate(const Standard_Size size)
{
  return Standard_MMgrFactory::GetMMgr()->Allocate(size);
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void Standard::Free (Standard_Address theStorage)
{
  Standard_MMgrFactory::GetMMgr()->Free(theStorage);
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_Address Standard::Reallocate (Standard_Address theStorage,
				       const Standard_Size theSize)
{
  return Standard_MMgrFactory::GetMMgr()->Reallocate (theStorage, theSize);
}

//=======================================================================
//function : Purge
//purpose  : 
//=======================================================================

Standard_Integer Standard::Purge()
{
  return Standard_MMgrFactory::GetMMgr()->Purge();
}

//=======================================================================
//function : AllocateAligned
//purpose  :
//=======================================================================

Standard_Address Standard::AllocateAligned (const Standard_Size theSize,
                                            const Standard_Size theAlign)
{
#if defined(_MSC_VER)
  return _aligned_malloc (theSize, theAlign);
#elif defined(__ANDROID__) || defined(__QNX__)
  return memalign (theAlign, theSize);
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && (defined(__i386) || defined(__x86_64)))
  return _mm_malloc (theSize, theAlign);
#else
  void* aPtr;
  if (posix_memalign (&aPtr, theAlign, theSize))
  {
    return NULL;
  }
  return aPtr;
#endif
}

//=======================================================================
//function : FreeAligned
//purpose  :
//=======================================================================

void Standard::FreeAligned (Standard_Address thePtrAligned)
{
#if defined(_MSC_VER)
  _aligned_free (thePtrAligned);
#elif defined(__ANDROID__) || defined(__QNX__)
  free (thePtrAligned);
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && (defined(__i386) || defined(__x86_64)))
  _mm_free (thePtrAligned);
#else
  free (thePtrAligned);
#endif
}

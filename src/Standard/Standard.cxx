// Copyright (c) 1999-2023 OPEN CASCADE SAS
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

#include <Standard_OutOfMemory.hxx>

#include <stdlib.h>

#if(defined(_WIN32) || defined(__WIN32__))
#include <windows.h>
#include <malloc.h>
#include <locale.h>
#endif

#if defined(_MSC_VER) || defined(__ANDROID__) || defined(__QNX__)
#include <malloc.h>
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) && (defined(__i386) || defined(__x86_64)))
#include <mm_malloc.h>
#else
extern "C" int posix_memalign(void** thePtr, size_t theAlign, size_t theSize);
#endif

namespace
{
  static Standard::AllocatorType& allocatorTypeInstance()
  {
    static Standard::AllocatorType aType =
#ifdef OCCT_MMGT_OPT_FLEXIBLE
      Standard::AllocatorType::NATIVE;
#elif defined OCCT_MMGT_OPT_TBB
      Standard::AllocatorType::TBB;
#elif defined OCCT_MMGT_OPT_JEMALLOC
      Standard::AllocatorType::JEMALLOC;
#else
      Standard::AllocatorType::NATIVE;
#endif
    return aType;
  }
}

#ifdef OCCT_MMGT_OPT_JEMALLOC
#define JEMALLOC_NO_DEMANGLE
#include <jemalloc.h>
#endif // OCCT_MMGT_OPT_JEMALLOC

// paralleling with Intel TBB
#ifdef HAVE_TBB
#include <tbb/scalable_allocator.h>
#else
#ifdef OCCT_MMGT_OPT_TBB
#undef OCCT_MMGT_OPT_TBB
#endif
#define scalable_malloc malloc
#define scalable_calloc calloc
#define scalable_realloc realloc
#define scalable_free free
#endif

// Available macros definition
// - OCCT_MMGT_OPT_FLEXIBLE, modifiable in real time
// - OCCT_MMGT_OPT_TBB, using tbb::scalable_allocator
// - OCCT_MMGT_OPT_NATIVE, using native calloc, free
// - OCCT_MMGT_OPT_JEMALLOC, using external jecalloc, jefree
#ifdef OCCT_MMGT_OPT_FLEXIBLE
#include <Standard_MMgrOpt.hxx>
#include <Standard_Assert.hxx>

// There is no support for environment variables in UWP
// OSD_Environment could not be used here because of cyclic dependency
#ifdef OCCT_UWP
#define getenv(x) NULL
#endif

#ifndef OCCT_MMGT_OPT_DEFAULT
#define OCCT_MMGT_OPT_DEFAULT 0
#endif
namespace
{
  /**
  * Implementation of raw OCC memory manager which uses standard C
  * functions: malloc (or calloc), free and realloc
  * without any optimization
  */
  class Standard_MMgrRaw : public Standard_MMgrRoot
  {
  public:
    //! Constructor; if theToClear is True, the memory will be nullified
    //! upon allocation.
    Standard_MMgrRaw(const Standard_Boolean theToClear = Standard_False)
    {
      myClear = theToClear;
    }

    //! Allocate theSize bytes 
    Standard_Address Allocate(const Standard_Size theSize) override
    {
      // we use ?: operator instead of if() since it is faster :-)
      Standard_Address aPtr = (myClear ? calloc(theSize, sizeof(char)) :
                               malloc(theSize));
      if (!aPtr)
        throw Standard_OutOfMemory("Standard_MMgrRaw::Allocate(): malloc failed");
      return aPtr;
    }

    //! Reallocate thePtr to the size theSize. 
    //! The new pointer is returned.
    Standard_Address Reallocate(Standard_Address thePtr,
                                const Standard_Size theSize) override
    {
      Standard_Address aNewStorage = (Standard_Address)realloc(thePtr, theSize);
      if (!aNewStorage)
        throw Standard_OutOfMemory("Standard_MMgrRaw::Reallocate(): realloc failed");
      // Note that it is not possible to ensure that additional memory
      // allocated by realloc will be cleared (so as to satisfy myClear mode);
      // in order to do that we would need using memset...
      return aNewStorage;
    }

    //! Free allocated memory. The pointer is nullified.
    void Free(Standard_Address thePtr) override
    {
      free(thePtr);
    }

  protected:
    Standard_Boolean myClear; //! Option to nullify allocated memory
  };

  //! Implementation of OCC memory manager which uses Intel TBB
  //! scalable allocator.
  //!
  //! On configurations where TBB is not available standard RTL functions 
  //! malloc() / free() are used.
  class Standard_MMgrTBBalloc : public Standard_MMgrRoot
  {
  public:
    //! Constructor; if theClear is True, the memory will be nullified
    //! upon allocation.
    Standard_MMgrTBBalloc(const Standard_Boolean theClear = Standard_False)
    {
      myClear = theClear;
    }

    //! Allocate theSize bytes 
    Standard_Address Allocate(const Standard_Size theSize) override
    {
      // we use ?: operator instead of if() since it is faster :-)
      Standard_Address aPtr = (myClear ? scalable_calloc(theSize, sizeof(char)) :
                               scalable_malloc(theSize));
      if (!aPtr)
        throw Standard_OutOfMemory("Standard_MMgrTBBalloc::Allocate(): malloc failed");
      return aPtr;
    }

    //! Reallocate thePtr to the size theSize. 
    //! The new pointer is returned.
    Standard_Address Reallocate(Standard_Address thePtr,
                                const Standard_Size theSize) override
    {
      Standard_Address aNewStorage = (Standard_Address)scalable_realloc(thePtr, theSize);
      if (!aNewStorage)
        throw Standard_OutOfMemory("Standard_MMgrTBBalloc::Reallocate(): realloc failed");
      // Note that it is not possible to ensure that additional memory
      // allocated by realloc will be cleared (so as to satisfy myClear mode);
      // in order to do that we would need using memset...
      return aNewStorage;
    }

    //! Free allocated memory
    void Free(Standard_Address thePtr) override
    {
      scalable_free(thePtr);
    }

  protected:
    Standard_Boolean myClear; //! Option to nullify allocated memory
  };

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
    Standard_MMgrFactory(const Standard_MMgrFactory&);
    Standard_MMgrFactory& operator= (const Standard_MMgrFactory&);

  private:
    Standard_MMgrRoot* myFMMgr;
  };

  //=======================================================================
  //function : Standard_MMgrFactory
  //purpose  : Check environment variables and create appropriate memory manager
  //=======================================================================

  Standard_MMgrFactory::Standard_MMgrFactory()
    : myFMMgr(NULL)
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
    Standard_STATIC_ASSERT(sizeof(Standard_Utf8Char) == 1);
    Standard_STATIC_ASSERT(sizeof(short) == 2);
    Standard_STATIC_ASSERT(sizeof(Standard_Utf16Char) == 2);
    Standard_STATIC_ASSERT(sizeof(Standard_Utf32Char) == 4);
#ifdef _WIN32
    Standard_STATIC_ASSERT(sizeof(Standard_WideChar) == sizeof(Standard_Utf16Char));
#endif

    char* aVar;
    aVar = getenv("MMGT_OPT");
    Standard_Integer anAllocId = (aVar ? atoi(aVar) : OCCT_MMGT_OPT_DEFAULT);

#if defined(HAVE_TBB) && defined(_M_IX86)
    if (anAllocId == 2)
    {
      // CR25396: Check if SSE2 instructions are supported on 32-bit x86 processor on Windows platform,
      // if not then use MMgrRaw instead of MMgrTBBalloc.
      // It is to avoid runtime crash when running on a CPU
      // that supports SSE but does not support SSE2 (some modifications of AMD Sempron).
      static const DWORD _SSE2_FEATURE_BIT(0x04000000);
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

    aVar = getenv("MMGT_CLEAR");
    Standard_Boolean toClear = (aVar ? (atoi(aVar) != 0) : Standard_True);

    // on Windows (actual for XP and 2000) activate low fragmentation heap
    // for CRT heap in order to get best performance.
    // Environment variable MMGT_LFH can be used to switch off this action (if set to 0)
#if defined(_MSC_VER)
    aVar = getenv("MMGT_LFH");
    if (aVar == NULL || atoi(aVar) != 0)
    {
      ULONG aHeapInfo = 2;
      HANDLE aCRTHeap = (HANDLE)_get_heap_handle();
      HeapSetInformation(aCRTHeap, HeapCompatibilityInformation, &aHeapInfo, sizeof(aHeapInfo));
    }
#endif

    switch (anAllocId)
    {
      case 1:  // OCCT optimized memory allocator
      {
        aVar = getenv("MMGT_MMAP");
        Standard_Boolean bMMap = (aVar ? (atoi(aVar) != 0) : Standard_True);
        aVar = getenv("MMGT_CELLSIZE");
        Standard_Integer aCellSize = (aVar ? atoi(aVar) : 200);
        aVar = getenv("MMGT_NBPAGES");
        Standard_Integer aNbPages = (aVar ? atoi(aVar) : 1000);
        aVar = getenv("MMGT_THRESHOLD");
        Standard_Integer aThreshold = (aVar ? atoi(aVar) : 40000);
        myFMMgr = new Standard_MMgrOpt(toClear, bMMap, aCellSize, aNbPages, aThreshold);
        break;
      }
      case 2:  // TBB memory allocator
        myFMMgr = new Standard_MMgrTBBalloc(toClear);
        break;
      case 0:
      default: // system default memory allocator
        myFMMgr = new Standard_MMgrRaw(toClear);
    }
    allocatorTypeInstance() = static_cast<Standard::AllocatorType>(anAllocId);
  }

  //=======================================================================
  //function : ~Standard_MMgrFactory
  //purpose  : 
  //=======================================================================

  Standard_MMgrFactory::~Standard_MMgrFactory()
  {
    if (myFMMgr)
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
  // that some other object calling memory manager is not placed also in that segment...
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
}
#endif // OCCT_MMGT_OPT_FLEXIBLE

//=======================================================================
//function : Allocate
//purpose  :
//=======================================================================
Standard::AllocatorType Standard::GetAllocatorType()
{
  return allocatorTypeInstance();
}

//=======================================================================
//function : Allocate
//purpose  :
//=======================================================================
Standard_Address Standard::Allocate(const Standard_Size theSize)
{
#ifdef OCCT_MMGT_OPT_FLEXIBLE
  return Standard_MMgrFactory::GetMMgr()->Allocate(theSize);
#elif defined OCCT_MMGT_OPT_JEMALLOC
  Standard_Address aPtr = je_calloc(theSize, sizeof(char));
  if (!aPtr)
    throw Standard_OutOfMemory("Standard_MMgrRaw::Allocate(): malloc failed");
  return aPtr;
#elif defined OCCT_MMGT_OPT_TBB
  Standard_Address aPtr = scalable_calloc(theSize, sizeof(char));
  if (!aPtr)
    throw Standard_OutOfMemory("Standard_MMgrRaw::Allocate(): malloc failed");
  return aPtr;
#else
  Standard_Address aPtr = calloc(theSize, sizeof(char));
  if (!aPtr)
    throw Standard_OutOfMemory("Standard_MMgrRaw::Allocate(): malloc failed");
  return aPtr;
#endif // OCCT_MMGT_OPT_FLEXIBLE
}

//=======================================================================
//function : AllocateOptimal
//purpose  :
//=======================================================================
Standard_Address Standard::AllocateOptimal(const Standard_Size theSize)
{
#ifdef OCCT_MMGT_OPT_FLEXIBLE
  return Standard_MMgrFactory::GetMMgr()->Allocate(theSize);
#elif defined OCCT_MMGT_OPT_JEMALLOC
  return je_malloc(theSize);
#elif defined OCCT_MMGT_OPT_TBB
  return scalable_malloc(theSize);
#else
  return malloc(theSize);
#endif
}

//=======================================================================
//function : Free
//purpose  :
//=======================================================================
void Standard::Free(Standard_Address theStorage)
{
#ifdef OCCT_MMGT_OPT_FLEXIBLE
  Standard_MMgrFactory::GetMMgr()->Free(theStorage);
#elif defined OCCT_MMGT_OPT_JEMALLOC
  je_free(theStorage);
#elif defined OCCT_MMGT_OPT_TBB
  scalable_free(theStorage);
#else
  free(theStorage);
#endif
}

//=======================================================================
//function : Reallocate
//purpose  :
//=======================================================================
Standard_Address Standard::Reallocate(Standard_Address theStorage,
                                      const Standard_Size theSize)
{
  // Note that it is not possible to ensure that additional memory
  // allocated by realloc will be cleared (so as to satisfy myClear mode);
  // in order to do that we would need using memset..
#ifdef OCCT_MMGT_OPT_FLEXIBLE
  return Standard_MMgrFactory::GetMMgr()->Reallocate(theStorage, theSize);
#elif defined OCCT_MMGT_OPT_JEMALLOC
  Standard_Address aNewStorage = (Standard_Address)je_realloc(theStorage, theSize);
  if (!aNewStorage)
    throw Standard_OutOfMemory("Standard_MMgrRaw::Reallocate(): realloc failed");
  return aNewStorage;
#elif defined OCCT_MMGT_OPT_TBB
  Standard_Address aNewStorage = (Standard_Address)scalable_realloc(theStorage, theSize);
  if (!aNewStorage)
    throw Standard_OutOfMemory("Standard_MMgrRaw::Reallocate(): realloc failed");
  return aNewStorage;
#else
  Standard_Address aNewStorage = (Standard_Address)realloc(theStorage, theSize);
  if (!aNewStorage)
    throw Standard_OutOfMemory("Standard_MMgrRaw::Reallocate(): realloc failed");
  return aNewStorage;
#endif
}

//=======================================================================
//function : Purge
//purpose  :
//=======================================================================
Standard_Integer Standard::Purge()
{
#ifdef OCCT_MMGT_OPT_FLEXIBLE
  return Standard_MMgrFactory::GetMMgr()->Purge();
#else
  return true;
#endif // OCCT_MMGT_OPT_FLEXIBLE
}

//=======================================================================
//function : AllocateAligned
//purpose  :
//=======================================================================
Standard_Address Standard::AllocateAligned(const Standard_Size theSize,
                                           const Standard_Size theAlign)
{
#ifdef OCCT_MMGT_OPT_JEMALLOC
  return je_aligned_alloc(theAlign, theSize);
#elif defined OCCT_MMGT_OPT_TBB
  return scalable_aligned_malloc(theSize, theAlign);
#else
#if defined(_MSC_VER)
  return _aligned_malloc(theSize, theAlign);
#elif defined(__ANDROID__) || defined(__QNX__)
  return memalign(theAlign, theSize);
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) && (defined(__i386) || defined(__x86_64)))
  return _mm_malloc(theSize, theAlign);
#else
  void* aPtr;
  if (posix_memalign(&aPtr, theAlign, theSize))
  {
    return NULL;
  }
  return aPtr;
#endif
#endif
}

//=======================================================================
//function : FreeAligned
//purpose  :
//=======================================================================
void Standard::FreeAligned(Standard_Address thePtrAligned)
{
#ifdef OCCT_MMGT_OPT_JEMALLOC
return je_free(thePtrAligned);
#elif defined OCCT_MMGT_OPT_TBB
  return scalable_aligned_free(thePtrAligned);
#else
#if defined(_MSC_VER)
  _aligned_free(thePtrAligned);
#elif defined(__ANDROID__) || defined(__QNX__)
  free(thePtrAligned);
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) && (defined(__i386) || defined(__x86_64)))
  _mm_free(thePtrAligned);
#else
  free(thePtrAligned);
#endif
#endif
}

// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _Standard_DefineAlloc_HeaderFile
#define _Standard_DefineAlloc_HeaderFile

//! @file Standard_DefineAlloc.hxx
//! @brief Macros for overriding memory allocation operators using OCCT memory manager.
//!
//! This header provides macros to override new/delete operators in classes:
//! - DEFINE_STANDARD_ALLOC - uses Standard::Allocate (exact size allocation)
//! - DEFINE_STANDARD_OPT_ALLOC - uses Standard::AllocateOptimal (rounded size for reuse)

//! @def DEFINE_STANDARD_ALLOC_ARRAY
//! @brief Override new[] and delete[] operators for arrays.
#define DEFINE_STANDARD_ALLOC_ARRAY                                                                \
  void* operator new[](size_t theSize)                                                             \
  {                                                                                                \
    return Standard::Allocate(theSize);                                                            \
  }                                                                                                \
  void operator delete[](void* theAddress) noexcept                                                \
  {                                                                                                \
    Standard::Free(theAddress);                                                                    \
  }

//! @def DEFINE_STANDARD_ALLOC_PLACEMENT
//! @brief Override placement new and placement delete operators.
#define DEFINE_STANDARD_ALLOC_PLACEMENT                                                            \
  void* operator new(size_t, void* theAddress) noexcept                                            \
  {                                                                                                \
    return theAddress;                                                                             \
  }                                                                                                \
  void operator delete(void*, void*) noexcept {}

//! @def DEFINE_STANDARD_ALLOC
//! @brief Override operators new and delete to use OCCT memory manager.
//! Uses Standard::Allocate which allocates exact requested size.
#define DEFINE_STANDARD_ALLOC                                                                      \
  void* operator new(size_t theSize)                                                               \
  {                                                                                                \
    return Standard::Allocate(theSize);                                                            \
  }                                                                                                \
  void operator delete(void* theAddress) noexcept                                                  \
  {                                                                                                \
    Standard::Free(theAddress);                                                                    \
  }                                                                                                \
  DEFINE_STANDARD_ALLOC_ARRAY                                                                      \
  DEFINE_STANDARD_ALLOC_PLACEMENT

//! @def DEFINE_STANDARD_OPT_ALLOC_ARRAY
//! @brief Override new[] and delete[] operators for arrays using optimal allocation.
#define DEFINE_STANDARD_OPT_ALLOC_ARRAY                                                            \
  void* operator new[](size_t theSize)                                                             \
  {                                                                                                \
    return Standard::AllocateOptimal(theSize);                                                     \
  }                                                                                                \
  void operator delete[](void* theAddress) noexcept                                                \
  {                                                                                                \
    Standard::Free(theAddress);                                                                    \
  }

//! @def DEFINE_STANDARD_OPT_ALLOC
//! @brief Override operators new and delete to use OCCT memory manager with optimal allocation.
//! Uses Standard::AllocateOptimal which rounds up size for better memory reuse.
//! Recommended for frequently allocated/deallocated objects.
#define DEFINE_STANDARD_OPT_ALLOC                                                                  \
  void* operator new(size_t theSize)                                                               \
  {                                                                                                \
    return Standard::AllocateOptimal(theSize);                                                     \
  }                                                                                                \
  void operator delete(void* theAddress) noexcept                                                  \
  {                                                                                                \
    Standard::Free(theAddress);                                                                    \
  }                                                                                                \
  DEFINE_STANDARD_OPT_ALLOC_ARRAY                                                                  \
  DEFINE_STANDARD_ALLOC_PLACEMENT

//! @def STANDARD_ALIGNED(theAlignment, theType, theVar)
//! @brief Declare variable with memory alignment.
//! @code
//!   static const STANDARD_ALIGNED(8, char, THE_ARRAY)[] = {0xFF, 0xFE, 0xFA, 0xFB, 0xFF, 0x11};
//! @endcode
#define STANDARD_ALIGNED(theAlignment, theType, theVar) alignas(theAlignment) theType theVar

#endif // _Standard_DefineAlloc_HeaderFile

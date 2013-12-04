// Created on: 2014-01-19
// Created by: Dmitry BOBYLEV 
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 2014-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Standard_DefineAlloc_HeaderFile
# define _Standard_DefineAlloc_HeaderFile

// Macro to override new and delete operators for arrays.
// Defined to empty for old SUN compiler
# if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
#  define DEFINE_STANDARD_ALLOC_ARRAY
# else
#  define DEFINE_STANDARD_ALLOC_ARRAY                                  \
   void* operator new[] (size_t theSize)                               \
   {                                                                   \
     return Standard::Allocate (theSize);                              \
   }                                                                   \
   void  operator delete[] (void* theAddress)                          \
   {                                                                   \
     Standard::Free (theAddress);                                      \
   }
# endif

// Macro to override placement new and placement delete operators. 
// For Borland C and old SUN compilers do not define placement delete
// as it is not supported.
# if defined(__BORLANDC__) || (defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530))
#  define DEFINE_STANDARD_ALLOC_PLACEMENT                              \
   void* operator new (size_t, void* theAddress)                       \
   {                                                                   \
     return theAddress;                                                \
   }
# else 
#  define DEFINE_STANDARD_ALLOC_PLACEMENT                              \
   void* operator new (size_t, void* theAddress)                       \
   {                                                                   \
     return theAddress;                                                \
   }                                                                   \
   void operator delete (void*, void*)                                 \
   {                                                                   \
   }
# endif

// Macro to override operators new and delete to use OCC memory manager
# define DEFINE_STANDARD_ALLOC                                         \
  void* operator new (size_t theSize)                                  \
  {                                                                    \
    return Standard::Allocate (theSize);                               \
  }                                                                    \
  void  operator delete (void* theAddress)                             \
  {                                                                    \
    Standard::Free ((Standard_Address&)theAddress);                    \
  }                                                                    \
  DEFINE_STANDARD_ALLOC_ARRAY                                          \
  DEFINE_STANDARD_ALLOC_PLACEMENT

// Declare operator new in global scope for old sun compiler
#ifndef WORKAROUND_SUNPRO_NEW_PLACEMENT
#define WORKAROUND_SUNPRO_NEW_PLACEMENT
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x420)
inline void* operator new(size_t,void* anAddress) 
{
  return anAddress;
}
#endif
#endif

#endif

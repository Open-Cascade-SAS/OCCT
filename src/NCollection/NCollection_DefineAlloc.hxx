// Created on: 2012-01-19
// Created by: Dmitry BOBYLEV 
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 2012-2012 OPEN CASCADE SAS
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

#ifndef _NCollection_DefineAlloc_HeaderFile
# define _NCollection_DefineAlloc_HeaderFile

#include <NCollection_BaseAllocator.hxx>

// Macro to overload placement new and delete operators for NCollection allocators.
// For Borland C and old SUN compilers do not define placement delete
// as it is not supported.
# if defined(__BORLANDC__) || (defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530))
#  define DEFINE_NCOLLECTION_ALLOC                                               \
   void* operator new (size_t theSize,                                           \
                       const Handle(NCollection_BaseAllocator)& theAllocator)    \
   {                                                                             \
     return theAllocator->Allocate(theSize);                                     \
   }
# else
#  define DEFINE_NCOLLECTION_ALLOC                                               \
   void* operator new (size_t theSize,                                           \
                       const Handle(NCollection_BaseAllocator)& theAllocator)    \
   {                                                                             \
     return theAllocator->Allocate(theSize);                                     \
   }                                                                             \
   void  operator delete (void* theAddress,                                      \
                          const Handle(NCollection_BaseAllocator)& theAllocator) \
   {                                                                             \
     theAllocator->Free(theAddress);                                             \
   }
# endif

#endif

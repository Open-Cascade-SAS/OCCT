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

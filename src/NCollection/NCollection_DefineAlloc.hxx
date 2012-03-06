// File:        Standard_DefineAlloc.hxx
// Created:     Jan 19 14:15:16 2012
// Author:      Dmitry BOBYLEV 
// Copyright:   Open CASCADE SAS 2012

#ifndef _NCollection_DefineAlloc_HeaderFile
# define _NCollection_DefineAlloc_HeaderFile

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
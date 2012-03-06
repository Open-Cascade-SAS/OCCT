// File:        Standard_DefineAlloc.hxx
// Created:     Jan 19 10:33:16 2012
// Author:      Dmitry BOBYLEV 
// Copyright:   Open CASCADE SAS 2012

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
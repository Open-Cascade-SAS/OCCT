// Created on: 1991-09-05
// Created by: J.P. TIRAUlt
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Standard_HeaderFile
#define _Standard_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Address.hxx>
#include <Standard_Size.hxx>
#include <Standard_Integer.hxx>

class Standard_ErrorHandler;
class Standard_Persistent;
class Standard_Transient;
class Standard_Failure;

//! The package Standard provides global memory allocator and other basic
//! services used by other OCCT components.

class Standard 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Allocates memory blocks
  //! aSize - bytes to  allocate
  Standard_EXPORT static Standard_Address Allocate (const Standard_Size aSize);
  
  //! Deallocates memory blocks
  //! @param thePtr - previously allocated memory block to be freed
  Standard_EXPORT static void Free (const Standard_Address thePtr);
  
  //! Template version of function Free(), nullifies the argument pointer
  //! @param thePtr - previously allocated memory block to be freed
  template <typename T>
  static inline void Free (T*& thePtr) 
  { 
    Free ((void*)thePtr);
    thePtr = 0;
  }
  
  //! Reallocates memory blocks
  //! aStorage - previously allocated memory block
  //! aNewSize - new size in bytes
  Standard_EXPORT static Standard_Address Reallocate (const Standard_Address aStorage, const Standard_Size aNewSize);
  
  //! Allocates aligned memory blocks.
  //! Should be used with CPU instructions which require specific alignment.
  //! For example: SSE requires 16 bytes, AVX requires 32 bytes.
  //! @param theSize  bytes to allocate
  //! @param theAlign alignment in bytes
  Standard_EXPORT static Standard_Address AllocateAligned (const Standard_Size theSize, const Standard_Size theAlign);
  
  //! Deallocates memory blocks
  //! @param thePtrAligned the memory block previously allocated with AllocateAligned()
  Standard_EXPORT static void FreeAligned (const Standard_Address thePtrAligned);
  
  //! Template version of function FreeAligned(), nullifies the argument pointer
  //! @param thePtrAligned the memory block previously allocated with AllocateAligned()
  template <typename T>
  static inline void FreeAligned (T*& thePtrAligned)
  {
    FreeAligned ((void* )thePtrAligned);
    thePtrAligned = 0;
  }
  
  //! Deallocates the storage retained on the free list
  //! and clears the list.
  //! Returns non-zero if some memory has been actually freed.
  Standard_EXPORT static Standard_Integer Purge();

  //! Appends backtrace to a message buffer.
  //! Stack information might be incomplete in case of stripped binaries.
  //! Implementation details:
  //! - Not implemented for Android, iOS, QNX and UWP platforms.
  //! - On non-Windows platform, this function is a wrapper to backtrace() system call.
  //! - On Windows (Win32) platform, the function loads DbgHelp.dll dynamically,
  //!   and no stack will be provided if this or companion libraries (SymSrv.dll, SrcSrv.dll, etc.) will not be found;
  //!   .pdb symbols should be provided on Windows platform to retrieve a meaningful stack;
  //!   only x86_64 CPU architecture is currently implemented.
  //! @param theBuffer [in] [out] message buffer to extend
  //! @param theBufferSize [in] message buffer size
  //! @param theNbTraces [in] maximum number of stack traces
  //! @param theContext [in] optional platform-dependent frame context;
  //!                        in case of DbgHelp (Windows) should be a pointer to CONTEXT
  //! @param theNbTopSkip [in] number of traces on top of the stack to skip
  //! @return TRUE on success
  Standard_EXPORT static Standard_Boolean StackTrace (char* theBuffer,
                                                      const int theBufferSize,
                                                      const int theNbTraces,
                                                      void* theContext = NULL,
                                                      const int theNbTopSkip = 0);

};

// include definition of handle to make it always visible
// (put at the and of the file due to cyclic dependency between headers)
#include <Standard_Transient.hxx>

#endif // _Standard_HeaderFile

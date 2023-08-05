// Created on: 2011-07-11
// Created by: Kirill GAVRILOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef NCollection_WinHeapAllocator_HeaderFile
#define NCollection_WinHeapAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>

//! This memory allocator creates dedicated heap for allocations.
//! This technics available only on Windows platform
//! (no alternative on Unix systems).
//! It may be used to take control over memory fragmentation
//! because on destruction ALL allocated memory will be released
//! to the system.
//!
//! This allocator can also be created per each working thread
//! hovewer it real multi-threading performance is dubious.
//!
//! Notice that this also means that existing pointers will be broken
//! and you shoould control that allocator is alive along all objects
//! allocated with him.
class NCollection_WinHeapAllocator : public NCollection_BaseAllocator
{
public:

  //! Main constructor
  Standard_EXPORT NCollection_WinHeapAllocator (const size_t theInitSizeBytes = 0x80000);

  //! Destructor
  Standard_EXPORT virtual ~NCollection_WinHeapAllocator();

  //! Allocate memory
  Standard_EXPORT virtual void* Allocate (const Standard_Size theSize) Standard_OVERRIDE;

  //! Allocate memory
  void* AllocateOptimal(const Standard_Size theSize) Standard_OVERRIDE { return Allocate(theSize); }

  //! Release memory
  Standard_EXPORT virtual void  Free (void* theAddress) Standard_OVERRIDE;

  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(NCollection_WinHeapAllocator,NCollection_BaseAllocator)

private:
  //! Copy constructor - prohibited
  NCollection_WinHeapAllocator (const NCollection_WinHeapAllocator& );

private:
#if(defined(_WIN32) || defined(__WIN32__))
  void* myHeapH;
#endif
  Standard_Boolean myToZeroMemory;

};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NCollection_WinHeapAllocator, NCollection_BaseAllocator)

#endif //NCollection_WinHeapAllocator_HeaderFile

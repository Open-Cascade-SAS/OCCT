// Created on: 2016-06-20
// Created by: Oleg AGASHIN
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _BRepMesh_IncAllocator_HeaderFile
#define _BRepMesh_IncAllocator_HeaderFile

#include <NCollection_IncAllocator.hxx>
#include <Standard_Mutex.hxx>

//! Extension for NCollection_IncAllocator implementing simple thread safety
//! by introduction of Mutex. Intended for use in couple with BRepMeshData
//! entities in order to prevent data races while building data model in
//! parallel mode. Note that this allocator is supposed for use by collections
//! which allocate memory by huge blocks at arbitrary moment, thus it should
//! not introduce significant performance slow down.
class BRepMesh_IncAllocator : public NCollection_IncAllocator
{
public:
  //! Constructor
  BRepMesh_IncAllocator(const size_t theBlockSize = DefaultBlockSize)
    : NCollection_IncAllocator(theBlockSize)
  {
  }

  //! Allocate memory with given size. Returns NULL on failure
  virtual void* Allocate(const size_t size) Standard_OVERRIDE
  {
    Standard_Mutex::Sentry aSentry(myMutex);
    return NCollection_IncAllocator::Allocate(size);
  }

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_IncAllocator, NCollection_IncAllocator)

private:
  Standard_Mutex myMutex;
};

#endif

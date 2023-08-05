// Created on: 2014-03-31
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef NCollection_AlignedAllocator_HeaderFile
#define NCollection_AlignedAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <Standard.hxx>

//! NCollection allocator with managed memory alignment capabilities.
class NCollection_AlignedAllocator : public NCollection_BaseAllocator
{
public:

  //! Constructor. The alignment should be specified explicitly:
  //! 16 bytes for SSE instructions
  //! 32 bytes for AVX instructions
  Standard_EXPORT NCollection_AlignedAllocator (const size_t theAlignment);

  //! Allocate memory with given size. Returns NULL on failure.
  Standard_EXPORT virtual void* Allocate (const size_t theSize) Standard_OVERRIDE;

  //! Allocate memory with given size. Returns NULL on failure.
  void* AllocateOptimal(const size_t theSize) Standard_OVERRIDE
  {
    return Allocate(theSize);
  }

  //! Free a previously allocated memory.
  Standard_EXPORT virtual void  Free (void* thePtr) Standard_OVERRIDE;

private:

  NCollection_AlignedAllocator            (const NCollection_AlignedAllocator& );
  NCollection_AlignedAllocator& operator= (const NCollection_AlignedAllocator& );

protected:

  size_t myAlignment; //!< alignment in bytes

public:

  DEFINE_STANDARD_RTTIEXT(NCollection_AlignedAllocator,NCollection_BaseAllocator)

};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NCollection_AlignedAllocator, NCollection_BaseAllocator)

#endif // NCollection_AlignedAllocator_HeaderFile

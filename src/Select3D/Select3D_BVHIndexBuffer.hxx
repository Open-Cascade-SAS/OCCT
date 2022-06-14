// Created on: 2016-02-25
// Created by: Kirill Gavrilov
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

#ifndef _Select3D_BVHIndexBuffer_Header
#define _Select3D_BVHIndexBuffer_Header

#include <Graphic3d_Buffer.hxx>

//! Index buffer for BVH tree.
class Select3D_BVHIndexBuffer : public Graphic3d_Buffer
{
public:

  //! Empty constructor.
  Select3D_BVHIndexBuffer (const Handle(NCollection_BaseAllocator)& theAlloc)
  : Graphic3d_Buffer (theAlloc), myHasPatches (false) {}

  bool HasPatches() const { return myHasPatches; }

  //! Allocates new empty index array
  bool Init (const Standard_Integer theNbElems,
             const bool theHasPatches)
  {
    release();
    Stride = sizeof(unsigned int);
    myHasPatches = theHasPatches;
    if (theHasPatches)
    {
      Stride += sizeof(unsigned int);
    }

    NbElements   = theNbElems;
    NbAttributes = 0;
    if (NbElements != 0
    && !Allocate (size_t(Stride) * size_t(NbElements)))
    {
      release();
      return false;
    }
    return true;
  }

  //! Access index at specified position
  Standard_Integer Index (const Standard_Integer theIndex) const
  {
    return Standard_Integer(*reinterpret_cast<const unsigned int* >(value (theIndex)));
  }

  //! Access index at specified position
  Standard_Integer PatchSize (const Standard_Integer theIndex) const
  {
    return myHasPatches
         ? Standard_Integer(*reinterpret_cast<const unsigned int* >(value (theIndex) + sizeof(unsigned int)))
         : 1;
  }

  //! Change index at specified position
  void SetIndex (const Standard_Integer theIndex,
                 const Standard_Integer theValue)
  {
    *reinterpret_cast<unsigned int* >(changeValue (theIndex)) = (unsigned int )theValue;
  }

  //! Change index at specified position
  void SetIndex (const Standard_Integer theIndex,
                 const Standard_Integer theValue,
                 const Standard_Integer thePatchSize)
  {
    *reinterpret_cast<unsigned int* >(changeValue (theIndex))                        = (unsigned int )theValue;
    *reinterpret_cast<unsigned int* >(changeValue (theIndex) + sizeof(unsigned int)) = (unsigned int )thePatchSize;
  }

private:

  bool myHasPatches;
  
public:

  DEFINE_STANDARD_RTTI_INLINE(Select3D_BVHIndexBuffer,Graphic3d_Buffer)

};

DEFINE_STANDARD_HANDLE(Select3D_BVHIndexBuffer, Graphic3d_Buffer)

#endif // _Select3D_BVHIndexBuffer_Header

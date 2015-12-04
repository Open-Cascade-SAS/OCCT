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

#ifndef _Graphic3d_BoundBuffer_HeaderFile
#define _Graphic3d_BoundBuffer_HeaderFile

#include <Graphic3d_Buffer.hxx>

//! Bounds buffer.
class Graphic3d_BoundBuffer : public NCollection_Buffer
{
public:

  //! Empty constructor.
  Graphic3d_BoundBuffer (const Handle(NCollection_BaseAllocator)& theAlloc)
  : NCollection_Buffer (theAlloc),
    Colors   (NULL),
    Bounds   (NULL),
    NbBounds (0) {}

  //! Allocates new empty array
  bool Init (const Standard_Integer theNbBounds,
             const Standard_Boolean theHasColors)
  {
    Colors   = NULL;
    Bounds   = NULL;
    NbBounds = 0;
    Free();
    if (theNbBounds < 1)
    {
      return false;
    }

    const size_t aBoundsSize = sizeof(Standard_Integer) * theNbBounds;
    const size_t aColorsSize = theHasColors
                             ? sizeof(Graphic3d_Vec4) * theNbBounds
                             : 0;
    if (!Allocate (aColorsSize + aBoundsSize))
    {
      Free();
      return false;
    }

    NbBounds = theNbBounds;
    Colors   = theHasColors ? reinterpret_cast<Graphic3d_Vec4* >(myData) : NULL;
    Bounds   = reinterpret_cast<Standard_Integer* >(theHasColors ? (myData + aColorsSize) : myData);
    return true;
  }

public:

  Graphic3d_Vec4*   Colors;   //!< pointer to facet color values
  Standard_Integer* Bounds;   //!< pointer to bounds array
  Standard_Integer  NbBounds; //!< number of bounds

public:

  DEFINE_STANDARD_RTTI_INLINE(Graphic3d_BoundBuffer,NCollection_Buffer) // Type definition

};

DEFINE_STANDARD_HANDLE(Graphic3d_BoundBuffer, NCollection_Buffer)

#endif // _Graphic3d_BoundBuffer_HeaderFile

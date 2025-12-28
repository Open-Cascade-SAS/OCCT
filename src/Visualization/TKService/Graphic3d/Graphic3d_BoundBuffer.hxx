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
  DEFINE_STANDARD_RTTIEXT(Graphic3d_BoundBuffer, NCollection_Buffer)
public:
  //! Empty constructor.
  Graphic3d_BoundBuffer(const occ::handle<NCollection_BaseAllocator>& theAlloc)
      : NCollection_Buffer(theAlloc),
        Colors(NULL),
        Bounds(NULL),
        NbBounds(0),
        NbMaxBounds(0)
  {
  }

  //! Allocates new empty array
  bool Init(const int theNbBounds, const bool theHasColors)
  {
    Colors      = NULL;
    Bounds      = NULL;
    NbBounds    = 0;
    NbMaxBounds = 0;
    Free();
    if (theNbBounds < 1)
    {
      return false;
    }

    const size_t aBoundsSize = sizeof(int) * theNbBounds;
    const size_t aColorsSize = theHasColors ? sizeof(NCollection_Vec4<float>) * theNbBounds : 0;
    if (!Allocate(aColorsSize + aBoundsSize))
    {
      Free();
      return false;
    }

    NbBounds    = theNbBounds;
    NbMaxBounds = theNbBounds;
    Colors      = theHasColors ? reinterpret_cast<NCollection_Vec4<float>*>(myData) : NULL;
    Bounds      = reinterpret_cast<int*>(theHasColors ? (myData + aColorsSize) : myData);
    return true;
  }

  //! Dumps the content of me into the stream
  virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override
  {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
    OCCT_DUMP_BASE_CLASS(theOStream, theDepth, NCollection_Buffer)

    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, Colors)
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, Bounds)

    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, NbBounds)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, NbMaxBounds)
  }

public:
  NCollection_Vec4<float>* Colors;      //!< pointer to facet color values
  int*                     Bounds;      //!< pointer to bounds array
  int                      NbBounds;    //!< number of bounds
  int                      NbMaxBounds; //!< number of allocated bounds
};

#endif // _Graphic3d_BoundBuffer_HeaderFile

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

#ifndef _Graphic3d_IndexBuffer_HeaderFile
#define _Graphic3d_IndexBuffer_HeaderFile

#include <Graphic3d_Buffer.hxx>

//! Index buffer.
class Graphic3d_IndexBuffer : public Graphic3d_Buffer
{
public:

  //! Empty constructor.
  Graphic3d_IndexBuffer (const Handle(NCollection_BaseAllocator)& theAlloc)
  : Graphic3d_Buffer (theAlloc) {}

  //! Allocates new empty index array
  template<typename IndexType_t>
  bool Init (const Standard_Integer theNbElems)
  {
    release();
    Stride = sizeof(IndexType_t);
    if (Stride != sizeof(unsigned short)
     && Stride != sizeof(unsigned int))
    {
      return false;
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
    return Stride == sizeof(unsigned short)
         ? Standard_Integer(Value<unsigned short> (theIndex))
         : Standard_Integer(Value<unsigned int>   (theIndex));
  }

  //! Change index at specified position
  void SetIndex (const Standard_Integer theIndex,
                 const Standard_Integer theValue)
  {
    if (Stride == sizeof(unsigned short))
    {
      ChangeValue<unsigned short> (theIndex) = (unsigned short )theValue;
    }
    else
    {
      ChangeValue<unsigned int>   (theIndex) = (unsigned int   )theValue;
    }
  }

public:

  DEFINE_STANDARD_RTTI(Graphic3d_IndexBuffer) // Type definition

};

DEFINE_STANDARD_HANDLE(Graphic3d_IndexBuffer, Graphic3d_Buffer)

#endif // _Graphic3d_IndexBuffer_HeaderFile

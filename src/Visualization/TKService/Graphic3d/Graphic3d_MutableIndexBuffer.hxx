// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _Graphic3d_MutableIndexBuffer_HeaderFile
#define _Graphic3d_MutableIndexBuffer_HeaderFile

#include <Graphic3d_IndexBuffer.hxx>

//! Mutable index buffer.
class Graphic3d_MutableIndexBuffer : public Graphic3d_IndexBuffer
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_MutableIndexBuffer, Graphic3d_IndexBuffer)
public:
  //! Empty constructor.
  Graphic3d_MutableIndexBuffer(const Handle(NCollection_BaseAllocator)& theAlloc)
      : Graphic3d_IndexBuffer(theAlloc)
  {
  }

  //! Return TRUE if data can be invalidated.
  virtual Standard_Boolean IsMutable() const Standard_OVERRIDE { return Standard_True; }

  //! Return invalidated range.
  virtual Graphic3d_BufferRange InvalidatedRange() const Standard_OVERRIDE
  {
    return myInvalidatedRange;
  }

  //! Reset invalidated range.
  virtual void Validate() Standard_OVERRIDE { myInvalidatedRange.Clear(); }

  //! Invalidate the entire buffer data.
  virtual void Invalidate() Standard_OVERRIDE
  {
    invalidate(Graphic3d_BufferRange(0, (Standard_Integer)mySize));
  }

  //! Invalidate the given indexes (starting from 0)
  void Invalidate(Standard_Integer theIndexLower, Standard_Integer theIndexUpper)
  {
    Standard_OutOfRange_Raise_if(theIndexLower > theIndexUpper,
                                 "Graphic3d_MutableIndexBuffer::Invalidate()");
    invalidate(
      Graphic3d_BufferRange(Stride * theIndexLower, Stride * (theIndexUpper - theIndexLower + 1)));
  }

  //! Invalidate specified sub-range of data (as byte offsets).
  void invalidate(const Graphic3d_BufferRange& theRange) { myInvalidatedRange.Unite(theRange); }

protected:
  Graphic3d_BufferRange myInvalidatedRange; //!< invalidated buffer data range (as byte offsets)
};

#endif // _Graphic3d_MutableIndexBuffer_HeaderFile

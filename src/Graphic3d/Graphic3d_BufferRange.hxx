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

#ifndef _Graphic3d_BufferRange_HeaderFile
#define _Graphic3d_BufferRange_HeaderFile

#include <Graphic3d_Vec.hxx>
#include <Standard_Integer.hxx>

//! Range of values defined as Start + Length pair.
struct Graphic3d_BufferRange
{
  Standard_Integer Start;  //!< first element within the range
  Standard_Integer Length; //!< number of elements within the range

  //! Empty constructor.
  Graphic3d_BufferRange() : Start (0), Length (0) {}

  //! Constructor.
  Graphic3d_BufferRange (Standard_Integer theStart, Standard_Integer theLength) : Start (theStart), Length (theLength) {}

  //! Return TRUE if range is empty.
  Standard_Boolean IsEmpty() const { return Length == 0; }

  //! Return the Upper element within the range
  Standard_Integer Upper() const { return Start + Length - 1; }

  //! Clear the range.
  void Clear()
  {
    Start  = 0;
    Length = 0;
  }

  //! Add another range to this one.
  void Unite (const Graphic3d_BufferRange& theRange)
  {
    if (IsEmpty())
    {
      *this = theRange;
      return;
    }
    else if (theRange.IsEmpty())
    {
      return;
    }

    const Standard_Integer aStart = Min (Start,   theRange.Start);
    const Standard_Integer aLast  = Max (Upper(), theRange.Upper());
    Start  = aStart;
    Length = aLast - aStart + 1;
  }
};

#endif // _Graphic3d_BufferRange_HeaderFile

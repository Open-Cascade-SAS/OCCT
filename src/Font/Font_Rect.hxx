// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2015 OPEN CASCADE SAS
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

#ifndef _Font_Rect_H__
#define _Font_Rect_H__

#include <NCollection_Vec2.hxx>
#include <Standard_Dump.hxx>

//! Auxiliary POD structure - 2D rectangle definition.
struct Font_Rect
{

  float Left;   //!< left   position
  float Right;  //!< right  position
  float Top;    //!< top    position
  float Bottom; //!< bottom position

  //! Top-left corner as vec2.
  NCollection_Vec2<float> TopLeft() const
  {
    return NCollection_Vec2<float> (Left, Top);
  }

  //! Top-left corner as vec2.
  NCollection_Vec2<float>& TopLeft (NCollection_Vec2<float>& theVec) const
  {
    theVec.x() = Left;
    theVec.y() = Top;
    return theVec;
  }

  //! Top-right corner as vec2.
  NCollection_Vec2<float>& TopRight (NCollection_Vec2<float>& theVec) const
  {
    theVec.x() = Right;
    theVec.y() = Top;
    return theVec;
  }

  //! Bottom-left corner as vec2.
  NCollection_Vec2<float>& BottomLeft (NCollection_Vec2<float>& theVec) const
  {
    theVec.x() = Left;
    theVec.y() = Bottom;
    return theVec;
  }

  //! Bottom-right corner as vec2.
  NCollection_Vec2<float>& BottomRight (NCollection_Vec2<float>& theVec) const
  {
    theVec.x() = Right;
    theVec.y() = Bottom;
    return theVec;
  }

  //! Rectangle width.
  float Width() const
  {
    return Right - Left;
  }

  //! Rectangle height.
  float Height() const
  {
    return Top - Bottom;
  }

  //! Dumps the content of me into the stream
  void DumpJson (Standard_OStream& theOStream, Standard_Integer) const
  {
    OCCT_DUMP_CLASS_BEGIN (theOStream, Font_Rect)

    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Left)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Right)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Top)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Bottom)
  }
};

#endif // _Font_Rect_H__

// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Aspect_ScrollDelta_HeaderFile
#define _Aspect_ScrollDelta_HeaderFile

#include <Aspect_VKeyFlags.hxx>
#include <NCollection_Vec2.hxx>

//! Parameters for mouse scroll action.
struct Aspect_ScrollDelta
{

  NCollection_Vec2<int> Point; //!< scale position
  Standard_Real         Delta; //!< delta in pixels
  Aspect_VKeyFlags      Flags; //!< key flags

  //! Return true if action has point defined.
  bool HasPoint() const
  {
    return Point.x() >= 0
        && Point.y() >= 0;
  }

  //! Reset at point.
  void ResetPoint()
  {
    Point.SetValues (-1, -1);
  }

  //! Empty constructor.
  Aspect_ScrollDelta()
  : Point (-1, -1), Delta (0.0), Flags (Aspect_VKeyFlags_NONE) {}

  //! Constructor.
  Aspect_ScrollDelta (const NCollection_Vec2<int>& thePnt,
                      Standard_Real theValue,
                      Aspect_VKeyFlags theFlags = Aspect_VKeyFlags_NONE)
  : Point (thePnt), Delta (theValue), Flags (theFlags) {}

  //! Constructor with undefined point.
  Aspect_ScrollDelta (Standard_Real theValue,
                      Aspect_VKeyFlags theFlags = Aspect_VKeyFlags_NONE)
  : Point (-1, -1), Delta (theValue), Flags (theFlags) {}

};

#endif // _Aspect_ScrollDelta_HeaderFile

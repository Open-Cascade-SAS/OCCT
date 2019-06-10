// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _Aspect_Touch_HeaderFile
#define _Aspect_Touch_HeaderFile

#include <NCollection_Vec2.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>

//! Structure holding touch position - original and current location.
class Aspect_Touch
{
public:

  NCollection_Vec2<Standard_Real> From;            //!< original touch position
  NCollection_Vec2<Standard_Real> To;              //!< current  touch position
  Standard_Boolean                IsPreciseDevice; //!< precise device input (e.g. mouse cursor, NOT emulated from touch screen)

  //! Return values delta.
  NCollection_Vec2<Standard_Real> Delta() const { return To - From; }

  //! Empty constructor
  Aspect_Touch()
  : From (0.0, 0.0), To (0.0, 0.0), IsPreciseDevice (false) {}

  //! Constructor with initialization.
  Aspect_Touch (const NCollection_Vec2<Standard_Real>& thePnt,
                Standard_Boolean theIsPreciseDevice)
  : From (thePnt), To (thePnt), IsPreciseDevice (theIsPreciseDevice) {}

  //! Constructor with initialization.
  Aspect_Touch (Standard_Real theX, Standard_Real theY,
                Standard_Boolean theIsPreciseDevice)
  : From (theX, theY), To (theX, theY), IsPreciseDevice (theIsPreciseDevice) {}

};

#endif // _Aspect_Touch_HeaderFile

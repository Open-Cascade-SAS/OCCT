// Created on: 2013-09-04
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _SelectBasics_PickArgs_HeaderFile
#define _SelectBasics_PickArgs_HeaderFile

#include <Standard_TypeDef.hxx>
#include <gp_Lin.hxx>

//! Structure to provide all-in-one information on picking arguments
//! for "Matches" method of SelectBasics_SensitiveEntity.
struct SelectBasics_PickArgs
{
public:

  //! Constructor.
  //! @param theX mouse picking coordinate on x-axis of selection coord space.
  //! @param theY mouse picking coordinate on y-axis of selection coord space.
  //! @param theTolerance x, y coordinate tolerance.
  //! @param theDepthMin minimum picking depth in selection coord space.
  //! @param theDepthMax maximum picking depth in selection coord space.
  //! @param thePickingLine line going through picking point.
  SelectBasics_PickArgs (const Standard_Real theX,
                         const Standard_Real theY,
                         const Standard_Real theTolerance,
                         const Standard_Real theDepthMin,
                         const Standard_Real theDepthMax,
                         const gp_Lin& thePickingLine)
  : myX (theX), myY (theY), myTolerance (theTolerance),
    myDepthMin (theDepthMin), myDepthMax (theDepthMax),
    myPickingLine (thePickingLine) {}

public:

  inline Standard_Real X() const { return myX; }

  inline Standard_Real Y() const { return myY; }

  inline Standard_Real Tolerance() const { return myTolerance; }

  inline Standard_Real DepthMin() const { return myDepthMin; }

  inline Standard_Real DepthMax() const { return myDepthMax; }

  inline const gp_Lin& PickLine() const { return myPickingLine; }

  //! @return True if passed depth lies outside valid depth range.
  inline Standard_Boolean IsClipped(const Standard_Real theDepth) const
  {
    return (theDepth <= myDepthMin || theDepth >= myDepthMax);
  }

private:

  Standard_Real myX;         //!< mouse picking coordinate on x-axis of selection coord space.
  Standard_Real myY;         //!< mouse picking coordinate on y-axis of selection coord space.
  Standard_Real myTolerance; //!< x, y coordinate tolerance
  Standard_Real myDepthMin;  //!< minimum picking depth in selection coord space.
  Standard_Real myDepthMax;  //!< maximum picking depth in selection coord space.
  gp_Lin myPickingLine;      //!< line going through picking point
};

#endif

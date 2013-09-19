// Created on: 2013-09-04
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 65 (the "License") You may not use the content of this file
// except in compliance with the License Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file
//
// The Initial Developer of the Original Code is Open CASCADE SAS, having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement Please see the License for the specific terms
// and conditions governing the rights and limitations under the License

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

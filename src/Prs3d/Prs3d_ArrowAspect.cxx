// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Prs3d_ArrowAspect.hxx>

#include <Prs3d_InvalidAngle.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_ArrowAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_ArrowAspect
// purpose  :
// =======================================================================
Prs3d_ArrowAspect::Prs3d_ArrowAspect()
: myArrowAspect (new Graphic3d_AspectLine3d (Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0)),
  myAngle (M_PI / 180.0 * 10.0),
  myLength(1.0)
{
  //
}

// =======================================================================
// function : Prs3d_ArrowAspect
// purpose  :
// =======================================================================
Prs3d_ArrowAspect::Prs3d_ArrowAspect (const Standard_Real theAngle,
                                      const Standard_Real theLength)
: myArrowAspect (new Graphic3d_AspectLine3d (Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0)),
  myAngle (theAngle),
  myLength(theLength)
{
  //
}

// =======================================================================
// function : Prs3d_ArrowAspect
// purpose  :
// =======================================================================
Prs3d_ArrowAspect::Prs3d_ArrowAspect (const Handle(Graphic3d_AspectLine3d)& theAspect)
: myArrowAspect (theAspect),
  myAngle (M_PI / 180.0 * 10.0),
  myLength(1.0)
{
  //
}

// =======================================================================
// function : SetAngle
// purpose  :
// =======================================================================
void Prs3d_ArrowAspect::SetAngle (const Standard_Real theAngle)
{
  Prs3d_InvalidAngle_Raise_if (theAngle <= 0.0
                            || theAngle >= M_PI / 2.0, "Prs3d_ArrowAspect::SetAngle() - angle out of range");
  myAngle = theAngle;
}

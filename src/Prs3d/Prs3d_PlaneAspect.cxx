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

#include <Prs3d_PlaneAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_PlaneAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_PlaneAspect
// purpose  :
// =======================================================================
Prs3d_PlaneAspect::Prs3d_PlaneAspect()
: myEdgesAspect (new Prs3d_LineAspect (Quantity_NOC_GREEN,     Aspect_TOL_SOLID, 1.0)),
  myIsoAspect   (new Prs3d_LineAspect (Quantity_NOC_GRAY75,    Aspect_TOL_SOLID, 0.5)),
  myArrowAspect (new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF, Aspect_TOL_SOLID, 1.0)),
  myArrowsLength(0.02),
  myArrowsSize  (0.1),
  myArrowsAngle (M_PI / 8.0),
  myPlaneXLength(1.0),
  myPlaneYLength(1.0),
  myIsoDistance (0.5),
  myDrawCenterArrow (Standard_False),
  myDrawEdgesArrows (Standard_False),
  myDrawEdges (Standard_True),
  myDrawIso   (Standard_False)
{
  //
}

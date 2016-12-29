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

#include <Prs3d_DatumAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_DatumAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_DatumAspect
// purpose  :
// =======================================================================
Prs3d_DatumAspect::Prs3d_DatumAspect()
: myDrawFirstAndSecondAxis (Standard_True),
  myDrawThirdAxis (Standard_True),
  myToDrawLabels (Standard_True),
  myFirstAxisLength (10.0),
  mySecondAxisLength (10.0),
  myThirdAxisLength (10.0)
{
  myFirstAxisAspect = new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID, 1.0);
  mySecondAxisAspect = new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID, 1.0);
  myThirdAxisAspect = new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID, 1.0);
}

// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Graphic3d_AspectLine3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectLine3d, Standard_Transient)

// =======================================================================
// function : Graphic3d_AspectLine3d
// purpose  :
// =======================================================================
Graphic3d_AspectLine3d::Graphic3d_AspectLine3d()
: myColor (Quantity_NOC_YELLOW),
  myType  (Aspect_TOL_SOLID),
  myWidth (1.0f)
{
  //
}

// =======================================================================
// function : Graphic3d_AspectLine3d
// purpose  :
// =======================================================================
Graphic3d_AspectLine3d::Graphic3d_AspectLine3d (const Quantity_Color&   theColor,
                                                const Aspect_TypeOfLine theType,
                                                const Standard_Real     theWidth)
: myColor (theColor),
  myType  (theType),
  myWidth ((float )theWidth)
{
  if (myWidth <= 0.0f)
  {
    throw Aspect_AspectLineDefinitionError("Graphic3d_AspectLine3d, Bad value for LineWidth");
  }
}

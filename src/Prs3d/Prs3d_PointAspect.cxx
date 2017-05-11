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

#include <Prs3d_PointAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_PointAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_PointAspect
// purpose  :
// =======================================================================
Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker theType,
                                      const Quantity_Color& theColor,
                                      const Standard_Real theScale)
: myAspect (new Graphic3d_AspectMarker3d (theType, theColor, theScale))
{
  //
}

// =======================================================================
// function : Prs3d_PointAspect
// purpose  :
// =======================================================================
Prs3d_PointAspect::Prs3d_PointAspect (const Quantity_Color& theColor,
                                      const Standard_Integer theWidth,
                                      const Standard_Integer theHeight,
                                      const Handle(TColStd_HArray1OfByte)& theTexture) 
: myAspect (new Graphic3d_AspectMarker3d (theColor, theWidth, theHeight, theTexture))
{
  //
}

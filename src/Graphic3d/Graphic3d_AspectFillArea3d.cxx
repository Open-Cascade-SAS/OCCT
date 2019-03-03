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

#include <Graphic3d_AspectFillArea3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectFillArea3d, Graphic3d_Aspects)

// =======================================================================
// function : Graphic3d_AspectFillArea3d
// purpose  :
// =======================================================================
Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d()
{
  myInteriorStyle = Aspect_IS_EMPTY;
}

// =======================================================================
// function : Graphic3d_AspectFillArea3d
// purpose  :
// =======================================================================
Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d (const Aspect_InteriorStyle theInteriorStyle,
                                                        const Quantity_Color&      theInteriorColor,
                                                        const Quantity_Color&      theEdgeColor,
                                                        const Aspect_TypeOfLine    theEdgeLineType,
                                                        const Standard_Real        theEdgeLineWidth,
                                                        const Graphic3d_MaterialAspect& theFrontMaterial,
                                                        const Graphic3d_MaterialAspect& theBackMaterial)
{
  myFrontMaterial     = theFrontMaterial;
  myBackMaterial      = theBackMaterial;
  myInteriorColor.SetRGB (theInteriorColor);
  myBackInteriorColor.SetRGB (theInteriorColor);
  myEdgeColor.SetRGB (theEdgeColor);
  myInteriorStyle     = theInteriorStyle;
  myLineType          = theEdgeLineType;
  SetEdgeWidth ((float )theEdgeLineWidth);
}

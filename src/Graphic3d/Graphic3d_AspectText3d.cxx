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

#include <Graphic3d_AspectText3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectText3d, Standard_Transient)

// =======================================================================
// function : Graphic3d_AspectText3d
// purpose  :
// =======================================================================
Graphic3d_AspectText3d::Graphic3d_AspectText3d()
: myFont   (Font_NOF_ASCII_MONO),
  myColor  (Quantity_NOC_YELLOW),
  myFactor (1.0),
  mySpace  (0.0),
  myStyle  (Aspect_TOST_NORMAL),
  myDisplayType   (Aspect_TODT_NORMAL),
  myColorSubTitle (Quantity_NOC_WHITE),
  myTextZoomable  (false),
  myTextAngle     (0.0),
  myTextFontAspect(Font_FA_Regular)
{
  //
}

// =======================================================================
// function : Graphic3d_AspectText3d
// purpose  :
// =======================================================================
Graphic3d_AspectText3d::Graphic3d_AspectText3d (const Quantity_Color&  theColor,
                                                const Standard_CString theFont,
                                                const Standard_Real    theExpansionFactor,
                                                const Standard_Real    theSpace,
                                                const Aspect_TypeOfStyleText   theStyle,
                                                const Aspect_TypeOfDisplayText theDisplayType)
: myFont   (theFont),
  myColor  (theColor),
  myFactor (theExpansionFactor),
  mySpace  (theSpace),
  myStyle  (theStyle),
  myDisplayType   (theDisplayType),
  myColorSubTitle (Quantity_NOC_WHITE),
  myTextZoomable  (false),
  myTextAngle     (0.0),
  myTextFontAspect(Font_FA_Regular)
{
  if (myFont.IsEmpty())
  {
    myFont = Font_NOF_ASCII_MONO;
  }

  if (theExpansionFactor <= 0.0)
  {
    throw Graphic3d_AspectTextDefinitionError("Bad value for TextScaleFactor");
  }
}

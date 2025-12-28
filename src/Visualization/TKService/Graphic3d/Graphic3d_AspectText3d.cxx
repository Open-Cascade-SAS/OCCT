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

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectText3d, Graphic3d_Aspects)

//=================================================================================================

Graphic3d_AspectText3d::Graphic3d_AspectText3d()
{
  SetAlphaMode(Graphic3d_AlphaMode_MaskBlend, 0.285f);
  myShadingModel = Graphic3d_TypeOfShadingModel_Unlit;
  myInteriorColor.SetRGB(Quantity_NOC_YELLOW);
  myEdgeColor.SetRGB(Quantity_NOC_WHITE);
}

//=================================================================================================

Graphic3d_AspectText3d::Graphic3d_AspectText3d(const Quantity_Color& theColor,
                                               const char*           theFont,
                                               double,
                                               double,
                                               Aspect_TypeOfStyleText   theStyle,
                                               Aspect_TypeOfDisplayText theDisplayType)
{
  SetAlphaMode(Graphic3d_AlphaMode_MaskBlend, 0.285f);
  myShadingModel    = Graphic3d_TypeOfShadingModel_Unlit;
  myTextStyle       = theStyle;
  myTextDisplayType = theDisplayType;
  myInteriorColor.SetRGB(theColor);
  myEdgeColor.SetRGB(Quantity_NOC_WHITE);
  if (theFont != NULL && *theFont != '\0')
  {
    myTextFont = new TCollection_HAsciiString(theFont);
  }
}

//=================================================================================================

void Graphic3d_AspectText3d::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Graphic3d_Aspects)
}

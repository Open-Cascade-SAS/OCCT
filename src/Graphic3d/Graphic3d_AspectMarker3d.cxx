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

#include <Graphic3d_AspectMarker3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectMarker3d, Graphic3d_Aspects)

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d()
{
  myShadingModel = Graphic3d_TOSM_UNLIT;
  myInteriorColor.SetRGB (Quantity_NOC_YELLOW);
  myMarkerType = Aspect_TOM_X;
  myMarkerScale = 1.0f;
}

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Aspect_TypeOfMarker theType,
                                                    const Quantity_Color&     theColor,
                                                    const Standard_Real       theScale)
{
  myShadingModel = Graphic3d_TOSM_UNLIT;
  myInteriorColor.SetRGB (theColor);
  myMarkerType = theType;
  SetMarkerScale ((float )theScale);
}

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Quantity_Color&  theColor,
                                                    const Standard_Integer theWidth,
                                                    const Standard_Integer theHeight,
                                                    const Handle(TColStd_HArray1OfByte)& theTextureBitMap)
{
  myShadingModel = Graphic3d_TOSM_UNLIT;
  myMarkerImage = new Graphic3d_MarkerImage(theTextureBitMap, theWidth, theHeight);
  myInteriorColor.SetRGB (theColor),
  myMarkerType = Aspect_TOM_USERDEFINED;
  myMarkerScale = 1.0f;
}

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Handle(Image_PixMap)& theTextureImage)
{
  myShadingModel = Graphic3d_TOSM_UNLIT;
  myMarkerImage = new Graphic3d_MarkerImage (theTextureImage);
  myInteriorColor.SetRGB (Quantity_NOC_YELLOW);
  myMarkerType = Aspect_TOM_USERDEFINED;
  myMarkerScale = 1.0f;
}

// =======================================================================
// function : GetTextureSize
// purpose  :
// =======================================================================
void Graphic3d_AspectMarker3d::GetTextureSize (Standard_Integer& theWidth,
                                               Standard_Integer& theHeight) const
{
  if (!myMarkerImage.IsNull())
  {
    myMarkerImage->GetTextureSize (theWidth, theHeight);
  }
  else
  {
    theWidth  = 0;
    theHeight = 0;
  }
}

// =======================================================================
// function : SetBitMap
// purpose  :
// =======================================================================
void Graphic3d_AspectMarker3d::SetBitMap (const Standard_Integer theWidth,
                                          const Standard_Integer theHeight,
                                          const Handle(TColStd_HArray1OfByte)& theTextureBitMap)
{
  myMarkerImage.Nullify();
  myMarkerImage = new Graphic3d_MarkerImage (theTextureBitMap, theWidth, theHeight);
}

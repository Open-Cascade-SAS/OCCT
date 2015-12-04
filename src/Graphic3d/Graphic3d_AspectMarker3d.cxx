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
#include <Graphic3d_MarkerImage.hxx>
#include <Image_PixMap.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfByte.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectMarker3d,Aspect_AspectMarker)

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d()
: Aspect_AspectMarker()
{}

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Aspect_TypeOfMarker theType,
                                                    const Quantity_Color&     theColor,
                                                    const Standard_Real       theScale)
: Aspect_AspectMarker (theColor, theType, theScale)
{}

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Quantity_Color&  theColor,
                                                    const Standard_Integer theWidth,
                                                    const Standard_Integer theHeight,
                                                    const Handle(TColStd_HArray1OfByte)& theTextureBitMap)
: Aspect_AspectMarker (theColor, Aspect_TOM_USERDEFINED, 1.0),
  myMarkerImage (new Graphic3d_MarkerImage (theTextureBitMap, theWidth, theHeight))
{}

// =======================================================================
// function : Graphic3d_AspectMarker3d
// purpose  :
// =======================================================================
Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Handle(Image_PixMap)& theTextureImage)
: Aspect_AspectMarker (Quantity_NOC_YELLOW, Aspect_TOM_USERDEFINED, 1.0),
  myMarkerImage (new Graphic3d_MarkerImage (theTextureImage))
{}

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
// function : GetMarkerImage
// purpose  :
// =======================================================================
const Handle(Graphic3d_MarkerImage)& Graphic3d_AspectMarker3d::GetMarkerImage() const
{
  return myMarkerImage;
}

// =======================================================================
// function : SetMarkerImage
// purpose  :
// =======================================================================
void Graphic3d_AspectMarker3d::SetMarkerImage (const Handle(Graphic3d_MarkerImage)& theImage)
{
  myMarkerImage = theImage;
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

// =======================================================================
// function : SetShaderProgram
// purpose  :
// =======================================================================
void Graphic3d_AspectMarker3d::SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram)
{
  MyShaderProgram = theProgram;
}

// =======================================================================
// function : ShaderProgram
// purpose  :
// =======================================================================
const Handle(Graphic3d_ShaderProgram)& Graphic3d_AspectMarker3d::ShaderProgram() const
{
  return MyShaderProgram;
}

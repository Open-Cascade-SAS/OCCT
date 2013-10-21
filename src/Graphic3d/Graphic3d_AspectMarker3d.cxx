// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Graphic3d_AspectMarker3d.ixx>
#include <TColStd_Array1OfByte.hxx>
#include <Image_PixMap.hxx>
#include <Graphic3d_MarkerImage.hxx>

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

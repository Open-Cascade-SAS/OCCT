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


#include <Graphic3d_AspectMarker3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_PointAspect,Prs3d_BasicAspect)

Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker aType,
				  const Quantity_Color &aColor,
				  const Standard_Real aScale) {
  myAspect = new Graphic3d_AspectMarker3d(aType,aColor,aScale);
}

Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker aType,
				  const Quantity_NameOfColor aColor,
				  const Standard_Real aScale) {
  myAspect = new Graphic3d_AspectMarker3d
    (aType,Quantity_Color(aColor),aScale);
}

Prs3d_PointAspect::Prs3d_PointAspect (const Quantity_Color &aColor,
                                      const Standard_Integer aWidth,
                                      const Standard_Integer aHeight,
				      const Handle(TColStd_HArray1OfByte)& aTexture
                                      ) 
{
  myAspect = new Graphic3d_AspectMarker3d
    (aColor,aWidth,aHeight,aTexture);
}

Prs3d_PointAspect::Prs3d_PointAspect( const Handle( Graphic3d_AspectMarker3d )& theAspect ) 
{
  myAspect = theAspect;
}


void Prs3d_PointAspect::SetColor(const Quantity_Color &aColor) {
  myAspect->SetColor(aColor);
}


void Prs3d_PointAspect::SetColor(const Quantity_NameOfColor aColor) {
  myAspect->SetColor(Quantity_Color(aColor));
}

void Prs3d_PointAspect::SetTypeOfMarker(const Aspect_TypeOfMarker aType){
  myAspect->SetType(aType);
}

void Prs3d_PointAspect::SetScale(const Standard_Real aScale){
  myAspect->SetScale(aScale);
}

Handle (Graphic3d_AspectMarker3d) Prs3d_PointAspect::Aspect () const {
  return myAspect;
}

void Prs3d_PointAspect::SetAspect( const Handle( Graphic3d_AspectMarker3d )& theAspect ) 
{
  myAspect = theAspect;
}

void Prs3d_PointAspect::GetTextureSize(Standard_Integer& AWidth, Standard_Integer& AHeight)
{
  myAspect->GetTextureSize( AWidth, AHeight);
}

const Handle(Graphic3d_MarkerImage)& Prs3d_PointAspect::GetTexture()
{
  return myAspect->GetMarkerImage();
}

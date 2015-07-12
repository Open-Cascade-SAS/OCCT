// Created on: 1993-09-14
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Font_NameOfFont.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

Prs3d_TextAspect::Prs3d_TextAspect () 
: myAngle(0.),
  myHeight(16.),
  myHorizontalJustification(Graphic3d_HTA_LEFT),
  myVerticalJustification(Graphic3d_VTA_BOTTOM),
  myOrientation(Graphic3d_TP_RIGHT) {

  myTextAspect = new Graphic3d_AspectText3d (
                                             Quantity_Color(Quantity_NOC_YELLOW),
                                             Font_NOF_ASCII_TRIPLEX,
                                             1.,
                                             0.);
}

Prs3d_TextAspect::Prs3d_TextAspect( const Handle( Graphic3d_AspectText3d )& theAspect )
: myAngle(0.),
  myHeight(16.),
  myHorizontalJustification(Graphic3d_HTA_LEFT),
  myVerticalJustification(Graphic3d_VTA_BOTTOM),
  myOrientation(Graphic3d_TP_RIGHT)
{
  myTextAspect = theAspect;
}


void Prs3d_TextAspect::SetColor(const Quantity_Color &aColor) {
  myTextAspect->SetColor(aColor);
}

void Prs3d_TextAspect::SetColor(const Quantity_NameOfColor aColor) {
  myTextAspect->SetColor(Quantity_Color(aColor));
}

void Prs3d_TextAspect::SetFont(const Standard_CString aFont) {
  myTextAspect->SetFont(aFont);
}

void Prs3d_TextAspect::SetHeightWidthRatio(const Standard_Real aRatio) {
  myTextAspect->SetExpansionFactor(aRatio);
}

void Prs3d_TextAspect::SetSpace(const Quantity_Length aSpace) {
  myTextAspect->SetSpace(aSpace);
}

void Prs3d_TextAspect::SetHeight(const Standard_Real aHeight) {
  myHeight = aHeight;
}

void Prs3d_TextAspect::SetAngle(const Quantity_PlaneAngle anAngle) {
  myAngle = anAngle;
}

void Prs3d_TextAspect::SetHorizontalJustification(const Graphic3d_HorizontalTextAlignment aJustification) {
  myHorizontalJustification = aJustification;
}


void Prs3d_TextAspect::SetVerticalJustification(const Graphic3d_VerticalTextAlignment aJustification) {
  myVerticalJustification = aJustification;
}

void Prs3d_TextAspect::SetOrientation(const Graphic3d_TextPath anOrientation) {

  myOrientation = anOrientation;
}

Standard_Real Prs3d_TextAspect::Height () const {return myHeight;}

Quantity_PlaneAngle Prs3d_TextAspect::Angle () const {return myAngle;}

Graphic3d_HorizontalTextAlignment Prs3d_TextAspect::HorizontalJustification () const { return myHorizontalJustification;}

Graphic3d_VerticalTextAlignment Prs3d_TextAspect::VerticalJustification () const { return myVerticalJustification;}

Graphic3d_TextPath Prs3d_TextAspect::Orientation () const {return myOrientation;}

Handle(Graphic3d_AspectText3d) Prs3d_TextAspect::Aspect() const {
  return myTextAspect;
}

void Prs3d_TextAspect::SetAspect( const Handle( Graphic3d_AspectText3d )& theAspect )
{
  myTextAspect = theAspect;
}

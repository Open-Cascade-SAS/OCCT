// Created on: 1993-09-14
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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


#define GER61351    //GG_171199     Enable to set an object RGB color
//              instead a restricted object NameOfColor.


#include <Prs3d_TextAspect.ixx>
#include <Quantity_Color.hxx>
#include <Font_NameOfFont.hxx>

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

#ifdef GER61351
void Prs3d_TextAspect::SetColor(const Quantity_Color &aColor) {
  myTextAspect->SetColor(aColor);
}
#endif

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

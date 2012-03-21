// Copyright (c) 1995-1999 Matra Datavision
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

#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <Prs3d_LineAspect.ixx>

#ifdef GER61351
Prs3d_LineAspect::Prs3d_LineAspect (const Quantity_Color &aColor,
				const Aspect_TypeOfLine aType,
				const Standard_Real aWidth) {
  myAspect = new Graphic3d_AspectLine3d(aColor,aType,aWidth);
}
#endif

Prs3d_LineAspect::Prs3d_LineAspect (const Quantity_NameOfColor aColor,
				const Aspect_TypeOfLine aType,
				const Standard_Real aWidth) {

  myAspect = new Graphic3d_AspectLine3d
    (Quantity_Color(aColor),aType,aWidth);
}

#ifdef GER61351
void Prs3d_LineAspect::SetColor(const Quantity_Color &aColor) {
 myAspect->SetColor(aColor);
}
#endif

void Prs3d_LineAspect::SetColor(const Quantity_NameOfColor aColor) {
  myAspect->SetColor(Quantity_Color(aColor));
}

void Prs3d_LineAspect::SetTypeOfLine(const Aspect_TypeOfLine aType){
  myAspect->SetType(aType);
}

void Prs3d_LineAspect::SetWidth(const Standard_Real aWidth){
  myAspect->SetWidth(aWidth);
}

Handle (Graphic3d_AspectLine3d) Prs3d_LineAspect::Aspect () const {
  return myAspect;
}

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

#define IMP120100	//GG 01/01/00 Add SetColor() methods

#include <Prs3d_ArrowAspect.ixx>

Prs3d_ArrowAspect::Prs3d_ArrowAspect () 
     : myAngle(M_PI/180.*10), myLength(1.) {
#ifdef IMP120100
  myArrowAspect = 
	new Graphic3d_AspectLine3d (
		Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0);
#endif
}


Prs3d_ArrowAspect::Prs3d_ArrowAspect (const Quantity_PlaneAngle anAngle,
				      const Quantity_Length aLength) 
     : myAngle(anAngle), myLength(aLength) {
#ifdef IMP120100
  myArrowAspect = 
	new Graphic3d_AspectLine3d (
		Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0);
#endif
}

void Prs3d_ArrowAspect::SetAngle ( const Quantity_PlaneAngle anAngle) {
  Prs3d_InvalidAngle_Raise_if ( anAngle <= 0.  ||
                              anAngle >= M_PI /2. , "");
  myAngle = anAngle;
}
Quantity_PlaneAngle Prs3d_ArrowAspect::Angle () const
{
return myAngle;
}

void Prs3d_ArrowAspect::SetLength ( const Quantity_Length aLength)
{
  myLength = aLength;
}
Quantity_Length Prs3d_ArrowAspect::Length () const
{
return myLength;
}

#ifdef IMP120100
void Prs3d_ArrowAspect::SetColor(const Quantity_Color &aColor) {
  myArrowAspect->SetColor(aColor);
}

void Prs3d_ArrowAspect::SetColor(const Quantity_NameOfColor aColor) {
  SetColor(Quantity_Color(aColor));
}

Handle(Graphic3d_AspectLine3d) Prs3d_ArrowAspect::Aspect() const {
  return myArrowAspect;
}
#endif

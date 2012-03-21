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

#include <Prs3d_LengthAspect.ixx>

Prs3d_LengthAspect::Prs3d_LengthAspect() {
  myLineAspect = new Prs3d_LineAspect 
                          (Quantity_NOC_LAWNGREEN,Aspect_TOL_SOLID,1.); 
  myArrow1Aspect = new Prs3d_ArrowAspect;
  myArrow2Aspect = new Prs3d_ArrowAspect;
  myTextAspect = new Prs3d_TextAspect;
  myDrawFirstArrow = Standard_True;
  myDrawSecondArrow = Standard_True;
}

Handle (Prs3d_LineAspect) Prs3d_LengthAspect::LineAspect () const {
  return myLineAspect;}

void Prs3d_LengthAspect::SetLineAspect(const Handle(Prs3d_LineAspect)& anAspect) {
  myLineAspect = anAspect;}

Handle(Prs3d_ArrowAspect) Prs3d_LengthAspect::Arrow1Aspect () const {
  return myArrow1Aspect;}

void Prs3d_LengthAspect::SetArrow1Aspect (
				  const Handle(Prs3d_ArrowAspect)& anAspect) {
  myArrow1Aspect = anAspect;
}
Handle(Prs3d_ArrowAspect) Prs3d_LengthAspect::Arrow2Aspect () const {
  return myArrow2Aspect;}

void Prs3d_LengthAspect::SetArrow2Aspect (
				  const Handle(Prs3d_ArrowAspect)& anAspect) {
  myArrow2Aspect = anAspect;
}

Handle(Prs3d_TextAspect) Prs3d_LengthAspect::TextAspect () const {
  return myTextAspect;}

void Prs3d_LengthAspect::SetTextAspect (
				  const Handle(Prs3d_TextAspect)& anAspect) {
  myTextAspect = anAspect;
}
void Prs3d_LengthAspect::SetDrawFirstArrow (const Standard_Boolean draw) {
  myDrawFirstArrow = draw;
}
Standard_Boolean Prs3d_LengthAspect::DrawFirstArrow () const {
  return myDrawFirstArrow;
}

void Prs3d_LengthAspect::SetDrawSecondArrow (const Standard_Boolean draw) {
  myDrawSecondArrow = draw;
}
Standard_Boolean Prs3d_LengthAspect::DrawSecondArrow () const {
  return myDrawSecondArrow;
}

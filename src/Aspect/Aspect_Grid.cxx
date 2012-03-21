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

// Updated:     GG IMP230300 Add grid color parameters in constructor
//              and add new methods SetColors() & Colors()

#include <Aspect_Grid.ixx>


Aspect_Grid::Aspect_Grid(
			       const Quantity_Length anXOrigin,
			       const Quantity_Length anYOrigin,
			       const Quantity_PlaneAngle anAngle,
			       const Quantity_Color& aColor,
			       const Quantity_Color& aTenthColor)
: myRotationAngle(anAngle),
  myXOrigin(anXOrigin),
  myYOrigin(anYOrigin),
  myColor(aColor),
  myTenthColor(aTenthColor),
  myIsActive(Standard_False),
  myDrawMode(Aspect_GDM_Lines)
{
}


void Aspect_Grid::SetXOrigin(const Quantity_Length anOrigin) {
  myXOrigin = anOrigin;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetYOrigin(const Quantity_Length anOrigin) {
  myYOrigin = anOrigin;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetRotationAngle(const Quantity_Length anAngle){


  myRotationAngle = anAngle;
  Init();
  UpdateDisplay();
}
void Aspect_Grid::Rotate(const Quantity_PlaneAngle anAngle) {
  myRotationAngle += anAngle;
  Init();
  UpdateDisplay();
}
void Aspect_Grid::Translate(const Quantity_Length aDx,
				  const Quantity_Length aDy) {
  myXOrigin += aDx;
  myYOrigin += aDy;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetColors(const Quantity_Color& aColor,
			    const Quantity_Color& aTenthColor) {
  myColor = aColor;
  myTenthColor = aTenthColor;
  UpdateDisplay();
}

void Aspect_Grid::Colors(Quantity_Color& aColor,
			 Quantity_Color& aTenthColor) const {
  aColor = myColor;
  aTenthColor = myTenthColor;
}

void Aspect_Grid::Hit(const Quantity_Length X,
			 const Quantity_Length Y,
			 Quantity_Length& gridX,
			 Quantity_Length& gridY) const {
   if (myIsActive) {
     Compute(X,Y,gridX,gridY);}
   else{
    gridX = X;
    gridY = Y;
  }
 }
void Aspect_Grid::Activate () {
  myIsActive = Standard_True;
}

void Aspect_Grid::Deactivate () {
  myIsActive = Standard_False;
}

Quantity_Length Aspect_Grid::XOrigin() const {
  return myXOrigin;
}

Quantity_Length Aspect_Grid::YOrigin() const {
  return myYOrigin;
}

Quantity_Length Aspect_Grid::RotationAngle() const {
  return myRotationAngle;
}

Standard_Boolean Aspect_Grid::IsActive() const {
  return myIsActive;
}
void Aspect_Grid::Display() {}

void Aspect_Grid::Erase () const {}

void Aspect_Grid::UpdateDisplay () {}


Standard_Boolean Aspect_Grid::IsDisplayed() const {
  return Standard_False;}

void Aspect_Grid::SetDrawMode(const Aspect_GridDrawMode aDrawMode) {
  myDrawMode = aDrawMode; 
  UpdateDisplay();
}
Aspect_GridDrawMode Aspect_Grid::DrawMode() const {
  return myDrawMode;
}

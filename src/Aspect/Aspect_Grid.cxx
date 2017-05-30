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


#include <Aspect_Grid.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_Grid,MMgt_TShared)

Aspect_Grid::Aspect_Grid(
			       const Standard_Real anXOrigin,
			       const Standard_Real anYOrigin,
			       const Standard_Real anAngle,
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


void Aspect_Grid::SetXOrigin(const Standard_Real anOrigin) {
  myXOrigin = anOrigin;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetYOrigin(const Standard_Real anOrigin) {
  myYOrigin = anOrigin;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetRotationAngle(const Standard_Real anAngle){


  myRotationAngle = anAngle;
  Init();
  UpdateDisplay();
}
void Aspect_Grid::Rotate(const Standard_Real anAngle) {
  myRotationAngle += anAngle;
  Init();
  UpdateDisplay();
}
void Aspect_Grid::Translate(const Standard_Real aDx,
				  const Standard_Real aDy) {
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

void Aspect_Grid::Hit(const Standard_Real X,
			 const Standard_Real Y,
			 Standard_Real& gridX,
			 Standard_Real& gridY) const {
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

Standard_Real Aspect_Grid::XOrigin() const {
  return myXOrigin;
}

Standard_Real Aspect_Grid::YOrigin() const {
  return myYOrigin;
}

Standard_Real Aspect_Grid::RotationAngle() const {
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

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

IMPLEMENT_STANDARD_RTTIEXT(Aspect_Grid,Standard_Transient)

Aspect_Grid::Aspect_Grid (const Standard_Real theXOrigin,
			                    const Standard_Real theYOrigin,
			                    const Standard_Real theAngle,
			                    const Quantity_Color& theColor,
			                    const Quantity_Color& theTenthColor)
: myRotationAngle (theAngle),
  myXOrigin (theXOrigin),
  myYOrigin (theYOrigin),
  myColor (theColor),
  myTenthColor (theTenthColor),
  myIsActive (Standard_False),
  myDrawMode (Aspect_GDM_Lines)
{
  //
}

void Aspect_Grid::SetXOrigin (const Standard_Real theOrigin)
{
  myXOrigin = theOrigin;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetYOrigin (const Standard_Real theOrigin)
{
  myYOrigin = theOrigin;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetRotationAngle (const Standard_Real theAngle)
{
  myRotationAngle = theAngle;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::Rotate (const Standard_Real theAngle)
{
  myRotationAngle += theAngle;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::Translate (const Standard_Real theDx,
				                     const Standard_Real theDy)
{
  myXOrigin += theDx;
  myYOrigin += theDy;
  Init();
  UpdateDisplay();
}

void Aspect_Grid::SetColors (const Quantity_Color& theColor,
			                       const Quantity_Color& theTenthColor)
{
  myColor = theColor;
  myTenthColor = theTenthColor;
  UpdateDisplay();
}

void Aspect_Grid::Colors (Quantity_Color& theColor,
			                    Quantity_Color& theTenthColor) const
{
  theColor = myColor;
  theTenthColor = myTenthColor;
}

void Aspect_Grid::Hit (const Standard_Real theX,
			                 const Standard_Real theY,
			                 Standard_Real& theGridX,
			                 Standard_Real& theGridY) const
{
  if (myIsActive)
  {
    Compute (theX, theY, theGridX, theGridY);
  }
  else
  {
    theGridX = theX;
    theGridY = theY;
  }
}

void Aspect_Grid::SetDrawMode (const Aspect_GridDrawMode theDrawMode)
{
  myDrawMode = theDrawMode;
  UpdateDisplay();
}

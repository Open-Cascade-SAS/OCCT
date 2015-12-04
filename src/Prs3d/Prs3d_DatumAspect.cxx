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


#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_DatumAspect,Prs3d_BasicAspect)

Prs3d_DatumAspect::Prs3d_DatumAspect()
: myDrawFirstAndSecondAxis (Standard_True),
  myDrawThirdAxis (Standard_True),
  myToDrawLabels (Standard_True),
  myFirstAxisLength (10.0),
  mySecondAxisLength (10.0),
  myThirdAxisLength (10.0)
{
  myFirstAxisAspect = new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID, 1.0);
  mySecondAxisAspect = new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID, 1.0);
  myThirdAxisAspect = new Prs3d_LineAspect (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID, 1.0);
}

Handle(Prs3d_LineAspect) Prs3d_DatumAspect::FirstAxisAspect() const {

  return myFirstAxisAspect;
}

Handle(Prs3d_LineAspect) Prs3d_DatumAspect::SecondAxisAspect()  const {

  return mySecondAxisAspect;
}

Handle(Prs3d_LineAspect) Prs3d_DatumAspect::ThirdAxisAspect()  const {

  return myThirdAxisAspect;

}
Standard_Boolean Prs3d_DatumAspect::DrawFirstAndSecondAxis () const {

  return myDrawFirstAndSecondAxis;

}

void Prs3d_DatumAspect::SetDrawFirstAndSecondAxis (const Standard_Boolean draw)
{
  myDrawFirstAndSecondAxis = draw;
}

Standard_Boolean Prs3d_DatumAspect::DrawThirdAxis () const {

  return myDrawThirdAxis;

}

void Prs3d_DatumAspect::SetDrawThirdAxis (const Standard_Boolean draw)
{
  myDrawThirdAxis = draw;
}

void Prs3d_DatumAspect::SetAxisLength (const Quantity_Length L1,
				       const Quantity_Length L2,
				       const Quantity_Length L3) {

  myFirstAxisLength = L1;
  mySecondAxisLength = L2;
  myThirdAxisLength = L3;
}


Quantity_Length Prs3d_DatumAspect::FirstAxisLength () const {

  return myFirstAxisLength;

}

Quantity_Length Prs3d_DatumAspect::SecondAxisLength () const {

  return mySecondAxisLength;

}

Quantity_Length Prs3d_DatumAspect::ThirdAxisLength () const {

  return myThirdAxisLength;

}

//=======================================================================
//function : SetToDrawLabels
//purpose  : 
//=======================================================================
void Prs3d_DatumAspect::SetToDrawLabels (const Standard_Boolean theToDraw)
{
  myToDrawLabels = theToDraw;
}

//=======================================================================
//function : ToDrawLabels
//purpose  : 
//=======================================================================
Standard_Boolean Prs3d_DatumAspect::ToDrawLabels() const
{
  return myToDrawLabels;
}

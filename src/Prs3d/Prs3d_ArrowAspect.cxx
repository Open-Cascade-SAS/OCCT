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


#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_InvalidAngle.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_ArrowAspect,Prs3d_BasicAspect)

Prs3d_ArrowAspect::Prs3d_ArrowAspect () 
     : myAngle(M_PI/180.*10), myLength(1.) {
  myArrowAspect = 
	new Graphic3d_AspectLine3d (
		Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0);
}


Prs3d_ArrowAspect::Prs3d_ArrowAspect (const Quantity_PlaneAngle anAngle,
				      const Quantity_Length aLength) 
     : myAngle(anAngle), myLength(aLength) {
  myArrowAspect = 
	new Graphic3d_AspectLine3d (
		Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0);
}

Prs3d_ArrowAspect::Prs3d_ArrowAspect( const Handle( Graphic3d_AspectLine3d )& theAspect )
  : myAngle(M_PI/180.*10), myLength(1.)
{
  myArrowAspect = theAspect;
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


void Prs3d_ArrowAspect::SetColor(const Quantity_Color &aColor) {
  myArrowAspect->SetColor(aColor);
}

void Prs3d_ArrowAspect::SetColor(const Quantity_NameOfColor aColor) {
  SetColor(Quantity_Color(aColor));
}

Handle(Graphic3d_AspectLine3d) Prs3d_ArrowAspect::Aspect() const {
  return myArrowAspect;
}


void Prs3d_ArrowAspect::SetAspect( const Handle( Graphic3d_AspectLine3d )& theAspect )
{
  myArrowAspect = theAspect;
}


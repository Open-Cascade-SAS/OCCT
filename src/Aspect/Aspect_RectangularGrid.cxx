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

#include <Aspect_RectangularGrid.hxx>

#include <Standard_NegativeValue.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_NumericError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_RectangularGrid,Aspect_Grid)

Aspect_RectangularGrid::Aspect_RectangularGrid(
                               const Standard_Real aXStep,
                               const Standard_Real aYStep,
                               const Standard_Real anXOrigin,
                               const Standard_Real anYOrigin,
                               const Standard_Real aFirstAngle,
                               const Standard_Real aSecondAngle,
                               const Standard_Real aRotationAngle)
:Aspect_Grid(anXOrigin,anYOrigin,aRotationAngle),myXStep(aXStep),myYStep(aYStep),myFirstAngle(aFirstAngle),mySecondAngle(aSecondAngle)

{
  Standard_NumericError_Raise_if(!CheckAngle (aFirstAngle,mySecondAngle),
                                 "networks are parallel");

  Standard_NegativeValue_Raise_if(aXStep < 0. , "invalid x step");
  Standard_NegativeValue_Raise_if(aYStep < 0. , "invalid y step");
  Standard_NullValue_Raise_if(aXStep == 0. , "invalid x step");
  Standard_NullValue_Raise_if(aYStep == 0. , "invalid y step");
}



void Aspect_RectangularGrid::SetXStep(const Standard_Real aStep) {
  Standard_NegativeValue_Raise_if(aStep < 0. , "invalid x step");
  Standard_NullValue_Raise_if(aStep == 0. , "invalid y step");
  myXStep = aStep;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::SetYStep(const Standard_Real aStep) {
  Standard_NegativeValue_Raise_if(aStep < 0. , "invalid x step");
  Standard_NullValue_Raise_if(aStep == 0. , "invalid y step");
  myYStep = aStep;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::SetAngle (const Standard_Real anAngle1,
                                       const Standard_Real anAngle2)
{
  Standard_NumericError_Raise_if(!CheckAngle (anAngle1,anAngle2 ),
                                 "axis are parallel");
  myFirstAngle = anAngle1;
  mySecondAngle = anAngle2;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::SetGridValues(
	const Standard_Real theXOrigin,
	const Standard_Real theYOrigin,
	const Standard_Real theXStep,
	const Standard_Real theYStep,
	const Standard_Real theRotationAngle) {

  myXOrigin = theXOrigin;
  myYOrigin = theYOrigin;
  Standard_NegativeValue_Raise_if(theXStep < 0. , "invalid x step");
  Standard_NullValue_Raise_if(theXStep == 0. , "invalid x step");
  myXStep = theXStep;
  Standard_NegativeValue_Raise_if(theYStep < 0. , "invalid y step");
  Standard_NullValue_Raise_if(theYStep == 0. , "invalid y step");
  myYStep = theYStep;
  myRotationAngle = theRotationAngle;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::Compute(const Standard_Real X,
                         const Standard_Real Y,
                         Standard_Real& gridX,
                         Standard_Real& gridY) const {
    Standard_Real D1 = b1 * X - a1 * Y - c1;
    Standard_Real D2 = b2 * X - a2 * Y - c2;
    Standard_Integer n1 = Standard_Integer ( Abs(D1)/myXStep + 0.5);
    Standard_Integer n2 = Standard_Integer ( Abs(D2)/myYStep + 0.5);
    Standard_Real offset1 = c1 + Standard_Real(n1) * Sign (myXStep , D1);
    Standard_Real offset2 = c2 + Standard_Real(n2) * Sign (myYStep , D2);
    Standard_Real Delta = a1*b2 - b1*a2;
    gridX = ( offset2*a1 - offset1*a2) /Delta;
    gridY = ( offset2*b1 - offset1*b2) /Delta;
}

Standard_Real Aspect_RectangularGrid::XStep() const {
  return myXStep;
}

Standard_Real Aspect_RectangularGrid::YStep() const {
  return myYStep;
}

Standard_Real Aspect_RectangularGrid::FirstAngle() const {
  return myFirstAngle;
}

Standard_Real Aspect_RectangularGrid::SecondAngle() const {
  return mySecondAngle;
}

void Aspect_RectangularGrid::Init () {

//+zov Fixing CTS17856
//  a1 = Cos (myFirstAngle + RotationAngle() ); 
//  b1 = Sin (myFirstAngle + RotationAngle() );
//  c1 = XOrigin() * b1 - YOrigin() * a1;
//
//  a2 = Cos (mySecondAngle + RotationAngle() + M_PI / 2.); 
//  b2 = Sin (mySecondAngle + RotationAngle() + M_PI / 2.);
//  c2 = XOrigin() * b2 - YOrigin() * a2;

  Standard_Real angle1 = myFirstAngle + RotationAngle();
  Standard_Real angle2 = mySecondAngle + RotationAngle();
  if ( angle1 != 0. ) {
    a1 = -Sin (angle1); 
    b1 = Cos (angle1);
    c1 = XOrigin() * b1 - YOrigin() * a1;
  } else {
    a1 = 0.; b1 = 1.; c1 = XOrigin();
  }

  if ( angle2 != 0. ) {
    angle2 += M_PI / 2.;
    a2 = -Sin (angle2); 
    b2 = Cos (angle2);
    c2 = XOrigin() * b2 - YOrigin() * a2;
  } else {
    a2 = -1.; b2 = 0.; c2 = YOrigin();
  }
//-zov
}

Standard_Boolean Aspect_RectangularGrid::CheckAngle(const Standard_Real alpha,
                                            const Standard_Real beta) const {
  return (Abs( Sin(alpha) * Cos(beta + M_PI / 2.) - Cos(alpha) * Sin(beta + M_PI / 2.)) != 0) ;
}

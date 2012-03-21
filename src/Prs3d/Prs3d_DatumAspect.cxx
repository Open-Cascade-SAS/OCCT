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

#include <Prs3d_DatumAspect.ixx>

Prs3d_DatumAspect::Prs3d_DatumAspect () {

  myFirstAxisAspect = new Prs3d_LineAspect
      (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  mySecondAxisAspect = new Prs3d_LineAspect
      (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  myThirdAxisAspect = new Prs3d_LineAspect
      (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  myDrawFirstAndSecondAxis = Standard_True;
  myDrawThirdAxis = Standard_True;
  myFirstAxisLength = 10.;
  mySecondAxisLength = 10.;
  myThirdAxisLength = 10.;
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

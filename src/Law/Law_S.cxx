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

// pmn -> modified 17/01/1996 : utilisation de Curve() et SetCurve()

#include <Law_S.ixx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Law_BSpline.hxx>

Law_S::Law_S ()
{}


void Law_S::Set (const Standard_Real Pdeb,
		 const Standard_Real Valdeb,
		 const Standard_Real Pfin,
		 const Standard_Real Valfin)
{
  Set(Pdeb,Valdeb,0.,Pfin,Valfin,0.);
}


void Law_S::Set (const Standard_Real Pdeb,
		 const Standard_Real Valdeb,
		 const Standard_Real Ddeb,
		 const Standard_Real Pfin,
		 const Standard_Real Valfin,
		 const Standard_Real Dfin)
{
  TColStd_Array1OfReal    poles(1,4);
  TColStd_Array1OfReal    knots(1,2);
  TColStd_Array1OfInteger mults(1,2);
  poles(1) = Valdeb; poles(4) = Valfin;
  Standard_Real coe = (Pfin-Pdeb) / 3.;
  poles(2) = Valdeb + coe * Ddeb;
  poles(3) = Valfin - coe * Dfin;
  knots(1) = Pdeb; knots(2) = Pfin;
  mults(1) = mults(2) = 4; 

  SetCurve( new Law_BSpline(poles,knots,mults,3) );
}

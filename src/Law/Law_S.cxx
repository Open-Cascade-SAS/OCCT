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

// pmn -> modified 17/01/1996 : utilisation de Curve() et SetCurve()

#include <Law_BSpline.hxx>
#include <Law_S.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_S,Law_BSpFunc)

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

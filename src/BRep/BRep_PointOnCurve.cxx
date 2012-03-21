// Created on: 1993-08-10
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <BRep_PointOnCurve.ixx>


//=======================================================================
//function : BRep_PointOnCurve
//purpose  : 
//=======================================================================

BRep_PointOnCurve::BRep_PointOnCurve(const Standard_Real P,
				     const Handle(Geom_Curve)& C, 
				     const TopLoc_Location& L) :
       BRep_PointRepresentation(P,L),
       myCurve(C)
{
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurve::IsPointOnCurve()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurve::IsPointOnCurve
  (const Handle(Geom_Curve)& C,
   const TopLoc_Location& L)const 
{
  return (myCurve == C) && (Location() == L);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_PointOnCurve::Curve()const 
{
  return myCurve;
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

void  BRep_PointOnCurve::Curve(const Handle(Geom_Curve)& C)
{
  myCurve = C;
}



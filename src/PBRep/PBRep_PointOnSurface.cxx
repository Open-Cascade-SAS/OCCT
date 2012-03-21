// Created on: 1993-08-11
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



#include <PBRep_PointOnSurface.ixx>


//=======================================================================
//function : PBRep_PointOnSurface
//purpose  : 
//=======================================================================

PBRep_PointOnSurface::PBRep_PointOnSurface(const Standard_Real P1,
					   const Standard_Real P2, 
					   const Handle(PGeom_Surface)& S, 
					   const PTopLoc_Location& L) :
       PBRep_PointsOnSurface(P1,S,L),
       myParameter2(P2)
{
}


//=======================================================================
//function : Parameter2
//purpose  : 
//=======================================================================

Standard_Real  PBRep_PointOnSurface::Parameter2()const 
{
  return myParameter2;
}

//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointOnSurface::IsPointOnSurface() const
{
  return Standard_True;
}


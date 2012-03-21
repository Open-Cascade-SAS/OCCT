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



#include <PBRep_PointsOnSurface.ixx>

//=======================================================================
//function : PBRep_PointsOnSurface
//purpose  : 
//=======================================================================

PBRep_PointsOnSurface::PBRep_PointsOnSurface
  (const Standard_Real P, 
   const Handle(PGeom_Surface)& S, 
   const PTopLoc_Location& L) :
  PBRep_PointRepresentation(P,L),
  mySurface(S)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_PointsOnSurface::Surface()const 
{
  return mySurface;
}



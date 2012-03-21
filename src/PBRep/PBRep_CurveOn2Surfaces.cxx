// Created on: 1993-07-26
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



#include <PBRep_CurveOn2Surfaces.ixx>


//=======================================================================
//function : PBRep_CurveOn2Surfaces
//purpose  : 
//=======================================================================

PBRep_CurveOn2Surfaces::PBRep_CurveOn2Surfaces
  (const Handle(PGeom_Surface)& S1,
   const Handle(PGeom_Surface)& S2,
   const PTopLoc_Location& L1, 
   const PTopLoc_Location& L2, 
   const GeomAbs_Shape C) :
  PBRep_CurveRepresentation(L1),
  mySurface(S1),
  mySurface2(S2),
  myLocation2(L2),
  myContinuity(C)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_CurveOn2Surfaces::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : Surface2
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_CurveOn2Surfaces::Surface2()const 
{
  return mySurface2;
}


//=======================================================================
//function : Location2
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_CurveOn2Surfaces::Location2()const 
{
  return myLocation2;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape  PBRep_CurveOn2Surfaces::Continuity()const 
{
  return myContinuity;
}

//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOn2Surfaces::IsRegularity()const 
{
  return Standard_True;
}



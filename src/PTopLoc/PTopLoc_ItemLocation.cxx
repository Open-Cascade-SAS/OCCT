// Created on: 1993-03-03
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



#include <PTopLoc_ItemLocation.ixx>

//=======================================================================
//function : PTopLoc_ItemLocation
//purpose  : 
//=======================================================================

PTopLoc_ItemLocation::PTopLoc_ItemLocation(const Handle(PTopLoc_Datum3D)& D, 
					   const Standard_Integer P,
					   const PTopLoc_Location& N) :
       myDatum(D),
       myPower(P),
       myNext(N)
{
}


//=======================================================================
//function : Datum3D
//purpose  : 
//=======================================================================

Handle(PTopLoc_Datum3D)  PTopLoc_ItemLocation::Datum3D()const 
{
  return myDatum;
}


//=======================================================================
//function : Power
//purpose  : 
//=======================================================================

Standard_Integer  PTopLoc_ItemLocation::Power()const 
{
  return myPower;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

PTopLoc_Location  PTopLoc_ItemLocation::Next()const 
{
  return myNext;
}



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



#include <PTopLoc_Location.ixx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : PTopLoc_Location
//purpose  : 
//=======================================================================

PTopLoc_Location::PTopLoc_Location() 
{
}


//=======================================================================
//function : PTopLoc_Location
//purpose  : 
//=======================================================================

PTopLoc_Location::PTopLoc_Location(const Handle(PTopLoc_Datum3D)& D,
				   const Standard_Integer P,
				   const PTopLoc_Location& N) 
{
  myData = new PTopLoc_ItemLocation(D,P,N);
}


//=======================================================================
//function : IsIdentity
//purpose  : 
//=======================================================================

Standard_Boolean PTopLoc_Location::IsIdentity() const 
{
  return myData.IsNull();
}

//=======================================================================
//function : Datum3D
//purpose  : 
//=======================================================================

Handle(PTopLoc_Datum3D)  PTopLoc_Location::Datum3D()const 
{
  Standard_NoSuchObject_Raise_if(IsIdentity(),"PTopLoc_Location::Datum3D");
  return myData->Datum3D();
}


//=======================================================================
//function : Power
//purpose  : 
//=======================================================================

Standard_Integer  PTopLoc_Location::Power()const 
{
  Standard_NoSuchObject_Raise_if(IsIdentity(),"PTopLoc_Location::Power");
  return myData->Power();
}


//=======================================================================
//function : PTopLoc_Location
//purpose  : 
//=======================================================================

PTopLoc_Location PTopLoc_Location::Next() const
{
  Standard_NoSuchObject_Raise_if(IsIdentity(),"PTopLoc_Location::Next");
  return myData->Next();
}




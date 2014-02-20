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

#include <PDataXtd_Constraint.ixx>



PDataXtd_Constraint::PDataXtd_Constraint () : 
    myType(0),
    myIsReversed(Standard_False),
    myIsInverted(Standard_False),
    myIsVerified(Standard_False)
{ }

PDataXtd_Constraint::PDataXtd_Constraint(const Standard_Integer Type,
					 const Handle(PDF_HAttributeArray1)& Geometries,
					 const Handle(PDataStd_Real)& Value,
					 const Handle(PNaming_NamedShape)& Plane)

: myType (Type), myGeometries (Geometries), myValue (Value), myIsReversed(Standard_False),
  myIsInverted(Standard_False),
  myIsVerified(Standard_True),
  myPlane(Plane)
  {}

Handle(PDataStd_Real) PDataXtd_Constraint::GetValue() const
{ return myValue; }

Standard_Integer PDataXtd_Constraint::GetType() const
{ return myType; }

Handle(PDF_HAttributeArray1) PDataXtd_Constraint::GetGeometries() const
{ return myGeometries; }

Handle(PNaming_NamedShape) PDataXtd_Constraint::GetPlane() const 
{ return myPlane;}

void PDataXtd_Constraint::SetPlane(const Handle(PNaming_NamedShape)& plane)
{ myPlane = plane;}

void PDataXtd_Constraint::Set(const Handle(PDataStd_Real)& V) 
{ myValue = V; }

void PDataXtd_Constraint::SetType(const Standard_Integer Type) 
{ myType = Type; }

void PDataXtd_Constraint::SetGeometries(const Handle(PDF_HAttributeArray1)& Geometries) 
{ myGeometries = Geometries; }

//=======================================================================
//function : Verified
//purpose  : 
//=======================================================================
void PDataXtd_Constraint::Verified(const Standard_Boolean status)
{
  myIsVerified = status;
}

//=======================================================================
//function : Verified
//purpose  : 
//=======================================================================
Standard_Boolean PDataXtd_Constraint::Verified() const 
{
  return myIsVerified;
}

//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================
void PDataXtd_Constraint::Reversed(const Standard_Boolean status)
{
  myIsReversed = status;
}

//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================
Standard_Boolean PDataXtd_Constraint::Reversed() const 
{
  return myIsReversed;
}

//=======================================================================
//function : Inverted
//purpose  : 
//=======================================================================
void PDataXtd_Constraint::Inverted(const Standard_Boolean status)
{
  myIsInverted = status;
}

//=======================================================================
//function : Inverted
//purpose  : 
//=======================================================================
Standard_Boolean PDataXtd_Constraint::Inverted() const 
{
  return myIsInverted;
}

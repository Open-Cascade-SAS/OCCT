// Created on: 1993-08-11
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <PBRep_PointRepresentation.ixx>

//=======================================================================
//function : PBRep_PointRepresentation
//purpose  : 
//=======================================================================

PBRep_PointRepresentation::PBRep_PointRepresentation
  (const Standard_Real P,
   const PTopLoc_Location& L) :
   myLocation(L),
   myParameter(P)
  
{
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_PointRepresentation::Location()const 
{
  return myLocation;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  PBRep_PointRepresentation::Parameter()const 
{
  return myParameter;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  PBRep_PointRepresentation::Parameter(const Standard_Real P)
{
  myParameter = P;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

Handle(PBRep_PointRepresentation)  PBRep_PointRepresentation::Next()const 
{
  return myNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  PBRep_PointRepresentation::Next(const Handle(PBRep_PointRepresentation)& N)
{
  myNext = N;
}

//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointRepresentation::IsPointOnCurve() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointRepresentation::IsPointOnCurveOnSurface() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointRepresentation::IsPointOnSurface() const
{
  return Standard_False;
}

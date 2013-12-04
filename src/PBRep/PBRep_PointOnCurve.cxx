// Created on: 1993-08-11
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PBRep_PointOnCurve.ixx>

//=======================================================================
//function : PBRep_PointOnCurve
//purpose  : 
//=======================================================================

PBRep_PointOnCurve::PBRep_PointOnCurve(const Standard_Real P,
				       const Handle(PGeom_Curve)& C, 
				       const PTopLoc_Location& L) :
       PBRep_PointRepresentation(P,L),
       myCurve(C)
{
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PBRep_PointOnCurve::Curve()const 
{
  return myCurve;
}

//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointOnCurve::IsPointOnCurve() const
{
  return Standard_True;
}


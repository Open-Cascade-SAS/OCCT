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


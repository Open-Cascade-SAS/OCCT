// Created on: 1993-07-26
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



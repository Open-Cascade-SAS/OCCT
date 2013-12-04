// Copyright (c) 1998-1999 Matra Datavision
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

#include <PBRep_PolygonOnTriangulation.ixx>


//=======================================================================
//function : PBRep_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PBRep_PolygonOnTriangulation::PBRep_PolygonOnTriangulation
(const Handle(PPoly_PolygonOnTriangulation)& aPPOT,
 const Handle(PPoly_Triangulation)&          aPPol,
 const PTopLoc_Location&                     aPLoc) :
 PBRep_CurveRepresentation(aPLoc),
 myPolygon(aPPOT),
 myTriangulation(aPPol)

{
}


//=======================================================================
//function : PBRep_PolygonOnTriangulation::IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnTriangulation::IsPolygonOnTriangulation() const 
{
  return Standard_True;
}


//=======================================================================
//function : PBRep_PolygonOnTriangulation::Triangulation
//purpose  : 
//=======================================================================

Handle(PPoly_Triangulation) PBRep_PolygonOnTriangulation::Triangulation() const 
{
  return myTriangulation;
}


//=======================================================================
//function : PBRep_CurveRepresentation::PolygonOnTriangulation
//purpose  : 
//=======================================================================

Handle(PPoly_PolygonOnTriangulation) 
     PBRep_PolygonOnTriangulation::PolygonOnTriangulation() const 
{
  return myPolygon;
}

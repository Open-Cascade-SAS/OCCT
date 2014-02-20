// Copyright (c) 1998-1999 Matra Datavision
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

#include <PBRep_PolygonOnSurface.ixx>


//=======================================================================
//function : PBRep_PolygonOnSurface
//purpose  : 
//=======================================================================

PBRep_PolygonOnSurface::PBRep_PolygonOnSurface
(const Handle(PPoly_Polygon2D)& aPPol2d,
 const Handle(PGeom_Surface)&   aPSurf,
 const PTopLoc_Location&        aPLoc) :
 PBRep_CurveRepresentation(aPLoc),
 myPolygon2D(aPPol2d),
 mySurface(aPSurf)
{
}


//=======================================================================
//function : PBRep_PolygonOnSurface::IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnSurface::IsPolygonOnSurface() const 
{
  return Standard_True;
}


//=======================================================================
//function : PBRep_PolygonOnSurface::Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface) PBRep_PolygonOnSurface::Surface() const 
{
  return mySurface;
}

//=======================================================================
//function : PBRep_PolygonOnSurface::Polygon
//purpose  : 
//=======================================================================

Handle(PPoly_Polygon2D) PBRep_PolygonOnSurface::Polygon() const 
{
  return myPolygon2D;
}

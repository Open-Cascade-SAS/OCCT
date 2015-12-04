// Created on: 1995-03-15
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRep_CurveRepresentation.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_PolygonOnClosedSurface,BRep_PolygonOnSurface)

//=======================================================================
//function : BRep_PolygonOnClosedSurface
//purpose  : 
//=======================================================================
BRep_PolygonOnClosedSurface::BRep_PolygonOnClosedSurface(const Handle(Poly_Polygon2D)& P1, 
							 const Handle(Poly_Polygon2D)& P2, 
							 const Handle(Geom_Surface)&   S, 
							 const TopLoc_Location&        L):
							 BRep_PolygonOnSurface(P1, S, L),
							 myPolygon2(P2)
{
}

//=======================================================================
//function : IsPolygonOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnClosedSurface::IsPolygonOnClosedSurface() const 
{
  return Standard_True;
}

//=======================================================================
//function : Polygon2
//purpose  : 
//=======================================================================

const Handle(Poly_Polygon2D)& BRep_PolygonOnClosedSurface::Polygon2() const 
{
  return myPolygon2;
}

//=======================================================================
//function : Polygon2
//purpose  : 
//=======================================================================

void BRep_PolygonOnClosedSurface::Polygon2(const Handle(Poly_Polygon2D)& P)
{
  myPolygon2 = P;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnClosedSurface::Copy() const 
{
  Handle(BRep_PolygonOnClosedSurface) P = 
    new BRep_PolygonOnClosedSurface(Polygon(), myPolygon2, Surface(), Location());
  return P;
}


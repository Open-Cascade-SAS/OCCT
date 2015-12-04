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
#include <BRep_PolygonOnSurface.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_PolygonOnSurface,BRep_CurveRepresentation)

//=======================================================================
//function : BRep_PolygonOnSurface
//purpose  : 
//=======================================================================
BRep_PolygonOnSurface::BRep_PolygonOnSurface(const Handle(Poly_Polygon2D)& P, 
					     const Handle(Geom_Surface)&   S, 
					     const TopLoc_Location&        L):
					     BRep_CurveRepresentation(L),
					     myPolygon2D(P),
					     mySurface(S)
{
}

//=======================================================================
//function : IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnSurface::IsPolygonOnSurface() const 
{
  return Standard_True;
}

//=======================================================================
//function : IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnSurface::IsPolygonOnSurface(const Handle(Geom_Surface)& S, 
							   const TopLoc_Location&      L) const 
{  
  return (S == mySurface) && (L == myLocation);
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)& BRep_PolygonOnSurface::Surface() const 
{
  return mySurface;
}

//=======================================================================
//function : Polygon
//purpose  : 
//=======================================================================

const Handle(Poly_Polygon2D)& BRep_PolygonOnSurface::Polygon() const 
{
  return myPolygon2D;
}

//=======================================================================
//function : Polygon
//purpose  : 
//=======================================================================

void BRep_PolygonOnSurface::Polygon(const Handle(Poly_Polygon2D)& P)
{
  myPolygon2D = P;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnSurface::Copy() const 
{
  Handle(BRep_PolygonOnSurface) P = new BRep_PolygonOnSurface(myPolygon2D,
							      mySurface,
							      Location());
  return P;
}


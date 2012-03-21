// Created on: 1995-03-15
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRep_PolygonOnClosedSurface.ixx>

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


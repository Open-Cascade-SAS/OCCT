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



#include <BRep_PolygonOnSurface.ixx>



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


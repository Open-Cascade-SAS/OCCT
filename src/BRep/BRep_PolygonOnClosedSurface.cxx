// File:	BRep_PolygonOnClosedSurface.cxx
// Created:	Wed Mar 15 15:44:40 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


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


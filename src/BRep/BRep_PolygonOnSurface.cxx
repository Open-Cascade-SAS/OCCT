// File:	BRep_PolygonOnSurface.cxx
// Created:	Wed Mar 15 13:53:11 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


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


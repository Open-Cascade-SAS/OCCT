// File:	PBRep_PointOnSurface.cxx
// Created:	Wed Aug 11 12:40:37 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


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


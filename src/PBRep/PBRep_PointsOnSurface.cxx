// File:	PBRep_PointsOnSurface.cxx
// Created:	Wed Aug 11 12:36:55 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <PBRep_PointsOnSurface.ixx>

//=======================================================================
//function : PBRep_PointsOnSurface
//purpose  : 
//=======================================================================

PBRep_PointsOnSurface::PBRep_PointsOnSurface
  (const Standard_Real P, 
   const Handle(PGeom_Surface)& S, 
   const PTopLoc_Location& L) :
  PBRep_PointRepresentation(P,L),
  mySurface(S)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_PointsOnSurface::Surface()const 
{
  return mySurface;
}



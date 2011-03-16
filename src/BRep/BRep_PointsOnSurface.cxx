// File:	BRep_PointsOnSurface.cxx
// Created:	Tue Aug 10 14:44:35 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRep_PointsOnSurface.ixx>


//=======================================================================
//function : BRep_PointsOnSurface
//purpose  : 
//=======================================================================

BRep_PointsOnSurface::BRep_PointsOnSurface(const Standard_Real P, 
					   const Handle(Geom_Surface)& S, 
					   const TopLoc_Location& L) :
       BRep_PointRepresentation(P,L),
       mySurface(S)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_PointsOnSurface::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

void  BRep_PointsOnSurface::Surface(const Handle(Geom_Surface)& S)
{
  mySurface = S;
}



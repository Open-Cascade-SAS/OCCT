// File:	BRep_PointOnSurface.cxx
// Created:	Tue Aug 10 14:45:11 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRep_PointOnSurface.ixx>


//=======================================================================
//function : BRep_PointOnSurface
//purpose  : 
//=======================================================================

BRep_PointOnSurface::BRep_PointOnSurface(const Standard_Real P1, 
					 const Standard_Real P2, 
					 const Handle(Geom_Surface)& S,
					 const TopLoc_Location& L) :
       BRep_PointsOnSurface(P1,S,L),
       myParameter2(P2)
{
}


//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnSurface::IsPointOnSurface()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnSurface::IsPointOnSurface
  (const Handle(Geom_Surface)& S, 
   const TopLoc_Location& L)const 
{
  return (Surface() == S) && (Location() == L);
}


//=======================================================================
//function : Parameter2
//purpose  : 
//=======================================================================

Standard_Real  BRep_PointOnSurface::Parameter2()const 
{
  return myParameter2;
}


//=======================================================================
//function : Parameter2
//purpose  : 
//=======================================================================

void  BRep_PointOnSurface::Parameter2(const Standard_Real P)
{
  myParameter2 = P;
}



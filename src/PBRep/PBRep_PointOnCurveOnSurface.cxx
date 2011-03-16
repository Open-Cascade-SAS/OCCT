// File:	PBRep_PointOnCurveOnSurface.cxx
// Created:	Wed Aug 11 12:39:26 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <PBRep_PointOnCurveOnSurface.ixx>


//=======================================================================
//function : PBRep_PointOnCurveOnSurface
//purpose  : 
//=======================================================================

PBRep_PointOnCurveOnSurface::PBRep_PointOnCurveOnSurface
  (const Standard_Real P, 
   const Handle(PGeom2d_Curve)& C, 
   const Handle(PGeom_Surface)& S, 
   const PTopLoc_Location& L) :
  PBRep_PointsOnSurface(P,S,L),
  myPCurve(C)
{
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PBRep_PointOnCurveOnSurface::PCurve()const 
{
  return myPCurve;
}

//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointOnCurveOnSurface::IsPointOnCurveOnSurface() const
{
  return Standard_True;
}


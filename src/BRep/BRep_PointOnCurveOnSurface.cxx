// File:	BRep_PointOnCurveOnSurface.cxx
// Created:	Tue Aug 10 14:44:59 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRep_PointOnCurveOnSurface.ixx>


//=======================================================================
//function : BRep_PointOnCurveOnSurface
//purpose  : 
//=======================================================================

BRep_PointOnCurveOnSurface::BRep_PointOnCurveOnSurface
  (const Standard_Real P, 
   const Handle(Geom2d_Curve)& C,
   const Handle(Geom_Surface)& S,
   const TopLoc_Location& L) :
  BRep_PointsOnSurface(P,S,L),
  myPCurve(C)
{
}


//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurveOnSurface::IsPointOnCurveOnSurface()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurveOnSurface::IsPointOnCurveOnSurface
  (const Handle(Geom2d_Curve)& PC,
   const Handle(Geom_Surface)& S, 
   const TopLoc_Location& L)const 
{
  return (myPCurve == PC) && (Surface() == S) && (Location() == L);
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_PointOnCurveOnSurface::PCurve()const 
{
  return myPCurve;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  BRep_PointOnCurveOnSurface::PCurve(const Handle(Geom2d_Curve)& C)
{
  myPCurve = C;
}



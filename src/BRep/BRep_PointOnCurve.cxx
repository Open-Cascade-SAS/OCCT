// File:	BRep_PointOnCurve.cxx
// Created:	Tue Aug 10 14:44:47 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRep_PointOnCurve.ixx>


//=======================================================================
//function : BRep_PointOnCurve
//purpose  : 
//=======================================================================

BRep_PointOnCurve::BRep_PointOnCurve(const Standard_Real P,
				     const Handle(Geom_Curve)& C, 
				     const TopLoc_Location& L) :
       BRep_PointRepresentation(P,L),
       myCurve(C)
{
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurve::IsPointOnCurve()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurve::IsPointOnCurve
  (const Handle(Geom_Curve)& C,
   const TopLoc_Location& L)const 
{
  return (myCurve == C) && (Location() == L);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_PointOnCurve::Curve()const 
{
  return myCurve;
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

void  BRep_PointOnCurve::Curve(const Handle(Geom_Curve)& C)
{
  myCurve = C;
}



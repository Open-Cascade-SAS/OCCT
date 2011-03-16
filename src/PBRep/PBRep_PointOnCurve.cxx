// File:	PBRep_PointOnCurve.cxx
// Created:	Wed Aug 11 12:38:02 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <PBRep_PointOnCurve.ixx>

//=======================================================================
//function : PBRep_PointOnCurve
//purpose  : 
//=======================================================================

PBRep_PointOnCurve::PBRep_PointOnCurve(const Standard_Real P,
				       const Handle(PGeom_Curve)& C, 
				       const PTopLoc_Location& L) :
       PBRep_PointRepresentation(P,L),
       myCurve(C)
{
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PBRep_PointOnCurve::Curve()const 
{
  return myCurve;
}

//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointOnCurve::IsPointOnCurve() const
{
  return Standard_True;
}


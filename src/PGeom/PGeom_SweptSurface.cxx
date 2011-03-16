// File:	PGeom_SweptSurface.cxx
// Created:	Thu Mar  4 10:53:28 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_SweptSurface.ixx>

//=======================================================================
//function : PGeom_SweptSurface
//purpose  : 
//=======================================================================

PGeom_SweptSurface::PGeom_SweptSurface()
{}


//=======================================================================
//function : PGeom_SweptSurface
//purpose  : 
//=======================================================================

PGeom_SweptSurface::PGeom_SweptSurface
  (const Handle(PGeom_Curve)& aBasisCurve,
   const gp_Dir& aDirection) :
  basisCurve(aBasisCurve),
  direction(aDirection)
{}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom_SweptSurface::BasisCurve
  (const Handle(PGeom_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PGeom_SweptSurface::BasisCurve() const 
{ return basisCurve; }


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

void  PGeom_SweptSurface::Direction(const gp_Dir& aDirection)
{ direction = aDirection; }


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir  PGeom_SweptSurface::Direction() const 
{ return direction; }

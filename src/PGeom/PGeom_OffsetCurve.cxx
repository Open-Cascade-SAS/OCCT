// File:	PGeom_OffsetCurve.cxx
// Created:	Thu Mar  4 10:01:30 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_OffsetCurve.ixx>

//=======================================================================
//function : PGeom_OffsetCurve
//purpose  : 
//=======================================================================

PGeom_OffsetCurve::PGeom_OffsetCurve()
{}


//=======================================================================
//function : PGeom_OffsetCurve
//purpose  : 
//=======================================================================

PGeom_OffsetCurve::PGeom_OffsetCurve
  (const Handle(PGeom_Curve)& aBasisCurve,
   const Standard_Real aOffsetValue,
   const gp_Dir& aOffsetDirection) :
  basisCurve(aBasisCurve),
  offsetDirection(aOffsetDirection),
  offsetValue(aOffsetValue)
{}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom_OffsetCurve::BasisCurve(const Handle(PGeom_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PGeom_OffsetCurve::BasisCurve() const 
{ return basisCurve; }


//=======================================================================
//function : OffsetDirection
//purpose  : 
//=======================================================================

void  PGeom_OffsetCurve::OffsetDirection(const gp_Dir& aOffsetDirection)
{ offsetDirection = aOffsetDirection; }


//=======================================================================
//function : OffsetDirection
//purpose  : 
//=======================================================================

gp_Dir  PGeom_OffsetCurve::OffsetDirection() const 
{ return offsetDirection; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

void  PGeom_OffsetCurve::OffsetValue(const Standard_Real aOffsetValue)
{ offsetValue = aOffsetValue; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real  PGeom_OffsetCurve::OffsetValue() const 
{ return offsetValue; }

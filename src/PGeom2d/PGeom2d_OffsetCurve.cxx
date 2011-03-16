// File:	PGeom2d_OffsetCurve.cxx
// Created:	Thu Mar  4 10:01:30 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_OffsetCurve.ixx>

//=======================================================================
//function : PGeom2d_OffsetCurve
//purpose  : 
//=======================================================================

PGeom2d_OffsetCurve::PGeom2d_OffsetCurve()
{}


//=======================================================================
//function : PGeom2d_OffsetCurve
//purpose  : 
//=======================================================================

PGeom2d_OffsetCurve::PGeom2d_OffsetCurve
  (const Handle(PGeom2d_Curve)& aBasisCurve,
   const Standard_Real aOffsetValue) :
  basisCurve(aBasisCurve),
  offsetValue(aOffsetValue)
{}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom2d_OffsetCurve::BasisCurve(const Handle(PGeom2d_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PGeom2d_OffsetCurve::BasisCurve() const 
{ return basisCurve; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

void  PGeom2d_OffsetCurve::OffsetValue(const Standard_Real aOffsetValue)
{ offsetValue = aOffsetValue; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_OffsetCurve::OffsetValue() const 
{ return offsetValue; }

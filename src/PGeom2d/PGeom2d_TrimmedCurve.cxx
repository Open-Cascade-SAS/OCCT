// File:	PGeom2d_TrimmedCurve.cxx
// Created:	Thu Mar  4 11:06:35 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_TrimmedCurve.ixx>

//=======================================================================
//function : PGeom2d_TrimmedCurve
//purpose  : 
//=======================================================================

PGeom2d_TrimmedCurve::PGeom2d_TrimmedCurve()
{}


//=======================================================================
//function : PGeom2d_TrimmedCurve
//purpose  : 
//=======================================================================

PGeom2d_TrimmedCurve::PGeom2d_TrimmedCurve
  (const Handle(PGeom2d_Curve)& aBasisCurve,
   const Standard_Real aFirstU,
   const Standard_Real aLastU) :
  basisCurve(aBasisCurve),
  firstU(aFirstU),
  lastU(aLastU)
{}


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

void  PGeom2d_TrimmedCurve::FirstU(const Standard_Real aFirstU)
{ firstU = aFirstU; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_TrimmedCurve::FirstU() const 
{ return firstU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

void  PGeom2d_TrimmedCurve::LastU(const Standard_Real aLastU)
{ lastU = aLastU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_TrimmedCurve::LastU() const 
{ return lastU; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom2d_TrimmedCurve::BasisCurve(const Handle(PGeom2d_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PGeom2d_TrimmedCurve::BasisCurve() const 
{ return basisCurve; }

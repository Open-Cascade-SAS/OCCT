// File:	PGeom_TrimmedCurve.cxx
// Created:	Thu Mar  4 11:06:35 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_TrimmedCurve.ixx>

//=======================================================================
//function : PGeom_TrimmedCurve
//purpose  : 
//=======================================================================

PGeom_TrimmedCurve::PGeom_TrimmedCurve()
{}


//=======================================================================
//function : PGeom_TrimmedCurve
//purpose  : 
//=======================================================================

PGeom_TrimmedCurve::PGeom_TrimmedCurve
  (const Handle(PGeom_Curve)& aBasisCurve,
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

void  PGeom_TrimmedCurve::FirstU(const Standard_Real aFirstU)
{ firstU = aFirstU; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_TrimmedCurve::FirstU() const 
{ return firstU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

void  PGeom_TrimmedCurve::LastU(const Standard_Real aLastU)
{ lastU = aLastU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_TrimmedCurve::LastU() const 
{ return lastU; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom_TrimmedCurve::BasisCurve(const Handle(PGeom_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PGeom_TrimmedCurve::BasisCurve() const 
{ return basisCurve; }

// File:	PGeom_SurfaceOfRevolution.cxx
// Created:	Thu Mar  4 10:42:16 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_SurfaceOfRevolution.ixx>

//=======================================================================
//function : PGeom_SurfaceOfRevolution
//purpose  : 
//=======================================================================

PGeom_SurfaceOfRevolution::PGeom_SurfaceOfRevolution()
{}


//=======================================================================
//function : PGeom_SurfaceOfRevolution
//purpose  : 
//=======================================================================

PGeom_SurfaceOfRevolution::PGeom_SurfaceOfRevolution
  (const Handle(PGeom_Curve)& aBasisCurve,
   const gp_Dir& aDirection,
   const gp_Pnt& aLocation) :
  PGeom_SweptSurface(aBasisCurve, aDirection),
  location(aLocation)
{}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

void  PGeom_SurfaceOfRevolution::Location(const gp_Pnt& aLocation)
{ location = aLocation; }


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

gp_Pnt  PGeom_SurfaceOfRevolution::Location() const 
{ return location; }

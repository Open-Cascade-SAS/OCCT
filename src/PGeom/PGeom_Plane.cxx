// File:	PGeom_Plane.cxx
// Created:	Thu Mar  4 10:11:04 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_Plane.ixx>

//=======================================================================
//function : PGeom_Plane
//purpose  : 
//=======================================================================

PGeom_Plane::PGeom_Plane()
{}


//=======================================================================
//function : PGeom_Plane
//purpose  : 
//=======================================================================

PGeom_Plane::PGeom_Plane
  (const gp_Ax3& aPosition) :
  PGeom_ElementarySurface(aPosition)
{}

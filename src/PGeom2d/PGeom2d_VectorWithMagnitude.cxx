// File:	PGeom2d_VectorWithMagnitude.cxx
// Created:	Thu Mar  4 11:08:17 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_VectorWithMagnitude.ixx>

//=======================================================================
//function : PGeom2d_VectorWithMagnitude
//purpose  : 
//=======================================================================

PGeom2d_VectorWithMagnitude::PGeom2d_VectorWithMagnitude()
{}


//=======================================================================
//function : PGeom2d_VectorWithMagnitude
//purpose  : 
//=======================================================================

PGeom2d_VectorWithMagnitude::PGeom2d_VectorWithMagnitude
  (const gp_Vec2d& aVec) :
       PGeom2d_Vector(aVec)
{}

// File:	PGeom_VectorWithMagnitude.cxx
// Created:	Thu Mar  4 11:08:17 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_VectorWithMagnitude.ixx>

//=======================================================================
//function : PGeom_VectorWithMagnitude
//purpose  : 
//=======================================================================

PGeom_VectorWithMagnitude::PGeom_VectorWithMagnitude()
{}


//=======================================================================
//function : PGeom_VectorWithMagnitude
//purpose  : 
//=======================================================================

PGeom_VectorWithMagnitude::PGeom_VectorWithMagnitude(const gp_Vec& aVec) :
       PGeom_Vector(aVec)
{}

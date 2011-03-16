// File:	PGeom_Vector.cxx
// Created:	Thu Mar  4 11:07:18 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_Vector.ixx>

//=======================================================================
//function : PGeom_Vector
//purpose  : 
//=======================================================================

PGeom_Vector::PGeom_Vector(const gp_Vec& aVec) :
       vec(aVec)
{}


//=======================================================================
//function : PGeom_Vector
//purpose  : 
//=======================================================================

PGeom_Vector::PGeom_Vector()
{}


//=======================================================================
//function : Vec
//purpose  : 
//=======================================================================

void  PGeom_Vector::Vec(const gp_Vec& aVec)
{ vec = aVec; }


//=======================================================================
//function : Vec
//purpose  : 
//=======================================================================

gp_Vec  PGeom_Vector::Vec() const 
{ return vec; }

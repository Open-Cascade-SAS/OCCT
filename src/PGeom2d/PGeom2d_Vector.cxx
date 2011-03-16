// File:	PGeom2d_Vector.cxx
// Created:	Thu Mar  4 11:07:18 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Vector.ixx>

//=======================================================================
//function : PGeom2d_Vector
//purpose  : 
//=======================================================================

PGeom2d_Vector::PGeom2d_Vector(const gp_Vec2d& aVec) :
       vec(aVec)
{}


//=======================================================================
//function : PGeom2d_Vector
//purpose  : 
//=======================================================================

PGeom2d_Vector::PGeom2d_Vector()
{}


//=======================================================================
//function : Vec
//purpose  : 
//=======================================================================

void  PGeom2d_Vector::Vec(const gp_Vec2d& aVec)
{ vec = aVec; }


//=======================================================================
//function : Vec
//purpose  : 
//=======================================================================

gp_Vec2d PGeom2d_Vector::Vec() const 
{ return vec; }

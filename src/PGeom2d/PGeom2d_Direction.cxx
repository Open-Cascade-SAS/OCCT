// File:	PGeom2d_Direction.cxx
// Created:	Wed Mar  3 17:40:27 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Direction.ixx>

//=======================================================================
//function : PGeom2d_Direction
//purpose  : 
//=======================================================================

PGeom2d_Direction::PGeom2d_Direction()
{}


//=======================================================================
//function : PGeom2d_Direction
//purpose  : 
//=======================================================================

PGeom2d_Direction::PGeom2d_Direction(const gp_Vec2d& aVec) :
       PGeom2d_Vector(aVec)
{}

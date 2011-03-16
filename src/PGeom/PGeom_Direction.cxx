// File:	PGeom_Direction.cxx
// Created:	Wed Mar  3 17:40:27 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Direction.ixx>

//=======================================================================
//function : PGeom_Direction
//purpose  : 
//=======================================================================

PGeom_Direction::PGeom_Direction()
{}


//=======================================================================
//function : PGeom_Direction
//purpose  : 
//=======================================================================

PGeom_Direction::PGeom_Direction(const gp_Vec& aVec) :
       PGeom_Vector(aVec)
{}

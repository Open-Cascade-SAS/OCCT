// File:	PGeom2d_Conic.cxx
// Created:	Wed Mar  3 17:33:38 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Conic.ixx>

//=======================================================================
//function : PGeom2d_Conic
//purpose  : 
//=======================================================================

PGeom2d_Conic::PGeom2d_Conic(const gp_Ax22d& aPosition) :
       position(aPosition)
{}


//=======================================================================
//function : PGeom2d_Conic
//purpose  : 
//=======================================================================

PGeom2d_Conic::PGeom2d_Conic()
{}


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

void  PGeom2d_Conic::Position(const gp_Ax22d& aPosition)
{ position = aPosition; }


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

gp_Ax22d PGeom2d_Conic::Position() const 
{ return position; }

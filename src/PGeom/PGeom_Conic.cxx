// File:	PGeom_Conic.cxx
// Created:	Wed Mar  3 17:33:38 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Conic.ixx>

//=======================================================================
//function : PGeom_Conic
//purpose  : 
//=======================================================================

PGeom_Conic::PGeom_Conic(const gp_Ax2& aPosition) :
       position(aPosition)
{}


//=======================================================================
//function : PGeom_Conic
//purpose  : 
//=======================================================================

PGeom_Conic::PGeom_Conic()
{}


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

void  PGeom_Conic::Position(const gp_Ax2& aPosition)
{ position = aPosition; }


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

gp_Ax2  PGeom_Conic::Position() const 
{ return position; }

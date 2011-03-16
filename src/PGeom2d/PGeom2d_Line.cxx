// File:	PGeom2d_Line.cxx
// Created:	Wed Mar  3 18:17:29 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Line.ixx>

//=======================================================================
//function : PGeom2d_Line
//purpose  : 
//=======================================================================

PGeom2d_Line::PGeom2d_Line()
{}


//=======================================================================
//function : PGeom2d_Line
//purpose  : 
//=======================================================================

PGeom2d_Line::PGeom2d_Line(const gp_Ax2d& aPosition) :
       position(aPosition)
{}


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

void  PGeom2d_Line::Position(const gp_Ax2d& aPosition)
{ position = aPosition; }


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

gp_Ax2d PGeom2d_Line::Position() const
{ return position; }

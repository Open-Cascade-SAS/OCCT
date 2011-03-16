// File:	PGeom_Line.cxx
// Created:	Wed Mar  3 18:17:29 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Line.ixx>

//=======================================================================
//function : PGeom_Line
//purpose  : 
//=======================================================================

PGeom_Line::PGeom_Line()
{}


//=======================================================================
//function : PGeom_Line
//purpose  : 
//=======================================================================

PGeom_Line::PGeom_Line(const gp_Ax1& aPosition) :
       position(aPosition)
{}


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

void  PGeom_Line::Position(const gp_Ax1& aPosition)
{ position = aPosition; }


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

gp_Ax1  PGeom_Line::Position() const
{ return position; }

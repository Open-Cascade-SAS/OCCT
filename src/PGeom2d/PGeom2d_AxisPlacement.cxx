// File:	PGeom2d_AxisPlacement.cxx
// Created:	Wed Mar  3 14:31:38 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_AxisPlacement.ixx>

//=======================================================================
//function : PGeom2d_AxisPlacement
//purpose  : 
//=======================================================================

PGeom2d_AxisPlacement::PGeom2d_AxisPlacement(const gp_Ax2d& aAxis) :
       axis(aAxis)
{}


//=======================================================================
//function : PGeom2d_AxisPlacement
//purpose  : 
//=======================================================================

PGeom2d_AxisPlacement::PGeom2d_AxisPlacement()
{}


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

void  PGeom2d_AxisPlacement::Axis(const gp_Ax2d& aAxis)
{ axis = aAxis; }


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

gp_Ax2d PGeom2d_AxisPlacement::Axis() const 
{ return axis; }

// File:	PGeom_AxisPlacement.cxx
// Created:	Wed Mar  3 14:31:38 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_AxisPlacement.ixx>

//=======================================================================
//function : PGeom_AxisPlacement
//purpose  : 
//=======================================================================

PGeom_AxisPlacement::PGeom_AxisPlacement(const gp_Ax1& aAxis) :
       axis(aAxis)
{}


//=======================================================================
//function : PGeom_AxisPlacement
//purpose  : 
//=======================================================================

PGeom_AxisPlacement::PGeom_AxisPlacement()
{}


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

void  PGeom_AxisPlacement::Axis(const gp_Ax1& aAxis)
{ axis = aAxis; }


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

gp_Ax1  PGeom_AxisPlacement::Axis() const 
{ return axis; }

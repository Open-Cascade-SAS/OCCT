// File:	PGeom_Axis1Placement.cxx
// Created:	Wed Mar  3 12:18:43 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Axis1Placement.ixx>

//=======================================================================
//function : PGeom_Axis1Placement
//purpose  : 
//=======================================================================

PGeom_Axis1Placement::PGeom_Axis1Placement()
{}


//=======================================================================
//function : PGeom_Axis1Placement
//purpose  : 
//=======================================================================

PGeom_Axis1Placement::PGeom_Axis1Placement(const gp_Ax1& aAxis) :
       PGeom_AxisPlacement(aAxis)
{}

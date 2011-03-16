// File:	PGeom_Axis2Placement.cxx
// Created:	Wed Mar  3 12:38:41 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Axis2Placement.ixx>

//=======================================================================
//function : PGeom_Axis2Placement
//purpose  : 
//=======================================================================

PGeom_Axis2Placement::PGeom_Axis2Placement()
{}


//=======================================================================
//function : PGeom_Axis2Placement
//purpose  : 
//=======================================================================

PGeom_Axis2Placement::PGeom_Axis2Placement
  (const gp_Ax1& aAxis,
   const gp_Dir& aXDirection) :
  PGeom_AxisPlacement(aAxis),
  xDirection(aXDirection)
{}


//=======================================================================
//function : XDirection
//purpose  : 
//=======================================================================

void  PGeom_Axis2Placement::XDirection(const gp_Dir& aXDirection)
{ xDirection = aXDirection; }


//=======================================================================
//function : XDirection
//purpose  : 
//=======================================================================

gp_Dir  PGeom_Axis2Placement::XDirection() const 
{ return xDirection; }

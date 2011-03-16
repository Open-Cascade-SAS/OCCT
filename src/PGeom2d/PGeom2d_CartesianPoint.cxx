// File:	PGeom2d_CartesianPoint.cxx
// Created:	Wed Mar  3 16:39:53 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_CartesianPoint.ixx>

//=======================================================================
//function : PGeom2d_CartesianPoint
//purpose  : 
//=======================================================================

PGeom2d_CartesianPoint::PGeom2d_CartesianPoint ()
{}


//=======================================================================
//function : PGeom2d_CartesianPoint
//purpose  : 
//=======================================================================

PGeom2d_CartesianPoint::PGeom2d_CartesianPoint (const gp_Pnt2d& aPnt) :
       pnt(aPnt)
{}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

void  PGeom2d_CartesianPoint::Pnt(const gp_Pnt2d& aPnt)
{ pnt = aPnt; }


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt2d PGeom2d_CartesianPoint::Pnt() const 
{ return pnt; }

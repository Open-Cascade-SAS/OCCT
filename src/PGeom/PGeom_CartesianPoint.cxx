// File:	PGeom_CartesianPoint.cxx
// Created:	Wed Mar  3 16:39:53 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_CartesianPoint.ixx>

//=======================================================================
//function : PGeom_CartesianPoint
//purpose  : 
//=======================================================================

PGeom_CartesianPoint::PGeom_CartesianPoint ()
{}


//=======================================================================
//function : PGeom_CartesianPoint
//purpose  : 
//=======================================================================

PGeom_CartesianPoint::PGeom_CartesianPoint (const gp_Pnt& aPnt) :
       pnt(aPnt)
{}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

void  PGeom_CartesianPoint::Pnt(const gp_Pnt& aPnt)
{ pnt = aPnt; }


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt  PGeom_CartesianPoint::Pnt() const 
{ return pnt; }

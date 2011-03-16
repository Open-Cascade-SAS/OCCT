// File:	PGeom_Circle.cxx
// Created:	Wed Mar  3 17:31:21 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Circle.ixx>

//=======================================================================
//function : PGeom_Circle
//purpose  : 
//=======================================================================

PGeom_Circle::PGeom_Circle()
{}


//=======================================================================
//function : PGeom_Circle
//purpose  : 
//=======================================================================

PGeom_Circle::PGeom_Circle
  (const gp_Ax2& aPosition,
   const Standard_Real aRadius) :
  PGeom_Conic(aPosition),
  radius(aRadius)
{}


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

void  PGeom_Circle::Radius(const Standard_Real aRadius)
{ radius = aRadius; }


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_Circle::Radius() const 
{ return radius; }

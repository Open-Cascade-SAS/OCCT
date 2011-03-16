// File:	PGeom2d_Circle.cxx
// Created:	Wed Mar  3 17:31:21 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Circle.ixx>

//=======================================================================
//function : PGeom2d_Circle
//purpose  : 
//=======================================================================

PGeom2d_Circle::PGeom2d_Circle()
{}


//=======================================================================
//function : PGeom2d_Circle
//purpose  : 
//=======================================================================

PGeom2d_Circle::PGeom2d_Circle
  (const gp_Ax22d& aPosition,
   const Standard_Real aRadius) :
  PGeom2d_Conic(aPosition),
  radius(aRadius)
{}


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

void  PGeom2d_Circle::Radius(const Standard_Real aRadius)
{ radius = aRadius; }


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_Circle::Radius() const 
{ return radius; }

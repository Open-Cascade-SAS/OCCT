// File:	PGeom2d_Ellipse.cxx
// Created:	Wed Mar  3 18:09:05 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Ellipse.ixx>

//=======================================================================
//function : PGeom2d_Ellipse
//purpose  : 
//=======================================================================

PGeom2d_Ellipse::PGeom2d_Ellipse()
{}


//=======================================================================
//function : PGeom2d_Ellipse
//purpose  : 
//=======================================================================

PGeom2d_Ellipse::PGeom2d_Ellipse
  (const gp_Ax22d& aPosition,
   const Standard_Real aMajorRadius,
   const Standard_Real aMinorRadius) :
  PGeom2d_Conic(aPosition),
  majorRadius(aMajorRadius),
  minorRadius(aMinorRadius)
{}


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

void  PGeom2d_Ellipse::MajorRadius(const Standard_Real aMajorRadius)
{ majorRadius = aMajorRadius; }


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_Ellipse::MajorRadius() const 
{ return majorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

void  PGeom2d_Ellipse::MinorRadius(const Standard_Real aMinorRadius)
{ minorRadius = aMinorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_Ellipse::MinorRadius() const 
{ return minorRadius; }

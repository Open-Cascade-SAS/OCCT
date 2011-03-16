// File:	PGeom_Ellipse.cxx
// Created:	Wed Mar  3 18:09:05 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Ellipse.ixx>

//=======================================================================
//function : PGeom_Ellipse
//purpose  : 
//=======================================================================

PGeom_Ellipse::PGeom_Ellipse()
{}


//=======================================================================
//function : PGeom_Ellipse
//purpose  : 
//=======================================================================

PGeom_Ellipse::PGeom_Ellipse
  (const gp_Ax2& aPosition,
   const Standard_Real aMajorRadius,
   const Standard_Real aMinorRadius) :
  PGeom_Conic(aPosition),
  majorRadius(aMajorRadius),
  minorRadius(aMinorRadius)
{}


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

void  PGeom_Ellipse::MajorRadius(const Standard_Real aMajorRadius)
{ majorRadius = aMajorRadius; }


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_Ellipse::MajorRadius() const 
{ return majorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

void  PGeom_Ellipse::MinorRadius(const Standard_Real aMinorRadius)
{ minorRadius = aMinorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_Ellipse::MinorRadius() const 
{ return minorRadius; }

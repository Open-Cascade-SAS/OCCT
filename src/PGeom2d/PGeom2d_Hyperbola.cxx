// File:	PGeom2d_Hyperbola.cxx
// Created:	Wed Mar  3 18:12:07 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Hyperbola.ixx>

//=======================================================================
//function : PGeom2d_Hyperbola
//purpose  : 
//=======================================================================

PGeom2d_Hyperbola::PGeom2d_Hyperbola()
{}


//=======================================================================
//function : PGeom2d_Hyperbola
//purpose  : 
//=======================================================================

PGeom2d_Hyperbola::PGeom2d_Hyperbola
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

void  PGeom2d_Hyperbola::MajorRadius(const Standard_Real aMajorRadius)
{ majorRadius = aMajorRadius; }


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_Hyperbola::MajorRadius() const 
{ return majorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

void  PGeom2d_Hyperbola::MinorRadius(const Standard_Real aMinorRadius)
{ minorRadius = aMinorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_Hyperbola::MinorRadius() const 
{ return minorRadius; }

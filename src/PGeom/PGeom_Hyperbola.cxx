// File:	PGeom_Hyperbola.cxx
// Created:	Wed Mar  3 18:12:07 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_Hyperbola.ixx>

//=======================================================================
//function : PGeom_Hyperbola
//purpose  : 
//=======================================================================

PGeom_Hyperbola::PGeom_Hyperbola()
{}


//=======================================================================
//function : PGeom_Hyperbola
//purpose  : 
//=======================================================================

PGeom_Hyperbola::PGeom_Hyperbola
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

void  PGeom_Hyperbola::MajorRadius(const Standard_Real aMajorRadius)
{ majorRadius = aMajorRadius; }


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_Hyperbola::MajorRadius() const 
{ return majorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

void  PGeom_Hyperbola::MinorRadius(const Standard_Real aMinorRadius)
{ minorRadius = aMinorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_Hyperbola::MinorRadius() const 
{ return minorRadius; }

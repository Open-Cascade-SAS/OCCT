// File:	PGeom_ToroidalSurface.cxx
// Created:	Thu Mar  4 10:55:31 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_ToroidalSurface.ixx>

//=======================================================================
//function : PGeom_ToroidalSurface
//purpose  : 
//=======================================================================

PGeom_ToroidalSurface::PGeom_ToroidalSurface()
{}


//=======================================================================
//function : PGeom_ToroidalSurface
//purpose  : 
//=======================================================================

PGeom_ToroidalSurface::PGeom_ToroidalSurface
  (const gp_Ax3& aPosition,
   const Standard_Real aMajorRadius,
   const Standard_Real aMinorRadius) :
  PGeom_ElementarySurface(aPosition),
  majorRadius(aMajorRadius),
  minorRadius(aMinorRadius)
{}


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

void  PGeom_ToroidalSurface::MajorRadius(const Standard_Real aMajorRadius)
{ majorRadius = aMajorRadius; }


//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_ToroidalSurface::MajorRadius() const 
{ return majorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

void  PGeom_ToroidalSurface::MinorRadius(const Standard_Real aMinorRadius)
{ minorRadius = aMinorRadius; }


//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_ToroidalSurface::MinorRadius() const 
{ return minorRadius; }

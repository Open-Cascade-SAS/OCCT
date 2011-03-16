// File:	PGeom_ConicalSurface.cxx
// Created:	Wed Mar  3 17:34:58 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_ConicalSurface.ixx>

//=======================================================================
//function : PGeom_ConicalSurface
//purpose  : 
//=======================================================================

PGeom_ConicalSurface::PGeom_ConicalSurface()
{}


//=======================================================================
//function : PGeom_ConicalSurface
//purpose  : 
//=======================================================================

PGeom_ConicalSurface::PGeom_ConicalSurface
  (const gp_Ax3& aPosition,
   const Standard_Real aRadius,
   const Standard_Real aSemiAngle) :
  PGeom_ElementarySurface(aPosition),
  radius(aRadius),
  semiAngle(aSemiAngle)
{}


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

void  PGeom_ConicalSurface::Radius(const Standard_Real aRadius)
{ radius = aRadius; }


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_ConicalSurface::Radius() const 
{ return radius; }


//=======================================================================
//function : SemiAngle
//purpose  : 
//=======================================================================

void  PGeom_ConicalSurface::SemiAngle(const Standard_Real aSemiAngle)
{ semiAngle = aSemiAngle; }


//=======================================================================
//function : SemiAngle
//purpose  : 
//=======================================================================

Standard_Real  PGeom_ConicalSurface::SemiAngle() const 
{ return semiAngle; }

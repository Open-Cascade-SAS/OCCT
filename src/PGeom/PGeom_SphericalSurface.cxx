// File:	PGeom_SphericalSurface.cxx
// Created:	Thu Mar  4 10:27:24 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_SphericalSurface.ixx>

//=======================================================================
//function : PGeom_SphericalSurface
//purpose  : 
//=======================================================================

PGeom_SphericalSurface::PGeom_SphericalSurface()
{}


//=======================================================================
//function : PGeom_SphericalSurface
//purpose  : 
//=======================================================================

PGeom_SphericalSurface::PGeom_SphericalSurface
  (const gp_Ax3& aPosition,
   const Standard_Real aRadius) :
  PGeom_ElementarySurface(aPosition),
  radius(aRadius)
{}


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

void  PGeom_SphericalSurface::Radius(const Standard_Real aRadius)
{ radius = aRadius; }


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_SphericalSurface::Radius() const 
{ return radius; }

// File:	PGeom_CylindricalSurface.cxx
// Created:	Wed Mar  3 17:38:33 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_CylindricalSurface.ixx>

//=======================================================================
//function : PGeom_CylindricalSurface
//purpose  : 
//=======================================================================

PGeom_CylindricalSurface::PGeom_CylindricalSurface()
{}


//=======================================================================
//function : PGeom_CylindricalSurface
//purpose  : 
//=======================================================================

PGeom_CylindricalSurface::PGeom_CylindricalSurface
  (const gp_Ax3& aPosition,
   const Standard_Real aRadius) :
  PGeom_ElementarySurface(aPosition),
  radius(aRadius)
{}


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

void  PGeom_CylindricalSurface::Radius(const Standard_Real aRadius)
{ radius = aRadius; }


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_CylindricalSurface::Radius() const 
{ return radius; }

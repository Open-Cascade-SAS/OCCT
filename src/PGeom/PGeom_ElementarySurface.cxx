// File:	PGeom_ElementarySurface.cxx
// Created:	Wed Mar  3 18:07:27 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_ElementarySurface.ixx>

//=======================================================================
//function : PGeom_ElementarySurface
//purpose  : 
//=======================================================================

PGeom_ElementarySurface::PGeom_ElementarySurface()
{}


//=======================================================================
//function : PGeom_ElementarySurface
//purpose  : 
//=======================================================================

PGeom_ElementarySurface::PGeom_ElementarySurface
  (const gp_Ax3& aPosition) :
  position(aPosition)
{}


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

void  PGeom_ElementarySurface::Position(const gp_Ax3& aPosition)
{ position = aPosition; }


//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

gp_Ax3  PGeom_ElementarySurface::Position() const 
{ return position; }



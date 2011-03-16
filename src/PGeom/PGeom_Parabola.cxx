// File:	PGeom_Parabola.cxx
// Created:	Thu Mar  4 10:08:42 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_Parabola.ixx>

//=======================================================================
//function : PGeom_Parabola
//purpose  : 
//=======================================================================

PGeom_Parabola::PGeom_Parabola()
{}


//=======================================================================
//function : PGeom_Parabola
//purpose  : 
//=======================================================================

PGeom_Parabola::PGeom_Parabola
  (const gp_Ax2& aPosition,
   const Standard_Real aFocalLength) :
  PGeom_Conic(aPosition),
  focalLength(aFocalLength)
{}


//=======================================================================
//function : FocalLength
//purpose  : 
//=======================================================================

void  PGeom_Parabola::FocalLength(const Standard_Real aFocalLength)
{ focalLength = aFocalLength; }


//=======================================================================
//function : FocalLength
//purpose  : 
//=======================================================================

Standard_Real  PGeom_Parabola::FocalLength() const 
{ return focalLength; }

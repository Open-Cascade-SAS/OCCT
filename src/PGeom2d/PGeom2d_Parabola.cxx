// File:	PGeom2d_Parabola.cxx
// Created:	Thu Mar  4 10:08:42 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Parabola.ixx>

//=======================================================================
//function : PGeom2d_Parabola
//purpose  : 
//=======================================================================

PGeom2d_Parabola::PGeom2d_Parabola()
{}


//=======================================================================
//function : PGeom2d_Parabola
//purpose  : 
//=======================================================================

PGeom2d_Parabola::PGeom2d_Parabola
  (const gp_Ax22d& aPosition,
   const Standard_Real aFocalLength) :
  PGeom2d_Conic(aPosition),
  focalLength(aFocalLength)
{}


//=======================================================================
//function : FocalLength
//purpose  : 
//=======================================================================

void  PGeom2d_Parabola::FocalLength(const Standard_Real aFocalLength)
{ focalLength = aFocalLength; }


//=======================================================================
//function : FocalLength
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_Parabola::FocalLength() const 
{ return focalLength; }

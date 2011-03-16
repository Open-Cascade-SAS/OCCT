// File:	Geom_SweptSurface.cxx
// Created:	Wed Mar 10 10:55:23 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_SweptSurface.cxx, JCV 17/02/91

#include <Geom_SweptSurface.ixx>

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_SweptSurface::Continuity () const { return smooth; }

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

const gp_Dir& Geom_SweptSurface::Direction () const  { return direction; }

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SweptSurface::BasisCurve () const 
{ 
  return basisCurve;
}

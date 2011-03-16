// File:	GC_MakeMirror.cxx
// Created:	Fri Oct  2 16:36:54 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeMirror.ixx>
#include <GC_MakeMirror.hxx>
#include <gp_Ax3.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une symetrie de Geom par rapport a un point.              +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Pnt&  Point ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Point);
}

//=========================================================================
//   Creation d une symetrie de Geom par rapport a une droite.            +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Ax1&  Axis ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Axis);
}

//=========================================================================
//   Creation d une symetrie de Geom par rapport a une droite.            +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Lin&  Line ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(gp_Ax1(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie 3d de Geom par rapport a une droite definie  +
//   par un point et une direction.                                       +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Pnt&  Point ,
					const gp_Dir&  Direc ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(gp_Ax1(Point,Direc));
}

//=========================================================================
//   Creation d une symetrie 3d de Geom par rapport a un plan defini par  +
//   un Ax2 (Normale au plan et axe x du plan).                           +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Ax2&  Plane ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Plane);
}

//=========================================================================
//   Creation d une symetrie 3d de gp par rapport a un plan Plane.        +
//=========================================================================

GC_MakeMirror::GC_MakeMirror(const gp_Pln&  Plane ) {
  TheMirror = new Geom_Transformation();
  TheMirror->SetMirror(Plane.Position().Ax2());
}

const Handle(Geom_Transformation)& GC_MakeMirror::Value() const
{ 
  return TheMirror;
}

const Handle(Geom_Transformation)& GC_MakeMirror::Operator() const 
{
  return TheMirror;
}

GC_MakeMirror::operator Handle(Geom_Transformation) () const
{
  return TheMirror;
}


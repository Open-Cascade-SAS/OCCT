// File:	GCE2d_MakeMirror.cxx
// Created:	Fri Oct  2 16:36:54 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeMirror.ixx>

//=========================================================================
//   Creation d une symetrie de Geom2d par rapport a un point.              +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Pnt2d&  Point ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(Point);
}

//=========================================================================
//   Creation d une symetrie de Geom2d par rapport a une droite.            +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Ax2d&  Axis ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(Axis);
}

//=========================================================================
//   Creation d une symetrie de Geom2d par rapport a une droite.            +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Lin2d&  Line ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(gp_Ax2d(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie 3d de Geom2d par rapport a une droite definie  +
//   par un point et une direction.                                       +
//=========================================================================

GCE2d_MakeMirror::GCE2d_MakeMirror(const gp_Pnt2d&  Point ,
				   const gp_Dir2d&  Direc ) {
  TheMirror = new Geom2d_Transformation();
  TheMirror->SetMirror(gp_Ax2d(Point,Direc));
}

const Handle(Geom2d_Transformation)& GCE2d_MakeMirror::Value() const
{ 
  return TheMirror;
}

const Handle(Geom2d_Transformation)& GCE2d_MakeMirror::Operator() const 
{
  return TheMirror;
}

GCE2d_MakeMirror::operator Handle(Geom2d_Transformation) () const
{
  return TheMirror;
}


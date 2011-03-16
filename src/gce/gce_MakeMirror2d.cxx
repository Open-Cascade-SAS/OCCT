// File:	gce_MakeMirror2d.cxx
// Created:	Fri Sep  4 10:56:35 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeMirror2d.ixx>

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a un point.             +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Pnt2d&  Point ) 
{
  TheMirror2d.SetMirror(Point); 
}

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a une droite.           +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Ax2d& Axis ) 
{
  TheMirror2d.SetMirror(Axis); 
}

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a une droite.           +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Lin2d&  Line ) 
{
  TheMirror2d.SetMirror(gp_Ax2d(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a une droite definie    +
//   par un point et une direction.                                       +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Pnt2d&  Point ,
				   const gp_Dir2d&  Direc ) 
{
  TheMirror2d.SetMirror(gp_Ax2d(Point,Direc));
}

const gp_Trsf2d& gce_MakeMirror2d::Value() const 
{ 
  return TheMirror2d; 
}

const gp_Trsf2d& gce_MakeMirror2d::Operator() const 
{
  return TheMirror2d;
}

gce_MakeMirror2d::operator gp_Trsf2d() const
{
  return TheMirror2d;
}

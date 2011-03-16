// File:	gce_MakeMirror.cxx
// Created:	Fri Sep  4 10:56:35 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeMirror.ixx>
#include <gp_Ax3.hxx>

//=========================================================================
//   Creation d une symetrie  de gp par rapport a un point.             +
//=========================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Pnt&  Point ) 
{ 
  TheMirror.SetMirror(Point); 
}

//=========================================================================
//   Creation d une symetrie  de gp par rapport a une droite.           +
//=========================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Ax1& Axis ) 
{ 
  TheMirror.SetMirror(Axis); 
}

//=========================================================================
//   Creation d une symetrie  de gp par rapport a une droite.           +
//=========================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Lin&  Line ) 
{
  TheMirror.SetMirror(gp_Ax1(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie  de gp par rapport a une droite definie    +
//   par un point et une direction.                                       +
//=========================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Pnt&  Point ,
			       const gp_Dir&  Direc ) 
{
  TheMirror.SetMirror(gp_Ax1(Point,Direc));
}

//=========================================================================
//   Creation d une symetrie 3d de gp par rapport a un plan defini par    +
//   un Ax2 (Normale au plan et axe x du plan).                           +
//=========================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Ax2&  Plane ) 
{ 
  TheMirror.SetMirror(Plane); 
}

//=========================================================================
//   Creation d une symetrie 3d de gp par rapport a un plan Plane.        +
//=========================================================================

gce_MakeMirror::gce_MakeMirror(const gp_Pln& Plane ) 
{
  TheMirror.SetMirror(Plane.Position().Ax2());
}

const gp_Trsf& gce_MakeMirror::Value() const
{ 
  return TheMirror; 
}

const gp_Trsf& gce_MakeMirror::Operator() const 
{
  return TheMirror;
}

gce_MakeMirror::operator gp_Trsf() const
{
  return TheMirror;
}

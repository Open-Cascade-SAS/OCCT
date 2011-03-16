// File:	gce_MakeDir2d.cxx
// Created:	Wed Sep  2 11:35:00 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeDir2d.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================
//   Creation d une direction 2d (Dir2d) de gp a partir de 2 Pnt2d de gp. +
//=========================================================================

gce_MakeDir2d::gce_MakeDir2d(const gp_Pnt2d& P1,
			     const gp_Pnt2d& P2)
{
  if (P1.Distance(P2) <= gp::Resolution()) { TheError = gce_ConfusedPoints; }
  else {
    TheDir2d = gp_Dir2d(P2.XY()-P1.XY());
    TheError = gce_Done;
  }
}

gce_MakeDir2d::gce_MakeDir2d(const gp_XY& Coord)
{
  if (Coord.Modulus() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir2d = gp_Dir2d(Coord);
    TheError = gce_Done;
  }
}

gce_MakeDir2d::gce_MakeDir2d(const gp_Vec2d& V)
{
  if (V.Magnitude() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir2d = gp_Dir2d(V);
    TheError = gce_Done;
  }
}

gce_MakeDir2d::gce_MakeDir2d(const Standard_Real Xv,
			     const Standard_Real Yv)
{
  if (Xv*Xv+Yv*Yv <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir2d = gp_Dir2d(Xv,Yv);
    TheError = gce_Done;
  }
}

const gp_Dir2d& gce_MakeDir2d::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheDir2d;
}

const gp_Dir2d& gce_MakeDir2d::Operator() const 
{
  return Value();
}

gce_MakeDir2d::operator gp_Dir2d() const
{
  return Value();
}


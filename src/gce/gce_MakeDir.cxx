// File:	gce_MakeDir.cxx
// Created:	Wed Sep  2 11:35:00 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeDir.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================
//   Creation d une direction 3d (Dir) de gp a partir de 2 Pnt de gp.     +
//=========================================================================

gce_MakeDir::gce_MakeDir(const gp_Pnt& P1,
			 const gp_Pnt& P2)
{
  if (P1.Distance(P2) <= gp::Resolution()) { TheError = gce_ConfusedPoints; }
  else {
    TheDir = gp_Dir(P2.XYZ()-P1.XYZ());
    TheError = gce_Done;
  }
}

gce_MakeDir::gce_MakeDir(const gp_XYZ& Coord)
{
  if (Coord.Modulus() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir = gp_Dir(Coord);
    TheError = gce_Done;
  }
}

gce_MakeDir::gce_MakeDir(const gp_Vec& V)
{
  if (V.Magnitude() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir = gp_Dir(V);
    TheError = gce_Done;
  }
}

gce_MakeDir::gce_MakeDir(const Standard_Real Xv,
			 const Standard_Real Yv,
			 const Standard_Real Zv)
{
  if (Xv*Xv+Yv*Yv+Zv*Zv <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir = gp_Dir(Xv,Yv,Zv);
    TheError = gce_Done;
  }
}

const gp_Dir& gce_MakeDir::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheDir;
}

const gp_Dir& gce_MakeDir::Operator() const 
{
  return Value();
}

gce_MakeDir::operator gp_Dir() const
{
  return Value();
}


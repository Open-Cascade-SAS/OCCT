// File vgeom_Ax22d.cxx , JCV 30/08/90
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 Modif mineur suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 

#include <gp_Ax22d.ixx>

void gp_Ax22d::Mirror (const gp_Pnt2d& P)
{
  gp_Pnt2d Temp = point;
  Temp.Mirror (P);
  point = Temp;
  vxdir.Reverse ();
  vydir.Reverse ();
}

gp_Ax22d gp_Ax22d::Mirrored(const gp_Pnt2d& P) const
{
  gp_Ax22d Temp = *this;
  Temp.Mirror (P);
  return Temp;
}

void gp_Ax22d::Mirror (const gp_Ax2d& A1)
{
  vydir.Mirror (A1);
  vxdir.Mirror (A1);
  gp_Pnt2d Temp = point;
  Temp.Mirror (A1);
  point = Temp;
}

gp_Ax22d gp_Ax22d::Mirrored(const gp_Ax2d& A1) const
{
  gp_Ax22d Temp = *this;
  Temp.Mirror (A1);
  return Temp;
}


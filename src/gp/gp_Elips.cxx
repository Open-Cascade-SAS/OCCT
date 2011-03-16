//File gp_Elips.cxx JCV 06/10/90
//Modif JCV 12/12/90

#include <gp_Elips.ixx>

void gp_Elips::Mirror (const gp_Pnt& P)
{ pos.Mirror(P); }

gp_Elips gp_Elips::Mirrored (const gp_Pnt& P) const
{
  gp_Elips E = *this;
  E.pos.Mirror (P);
  return E; 
}

void gp_Elips::Mirror (const gp_Ax1& A1)
{ pos.Mirror(A1); }

gp_Elips gp_Elips::Mirrored (const gp_Ax1& A1) const
{
  gp_Elips E = *this;
  E.pos.Mirror (A1);
  return E; 
}

void gp_Elips::Mirror (const gp_Ax2& A2)
{ pos.Mirror(A2); }

gp_Elips gp_Elips::Mirrored (const gp_Ax2& A2) const
{
  gp_Elips E = *this;
  E.pos.Mirror (A2);
  return E; 
}


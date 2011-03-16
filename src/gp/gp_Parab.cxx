//File gp_Parab.cxx JCV 12/10/90
// Modif jcv 14/12/90 suite a la premiere revue de projet

#include <gp_Parab.ixx>

void gp_Parab::Mirror (const gp_Pnt& P)
{ pos.Mirror (P); }

gp_Parab gp_Parab::Mirrored (const gp_Pnt& P) const
{
  gp_Parab Prb = *this;
  Prb.pos.Mirror (P);
  return Prb;     
}

void gp_Parab::Mirror (const gp_Ax1& A1)
{ pos.Mirror (A1); }

gp_Parab gp_Parab::Mirrored (const gp_Ax1& A1) const
{
  gp_Parab Prb = *this;
  Prb.pos.Mirror (A1);
  return Prb;     
}

void gp_Parab::Mirror (const gp_Ax2& A2)
{ pos.Mirror (A2); }

gp_Parab gp_Parab::Mirrored (const gp_Ax2& A2) const
{
  gp_Parab Prb = *this;
  Prb.pos.Mirror (A2);
  return Prb;     
}


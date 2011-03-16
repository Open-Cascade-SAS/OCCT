//File gp_Hypr.cxx JCV 12/10/90
// Modif jcv 14/12/90 suite a la premiere revue de projet

#include <gp_Hypr.ixx>

void gp_Hypr::Mirror (const gp_Pnt& P)
{ pos.Mirror(P); }

gp_Hypr gp_Hypr::Mirrored (const gp_Pnt& P) const
{
  gp_Hypr H = *this;
  H.pos.Mirror(P);
  return H; 
}

void gp_Hypr::Mirror (const gp_Ax1& A1)
{ pos.Mirror(A1); }

gp_Hypr gp_Hypr::Mirrored (const gp_Ax1& A1) const
{
  gp_Hypr H = *this;
  H.pos.Mirror(A1);
  return H; 
}

void gp_Hypr::Mirror (const gp_Ax2& A2)
{ pos.Mirror(A2); }

gp_Hypr gp_Hypr::Mirrored (const gp_Ax2& A2) const
{
  gp_Hypr H = *this;
  H.pos.Mirror(A2);
  return H; 
}


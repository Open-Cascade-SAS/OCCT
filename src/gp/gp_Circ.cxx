//File gp_Circ.cxx JCV 06/10/90

#include <gp_Circ.ixx>

void gp_Circ::Mirror (const gp_Pnt& P)
{ pos.Mirror(P); }

gp_Circ gp_Circ::Mirrored (const gp_Pnt& P) const
{
  gp_Circ C = *this;
  C.pos.Mirror (P);
  return C; 
}

void gp_Circ::Mirror (const gp_Ax1& A1)
{ pos.Mirror(A1); }

gp_Circ gp_Circ::Mirrored (const gp_Ax1& A1) const
{
  gp_Circ C = *this;
  C.pos.Mirror (A1);
  return C; 
}

void gp_Circ::Mirror (const gp_Ax2& A2)
{ pos.Mirror(A2); }

gp_Circ gp_Circ::Mirrored (const gp_Ax2& A2) const
{
  gp_Circ C = *this;
  C.pos.Mirror (A2);
  return C; 
}


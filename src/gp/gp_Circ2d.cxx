//File gp_Circ2d.cxx FID 30/10/90
//modif JCV 10/01/91 suite a la deuxieme revue de projet toolkit geometry

#include <gp_Circ2d.ixx>

void gp_Circ2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror(P); }

gp_Circ2d gp_Circ2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Circ2d C = *this;
  C.pos.Mirror (P);
  return C; 
}

void gp_Circ2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror (A); }

gp_Circ2d gp_Circ2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Circ2d C = *this;
  C.pos.Mirror (A);
  return C; 
}


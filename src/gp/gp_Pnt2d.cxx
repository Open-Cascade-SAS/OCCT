// File gp_Pnt2d.cxx, JCV 06/90 
// File gp_Pnt2d.cxx, REG 26/10/90 nouvelle version
// JCV 08/01/91 Modifs introduction des classes XY, Mat2d dans le package gp

#define No_Standard_OutOfRange

#include <gp_Pnt2d.ixx>

void gp_Pnt2d::Transform (const gp_Trsf2d& T)
{
  if (T.Form () == gp_Identity) { }
  else if (T.Form () == gp_Translation)
    { coord.Add (T.TranslationPart ()); }
  else if (T.Form () == gp_Scale) {
    coord.Multiply (T.ScaleFactor ());
    coord.Add      (T.TranslationPart ());
  }
  else if (T.Form () == gp_PntMirror) {
    coord.Reverse ();
    coord.Add     (T.TranslationPart ());
  }
  else { T.Transforms(coord); }
}

void gp_Pnt2d::Mirror (const gp_Pnt2d& P)
{
  coord.Reverse ();
  gp_XY XY = P.coord;
  XY.Multiply (2.0);
  coord.Add (XY);
}

gp_Pnt2d gp_Pnt2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Pnt2d Pres = *this;
  Pres.Mirror (P);
  return Pres;
}

void gp_Pnt2d::Mirror (const gp_Ax2d& A)
{
  gp_Trsf2d T;
  T.SetMirror  (A);
  T.Transforms (coord);
}

gp_Pnt2d gp_Pnt2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Pnt2d P = *this;
  P.Mirror (A);
  return P;
}


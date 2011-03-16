//File gp_Parab2d.cxx JCV 10/01/91

#define No_Standard_OutOfRange

#include <gp_Parab2d.ixx>

gp_Parab2d::gp_Parab2d (const gp_Ax22d& D, 
			const gp_Pnt2d& F)
{
  gp_XY DCoord = D.XDirection().XY();
  gp_XY GCoord = D.YDirection().XY();
  gp_XY PCoord = D.Location().XY();
  gp_XY MCoord = F.XY();
  focalLength = DCoord.Dot ( MCoord.Subtracted (PCoord));
  if (focalLength < 0) focalLength = - focalLength;
  gp_XY N = GCoord;
  N.Multiply (focalLength);
  MCoord.Add (N);
  N.Reverse();
  pos = gp_Ax22d (gp_Pnt2d (MCoord), gp_Dir2d (N));
  focalLength = focalLength / 2.0;
}

gp_Parab2d::gp_Parab2d (const gp_Ax2d& D, 
			const gp_Pnt2d& F,
			const Standard_Boolean Sense)
{
  gp_XY DCoord = D.Direction().XY();
  gp_XY PCoord = D.Location().XY();
  gp_XY MCoord = F.XY();
  focalLength = DCoord.Dot ( MCoord.Subtracted (PCoord));
  if (focalLength < 0) focalLength = - focalLength;
  gp_XY N;
  if (Sense) N.SetCoord(DCoord.Y(), -DCoord.X());
  else  N.SetCoord(-DCoord.Y(), DCoord.X());
  N.Multiply (focalLength);
  MCoord.Add (N);
  N.Reverse();
  pos = gp_Ax22d (gp_Pnt2d (MCoord), gp_Dir2d (N),Sense);
  focalLength = focalLength / 2.0;
}

void gp_Parab2d::Coefficients
(Standard_Real& A, Standard_Real& B, Standard_Real& C,
 Standard_Real& D, Standard_Real& E, Standard_Real& F) const
{
  Standard_Real P = 2.0 * focalLength;
  gp_Trsf2d T;
  T.SetTransformation (pos.XAxis());
  Standard_Real T11 = T.Value (1, 1);
  Standard_Real T12 = T.Value (1, 2);
  Standard_Real T13 = T.Value (1, 3);
  Standard_Real T21 = T.Value (2, 1);
  Standard_Real T22 = T.Value (2, 2);
  Standard_Real T23 = T.Value (2, 3);
  A = T21 * T21;
  B = T22 * T22;
  C = T21 * T22;
  D = (T21 * T23) - (P * T11);
  E = (T22 * T23) - (P * T12);
  F = (T23 * T23) - (2.0 * P * T13);
}

void gp_Parab2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror (P); }

gp_Parab2d gp_Parab2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Parab2d Prb = *this;
  Prb.pos.Mirror (P);
  return Prb;     
}

void gp_Parab2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror (A); }

gp_Parab2d gp_Parab2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Parab2d Prb = *this;
  Prb.pos.Mirror (A);
  return Prb;     
}


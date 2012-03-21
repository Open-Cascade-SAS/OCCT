// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// JCV 08/01/90 Modifs suite a l'introduction des classes XY et Mat2d dans gp

#include <gp_Dir2d.ixx>

Standard_Real gp_Dir2d::Angle (const gp_Dir2d& Other) const
{
  //    Commentaires :
  //    Au dessus de 45 degres l'arccos donne la meilleur precision pour le
  //    calcul de l'angle. Sinon il vaut mieux utiliser l'arcsin.
  //    Les erreurs commises sont loin d'etre negligeables lorsque l'on est
  //    proche de zero ou de 90 degres.
  //    En 2D les valeurs angulaires sont comprises entre -PI et PI
  Standard_Real Cosinus = coord.Dot   (Other.coord);
  Standard_Real Sinus = coord.Crossed (Other.coord);
  if (Cosinus > -0.70710678118655 && Cosinus < 0.70710678118655) { 
    if (Sinus > 0.0) return   acos (Cosinus);
    else             return - acos (Cosinus);
  }
  else {
    if (Cosinus > 0.0)  return      asin (Sinus);
    else { 
      if (Sinus > 0.0) return  M_PI - asin (Sinus);
      else             return -M_PI - asin (Sinus);
    }
  }
}

void gp_Dir2d::Mirror (const gp_Ax2d& A2)
{
  const gp_XY& XY = A2.Direction().XY();
  Standard_Real A = XY.X();
  Standard_Real B = XY.Y();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real XX = ((2.0 * A * A) - 1.0) * X + M1 * Y;
  Standard_Real YY = M1 * X + ((2.0 * B * B) - 1.0) * Y;
  coord.SetCoord(XX,YY);
}

void gp_Dir2d::Transform (const gp_Trsf2d& T)
{
  if (T.Form() == gp_Identity || T.Form() == gp_Translation)    { }
  else if (T.Form() == gp_PntMirror) { coord.Reverse(); }
  else if (T.Form() == gp_Scale) {
    if (T.ScaleFactor() < 0.0) { coord.Reverse(); }
  }
  else {
    coord.Multiply (T.HVectorialPart());
    Standard_Real D = coord.Modulus();
    coord.Divide(D);
    if (T.ScaleFactor() < 0.0) { coord.Reverse(); }
  } 
}

void gp_Dir2d::Mirror (const gp_Dir2d& V)
{
  const gp_XY& XY = V.coord;
  Standard_Real A = XY.X();
  Standard_Real B = XY.Y();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real XX = ((2.0 * A * A) - 1.0) * X + M1 * Y;
  Standard_Real YY = M1 * X + ((2.0 * B * B) - 1.0) * Y;
  coord.SetCoord(XX,YY);
}

gp_Dir2d gp_Dir2d::Mirrored (const gp_Dir2d& V) const
{
  gp_Dir2d Vres = *this;
  Vres.Mirror (V);
  return Vres;
}

gp_Dir2d gp_Dir2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Dir2d V = *this;
  V.Mirror (A);
  return V;
}


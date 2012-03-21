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

// JCV 07/12/90 Modifs suite a l'introduction des classes XYZ et Mat dans gp

#include <gp_Dir.ixx>


Standard_Real gp_Dir::Angle (const gp_Dir& Other) const
{
  //    Commentaires :
  //    Au dessus de 45 degres l'arccos donne la meilleur precision pour le
  //    calcul de l'angle. Sinon il vaut mieux utiliser l'arcsin.
  //    Les erreurs commises sont loin d'etre negligeables lorsque l'on est
  //    proche de zero ou de 90 degres.
  //    En 3d les valeurs angulaires sont toujours positives et comprises entre
  //    0 et PI
  Standard_Real Cosinus = coord.Dot (Other.coord);
  if (Cosinus > -0.70710678118655 && Cosinus < 0.70710678118655)
    return acos (Cosinus);
  else {
    Standard_Real Sinus = (coord.Crossed (Other.coord)).Modulus ();
    if(Cosinus < 0.0)  return M_PI - asin (Sinus);
    else               return      asin (Sinus);
  }
}

Standard_Real gp_Dir::AngleWithRef (const gp_Dir& Other,
				    const gp_Dir& Vref) const
{
  Standard_Real Ang;
  gp_XYZ XYZ = coord.Crossed (Other.coord);
  Standard_Real Cosinus = coord.Dot(Other.coord);
  Standard_Real Sinus   = XYZ.Modulus ();
  if (Cosinus > -0.70710678118655 && Cosinus < 0.70710678118655)
    Ang =  acos (Cosinus);
  else {
    if(Cosinus < 0.0)  Ang = M_PI - asin (Sinus);
    else               Ang =      asin (Sinus);
  }
  if (XYZ.Dot (Vref.coord) >= 0.0)  return  Ang;
  else                              return -Ang;
} 

void gp_Dir::Mirror (const gp_Dir& V)
{
  const gp_XYZ& XYZ = V.coord;
  Standard_Real A = XYZ.X();
  Standard_Real B = XYZ.Y();
  Standard_Real C = XYZ.Z();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real Z = coord.Z();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real M2 = 2.0 * A * C;
  Standard_Real M3 = 2.0 * B * C;
  Standard_Real XX = ((2.0 * A * A) - 1.0) * X + M1 * Y + M2 * Z;
  Standard_Real YY = M1 * X + ((2.0 * B * B) - 1.0) * Y + M3 * Z;
  Standard_Real ZZ = M2 * X + M3 * Y + ((2.0 * C * C) - 1.0) * Z;
  coord.SetCoord(XX,YY,ZZ);
}

void gp_Dir::Mirror (const gp_Ax1& A1)
{
  const gp_XYZ& XYZ = A1.Direction().coord;
  Standard_Real A = XYZ.X();
  Standard_Real B = XYZ.Y();
  Standard_Real C = XYZ.Y();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real Z = coord.Z();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real M2 = 2.0 * A * C;
  Standard_Real M3 = 2.0 * B * C;
  Standard_Real XX = ((2.0 * A * A) - 1.0) * X + M1 * Y + M2 * Z;
  Standard_Real YY = M1 * X + ((2.0 * B * B) - 1.0) * Y + M3 * Z;
  Standard_Real ZZ = M2 * X + M3 * Y + ((2.0 * C * C) - 1.0) * Z;
  coord.SetCoord(XX,YY,ZZ);
}

void gp_Dir::Mirror (const gp_Ax2& A2)
{
  const gp_Dir& Vz = A2.Direction();
  Mirror(Vz);
  Reverse();
}

void gp_Dir::Transform (const gp_Trsf& T)
{
  if (T.Form() == gp_Identity ||  T.Form() == gp_Translation)    { }
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

gp_Dir gp_Dir::Mirrored (const gp_Dir& V) const
{
  gp_Dir Vres = *this;
  Vres.Mirror (V);
  return Vres;
}

gp_Dir gp_Dir::Mirrored (const gp_Ax1& A1) const
{
  gp_Dir V = *this;
  V.Mirror (A1);
  return V;
}

gp_Dir gp_Dir::Mirrored (const gp_Ax2& A2) const
{
  gp_Dir V = *this;
  V.Mirror (A2);
  return V;
}


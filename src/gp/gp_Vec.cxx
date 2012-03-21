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

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 07/12/90 Modifs suite a l'introduction des classes XYZ et Mat dans gp

#define No_Standard_OutOfRange

#include <gp_Vec.ixx>
#include <gp.hxx>

Standard_Boolean gp_Vec::IsEqual
(const gp_Vec& Other, 
 const Standard_Real LinearTolerance,
 const Standard_Real AngularTolerance) const
{
  if (Magnitude ()       <= LinearTolerance || 
      Other.Magnitude () <= LinearTolerance) {
    Standard_Real val = Magnitude() - Other.Magnitude();
    if (val < 0) val = - val;
    return val <= LinearTolerance;
  }
  else {
    Standard_Real val = Magnitude() - Other.Magnitude();
    if (val < 0) val = - val;
    return val <= LinearTolerance && Angle(Other) <= AngularTolerance;
  }
}    

void gp_Vec::Mirror (const gp_Vec& V)
{
  Standard_Real D = V.coord.Modulus();
  if (D > gp::Resolution()) {
    const gp_XYZ& XYZ = V.coord;
    Standard_Real A = XYZ.X() / D;
    Standard_Real B = XYZ.Y() / D;
    Standard_Real C = XYZ.Z() / D; 
    Standard_Real M1 = 2.0 * A * B;
    Standard_Real M2 = 2.0 * A * C;
    Standard_Real M3 = 2.0 * B * C;
    Standard_Real X = coord.X();
    Standard_Real Y = coord.Y();
    Standard_Real Z = coord.Z();
    coord.SetX(((2.0 * A * A) - 1.0) * X + M1 * Y + M2 * Z);
    coord.SetY(M1 * X + ((2.0 * B * B) - 1.0) * Y + M3 * Z);
    coord.SetZ(M2 * X + M3 * Y + ((2.0 * C * C) - 1.0) * Z);
  }
}

void gp_Vec::Mirror (const gp_Ax1& A1)
{
  const gp_XYZ& V = A1.Direction().XYZ();
  Standard_Real A = V.X();
  Standard_Real B = V.Y();
  Standard_Real C = V.Z();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real Z = coord.Z();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real M2 = 2.0 * A * C;
  Standard_Real M3 = 2.0 * B * C;
  coord.SetX(((2.0 * A * A) - 1.0) * X + M1 * Y + M2 * Z);
  coord.SetY(M1 * X + ((2.0 * B * B) - 1.0) * Y + M3 * Z);
  coord.SetZ(M2 * X + M3 * Y + ((2.0 * C * C) - 1.0) * Z);
}

void gp_Vec::Mirror (const gp_Ax2& A2)
{
  gp_XYZ Z      = A2.Direction().XYZ();
  gp_XYZ MirXYZ = Z.Crossed (coord);
  if (MirXYZ.Modulus() <= gp::Resolution()) { coord.Reverse(); }
  else {
    Z.Cross (MirXYZ);
    Mirror (Z);
  }
}

void gp_Vec::Transform(const gp_Trsf& T)
{
  if (T.Form() == gp_Identity || T.Form() == gp_Translation) { }
  else if (T.Form() == gp_PntMirror) { coord.Reverse(); }
  else if (T.Form() == gp_Scale) { coord.Multiply (T.ScaleFactor()); }
  else { coord.Multiply (T.VectorialPart()); }
}

gp_Vec gp_Vec::Mirrored (const gp_Vec& V) const
{
  gp_Vec Vres = *this;
  Vres.Mirror (V);
  return Vres;                     
}

gp_Vec gp_Vec::Mirrored (const gp_Ax1& A1) const
{
  gp_Vec Vres = *this;
  Vres.Mirror (A1);
  return Vres;                     
}

gp_Vec gp_Vec::Mirrored (const gp_Ax2& A2) const
{
  gp_Vec Vres = *this;
  Vres.Mirror (A2);
  return Vres;                     
}

// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// JCV 08/01/90 Modifs suite a l'introduction des classes XY et Mat2d dans gp

#define No_Standard_OutOfRange


#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Boolean gp_Vec2d::IsEqual
(const gp_Vec2d& Other, 
 const Standard_Real LinearTolerance,
 const Standard_Real AngularTolerance) const
{
  const Standard_Real theNorm = Magnitude();
  const Standard_Real theOtherNorm = Other.Magnitude();
  Standard_Real val = theNorm - theOtherNorm;
  if (val < 0.0) val = -val;
  // Check for equal lengths
  const Standard_Boolean isEqualLength = (val <= LinearTolerance);
  // Check for small vectors
  if (theNorm > LinearTolerance && theOtherNorm > LinearTolerance)
  {
    Standard_Real Ang = Angle(Other);
    if (Ang < 0.0) Ang = -Ang;
    // Check for zero angle
    return isEqualLength && (Ang <= AngularTolerance);
  }
  return isEqualLength;
}    

Standard_Real gp_Vec2d::Angle (const gp_Vec2d& Other) const
{
  //    Commentaires :
  //    Au dessus de 45 degres l'arccos donne la meilleur precision pour le
  //    calcul de l'angle. Sinon il vaut mieux utiliser l'arcsin.
  //    Les erreurs commises sont loin d'etre negligeables lorsque l'on est
  //    proche de zero ou de 90 degres.
  //    En 2D les valeurs angulaires sont comprises entre -PI et PI
  const Standard_Real theNorm = Magnitude();
  const Standard_Real theOtherNorm = Other.Magnitude();
  if (theNorm <= gp::Resolution() || theOtherNorm <= gp::Resolution())
    throw gp_VectorWithNullMagnitude();

  const Standard_Real D = theNorm * theOtherNorm;
  const Standard_Real Cosinus = coord.Dot   (Other.coord) / D;
  const Standard_Real Sinus = coord.Crossed (Other.coord) / D;
  if (Cosinus > -0.70710678118655 && Cosinus < 0.70710678118655)
  {
    if (Sinus > 0.0)  return  acos (Cosinus);
    else              return -acos (Cosinus); 
  }
  else
  {
    if (Cosinus > 0.0) return        asin (Sinus);
    else
    { 
      if (Sinus > 0.0) return   M_PI - asin (Sinus);
      else             return - M_PI - asin (Sinus);
    }
  }  
}

void gp_Vec2d::Mirror (const gp_Ax2d& A1)
{
  const gp_XY& XY = A1.Direction().XY();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real A = XY.X();
  Standard_Real B = XY.Y();
  Standard_Real M1 = 2.0 * A * B;
  coord.SetX(((2.0 * A * A) - 1.) * X + M1 * Y);
  coord.SetY(M1 * X + ((2. * B * B) - 1.0) * Y);
}

gp_Vec2d gp_Vec2d::Mirrored (const gp_Ax2d& A1) const
{
  gp_Vec2d Vres = *this;
  Vres.Mirror(A1);
  return Vres;                     
}

void gp_Vec2d::Transform (const gp_Trsf2d& T)
{
  if (T.Form() == gp_Identity || T.Form() == gp_Translation) { }
  else if (T.Form() == gp_PntMirror) coord.Reverse ();
  else if (T.Form() == gp_Scale)     coord.Multiply (T.ScaleFactor ());
  else                               coord.Multiply (T.VectorialPart ());
}

void gp_Vec2d::Mirror (const gp_Vec2d& V)
{
  const Standard_Real D = V.coord.Modulus();
  if (D > gp::Resolution())
  {
    const gp_XY& XY = V.coord;
    Standard_Real X = XY.X();
    Standard_Real Y = XY.Y();
    Standard_Real A = X / D;
    Standard_Real B = Y / D;
    Standard_Real M1 = 2.0 * A * B;
    coord.SetX(((2.0 * A * A) - 1.0) * X + M1 * Y);
    coord.SetY(M1 * X + ((2.0 * B * B) - 1.0) * Y);
  }
}

gp_Vec2d gp_Vec2d::Mirrored (const gp_Vec2d& V) const
{
  gp_Vec2d Vres = *this;
  Vres.Mirror(V);
  return Vres;                     
}

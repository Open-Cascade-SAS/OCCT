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

#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

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


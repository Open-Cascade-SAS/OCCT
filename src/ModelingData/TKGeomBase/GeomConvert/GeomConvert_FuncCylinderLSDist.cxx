// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2022 OPEN CASCADE SAS
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

#include <GeomConvert_FuncCylinderLSDist.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Vector.hxx>

//=================================================================================================

GeomConvert_FuncCylinderLSDist::GeomConvert_FuncCylinderLSDist(
  const Handle(TColgp_HArray1OfXYZ)& thePoints,
  const gp_Dir&                      theDir)
    : myPoints(thePoints),
      myDir(theDir)
{
}

//=================================================================================================

Standard_Integer GeomConvert_FuncCylinderLSDist::NbVariables() const
{
  return 4;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncCylinderLSDist::Value(const math_Vector& X, Standard_Real& F)
{
  gp_XYZ        aLoc(X(1), X(2), X(3));
  Standard_Real anR2 = X(4) * X(4);

  F = 0.;
  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    gp_Vec        aV(myPoints->Value(i) - aLoc);
    Standard_Real aD2 = aV.CrossSquareMagnitude(myDir);
    Standard_Real d   = aD2 - anR2;
    F += d * d;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncCylinderLSDist::Gradient(const math_Vector& X, math_Vector& G)

{
  gp_XYZ        aLoc(X(1), X(2), X(3));
  Standard_Real anR = X(4), anR2 = anR * anR;
  Standard_Real x = myDir.X(), y = myDir.Y(), z = myDir.Z();
  G.Init(0.);

  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    gp_Vec        aV(myPoints->Value(i) - aLoc);
    Standard_Real aD2 = aV.CrossSquareMagnitude(myDir);
    Standard_Real d   = aD2 - anR2;
    Standard_Real Dx0 = 2. * (aV.Z() * x - aV.X() * z) * z - 2. * (aV.X() * y - aV.Y() * x) * y;
    Standard_Real Dy0 = -2. * (aV.Y() * z - aV.Z() * y) * z + 2. * (aV.X() * y - aV.Y() * x) * x;
    Standard_Real Dz0 = 2. * (aV.Y() * z - aV.Z() * y) * y - 2. * (aV.Z() * x - aV.X() * z) * x;

    G(1) += d * Dx0;
    G(2) += d * Dy0;
    G(3) += d * Dz0;
    //
    G(4) += d;
  }

  G *= 2;
  G(6) *= -2. * anR;

  return Standard_True;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncCylinderLSDist::Values(const math_Vector& X,
                                                        Standard_Real&     F,
                                                        math_Vector&       G)
{
  gp_XYZ        aLoc(X(1), X(2), X(3));
  Standard_Real anR = X(4), anR2 = anR * anR;
  Standard_Real x = myDir.X(), y = myDir.Y(), z = myDir.Z();

  F = 0.;
  G.Init(0.);
  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    gp_Vec        aV(myPoints->Value(i) - aLoc);
    Standard_Real aD2 = aV.CrossSquareMagnitude(myDir);
    Standard_Real d   = aD2 - anR2;
    Standard_Real Dx0 = 2. * (aV.Z() * x - aV.X() * z) * z - 2. * (aV.X() * y - aV.Y() * x) * y;
    Standard_Real Dy0 = -2. * (aV.Y() * z - aV.Z() * y) * z + 2. * (aV.X() * y - aV.Y() * x) * x;
    Standard_Real Dz0 = 2. * (aV.Y() * z - aV.Z() * y) * y - 2. * (aV.Z() * x - aV.X() * z) * x;

    G(1) += d * Dx0;
    G(2) += d * Dy0;
    G(3) += d * Dz0;
    //
    G(4) += d;
    //
    F += d * d;
  }

  G *= 2;
  G(4) *= -2. * anR;

  return true;
}

// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomGridEval_Hyperbola.hxx>

#include <gp_Hypr.hxx>

#include <cmath>

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_Hyperbola::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNb = theParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNb);

  const gp_Hypr& aHypr   = myGeom->Hypr();
  const gp_Pnt&  aCenter = aHypr.Location();
  const gp_Dir&  aXDir   = aHypr.XAxis().Direction();
  const gp_Dir&  aYDir   = aHypr.YAxis().Direction();
  const double   aMajR   = aHypr.MajorRadius();
  const double   aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // P = Center + MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir
    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Pnt(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                            aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY,
                            aCZ + aMajR * coshU * aXZ + aMinR * sinhU * aYZ));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_Hyperbola::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNb);

  const gp_Hypr& aHypr   = myGeom->Hypr();
  const gp_Pnt&  aCenter = aHypr.Location();
  const gp_Dir&  aXDir   = aHypr.XAxis().Direction();
  const gp_Dir&  aYDir   = aHypr.YAxis().Direction();
  const double   aMajR   = aHypr.MajorRadius();
  const double   aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // P = Center + MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir
    // D1 = MajorR * sinh(u) * XDir + MinorR * cosh(u) * YDir

    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                       aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY,
                                       aCZ + aMajR * coshU * aXZ + aMinR * sinhU * aYZ),
                                gp_Vec(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                       aMajR * sinhU * aXY + aMinR * coshU * aYY,
                                       aMajR * sinhU * aXZ + aMinR * coshU * aYZ)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_Hyperbola::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNb);

  const gp_Hypr& aHypr   = myGeom->Hypr();
  const gp_Pnt&  aCenter = aHypr.Location();
  const gp_Dir&  aXDir   = aHypr.XAxis().Direction();
  const gp_Dir&  aYDir   = aHypr.YAxis().Direction();
  const double   aMajR   = aHypr.MajorRadius();
  const double   aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // P = Center + MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir
    // D1 = MajorR * sinh(u) * XDir + MinorR * cosh(u) * YDir
    // D2 = MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir = (P - Center)

    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                       aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY,
                                       aCZ + aMajR * coshU * aXZ + aMinR * sinhU * aYZ),
                                gp_Vec(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                       aMajR * sinhU * aXY + aMinR * coshU * aYY,
                                       aMajR * sinhU * aXZ + aMinR * coshU * aYZ),
                                gp_Vec(aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                       aMajR * coshU * aXY + aMinR * sinhU * aYY,
                                       aMajR * coshU * aXZ + aMinR * sinhU * aYZ)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_Hyperbola::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNb);

  const gp_Hypr& aHypr   = myGeom->Hypr();
  const gp_Pnt&  aCenter = aHypr.Location();
  const gp_Dir&  aXDir   = aHypr.XAxis().Direction();
  const gp_Dir&  aYDir   = aHypr.YAxis().Direction();
  const double   aMajR   = aHypr.MajorRadius();
  const double   aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // P = Center + MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir
    // D1 = MajorR * sinh(u) * XDir + MinorR * cosh(u) * YDir
    // D2 = MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir
    // D3 = MajorR * sinh(u) * XDir + MinorR * cosh(u) * YDir = D1

    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                       aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY,
                                       aCZ + aMajR * coshU * aXZ + aMinR * sinhU * aYZ),
                                gp_Vec(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                       aMajR * sinhU * aXY + aMinR * coshU * aYY,
                                       aMajR * sinhU * aXZ + aMinR * coshU * aYZ),
                                gp_Vec(aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                       aMajR * coshU * aXY + aMinR * sinhU * aYY,
                                       aMajR * coshU * aXZ + aMinR * sinhU * aYZ),
                                gp_Vec(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                       aMajR * sinhU * aXY + aMinR * coshU * aYY,
                                       aMajR * sinhU * aXZ + aMinR * coshU * aYZ)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_Hyperbola::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNb = theParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNb);

  const gp_Hypr& aHypr = myGeom->Hypr();
  const gp_Dir&  aXDir = aHypr.XAxis().Direction();
  const gp_Dir&  aYDir = aHypr.YAxis().Direction();
  const double   aMajR = aHypr.MajorRadius();
  const double   aMinR = aHypr.MinorRadius();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  // Hyperbola derivatives are cyclic with period 2:
  // D0 = MajR * cosh(u) * X + MinR * sinh(u) * Y  -> coefficients: (cosh, sinh)
  // D1 = MajR * sinh(u) * X + MinR * cosh(u) * Y  -> coefficients: (sinh, cosh)
  // D2 = D0, D3 = D1, etc.
  const bool isOdd = (theN % 2) == 1;

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    if (isOdd)
    {
      // Odd derivatives (D1, D3, D5, ...): coefficients (sinh, cosh)
      aResult.SetValue(i - theParams.Lower() + 1,
                       gp_Vec(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                              aMajR * sinhU * aXY + aMinR * coshU * aYY,
                              aMajR * sinhU * aXZ + aMinR * coshU * aYZ));
    }
    else
    {
      // Even derivatives (D2, D4, D6, ...): coefficients (cosh, sinh) = D0
      aResult.SetValue(i - theParams.Lower() + 1,
                       gp_Vec(aMajR * coshU * aXX + aMinR * sinhU * aYX,
                              aMajR * coshU * aXY + aMinR * sinhU * aYY,
                              aMajR * coshU * aXZ + aMinR * sinhU * aYZ));
    }
  }
  return aResult;
}

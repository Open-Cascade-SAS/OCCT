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

#include <Geom2dGridEval_Hyperbola.hxx>

#include <gp_Hypr2d.hxx>

#include <cmath>

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_Hyperbola::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Pnt2d> aResult(1, aNb);

  const gp_Hypr2d& aHypr   = myGeom->Hypr2d();
  const gp_Pnt2d&  aCenter = aHypr.Location();
  const gp_Dir2d   aXDir   = aHypr.XAxis().Direction();
  const gp_Dir2d   aYDir   = aHypr.YAxis().Direction();
  const double     aMajR   = aHypr.MajorRadius();
  const double     aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Pnt2d(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                              aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_Hyperbola::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, aNb);

  const gp_Hypr2d& aHypr   = myGeom->Hypr2d();
  const gp_Pnt2d&  aCenter = aHypr.Location();
  const gp_Dir2d   aXDir   = aHypr.XAxis().Direction();
  const gp_Dir2d   aYDir   = aHypr.YAxis().Direction();
  const double     aMajR   = aHypr.MajorRadius();
  const double     aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // P = Center + MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir
    // D1 = MajorR * sinh(u) * XDir + MinorR * cosh(u) * YDir

    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt2d(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                         aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY),
                                gp_Vec2d(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                         aMajR * sinhU * aXY + aMinR * coshU * aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_Hyperbola::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, aNb);

  const gp_Hypr2d& aHypr   = myGeom->Hypr2d();
  const gp_Pnt2d&  aCenter = aHypr.Location();
  const gp_Dir2d   aXDir   = aHypr.XAxis().Direction();
  const gp_Dir2d   aYDir   = aHypr.YAxis().Direction();
  const double     aMajR   = aHypr.MajorRadius();
  const double     aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // D2 = MajorR * cosh(u) * XDir + MinorR * sinh(u) * YDir = (P - Center)

    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt2d(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                         aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY),
                                gp_Vec2d(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                         aMajR * sinhU * aXY + aMinR * coshU * aYY),
                                gp_Vec2d(aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                         aMajR * coshU * aXY + aMinR * sinhU * aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_Hyperbola::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, aNb);

  const gp_Hypr2d& aHypr   = myGeom->Hypr2d();
  const gp_Pnt2d&  aCenter = aHypr.Location();
  const gp_Dir2d   aXDir   = aHypr.XAxis().Direction();
  const gp_Dir2d   aYDir   = aHypr.YAxis().Direction();
  const double     aMajR   = aHypr.MajorRadius();
  const double     aMinR   = aHypr.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u     = theParams.Value(i);
    const double coshU = std::cosh(u);
    const double sinhU = std::sinh(u);

    // D3 = MajorR * sinh(u) * XDir + MinorR * cosh(u) * YDir = D1

    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt2d(aCX + aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                         aCY + aMajR * coshU * aXY + aMinR * sinhU * aYY),
                                gp_Vec2d(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                         aMajR * sinhU * aXY + aMinR * coshU * aYY),
                                gp_Vec2d(aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                         aMajR * coshU * aXY + aMinR * sinhU * aYY),
                                gp_Vec2d(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                         aMajR * sinhU * aXY + aMinR * coshU * aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_Hyperbola::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Vec2d> aResult(1, aNb);

  const gp_Hypr2d& aHypr = myGeom->Hypr2d();
  const gp_Dir2d   aXDir = aHypr.XAxis().Direction();
  const gp_Dir2d   aYDir = aHypr.YAxis().Direction();
  const double     aMajR = aHypr.MajorRadius();
  const double     aMinR = aHypr.MinorRadius();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

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
      aResult.SetValue(i - theParams.Lower() + 1,
                       gp_Vec2d(aMajR * sinhU * aXX + aMinR * coshU * aYX,
                                aMajR * sinhU * aXY + aMinR * coshU * aYY));
    }
    else
    {
      aResult.SetValue(i - theParams.Lower() + 1,
                       gp_Vec2d(aMajR * coshU * aXX + aMinR * sinhU * aYX,
                                aMajR * coshU * aXY + aMinR * sinhU * aYY));
    }
  }
  return aResult;
}

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

#include <Geom2dGridEval_Parabola.hxx>

#include <gp_Parab2d.hxx>

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_Parabola::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Pnt2d> aResult(1, aNb);

  const gp_Parab2d& aParab  = myGeom->Parab2d();
  const gp_Pnt2d&   aCenter = aParab.Location();
  const gp_Dir2d    aXDir   = aParab.MirrorAxis().Direction();
  const gp_Dir2d    aYDir(-aXDir.Y(), aXDir.X());
  const double      aFocal = aParab.Focal();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  const double aCoeff = 1.0 / (4.0 * aFocal);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u = theParams.Value(i);

    // P = Center + (u^2 / 4F) * XDir + u * YDir
    const double u2Term = u * u * aCoeff;

    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Pnt2d(aCX + u2Term * aXX + u * aYX, aCY + u2Term * aXY + u * aYY));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_Parabola::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, aNb);

  const gp_Parab2d& aParab  = myGeom->Parab2d();
  const gp_Pnt2d&   aCenter = aParab.Location();
  const gp_Dir2d    aXDir   = aParab.MirrorAxis().Direction();
  const gp_Dir2d    aYDir(-aXDir.Y(), aXDir.X());
  const double      aFocal = aParab.Focal();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  const double aCoeff  = 1.0 / (4.0 * aFocal);
  const double aCoeff2 = 1.0 / (2.0 * aFocal);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u = theParams.Value(i);

    // P = Center + (u^2 / 4F) * XDir + u * YDir
    // D1 = (u / 2F) * XDir + YDir

    const double u2Term = u * u * aCoeff;
    const double d1Term = u * aCoeff2;

    aResult.ChangeValue(i - theParams.Lower() + 1) = {
      gp_Pnt2d(aCX + u2Term * aXX + u * aYX, aCY + u2Term * aXY + u * aYY),
      gp_Vec2d(d1Term * aXX + aYX, d1Term * aXY + aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_Parabola::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, aNb);

  const gp_Parab2d& aParab  = myGeom->Parab2d();
  const gp_Pnt2d&   aCenter = aParab.Location();
  const gp_Dir2d    aXDir   = aParab.MirrorAxis().Direction();
  const gp_Dir2d    aYDir(-aXDir.Y(), aXDir.X());
  const double      aFocal = aParab.Focal();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  const double aCoeff  = 1.0 / (4.0 * aFocal);
  const double aCoeff2 = 1.0 / (2.0 * aFocal);

  // D2 is constant for parabola: (1/2F) * XDir
  const gp_Vec2d aD2(aCoeff2 * aXX, aCoeff2 * aXY);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u = theParams.Value(i);

    const double u2Term = u * u * aCoeff;
    const double d1Term = u * aCoeff2;

    aResult.ChangeValue(i - theParams.Lower() + 1) = {
      gp_Pnt2d(aCX + u2Term * aXX + u * aYX, aCY + u2Term * aXY + u * aYY),
      gp_Vec2d(d1Term * aXX + aYX, d1Term * aXY + aYY),
      aD2};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_Parabola::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, aNb);

  const gp_Parab2d& aParab  = myGeom->Parab2d();
  const gp_Pnt2d&   aCenter = aParab.Location();
  const gp_Dir2d    aXDir   = aParab.MirrorAxis().Direction();
  const gp_Dir2d    aYDir(-aXDir.Y(), aXDir.X());
  const double      aFocal = aParab.Focal();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  const double aCoeff  = 1.0 / (4.0 * aFocal);
  const double aCoeff2 = 1.0 / (2.0 * aFocal);

  const gp_Vec2d aD2(aCoeff2 * aXX, aCoeff2 * aXY);
  const gp_Vec2d aD3(0.0, 0.0);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u = theParams.Value(i);

    const double u2Term = u * u * aCoeff;
    const double d1Term = u * aCoeff2;

    aResult.ChangeValue(i - theParams.Lower() + 1) = {
      gp_Pnt2d(aCX + u2Term * aXX + u * aYX, aCY + u2Term * aXY + u * aYY),
      gp_Vec2d(d1Term * aXX + aYX, d1Term * aXY + aYY),
      aD2,
      aD3};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_Parabola::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Vec2d> aResult(1, aNb);

  const gp_Parab2d& aParab = myGeom->Parab2d();
  const gp_Dir2d    aXDir  = aParab.MirrorAxis().Direction();
  const gp_Dir2d    aYDir(-aXDir.Y(), aXDir.X());
  const double      aFocal = aParab.Focal();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  const double aCoeff2 = 1.0 / (2.0 * aFocal);

  if (theN == 1)
  {
    // D1 = (u/2F) * X + Y (depends on u)
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double u      = theParams.Value(i);
      const double d1Term = u * aCoeff2;
      aResult.SetValue(i - theParams.Lower() + 1, gp_Vec2d(d1Term * aXX + aYX, d1Term * aXY + aYY));
    }
  }
  else if (theN == 2)
  {
    // D2 = (1/2F) * X (constant)
    const gp_Vec2d aD2(aCoeff2 * aXX, aCoeff2 * aXY);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD2);
    }
  }
  else
  {
    // DN = 0 for N >= 3
    const gp_Vec2d aZero(0.0, 0.0);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aZero);
    }
  }
  return aResult;
}

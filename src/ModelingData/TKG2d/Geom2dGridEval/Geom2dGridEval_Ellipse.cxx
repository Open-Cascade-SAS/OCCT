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

#include <Geom2dGridEval_Ellipse.hxx>

#include <gp_Elips2d.hxx>

#include <cmath>

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_Ellipse::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Pnt2d> aResult(1, aNb);

  const gp_Elips2d& anElips = myGeom->Elips2d();
  const gp_Pnt2d&   aCenter = anElips.Location();
  const gp_Dir2d    aXDir   = anElips.XAxis().Direction();
  const gp_Dir2d    aYDir   = anElips.YAxis().Direction();
  const double      aMajR   = anElips.MajorRadius();
  const double      aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u    = theParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Pnt2d(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
                              aCY + aMajR * cosU * aXY + aMinR * sinU * aYY));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_Ellipse::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, aNb);

  const gp_Elips2d& anElips = myGeom->Elips2d();
  const gp_Pnt2d&   aCenter = anElips.Location();
  const gp_Dir2d    aXDir   = anElips.XAxis().Direction();
  const gp_Dir2d    aYDir   = anElips.YAxis().Direction();
  const double      aMajR   = anElips.MajorRadius();
  const double      aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u    = theParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    // D1 = -MajorR * sin(u) * XDir + MinorR * cos(u) * YDir

    aResult.ChangeValue(i - theParams.Lower() + 1) = {
      gp_Pnt2d(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
               aCY + aMajR * cosU * aXY + aMinR * sinU * aYY),
      gp_Vec2d(-aMajR * sinU * aXX + aMinR * cosU * aYX, -aMajR * sinU * aXY + aMinR * cosU * aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_Ellipse::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, aNb);

  const gp_Elips2d& anElips = myGeom->Elips2d();
  const gp_Pnt2d&   aCenter = anElips.Location();
  const gp_Dir2d    aXDir   = anElips.XAxis().Direction();
  const gp_Dir2d    aYDir   = anElips.YAxis().Direction();
  const double      aMajR   = anElips.MajorRadius();
  const double      aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u    = theParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    // D1 = -MajorR * sin(u) * XDir + MinorR * cos(u) * YDir
    // D2 = -MajorR * cos(u) * XDir - MinorR * sin(u) * YDir = -(P - Center)

    aResult.ChangeValue(i - theParams.Lower() + 1) = {
      gp_Pnt2d(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
               aCY + aMajR * cosU * aXY + aMinR * sinU * aYY),
      gp_Vec2d(-aMajR * sinU * aXX + aMinR * cosU * aYX, -aMajR * sinU * aXY + aMinR * cosU * aYY),
      gp_Vec2d(-aMajR * cosU * aXX - aMinR * sinU * aYX, -aMajR * cosU * aXY - aMinR * sinU * aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_Ellipse::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, aNb);

  const gp_Elips2d& anElips = myGeom->Elips2d();
  const gp_Pnt2d&   aCenter = anElips.Location();
  const gp_Dir2d    aXDir   = anElips.XAxis().Direction();
  const gp_Dir2d    aYDir   = anElips.YAxis().Direction();
  const double      aMajR   = anElips.MajorRadius();
  const double      aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u    = theParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    // D1 = -MajorR * sin(u) * XDir + MinorR * cos(u) * YDir
    // D2 = -MajorR * cos(u) * XDir - MinorR * sin(u) * YDir
    // D3 =  MajorR * sin(u) * XDir - MinorR * cos(u) * YDir = -D1

    aResult.ChangeValue(i - theParams.Lower() + 1) = {
      gp_Pnt2d(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
               aCY + aMajR * cosU * aXY + aMinR * sinU * aYY),
      gp_Vec2d(-aMajR * sinU * aXX + aMinR * cosU * aYX, -aMajR * sinU * aXY + aMinR * cosU * aYY),
      gp_Vec2d(-aMajR * cosU * aXX - aMinR * sinU * aYX, -aMajR * cosU * aXY - aMinR * sinU * aYY),
      gp_Vec2d(aMajR * sinU * aXX - aMinR * cosU * aYX, aMajR * sinU * aXY - aMinR * cosU * aYY)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_Ellipse::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Vec2d> aResult(1, aNb);

  const gp_Elips2d& anElips = myGeom->Elips2d();
  const gp_Dir2d    aXDir   = anElips.XAxis().Direction();
  const gp_Dir2d    aYDir   = anElips.YAxis().Direction();
  const double      aMajR   = anElips.MajorRadius();
  const double      aMinR   = anElips.MinorRadius();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  // Ellipse derivatives are cyclic with period 4:
  // D1 = -MajR * sin(u) * X + MinR * cos(u) * Y  -> coefficients: (-sin, cos)
  // D2 = -MajR * cos(u) * X - MinR * sin(u) * Y  -> coefficients: (-cos, -sin)
  // D3 =  MajR * sin(u) * X - MinR * cos(u) * Y  -> coefficients: (sin, -cos)
  // D4 =  MajR * cos(u) * X + MinR * sin(u) * Y  -> coefficients: (cos, sin) = D0
  const int aPhase = (theN - 1) % 4;

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u    = theParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    double aCoeffMajR, aCoeffMinR;
    switch (aPhase)
    {
      case 0: // D1, D5, D9, ...
        aCoeffMajR = -sinU;
        aCoeffMinR = cosU;
        break;
      case 1: // D2, D6, D10, ...
        aCoeffMajR = -cosU;
        aCoeffMinR = -sinU;
        break;
      case 2: // D3, D7, D11, ...
        aCoeffMajR = sinU;
        aCoeffMinR = -cosU;
        break;
      default: // D4, D8, D12, ... (case 3)
        aCoeffMajR = cosU;
        aCoeffMinR = sinU;
        break;
    }

    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Vec2d(aMajR * aCoeffMajR * aXX + aMinR * aCoeffMinR * aYX,
                              aMajR * aCoeffMajR * aXY + aMinR * aCoeffMinR * aYY));
  }
  return aResult;
}

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

#include <Geom2dGridEval_Circle.hxx>

#include <gp_Circ2d.hxx>

#include <cmath>

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_Circle::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Pnt2d> aResult(1, aNb);

  // Extract circle data from geometry
  const gp_Circ2d& aCirc   = myGeom->Circ2d();
  const gp_Pnt2d&  aCenter = aCirc.Location();
  const gp_Dir2d&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir2d&  aYDir   = aCirc.Position().YDirection();
  const double     aRadius = aCirc.Radius();

  // Pre-extract coordinates for performance
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

    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Pnt2d(aCX + aRadius * (cosU * aXX + sinU * aYX),
                              aCY + aRadius * (cosU * aXY + sinU * aYY)));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_Circle::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  const int                                     aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, aNb);

  const gp_Circ2d& aCirc   = myGeom->Circ2d();
  const gp_Pnt2d&  aCenter = aCirc.Location();
  const gp_Dir2d&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir2d&  aYDir   = aCirc.Position().YDirection();
  const double     aRadius = aCirc.Radius();

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

    // P = C + R * (cos(u) * X + sin(u) * Y)
    // D1 = R * (-sin(u) * X + cos(u) * Y)
    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt2d(aCX + aRadius * (cosU * aXX + sinU * aYX),
                                         aCY + aRadius * (cosU * aXY + sinU * aYY)),
                                gp_Vec2d(aRadius * (-sinU * aXX + cosU * aYX),
                                         aRadius * (-sinU * aXY + cosU * aYY))};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_Circle::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  const int                                     aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, aNb);

  const gp_Circ2d& aCirc   = myGeom->Circ2d();
  const gp_Pnt2d&  aCenter = aCirc.Location();
  const gp_Dir2d&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir2d&  aYDir   = aCirc.Position().YDirection();
  const double     aRadius = aCirc.Radius();

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

    // P = C + R * (cos(u) * X + sin(u) * Y)
    // D1 = R * (-sin(u) * X + cos(u) * Y)
    // D2 = R * (-cos(u) * X - sin(u) * Y)
    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt2d(aCX + aRadius * (cosU * aXX + sinU * aYX),
                                         aCY + aRadius * (cosU * aXY + sinU * aYY)),
                                gp_Vec2d(aRadius * (-sinU * aXX + cosU * aYX),
                                         aRadius * (-sinU * aXY + cosU * aYY)),
                                gp_Vec2d(aRadius * (-cosU * aXX - sinU * aYX),
                                         aRadius * (-cosU * aXY - sinU * aYY))};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_Circle::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  const int                                     aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, aNb);

  const gp_Circ2d& aCirc   = myGeom->Circ2d();
  const gp_Pnt2d&  aCenter = aCirc.Location();
  const gp_Dir2d&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir2d&  aYDir   = aCirc.Position().YDirection();
  const double     aRadius = aCirc.Radius();

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

    // P = C + R * (cos(u) * X + sin(u) * Y)
    // D1 = R * (-sin(u) * X + cos(u) * Y)
    // D2 = R * (-cos(u) * X - sin(u) * Y)
    // D3 = R * (sin(u) * X - cos(u) * Y)
    aResult.ChangeValue(i - theParams.Lower()
                        + 1) = {gp_Pnt2d(aCX + aRadius * (cosU * aXX + sinU * aYX),
                                         aCY + aRadius * (cosU * aXY + sinU * aYY)),
                                gp_Vec2d(aRadius * (-sinU * aXX + cosU * aYX),
                                         aRadius * (-sinU * aXY + cosU * aYY)),
                                gp_Vec2d(aRadius * (-cosU * aXX - sinU * aYX),
                                         aRadius * (-cosU * aXY - sinU * aYY)),
                                gp_Vec2d(aRadius * (sinU * aXX - cosU * aYX),
                                         aRadius * (sinU * aXY - cosU * aYY))};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_Circle::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Vec2d> aResult(1, aNb);

  const gp_Circ2d& aCirc   = myGeom->Circ2d();
  const gp_Dir2d&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir2d&  aYDir   = aCirc.Position().YDirection();
  const double     aRadius = aCirc.Radius();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();

  // Circle derivatives are cyclic with period 4:
  // D1 = R * (-sin(u) * X + cos(u) * Y)  -> coefficients: (-sin, cos)
  // D2 = R * (-cos(u) * X - sin(u) * Y)  -> coefficients: (-cos, -sin)
  // D3 = R * (sin(u) * X - cos(u) * Y)   -> coefficients: (sin, -cos)
  // D4 = R * (cos(u) * X + sin(u) * Y)   -> coefficients: (cos, sin) = D0
  const int aPhase = (theN - 1) % 4; // 0->D1, 1->D2, 2->D3, 3->D4

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double u    = theParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    double aCoeffX, aCoeffY;
    switch (aPhase)
    {
      case 0: // D1, D5, D9, ...
        aCoeffX = -sinU;
        aCoeffY = cosU;
        break;
      case 1: // D2, D6, D10, ...
        aCoeffX = -cosU;
        aCoeffY = -sinU;
        break;
      case 2: // D3, D7, D11, ...
        aCoeffX = sinU;
        aCoeffY = -cosU;
        break;
      default: // D4, D8, D12, ... (case 3)
        aCoeffX = cosU;
        aCoeffY = sinU;
        break;
    }

    aResult.SetValue(i - theParams.Lower() + 1,
                     gp_Vec2d(aRadius * (aCoeffX * aXX + aCoeffY * aYX),
                              aRadius * (aCoeffX * aXY + aCoeffY * aYY)));
  }
  return aResult;
}

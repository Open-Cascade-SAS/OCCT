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

#include <GeomGridEval_Circle.hxx>

#include <gp_Circ.hxx>

#include <cmath>

//==================================================================================================

void GeomGridEval_Circle::SetParams(const TColStd_Array1OfReal& theParams)
{
  myParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_Circle::EvaluateGrid() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  NCollection_Array1<gp_Pnt> aResult(myParams.Lower(), myParams.Upper());

  // Extract circle data from geometry
  const gp_Circ& aCirc   = myGeom->Circ();
  const gp_Pnt&  aCenter = aCirc.Location();
  const gp_Dir&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir&  aYDir   = aCirc.Position().YDirection();
  const double   aRadius = aCirc.Radius();

  // Pre-extract coordinates for performance
  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    aResult.SetValue(i,
                     gp_Pnt(aCX + aRadius * (cosU * aXX + sinU * aYX),
                            aCY + aRadius * (cosU * aXY + sinU * aYY),
                            aCZ + aRadius * (cosU * aXZ + sinU * aYZ)));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_Circle::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  NCollection_Array1<GeomGridEval::CurveD1> aResult(myParams.Lower(), myParams.Upper());

  const gp_Circ& aCirc   = myGeom->Circ();
  const gp_Pnt&  aCenter = aCirc.Location();
  const gp_Dir&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir&  aYDir   = aCirc.Position().YDirection();
  const double   aRadius = aCirc.Radius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = C + R * (cos(u) * X + sin(u) * Y)
    // D1 = R * (-sin(u) * X + cos(u) * Y)
    aResult.ChangeValue(i) = {gp_Pnt(aCX + aRadius * (cosU * aXX + sinU * aYX),
                                     aCY + aRadius * (cosU * aXY + sinU * aYY),
                                     aCZ + aRadius * (cosU * aXZ + sinU * aYZ)),
                              gp_Vec(aRadius * (-sinU * aXX + cosU * aYX),
                                     aRadius * (-sinU * aXY + cosU * aYY),
                                     aRadius * (-sinU * aXZ + cosU * aYZ))};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_Circle::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  NCollection_Array1<GeomGridEval::CurveD2> aResult(myParams.Lower(), myParams.Upper());

  const gp_Circ& aCirc   = myGeom->Circ();
  const gp_Pnt&  aCenter = aCirc.Location();
  const gp_Dir&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir&  aYDir   = aCirc.Position().YDirection();
  const double   aRadius = aCirc.Radius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = C + R * (cos(u) * X + sin(u) * Y)
    // D1 = R * (-sin(u) * X + cos(u) * Y)
    // D2 = R * (-cos(u) * X - sin(u) * Y)
    aResult.ChangeValue(i) = {gp_Pnt(aCX + aRadius * (cosU * aXX + sinU * aYX),
                                     aCY + aRadius * (cosU * aXY + sinU * aYY),
                                     aCZ + aRadius * (cosU * aXZ + sinU * aYZ)),
                              gp_Vec(aRadius * (-sinU * aXX + cosU * aYX),
                                     aRadius * (-sinU * aXY + cosU * aYY),
                                     aRadius * (-sinU * aXZ + cosU * aYZ)),
                              gp_Vec(aRadius * (-cosU * aXX - sinU * aYX),
                                     aRadius * (-cosU * aXY - sinU * aYY),
                                     aRadius * (-cosU * aXZ - sinU * aYZ))};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_Circle::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  NCollection_Array1<GeomGridEval::CurveD3> aResult(myParams.Lower(), myParams.Upper());

  const gp_Circ& aCirc   = myGeom->Circ();
  const gp_Pnt&  aCenter = aCirc.Location();
  const gp_Dir&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir&  aYDir   = aCirc.Position().YDirection();
  const double   aRadius = aCirc.Radius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = C + R * (cos(u) * X + sin(u) * Y)
    // D1 = R * (-sin(u) * X + cos(u) * Y)
    // D2 = R * (-cos(u) * X - sin(u) * Y)
    // D3 = R * (sin(u) * X - cos(u) * Y)
    aResult.ChangeValue(i) = {gp_Pnt(aCX + aRadius * (cosU * aXX + sinU * aYX),
                                     aCY + aRadius * (cosU * aXY + sinU * aYY),
                                     aCZ + aRadius * (cosU * aXZ + sinU * aYZ)),
                              gp_Vec(aRadius * (-sinU * aXX + cosU * aYX),
                                     aRadius * (-sinU * aXY + cosU * aYY),
                                     aRadius * (-sinU * aXZ + cosU * aYZ)),
                              gp_Vec(aRadius * (-cosU * aXX - sinU * aYX),
                                     aRadius * (-cosU * aXY - sinU * aYY),
                                     aRadius * (-cosU * aXZ - sinU * aYZ)),
                              gp_Vec(aRadius * (sinU * aXX - cosU * aYX),
                                     aRadius * (sinU * aXY - cosU * aYY),
                                     aRadius * (sinU * aXZ - cosU * aYZ))};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_Circle::EvaluateGridDN(int theN) const
{
  if (myGeom.IsNull() || myParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  NCollection_Array1<gp_Vec> aResult(myParams.Lower(), myParams.Upper());

  const gp_Circ& aCirc   = myGeom->Circ();
  const gp_Dir&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir&  aYDir   = aCirc.Position().YDirection();
  const double   aRadius = aCirc.Radius();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  // Circle derivatives are cyclic with period 4:
  // D1 = R * (-sin(u) * X + cos(u) * Y)  -> coefficients: (-sin, cos)
  // D2 = R * (-cos(u) * X - sin(u) * Y)  -> coefficients: (-cos, -sin)
  // D3 = R * (sin(u) * X - cos(u) * Y)   -> coefficients: (sin, -cos)
  // D4 = R * (cos(u) * X + sin(u) * Y)   -> coefficients: (cos, sin) = D0
  const int aPhase = (theN - 1) % 4; // 0->D1, 1->D2, 2->D3, 3->D4

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
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

    aResult.SetValue(i,
                     gp_Vec(aRadius * (aCoeffX * aXX + aCoeffY * aYX),
                            aRadius * (aCoeffX * aXY + aCoeffY * aYY),
                            aRadius * (aCoeffX * aXZ + aCoeffY * aYZ)));
  }
  return aResult;
}

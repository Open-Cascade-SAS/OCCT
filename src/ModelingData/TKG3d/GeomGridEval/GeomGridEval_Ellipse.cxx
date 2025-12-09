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

#include <GeomGridEval_Ellipse.hxx>

#include <gp_Elips.hxx>

#include <cmath>

//==================================================================================================

void GeomGridEval_Ellipse::SetParams(const TColStd_Array1OfReal& theParams)
{
  const int aNb = theParams.Size();
  myParams.Resize(1, aNb, false);
  for (int i = 1; i <= aNb; ++i)
  {
    myParams.SetValue(i, theParams.Value(theParams.Lower() + i - 1));
  }
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_Ellipse::EvaluateGrid() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNb = myParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNb);

  const gp_Elips& anElips = myGeom->Elips();
  const gp_Pnt&   aCenter = anElips.Location();
  const gp_Dir&   aXDir   = anElips.XAxis().Direction();
  const gp_Dir&   aYDir   = anElips.YAxis().Direction();
  const double    aMajR   = anElips.MajorRadius();
  const double    aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = 1; i <= aNb; ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    aResult.SetValue(i,
                     gp_Pnt(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
                            aCY + aMajR * cosU * aXY + aMinR * sinU * aYY,
                            aCZ + aMajR * cosU * aXZ + aMinR * sinU * aYZ));
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_Ellipse::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                   aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNb);

  const gp_Elips& anElips = myGeom->Elips();
  const gp_Pnt&   aCenter = anElips.Location();
  const gp_Dir&   aXDir   = anElips.XAxis().Direction();
  const gp_Dir&   aYDir   = anElips.YAxis().Direction();
  const double    aMajR   = anElips.MajorRadius();
  const double    aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = 1; i <= aNb; ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    // D1 = -MajorR * sin(u) * XDir + MinorR * cos(u) * YDir

    aResult.ChangeValue(i) = {
      gp_Pnt(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
             aCY + aMajR * cosU * aXY + aMinR * sinU * aYY,
             aCZ + aMajR * cosU * aXZ + aMinR * sinU * aYZ),
      gp_Vec(-aMajR * sinU * aXX + aMinR * cosU * aYX,
             -aMajR * sinU * aXY + aMinR * cosU * aYY,
             -aMajR * sinU * aXZ + aMinR * cosU * aYZ)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_Ellipse::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                   aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNb);

  const gp_Elips& anElips = myGeom->Elips();
  const gp_Pnt&   aCenter = anElips.Location();
  const gp_Dir&   aXDir   = anElips.XAxis().Direction();
  const gp_Dir&   aYDir   = anElips.YAxis().Direction();
  const double    aMajR   = anElips.MajorRadius();
  const double    aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = 1; i <= aNb; ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    // D1 = -MajorR * sin(u) * XDir + MinorR * cos(u) * YDir
    // D2 = -MajorR * cos(u) * XDir - MinorR * sin(u) * YDir = -(P - Center)

    aResult.ChangeValue(i) = {
      gp_Pnt(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
             aCY + aMajR * cosU * aXY + aMinR * sinU * aYY,
             aCZ + aMajR * cosU * aXZ + aMinR * sinU * aYZ),
      gp_Vec(-aMajR * sinU * aXX + aMinR * cosU * aYX,
             -aMajR * sinU * aXY + aMinR * cosU * aYY,
             -aMajR * sinU * aXZ + aMinR * cosU * aYZ),
      gp_Vec(-aMajR * cosU * aXX - aMinR * sinU * aYX,
             -aMajR * cosU * aXY - aMinR * sinU * aYY,
             -aMajR * cosU * aXZ - aMinR * sinU * aYZ)};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_Ellipse::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                   aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNb);

  const gp_Elips& anElips = myGeom->Elips();
  const gp_Pnt&   aCenter = anElips.Location();
  const gp_Dir&   aXDir   = anElips.XAxis().Direction();
  const gp_Dir&   aYDir   = anElips.YAxis().Direction();
  const double    aMajR   = anElips.MajorRadius();
  const double    aMinR   = anElips.MinorRadius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = 1; i <= aNb; ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // P = Center + MajorR * cos(u) * XDir + MinorR * sin(u) * YDir
    // D1 = -MajorR * sin(u) * XDir + MinorR * cos(u) * YDir
    // D2 = -MajorR * cos(u) * XDir - MinorR * sin(u) * YDir
    // D3 =  MajorR * sin(u) * XDir - MinorR * cos(u) * YDir = -D1

    aResult.ChangeValue(i) = {
      gp_Pnt(aCX + aMajR * cosU * aXX + aMinR * sinU * aYX,
             aCY + aMajR * cosU * aXY + aMinR * sinU * aYY,
             aCZ + aMajR * cosU * aXZ + aMinR * sinU * aYZ),
      gp_Vec(-aMajR * sinU * aXX + aMinR * cosU * aYX,
             -aMajR * sinU * aXY + aMinR * cosU * aYY,
             -aMajR * sinU * aXZ + aMinR * cosU * aYZ),
      gp_Vec(-aMajR * cosU * aXX - aMinR * sinU * aYX,
             -aMajR * cosU * aXY - aMinR * sinU * aYY,
             -aMajR * cosU * aXZ - aMinR * sinU * aYZ),
      gp_Vec(aMajR * sinU * aXX - aMinR * cosU * aYX,
             aMajR * sinU * aXY - aMinR * cosU * aYY,
             aMajR * sinU * aXZ - aMinR * cosU * aYZ)};
  }
  return aResult;
}

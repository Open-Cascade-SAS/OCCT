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

#include <GeomGridEval_Cylinder.hxx>

#include <gp_Cylinder.hxx>

#include <cmath>

//==================================================================================================

void GeomGridEval_Cylinder::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                        const TColStd_Array1OfReal& theVParams)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  myUParams.Resize(1, aNbU, false);
  for (int i = 1; i <= aNbU; ++i)
  {
    myUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i - 1));
  }

  myVParams.Resize(1, aNbV, false);
  for (int j = 1; j <= aNbV; ++j)
  {
    myVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j - 1));
  }
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_Cylinder::EvaluateGrid() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Extract cylinder data
  const gp_Cylinder& aCyl    = myGeom->Cylinder();
  const gp_Pnt&      aCenter = aCyl.Location();
  const gp_Dir&      aXDir   = aCyl.Position().XDirection();
  const gp_Dir&      aYDir   = aCyl.Position().YDirection();
  const gp_Dir&      aZDir   = aCyl.Position().Direction();
  const double       aRadius = aCyl.Radius();

  // Pre-extract coordinates
  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // Pre-calculate U-dependent components
    // Base circle point (relative to center)
    const double aCircleX = aRadius * (cosU * aXX + sinU * aYX);
    const double aCircleY = aRadius * (cosU * aXY + sinU * aYY);
    const double aCircleZ = aRadius * (cosU * aXZ + sinU * aYZ);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = myVParams.Value(iV);

      // P = Center + (Circle Point) + v * ZDir
      aResult.SetValue(iU, iV,
                       gp_Pnt(aCX + aCircleX + v * aZX,
                              aCY + aCircleY + v * aZY,
                              aCZ + aCircleZ + v * aZZ));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Cylinder::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  const gp_Cylinder& aCyl    = myGeom->Cylinder();
  const gp_Pnt&      aCenter = aCyl.Location();
  const gp_Dir&      aXDir   = aCyl.Position().XDirection();
  const gp_Dir&      aYDir   = aCyl.Position().YDirection();
  const gp_Dir&      aZDir   = aCyl.Position().Direction();
  const double       aRadius = aCyl.Radius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // Pre-calculate U-dependent components
    // P relative to center (without V component)
    const double aCircleX = aRadius * (cosU * aXX + sinU * aYX);
    const double aCircleY = aRadius * (cosU * aXY + sinU * aYY);
    const double aCircleZ = aRadius * (cosU * aXZ + sinU * aYZ);

    // D1U = R * (-sinU * XDir + cosU * YDir)
    const double dU1 = aRadius * (-sinU * aXX + cosU * aYX);
    const double dU2 = aRadius * (-sinU * aXY + cosU * aYY);
    const double dU3 = aRadius * (-sinU * aXZ + cosU * aYZ);

    // D1V = ZDir (constant)
    const double dV1 = aZX;
    const double dV2 = aZY;
    const double dV3 = aZZ;

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = myVParams.Value(iV);

      aResult.ChangeValue(iU, iV) = {
        gp_Pnt(aCX + aCircleX + v * aZX,
               aCY + aCircleY + v * aZY,
               aCZ + aCircleZ + v * aZZ),
        gp_Vec(dU1, dU2, dU3),
        gp_Vec(dV1, dV2, dV3)
      };
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Cylinder::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  const gp_Cylinder& aCyl    = myGeom->Cylinder();
  const gp_Pnt&      aCenter = aCyl.Location();
  const gp_Dir&      aXDir   = aCyl.Position().XDirection();
  const gp_Dir&      aYDir   = aCyl.Position().YDirection();
  const gp_Dir&      aZDir   = aCyl.Position().Direction();
  const double       aRadius = aCyl.Radius();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // Pre-calculate U-dependent components
    const double aCircleX = aRadius * (cosU * aXX + sinU * aYX);
    const double aCircleY = aRadius * (cosU * aXY + sinU * aYY);
    const double aCircleZ = aRadius * (cosU * aXZ + sinU * aYZ);

    // D1U = R * (-sinU * XDir + cosU * YDir)
    const double dU1 = aRadius * (-sinU * aXX + cosU * aYX);
    const double dU2 = aRadius * (-sinU * aXY + cosU * aYY);
    const double dU3 = aRadius * (-sinU * aXZ + cosU * aYZ);

    // D1V = ZDir
    const double dV1 = aZX;
    const double dV2 = aZY;
    const double dV3 = aZZ;

    // D2U = R * (-cosU * XDir - sinU * YDir) = - (Circle point relative to center)
    const double d2U1 = -aCircleX;
    const double d2U2 = -aCircleY;
    const double d2U3 = -aCircleZ;

    // D2V = 0, D2UV = 0

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = myVParams.Value(iV);

      aResult.ChangeValue(iU, iV) = {
        gp_Pnt(aCX + aCircleX + v * aZX,
               aCY + aCircleY + v * aZY,
               aCZ + aCircleZ + v * aZZ),
        gp_Vec(dU1, dU2, dU3),
        gp_Vec(dV1, dV2, dV3),
        gp_Vec(d2U1, d2U2, d2U3),
        gp_Vec(0.0, 0.0, 0.0), // D2V
        gp_Vec(0.0, 0.0, 0.0)  // D2UV
      };
    }
  }
  return aResult;
}

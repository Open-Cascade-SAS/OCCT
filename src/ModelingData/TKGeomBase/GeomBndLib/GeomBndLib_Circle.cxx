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

#include <GeomBndLib_Circle.hxx>

#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <Precision.hxx>

//=================================================================================================

Bnd_Box GeomBndLib_Circle::Box(const gp_Circ& theCirc, double theTol)
{
  Bnd_Box      aBox;
  const double aR  = theCirc.Radius();
  const gp_XYZ aO  = theCirc.Location().XYZ();
  const gp_XYZ aXd = theCirc.XAxis().Direction().XYZ();
  const gp_XYZ aYd = theCirc.YAxis().Direction().XYZ();

  // For a full circle, compute per-coordinate analytical extrema.
  double aMin[3], aMax[3];
  for (int k = 1; k <= 3; ++k)
  {
    double aXk = aXd.Coord(k);
    double aYk = aYd.Coord(k);
    double aAmp = std::sqrt(aR * aR * aXk * aXk + aR * aR * aYk * aYk);
    aMin[k - 1] = aO.Coord(k) - aAmp;
    aMax[k - 1] = aO.Coord(k) + aAmp;
  }
  aBox.Update(aMin[0], aMin[1], aMin[2], aMax[0], aMax[1], aMax[2]);
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_Circle::Box(const gp_Circ& theCirc,
                                double         theU1,
                                double         theU2,
                                double         theTol)
{
  Bnd_Box      aBox;
  const double aR  = theCirc.Radius();
  const gp_XYZ aO  = theCirc.Location().XYZ();
  const gp_XYZ aXd = theCirc.XAxis().Direction().XYZ();
  const gp_XYZ aYd = theCirc.YAxis().Direction().XYZ();

  constexpr double aPeriod = 2. * M_PI - Precision::PConfusion();

  if (theU2 - theU1 >= aPeriod)
  {
    return Box(theCirc, theTol);
  }

  double aU1 = theU1, aU2 = theU2;
  double aTol = Epsilon(1.);
  ElCLib::AdjustPeriodic(0., 2. * M_PI, aTol, aU1, aU2);

  // Add arc endpoints.
  const gp_Ax2& aPos = theCirc.Position();
  aBox.Add(ElCLib::CircleValue(aU1, aPos, aR));
  aBox.Add(ElCLib::CircleValue(aU2, aPos, aR));

  // For each coordinate, check if the extremal parameter lies within the arc.
  for (int k = 1; k <= 3; ++k)
  {
    double aXk = aXd.Coord(k);
    double aYk = aYd.Coord(k);

    // Extremal parameter for min: tan(t) = (R * Yk) / (R * Xk)
    double aTExtrMin;
    if (std::abs(aXk) > gp::Resolution())
    {
      aTExtrMin = std::atan(aYk / aXk);
      aTExtrMin = ElCLib::InPeriod(aTExtrMin, 0., 2. * M_PI);
    }
    else
    {
      aTExtrMin = M_PI / 2.;
    }
    double aTExtrMax = aTExtrMin <= M_PI ? aTExtrMin + M_PI : aTExtrMin - M_PI;

    double aValMin = aR * std::cos(aTExtrMin) * aXk + aR * std::sin(aTExtrMin) * aYk + aO.Coord(k);
    double aValMax = aR * std::cos(aTExtrMax) * aXk + aR * std::sin(aTExtrMax) * aYk + aO.Coord(k);
    if (aValMin > aValMax)
    {
      std::swap(aValMin, aValMax);
      std::swap(aTExtrMin, aTExtrMax);
    }

    double aTk = ElCLib::InPeriod(aTExtrMin, aU1, aU1 + 2. * M_PI);
    if (aTk >= aU1 && aTk <= aU2)
    {
      aBox.Add(ElCLib::CircleValue(aTExtrMin, aPos, aR));
    }
    aTk = ElCLib::InPeriod(aTExtrMax, aU1, aU1 + 2. * M_PI);
    if (aTk >= aU1 && aTk <= aU2)
    {
      aBox.Add(ElCLib::CircleValue(aTExtrMax, aPos, aR));
    }
  }

  aBox.Enlarge(theTol);
  return aBox;
}

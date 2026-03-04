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

#include <GeomBndLib_Ellipse.hxx>

#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Elips.hxx>
#include <Precision.hxx>

//=================================================================================================

void GeomBndLib_Ellipse::Add(const gp_Elips& theElips, double theTol, Bnd_Box& theBox)
{
  const double aMajR = theElips.MajorRadius();
  const double aMinR = theElips.MinorRadius();
  const gp_XYZ aO    = theElips.Location().XYZ();
  const gp_XYZ aXd   = theElips.XAxis().Direction().XYZ();
  const gp_XYZ aYd   = theElips.YAxis().Direction().XYZ();

  // For a full ellipse, compute per-coordinate analytical extrema.
  double aMin[3], aMax[3];
  for (int k = 1; k <= 3; ++k)
  {
    double aXk = aXd.Coord(k);
    double aYk = aYd.Coord(k);
    double aAmp = std::sqrt(aMajR * aMajR * aXk * aXk + aMinR * aMinR * aYk * aYk);
    aMin[k - 1] = aO.Coord(k) - aAmp;
    aMax[k - 1] = aO.Coord(k) + aAmp;
  }
  theBox.Update(aMin[0], aMin[1], aMin[2], aMax[0], aMax[1], aMax[2]);
  theBox.Enlarge(theTol);
}

//=================================================================================================

void GeomBndLib_Ellipse::Add(const gp_Elips& theElips,
                             double          theU1,
                             double          theU2,
                             double          theTol,
                             Bnd_Box&        theBox)
{
  const double aMajR = theElips.MajorRadius();
  const double aMinR = theElips.MinorRadius();
  const gp_XYZ aO    = theElips.Location().XYZ();
  const gp_XYZ aXd   = theElips.XAxis().Direction().XYZ();
  const gp_XYZ aYd   = theElips.YAxis().Direction().XYZ();

  constexpr double aPeriod = 2. * M_PI - Precision::PConfusion();

  if (theU2 - theU1 >= aPeriod)
  {
    Add(theElips, theTol, theBox);
    return;
  }

  double aU1 = theU1, aU2 = theU2;
  double aTol = Epsilon(1.);
  ElCLib::AdjustPeriodic(0., 2. * M_PI, aTol, aU1, aU2);

  // Add arc endpoints.
  const gp_Ax2& aPos = theElips.Position();
  theBox.Add(ElCLib::EllipseValue(aU1, aPos, aMajR, aMinR));
  theBox.Add(ElCLib::EllipseValue(aU2, aPos, aMajR, aMinR));

  // For each coordinate, check if the extremal parameter lies within the arc.
  for (int k = 1; k <= 3; ++k)
  {
    double aXk = aXd.Coord(k);
    double aYk = aYd.Coord(k);

    double aTExtrMin;
    if (std::abs(aXk) > gp::Resolution())
    {
      aTExtrMin = std::atan((aMinR * aYk) / (aMajR * aXk));
      aTExtrMin = ElCLib::InPeriod(aTExtrMin, 0., 2. * M_PI);
    }
    else
    {
      aTExtrMin = M_PI / 2.;
    }
    double aTExtrMax = aTExtrMin <= M_PI ? aTExtrMin + M_PI : aTExtrMin - M_PI;

    double aValMin =
      aMajR * std::cos(aTExtrMin) * aXk + aMinR * std::sin(aTExtrMin) * aYk + aO.Coord(k);
    double aValMax =
      aMajR * std::cos(aTExtrMax) * aXk + aMinR * std::sin(aTExtrMax) * aYk + aO.Coord(k);
    if (aValMin > aValMax)
    {
      std::swap(aTExtrMin, aTExtrMax);
    }

    double aTk = ElCLib::InPeriod(aTExtrMin, aU1, aU1 + 2. * M_PI);
    if (aTk >= aU1 && aTk <= aU2)
    {
      theBox.Add(ElCLib::EllipseValue(aTExtrMin, aPos, aMajR, aMinR));
    }
    aTk = ElCLib::InPeriod(aTExtrMax, aU1, aU1 + 2. * M_PI);
    if (aTk >= aU1 && aTk <= aU2)
    {
      theBox.Add(ElCLib::EllipseValue(aTExtrMax, aPos, aMajR, aMinR));
    }
  }

  theBox.Enlarge(theTol);
}

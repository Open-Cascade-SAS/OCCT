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

#include <GeomBndLib_Ellipse2d.hxx>

#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Elips2d.hxx>
#include <Precision.hxx>

//=================================================================================================

Bnd_Box2d GeomBndLib_Ellipse2d::Box(const gp_Elips2d& theElips, double theTol)
{
  Bnd_Box2d    aBox;
  const double aMajR = theElips.MajorRadius();
  const double aMinR = theElips.MinorRadius();
  const gp_XY  aO    = theElips.Location().XY();
  const gp_XY  aXd   = theElips.XAxis().Direction().XY();
  const gp_XY  aYd   = theElips.YAxis().Direction().XY();

  double aMin[2], aMax[2];
  for (int k = 1; k <= 2; ++k)
  {
    double aXk  = aXd.Coord(k);
    double aYk  = aYd.Coord(k);
    double aAmp = std::sqrt(aMajR * aMajR * aXk * aXk + aMinR * aMinR * aYk * aYk);
    aMin[k - 1] = aO.Coord(k) - aAmp;
    aMax[k - 1] = aO.Coord(k) + aAmp;
  }
  aBox.Update(aMin[0], aMin[1], aMax[0], aMax[1]);
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box2d GeomBndLib_Ellipse2d::Box(const gp_Elips2d& theElips,
                                    double            theU1,
                                    double            theU2,
                                    double            theTol)
{
  Bnd_Box2d    aBox;
  const double aMajR = theElips.MajorRadius();
  const double aMinR = theElips.MinorRadius();
  const gp_XY  aO    = theElips.Location().XY();
  const gp_XY  aXd   = theElips.XAxis().Direction().XY();
  const gp_XY  aYd   = theElips.YAxis().Direction().XY();

  if (theU2 - theU1 >= 2. * M_PI - Precision::PConfusion())
  {
    return Box(theElips, theTol);
  }

  // Add arc endpoints.
  auto aEval = [&](double theT) -> gp_Pnt2d {
    return gp_Pnt2d(aO + aMajR * std::cos(theT) * aXd + aMinR * std::sin(theT) * aYd);
  };
  aBox.Add(aEval(theU1));
  aBox.Add(aEval(theU2));

  // For each coordinate, check if the extremal parameter lies within the arc.
  for (int k = 1; k <= 2; ++k)
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

    double aTk = ElCLib::InPeriod(aTExtrMin, theU1, theU1 + 2. * M_PI);
    if (aTk >= theU1 && aTk <= theU2)
    {
      aBox.Add(aEval(aTExtrMin));
    }
    aTk = ElCLib::InPeriod(aTExtrMax, theU1, theU1 + 2. * M_PI);
    if (aTk >= theU1 && aTk <= theU2)
    {
      aBox.Add(aEval(aTExtrMax));
    }
  }

  aBox.Enlarge(theTol);
  return aBox;
}

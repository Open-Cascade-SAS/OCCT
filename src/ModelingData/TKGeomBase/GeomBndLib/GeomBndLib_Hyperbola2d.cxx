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

#include <GeomBndLib_Hyperbola2d.hxx>

#include <ElCLib.hxx>
#include <gp_Hypr2d.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

namespace
{

//! Compute bounding box for finite 2D hyperbola arc, adding per-coordinate extrema.
void computeHyperbola2dBox(const gp_Hypr2d& theHypr,
                           const double     theT1,
                           const double     theT2,
                           Bnd_Box2d&       theBox)
{
  gp_Pnt2d aP1 = ElCLib::Value(theT1, theHypr);
  gp_Pnt2d aP2 = ElCLib::Value(theT2, theHypr);
  theBox.Add(aP1);
  theBox.Add(aP2);

  if (theT1 * theT2 < 0.)
  {
    theBox.Add(ElCLib::Value(0., theHypr));
  }

  const gp_Ax22d& aPos  = theHypr.Axis();
  const gp_XY&    aXDir = aPos.XDirection().XY();
  const gp_XY&    aYDir = aPos.YDirection().XY();
  double          aRmaj = theHypr.MajorRadius();
  double          aRmin = theHypr.MinorRadius();
  double          aEps  = Epsilon(1.);

  for (int i = 1; i <= 2; ++i)
  {
    double aA = aRmin * aYDir.Coord(i);
    double aB = aRmaj * aXDir.Coord(i);

    double aABP = std::abs(aA + aB);
    double aBAM = std::abs(aB - aA);

    if (aABP < aEps || aBAM < aEps)
    {
      continue;
    }

    double aCf = aBAM / aABP;
    double aT3 = 0.5 * std::log(aCf);

    if (aT3 < theT1 || aT3 > theT2)
    {
      continue;
    }
    theBox.Add(ElCLib::Value(aT3, theHypr));
  }
}

} // namespace

//=================================================================================================

void GeomBndLib_Hyperbola2d::Add(const gp_Hypr2d& theHypr,
                                 double           theU1,
                                 double           theU2,
                                 double           theTol,
                                 Bnd_Box2d&       theBox)
{
  if (Precision::IsNegativeInfinite(theU1))
  {
    if (Precision::IsNegativeInfinite(theU2))
    {
      throw Standard_Failure("GeomBndLib_Hyperbola2d::Add - bad parameter");
    }
    else if (Precision::IsPositiveInfinite(theU2))
    {
      theBox.OpenXmax();
      theBox.OpenYmax();
    }
    else
    {
      theBox.Add(ElCLib::Value(theU2, theHypr));
    }
    theBox.OpenXmin();
    theBox.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(theU1))
  {
    if (Precision::IsNegativeInfinite(theU2))
    {
      theBox.OpenXmin();
      theBox.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(theU2))
    {
      throw Standard_Failure("GeomBndLib_Hyperbola2d::Add - bad parameter");
    }
    else
    {
      theBox.Add(ElCLib::Value(theU2, theHypr));
    }
    theBox.OpenXmax();
    theBox.OpenYmax();
  }
  else
  {
    theBox.Add(ElCLib::Value(theU1, theHypr));
    if (Precision::IsNegativeInfinite(theU2))
    {
      theBox.OpenXmin();
      theBox.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(theU2))
    {
      theBox.OpenXmax();
      theBox.OpenYmax();
    }
    else
    {
      computeHyperbola2dBox(theHypr, theU1, theU2, theBox);
    }
  }
  theBox.Enlarge(theTol);
}

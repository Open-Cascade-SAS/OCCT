// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// JCV 16/10/91

#include <Convert_HyperbolaToBSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

constexpr int TheDegree  = 2;
constexpr int MaxNbKnots = 2;
constexpr int MaxNbPoles = 3;

//==================================================================================================

Convert_HyperbolaToBSplineCurve::Convert_HyperbolaToBSplineCurve(const gp_Hypr2d& H,
                                                                 const double     U1,
                                                                 const double     U2)

    : Convert_ConicToBSplineCurve(MaxNbPoles, MaxNbKnots, TheDegree)
{
  Standard_DomainError_Raise_if(std::abs(U2 - U1) < Epsilon(0.), "Convert_HyperbolaToBSplineCurve");

  double UF = std::min(U1, U2);
  double UL = std::max(U1, U2);

  myIsPeriodic = false;
  myKnots(1)   = UF;
  myMults(1)   = 3;
  myKnots(2)   = UL;
  myMults(2)   = 3;

  // construction of hyperbola in the reference xOy.

  double   R  = H.MajorRadius();
  double   r  = H.MinorRadius();
  gp_Dir2d Ox = H.Axis().XDirection();
  gp_Dir2d Oy = H.Axis().YDirection();
  double   S  = (Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.) ? 1 : -1;

  // poles expressed in the reference mark
  // the 2nd pole is at the intersection of 2 tangents to the curve
  // at points P(UF), P(UL)
  // the weight of this pole is equal to : std::cosh((UL-UF)/2)

  myWeights(1) = 1.;
  myWeights(2) = std::cosh((UL - UF) / 2);
  myWeights(3) = 1.;

  double delta = std::sinh(UL - UF);
  double x     = R * (std::sinh(UL) - std::sinh(UF)) / delta;
  double y     = S * r * (std::cosh(UL) - std::cosh(UF)) / delta;
  myPoles(1)   = gp_Pnt2d(R * std::cosh(UF), S * r * std::sinh(UF));
  myPoles(2)   = gp_Pnt2d(x, y);
  myPoles(3)   = gp_Pnt2d(R * std::cosh(UL), S * r * std::sinh(UL));

  // replace the bspline in the mark of the hyperbola
  gp_Trsf2d Trsf;
  Trsf.SetTransformation(H.Axis().XAxis(), gp::OX2d());
  myPoles(1).Transform(Trsf);
  myPoles(2).Transform(Trsf);
  myPoles(3).Transform(Trsf);
}

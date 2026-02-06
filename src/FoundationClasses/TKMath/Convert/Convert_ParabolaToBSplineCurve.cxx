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

#include <Convert_ParabolaToBSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

static int TheDegree  = 2;
static int MaxNbKnots = 2;
static int MaxNbPoles = 3;

//==================================================================================================

Convert_ParabolaToBSplineCurve::Convert_ParabolaToBSplineCurve(const gp_Parab2d& Prb,
                                                               const double      U1,
                                                               const double      U2)
    : Convert_ConicToBSplineCurve(MaxNbPoles, MaxNbKnots, TheDegree)
{
  Standard_DomainError_Raise_if(std::abs(U2 - U1) < Epsilon(0.), "Convert_ParabolaToBSplineCurve");

  double UF = std::min(U1, U2);
  double UL = std::max(U1, U2);

  double p = Prb.Parameter();

  myData.IsPeriodic               = false;
  myData.Knots(1) = UF;
  myData.Mults(1) = 3;
  myData.Knots(2) = UL;
  myData.Mults(2) = 3;

  myData.Weights(1) = 1.;
  myData.Weights(2) = 1.;
  myData.Weights(3) = 1.;

  gp_Dir2d Ox = Prb.Axis().XDirection();
  gp_Dir2d Oy = Prb.Axis().YDirection();
  double   S  = (Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.) ? 1 : -1;

  // poles expressed in the reference mark
  myData.Poles(1) = gp_Pnt2d((UF * UF) / (2. * p), S * UF);
  myData.Poles(2) = gp_Pnt2d((UF * UL) / (2. * p), S * (UF + UL) / 2.);
  myData.Poles(3) = gp_Pnt2d((UL * UL) / (2. * p), S * UL);

  // replace the bspline in the mark of the parabola
  gp_Trsf2d Trsf;
  Trsf.SetTransformation(Prb.Axis().XAxis(), gp::OX2d());
  myData.Poles(1).Transform(Trsf);
  myData.Poles(2).Transform(Trsf);
  myData.Poles(3).Transform(Trsf);
}

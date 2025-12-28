// Created on: 1995-07-19
// Created by: Modelistation
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

#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <GCPnts_TangentialDeflection.hxx>

//=================================================================================================

bool Extrema_CurveTool::IsPeriodic(const Adaptor3d_Curve& C)
{
  GeomAbs_CurveType aType = GetType(C);
  if (aType == GeomAbs_Circle || aType == GeomAbs_Ellipse)
    return true;
  else
    return C.IsPeriodic();
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> Extrema_CurveTool::DeflCurvIntervals(const Adaptor3d_Curve& C)
{
  const double           epsd    = 1.e-3;
  const double           maxdefl = 1.e3;
  const double           mindefl = 1.e-3;
  occ::handle<NCollection_HArray1<double>> Intervals;
  int              nbpnts = 23, i;
  double                 L      = 0.;
  double                 tf = C.FirstParameter(), tl = C.LastParameter();
  gp_Pnt                        aP = C.Value(tf);
  for (i = 2; i <= nbpnts; ++i)
  {
    double t   = (tf * (nbpnts - i) + (i - 1) * tl) / (nbpnts - 1);
    gp_Pnt        aP1 = C.Value(t);
    L += aP.Distance(aP1);
  }
  //
  double dLdt = L / (tl - tf);
  if (L <= Precision::Confusion() || dLdt < epsd
      || (tl - tf) > 10000.) // To avoid problemwith GCPnts
  {
    nbpnts    = 2;
    Intervals = new NCollection_HArray1<double>(1, nbpnts);
    Intervals->SetValue(1, tf);
    Intervals->SetValue(nbpnts, tl);
    return Intervals;
  }
  //
  double aDefl = std::max(0.01 * L / (2. * M_PI), mindefl);
  if (aDefl > maxdefl)
  {
    nbpnts    = 2;
    Intervals = new NCollection_HArray1<double>(1, nbpnts);
    Intervals->SetValue(1, tf);
    Intervals->SetValue(nbpnts, tl);
    return Intervals;
  }
  //
  double aMinLen = std::max(.00001 * L, Precision::Confusion());
  double aTol    = std::max(0.00001 * (tl - tf), Precision::PConfusion());
  //
  GCPnts_TangentialDeflection aPntGen(C, M_PI / 6, aDefl, 2, aTol, aMinLen);
  nbpnts    = aPntGen.NbPoints();
  Intervals = new NCollection_HArray1<double>(1, nbpnts);
  for (i = 1; i <= nbpnts; ++i)
  {
    Intervals->SetValue(i, aPntGen.Parameter(i));
  }
  return Intervals;
}

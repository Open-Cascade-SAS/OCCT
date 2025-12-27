// Created on: 1995-07-17
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

#include <BRepBlend_HCurve2dTool.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>

//============================================================
int BRepBlend_HCurve2dTool::NbSamples(const occ::handle<Adaptor2d_Curve2d>& C,
                                                   const double              U0,
                                                   const double              U1)
{
  GeomAbs_CurveType    typC     = C->GetType();
  static double nbsOther = 10.0;
  double        nbs      = nbsOther;

  if (typC == GeomAbs_Line)
    nbs = 2;
  else if (typC == GeomAbs_BezierCurve)
    nbs = 3 + C->Bezier()->NbPoles();
  else if (typC == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom2d_BSplineCurve> BSC = C->BSpline();
    nbs                             = BSC->NbKnots();
    nbs *= BSC->Degree();
    nbs *= BSC->LastParameter() - BSC->FirstParameter();
    nbs /= U1 - U0;
    if (nbs < 2.0)
      nbs = 2;
  }
  if (nbs > 50)
    nbs = 50;
  return ((int)nbs);
}

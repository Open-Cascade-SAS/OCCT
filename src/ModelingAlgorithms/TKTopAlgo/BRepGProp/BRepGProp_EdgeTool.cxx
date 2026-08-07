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

#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp_EdgeTool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

double BRepGProp_EdgeTool::FirstParameter(const BRepAdaptor_Curve& C)
{
  return C.FirstParameter();
}

double BRepGProp_EdgeTool::LastParameter(const BRepAdaptor_Curve& C)
{
  return C.LastParameter();
}

int BRepGProp_EdgeTool::IntegrationOrder(const BRepAdaptor_Curve& BAC)
{
  switch (BAC.GetType())
  {

    case GeomAbs_Line:
      return 2;

    case GeomAbs_Parabola:
      return 5;

    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve: {
      // Use the adaptor's own NbPoles(), which handles the curve-on-surface case;
      // Curve() is null there and down-casting it segfaults.
      int n = 2 * BAC.NbPoles() - 1;
      return n;
    }
    break;

    default:
      return 10;
  }
}

gp_Pnt BRepGProp_EdgeTool::Value(const BRepAdaptor_Curve& C, const double U)
{
  return C.Value(U);
}

void BRepGProp_EdgeTool::D1(const BRepAdaptor_Curve& C, const double U, gp_Pnt& P, gp_Vec& V1)
{
  C.D1(U, P, V1);
}

// modified by NIZHNY-MKK  Thu Jun  9 12:15:15 2005.BEGIN
int BRepGProp_EdgeTool::NbIntervals(const BRepAdaptor_Curve& C, const GeomAbs_Shape S)
{
  // clang-format off
  BRepAdaptor_Curve* pC = (BRepAdaptor_Curve*) &C; // at the moment actually NbIntervals() does not modify the 
                                                   // object "C". So it is safe to do such a cast.
  return pC->NbIntervals(S);
}

void BRepGProp_EdgeTool::Intervals(const BRepAdaptor_Curve& C,NCollection_Array1<double>& T,const GeomAbs_Shape S) 
{
  BRepAdaptor_Curve* pC = (BRepAdaptor_Curve*) &C; // at the moment actually Intervals() does not modify the
                                                   // object "C". So it is safe to do such a cast.
  // clang-format on
  pC->Intervals(T, S);
}

// modified by NIZHNY-MKK  Thu Jun  9 12:15:18 2005.END

// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _GCPnts_TCurveTypes_HeaderFile
#define _GCPnts_TCurveTypes_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GCPnts_DistFunction.hxx>
#include <GCPnts_DistFunction2d.hxx>

//! Auxiliary tool to resolve 2D/3D curve classes.
template <class TheCurve>
struct GCPnts_TCurveTypes
{
};

//! Auxiliary tool to resolve 3D curve classes.
template <>
struct GCPnts_TCurveTypes<Adaptor3d_Curve>
{
  using Point = gp_Pnt;
  using BezierCurve = Geom_BezierCurve;
  using BSplineCurve = Geom_BSplineCurve;
  using DistFunction = GCPnts_DistFunction;
  using DistFunctionMV = GCPnts_DistFunctionMV;
};

//! Auxiliary tool to resolve 2D curve classes.
template <>
struct GCPnts_TCurveTypes<Adaptor2d_Curve2d>
{
  using Point = gp_Pnt2d;
  using BezierCurve = Geom2d_BezierCurve;
  using BSplineCurve = Geom2d_BSplineCurve;
  using DistFunction = GCPnts_DistFunction2d;
  using DistFunctionMV = GCPnts_DistFunction2dMV;
};

#endif // _GCPnts_TCurveTypes_HeaderFile

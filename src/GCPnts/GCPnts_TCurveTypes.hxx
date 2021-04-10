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
template<class TheCurve> struct GCPnts_TCurveTypes {};

//! Auxiliary tool to resolve 3D curve classes.
template<> struct GCPnts_TCurveTypes<Adaptor3d_Curve>
{
  typedef gp_Pnt                Point;
  typedef Geom_BezierCurve      BezierCurve;
  typedef Geom_BSplineCurve     BSplineCurve;
  typedef GCPnts_DistFunction   DistFunction;
  typedef GCPnts_DistFunctionMV DistFunctionMV;
};

//! Auxiliary tool to resolve 2D curve classes.
template<> struct GCPnts_TCurveTypes<Adaptor2d_Curve2d>
{
  typedef gp_Pnt2d                Point;
  typedef Geom2d_BezierCurve      BezierCurve;
  typedef Geom2d_BSplineCurve     BSplineCurve;
  typedef GCPnts_DistFunction2d   DistFunction;
  typedef GCPnts_DistFunction2dMV DistFunctionMV;
};

#endif // _GCPnts_TCurveTypes_HeaderFile

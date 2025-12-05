// Created on: 1995-07-18
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

#ifndef _Extrema_CurveTool_HeaderFile
#define _Extrema_CurveTool_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

class Extrema_CurveTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstParameter(const Adaptor3d_Curve& theC) { return theC.FirstParameter(); }

  static double LastParameter(const Adaptor3d_Curve& theC) { return theC.LastParameter(); }

  static GeomAbs_Shape Continuity(const Adaptor3d_Curve& theC) { return theC.Continuity(); }

  //! Returns the number of intervals for continuity <S>.
  //! May be one if Continuity(me) >= <S>
  static int NbIntervals(Adaptor3d_Curve& theC, const GeomAbs_Shape theS)
  {
    return theC.NbIntervals(theS);
  }

  //! Stores in <T> the parameters bounding the intervals of continuity <S>.
  //! The array must provide enough room to accommodate for the parameters.
  //! i.e. T.Length() > NbIntervals()
  static void Intervals(Adaptor3d_Curve& theC, TColStd_Array1OfReal& theT, const GeomAbs_Shape theS)
  {
    theC.Intervals(theT, theS);
  }

  //! Returns the parameters bounding the intervals of subdivision of curve
  //! according to Curvature deflection. Value of deflection is defined in method.
  Standard_EXPORT static Handle(TColStd_HArray1OfReal) DeflCurvIntervals(
    const Adaptor3d_Curve& theC);

  Standard_EXPORT static bool IsPeriodic(const Adaptor3d_Curve& theC);

  static double Period(const Adaptor3d_Curve& theC) { return theC.Period(); }

  static double Resolution(const Adaptor3d_Curve& theC, const double theR3d)
  {
    return theC.Resolution(theR3d);
  }

  static GeomAbs_CurveType GetType(const Adaptor3d_Curve& theC) { return theC.GetType(); }

  static gp_Pnt Value(const Adaptor3d_Curve& theC, const double theU) { return theC.Value(theU); }

  static void D0(const Adaptor3d_Curve& theC, const double theU, gp_Pnt& theP)
  {
    theC.D0(theU, theP);
  }

  static void D1(const Adaptor3d_Curve& theC, const double theU, gp_Pnt& theP, gp_Vec& theV)
  {
    theC.D1(theU, theP, theV);
  }

  static void D2(const Adaptor3d_Curve& theC,
                 const double           theU,
                 gp_Pnt&                theP,
                 gp_Vec&                theV1,
                 gp_Vec&                theV2)
  {
    theC.D2(theU, theP, theV1, theV2);
  }

  static void D3(const Adaptor3d_Curve& theC,
                 const double           theU,
                 gp_Pnt&                theP,
                 gp_Vec&                theV1,
                 gp_Vec&                theV2,
                 gp_Vec&                theV3)
  {
    theC.D3(theU, theP, theV1, theV2, theV3);
  }

  static gp_Vec DN(const Adaptor3d_Curve& theC, const double theU, const int theN)
  {
    return theC.DN(theU, theN);
  }

  static gp_Lin Line(const Adaptor3d_Curve& theC) { return theC.Line(); }

  static gp_Circ Circle(const Adaptor3d_Curve& theC) { return theC.Circle(); }

  static gp_Elips Ellipse(const Adaptor3d_Curve& theC) { return theC.Ellipse(); }

  static gp_Hypr Hyperbola(const Adaptor3d_Curve& theC) { return theC.Hyperbola(); }

  static gp_Parab Parabola(const Adaptor3d_Curve& theC) { return theC.Parabola(); }

  static int Degree(const Adaptor3d_Curve& theC) { return theC.Degree(); }

  static bool IsRational(const Adaptor3d_Curve& theC) { return theC.IsRational(); }

  static int NbPoles(const Adaptor3d_Curve& theC) { return theC.NbPoles(); }

  static int NbKnots(const Adaptor3d_Curve& theC) { return theC.NbKnots(); }

  static Handle(Geom_BezierCurve) Bezier(const Adaptor3d_Curve& theC) { return theC.Bezier(); }

  static Handle(Geom_BSplineCurve) BSpline(const Adaptor3d_Curve& theC) { return theC.BSpline(); }
};

#endif // _Extrema_CurveTool_HeaderFile

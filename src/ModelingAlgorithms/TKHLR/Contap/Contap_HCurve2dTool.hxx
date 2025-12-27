// Created on: 1995-07-17
// Created by: Laurent BUCHARD
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

#ifndef _Contap_HCurve2dTool_HeaderFile
#define _Contap_HCurve2dTool_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <NCollection_Array1.hxx>

class gp_Pnt2d;
class gp_Vec2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;

class Contap_HCurve2dTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstParameter(const occ::handle<Adaptor2d_Curve2d>& C);

  static double LastParameter(const occ::handle<Adaptor2d_Curve2d>& C);

  static GeomAbs_Shape Continuity(const occ::handle<Adaptor2d_Curve2d>& C);

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(myclass) >= <S>
  static int NbIntervals(const occ::handle<Adaptor2d_Curve2d>& C, const GeomAbs_Shape S);

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  static void Intervals(const occ::handle<Adaptor2d_Curve2d>& C,
                        NCollection_Array1<double>&            T,
                        const GeomAbs_Shape              S);

  static bool IsClosed(const occ::handle<Adaptor2d_Curve2d>& C);

  static bool IsPeriodic(const occ::handle<Adaptor2d_Curve2d>& C);

  static double Period(const occ::handle<Adaptor2d_Curve2d>& C);

  //! Computes the point of parameter U on the curve.
  static gp_Pnt2d Value(const occ::handle<Adaptor2d_Curve2d>& C, const double U);

  //! Computes the point of parameter U on the curve.
  static void D0(const occ::handle<Adaptor2d_Curve2d>& C, const double U, gp_Pnt2d& P);

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  static void D1(const occ::handle<Adaptor2d_Curve2d>& C,
                 const double              U,
                 gp_Pnt2d&                        P,
                 gp_Vec2d&                        V);

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  static void D2(const occ::handle<Adaptor2d_Curve2d>& C,
                 const double              U,
                 gp_Pnt2d&                        P,
                 gp_Vec2d&                        V1,
                 gp_Vec2d&                        V2);

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  static void D3(const occ::handle<Adaptor2d_Curve2d>& C,
                 const double              U,
                 gp_Pnt2d&                        P,
                 gp_Vec2d&                        V1,
                 gp_Vec2d&                        V2,
                 gp_Vec2d&                        V3);

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  static gp_Vec2d DN(const occ::handle<Adaptor2d_Curve2d>& C,
                     const double              U,
                     const int           N);

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  static double Resolution(const occ::handle<Adaptor2d_Curve2d>& C, const double R3d);

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  static GeomAbs_CurveType GetType(const occ::handle<Adaptor2d_Curve2d>& C);

  static gp_Lin2d Line(const occ::handle<Adaptor2d_Curve2d>& C);

  static gp_Circ2d Circle(const occ::handle<Adaptor2d_Curve2d>& C);

  static gp_Elips2d Ellipse(const occ::handle<Adaptor2d_Curve2d>& C);

  static gp_Hypr2d Hyperbola(const occ::handle<Adaptor2d_Curve2d>& C);

  static gp_Parab2d Parabola(const occ::handle<Adaptor2d_Curve2d>& C);

  static occ::handle<Geom2d_BezierCurve> Bezier(const occ::handle<Adaptor2d_Curve2d>& C);

  static occ::handle<Geom2d_BSplineCurve> BSpline(const occ::handle<Adaptor2d_Curve2d>& C);

  Standard_EXPORT static int NbSamples(const occ::handle<Adaptor2d_Curve2d>& C,
                                                    const double              U0,
                                                    const double              U1);

};

#include <Contap_HCurve2dTool.lxx>

#endif // _Contap_HCurve2dTool_HeaderFile

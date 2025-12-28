// Created on: 1993-08-18
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HLRBRep_LineTool_HeaderFile
#define _HLRBRep_LineTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_HArray1.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! The LineTool class provides class methods to
//! access the methodes of the Line.
class HLRBRep_LineTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstParameter(const gp_Lin& C);

  static double LastParameter(const gp_Lin& C);

  static GeomAbs_Shape Continuity(const gp_Lin& C);

  //! If necessary, breaks the line in intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
  static int NbIntervals(const gp_Lin& C, const GeomAbs_Shape S);

  //! Sets the current working interval.
  static void Intervals(const gp_Lin& C, NCollection_Array1<double>& T, const GeomAbs_Shape Sh);

  //! Returns the first parameter of the current
  //! interval.
  static double IntervalFirst(const gp_Lin& C);

  //! Returns the last parameter of the current
  //! interval.
  static double IntervalLast(const gp_Lin& C);

  static GeomAbs_Shape IntervalContinuity(const gp_Lin& C);

  static bool IsClosed(const gp_Lin& C);

  static bool IsPeriodic(const gp_Lin& C);

  static double Period(const gp_Lin& C);

  //! Computes the point of parameter U on the line.
  static gp_Pnt Value(const gp_Lin& C, const double U);

  //! Computes the point of parameter U on the line.
  static void D0(const gp_Lin& C, const double U, gp_Pnt& P);

  //! Computes the point of parameter U on the line with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  static void D1(const gp_Lin& C, const double U, gp_Pnt& P, gp_Vec& V);

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  static void D2(const gp_Lin& C, const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  static void D3(const gp_Lin& C, const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  static gp_Vec DN(const gp_Lin& C, const double U, const int N);

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  static double Resolution(const gp_Lin& C, const double R3d);

  //! Returns the type of the line in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  static GeomAbs_CurveType GetType(const gp_Lin& C);

  static gp_Lin Line(const gp_Lin& C);

  static gp_Circ Circle(const gp_Lin& C);

  static gp_Elips Ellipse(const gp_Lin& C);

  static gp_Hypr Hyperbola(const gp_Lin& C);

  static gp_Parab Parabola(const gp_Lin& C);

  static occ::handle<Geom_BezierCurve> Bezier(const gp_Lin& C);

  static occ::handle<Geom_BSplineCurve> BSpline(const gp_Lin& C);

  static int Degree(const gp_Lin& C);

  static int NbPoles(const gp_Lin& C);

  static void Poles(const gp_Lin& C, NCollection_Array1<gp_Pnt>& TP);

  static bool IsRational(const gp_Lin& C);

  static void PolesAndWeights(const gp_Lin&               C,
                              NCollection_Array1<gp_Pnt>& TP,
                              NCollection_Array1<double>& TW);

  static int NbKnots(const gp_Lin& C);

  static void KnotsAndMultiplicities(const gp_Lin&               C,
                                     NCollection_Array1<double>& TK,
                                     NCollection_Array1<int>&    TM);

  static int NbSamples(const gp_Lin& C, const double U0, const double U1);

  static void SamplePars(const gp_Lin&                             C,
                         const double                              U0,
                         const double                              U1,
                         const double                              Defl,
                         const int                                 NbMin,
                         occ::handle<NCollection_HArray1<double>>& Pars);
};

#include <HLRBRep_LineTool.lxx>

#endif // _HLRBRep_LineTool_HeaderFile

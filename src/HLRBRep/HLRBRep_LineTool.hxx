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
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! The  LineTool  class  provides  class  methods to
//! access the methodes of the Line.
class HLRBRep_LineTool
{
public:
  DEFINE_STANDARD_ALLOC

  static Standard_Real FirstParameter(const gp_Lin& C);

  static Standard_Real LastParameter(const gp_Lin& C);

  static GeomAbs_Shape Continuity(const gp_Lin& C);

  //! If necessary,   breaks the line  in  intervals of
  //! continuity <S>.     And   returns  the  number  of
  //! intervals.
  static Standard_Integer NbIntervals(const gp_Lin& C, const GeomAbs_Shape S);

  //! Sets the current working interval.
  static void Intervals(const gp_Lin& C, TColStd_Array1OfReal& T, const GeomAbs_Shape Sh);

  //! Returns  the  first  parameter    of  the  current
  //! interval.
  static Standard_Real IntervalFirst(const gp_Lin& C);

  //! Returns  the  last  parameter    of  the  current
  //! interval.
  static Standard_Real IntervalLast(const gp_Lin& C);

  static GeomAbs_Shape IntervalContinuity(const gp_Lin& C);

  static Standard_Boolean IsClosed(const gp_Lin& C);

  static Standard_Boolean IsPeriodic(const gp_Lin& C);

  static Standard_Real Period(const gp_Lin& C);

  //! Computes the point of parameter U on the line.
  static gp_Pnt Value(const gp_Lin& C, const Standard_Real U);

  //! Computes the point of parameter U on the line.
  static void D0(const gp_Lin& C, const Standard_Real U, gp_Pnt& P);

  //! Computes the point of parameter U on the line with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  static void D1(const gp_Lin& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V);

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  static void D2(const gp_Lin& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  static void D3(const gp_Lin&       C,
                 const Standard_Real U,
                 gp_Pnt&             P,
                 gp_Vec&             V1,
                 gp_Vec&             V2,
                 gp_Vec&             V3);

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  static gp_Vec DN(const gp_Lin& C, const Standard_Real U, const Standard_Integer N);

  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
  static Standard_Real Resolution(const gp_Lin& C, const Standard_Real R3d);

  //! Returns  the  type of the   line  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  static GeomAbs_CurveType GetType(const gp_Lin& C);

  static gp_Lin Line(const gp_Lin& C);

  static gp_Circ Circle(const gp_Lin& C);

  static gp_Elips Ellipse(const gp_Lin& C);

  static gp_Hypr Hyperbola(const gp_Lin& C);

  static gp_Parab Parabola(const gp_Lin& C);

  static Handle(Geom_BezierCurve) Bezier(const gp_Lin& C);

  static Handle(Geom_BSplineCurve) BSpline(const gp_Lin& C);

  static Standard_Integer Degree(const gp_Lin& C);

  static Standard_Integer NbPoles(const gp_Lin& C);

  static void Poles(const gp_Lin& C, TColgp_Array1OfPnt& TP);

  static Standard_Boolean IsRational(const gp_Lin& C);

  static void PolesAndWeights(const gp_Lin& C, TColgp_Array1OfPnt& TP, TColStd_Array1OfReal& TW);

  static Standard_Integer NbKnots(const gp_Lin& C);

  static void KnotsAndMultiplicities(const gp_Lin&            C,
                                     TColStd_Array1OfReal&    TK,
                                     TColStd_Array1OfInteger& TM);

  static Standard_Integer NbSamples(const gp_Lin&       C,
                                    const Standard_Real U0,
                                    const Standard_Real U1);

  static void SamplePars(const gp_Lin&                  C,
                         const Standard_Real            U0,
                         const Standard_Real            U1,
                         const Standard_Real            Defl,
                         const Standard_Integer         NbMin,
                         Handle(TColStd_HArray1OfReal)& Pars);

protected:
private:
};

#include <HLRBRep_LineTool.lxx>

#endif // _HLRBRep_LineTool_HeaderFile

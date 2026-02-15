// Created on: 1993-03-31
// Created by: Bruno DUMORTIER
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

#ifndef _Adaptor3d_Curve_HeaderFile
#define _Adaptor3d_Curve_HeaderFile

#include <Geom_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_CurveType.hxx>

class Geom_BezierCurve;
class Geom_BSplineCurve;
class Geom_OffsetCurve;

//! Root class for 3D curves on which geometric
//! algorithms work.
//! An adapted curve is an interface between the
//! services provided by a curve and those required of
//! the curve by algorithms which use it.
//! Two derived concrete classes are provided:
//! - GeomAdaptor_Curve for a curve from the Geom package
//! - Adaptor3d_CurveOnSurface for a curve lying on
//! a surface from the Geom package.
//!
//! Polynomial coefficients of BSpline curves used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class Adaptor3d_Curve : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Adaptor3d_Curve, Standard_Transient)
public:
  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> ShallowCopy() const;

  Standard_EXPORT virtual double FirstParameter() const;

  Standard_EXPORT virtual double LastParameter() const;

  Standard_EXPORT virtual GeomAbs_Shape Continuity() const;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape         S) const;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> Trim(const double First,
                                                            const double Last,
                                                            const double Tol) const;

  Standard_EXPORT virtual bool IsClosed() const;

  Standard_EXPORT virtual bool IsPeriodic() const;

  Standard_EXPORT virtual double Period() const;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT virtual gp_Pnt Value(const double U) const;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT virtual void D0(const double U, gp_Pnt& P) const;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT virtual void D1(const double U, gp_Pnt& P, gp_Vec& V) const;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT virtual void D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT virtual void D3(const double U,
                                  gp_Pnt&      P,
                                  gp_Vec&      V1,
                                  gp_Vec&      V2,
                                  gp_Vec&      V3) const;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT virtual gp_Vec DN(const double U, const int N) const;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT virtual double Resolution(const double R3d) const;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT virtual GeomAbs_CurveType GetType() const;

  Standard_EXPORT virtual gp_Lin Line() const;

  Standard_EXPORT virtual gp_Circ Circle() const;

  Standard_EXPORT virtual gp_Elips Ellipse() const;

  Standard_EXPORT virtual gp_Hypr Hyperbola() const;

  Standard_EXPORT virtual gp_Parab Parabola() const;

  Standard_EXPORT virtual int Degree() const;

  Standard_EXPORT virtual bool IsRational() const;

  Standard_EXPORT virtual int NbPoles() const;

  Standard_EXPORT virtual int NbKnots() const;

  Standard_EXPORT virtual occ::handle<Geom_BezierCurve> Bezier() const;

  Standard_EXPORT virtual occ::handle<Geom_BSplineCurve> BSpline() const;

  Standard_EXPORT virtual occ::handle<Geom_OffsetCurve> OffsetCurve() const;

  //! Computes the point of parameter U on the curve.
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual gp_Pnt EvalD0(double U) const;

  //! Computes the point and first derivative at parameter U.
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual Geom_Curve::ResD1 EvalD1(double U) const;

  //! Computes the point and first two derivatives at parameter U.
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual Geom_Curve::ResD2 EvalD2(double U) const;

  //! Computes the point and first three derivatives at parameter U.
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual Geom_Curve::ResD3 EvalD3(double U) const;

  //! Computes the Nth derivative at parameter U.
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual gp_Vec EvalDN(double U, int N) const;

  Standard_EXPORT ~Adaptor3d_Curve() override;
};

#endif // _Adaptor3d_Curve_HeaderFile

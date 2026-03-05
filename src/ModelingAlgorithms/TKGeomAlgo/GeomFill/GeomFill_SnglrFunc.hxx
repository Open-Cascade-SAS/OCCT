// Created on: 1998-02-26
// Created by: Roman BORISOV
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomFill_SnglrFunc_HeaderFile
#define _GeomFill_SnglrFunc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_CurveType.hxx>

class gp_Pnt;
class gp_Vec;

//! to represent function C'(t)^C''(t)
class GeomFill_SnglrFunc : public Adaptor3d_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_SnglrFunc(const occ::handle<Adaptor3d_Curve>& HC);

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  Standard_EXPORT void SetRatio(const double Ratio);

  Standard_EXPORT double FirstParameter() const override;

  Standard_EXPORT double LastParameter() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT double Period() const override;

  //! Computes the point of parameter theU on the curve.
  [[nodiscard]] Standard_EXPORT gp_Pnt EvalD0(double theU) const final;

  //! Computes the point of parameter theU on the curve with its first derivative.
  //! Raised if the continuity of the current interval is not C1.
  [[nodiscard]] Standard_EXPORT Geom_Curve::ResD1 EvalD1(double theU) const final;

  //! Returns the point and the first and second derivatives at parameter theU.
  //! Raised if the continuity of the current interval is not C2.
  [[nodiscard]] Standard_EXPORT Geom_Curve::ResD2 EvalD2(double theU) const final;

  //! Returns the point and the first, second and third derivatives at parameter theU.
  //! Raised if the continuity of the current interval is not C3.
  [[nodiscard]] Standard_EXPORT Geom_Curve::ResD3 EvalD3(double theU) const final;

  //! Returns the derivative of order theN at parameter theU.
  //! Raised if theN < 1.
  [[nodiscard]] Standard_EXPORT gp_Vec EvalDN(double theU, int theN) const final;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double Resolution(const double R3d) const override;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const override;

private:
  occ::handle<Adaptor3d_Curve> myHCurve;
  double                       ratio;
};

#endif // _GeomFill_SnglrFunc_HeaderFile

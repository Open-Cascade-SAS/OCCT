// Created on: 1994-05-19
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Bisector_BisecAna_HeaderFile
#define _Bisector_BisecAna_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Bisector_Curve.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_JoinType.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_TrimmedCurve;
class Geom2d_Curve;
class gp_Pnt2d;
class gp_Vec2d;
class Geom2d_Point;
class GccInt_Bisec;
class Geom2d_Geometry;
class gp_Trsf2d;

//! This class provides the bisecting line between two
//! geometric elements.The elements are Circles,Lines or
//! Points.
class Bisector_BisecAna : public Bisector_Curve
{

public:
  Standard_EXPORT Bisector_BisecAna();

  //! Performs the bisecting line between the curves
  //! <Cu1> and <Cu2>.
  //! <oncurve> is True if the point <P> is common to <Cu1>
  //! and <Cu2>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Curve>& Cu1,
                               const occ::handle<Geom2d_Curve>& Cu2,
                               const gp_Pnt2d&                  P,
                               const gp_Vec2d&                  V1,
                               const gp_Vec2d&                  V2,
                               const double                     Sense,
                               const GeomAbs_JoinType           jointype,
                               const double                     Tolerance,
                               const bool                       oncurve = true);

  //! Performs the bisecting line between the curve
  //! <Cu1> and the point <Pnt>.
  //! <oncurve> is True if the point <P> is the point <Pnt>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Curve>& Cu,
                               const occ::handle<Geom2d_Point>& Pnt,
                               const gp_Pnt2d&                  P,
                               const gp_Vec2d&                  V1,
                               const gp_Vec2d&                  V2,
                               const double                     Sense,
                               const double                     Tolerance,
                               const bool                       oncurve = true);

  //! Performs the bisecting line between the curve
  //! <Cu> and the point <Pnt>.
  //! <oncurve> is True if the point <P> is the point <Pnt>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Point>& Pnt,
                               const occ::handle<Geom2d_Curve>& Cu,
                               const gp_Pnt2d&                  P,
                               const gp_Vec2d&                  V1,
                               const gp_Vec2d&                  V2,
                               const double                     Sense,
                               const double                     Tolerance,
                               const bool                       oncurve = true);

  //! Performs the bisecting line between the two points
  //! <Pnt1> and <Pnt2>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Point>& Pnt1,
                               const occ::handle<Geom2d_Point>& Pnt2,
                               const gp_Pnt2d&                  P,
                               const gp_Vec2d&                  V1,
                               const gp_Vec2d&                  V2,
                               const double                     Sense,
                               const double                     Tolerance = 0.0,
                               const bool                       oncurve   = true);

  Standard_EXPORT void Init(const occ::handle<Geom2d_TrimmedCurve>& bisector);

  Standard_EXPORT bool IsExtendAtStart() const override;

  Standard_EXPORT bool IsExtendAtEnd() const override;

  //! Trim <me> by a domain defined by the curve <Cu>.
  //! This domain is the set of the points which are
  //! nearest from <Cu> than the extremitis of <Cu>.
  Standard_EXPORT void SetTrim(const occ::handle<Geom2d_Curve>& Cu);

  //! Trim <me> by a domain defined by uf and ul
  Standard_EXPORT void SetTrim(const double uf, const double ul);

  Standard_EXPORT void Reverse() override;

  Standard_EXPORT double ReversedParameter(const double U) const override;

  //! Returns the order of continuity of the curve.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const override;

  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const override;

  Standard_EXPORT void Transform(const gp_Trsf2d& T) override;

  Standard_EXPORT double FirstParameter() const override;

  Standard_EXPORT double LastParameter() const override;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  Standard_EXPORT gp_Pnt2d EvalD0(const double U) const override;

  Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double U) const override;

  Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double U) const override;

  Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double U) const override;

  Standard_EXPORT gp_Vec2d EvalDN(const double U, const int N) const override;

  Standard_EXPORT occ::handle<Geom2d_Curve> Geom2dCurve() const;

  Standard_EXPORT double Parameter(const gp_Pnt2d& P) const override;

  Standard_EXPORT double ParameterOfStartPoint() const;

  Standard_EXPORT double ParameterOfEndPoint() const;

  //! If necessary, breaks the curve in intervals of
  //! continuity <C1>. And returns the number of
  //! intervals.
  Standard_EXPORT int NbIntervals() const override;

  //! Returns the first parameter of the current
  //! interval.
  Standard_EXPORT double IntervalFirst(const int Index) const override;

  //! Returns the last parameter of the current
  //! interval.
  Standard_EXPORT double IntervalLast(const int Index) const override;

  Standard_EXPORT void Dump(const int Deep = 0, const int Offset = 0) const;

  DEFINE_STANDARD_RTTIEXT(Bisector_BisecAna, Bisector_Curve)

private:
  //! Returns the distance between the point <P> and
  //! the bisecting <Bis>.
  Standard_EXPORT double Distance(const gp_Pnt2d&                  P,
                                  const occ::handle<GccInt_Bisec>& Bis,
                                  const gp_Vec2d&                  V1,
                                  const gp_Vec2d&                  V2,
                                  const gp_Vec2d&                  VecRef,
                                  const double                     Sense,
                                  double&                          U,
                                  bool&                            sense,
                                  bool&                            ok,
                                  const bool                       IsBisecOfTwoLines = false);

  occ::handle<Geom2d_TrimmedCurve> thebisector;
};

#endif // _Bisector_BisecAna_HeaderFile

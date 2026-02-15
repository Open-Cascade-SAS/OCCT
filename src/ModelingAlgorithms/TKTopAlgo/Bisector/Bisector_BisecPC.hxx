// Created on: 1994-01-10
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

#ifndef _Bisector_BisecPC_HeaderFile
#define _Bisector_BisecPC_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <Bisector_Curve.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_Curve;
class Geom2d_Geometry;
class gp_Trsf2d;
class gp_Vec2d;

//! Provides the bisector between a point and a curve.
//! the curvature on the curve has to be monoton.
//! the point can't be on the curve except at the extremities.
class Bisector_BisecPC : public Bisector_Curve
{

public:
  Standard_EXPORT Bisector_BisecPC();

  //! Constructs the bisector between the point <P> and
  //! the curve <Cu>.
  //! <Side> = 1. if the bisector curve is on the Left of <Cu>
  //! else <Side> = -1.
  //! <DistMax> is used to trim the bisector.The distance
  //! between the points of the bisector and <Cu> is smaller
  //! than <DistMax>.
  Standard_EXPORT Bisector_BisecPC(const occ::handle<Geom2d_Curve>& Cu,
                                   const gp_Pnt2d&                  P,
                                   const double                     Side,
                                   const double                     DistMax = 500);

  //! Constructs the bisector between the point <P> and
  //! the curve <Cu> Trimmed by <UMin> and <UMax>
  //! <Side> = 1. if the bisector curve is on the Left of <Cu>
  //! else <Side> = -1.
  //! Warning: the bisector is supposed all over defined between
  //! <UMin> and <UMax>.
  Standard_EXPORT Bisector_BisecPC(const occ::handle<Geom2d_Curve>& Cu,
                                   const gp_Pnt2d&                  P,
                                   const double                     Side,
                                   const double                     UMin,
                                   const double                     UMax);

  //! Construct the bisector between the point <P> and
  //! the curve <Cu>.
  //! <Side> = 1. if the bisector curve is on the Left of <Cu>
  //! else <Side> = -1.
  //! <DistMax> is used to trim the bisector.The distance
  //! between the points of the bisector and <Cu> is smaller
  //! than <DistMax>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Curve>& Cu,
                               const gp_Pnt2d&                  P,
                               const double                     Side,
                               const double                     DistMax = 500);

  //! Returns True if the bisector is extended at start.
  Standard_EXPORT bool IsExtendAtStart() const override;

  //! Returns True if the bisector is extended at end.
  Standard_EXPORT bool IsExtendAtEnd() const override;

  //! Changes the direction of parametrization of <me>.
  //! The orientation of the curve is modified. If the curve
  //! is bounded the StartPoint of the initial curve becomes the
  //! EndPoint of the reversed curve and the EndPoint of the initial
  //! curve becomes the StartPoint of the reversed curve.
  Standard_EXPORT void Reverse() override;

  //! Returns the parameter on the reversed curve for
  //! the point of parameter U on <me>.
  Standard_EXPORT double ReversedParameter(const double U) const override;

  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const override;

  //! Transformation of a geometric object. This transformation
  //! can be a translation, a rotation, a symmetry, a scaling
  //! or a complex transformation obtained by combination of
  //! the previous elementaries transformations.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) override;

  //! Returns the order of continuity of the curve.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const override;

  //! Value of the first parameter.
  Standard_EXPORT double FirstParameter() const override;

  //! Value of the last parameter.
  Standard_EXPORT double LastParameter() const override;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

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

  Standard_EXPORT GeomAbs_Shape IntervalContinuity() const;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  //! Returns the distance between the point of
  //! parameter U on <me> and my point or my curve.
  Standard_EXPORT double Distance(const double U) const;

  Standard_EXPORT gp_Pnt2d EvalD0(const double U) const override;

  Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double U) const override;

  Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double U) const override;

  Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double U) const override;

  Standard_EXPORT gp_Vec2d EvalDN(const double U, const int N) const override;

  Standard_EXPORT void Dump(const int Deep = 0, const int Offset = 0) const;

  //! Returns the parameter on the curve1 of the projection
  //! of the point of parameter U on <me>.
  Standard_EXPORT double LinkBisCurve(const double U) const;

  //! Returns the reciproque of LinkBisCurve.
  Standard_EXPORT double LinkCurveBis(const double U) const;

  //! Returns the parameter on <me> corresponding to <P>.
  Standard_EXPORT double Parameter(const gp_Pnt2d& P) const override;

  //! Returns <True> if the bisector is empty.
  Standard_EXPORT bool IsEmpty() const;

  DEFINE_STANDARD_RTTIEXT(Bisector_BisecPC, Bisector_Curve)

private:
  Standard_EXPORT void Values(const double U,
                              const int    N,
                              gp_Pnt2d&    P,
                              gp_Vec2d&    V1,
                              gp_Vec2d&    V2,
                              gp_Vec2d&    V3) const;

  Standard_EXPORT void Extension(const double U,
                                 gp_Pnt2d&    P,
                                 gp_Vec2d&    V1,
                                 gp_Vec2d&    V2,
                                 gp_Vec2d&    V3) const;

  //! Computes the interval where the bisector is defined.
  Standard_EXPORT void ComputeIntervals();

  Standard_EXPORT void CuspFilter();

  Standard_EXPORT double SearchBound(const double U1, const double U2) const;

  Standard_EXPORT void Init(const occ::handle<Geom2d_Curve>&    Curve,
                            const gp_Pnt2d&                     Point,
                            const double                        Sign,
                            const NCollection_Sequence<double>& StartIntervals,
                            const NCollection_Sequence<double>& EndIntervals,
                            const int                           BisInterval,
                            const int                           CurrentInterval,
                            const double                        ShiftParameter,
                            const double                        DistMax,
                            const bool                          IsEmpty,
                            const bool                          IsConvex,
                            const bool                          ExtensionStart,
                            const bool                          ExtensionEnd,
                            const gp_Pnt2d&                     PointStartBis,
                            const gp_Pnt2d&                     PointEndBis);

  occ::handle<Geom2d_Curve>    curve;
  gp_Pnt2d                     point;
  double                       sign;
  NCollection_Sequence<double> startIntervals;
  NCollection_Sequence<double> endIntervals;
  int                          bisInterval;
  int                          currentInterval;
  double                       shiftParameter;
  double                       distMax;
  bool                         isEmpty;
  bool                         isConvex;
  bool                         extensionStart;
  bool                         extensionEnd;
  gp_Pnt2d                     pointStartBis;
  gp_Pnt2d                     pointEndBis;
};

#endif // _Bisector_BisecPC_HeaderFile

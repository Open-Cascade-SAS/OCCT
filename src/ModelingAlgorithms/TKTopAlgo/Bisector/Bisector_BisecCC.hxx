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

#ifndef _Bisector_BisecCC_HeaderFile
#define _Bisector_BisecCC_HeaderFile

#include <Standard.hxx>

#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <Bisector_PolyBis.hxx>
#include <gp_Pnt2d.hxx>
#include <Bisector_Curve.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_Curve;
class Geom2d_Geometry;
class gp_Trsf2d;
class gp_Vec2d;

//! Construct the bisector between two curves.
//! The curves can intersect only in their extremities.
class Bisector_BisecCC : public Bisector_Curve
{

public:

  Standard_EXPORT Bisector_BisecCC();

  //! Constructs the bisector between the curves <Cu1>
  //! and <Cu2>.
  //!
  //! <Side1> (resp <Side2>) = 1 if the
  //! bisector curve is on the left of <Cu1> (resp <Cu2>)
  //! else <Side1> (resp <Side2>) = -1.
  //!
  //! the Bisector is trimmed by the Point <Origin>.
  //! <DistMax> is used to trim the bisector.The distance
  //! between the points of the bisector and <Cu> is smaller
  //! than <DistMax>.
  Standard_EXPORT Bisector_BisecCC(const occ::handle<Geom2d_Curve>& Cu1,
                                   const occ::handle<Geom2d_Curve>& Cu2,
                                   const double                     Side1,
                                   const double                     Side2,
                                   const gp_Pnt2d&                  Origin,
                                   const double                     DistMax = 500);

  //! Computes the bisector between the curves <Cu1>
  //! and <Cu2>.
  //!
  //! <Side1> (resp <Side2>) = 1 if the
  //! bisector curve is on the left of <Cu1> (resp <Cu2>)
  //! else <Side1> (resp <Side2>) = -1.
  //!
  //! the Bisector is trimmed by the Point <Origin>.
  //!
  //! <DistMax> is used to trim the bisector.The distance
  //! between the points of the bisector and <Cu> is smaller
  //! than <DistMax>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Curve>& Cu1,
                               const occ::handle<Geom2d_Curve>& Cu2,
                               const double                     Side1,
                               const double                     Side2,
                               const gp_Pnt2d&                  Origin,
                               const double                     DistMax = 500);

  Standard_EXPORT bool IsExtendAtStart() const override;

  Standard_EXPORT bool IsExtendAtEnd() const override;

  Standard_EXPORT void Reverse() override;

  Standard_EXPORT double ReversedParameter(const double U) const override;

  //! Returns the order of continuity of the curve.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const override;

  //! The parameter on <me> is linked to the parameter
  //! on the first curve. This method creates the same bisector
  //! where the curves are inversed.
  Standard_EXPORT occ::handle<Bisector_BisecCC> ChangeGuide() const;

  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const override;

  //! Transformation of a geometric object. This transformation
  //! can be a translation, a rotation, a symmetry, a scaling
  //! or a complex transformation obtained by combination of
  //! the previous elementaries transformations.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) override;

  Standard_EXPORT double FirstParameter() const override;

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

  //! Returns the point of parameter U.
  //! Computes the distance between the current point and
  //! the two curves I separate.
  //! Computes the parameters on each curve corresponding
  //! of the projection of the current point.
  Standard_EXPORT gp_Pnt2d ValueAndDist(const double U,
                                        double&      U1,
                                        double&      U2,
                                        double&      Distance) const;

  //! Returns the point of parameter U.
  //! Computes the distance between the current point and
  //! the two curves I separate.
  //! Computes the parameters on each curve corresponding
  //! of the projection of the current point.
  Standard_EXPORT gp_Pnt2d ValueByInt(const double U,
                                      double&      U1,
                                      double&      U2,
                                      double&      Distance) const;

  Standard_EXPORT std::optional<gp_Pnt2d> EvalD0(const double U) const override;

  Standard_EXPORT std::optional<Geom2d_CurveD1> EvalD1(const double U) const override;

  Standard_EXPORT std::optional<Geom2d_CurveD2> EvalD2(const double U) const override;

  Standard_EXPORT std::optional<Geom2d_CurveD3> EvalD3(const double U) const override;

  Standard_EXPORT std::optional<gp_Vec2d> EvalDN(const double U, const int N) const override;

  Standard_EXPORT bool IsEmpty() const;

  //! Returns the parameter on the curve1 of the projection
  //! of the point of parameter U on <me>.
  Standard_EXPORT double LinkBisCurve(const double U) const;

  //! Returns the reciproque of LinkBisCurve.
  Standard_EXPORT double LinkCurveBis(const double U) const;

  Standard_EXPORT double Parameter(const gp_Pnt2d& P) const override;

  Standard_EXPORT occ::handle<Geom2d_Curve> Curve(const int IndCurve) const;

  Standard_EXPORT const Bisector_PolyBis& Polygon() const;

  Standard_EXPORT void Dump(const int Deep = 0, const int Offset = 0) const;

  DEFINE_STANDARD_RTTIEXT(Bisector_BisecCC, Bisector_Curve)

private:
  Standard_EXPORT void Values(const double U,
                              const int    N,
                              gp_Pnt2d&    P,
                              gp_Vec2d&    V1,
                              gp_Vec2d&    V2,
                              gp_Vec2d&    V3) const;

  Standard_EXPORT void SupLastParameter();

  Standard_EXPORT gp_Pnt2d
    Extension(const double U, double& U1, double& U2, double& Dist, gp_Vec2d& T1) const;

  Standard_EXPORT double SearchBound(const double U1, const double U2) const;

  Standard_EXPORT void ComputePointEnd();

  Standard_EXPORT void Curve(const int Index, const occ::handle<Geom2d_Curve>& C);

  Standard_EXPORT void Sign(const int Index, const double Sign);

  Standard_EXPORT void Polygon(const Bisector_PolyBis& Poly);

  Standard_EXPORT void DistMax(const double DistMax);

  Standard_EXPORT void IsConvex(const int Index, const bool IsConvex);

  Standard_EXPORT void IsEmpty(const bool IsEmpty);

  Standard_EXPORT void ExtensionStart(const bool ExtensionStart);

  Standard_EXPORT void ExtensionEnd(const bool ExtensionEnd);

  Standard_EXPORT void PointStart(const gp_Pnt2d& Point);

  Standard_EXPORT void PointEnd(const gp_Pnt2d& Point);

  Standard_EXPORT void StartIntervals(const NCollection_Sequence<double>& StartIntervals);

  Standard_EXPORT void EndIntervals(const NCollection_Sequence<double>& EndIntervals);

  Standard_EXPORT void FirstParameter(const double U1);

  Standard_EXPORT void LastParameter(const double U1);

  occ::handle<Geom2d_Curve>    curve1;
  occ::handle<Geom2d_Curve>    curve2;
  double                       sign1;
  double                       sign2;
  NCollection_Sequence<double> startIntervals;
  NCollection_Sequence<double> endIntervals;
  int                          currentInterval;
  Bisector_PolyBis             myPolygon;
  double                       shiftParameter;
  double                       distMax;
  bool                         isEmpty;
  bool                         isConvex1;
  bool                         isConvex2;
  bool                         extensionStart;
  bool                         extensionEnd;
  gp_Pnt2d                     pointStart;
  gp_Pnt2d                     pointEnd;
};

#endif // _Bisector_BisecCC_HeaderFile

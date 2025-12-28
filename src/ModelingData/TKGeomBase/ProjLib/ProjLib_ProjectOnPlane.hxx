// Created on: 1994-09-02
// Created by: Bruno DUMORTIER
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

#ifndef _ProjLib_ProjectOnPlane_HeaderFile
#define _ProjLib_ProjectOnPlane_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>

class gp_Pnt;
class gp_Vec;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! Class used to project a 3d curve on a plane. The
//! result will be a 3d curve.
//!
//! You can ask the projected curve to have the same
//! parametrization as the original curve.
//!
//! The projection can be done along every direction not
//! parallel to the plane.
class ProjLib_ProjectOnPlane : public Adaptor3d_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT ProjLib_ProjectOnPlane();

  //! The projection will be normal to the Plane defined
  //! by the Ax3 <Pl>.
  Standard_EXPORT ProjLib_ProjectOnPlane(const gp_Ax3& Pl);

  //! The projection will be along the direction <D> on
  //! the plane defined by the Ax3 <Pl>.
  //! raises if the direction <D> is parallel to the
  //! plane <Pl>.
  Standard_EXPORT ProjLib_ProjectOnPlane(const gp_Ax3& Pl, const gp_Dir& D);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Sets the Curve and perform the projection.
  //! if <KeepParametrization> is true, the parametrization
  //! of the Projected Curve <PC> will be the same as
  //! the parametrization of the initial curve <C>.
  //! It means: proj(C(u)) = PC(u) for each u.
  //! Otherwise, the parametrization may change.
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Curve>& C,
                            const double                        Tolerance,
                            const bool                          KeepParametrization = true);

  Standard_EXPORT const gp_Ax3& GetPlane() const;

  Standard_EXPORT const gp_Dir& GetDirection() const;

  Standard_EXPORT const occ::handle<Adaptor3d_Curve>& GetCurve() const;

  Standard_EXPORT const occ::handle<GeomAdaptor_Curve>& GetResult() const;

  Standard_EXPORT double FirstParameter() const override;

  Standard_EXPORT double LastParameter() const override;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! If necessary, breaks the curve in intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Curve> Trim(const double First,
                                                    const double Last,
                                                    const double Tol) const override;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT double Period() const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt Value(const double U) const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const override;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V) const override;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const override;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt&      P,
                          gp_Vec&      V1,
                          gp_Vec&      V2,
                          gp_Vec&      V3) const override;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const double U, const int N) const override;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double Resolution(const double R3d) const override;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const override;

  Standard_EXPORT gp_Lin Line() const override;

  Standard_EXPORT gp_Circ Circle() const override;

  Standard_EXPORT gp_Elips Ellipse() const override;

  Standard_EXPORT gp_Hypr Hyperbola() const override;

  Standard_EXPORT gp_Parab Parabola() const override;

  Standard_EXPORT int Degree() const override;

  Standard_EXPORT bool IsRational() const override;

  Standard_EXPORT int NbPoles() const override;

  Standard_EXPORT int NbKnots() const override;

  //! Warning ! this will NOT make a copy of the
  //! Bezier Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT occ::handle<Geom_BezierCurve> Bezier() const override;

  //! Warning ! this will NOT make a copy of the
  //! BSpline Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSpline() const override;

protected:
  void GetTrimmedResult(const occ::handle<Geom_Curve>& theProjCurve);

  bool BuildParabolaByApex(occ::handle<Geom_Curve>& theGeomParabolaPtr);
  bool BuildHyperbolaByApex(occ::handle<Geom_Curve>& theGeomParabolaPtr);

  void BuildByApprox(const double theLimitParameter);

private:
  occ::handle<Adaptor3d_Curve>   myCurve;
  gp_Ax3                         myPlane;
  gp_Dir                         myDirection;
  bool                           myKeepParam;
  double                         myFirstPar;
  double                         myLastPar;
  double                         myTolerance;
  GeomAbs_CurveType              myType;
  occ::handle<GeomAdaptor_Curve> myResult;
  bool                           myIsApprox;
};

#endif // _ProjLib_ProjectOnPlane_HeaderFile

// Created on: 1993-06-03
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

#ifndef _Geom2dAdaptor_Curve_HeaderFile
#define _Geom2dAdaptor_Curve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <BSplCLib_Cache.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <NCollection_Array1.hxx>

#include <variant>

class gp_Vec2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;
namespace Geom2d_EvalRepCurveDesc
{
class Base;
}

//! An interface between the services provided by any
//! curve from the package Geom2d and those required
//! of the curve by algorithms which use it.
//!
//! Polynomial coefficients of BSpline curves used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class Geom2dAdaptor_Curve : public Adaptor2d_Curve2d
{
  DEFINE_STANDARD_RTTIEXT(Geom2dAdaptor_Curve, Adaptor2d_Curve2d)
public:
  //! Internal structure for 2D offset curve evaluation data.
  struct OffsetData
  {
    occ::handle<Geom2dAdaptor_Curve> BasisAdaptor; //!< Adaptor for basis curve
    double                           Offset = 0.0; //!< Offset distance
    occ::handle<Geom2d_EvalRepCurveDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Internal structure for Bezier curve evaluation data.
  struct BezierData
  {
    occ::handle<Geom2d_BezierCurve>    Curve; //!< Bezier curve to prevent downcasts
    mutable occ::handle<BSplCLib_Cache> Cache; //!< Cached data for evaluation
    occ::handle<Geom2d_EvalRepCurveDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Internal structure for BSpline curve evaluation data.
  struct BSplineData
  {
    occ::handle<Geom2d_BSplineCurve>    Curve; //!< BSpline curve to prevent downcasts
    mutable occ::handle<BSplCLib_Cache> Cache; //!< Cached data for evaluation
    occ::handle<Geom2d_EvalRepCurveDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Variant type for 2D curve-specific evaluation data.
  using CurveDataVariant = std::variant<std::monostate,
                                        gp_Lin2d,
                                        gp_Circ2d,
                                        gp_Elips2d,
                                        gp_Hypr2d,
                                        gp_Parab2d,
                                        OffsetData,
                                        BezierData,
                                        BSplineData>;

public:
  Standard_EXPORT Geom2dAdaptor_Curve();

  Standard_EXPORT Geom2dAdaptor_Curve(const occ::handle<Geom2d_Curve>& C);

  //! Standard_ConstructionError is raised if Ufirst>Ulast
  Standard_EXPORT Geom2dAdaptor_Curve(const occ::handle<Geom2d_Curve>& C,
                                      const double                     UFirst,
                                      const double                     ULast);

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> ShallowCopy() const override;

  //! Reset currently loaded curve (undone Load()).
  Standard_EXPORT void Reset();

  void Load(const occ::handle<Geom2d_Curve>& theCurve)
  {
    if (theCurve.IsNull())
    {
      throw Standard_NullObject();
    }
    load(theCurve, theCurve->FirstParameter(), theCurve->LastParameter());
  }

  //! Standard_ConstructionError is raised if theUFirst > theULast + Precision::PConfusion()
  void Load(const occ::handle<Geom2d_Curve>& theCurve,
            const double                     theUFirst,
            const double                     theULast)
  {
    if (theCurve.IsNull())
    {
      throw Standard_NullObject();
    }
    if (theUFirst > theULast + Precision::Confusion())
    {
      throw Standard_ConstructionError();
    }
    load(theCurve, theUFirst, theULast);
  }

  const occ::handle<Geom2d_Curve>& Curve() const { return myCurve; }

  double FirstParameter() const override { return myFirst; }

  double LastParameter() const override { return myLast; }

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! If necessary, breaks the curve in intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> Trim(const double First,
                                                      const double Last,
                                                      const double Tol) const override;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT double Period() const override;

  //! Computes the point of parameter U on the curve
  Standard_EXPORT gp_Pnt2d Value(const double U) const final;

  //! Computes the point of parameter U.
  Standard_EXPORT void D0(const double U, gp_Pnt2d& P) const final;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt2d& P, gp_Vec2d& V) const final;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const final;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt2d&    P,
                          gp_Vec2d&    V1,
                          gp_Vec2d&    V2,
                          gp_Vec2d&    V3) const final;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec2d DN(const double U, const int N) const final;

  //! returns the parametric resolution
  Standard_EXPORT double Resolution(const double Ruv) const override;

  GeomAbs_CurveType GetType() const override { return myTypeCurve; }

  Standard_EXPORT gp_Lin2d Line() const override;

  Standard_EXPORT gp_Circ2d Circle() const override;

  Standard_EXPORT gp_Elips2d Ellipse() const override;

  Standard_EXPORT gp_Hypr2d Hyperbola() const override;

  Standard_EXPORT gp_Parab2d Parabola() const override;

  Standard_EXPORT int Degree() const override;

  Standard_EXPORT bool IsRational() const override;

  Standard_EXPORT int NbPoles() const override;

  Standard_EXPORT int NbKnots() const override;

  Standard_EXPORT int NbSamples() const override;

  Standard_EXPORT occ::handle<Geom2d_BezierCurve> Bezier() const override;

  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> BSpline() const override;

  //! Non-throwing point evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<gp_Pnt2d> EvalD0(double U) const final;

  //! Non-throwing D1 evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<Geom2d_Curve::ResD1> EvalD1(double U) const final;

  //! Non-throwing D2 evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<Geom2d_Curve::ResD2> EvalD2(double U) const final;

  //! Non-throwing D3 evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<Geom2d_Curve::ResD3> EvalD3(double U) const final;

  //! Non-throwing DN evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<gp_Vec2d> EvalDN(double U, int N) const final;

private:
  Standard_EXPORT GeomAbs_Shape LocalContinuity(const double U1, const double U2) const;

  Standard_EXPORT void load(const occ::handle<Geom2d_Curve>& C,
                            const double                     UFirst,
                            const double                     ULast);

  //! Check theU relates to start or finish point of B-spline curve and return indices of span the
  //! point is located
  bool IsBoundary(const double theU, int& theSpanStart, int& theSpanFinish) const;

  //! Rebuilds B-spline cache
  //! \param theParameter the value on the knot axis which identifies the caching span
  void RebuildCache(const double theParameter) const;

protected:
  occ::handle<Geom2d_Curve> myCurve;
  GeomAbs_CurveType         myTypeCurve;
  double                    myFirst;
  double                    myLast;
  CurveDataVariant myCurveData; ///< Curve-specific evaluation data (BSpline, Bezier, offset)
};

#endif // _Geom2dAdaptor_Curve_HeaderFile

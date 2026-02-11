// Created on: 1992-09-01
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GeomAdaptor_Curve_HeaderFile
#define _GeomAdaptor_Curve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <BSplCLib_Cache.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>

#include <variant>

class Geom_BSplineCurve;

//! This class provides an interface between the services provided by any
//! curve from the package Geom and those required of the curve by algorithms which use it.
//! Creation of the loaded curve the curve is C1 by piece.
//!
//! Polynomial coefficients of BSpline curves used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class GeomAdaptor_Curve : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_Curve, Adaptor3d_Curve)
public:
  //! Internal structure for offset curve evaluation data.
  struct OffsetData
  {
    occ::handle<GeomAdaptor_Curve> BasisAdaptor; //!< Adaptor for basis curve
    double                         Offset = 0.0; //!< Offset distance
    gp_Dir                         Direction;    //!< Offset direction
  };

  //! Internal structure for Bezier curve cache data.
  struct BezierData
  {
    mutable occ::handle<BSplCLib_Cache> Cache; //!< Cached data for evaluation
  };

  //! Internal structure for BSpline curve cache data.
  struct BSplineData
  {
    occ::handle<Geom_BSplineCurve>      Curve; //!< BSpline curve to prevent downcasts
    mutable occ::handle<BSplCLib_Cache> Cache; //!< Cached data for evaluation
  };

  //! Variant type for curve-specific evaluation data.
  //! Elementary curve primitives (gp_Lin, gp_Circ, etc.) are stored directly
  //! to enable direct ElCLib dispatch without virtual calls.
  using CurveDataVariant = std::variant<std::monostate,
                                        gp_Lin,
                                        gp_Circ,
                                        gp_Elips,
                                        gp_Hypr,
                                        gp_Parab,
                                        OffsetData,
                                        BezierData,
                                        BSplineData>;

public:
  GeomAdaptor_Curve()
      : myTypeCurve(GeomAbs_OtherCurve),
        myFirst(0.0),
        myLast(0.0)
  {
  }

  GeomAdaptor_Curve(const occ::handle<Geom_Curve>& theCurve) { Load(theCurve); }

  //! Standard_ConstructionError is raised if theUFirst > theULast + Precision::PConfusion()
  GeomAdaptor_Curve(const occ::handle<Geom_Curve>& theCurve,
                    const double                   theUFirst,
                    const double                   theULast)
  {
    Load(theCurve, theUFirst, theULast);
  }

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Reset currently loaded curve (undone Load()).
  Standard_EXPORT void Reset();

  void Load(const occ::handle<Geom_Curve>& theCurve)
  {
    if (theCurve.IsNull())
    {
      throw Standard_NullObject();
    }
    load(theCurve, theCurve->FirstParameter(), theCurve->LastParameter());
  }

  //! Standard_ConstructionError is raised if theUFirst > theULast + Precision::PConfusion()
  void Load(const occ::handle<Geom_Curve>& theCurve, const double theUFirst, const double theULast)
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

  //! Provides a curve inherited from Hcurve from Adaptor.
  //! This is inherited to provide easy to use constructors.
  const occ::handle<Geom_Curve>& Curve() const { return myCurve; }

  double FirstParameter() const override { return myFirst; }

  double LastParameter() const override { return myLast; }

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

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

  //! Computes the point of parameter U on the curve
  Standard_EXPORT gp_Pnt Value(const double U) const final;

  //! Computes the point of parameter U.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const final;

  //! Computes the point of parameter U on the curve
  //! with its first derivative.
  //!
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity at least C1, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V) const final;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //!
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity at least C2, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  Standard_EXPORT void D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const final;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //!
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity at least C3, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt&      P,
                          gp_Vec&      V1,
                          gp_Vec&      V2,
                          gp_Vec&      V3) const final;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity CN, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const double U, const int N) const final;

  //! returns the parametric resolution
  Standard_EXPORT double Resolution(const double R3d) const override;

  GeomAbs_CurveType GetType() const override { return myTypeCurve; }

  Standard_EXPORT gp_Lin Line() const override;

  Standard_EXPORT gp_Circ Circle() const override;

  Standard_EXPORT gp_Elips Ellipse() const override;

  Standard_EXPORT gp_Hypr Hyperbola() const override;

  Standard_EXPORT gp_Parab Parabola() const override;

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT int Degree() const override;

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT bool IsRational() const override;

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT int NbPoles() const override;

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT int NbKnots() const override;

  //! this will NOT make a copy of the
  //! Bezier Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT occ::handle<Geom_BezierCurve> Bezier() const override;

  //! this will NOT make a copy of the
  //! BSpline Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSpline() const override;

  Standard_EXPORT occ::handle<Geom_OffsetCurve> OffsetCurve() const override;

  friend class GeomAdaptor_Surface;

private:
  Standard_EXPORT GeomAbs_Shape LocalContinuity(const double U1, const double U2) const;

  Standard_EXPORT void load(const occ::handle<Geom_Curve>& C,
                            const double                   UFirst,
                            const double                   ULast);

  //! Check theU relates to start or finish point of B-spline curve and return indices of span the
  //! point is located
  bool IsBoundary(const double theU, int& theSpanStart, int& theSpanFinish) const;

  //! Rebuilds B-spline cache
  //! \param theParameter the value on the knot axis which identifies the caching span
  void RebuildCache(const double theParameter) const;

private:
  occ::handle<Geom_Curve> myCurve;
  GeomAbs_CurveType       myTypeCurve;
  double                  myFirst;
  double                  myLast;
  CurveDataVariant myCurveData; ///< Curve-specific evaluation data (BSpline, Bezier, offset, etc.)
};

#endif // _GeomAdaptor_Curve_HeaderFile

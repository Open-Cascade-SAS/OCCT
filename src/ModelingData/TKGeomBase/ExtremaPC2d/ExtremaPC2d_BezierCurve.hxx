// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _ExtremaPC2d_BezierCurve_HeaderFile
#define _ExtremaPC2d_BezierCurve_HeaderFile

#include <ExtremaPC2d.hxx>
#include <ExtremaPC2d_GridEvaluator.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! @brief Point-BezierCurve extrema computation using numerical root isolation.
//!
//! Computes roots of the normalized point-curve stationarity function with a
//! derivative-aware multiple-root solver.
//!
//! The parameter partition is cached while geometry is evaluated for each query,
//! so mutations of the curve do not leave stale geometric samples.
//!
//! The algorithm:
//! 1. Build a parameter partition based on curve degree
//! 2. Isolate all roots of the stationarity function, including tangential roots
//! 3. Classify roots from the stationarity sign on both sides
//!
//! This approach is simpler and more stable than BVH-based methods,
//! with comparable accuracy for typical Bezier curves.
//!
//! The domain is fixed at construction time. The parameter partition is refreshed
//! on each query so changes to the referenced curve are observed.
class ExtremaPC2d_BezierCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with Bezier curve (uses full curve domain).
  //! Parameter partition is built on demand by Perform().
  //! @param[in] theCurve Bezier curve handle
  Standard_EXPORT explicit ExtremaPC2d_BezierCurve(const occ::handle<Geom2d_BezierCurve>& theCurve);

  //! Constructor with Bezier curve and parameter domain.
  //! Parameter partition is built on demand by Perform().
  //! @param[in] theCurve Bezier curve handle
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPC2d_BezierCurve(const occ::handle<Geom2d_BezierCurve>& theCurve,
                                          const ExtremaPC2d::Domain1D&           theDomain);

  //! Copy constructor is deleted.
  ExtremaPC2d_BezierCurve(const ExtremaPC2d_BezierCurve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC2d_BezierCurve& operator=(const ExtremaPC2d_BezierCurve&) = delete;

  //! Move constructor.
  ExtremaPC2d_BezierCurve(ExtremaPC2d_BezierCurve&&) = default;

  //! Move assignment operator.
  ExtremaPC2d_BezierCurve& operator=(ExtremaPC2d_BezierCurve&&) = default;

  //! Evaluates point on curve at parameter.
  //! @param theU parameter
  //! @return point on curve
  Standard_EXPORT gp_Pnt2d Value(double theU) const;

  //! Returns true if domain is bounded (subset of curve domain).
  bool IsBounded() const { return true; } // Bezier curves are always bounded

  //! Returns the domain.
  const ExtremaPC2d::Domain1D& Domain() const { return myDomain; }

  //! Compute extrema between point P and the curve.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC2d::Result& Perform(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const;

  //! Compute extrema between point P and the curve including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC2d::Result& PerformWithEndpoints(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const;

  //! Returns the Bezier curve.
  const occ::handle<Geom2d_BezierCurve>& Curve() const { return myCurve; }

private:
  //! Build parameter partition for the curve.
  void buildParams() const;

  occ::handle<Geom2d_BezierCurve> myCurve;     //!< Bezier curve
  ExtremaPC2d::Domain1D           myDomain;    //!< Parameter domain (fixed)
  mutable size_t                  myNbSamples; //!< Number of samples

  // Numerical evaluator with cached parameter partition and result
  mutable ExtremaPC2d_GridEvaluator myEvaluator;
};

#endif // _ExtremaPC2d_BezierCurve_HeaderFile

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

#ifndef _ExtremaPC2d_OtherCurve_HeaderFile
#define _ExtremaPC2d_OtherCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <ExtremaPC2d.hxx>
#include <ExtremaPC2d_GridEvaluator.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point-Curve extrema computation for general curves using numerical root isolation.
//!
//! Computes roots of the normalized point-curve stationarity function with a
//! derivative-aware multiple-root solver.
//!
//! The parameter partition is refreshed for each query so referenced-adaptor mutations are seen.
//!
//! The algorithm:
//! 1. Build a uniform parameter partition
//! 2. Isolate all roots of the stationarity function, including tangential roots
//! 3. Classify roots from the stationarity sign on both sides
//!
//! This is a fallback implementation that works with any curve type
//! through the Adaptor2d_Curve2d interface.
//!
//! The domain is fixed at construction time and the adaptor must remain valid.
class ExtremaPC2d_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve adaptor (uses full curve domain).
  //! Parameter partition is built on demand by Perform().
  //! @param[in] theCurve curve adaptor (must remain valid)
  Standard_EXPORT explicit ExtremaPC2d_OtherCurve(const Adaptor2d_Curve2d& theCurve);

  //! Constructor with curve adaptor and parameter domain.
  //! Parameter partition is built on demand by Perform().
  //! @param[in] theCurve curve adaptor (must remain valid)
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPC2d_OtherCurve(const Adaptor2d_Curve2d&     theCurve,
                                         const ExtremaPC2d::Domain1D& theDomain);

  //! Copy constructor is deleted.
  ExtremaPC2d_OtherCurve(const ExtremaPC2d_OtherCurve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC2d_OtherCurve& operator=(const ExtremaPC2d_OtherCurve&) = delete;

  //! Move constructor.
  ExtremaPC2d_OtherCurve(ExtremaPC2d_OtherCurve&&) = default;

  //! Move assignment operator.
  ExtremaPC2d_OtherCurve& operator=(ExtremaPC2d_OtherCurve&&) = default;

  //! Evaluates point on curve at parameter.
  //! @param theU parameter
  //! @return point on curve
  Standard_EXPORT gp_Pnt2d Value(double theU) const;

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.IsFinite(); }

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

private:
  //! Build parameter partition for the current adaptor state.
  void buildParams() const;

  const Adaptor2d_Curve2d* myCurve;  //!< Curve adaptor (not owned)
  ExtremaPC2d::Domain1D    myDomain; //!< Parameter domain (fixed)

  //! Numerical evaluator with cached parameter partition and result.
  mutable ExtremaPC2d_GridEvaluator myEvaluator;
};

#endif // _ExtremaPC2d_OtherCurve_HeaderFile

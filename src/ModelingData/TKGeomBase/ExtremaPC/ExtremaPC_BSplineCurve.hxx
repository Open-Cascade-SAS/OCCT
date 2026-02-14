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

#ifndef _ExtremaPC_BSplineCurve_HeaderFile
#define _ExtremaPC_BSplineCurve_HeaderFile

#include <ExtremaPC.hxx>
#include <ExtremaPC_GridEvaluator.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <math_Vector.hxx>

//! @brief Point-BSplineCurve extrema computation using grid-based approach.
//!
//! Computes the extrema between a 3D point and a BSpline curve using
//! a grid-based approach with Newton refinement.
//!
//! The grid is cached for efficiency when performing multiple queries
//! with the same parameter domain.
//!
//! The algorithm:
//! 1. Build knot-aware grid with 2*(degree + 1) samples per knot span using GeomGridEval
//! 2. Linear scan of grid to find candidate intervals (sign changes in F(u))
//! 3. Newton refinement on each candidate interval
//!
//! This approach is simpler and more stable than BVH-based methods,
//! with comparable accuracy for typical BSpline curves.
//!
//! The domain is fixed at construction time and the grid is built eagerly
//! for optimal performance with multiple queries.
class ExtremaPC_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with BSpline curve (uses full curve domain).
  //! Grid is built eagerly at construction time.
  //! @param[in] theCurve BSpline curve handle
  Standard_EXPORT explicit ExtremaPC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve);

  //! Constructor with BSpline curve and parameter domain.
  //! Grid is built eagerly at construction time for the specified domain.
  //! @param[in] theCurve BSpline curve handle
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve,
                                         const ExtremaPC::Domain1D&            theDomain);

  //! Copy constructor is deleted.
  ExtremaPC_BSplineCurve(const ExtremaPC_BSplineCurve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_BSplineCurve& operator=(const ExtremaPC_BSplineCurve&) = delete;

  //! Move constructor.
  ExtremaPC_BSplineCurve(ExtremaPC_BSplineCurve&&) = default;

  //! Move assignment operator.
  ExtremaPC_BSplineCurve& operator=(ExtremaPC_BSplineCurve&&) = default;

  //! Evaluates point on curve at parameter.
  //! @param theU parameter
  //! @return point on curve
  Standard_EXPORT gp_Pnt Value(double theU) const;

  //! Returns true if domain is bounded (subset of curve domain).
  bool IsBounded() const { return true; } // BSpline curves are always bounded

  //! Returns the domain.
  const ExtremaPC::Domain1D& Domain() const { return myDomain; }

  //! Compute extrema between point P and the curve.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const;

  //! Compute extrema between point P and the curve including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC::Result& PerformWithEndpoints(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const;

  //! Returns the BSpline curve.
  const occ::handle<Geom_BSplineCurve>& Curve() const { return myCurve; }

private:
  //! Build knot-aware parameter array for grid sampling.
  //! Places samples at knots and (degree + 1) intermediate points per span.
  //! @return array of parameter values for grid sampling
  math_Vector buildKnotAwareParams() const;

  //! Build grid for the curve.
  void buildGrid();

  occ::handle<Geom_BSplineCurve> myCurve;   //!< BSpline curve
  GeomAdaptor_Curve              myAdaptor; //!< Curve adaptor
  ExtremaPC::Domain1D            myDomain;  //!< Parameter domain (fixed)

  // Grid evaluator with cached state (grid, result, temporary vectors)
  mutable ExtremaPC_GridEvaluator myEvaluator;
};

#endif // _ExtremaPC_BSplineCurve_HeaderFile

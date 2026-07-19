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

#ifndef _ExtremaPC_OtherCurve_HeaderFile
#define _ExtremaPC_OtherCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_GridEvaluator.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point-Curve extrema computation for general curves using grid-based approach.
//!
//! Computes the extrema between a 3D point and a general curve using
//! a grid-based approach with Newton refinement.
//!
//! The grid is cached for efficiency when performing multiple queries
//! with the same parameter domain.
//!
//! The algorithm:
//! 1. Build uniform grid using GeomGridEval
//! 2. Linear scan of grid to find candidate intervals (sign changes in F(u))
//! 3. Newton refinement on each candidate interval
//!
//! This is a fallback implementation that works with any curve type
//! through the Adaptor3d_Curve interface.
//!
//! The domain is fixed at construction time and the grid is built eagerly
//! for optimal performance with multiple queries.
class ExtremaPC_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve adaptor (uses full curve domain).
  //! Grid is built eagerly at construction time.
  //! @param[in] theCurve curve adaptor (must remain valid)
  Standard_EXPORT explicit ExtremaPC_OtherCurve(const Adaptor3d_Curve& theCurve);

  //! Constructor with curve adaptor and parameter domain.
  //! Grid is built eagerly at construction time for the specified domain.
  //! @param[in] theCurve curve adaptor (must remain valid)
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPC_OtherCurve(const Adaptor3d_Curve&     theCurve,
                                       const ExtremaPC::Domain1D& theDomain);

  //! Copy constructor is deleted.
  ExtremaPC_OtherCurve(const ExtremaPC_OtherCurve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_OtherCurve& operator=(const ExtremaPC_OtherCurve&) = delete;

  //! Move constructor.
  ExtremaPC_OtherCurve(ExtremaPC_OtherCurve&&) = default;

  //! Move assignment operator.
  ExtremaPC_OtherCurve& operator=(ExtremaPC_OtherCurve&&) = default;

  //! Evaluates point on curve at parameter.
  //! @param theU parameter
  //! @return point on curve
  Standard_EXPORT gp_Pnt Value(double theU) const;

  //! Returns true if domain is bounded.
  bool IsBounded() const { return true; }

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

private:
  //! Build grid for the curve.
  void buildGrid();

  const Adaptor3d_Curve* myCurve;  //!< Curve adaptor (not owned)
  ExtremaPC::Domain1D    myDomain; //!< Parameter domain (fixed)

  //! Grid evaluator with cached state (grid, result, temporary vectors).
  mutable ExtremaPC_GridEvaluator myEvaluator;
};

#endif // _ExtremaPC_OtherCurve_HeaderFile

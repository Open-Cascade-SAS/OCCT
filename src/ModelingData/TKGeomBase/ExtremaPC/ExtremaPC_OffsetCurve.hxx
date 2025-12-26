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

#ifndef _ExtremaPC_OffsetCurve_HeaderFile
#define _ExtremaPC_OffsetCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_GridEvaluator.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point-OffsetCurve extrema computation using grid-based approach.
//!
//! Computes the extrema between a 3D point and an offset curve using
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
//! Offset curves are handled through the Adaptor3d_Curve interface,
//! which provides uniform access to the offset geometry.
class ExtremaPC_OffsetCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve adaptor.
  //! @param[in] theCurve curve adaptor for offset curve
  Standard_EXPORT explicit ExtremaPC_OffsetCurve(const Adaptor3d_Curve& theCurve);

  //! Copy constructor is deleted.
  ExtremaPC_OffsetCurve(const ExtremaPC_OffsetCurve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_OffsetCurve& operator=(const ExtremaPC_OffsetCurve&) = delete;

  //! Move constructor.
  ExtremaPC_OffsetCurve(ExtremaPC_OffsetCurve&&) = default;

  //! Move assignment operator.
  ExtremaPC_OffsetCurve& operator=(ExtremaPC_OffsetCurve&&) = default;

  //! Evaluates point on curve at parameter.
  //! @param theU parameter
  //! @return point on curve
  Standard_EXPORT gp_Pnt Value(double theU) const;

  //! Compute extrema between point P and the full curve (no bounds checking).
  //! Uses the curve's natural parameter bounds [FirstParameter, LastParameter].
  //! @param theP query point
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing extrema
  Standard_EXPORT ExtremaPC::Result Perform(const gp_Pnt&         theP,
                                            double                theTol,
                                            ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const;

  //! Compute extrema between point P and the curve segment (with bounds checking).
  //! The grid is cached for the parameter range - if the same range is used,
  //! subsequent calls reuse the cached grid for better performance.
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior extrema
  Standard_EXPORT ExtremaPC::Result Perform(const gp_Pnt&              theP,
                                            const ExtremaPC::Domain1D& theDomain,
                                            double                     theTol,
                                            ExtremaPC::SearchMode      theMode = ExtremaPC::SearchMode::MinMax) const;

  //! Compute extrema between point P and the curve segment including endpoints.
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance for root finding
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior + endpoint extrema
  Standard_EXPORT ExtremaPC::Result PerformWithEndpoints(const gp_Pnt&              theP,
                                                         const ExtremaPC::Domain1D& theDomain,
                                                         double                     theTol,
                                                         ExtremaPC::SearchMode      theMode = ExtremaPC::SearchMode::MinMax) const;

private:
  //! Core algorithm - finds extrema with bounds checking.
  ExtremaPC::Result performBounded(const gp_Pnt&              theP,
                                   const ExtremaPC::Domain1D& theDomain,
                                   double                     theTol,
                                   ExtremaPC::SearchMode      theMode) const;

  //! Rebuild cached grid if parameter range has changed.
  void updateCacheIfNeeded(const ExtremaPC::Domain1D& theDomain) const;

  const Adaptor3d_Curve* myCurve; //!< Curve adaptor (not owned)

  // Cached grid data (mutable for const Perform method)
  mutable NCollection_Array1<ExtremaPC_GridEvaluator::GridPoint> myGrid;
  mutable ExtremaPC::Domain1D myCachedDomain;  //!< Cached parameter domain
};

#endif // _ExtremaPC_OffsetCurve_HeaderFile

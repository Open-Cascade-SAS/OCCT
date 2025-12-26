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
class ExtremaPC_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with BSpline curve.
  //! @param theCurve BSpline curve handle
  Standard_EXPORT explicit ExtremaPC_BSplineCurve(const Handle(Geom_BSplineCurve)& theCurve);

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

  //! Returns the BSpline curve.
  const Handle(Geom_BSplineCurve)& Curve() const { return myCurve; }

private:
  //! Core algorithm - finds extrema with bounds checking.
  ExtremaPC::Result performBounded(const gp_Pnt&              theP,
                                   const ExtremaPC::Domain1D& theDomain,
                                   double                     theTol,
                                   ExtremaPC::SearchMode      theMode) const;

  //! Build knot-aware parameter array for grid sampling.
  //! Places samples at knots and (degree + 1) intermediate points per span.
  //! @param theUMin lower parameter bound
  //! @param theUMax upper parameter bound
  //! @return array of parameter values for grid sampling
  math_Vector buildKnotAwareParams(double theUMin, double theUMax) const;

  //! Rebuild cached grid if parameter range has changed.
  void updateCacheIfNeeded(const ExtremaPC::Domain1D& theDomain) const;

  Handle(Geom_BSplineCurve) myCurve;    //!< BSpline curve
  GeomAdaptor_Curve         myAdaptor;  //!< Curve adaptor (cached)

  // Cached grid data (mutable for const Perform method)
  mutable NCollection_Array1<ExtremaPC_GridEvaluator::GridPoint> myGrid;
  mutable ExtremaPC::Domain1D myCachedDomain;  //!< Cached parameter domain
};

#endif // _ExtremaPC_BSplineCurve_HeaderFile

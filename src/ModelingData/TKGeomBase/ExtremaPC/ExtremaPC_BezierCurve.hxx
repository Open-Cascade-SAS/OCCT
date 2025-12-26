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

#ifndef _ExtremaPC_BezierCurve_HeaderFile
#define _ExtremaPC_BezierCurve_HeaderFile

#include <ExtremaPC.hxx>
#include <ExtremaPC_GridEvaluator.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! @brief Point-BezierCurve extrema computation using grid-based approach.
//!
//! Computes the extrema between a 3D point and a Bezier curve using
//! a grid-based approach with Newton refinement.
//!
//! The grid is cached for efficiency when performing multiple queries
//! with the same parameter domain.
//!
//! The algorithm:
//! 1. Build grid with (3 * (degree + 1)) samples using GeomGridEval
//! 2. Linear scan of grid to find candidate intervals (sign changes in F(u))
//! 3. Newton refinement on each candidate interval
//!
//! This approach is simpler and more stable than BVH-based methods,
//! with comparable accuracy for typical Bezier curves.
//!
//! The domain is fixed at construction time and the grid is built eagerly
//! for optimal performance with multiple queries.
class ExtremaPC_BezierCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with Bezier curve (uses full curve domain).
  //! Grid is built eagerly at construction time.
  //! @param[in] theCurve Bezier curve handle
  Standard_EXPORT explicit ExtremaPC_BezierCurve(const Handle(Geom_BezierCurve)& theCurve);

  //! Constructor with Bezier curve and parameter domain.
  //! Grid is built eagerly at construction time for the specified domain.
  //! @param[in] theCurve Bezier curve handle
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPC_BezierCurve(const Handle(Geom_BezierCurve)& theCurve,
                                        const ExtremaPC::Domain1D&      theDomain);

  //! Copy constructor is deleted.
  ExtremaPC_BezierCurve(const ExtremaPC_BezierCurve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_BezierCurve& operator=(const ExtremaPC_BezierCurve&) = delete;

  //! Move constructor.
  ExtremaPC_BezierCurve(ExtremaPC_BezierCurve&&) = default;

  //! Move assignment operator.
  ExtremaPC_BezierCurve& operator=(ExtremaPC_BezierCurve&&) = default;

  //! Evaluates point on curve at parameter.
  //! @param theU parameter
  //! @return point on curve
  Standard_EXPORT gp_Pnt Value(double theU) const;

  //! Returns true if domain is bounded (subset of curve domain).
  bool IsBounded() const { return true; } // Bezier curves are always bounded

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

  //! Returns the Bezier curve.
  const Handle(Geom_BezierCurve)& Curve() const { return myCurve; }

private:
  //! Build grid for the curve.
  void buildGrid();

  Handle(Geom_BezierCurve) myCurve;     //!< Bezier curve
  GeomAdaptor_Curve        myAdaptor;   //!< Curve adaptor
  ExtremaPC::Domain1D      myDomain;    //!< Parameter domain (fixed)
  int                      myNbSamples; //!< Number of samples

  // Grid evaluator with cached state (grid, result, temporary vectors)
  mutable ExtremaPC_GridEvaluator myEvaluator;
};

#endif // _ExtremaPC_BezierCurve_HeaderFile

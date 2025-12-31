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

#ifndef _ExtremaPS_SurfaceOfRevolution_HeaderFile
#define _ExtremaPS_SurfaceOfRevolution_HeaderFile

#include <ExtremaPC_Curve.hxx>
#include <ExtremaPS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Optimized Point-SurfaceOfRevolution extrema computation.
//!
//! Exploits the rotational symmetry to reduce the 2D surface search
//! to a 1D curve search along the meridian (basis curve).
//!
//! Algorithm:
//! 1. For a query point P, compute the U parameter (rotation angle)
//!    analytically from the radial projection onto the rotation plane.
//! 2. Project P onto the meridian plane (containing the axis) to get P'.
//! 3. Find extrema between P' and the meridian curve - a 1D problem.
//! 4. Combine the analytical U with the V from curve extrema.
//!
//! This approach is significantly faster than the generic 2D grid search
//! (typically 10-30x speedup) because:
//! - U is computed analytically (no search needed)
//! - Only 1D curve extrema search is required for V
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes boundary extrema for bounded domains
//!
//! @note For a point on the rotation axis, there are infinite solutions
//!       at all U values for the same V - returns InfiniteSolutions status.
class ExtremaPS_SurfaceOfRevolution
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with surface geometry (uses natural domain from basis curve).
  //! @param[in] theSurface the surface of revolution
  Standard_EXPORT explicit ExtremaPS_SurfaceOfRevolution(
    const occ::handle<Geom_SurfaceOfRevolution>& theSurface);

  //! Constructor with surface geometry and parameter domain.
  //! @param[in] theSurface the surface of revolution
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPS_SurfaceOfRevolution(
    const occ::handle<Geom_SurfaceOfRevolution>& theSurface,
    const ExtremaPS::Domain2D&                   theDomain);

  //! Copy constructor is deleted (contains non-copyable ExtremaPC_Curve).
  ExtremaPS_SurfaceOfRevolution(const ExtremaPS_SurfaceOfRevolution&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPS_SurfaceOfRevolution& operator=(const ExtremaPS_SurfaceOfRevolution&) = delete;

  //! Move constructor.
  ExtremaPS_SurfaceOfRevolution(ExtremaPS_SurfaceOfRevolution&&) = default;

  //! Move assignment operator.
  ExtremaPS_SurfaceOfRevolution& operator=(ExtremaPS_SurfaceOfRevolution&&) = default;

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! @param theU U parameter (rotation angle)
  //! @param theV V parameter (along meridian curve)
  //! @return point on surface at (U, V)
  gp_Pnt Value(double theU, double theV) const;

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //! Uses domain specified at construction time.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] Standard_EXPORT const ExtremaPS::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const;

  //! Find extrema including boundary edges and corners.
  //! Uses domain specified at construction time.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPS::Result& PerformWithBoundary(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const;

  //! @}

  //! Returns the surface geometry.
  const occ::handle<Geom_SurfaceOfRevolution>& Surface() const { return mySurface; }

  //! Returns true if domain is bounded (not natural domain).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  //! Initialize cached components from surface geometry.
  void initCache();

  occ::handle<Geom_SurfaceOfRevolution> mySurface;      //!< Surface geometry
  std::optional<ExtremaPS::Domain2D>    myDomain;       //!< Parameter domain
  mutable ExtremaPS::Result             myResult;       //!< Reusable result storage
  GeomAdaptor_Surface                   myAdaptor;      //!< Surface adaptor for Value()
  ExtremaPC_Curve                       myCurveExtrema; //!< Curve extrema evaluator

  // Cached axis components for fast computation
  double myLocX, myLocY, myLocZ;    //!< Axis location
  double myAxisX, myAxisY, myAxisZ; //!< Axis direction
  double myXDirX, myXDirY, myXDirZ; //!< X direction (perpendicular to axis)
  double myYDirX, myYDirY, myYDirZ; //!< Y direction (perpendicular to axis and X)
};

#endif // _ExtremaPS_SurfaceOfRevolution_HeaderFile

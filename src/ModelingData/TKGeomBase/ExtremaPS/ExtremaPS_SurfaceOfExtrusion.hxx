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

#ifndef _ExtremaPS_SurfaceOfExtrusion_HeaderFile
#define _ExtremaPS_SurfaceOfExtrusion_HeaderFile

#include <ExtremaPC_Curve.hxx>
#include <ExtremaPS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Optimized Point-SurfaceOfExtrusion extrema computation.
//!
//! Exploits the linear extrusion structure to reduce the 2D surface search
//! to a 1D curve search along the basis curve.
//!
//! Algorithm:
//! 1. For a query point P, project it onto the extrusion direction to find
//!    the plane containing the closest point. This gives V analytically.
//! 2. Find extrema between P and the basis curve - a 1D problem.
//! 3. The surface point is at (U, V) where U is from curve extrema.
//!
//! This approach is significantly faster than the generic 2D grid search
//! (typically 10-30x speedup) because:
//! - V is computed analytically (no search needed)
//! - Only 1D curve extrema search is required for U
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes boundary extrema for bounded domains
class ExtremaPS_SurfaceOfExtrusion
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with surface geometry (uses natural domain from basis curve).
  //! @param[in] theSurface the surface of linear extrusion
  Standard_EXPORT explicit ExtremaPS_SurfaceOfExtrusion(
    const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurface);

  //! Constructor with surface geometry and parameter domain.
  //! @param[in] theSurface the surface of linear extrusion
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPS_SurfaceOfExtrusion(
    const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurface,
    const ExtremaPS::Domain2D&                        theDomain);

  //! Copy constructor is deleted (contains non-copyable ExtremaPC_Curve).
  ExtremaPS_SurfaceOfExtrusion(const ExtremaPS_SurfaceOfExtrusion&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPS_SurfaceOfExtrusion& operator=(const ExtremaPS_SurfaceOfExtrusion&) = delete;

  //! Move constructor.
  ExtremaPS_SurfaceOfExtrusion(ExtremaPS_SurfaceOfExtrusion&&) = default;

  //! Move assignment operator.
  ExtremaPS_SurfaceOfExtrusion& operator=(ExtremaPS_SurfaceOfExtrusion&&) = default;

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! @param theU U parameter (along basis curve)
  //! @param theV V parameter (along extrusion direction)
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
  const occ::handle<Geom_SurfaceOfLinearExtrusion>& Surface() const { return mySurface; }

  //! Returns true if domain is bounded (not natural domain).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  //! Initialize cached components from surface geometry.
  void initCache();

  //! Sample point data for curve sampling.
  struct Sample
  {
    double U;      //!< Curve parameter
    double V;      //!< Optimal V parameter
    double SqDist; //!< Squared distance to query point
  };

  occ::handle<Geom_SurfaceOfLinearExtrusion> mySurface;      //!< Surface geometry
  std::optional<ExtremaPS::Domain2D>         myDomain;       //!< Parameter domain
  mutable ExtremaPS::Result                  myResult;       //!< Reusable result storage
  GeomAdaptor_Surface                        myAdaptor;      //!< Surface adaptor for Value()
  ExtremaPC_Curve                            myCurveExtrema; //!< Curve extrema evaluator

  // Cached extrusion direction for fast computation
  double myDirX, myDirY, myDirZ; //!< Extrusion direction (normalized)

  // Reusable sample vectors (avoid allocation per query)
  mutable NCollection_Vector<Sample> mySamples;    //!< Cached sample points
  mutable NCollection_Vector<Sample> myCandidates; //!< Cached candidate extrema
};

#endif // _ExtremaPS_SurfaceOfExtrusion_HeaderFile

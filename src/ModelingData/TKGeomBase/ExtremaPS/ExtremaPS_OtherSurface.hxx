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

#ifndef _ExtremaPS_OtherSurface_HeaderFile
#define _ExtremaPS_OtherSurface_HeaderFile

#include <ExtremaPS.hxx>
#include <ExtremaPS_GridEvaluator.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_OtherSurface.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point-Surface extrema computation for general surfaces.
//!
//! Uses grid-based algorithm with GeomGridEval_OtherSurface for
//! surfaces that don't have specialized evaluators, followed by Newton refinement.
//!
//! The domain is fixed at construction time and the grid is built eagerly
//! for optimal performance with multiple queries.
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
class ExtremaPS_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from surface handle (uses surface bounds from adaptor).
  //! Grid is built eagerly at construction time.
  //! @param[in] theSurface surface handle
  Standard_EXPORT explicit ExtremaPS_OtherSurface(const occ::handle<Geom_Surface>& theSurface);

  //! Constructor with surface and parameter domain.
  //! Grid is built eagerly at construction time for the specified domain.
  //! @param[in] theSurface surface handle
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPS_OtherSurface(const occ::handle<Geom_Surface>& theSurface,
                                         const ExtremaPS::Domain2D&  theDomain);

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter
  //! @param theV V parameter
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

  //! Returns the surface handle.
  const occ::handle<Geom_Surface>& Surface() const { return mySurface; }

  //! Returns the parameter domain.
  const ExtremaPS::Domain2D& Domain() const { return myDomain; }

private:
  //! Build the grid for the current domain.
  void buildGrid();

private:
  occ::handle<Geom_Surface>  mySurface;  //!< Surface geometry
  GeomAdaptor_Surface   myAdaptor;  //!< Surface adaptor (cached)
  ExtremaPS::Domain2D   myDomain;   //!< Parameter domain (fixed at construction)

  // Grid evaluator with cached state (grid, result, temporary vectors)
  mutable ExtremaPS_GridEvaluator myEvaluator;
};

#endif // _ExtremaPS_OtherSurface_HeaderFile

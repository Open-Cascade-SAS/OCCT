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

#ifndef _ExtremaPS_Surface_HeaderFile
#define _ExtremaPS_Surface_HeaderFile

#include <ExtremaPS.hxx>
#include <ExtremaPS_BezierSurface.hxx>
#include <ExtremaPS_BSplineSurface.hxx>
#include <ExtremaPS_Cone.hxx>
#include <ExtremaPS_Cylinder.hxx>
#include <ExtremaPS_OffsetSurface.hxx>
#include <ExtremaPS_OtherSurface.hxx>
#include <ExtremaPS_Plane.hxx>
#include <ExtremaPS_Sphere.hxx>
#include <ExtremaPS_SurfaceOfExtrusion.hxx>
#include <ExtremaPS_SurfaceOfRevolution.hxx>
#include <ExtremaPS_Torus.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>
#include <variant>

//! @brief Main aggregator class for Point-Surface extrema computation.
//!
//! Dispatches to appropriate specialized evaluator based on surface type.
//! Uses std::variant for type-safe polymorphism without virtual overhead.
//!
//! Supported surface types:
//! - Plane (analytical)
//! - Cylinder (analytical with periodic U)
//! - Cone (analytical with apex handling)
//! - Sphere (analytical with pole handling)
//! - Torus (analytical, up to 4 extrema)
//! - BezierSurface (grid-based)
//! - BSplineSurface (knot-aware grid)
//! - OffsetSurface (grid via basis surface)
//! - SurfaceOfRevolution (1D curve search optimization)
//! - SurfaceOfExtrusion (1D curve search optimization)
//! - Other surfaces (general grid fallback)
//!
//! @section API Design
//!
//! The domain is fixed at construction time for optimal performance with multiple queries.
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
class ExtremaPS_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from Adaptor3d_Surface (uses adaptor bounds as domain).
  //! @param[in] theSurface surface adaptor
  Standard_EXPORT ExtremaPS_Surface(const Adaptor3d_Surface& theSurface);

  //! Constructor from Adaptor3d_Surface with specified domain.
  //! @param[in] theSurface surface adaptor
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPS_Surface(const Adaptor3d_Surface&   theSurface,
                                    const ExtremaPS::Domain2D& theDomain);

  //! Constructor from GeomAdaptor_Surface (uses adaptor bounds as domain).
  //! @param[in] theSurface surface adaptor
  Standard_EXPORT ExtremaPS_Surface(const GeomAdaptor_Surface& theSurface);

  //! Constructor from GeomAdaptor_Surface with specified domain.
  //! @param[in] theSurface surface adaptor
  //! @param[in] theDomain parameter domain (fixed for all queries)
  Standard_EXPORT ExtremaPS_Surface(const GeomAdaptor_Surface& theSurface,
                                    const ExtremaPS::Domain2D& theDomain);

  //! Constructor from Geom_Surface.
  //! For non-trimmed surfaces, does NOT set domain (uses natural/unbounded behavior).
  //! For trimmed surfaces, uses the trimmed bounds as domain.
  //! @param[in] theSurface geometric surface handle
  Standard_EXPORT explicit ExtremaPS_Surface(const occ::handle<Geom_Surface>& theSurface);

  //! Constructor from Geom_Surface with specified domain.
  //! For trimmed surfaces, intersects input bounds with trimmed bounds.
  //! @param[in] theSurface geometric surface handle
  //! @param[in] theDomain parameter domain
  Standard_EXPORT ExtremaPS_Surface(const occ::handle<Geom_Surface>& theSurface,
                                    const ExtremaPS::Domain2D&       theDomain);

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //! Uses domain specified at construction time.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, or Max)
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
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPS::Result& PerformWithBoundary(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const;

  //! @}

  //! Returns true if surface evaluator is initialized.
  bool IsInitialized() const;

private:
  //! Variant type for surface evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        ExtremaPS_Plane,
                                        ExtremaPS_Cylinder,
                                        ExtremaPS_Cone,
                                        ExtremaPS_Sphere,
                                        ExtremaPS_Torus,
                                        ExtremaPS_BezierSurface,
                                        ExtremaPS_BSplineSurface,
                                        ExtremaPS_OffsetSurface,
                                        ExtremaPS_SurfaceOfRevolution,
                                        ExtremaPS_SurfaceOfExtrusion,
                                        ExtremaPS_OtherSurface>;

  //! Initialize evaluator from adaptor with specified domain.
  void initializeEvaluator(const Adaptor3d_Surface&   theSurface,
                           const ExtremaPS::Domain2D& theDomain);

  //! Helper method to initialize evaluator from a Geom_Surface.
  //! Handles all surface type detection and evaluator creation.
  //! @param[in] theSurface the surface to initialize from (must not be null)
  //! @param[in] theDomain optional domain to use
  void initFromGeomSurface(const occ::handle<Geom_Surface>&          theSurface,
                           const std::optional<ExtremaPS::Domain2D>& theDomain);

  EvaluatorVariant                 myEvaluator; //!< Specialized evaluator
  occ::handle<GeomAdaptor_Surface> myAdaptor;   //!< Stored adaptor for Geom-based construction
};

#endif // _ExtremaPS_Surface_HeaderFile

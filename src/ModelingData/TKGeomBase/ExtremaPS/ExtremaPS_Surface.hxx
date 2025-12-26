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
#include <ExtremaPS_Torus.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Standard_DefineAlloc.hxx>

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
//! - Other surfaces (general grid fallback)
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
class ExtremaPS_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Standard_EXPORT ExtremaPS_Surface();

  //! Constructor from Adaptor3d_Surface.
  Standard_EXPORT ExtremaPS_Surface(const Adaptor3d_Surface& theSurface);

  //! Constructor from GeomAdaptor_Surface.
  Standard_EXPORT ExtremaPS_Surface(const GeomAdaptor_Surface& theSurface);

  //! Initialize from Adaptor3d_Surface.
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& theSurface);

  //! Initialize from GeomAdaptor_Surface.
  Standard_EXPORT void Initialize(const GeomAdaptor_Surface& theSurface);

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only, using full parameter range from adaptor.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @return result with interior extrema only
  Standard_EXPORT ExtremaPS::Result Perform(const gp_Pnt& theP, double theTol) const;

  //! Find interior extrema only within specified parameter bounds.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain [UMin,UMax] x [VMin,VMax]
  //! @param theTol tolerance
  //! @return result with interior extrema only
  Standard_EXPORT ExtremaPS::Result Perform(const gp_Pnt&              theP,
                                             const ExtremaPS::Domain2D& theDomain,
                                             double                     theTol) const;

  //! Find extrema including boundary, using full parameter range.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @return result with interior + boundary extrema
  Standard_EXPORT ExtremaPS::Result PerformWithBoundary(const gp_Pnt& theP, double theTol) const;

  //! Find extrema including boundary within specified parameter bounds.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  //! @param theTol tolerance
  //! @return result with interior + boundary extrema
  Standard_EXPORT ExtremaPS::Result PerformWithBoundary(const gp_Pnt&              theP,
                                                         const ExtremaPS::Domain2D& theDomain,
                                                         double                     theTol) const;

  //! @}

  //! Sets the search mode.
  void SetSearchMode(ExtremaPS::SearchMode theMode) { mySearchMode = theMode; }

  //! Returns current search mode.
  ExtremaPS::SearchMode SearchMode() const { return mySearchMode; }

  //! Returns true if surface evaluator is initialized.
  bool IsInitialized() const;

  //! Returns the default parameter domain.
  ExtremaPS::Domain2D DefaultDomain() const { return myDomain; }

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
                                        ExtremaPS_OtherSurface>;

  //! Initialize evaluator from adaptor.
  void initializeEvaluator(const Adaptor3d_Surface& theSurface);

  EvaluatorVariant        myEvaluator;
  ExtremaPS::Domain2D     myDomain;        //!< Default parameter domain from adaptor
  ExtremaPS::SearchMode   mySearchMode = ExtremaPS::SearchMode::MinMax;
};

#endif // _ExtremaPS_Surface_HeaderFile

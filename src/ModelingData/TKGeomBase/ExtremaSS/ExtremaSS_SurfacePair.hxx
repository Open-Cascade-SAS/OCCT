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

#ifndef _ExtremaSS_SurfacePair_HeaderFile
#define _ExtremaSS_SurfacePair_HeaderFile

#include <ExtremaSS.hxx>
#include <ExtremaSS_ConeCone.hxx>
#include <ExtremaSS_ConeSphere.hxx>
#include <ExtremaSS_ConeTorus.hxx>
#include <ExtremaSS_CylinderCone.hxx>
#include <ExtremaSS_CylinderCylinder.hxx>
#include <ExtremaSS_CylinderSphere.hxx>
#include <ExtremaSS_CylinderTorus.hxx>
#include <ExtremaSS_GenericPair.hxx>
#include <ExtremaSS_PlaneCone.hxx>
#include <ExtremaSS_PlaneCylinder.hxx>
#include <ExtremaSS_PlanePlane.hxx>
#include <ExtremaSS_PlaneSphere.hxx>
#include <ExtremaSS_PlaneTorus.hxx>
#include <ExtremaSS_SphereSphere.hxx>
#include <ExtremaSS_SphereTorus.hxx>
#include <ExtremaSS_TorusTorus.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>
#include <variant>

//! @brief Main aggregator class for Surface-Surface extrema computation.
//!
//! Dispatches to appropriate specialized evaluator based on surface types.
//! Uses std::variant for type-safe polymorphism without virtual overhead.
//!
//! Supported surface pairs:
//! - Plane-Plane, Plane-Cylinder, Plane-Cone, Plane-Sphere, Plane-Torus
//! - Cylinder-Cylinder, Cylinder-Cone, Cylinder-Sphere, Cylinder-Torus
//! - Cone-Cone, Cone-Sphere, Cone-Torus
//! - Sphere-Sphere, Sphere-Torus
//! - Torus-Torus
//! - Generic fallback for other surface types (BSpline, Bezier, Offset, etc.)
class ExtremaSS_SurfacePair
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type for surface pair evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        ExtremaSS_PlanePlane,
                                        ExtremaSS_PlaneCylinder,
                                        ExtremaSS_PlaneCone,
                                        ExtremaSS_PlaneSphere,
                                        ExtremaSS_PlaneTorus,
                                        ExtremaSS_CylinderCylinder,
                                        ExtremaSS_CylinderCone,
                                        ExtremaSS_CylinderSphere,
                                        ExtremaSS_CylinderTorus,
                                        ExtremaSS_ConeCone,
                                        ExtremaSS_ConeSphere,
                                        ExtremaSS_ConeTorus,
                                        ExtremaSS_SphereSphere,
                                        ExtremaSS_SphereTorus,
                                        ExtremaSS_TorusTorus,
                                        ExtremaSS_GenericPair>;

  //! Constructor from two GeomAdaptor_Surface objects.
  //! @param[in] theSurface1 first surface
  //! @param[in] theSurface2 second surface
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_Surface& theSurface1,
                                        const GeomAdaptor_Surface& theSurface2);

  //! Constructor with explicit domains.
  //! @param[in] theSurface1 first surface
  //! @param[in] theSurface2 second surface
  //! @param[in] theDomain parameter domains for both surfaces
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_Surface& theSurface1,
                                        const GeomAdaptor_Surface& theSurface2,
                                        const ExtremaSS::Domain4D& theDomain);

  //! Constructor from GeomAdaptor_Surface and GeomAdaptor_TransformedSurface.
  //! @param[in] theSurface1 first surface
  //! @param[in] theSurface2 second surface (transformed)
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_Surface&            theSurface1,
                                        const GeomAdaptor_TransformedSurface& theSurface2);

  //! Constructor with explicit domains.
  //! @param[in] theSurface1 first surface
  //! @param[in] theSurface2 second surface (transformed)
  //! @param[in] theDomain parameter domains for both surfaces
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_Surface&            theSurface1,
                                        const GeomAdaptor_TransformedSurface& theSurface2,
                                        const ExtremaSS::Domain4D&            theDomain);

  //! Constructor from GeomAdaptor_TransformedSurface and GeomAdaptor_Surface.
  //! @param[in] theSurface1 first surface (transformed)
  //! @param[in] theSurface2 second surface
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                        const GeomAdaptor_Surface&            theSurface2);

  //! Constructor with explicit domains.
  //! @param[in] theSurface1 first surface (transformed)
  //! @param[in] theSurface2 second surface
  //! @param[in] theDomain parameter domains for both surfaces
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                        const GeomAdaptor_Surface&            theSurface2,
                                        const ExtremaSS::Domain4D&            theDomain);

  //! Constructor from two GeomAdaptor_TransformedSurface objects.
  //! @param[in] theSurface1 first surface (transformed)
  //! @param[in] theSurface2 second surface (transformed)
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                        const GeomAdaptor_TransformedSurface& theSurface2);

  //! Constructor with explicit domains.
  //! @param[in] theSurface1 first surface (transformed)
  //! @param[in] theSurface2 second surface (transformed)
  //! @param[in] theDomain parameter domains for both surfaces
  Standard_EXPORT ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                        const GeomAdaptor_TransformedSurface& theSurface2,
                                        const ExtremaSS::Domain4D&            theDomain);

  //! Find interior extrema only.
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] Standard_EXPORT const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const;

  //! Find extrema including boundary edges and corners.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] Standard_EXPORT const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const;

  //! Returns true if evaluator is initialized.
  Standard_EXPORT bool IsInitialized() const;

  //! Returns true if surfaces were swapped internally for canonical ordering.
  bool IsSwapped() const { return mySwapped; }

private:
  //! Initialize evaluator based on surface types.
  void initializeEvaluator(const GeomAdaptor_Surface& theSurface1,
                           const GeomAdaptor_Surface& theSurface2,
                           const ExtremaSS::Domain4D& theDomain);

  EvaluatorVariant myEvaluator;
  bool             mySwapped = false; //!< True if surfaces were swapped for canonical ordering
};

#endif // _ExtremaSS_SurfacePair_HeaderFile

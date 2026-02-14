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

#ifndef _ExtremaCS_CurveSurface_HeaderFile
#define _ExtremaCS_CurveSurface_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <ExtremaCS.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

// Forward declarations for analytical pairs
class ExtremaCS_LinePlane;
class ExtremaCS_LineSphere;
class ExtremaCS_LineCylinder;
class ExtremaCS_LineCone;
class ExtremaCS_CirclePlane;
class ExtremaCS_CircleSphere;
class ExtremaCS_CircleCylinder;
class ExtremaCS_EllipsePlane;

//! @brief Main aggregator for curve-surface extrema computation.
//!
//! This class provides a uniform interface for computing extrema between
//! any 3D curve and surface, automatically selecting the optimal algorithm
//! based on the curve and surface types.
//!
//! For elementary curve-surface pairs (Line-Plane, Line-Sphere, etc.),
//! specialized analytical algorithms are used when available.
//!
//! For all other pairs (involving Bezier, BSpline, Offset, or other curves/surfaces),
//! grid-based numerical methods with Newton refinement are used.
//!
//! Usage:
//! @code
//!   GeomAdaptor_Curve aCurve(aGeomCurve);
//!   GeomAdaptor_Surface aSurface(aGeomSurface);
//!   ExtremaCS_CurveSurface anExtrema(aCurve, aSurface);
//!   const auto& aResult = anExtrema.Perform(1e-7);
//!   if (aResult.IsDone())
//!   {
//!     for (int i = 0; i < aResult.NbExtrema(); ++i)
//!     {
//!       const auto& anExt = aResult.Extremum(i);
//!       // Use anExt.PointOnCurve, anExt.PointOnSurface, anExt.SquareDistance, etc.
//!     }
//!   }
//! @endcode
class ExtremaCS_CurveSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve and surface (uses full parameter domains).
  //! @param[in] theCurve the curve
  //! @param[in] theSurface the surface
  Standard_EXPORT ExtremaCS_CurveSurface(const Adaptor3d_Curve&   theCurve,
                                          const Adaptor3d_Surface& theSurface);

  //! Constructor with curve and surface and parameter domains.
  //! @param[in] theCurve the curve
  //! @param[in] theSurface the surface
  //! @param[in] theDomain parameter domains for curve and surface
  Standard_EXPORT ExtremaCS_CurveSurface(const Adaptor3d_Curve&        theCurve,
                                          const Adaptor3d_Surface&      theSurface,
                                          const ExtremaCS::Domain3D&   theDomain);

  //! Destructor.
  Standard_EXPORT ~ExtremaCS_CurveSurface();

  //! Copy constructor is deleted.
  ExtremaCS_CurveSurface(const ExtremaCS_CurveSurface&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCS_CurveSurface& operator=(const ExtremaCS_CurveSurface&) = delete;

  //! Move constructor.
  Standard_EXPORT ExtremaCS_CurveSurface(ExtremaCS_CurveSurface&& theOther) noexcept;

  //! Move assignment operator.
  Standard_EXPORT ExtremaCS_CurveSurface& operator=(ExtremaCS_CurveSurface&& theOther) noexcept;

  //! Compute extrema between the curve and surface.
  //! @param[in] theTol tolerance for root finding
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCS::Result& Perform(
    double                theTol,
    ExtremaCS::SearchMode theMode = ExtremaCS::SearchMode::MinMax) const;

  //! Returns the domain.
  const ExtremaCS::Domain3D& Domain() const { return myDomain; }

private:
  //! Initializes the appropriate pair handler based on curve and surface types.
  void initPair(const Adaptor3d_Curve& theCurve, const Adaptor3d_Surface& theSurface);

  ExtremaCS::Domain3D       myDomain; //!< Parameter domains
  mutable ExtremaCS::Result myResult; //!< Result storage

  //! Implementation pointer (PIMPL idiom to reduce header dependencies).
  struct Impl;
  Impl* myImpl;
};

#endif // _ExtremaCS_CurveSurface_HeaderFile

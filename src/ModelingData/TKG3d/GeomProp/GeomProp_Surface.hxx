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

#ifndef _GeomProp_Surface_HeaderFile
#define _GeomProp_Surface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomProp.hxx>
#include <GeomProp_BezierSurface.hxx>
#include <GeomProp_BSplineSurface.hxx>
#include <GeomProp_Cone.hxx>
#include <GeomProp_Cylinder.hxx>
#include <GeomProp_OffsetSurface.hxx>
#include <GeomProp_OtherSurface.hxx>
#include <GeomProp_Plane.hxx>
#include <GeomProp_Sphere.hxx>
#include <GeomProp_SurfaceOfExtrusion.hxx>
#include <GeomProp_SurfaceOfRevolution.hxx>
#include <GeomProp_Torus.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Unified local differential property evaluator for any 3D surface.
//!
//! Uses std::variant for compile-time type safety and zero heap allocation
//! for the evaluator itself. Automatically detects surface type from
//! Adaptor3d_Surface or Geom_Surface and dispatches to the appropriate
//! specialized evaluator.
//!
//! Supported surface types with optimized evaluation:
//! - Plane: Trivial (constant normal, zero curvatures)
//! - Cylinder: Constant principal curvatures (0 and 1/R)
//! - Cone: Analytical curvatures (vary along meridian)
//! - Sphere: Constant curvature 1/R, umbilic
//! - Torus: Analytical curvatures (vary along meridian)
//! - BezierSurface: Numeric from derivatives
//! - BSplineSurface: Numeric from derivatives
//! - SurfaceOfRevolution: Numeric from derivatives
//! - SurfaceOfExtrusion: Numeric from derivatives
//! - OffsetSurface: Numeric from derivatives
//! - Other: Fallback using adaptor derivatives
//!
//! Usage:
//! @code
//!   GeomProp_Surface aProp(myGeomSurface);
//!   GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.5, Precision::Confusion());
//!   if (aCurv.IsDefined)
//!   {
//!     double aMinK = aCurv.MinCurvature;
//!     double aMaxK = aCurv.MaxCurvature;
//!   }
//! @endcode
class GeomProp_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible 3D surface property evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        GeomProp_Plane,
                                        GeomProp_Cylinder,
                                        GeomProp_Cone,
                                        GeomProp_Sphere,
                                        GeomProp_Torus,
                                        GeomProp_BezierSurface,
                                        GeomProp_BSplineSurface,
                                        GeomProp_SurfaceOfRevolution,
                                        GeomProp_SurfaceOfExtrusion,
                                        GeomProp_OffsetSurface,
                                        GeomProp_OtherSurface>;

  //! Construct from 3D adaptor reference (auto-detects surface type).
  //! For GeomAdaptor_Surface, extracts underlying Geom_Surface for optimized evaluation.
  //! @param[in] theSurface 3D surface adaptor reference
  Standard_EXPORT GeomProp_Surface(const Adaptor3d_Surface& theSurface);

  //! Construct from geometry handle (auto-detects surface type).
  //! @param[in] theSurface 3D geometry to evaluate
  Standard_EXPORT GeomProp_Surface(const occ::handle<Geom_Surface>& theSurface);

  //! Non-copyable and non-movable.
  GeomProp_Surface(const GeomProp_Surface&)            = delete;
  GeomProp_Surface& operator=(const GeomProp_Surface&) = delete;
  GeomProp_Surface(GeomProp_Surface&&)                 = delete;
  GeomProp_Surface& operator=(GeomProp_Surface&&)      = delete;

  //! Returns the detected surface type.
  GeomAbs_SurfaceType GetType() const { return mySurfaceType; }

  //! Compute surface normal at given (U, V) parameter.
  //! @param[in] theU U parameter on the surface
  //! @param[in] theV V parameter on the surface
  //! @param[in] theTol linear tolerance
  //! @return surface normal result with validity flag
  Standard_EXPORT GeomProp::SurfaceNormalResult Normal(double theU,
                                                       double theV,
                                                       double theTol) const;

  //! Compute principal curvatures at given (U, V) parameter.
  //! @param[in] theU U parameter on the surface
  //! @param[in] theV V parameter on the surface
  //! @param[in] theTol linear tolerance
  //! @return curvature result with principal curvatures and directions
  Standard_EXPORT GeomProp::SurfaceCurvatureResult Curvatures(double theU,
                                                              double theV,
                                                              double theTol) const;

  //! Compute mean and Gaussian curvatures at given (U, V) parameter.
  //! @param[in] theU U parameter on the surface
  //! @param[in] theV V parameter on the surface
  //! @param[in] theTol linear tolerance
  //! @return mean and Gaussian curvature result
  Standard_EXPORT GeomProp::MeanGaussianResult MeanGaussian(double theU,
                                                            double theV,
                                                            double theTol) const;

protected:
  //! Initialize from 3D adaptor reference (auto-detects surface type).
  //! @param[in] theSurface 3D surface adaptor reference
  Standard_EXPORT void initialization(const Adaptor3d_Surface& theSurface);

  //! Initialize from geometry handle (auto-detects surface type).
  //! @param[in] theSurface 3D geometry to evaluate
  Standard_EXPORT void initialization(const occ::handle<Geom_Surface>& theSurface);

private:
  //! Initialize from stored adaptor (dispatches to per-geometry evaluator).
  //! Must be called after myAdaptor is set. Per-geometry evaluators receive
  //! a non-owning pointer to myAdaptor; their lifetime is managed by the variant.
  Standard_EXPORT void initFromAdaptor();

  occ::handle<GeomAdaptor_Surface> myAdaptor; //!< Owns the adaptor (ensures lifetime).
  EvaluatorVariant    myEvaluator; //!< Per-geometry evaluator (non-owning pointer to myAdaptor).
  GeomAbs_SurfaceType mySurfaceType;
};

#endif // _GeomProp_Surface_HeaderFile

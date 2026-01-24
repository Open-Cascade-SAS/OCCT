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

#ifndef _GeomGridEval_Surface_HeaderFile
#define _GeomGridEval_Surface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Surface.hxx>
#include <GeomGridEval_BSplineSurface.hxx>
#include <GeomGridEval_BezierSurface.hxx>
#include <GeomGridEval_Cone.hxx>
#include <GeomGridEval_Cylinder.hxx>
#include <GeomGridEval_OffsetSurface.hxx>
#include <GeomGridEval_OtherSurface.hxx>
#include <GeomGridEval_Plane.hxx>
#include <GeomGridEval_Sphere.hxx>
#include <GeomGridEval_SurfaceOfExtrusion.hxx>
#include <GeomGridEval_SurfaceOfRevolution.hxx>
#include <GeomGridEval_Torus.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>
#include <variant>

//! @brief Unified grid evaluator for any 3D surface.
//!
//! Uses std::variant for compile-time type safety and zero heap allocation
//! for the evaluator itself. Automatically detects surface type from
//! Adaptor3d_Surface and dispatches to the appropriate specialized evaluator.
//!
//! Supported surface types with optimized evaluation:
//! - Plane: Direct analytical formula
//! - Cylinder: Analytical formula
//! - Sphere: Trigonometric formula
//! - Cone: Analytical formula
//! - Torus: Analytical formula
//! - BezierSurface: Optimized batch evaluation via BSplSLib
//! - BSplineSurface: Optimized batch evaluation with span-based caching
//! - OffsetSurface: Optimized batch evaluation using basis surface derivatives
//! - SurfaceOfRevolution: Batch evaluation using basis curve
//! - SurfaceOfExtrusion: Batch evaluation using basis curve
//! - Other: Fallback using Adaptor3d_Surface::D0
//!
//! Usage:
//! @code
//!   GeomGridEval_Surface anEval;
//!   anEval.Initialize(myAdaptorSurface);
//!   // OR
//!   anEval.Initialize(myGeomSurface);
//!   // Grid evaluation
//!   NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible surface evaluators.
  using EvaluatorVariant = std::variant<std::monostate,                   // Uninitialized state
                                        GeomGridEval_Plane,               // Plane surface
                                        GeomGridEval_Cylinder,            // Cylindrical surface
                                        GeomGridEval_Sphere,              // Spherical surface
                                        GeomGridEval_Cone,                // Conical surface
                                        GeomGridEval_Torus,               // Toroidal surface
                                        GeomGridEval_BezierSurface,       // Bezier surface
                                        GeomGridEval_BSplineSurface,      // B-spline surface
                                        GeomGridEval_OffsetSurface,       // Offset surface
                                        GeomGridEval_SurfaceOfRevolution, // Surface of revolution
                                        GeomGridEval_SurfaceOfExtrusion,  // Surface of extrusion
                                        GeomGridEval_OtherSurface>; // Fallback for other types

  //! Default constructor - uninitialized state.
  GeomGridEval_Surface()
      : myEvaluator(std::monostate{}),
        mySurfaceType(GeomAbs_OtherSurface)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Surface(const GeomGridEval_Surface&)            = delete;
  GeomGridEval_Surface& operator=(const GeomGridEval_Surface&) = delete;
  GeomGridEval_Surface(GeomGridEval_Surface&&)                 = delete;
  GeomGridEval_Surface& operator=(GeomGridEval_Surface&&)      = delete;

  //! Initialize from adaptor reference (auto-detects surface type).
  //! For GeomAdaptor_Surface, extracts underlying Geom_Surface for optimized evaluation.
  //! For other adaptors, stores reference for fallback evaluation.
  //! @note The surface adaptor reference must remain valid during the lifetime
  //!       of this evaluator when using fallback evaluation.
  //! @param theSurface surface adaptor reference to evaluate
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& theSurface);

  //! Initialize from geometry handle (auto-detects surface type).
  //! @param theSurface geometry to evaluate
  Standard_EXPORT void Initialize(const occ::handle<Geom_Surface>& theSurface);

  //! Returns true if properly initialized.
  Standard_EXPORT bool IsInitialized() const;

  //! Evaluate grid points at all specified parameters.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of 3D points (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first partial derivatives.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with derivatives up to third order.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @param[in] theNU derivative order in U direction
  //! @param[in] theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams,
    int                               theNU,
    int                               theNV) const;

  //! Returns the detected surface type.
  GeomAbs_SurfaceType GetType() const { return mySurfaceType; }

  //! Returns true if a transformation is applied.
  bool HasTransformation() const { return myTrsf.has_value(); }

  //! Returns the transformation (empty if not set).
  const std::optional<gp_Trsf>& GetTransformation() const { return myTrsf; }

private:
  //! Apply transformation to grid of points.
  void applyTransformation(NCollection_Array2<gp_Pnt>& theGrid) const;

  //! Apply transformation to grid of D1 results.
  void applyTransformation(NCollection_Array2<GeomGridEval::SurfD1>& theGrid) const;

  //! Apply transformation to grid of D2 results.
  void applyTransformation(NCollection_Array2<GeomGridEval::SurfD2>& theGrid) const;

  //! Apply transformation to grid of D3 results.
  void applyTransformation(NCollection_Array2<GeomGridEval::SurfD3>& theGrid) const;

  //! Apply transformation to grid of vectors.
  void applyTransformation(NCollection_Array2<gp_Vec>& theGrid) const;

  EvaluatorVariant       myEvaluator;
  GeomAbs_SurfaceType    mySurfaceType;
  std::optional<gp_Trsf> myTrsf; //!< Optional transformation for BRepAdaptor surfaces
};

#endif // _GeomGridEval_Surface_HeaderFile

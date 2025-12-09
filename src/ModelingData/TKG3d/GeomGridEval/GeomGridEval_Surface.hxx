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
#include <GeomGridEval_BSplineSurface.hxx>
#include <GeomGridEval_Cylinder.hxx>
#include <GeomGridEval_OtherSurface.hxx>
#include <GeomGridEval_Plane.hxx>
#include <GeomGridEval_Sphere.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

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
//! - BSplineSurface: Optimized batch evaluation with span-based caching
//! - Other: Fallback using Adaptor3d_Surface::D0
//!
//! Usage:
//! @code
//!   GeomGridEval_Surface anEval;
//!   anEval.Initialize(myAdaptorSurface);
//!   anEval.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid();
//! @endcode
class GeomGridEval_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible surface evaluators.
  using EvaluatorVariant = std::variant<std::monostate,                   // Uninitialized state
                                        GeomGridEval_Plane,          // Plane surface
                                        GeomGridEval_Cylinder,       // Cylindrical surface
                                        GeomGridEval_Sphere,         // Spherical surface
                                        GeomGridEval_BSplineSurface, // B-spline surface
                                        GeomGridEval_OtherSurface>;  // Fallback for other types

  //! Default constructor - uninitialized state.
  GeomGridEval_Surface()
      : myEvaluator(std::monostate{}),
        mySurfaceType(GeomAbs_OtherSurface)
  {
  }

  //! Initialize from adaptor (auto-detects surface type).
  //! @param theSurface surface adaptor to evaluate
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& theSurface);

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns true if properly initialized.
  Standard_EXPORT bool IsInitialized() const;

  //! Returns number of U parameters set.
  Standard_EXPORT int NbUParams() const;

  //! Returns number of V parameters set.
  Standard_EXPORT int NbVParams() const;

  //! Evaluate grid points at all set parameters.
  //! @return 2D array of 3D points (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

  //! Evaluate grid points with first partial derivatives.
  //! @return 2D array of SurfD1 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1() const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @return 2D array of SurfD2 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2() const;

  //! Returns the detected surface type.
  GeomAbs_SurfaceType GetType() const { return mySurfaceType; }

private:
  EvaluatorVariant    myEvaluator;
  GeomAbs_SurfaceType mySurfaceType;
};

#endif // _GeomGridEval_Surface_HeaderFile

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

#ifndef _GeomGridEval_BezierSurface_HeaderFile
#define _GeomGridEval_BezierSurface_HeaderFile

#include <BSplSLib_Cache.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for Bezier surface grid points.
//!
//! Uses BSplSLib_Cache for optimized polynomial evaluation.
//! Bezier surfaces are treated as single-span B-spline surfaces.
//!
//! Usage:
//! @code
//!   GeomGridEval_BezierSurface anEvaluator(myGeomBezier);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_BezierSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theBezier the bezier surface geometry to evaluate
  GeomGridEval_BezierSurface(const occ::handle<Geom_BezierSurface>& theBezier)
      : myGeom(theBezier)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_BezierSurface(const GeomGridEval_BezierSurface&)            = delete;
  GeomGridEval_BezierSurface& operator=(const GeomGridEval_BezierSurface&) = delete;
  GeomGridEval_BezierSurface(GeomGridEval_BezierSurface&&)                 = delete;
  GeomGridEval_BezierSurface& operator=(GeomGridEval_BezierSurface&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_BezierSurface>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate all grid points with first partial derivatives.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate all grid points with first and second partial derivatives.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate all grid points with derivatives up to third order.
  //! Uses direct Geom_BezierSurface::D3 evaluation.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses geometry DN method.
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

private:
  occ::handle<Geom_BezierSurface> myGeom;
};

#endif // _GeomGridEval_BezierSurface_HeaderFile

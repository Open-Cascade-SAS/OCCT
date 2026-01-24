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

#ifndef _GeomGridEval_BSplineSurface_HeaderFile
#define _GeomGridEval_BSplineSurface_HeaderFile

#include <BSplSLib_Cache.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for B-spline surface points.
//!
//! Stateless evaluator - constructor takes geometry, parameters passed to methods.
//!
//! Optimizes evaluation by:
//! - Pre-computing span indices for U and V parameters separately (O(aNbU + aNbV))
//! - Grouping evaluation by (USpan, VSpan) for cache-optimal iteration
//! - Rebuilding cache only once per span group (not per point)
//! - Writing results directly to 2D output grid (no intermediate buffers)
//!
//! Usage:
//! @code
//!   GeomGridEval_BSplineSurface anEvaluator(myBSplineSurface);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_BSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theSurface the B-spline surface to evaluate
  GeomGridEval_BSplineSurface(const occ::handle<Geom_BSplineSurface>& theSurface)
      : myGeom(theSurface)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_BSplineSurface(const GeomGridEval_BSplineSurface&)            = delete;
  GeomGridEval_BSplineSurface& operator=(const GeomGridEval_BSplineSurface&) = delete;
  GeomGridEval_BSplineSurface(GeomGridEval_BSplineSurface&&)                 = delete;
  GeomGridEval_BSplineSurface& operator=(GeomGridEval_BSplineSurface&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_BSplineSurface>& Geometry() const { return myGeom; }

  //! Evaluate grid points at Cartesian product of U and V parameters.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with derivatives up to third order.
  //! Uses direct BSplSLib::D3 evaluation (no caching for D3).
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! For orders > degree in either direction, returns zero.
  //! Uses direct geometry DN method.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams,
    int                               theNU,
    int                               theNV) const;

private:
  occ::handle<Geom_BSplineSurface> myGeom;
};

#endif // _GeomGridEval_BSplineSurface_HeaderFile

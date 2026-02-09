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

#ifndef _GeomGridEval_BSplineCurve_HeaderFile
#define _GeomGridEval_BSplineCurve_HeaderFile

#include <BSplCLib_Cache.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for B-spline curve grid points.
//!
//! Optimizes evaluation of multiple points on a B-spline curve by:
//! - Pre-computing span indices for input parameters (no runtime binary search)
//! - Pre-grouping parameters by span for cache-optimal iteration
//! - Rebuilding cache only once per span block (not per point)
//! - Using KnotSequence() from Geom_BSplineCurve for direct flat knot access
//!
//! Usage:
//! @code
//!   GeomGridEval_BSplineCurve anEvaluator(myBSplineCurve);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theCurve the B-spline curve to evaluate
  GeomGridEval_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve)
      : myGeom(theCurve)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_BSplineCurve(const GeomGridEval_BSplineCurve&)            = delete;
  GeomGridEval_BSplineCurve& operator=(const GeomGridEval_BSplineCurve&) = delete;
  GeomGridEval_BSplineCurve(GeomGridEval_BSplineCurve&&)                 = delete;
  GeomGridEval_BSplineCurve& operator=(GeomGridEval_BSplineCurve&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_BSplineCurve>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses geometry DN method.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  occ::handle<Geom_BSplineCurve> myGeom;
};

#endif // _GeomGridEval_BSplineCurve_HeaderFile

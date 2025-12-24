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
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for B-spline surface points.
//!
//! Stateless evaluator - constructor takes geometry, parameters passed to methods.
//!
//! Optimizes evaluation by:
//! - Pre-computing span indices during evaluation
//! - Sorting UV points by (USpan, VSpan, U) for cache-optimal iteration
//! - Rebuilding cache only once per span block (not per point)
//!
//! Usage (grid mode):
//! @code
//!   GeomGridEval_BSplineSurface anEvaluator(myBSplineSurface);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
//!
//! Usage (points mode):
//! @code
//!   GeomGridEval_BSplineSurface anEvaluator(myBSplineSurface);
//!   NCollection_Array1<gp_Pnt> aPoints = anEvaluator.EvaluatePoints(myUVPairs);
//! @endcode
class GeomGridEval_BSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theSurface the B-spline surface to evaluate
  GeomGridEval_BSplineSurface(const Handle(Geom_BSplineSurface)& theSurface)
      : myGeom(theSurface)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_BSplineSurface(const GeomGridEval_BSplineSurface&)            = delete;
  GeomGridEval_BSplineSurface& operator=(const GeomGridEval_BSplineSurface&) = delete;
  GeomGridEval_BSplineSurface(GeomGridEval_BSplineSurface&&)                 = delete;
  GeomGridEval_BSplineSurface& operator=(GeomGridEval_BSplineSurface&&)      = delete;

  //! Returns the geometry handle.
  const Handle(Geom_BSplineSurface)& Geometry() const { return myGeom; }

  //! Evaluate grid points at Cartesian product of U and V parameters.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate grid points with first partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate grid points with derivatives up to third order.
  //! Uses direct BSplSLib::D3 evaluation (no caching for D3).
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or parameters empty
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! For orders > degree in either direction, returns zero.
  //! Uses direct geometry DN method.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams,
    int                         theNU,
    int                         theNV) const;

  //! Evaluate points at arbitrary UV pairs.
  //! Results are returned in the original input order.
  //! @param theUVPairs array of UV coordinate pairs (gp_Pnt2d: X=U, Y=V)
  //! @return 1D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or pairs empty
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluatePoints(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate points with first partial derivatives.
  //! @param theUVPairs array of UV coordinate pairs
  //! @return 1D array of SurfD1 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::SurfD1> EvaluatePointsD1(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate points with first and second partial derivatives.
  //! @param theUVPairs array of UV coordinate pairs
  //! @return 1D array of SurfD2 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::SurfD2> EvaluatePointsD2(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate points with derivatives up to third order.
  //! @param theUVPairs array of UV coordinate pairs
  //! @return 1D array of SurfD3 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::SurfD3> EvaluatePointsD3(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate partial derivative at all UV pairs.
  //! @param theUVPairs array of UV coordinate pairs
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 1D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluatePointsDN(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs,
    int                                 theNU,
    int                                 theNV) const;

private:
  //! Find span index for a parameter value.
  //! @param[in,out] theParam parameter value (may be adjusted for periodicity)
  //! @param[in] theUDir true for U direction, false for V direction
  //! @param[in] theFlatKnots flat knots array
  //! @return span index in flat knots array
  int locateSpan(double& theParam, bool theUDir, const TColStd_Array1OfReal& theFlatKnots) const;

  //! Prepare UV points from grid parameters and sort for cache-optimal evaluation.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @param theUVPoints output array of UV points with span info
  void prepareGridPoints(const TColStd_Array1OfReal&                       theUParams,
                         const TColStd_Array1OfReal&                       theVParams,
                         NCollection_Array1<GeomGridEval::UVPointWithSpan>& theUVPoints) const;

  //! Prepare UV points from pairs and sort for cache-optimal evaluation.
  //! @param theUVPairs array of UV coordinate pairs
  //! @param theUVPoints output array of UV points with span info
  void preparePairPoints(const NCollection_Array1<gp_Pnt2d>&                theUVPairs,
                         NCollection_Array1<GeomGridEval::UVPointWithSpan>& theUVPoints) const;

  //! Compute span indices and local parameters, then sort by span.
  //! @param theUVPoints array of UV points to process (modified in place)
  void computeSpansAndSort(NCollection_Array1<GeomGridEval::UVPointWithSpan>& theUVPoints) const;

  // Private helper overloads for grid -> linear evaluation
  NCollection_Array1<GeomGridEval::SurfD1> EvaluatePointsD1(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;
  NCollection_Array1<GeomGridEval::SurfD2> EvaluatePointsD2(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;
  NCollection_Array1<GeomGridEval::SurfD3> EvaluatePointsD3(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;
  NCollection_Array1<gp_Vec> EvaluatePointsDN(const TColStd_Array1OfReal& theUParams,
                                              const TColStd_Array1OfReal& theVParams,
                                              int                         theNU,
                                              int                         theNV) const;

private:
  Handle(Geom_BSplineSurface) myGeom;

  // Mutable cache for efficient evaluation within spans
  mutable Handle(BSplSLib_Cache) myCache;
};

#endif // _GeomGridEval_BSplineSurface_HeaderFile

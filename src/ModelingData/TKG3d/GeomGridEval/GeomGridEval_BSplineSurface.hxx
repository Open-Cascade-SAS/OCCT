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
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for B-spline surface grid points.
//!
//! Optimizes evaluation of multiple points on a B-spline surface by:
//! - Pre-computing span indices during parameter setup (no runtime binary search)
//! - Pre-grouping parameters by span for cache-optimal iteration
//! - Rebuilding cache only once per span block (not per point)
//! - Using HArrayUFlatKnots()/HArrayVFlatKnots() from Geom_BSplineSurface for direct flat knot
//! access
//!
//! Usage:
//! @code
//!   GeomGridEval_BSplineSurface anEvaluator(myBSplineSurface);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_BSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Parameter value with pre-computed span index and local parameter.
  struct ParamWithSpan
  {
    double Param;      //!< Original parameter value
    double LocalParam; //!< Pre-computed local parameter in [-1, 1] range
    int    SpanIndex;  //!< Flat knot index identifying the span
  };

  //! Range of parameter indices belonging to the same span.
  struct SpanRange
  {
    int    SpanIndex;   //!< Flat knot index of this span
    int    StartIdx;    //!< First parameter index (0-based, inclusive)
    int    EndIdx;      //!< Past-the-end parameter index (exclusive)
    double SpanMid;     //!< Midpoint of span
    double SpanHalfLen; //!< Half-length of span
  };

  //! Constructor with geometry.
  //! @param theSurface the B-spline surface to evaluate
  GeomGridEval_BSplineSurface(const Handle(Geom_BSplineSurface)& theSurface)
      : myGeom(theSurface)
  {
  }

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the geometry handle.
  const Handle(Geom_BSplineSurface)& Geometry() const { return myGeom; }

  //! Returns number of U parameters.
  int NbUParams() const { return myRawUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myRawVParams.Size(); }

  //! Evaluate all grid points.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

  //! Evaluate all grid points with first partial derivatives.
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1() const;

  //! Evaluate all grid points with first and second partial derivatives.
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2() const;

  //! Evaluate all grid points with derivatives up to third order.
  //! Uses direct BSplSLib::D3 evaluation (no caching for D3).
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3() const;

  //! Evaluate partial derivative ∂^(NU+NV)S/(∂U^NU ∂V^NV) at all grid points.
  //! For orders > degree in either direction, returns zero.
  //! Uses direct geometry DN method.
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(int theNU, int theNV) const;

private:
  //! Find span index for a parameter value.
  int locateSpan(double& theParam, bool theUDir, const TColStd_Array1OfReal& theFlatKnots) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(double&                     theParam,
                         bool                        theUDir,
                         int                         theHint,
                         const TColStd_Array1OfReal& theFlatKnots) const;

  //! Compute span ranges from parameters array.
  static void computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                const TColStd_Array1OfReal&              theFlatKnots,
                                NCollection_Array1<SpanRange>&           theSpanRanges);

  //! Prepare internal data structures from raw parameters.
  //! Called lazily from EvaluateGrid() when myUSpanRanges is empty.
  void prepare() const;

private:
  Handle(Geom_BSplineSurface) myGeom;

  // Raw parameters (set by user)
  NCollection_Array1<double> myRawUParams;
  NCollection_Array1<double> myRawVParams;

  // Pre-computed parameters with span indices (0-based indexing internally)
  mutable NCollection_Array1<ParamWithSpan> myUParams;
  mutable NCollection_Array1<ParamWithSpan> myVParams;

  // Pre-computed span ranges for optimized iteration (empty = not prepared)
  mutable NCollection_Array1<SpanRange> myUSpanRanges;
  mutable NCollection_Array1<SpanRange> myVSpanRanges;

  // Cache for efficient evaluation within spans
  mutable Handle(BSplSLib_Cache) myCache;
};

#endif // _GeomGridEval_BSplineSurface_HeaderFile

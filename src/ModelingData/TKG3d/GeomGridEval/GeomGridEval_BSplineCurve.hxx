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
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for B-spline curve grid points.
//!
//! Optimizes evaluation of multiple points on a B-spline curve by:
//! - Pre-computing span indices during parameter setup (no runtime binary search)
//! - Pre-grouping parameters by span for cache-optimal iteration
//! - Rebuilding cache only once per span block (not per point)
//! - Using HArrayFlatKnots() from Geom_BSplineCurve for direct flat knot access
//!
//! Usage:
//! @code
//!   GeomGridEval_BSplineCurve anEvaluator(myBSplineCurve);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theCurve the B-spline curve to evaluate
  GeomGridEval_BSplineCurve(const Handle(Geom_BSplineCurve)& theCurve)
      : myGeom(theCurve)
  {
  }

  //! Parameter value with pre-computed span index and local parameter.
  struct ParamWithSpan
  {
    double Param;      //!< Original parameter value
    double LocalParam; //!< Pre-computed local parameter in [0, 1] range for polynomial evaluation
    int    SpanIndex;  //!< Flat knot index identifying the span
  };

  //! Range of parameter indices belonging to the same span.
  struct SpanRange
  {
    int    SpanIndex;   //!< Flat knot index of this span
    int    StartIdx;    //!< First parameter index (0-based, inclusive)
    int    EndIdx;      //!< Past-the-end parameter index (exclusive)
    double SpanMid;     //!< Midpoint of span (for cache convention: start + length/2)
    double SpanHalfLen; //!< Half-length of span (for cache convention: length/2)
  };

  //! Set parameters for grid evaluation (by const reference).
  //! Span indices and local parameters are pre-computed for optimal EvaluateGrid() performance.
  //! @param theParams array of parameter values
  Standard_EXPORT void SetParams(const TColStd_Array1OfReal& theParams);

  //! Set parameters for grid evaluation (by move).
  //! @param theParams array of parameter values to move
  void SetParams(NCollection_Array1<double>&& theParams)
  {
    myRawParams  = std::move(theParams);
    mySpanRanges = NCollection_Array1<SpanRange>(); // Clear cached span data
  }

  //! Returns the geometry handle.
  const Handle(Geom_BSplineCurve)& Geometry() const { return myGeom; }

  //! Returns number of parameters.
  int NbParams() const { return myRawParams.Size(); }

  //! Evaluate all grid points.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

  //! Evaluate all grid points with first derivative.
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1() const;

  //! Evaluate all grid points with first and second derivatives.
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2() const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3() const;

  //! Evaluate Nth derivative at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses geometry DN method.
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(int theN) const;

private:
  //! Find span index for a parameter value.
  int locateSpan(double theParam, const TColStd_Array1OfReal& theFlatKnots) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(double                      theParam,
                         int                         theHint,
                         const TColStd_Array1OfReal& theFlatKnots) const;

  //! Compute span ranges from parameters array.
  static void computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                const TColStd_Array1OfReal&              theFlatKnots,
                                NCollection_Array1<SpanRange>&           theSpanRanges);

  //! Prepare internal data structures from raw parameters.
  //! Called lazily from EvaluateGrid() when mySpanRanges is empty.
  void prepare() const;

private:
  Handle(Geom_BSplineCurve) myGeom;

  // Raw parameters storage
  NCollection_Array1<double> myRawParams;

  // Pre-computed parameters with span indices (0-based indexing internally)
  mutable NCollection_Array1<ParamWithSpan> myParams;

  // Pre-computed span ranges for optimized iteration (empty = not prepared)
  mutable NCollection_Array1<SpanRange> mySpanRanges;

  // Cache for efficient evaluation within spans
  mutable Handle(BSplCLib_Cache) myCache;
};

#endif // _GeomGridEval_BSplineCurve_HeaderFile

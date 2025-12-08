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

#ifndef _BSplCLib_GridEvaluator_HeaderFile
#define _BSplCLib_GridEvaluator_HeaderFile

#include <BSplCLib_Cache.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>

class gp_Pnt;

//! @brief Efficient batch evaluator for B-spline curve grid points.
//!
//! Optimizes evaluation of multiple points on a B-spline curve by:
//! - Pre-computing span indices during parameter setup (no runtime binary search)
//! - Pre-grouping parameters by span for cache-optimal iteration
//! - Rebuilding cache only once per span block (not per point)
//!
//! Usage:
//! @code
//!   BSplCLib_GridEvaluator anEvaluator;
//!   anEvaluator.Initialize(degree, poles, weights, flatKnots, rational, periodic);
//!   anEvaluator.SetParams(params);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class BSplCLib_GridEvaluator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT BSplCLib_GridEvaluator();

  //! Initialize with B-spline curve data.
  //! @param theDegree    polynomial degree (must be >= 1)
  //! @param thePoles     handle to array of control points
  //! @param theWeights   handle to array of weights (null handle for non-rational)
  //! @param theFlatKnots handle to flat knot sequence
  //! @param theRational  true if rational curve (requires non-null weights)
  //! @param thePeriodic  true if periodic curve
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool Initialize(int                                  theDegree,
                                  const Handle(TColgp_HArray1OfPnt)&   thePoles,
                                  const Handle(TColStd_HArray1OfReal)& theWeights,
                                  const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                  bool                                 theRational,
                                  bool                                 thePeriodic);

  //! Set parameters from array.
  //! Span indices and local parameters are pre-computed for optimal EvaluateGrid() performance.
  //! @param theParams handle to array of parameter values (1-based)
  Standard_EXPORT void SetParams(const Handle(TColStd_HArray1OfReal)& theParams);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Size(); }

  //! Evaluate all grid points and return as array.
  //! This is the main method - optimized for maximum performance.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

private:
  //! Parameter value with pre-computed span index and local parameter.
  struct ParamWithSpan
  {
    double Param;      //!< Original parameter value
    double LocalParam; //!< Pre-computed local parameter in [-1, 1] range for polynomial evaluation
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

  //! Find span index for a parameter value.
  int locateSpan(double theParam) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(double theParam, int theHint) const;

  //! Compute span ranges from parameters array.
  //! @param theParams      pre-computed parameters with span indices
  //! @param theFlatKnots   flat knots array for span start/length calculation
  //! @param theSpanRanges  output array of span ranges
  static void computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                const TColStd_Array1OfReal&              theFlatKnots,
                                NCollection_Array1<SpanRange>&           theSpanRanges);

  //! Rebuild cache for the given parameter value.
  void rebuildCache(double theParam) const;

private:
  int                           myDegree;
  Handle(TColgp_HArray1OfPnt)   myPoles;
  Handle(TColStd_HArray1OfReal) myWeights;
  Handle(TColStd_HArray1OfReal) myFlatKnots;
  bool                          myRational;
  bool                          myPeriodic;
  bool                          myIsInitialized;

  // Pre-computed parameters with span indices (0-based indexing internally)
  NCollection_Array1<ParamWithSpan> myParams;

  // Pre-computed span ranges for optimized iteration
  NCollection_Array1<SpanRange> mySpanRanges;

  // Cache for efficient evaluation within spans
  mutable Handle(BSplCLib_Cache) myCache;
};

#endif // _BSplCLib_GridEvaluator_HeaderFile

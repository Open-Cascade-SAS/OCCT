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

#ifndef _BSplSLib_GridEvaluator_HeaderFile
#define _BSplSLib_GridEvaluator_HeaderFile

#include <BSplSLib_Cache.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

class gp_Pnt;

//! @brief Efficient batch evaluator for B-spline surface grid points.
//!
//! Optimizes evaluation of multiple points on a B-spline surface by:
//! - Pre-computing span indices during parameter setup (no runtime binary search)
//! - Pre-grouping parameters by span for cache-optimal iteration
//! - Rebuilding cache only once per span block (not per point)
//!
//! Usage:
//! @code
//!   BSplSLib_GridEvaluator anEvaluator;
//!   anEvaluator.Initialize(degU, degV, poles, weights, uKnots, vKnots, ...);
//!   anEvaluator.SetUParams(uParams);
//!   anEvaluator.SetVParams(vParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class BSplSLib_GridEvaluator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT BSplSLib_GridEvaluator();

  //! Initialize with B-spline surface data.
  //! @param theDegreeU     degree in U direction (must be >= 1)
  //! @param theDegreeV     degree in V direction (must be >= 1)
  //! @param thePoles       handle to 2D array of control points
  //! @param theWeights     handle to 2D array of weights (null handle for non-rational)
  //! @param theUFlatKnots  handle to flat knot sequence in U direction
  //! @param theVFlatKnots  handle to flat knot sequence in V direction
  //! @param theURational   true if rational in U direction (requires non-null weights)
  //! @param theVRational   true if rational in V direction (requires non-null weights)
  //! @param theUPeriodic   true if periodic in U direction
  //! @param theVPeriodic   true if periodic in V direction
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool Initialize(int                                  theDegreeU,
                                  int                                  theDegreeV,
                                  const Handle(TColgp_HArray2OfPnt)&   thePoles,
                                  const Handle(TColStd_HArray2OfReal)& theWeights,
                                  const Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                  const Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                  bool                                 theURational,
                                  bool                                 theVRational,
                                  bool                                 theUPeriodic,
                                  bool                                 theVPeriodic);

  //! Set U parameters from array.
  //! Span indices and span ranges are pre-computed for optimal EvaluateGrid() performance.
  //! @param theUParams handle to array of U parameter values (1-based)
  Standard_EXPORT void SetUParams(const Handle(TColStd_HArray1OfReal)& theUParams);

  //! Set V parameters from array.
  //! Span indices and span ranges are pre-computed for optimal EvaluateGrid() performance.
  //! @param theVParams handle to array of V parameter values (1-based)
  Standard_EXPORT void SetVParams(const Handle(TColStd_HArray1OfReal)& theVParams);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points and return as 2D array.
  //! This is the main method - optimized for maximum performance.
  //! Points are evaluated in span-grouped order to minimize cache rebuilds.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

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
  int locateSpan(double theParam, bool theUDir) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(double theParam, bool theUDir, int theHint) const;

  //! Compute span ranges from parameters array.
  //! @param theParams      pre-computed parameters with span indices
  //! @param theFlatKnots   flat knots array for span start/length calculation
  //! @param theSpanRanges  output array of span ranges
  static void computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                const TColStd_Array1OfReal&              theFlatKnots,
                                NCollection_Array1<SpanRange>&           theSpanRanges);

  //! Rebuild cache for the given parameter values.
  void rebuildCache(double theUParam, double theVParam) const;

private:
  int                           myDegreeU;
  int                           myDegreeV;
  Handle(TColgp_HArray2OfPnt)   myPoles;
  Handle(TColStd_HArray2OfReal) myWeights;
  Handle(TColStd_HArray1OfReal) myUFlatKnots;
  Handle(TColStd_HArray1OfReal) myVFlatKnots;
  bool                          myURational;
  bool                          myVRational;
  bool                          myUPeriodic;
  bool                          myVPeriodic;
  bool                          myIsInitialized;

  // Pre-computed parameters with span indices (0-based indexing internally)
  NCollection_Array1<ParamWithSpan> myUParams;
  NCollection_Array1<ParamWithSpan> myVParams;

  // Pre-computed span ranges for optimized iteration
  NCollection_Array1<SpanRange> myUSpanRanges;
  NCollection_Array1<SpanRange> myVSpanRanges;

  // Cache for efficient evaluation within spans
  mutable Handle(BSplSLib_Cache) myCache;
};

#endif // _BSplSLib_GridEvaluator_HeaderFile

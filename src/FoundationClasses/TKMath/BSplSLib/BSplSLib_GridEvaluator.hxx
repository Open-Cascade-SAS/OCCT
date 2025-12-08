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

#include <optional>

class gp_Pnt;
class gp_Vec;

//! @brief Efficient grid evaluator for B-spline and Bezier surfaces.
//!
//! Optimizes evaluation of multiple points on a B-spline surface by
//! pre-computing span indices for parameter arrays and reusing a single
//! BSplSLib_Cache instance. The cache is rebuilt only when crossing span
//! boundaries, maximizing performance for sequential grid evaluation.
//!
//! The class is designed for scenarios where many points need to be evaluated
//! on a B-spline surface, such as:
//! - Building sampling grids for extrema computation
//! - Tessellation and meshing
//! - Surface analysis and visualization
//!
//! Usage for B-spline:
//! @code
//!   BSplSLib_GridEvaluator anEvaluator;
//!   anEvaluator.Initialize(degU, degV, poles, weights, uKnots, vKnots, ...);
//!   anEvaluator.PrepareUParamsFromKnots(uMin, uMax, minSamplesU);
//!   anEvaluator.PrepareVParamsFromKnots(vMin, vMax, minSamplesV);
//!
//!   for (int iu = 1; iu <= anEvaluator.NbUParams(); ++iu)
//!     for (int iv = 1; iv <= anEvaluator.NbVParams(); ++iv)
//!       if (auto aPt = anEvaluator.Value(iu, iv))
//!         ProcessPoint(*aPt);
//! @endcode
//!
//! Usage for Bezier:
//! @code
//!   BSplSLib_GridEvaluator anEvaluator;
//!   anEvaluator.InitializeBezier(poles, weights);
//!   anEvaluator.PrepareUParams(0.0, 1.0, 10);
//!   anEvaluator.PrepareVParams(0.0, 1.0, 10);
//!
//!   for (int iu = 1; iu <= anEvaluator.NbUParams(); ++iu)
//!     for (int iv = 1; iv <= anEvaluator.NbVParams(); ++iv)
//!       if (auto aPt = anEvaluator.Value(iu, iv))
//!         ProcessPoint(*aPt);
//! @endcode
class BSplSLib_GridEvaluator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Parameter value with pre-computed span index.
  struct ParamWithSpan
  {
    double Param;     //!< Parameter value
    int    SpanIndex; //!< Flat knot index identifying the span
  };

  //! Range of parameter indices belonging to the same span.
  struct SpanRange
  {
    int SpanIndex; //!< Flat knot index of this span
    int StartIdx;  //!< First parameter index in this span (1-based, inclusive)
    int EndIdx;    //!< Past-the-end parameter index (exclusive)
  };

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

  //! Initialize with Bezier surface data.
  //! Internally generates appropriate flat knot vectors.
  //! @param thePoles   handle to 2D array of control points (degreeU = NbUPoles - 1, degreeV =
  //! NbVPoles - 1)
  //! @param theWeights handle to 2D array of weights (null handle for non-rational)
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool InitializeBezier(const Handle(TColgp_HArray2OfPnt)&   thePoles,
                                        const Handle(TColStd_HArray2OfReal)& theWeights);

  //! Prepare U parameters aligned with knots (optimal for B-splines).
  //! Creates sample points within each knot span based on degree.
  //! The span indices are computed during this process - no binary search later.
  //! @param theUMin        minimum U parameter
  //! @param theUMax        maximum U parameter
  //! @param theMinSamples  minimum number of samples (may increase based on knots)
  //! @param theIncludeEnds if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareUParamsFromKnots(double theUMin,
                                               double theUMax,
                                               int    theMinSamples,
                                               bool   theIncludeEnds = true);

  //! Prepare V parameters aligned with knots (optimal for B-splines).
  //! @param theVMin        minimum V parameter
  //! @param theVMax        maximum V parameter
  //! @param theMinSamples  minimum number of samples (may increase based on knots)
  //! @param theIncludeEnds if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareVParamsFromKnots(double theVMin,
                                               double theVMax,
                                               int    theMinSamples,
                                               bool   theIncludeEnds = true);

  //! Prepare U parameters with uniform distribution.
  //! Span indices are computed for each parameter.
  //! @param theUMin        minimum U parameter
  //! @param theUMax        maximum U parameter
  //! @param theNbU         number of U samples (minimum 2)
  //! @param theIncludeEnds if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareUParams(double theUMin,
                                      double theUMax,
                                      int    theNbU,
                                      bool   theIncludeEnds = true);

  //! Prepare V parameters with uniform distribution.
  //! @param theVMin        minimum V parameter
  //! @param theVMax        maximum V parameter
  //! @param theNbV         number of V samples (minimum 2)
  //! @param theIncludeEnds if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareVParams(double theVMin,
                                      double theVMax,
                                      int    theNbV,
                                      bool   theIncludeEnds = true);

  //! Set U parameters from pre-computed array.
  //! Span indices are computed for each parameter value.
  //! Use this when exact parameter values must match external sources.
  //! @param theUParams handle to array of U parameter values (1-based)
  Standard_EXPORT void SetUParams(const Handle(TColStd_HArray1OfReal)& theUParams);

  //! Set V parameters from pre-computed array.
  //! Span indices are computed for each parameter value.
  //! Use this when exact parameter values must match external sources.
  //! @param theVParams handle to array of V parameter values (1-based)
  Standard_EXPORT void SetVParams(const Handle(TColStd_HArray1OfReal)& theVParams);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns the polynomial degree in U direction.
  int DegreeU() const { return myDegreeU; }

  //! Returns the polynomial degree in V direction.
  int DegreeV() const { return myDegreeV; }

  //! Returns handle to control points array.
  const Handle(TColgp_HArray2OfPnt)& Poles() const { return myPoles; }

  //! Returns handle to weights array (may be null for non-rational).
  const Handle(TColStd_HArray2OfReal)& Weights() const { return myWeights; }

  //! Returns handle to U flat knots array.
  const Handle(TColStd_HArray1OfReal)& UFlatKnots() const { return myUFlatKnots; }

  //! Returns handle to V flat knots array.
  const Handle(TColStd_HArray1OfReal)& VFlatKnots() const { return myVFlatKnots; }

  //! Returns true if the surface is rational in U direction.
  bool IsURational() const { return myURational; }

  //! Returns true if the surface is rational in V direction.
  bool IsVRational() const { return myVRational; }

  //! Returns true if the surface is periodic in U direction.
  bool IsUPeriodic() const { return myUPeriodic; }

  //! Returns true if the surface is periodic in V direction.
  bool IsVPeriodic() const { return myVPeriodic; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Length(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Length(); }

  //! Returns U parameter data at index (1-based).
  //! @param theIndex parameter index (1-based)
  //! @return parameter data if index is valid, empty optional otherwise
  std::optional<ParamWithSpan> UParamData(int theIndex) const
  {
    if (theIndex < 1 || theIndex > myUParams.Length())
    {
      return std::nullopt;
    }
    return myUParams.Value(theIndex);
  }

  //! Returns V parameter data at index (1-based).
  //! @param theIndex parameter index (1-based)
  //! @return parameter data if index is valid, empty optional otherwise
  std::optional<ParamWithSpan> VParamData(int theIndex) const
  {
    if (theIndex < 1 || theIndex > myVParams.Length())
    {
      return std::nullopt;
    }
    return myVParams.Value(theIndex);
  }

  //! Returns U parameter value at index (1-based).
  //! @param theIndex parameter index (1-based)
  //! @return parameter value if index is valid, empty optional otherwise
  std::optional<double> UParam(int theIndex) const
  {
    if (theIndex < 1 || theIndex > myUParams.Length())
    {
      return std::nullopt;
    }
    return myUParams.Value(theIndex).Param;
  }

  //! Returns V parameter value at index (1-based).
  //! @param theIndex parameter index (1-based)
  //! @return parameter value if index is valid, empty optional otherwise
  std::optional<double> VParam(int theIndex) const
  {
    if (theIndex < 1 || theIndex > myVParams.Length())
    {
      return std::nullopt;
    }
    return myVParams.Value(theIndex).Param;
  }

  //! Evaluate point at grid position (theIU, theIV).
  //! Uses pre-computed span indices - no binary search.
  //! @param theIU U index (1-based)
  //! @param theIV V index (1-based)
  //! @return computed point on surface, or empty optional if indices are invalid
  Standard_EXPORT std::optional<gp_Pnt> Value(int theIU, int theIV) const;

  //! Evaluate point at grid position.
  //! @param theIU U index (1-based)
  //! @param theIV V index (1-based)
  //! @param theP  output point
  //! @return true if evaluation succeeded, false if indices are invalid
  Standard_EXPORT bool D0(int theIU, int theIV, gp_Pnt& theP) const;

  //! Fast evaluation without bounds checking for performance-critical loops.
  //! Caller must ensure indices are valid: 1 <= theIU <= NbUParams(), 1 <= theIV <= NbVParams().
  //! @param theIU U index (1-based, unchecked)
  //! @param theIV V index (1-based, unchecked)
  //! @param theP  output point
  void D0Unchecked(int theIU, int theIV, gp_Pnt& theP) const
  {
    const ParamWithSpan& aUParam = myUParams.Value(theIU);
    const ParamWithSpan& aVParam = myVParams.Value(theIV);

    // Inline cache validity check for maximum performance
    if (myCachedUSpanIndex != aUParam.SpanIndex || myCachedVSpanIndex != aVParam.SpanIndex
        || myCache.IsNull())
    {
      rebuildCache(aUParam.SpanIndex, aVParam.SpanIndex, aUParam.Param, aVParam.Param);
    }
    myCache->D0(aUParam.Param, aVParam.Param, theP);
  }

  //! Returns raw pointer to U parameters array for zero-overhead iteration.
  //! Valid indices: [0..NbUParams()-1]. The pointer is valid as long as the evaluator exists.
  //! @return pointer to first U parameter, or nullptr if no parameters prepared
  const ParamWithSpan* UParamsData() const
  {
    return myUParams.IsEmpty() ? nullptr : &myUParams.First();
  }

  //! Returns raw pointer to V parameters array for zero-overhead iteration.
  //! Valid indices: [0..NbVParams()-1]. The pointer is valid as long as the evaluator exists.
  //! @return pointer to first V parameter, or nullptr if no parameters prepared
  const ParamWithSpan* VParamsData() const
  {
    return myVParams.IsEmpty() ? nullptr : &myVParams.First();
  }

  //! Fastest evaluation using raw parameter data (no array bounds checking).
  //! Use with UParamsData()/VParamsData() for maximum performance.
  //! IMPORTANT: Call EnsureCacheReady() once before using this method.
  //! @param theUParam pre-fetched U parameter data
  //! @param theVParam pre-fetched V parameter data
  //! @param theP output point
  void D0Raw(const ParamWithSpan& theUParam, const ParamWithSpan& theVParam, gp_Pnt& theP) const
  {
    // Note: No IsNull() check - assumes EnsureCacheReady() was called
    if (myCachedUSpanIndex != theUParam.SpanIndex || myCachedVSpanIndex != theVParam.SpanIndex)
    {
      rebuildCache(theUParam.SpanIndex, theVParam.SpanIndex, theUParam.Param, theVParam.Param);
    }
    myCache->D0(theUParam.Param, theVParam.Param, theP);
  }

  //! Ensures the cache is created and ready for evaluation.
  //! Call this once before using D0Raw() in a loop.
  void EnsureCacheReady() const
  {
    if (myCache.IsNull() && !myUParams.IsEmpty() && !myVParams.IsEmpty())
    {
      rebuildCache(myUParams.First().SpanIndex,
                   myVParams.First().SpanIndex,
                   myUParams.First().Param,
                   myVParams.First().Param);
    }
  }

  //! Returns true if all U parameters are in the same span.
  //! When true, the cache never needs to be rebuilt for U direction.
  bool IsSingleUSpan() const
  {
    if (myUParams.IsEmpty())
      return true;
    const int aFirstSpan = myUParams.First().SpanIndex;
    const int aLastSpan  = myUParams.Last().SpanIndex;
    return aFirstSpan == aLastSpan;
  }

  //! Returns true if all V parameters are in the same span.
  //! When true, the cache never needs to be rebuilt for V direction.
  bool IsSingleVSpan() const
  {
    if (myVParams.IsEmpty())
      return true;
    const int aFirstSpan = myVParams.First().SpanIndex;
    const int aLastSpan  = myVParams.Last().SpanIndex;
    return aFirstSpan == aLastSpan;
  }

  //! Returns true if all parameters fit in a single UV span.
  //! When true, D0SingleSpan() can be used for maximum performance.
  bool IsSingleSpan() const { return IsSingleUSpan() && IsSingleVSpan(); }

  //! Returns number of U span ranges (contiguous blocks of parameters in the same span).
  int NbUSpanRanges() const { return myUSpanRanges.Length(); }

  //! Returns number of V span ranges.
  int NbVSpanRanges() const { return myVSpanRanges.Length(); }

  //! Returns U span range at index (0-based).
  //! @param theIndex range index (0-based)
  //! @return span range data
  const SpanRange& USpanRange(int theIndex) const { return myUSpanRanges.Value(theIndex); }

  //! Returns V span range at index (0-based).
  //! @param theIndex range index (0-based)
  //! @return span range data
  const SpanRange& VSpanRange(int theIndex) const { return myVSpanRanges.Value(theIndex); }

  //! Fastest evaluation when all parameters are in a single span.
  //! Call IsSingleSpan() first to verify. Cache is never rebuilt.
  //! @param theUParam pre-fetched U parameter data
  //! @param theVParam pre-fetched V parameter data
  //! @param theP output point
  void D0SingleSpan(const ParamWithSpan& theUParam,
                    const ParamWithSpan& theVParam,
                    gp_Pnt&              theP) const
  {
    myCache->D0(theUParam.Param, theVParam.Param, theP);
  }

  //! Direct cache evaluation using raw parameter values.
  //! This is the absolute fastest path - no struct access, no span checking.
  //! Requires: EnsureCacheReady() called and IsSingleSpan() == true.
  //! @param theU U parameter value
  //! @param theV V parameter value
  //! @param theP output point
  void D0CacheDirect(double theU, double theV, gp_Pnt& theP) const
  {
    myCache->D0(theU, theV, theP);
  }

  //! Evaluation using cache's native validity check (same pattern as GeomAdaptor).
  //! This method bypasses span index tracking and uses the cache's IsCacheValid directly.
  //! For sequential access where cache hits are common, this should match GeomAdaptor performance.
  //! @param theU U parameter value
  //! @param theV V parameter value
  //! @param theP output point
  void D0Direct(double theU, double theV, gp_Pnt& theP) const
  {
    if (myCache.IsNull() || !myCache->IsCacheValid(theU, theV))
    {
      rebuildCacheDirect(theU, theV);
    }
    myCache->D0(theU, theV, theP);
  }

  //! Returns the internal cache (for advanced usage / testing).
  //! Allows direct access to the cache for performance-critical code
  //! that wants to replicate GeomAdaptor's exact pattern.
  //! @return handle to the internal cache (may be null if not yet built)
  const Handle(BSplSLib_Cache)& Cache() const { return myCache; }

  //! Ensures the internal cache is created and built for the given parameters.
  //! Call this once before using Cache() directly for evaluation.
  //! @param theU U parameter to build cache for
  //! @param theV V parameter to build cache for
  void BuildCache(double theU, double theV) const { rebuildCacheDirect(theU, theV); }

  //! Evaluate point and first derivatives at grid position.
  //! @param theIU U index (1-based)
  //! @param theIV V index (1-based)
  //! @param theP  output point
  //! @param theDU output derivative in U direction
  //! @param theDV output derivative in V direction
  //! @return true if evaluation succeeded, false if indices are invalid
  Standard_EXPORT bool D1(int theIU, int theIV, gp_Pnt& theP, gp_Vec& theDU, gp_Vec& theDV) const;

  //! Evaluate point and derivatives up to 2nd order at grid position.
  //! @param theIU  U index (1-based)
  //! @param theIV  V index (1-based)
  //! @param theP   output point
  //! @param theDU  output first derivative in U
  //! @param theDV  output first derivative in V
  //! @param theDUU output second derivative in U
  //! @param theDVV output second derivative in V
  //! @param theDUV output mixed second derivative
  //! @return true if evaluation succeeded, false if indices are invalid
  Standard_EXPORT bool D2(int     theIU,
                          int     theIV,
                          gp_Pnt& theP,
                          gp_Vec& theDU,
                          gp_Vec& theDV,
                          gp_Vec& theDUU,
                          gp_Vec& theDVV,
                          gp_Vec& theDUV) const;

  //! Evaluate all grid points and return as 2D array.
  //! Array indexing: result(iU, iV) where iU in [1..NbUParams], iV in [1..NbVParams].
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if evaluation failed
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

private:
  //! Compute parameters aligned with knots and their span indices (single-pass).
  //! @param theFlatKnots  handle to flat knot sequence
  //! @param theDegree     polynomial degree
  //! @param thePeriodic   periodicity flag
  //! @param theParamMin   minimum parameter value
  //! @param theParamMax   maximum parameter value
  //! @param theMinSamples minimum number of samples
  //! @param theIncludeEnds whether to include exact boundaries
  //! @param theParams     output array of parameters with span indices
  void computeKnotAlignedParams(const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                int                                  theDegree,
                                bool                                 thePeriodic,
                                double                               theParamMin,
                                double                               theParamMax,
                                int                                  theMinSamples,
                                bool                                 theIncludeEnds,
                                NCollection_Array1<ParamWithSpan>&   theParams) const;

  //! Compute uniform parameters with their span indices.
  //! @param theFlatKnots  handle to flat knot sequence
  //! @param theDegree     polynomial degree
  //! @param thePeriodic   periodicity flag
  //! @param theParamMin   minimum parameter value
  //! @param theParamMax   maximum parameter value
  //! @param theNbSamples  number of samples
  //! @param theIncludeEnds whether to include exact boundaries
  //! @param theParams     output array of parameters with span indices
  void computeUniformParams(const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                            int                                  theDegree,
                            bool                                 thePeriodic,
                            double                               theParamMin,
                            double                               theParamMax,
                            int                                  theNbSamples,
                            bool                                 theIncludeEnds,
                            NCollection_Array1<ParamWithSpan>&   theParams) const;

  //! Find span index for a parameter value using binary search.
  int locateSpan(const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                 int                                  theDegree,
                 bool                                 thePeriodic,
                 double                               theParam) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                         int                                  theDegree,
                         bool                                 thePeriodic,
                         double                               theParam,
                         int                                  theHint) const;

  //! Check if UV indices are valid for parameter access.
  bool areValidIndices(int theIU, int theIV) const
  {
    return theIU >= 1 && theIU <= myUParams.Length() && theIV >= 1 && theIV <= myVParams.Length();
  }

  //! Ensure cache is valid for the given span indices, rebuild if necessary.
  //! @param theUSpanIndex U span index to validate cache against
  //! @param theVSpanIndex V span index to validate cache against
  //! @param theUParam U parameter value within the span (used for cache rebuild)
  //! @param theVParam V parameter value within the span (used for cache rebuild)
  void ensureCacheValid(int    theUSpanIndex,
                        int    theVSpanIndex,
                        double theUParam,
                        double theVParam) const;

  //! Rebuild cache for the given span (called when cache is invalid).
  //! Separated from ensureCacheValid to allow inlining of the fast path.
  void rebuildCache(int theUSpanIndex, int theVSpanIndex, double theUParam, double theVParam) const;

  //! Rebuild cache using only parameter values (for D0Direct method).
  //! Does not update span index tracking.
  void rebuildCacheDirect(double theUParam, double theVParam) const;

  //! Compute span ranges from parameters array.
  //! @param theParams     source parameters with span indices
  //! @param theSpanRanges output span ranges array
  static void computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                NCollection_Array1<SpanRange>&           theSpanRanges);

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

  // Pre-computed parameters with span indices
  NCollection_Array1<ParamWithSpan> myUParams;
  NCollection_Array1<ParamWithSpan> myVParams;

  // Pre-computed span ranges for optimized iteration
  NCollection_Array1<SpanRange> myUSpanRanges;
  NCollection_Array1<SpanRange> myVSpanRanges;

  // Cache for efficient evaluation within spans
  mutable Handle(BSplSLib_Cache) myCache;
  mutable int                    myCachedUSpanIndex;
  mutable int                    myCachedVSpanIndex;
};

#endif // _BSplSLib_GridEvaluator_HeaderFile

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

#include <optional>

class gp_Pnt;
class gp_Vec;

//! @brief Efficient grid evaluator for B-spline and Bezier curves.
//!
//! Optimizes evaluation of multiple points on a B-spline curve by
//! pre-computing span indices for parameter arrays and reusing a single
//! BSplCLib_Cache instance. The cache is rebuilt only when crossing span
//! boundaries, maximizing performance for sequential grid evaluation.
//!
//! The class is designed for scenarios where many points need to be evaluated
//! on a B-spline curve, such as:
//! - Building sampling grids for extrema computation
//! - Tessellation and discretization
//! - Curve analysis
//!
//! Usage for B-spline:
//! @code
//!   BSplCLib_GridEvaluator anEvaluator;
//!   anEvaluator.Initialize(degree, poles, weights, flatKnots, rational, periodic);
//!   anEvaluator.PrepareParamsFromKnots(tMin, tMax, minSamples);
//!
//!   for (int i = 1; i <= anEvaluator.NbParams(); ++i)
//!   {
//!     if (auto aPt = anEvaluator.Value(i))
//!       ProcessPoint(*aPt);
//!   }
//! @endcode
//!
//! Usage for Bezier:
//! @code
//!   BSplCLib_GridEvaluator anEvaluator;
//!   anEvaluator.InitializeBezier(poles, weights);
//!   anEvaluator.PrepareParams(0.0, 1.0, 20);
//!
//!   for (int i = 1; i <= anEvaluator.NbParams(); ++i)
//!   {
//!     if (auto aPt = anEvaluator.Value(i))
//!       ProcessPoint(*aPt);
//!   }
//! @endcode
class BSplCLib_GridEvaluator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Parameter value with pre-computed span index.
  struct ParamWithSpan
  {
    double Param;     //!< Parameter value
    int    SpanIndex; //!< Flat knot index identifying the span
  };

  //! Empty constructor.
  Standard_EXPORT BSplCLib_GridEvaluator();

  //! Initialize with B-spline curve data.
  //! @param theDegree    polynomial degree (must be >= 1)
  //! @param thePoles     handle to array of control points
  //! @param theWeights   handle to array of weights (null handle for non-rational)
  //! @param theFlatKnots handle to flat knot sequence (must be sorted, size = NbPoles + Degree + 1)
  //! @param theRational  true if rational curve (requires non-null weights)
  //! @param thePeriodic  true if periodic curve
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool Initialize(int                                  theDegree,
                                  const Handle(TColgp_HArray1OfPnt)&   thePoles,
                                  const Handle(TColStd_HArray1OfReal)& theWeights,
                                  const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                  bool                                 theRational,
                                  bool                                 thePeriodic);

  //! Initialize with Bezier curve data.
  //! Internally generates appropriate flat knot vector [0,0,...,0,1,1,...,1].
  //! @param thePoles   handle to array of control points (degree = NbPoles - 1)
  //! @param theWeights handle to array of weights (null handle for non-rational)
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool InitializeBezier(const Handle(TColgp_HArray1OfPnt)&   thePoles,
                                        const Handle(TColStd_HArray1OfReal)& theWeights);

  //! Prepare parameters aligned with knots (optimal for B-splines).
  //! Creates sample points within each knot span based on degree.
  //! The span indices are computed during this process - no binary search later.
  //! @param theParamMin     minimum parameter value
  //! @param theParamMax     maximum parameter value
  //! @param theMinSamples   minimum number of samples (may increase based on knots)
  //! @param theIncludeEnds  if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareParamsFromKnots(double theParamMin,
                                              double theParamMax,
                                              int    theMinSamples,
                                              bool   theIncludeEnds = true);

  //! Prepare parameters with uniform distribution.
  //! Span indices are computed for each parameter.
  //! @param theParamMin    minimum parameter value
  //! @param theParamMax    maximum parameter value
  //! @param theNbSamples   number of samples (minimum 2)
  //! @param theIncludeEnds if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareParams(double theParamMin,
                                     double theParamMax,
                                     int    theNbSamples,
                                     bool   theIncludeEnds = true);

  //! Set parameters from pre-computed array.
  //! Span indices are computed for each parameter value.
  //! Use this when exact parameter values must match external sources.
  //! @param theParams handle to array of parameter values (1-based)
  Standard_EXPORT void SetParams(const Handle(TColStd_HArray1OfReal)& theParams);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns the polynomial degree.
  int Degree() const { return myDegree; }

  //! Returns handle to control points array.
  const Handle(TColgp_HArray1OfPnt)& Poles() const { return myPoles; }

  //! Returns handle to weights array (may be null for non-rational).
  const Handle(TColStd_HArray1OfReal)& Weights() const { return myWeights; }

  //! Returns handle to flat knots array.
  const Handle(TColStd_HArray1OfReal)& FlatKnots() const { return myFlatKnots; }

  //! Returns true if the curve is rational.
  bool IsRational() const { return myRational; }

  //! Returns true if the curve is periodic.
  bool IsPeriodic() const { return myPeriodic; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Length(); }

  //! Returns parameter data at index (1-based).
  //! @param theIndex parameter index (1-based)
  //! @return parameter data if index is valid, empty optional otherwise
  std::optional<ParamWithSpan> ParamData(int theIndex) const
  {
    if (theIndex < 1 || theIndex > myParams.Length())
    {
      return std::nullopt;
    }
    return myParams.Value(theIndex);
  }

  //! Returns parameter value at index (1-based).
  //! @param theIndex parameter index (1-based)
  //! @return parameter value if index is valid, empty optional otherwise
  std::optional<double> Param(int theIndex) const
  {
    if (theIndex < 1 || theIndex > myParams.Length())
    {
      return std::nullopt;
    }
    return myParams.Value(theIndex).Param;
  }

  //! Evaluate point at grid position.
  //! Uses pre-computed span indices - no binary search.
  //! @param theIndex parameter index (1-based)
  //! @return computed point on curve, or empty optional if index is invalid
  Standard_EXPORT std::optional<gp_Pnt> Value(int theIndex) const;

  //! Evaluate point at grid position.
  //! @param theIndex parameter index (1-based)
  //! @param theP     output point
  //! @return true if evaluation succeeded, false if index is invalid
  Standard_EXPORT bool D0(int theIndex, gp_Pnt& theP) const;

  //! Evaluate point and first derivative at grid position.
  //! @param theIndex parameter index (1-based)
  //! @param theP     output point
  //! @param theD1    output derivative
  //! @return true if evaluation succeeded, false if index is invalid
  Standard_EXPORT bool D1(int theIndex, gp_Pnt& theP, gp_Vec& theD1) const;

  //! Evaluate point and derivatives up to 2nd order at grid position.
  //! @param theIndex parameter index (1-based)
  //! @param theP     output point
  //! @param theD1    output first derivative
  //! @param theD2    output second derivative
  //! @return true if evaluation succeeded, false if index is invalid
  Standard_EXPORT bool D2(int theIndex, gp_Pnt& theP, gp_Vec& theD1, gp_Vec& theD2) const;

  //! Evaluate all grid points and return as array.
  //! @return array of evaluated points (1-based indexing, size = NbParams()),
  //!         or empty array if evaluation failed
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

private:
  //! Compute parameters aligned with knots and their span indices (single-pass).
  void computeKnotAlignedParams(double theParamMin,
                                double theParamMax,
                                int    theMinSamples,
                                bool   theIncludeEnds);

  //! Compute uniform parameters with their span indices.
  void computeUniformParams(double theParamMin,
                            double theParamMax,
                            int    theNbSamples,
                            bool   theIncludeEnds);

  //! Find span index for a parameter value using binary search.
  int locateSpan(double theParam) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(double theParam, int theHint) const;

  //! Check if index is valid for parameter access.
  bool isValidIndex(int theIndex) const { return theIndex >= 1 && theIndex <= myParams.Length(); }

  //! Ensure cache is valid for the given span index, rebuild if necessary.
  //! @param theSpanIndex span index to validate cache against
  //! @param theParam parameter value within the span (used for cache rebuild)
  void ensureCacheValid(int theSpanIndex, double theParam) const;

private:
  int                           myDegree;
  Handle(TColgp_HArray1OfPnt)   myPoles;
  Handle(TColStd_HArray1OfReal) myWeights;
  Handle(TColStd_HArray1OfReal) myFlatKnots;
  bool                          myRational;
  bool                          myPeriodic;
  bool                          myIsInitialized;

  // Pre-computed parameters with span indices
  NCollection_Array1<ParamWithSpan> myParams;

  // Cache for efficient evaluation within spans
  mutable Handle(BSplCLib_Cache) myCache;
  mutable int                    myCachedSpanIndex;
};

#endif // _BSplCLib_GridEvaluator_HeaderFile

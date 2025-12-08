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

#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

class gp_Pnt;
class gp_Vec;

//! @brief Efficient grid evaluator for BSpline curves.
//!
//! Optimizes evaluation of multiple points on a BSpline curve by
//! pre-computing span indices for parameter arrays. This eliminates
//! the binary search (LocateParameter) overhead when evaluating grid points.
//!
//! The class is designed for scenarios where many points need to be evaluated
//! on a BSpline curve, such as:
//! - Building sampling grids for extrema computation
//! - Tessellation and discretization
//! - Curve analysis
//!
//! Usage:
//! @code
//!   BSplCLib_GridEvaluator anEvaluator;
//!   anEvaluator.Initialize(poles, weights, knots, degree, ...);
//!   anEvaluator.PrepareParamsFromKnots(tMin, tMax, minSamples);
//!
//!   // Evaluate all grid points efficiently - no binary search per point
//!   for (int i = 1; i <= anEvaluator.NbParams(); ++i)
//!       gp_Pnt P = anEvaluator.Value(i);
//! @endcode
class BSplCLib_GridEvaluator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Parameter value with pre-computed span index.
  struct ParamWithSpan
  {
    double Param;     //!< parameter value
    int    SpanIndex; //!< flat knot index identifying the span
  };

  //! Empty constructor.
  Standard_EXPORT BSplCLib_GridEvaluator();

  //! Initialize with BSpline curve data.
  //! @param theDegree    polynomial degree
  //! @param thePoles     array of control points
  //! @param theWeights   array of weights (nullptr for non-rational)
  //! @param theFlatKnots flat knot sequence
  //! @param theRational  true if rational curve
  //! @param thePeriodic  true if periodic curve
  Standard_EXPORT void Initialize(int                        theDegree,
                                  const TColgp_Array1OfPnt&  thePoles,
                                  const TColStd_Array1OfReal* theWeights,
                                  const TColStd_Array1OfReal& theFlatKnots,
                                  bool                       theRational,
                                  bool                       thePeriodic);

  //! Prepare parameters aligned with knots (optimal for BSplines).
  //! Creates sample points within each knot span based on degree.
  //! The span indices are computed during this process - no binary search later.
  //! @param theParamMin   minimum parameter value
  //! @param theParamMax   maximum parameter value
  //! @param theMinSamples minimum number of samples (may increase based on knots)
  Standard_EXPORT void PrepareParamsFromKnots(double theParamMin, double theParamMax, int theMinSamples);

  //! Prepare parameters with uniform distribution.
  //! Span indices are computed for each parameter.
  //! @param theParamMin  minimum parameter value
  //! @param theParamMax  maximum parameter value
  //! @param theNbSamples number of samples
  Standard_EXPORT void PrepareParams(double theParamMin, double theParamMax, int theNbSamples);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Length(); }

  //! Returns parameter data at index (1-based).
  const ParamWithSpan& ParamData(int theIndex) const { return myParams.Value(theIndex); }

  //! Returns parameter value at index (1-based).
  double Param(int theIndex) const { return myParams.Value(theIndex).Param; }

  //! Evaluate point at grid position.
  //! Uses pre-computed span indices - no binary search.
  //! @param theIndex parameter index (1-based)
  //! @return computed point on curve
  Standard_EXPORT gp_Pnt Value(int theIndex) const;

  //! Evaluate point at grid position.
  //! @param theIndex parameter index (1-based)
  //! @param theP     output point
  Standard_EXPORT void D0(int theIndex, gp_Pnt& theP) const;

  //! Evaluate point and first derivative at grid position.
  //! @param theIndex parameter index (1-based)
  //! @param theP     output point
  //! @param theD1    output derivative
  Standard_EXPORT void D1(int theIndex, gp_Pnt& theP, gp_Vec& theD1) const;

  //! Evaluate point and derivatives up to 2nd order at grid position.
  //! @param theIndex parameter index (1-based)
  //! @param theP     output point
  //! @param theD1    output first derivative
  //! @param theD2    output second derivative
  Standard_EXPORT void D2(int theIndex, gp_Pnt& theP, gp_Vec& theD1, gp_Vec& theD2) const;

private:
  //! Compute parameters aligned with knots and their span indices.
  void computeKnotAlignedParams(const TColStd_Array1OfReal&     theFlatKnots,
                                int                             theDegree,
                                bool                            thePeriodic,
                                double                          theParamMin,
                                double                          theParamMax,
                                int                             theMinSamples,
                                NCollection_Array1<ParamWithSpan>& theParams) const;

  //! Compute uniform parameters with their span indices.
  void computeUniformParams(const TColStd_Array1OfReal&     theFlatKnots,
                            int                             theDegree,
                            bool                            thePeriodic,
                            double                          theParamMin,
                            double                          theParamMax,
                            int                             theNbSamples,
                            NCollection_Array1<ParamWithSpan>& theParams) const;

  //! Find span index for a parameter value using binary search.
  int locateSpan(const TColStd_Array1OfReal& theFlatKnots,
                 int                         theDegree,
                 bool                        thePeriodic,
                 double                      theParam) const;

  //! Find span index with a hint for better performance on sorted parameters.
  int locateSpanWithHint(const TColStd_Array1OfReal& theFlatKnots,
                         int                         theDegree,
                         bool                        thePeriodic,
                         double                      theParam,
                         int                         theHint) const;

private:
  // Curve data (stored by pointer)
  int                         myDegree;
  const TColgp_Array1OfPnt*   myPoles;
  const TColStd_Array1OfReal* myWeights;
  const TColStd_Array1OfReal* myFlatKnots;
  bool                        myRational;
  bool                        myPeriodic;
  bool                        myIsInitialized;

  // Pre-computed parameters with span indices
  NCollection_Array1<ParamWithSpan> myParams;
};

#endif // _BSplCLib_GridEvaluator_HeaderFile

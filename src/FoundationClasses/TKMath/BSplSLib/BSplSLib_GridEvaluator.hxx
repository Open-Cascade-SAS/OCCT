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

#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <optional>

class gp_Pnt;
class gp_Vec;

//! @brief Efficient grid evaluator for B-spline and Bezier surfaces.
//!
//! Optimizes evaluation of multiple points on a B-spline surface by
//! pre-computing span indices for parameter arrays. This eliminates
//! the binary search (LocateParameter) overhead when evaluating grid points.
//!
//! The class is designed for scenarios where many points need to be evaluated
//! on a B-spline surface, such as:
//! - Building sampling grids for extrema computation
//! - Tessellation and meshing
//! - Surface analysis and visualization
//!
//! @warning LIFETIME REQUIREMENT: This class stores pointers to external data
//! (poles, weights, flat knots). The caller MUST ensure these arrays remain
//! valid and unchanged for the entire lifetime of the evaluator. Destroying
//! or modifying the source arrays after Initialize() leads to undefined behavior.
//!
//! For Bezier surfaces, use InitializeBezier() which generates internal flat knots.
//!
//! Usage for B-spline:
//! @code
//!   BSplSLib_GridEvaluator anEvaluator;
//!   anEvaluator.Initialize(poles, weights, uKnots, vKnots, ...);
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

  //! Empty constructor.
  Standard_EXPORT BSplSLib_GridEvaluator();

  //! Initialize with B-spline surface data.
  //! @warning The caller must ensure all referenced arrays remain valid
  //!          for the lifetime of this evaluator.
  //! @param theDegreeU     degree in U direction (must be >= 1)
  //! @param theDegreeV     degree in V direction (must be >= 1)
  //! @param thePoles       2D array of control points
  //! @param theWeights     2D array of weights (nullptr for non-rational)
  //! @param theUFlatKnots  flat knot sequence in U direction
  //! @param theVFlatKnots  flat knot sequence in V direction
  //! @param theURational   true if rational in U direction (requires non-null weights)
  //! @param theVRational   true if rational in V direction (requires non-null weights)
  //! @param theUPeriodic   true if periodic in U direction
  //! @param theVPeriodic   true if periodic in V direction
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool Initialize(int                         theDegreeU,
                                  int                         theDegreeV,
                                  const TColgp_Array2OfPnt&   thePoles,
                                  const TColStd_Array2OfReal* theWeights,
                                  const TColStd_Array1OfReal& theUFlatKnots,
                                  const TColStd_Array1OfReal& theVFlatKnots,
                                  bool                        theURational,
                                  bool                        theVRational,
                                  bool                        theUPeriodic,
                                  bool                        theVPeriodic);

  //! Initialize with Bezier surface data.
  //! Internally generates appropriate flat knot vectors.
  //! @param thePoles   2D array of control points (degreeU = NbUPoles - 1, degreeV = NbVPoles - 1)
  //! @param theWeights 2D array of weights (nullptr for non-rational)
  //! @return true if initialization succeeded, false if parameters are invalid
  Standard_EXPORT bool InitializeBezier(const TColgp_Array2OfPnt&   thePoles,
                                        const TColStd_Array2OfReal* theWeights);

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
  Standard_EXPORT void PrepareUParams(double theUMin, double theUMax, int theNbU, bool theIncludeEnds = true);

  //! Prepare V parameters with uniform distribution.
  //! @param theVMin        minimum V parameter
  //! @param theVMax        maximum V parameter
  //! @param theNbV         number of V samples (minimum 2)
  //! @param theIncludeEnds if true, include exact boundary values (default: true)
  Standard_EXPORT void PrepareVParams(double theVMin, double theVMax, int theNbV, bool theIncludeEnds = true);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

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

private:
  //! Compute parameters aligned with knots and their span indices (single-pass).
  //! @param theFlatKnots  flat knot sequence
  //! @param theDegree     polynomial degree
  //! @param theParamMin   minimum parameter value
  //! @param theParamMax   maximum parameter value
  //! @param theMinSamples minimum number of samples
  //! @param theIncludeEnds whether to include exact boundaries
  //! @param theParams     output array of parameters with span indices
  void computeKnotAlignedParams(const TColStd_Array1OfReal&        theFlatKnots,
                                int                                theDegree,
                                bool                               thePeriodic,
                                double                             theParamMin,
                                double                             theParamMax,
                                int                                theMinSamples,
                                bool                               theIncludeEnds,
                                NCollection_Array1<ParamWithSpan>& theParams) const;

  //! Compute uniform parameters with their span indices.
  //! @param theFlatKnots  flat knot sequence
  //! @param theDegree     polynomial degree
  //! @param thePeriodic   periodicity flag
  //! @param theParamMin   minimum parameter value
  //! @param theParamMax   maximum parameter value
  //! @param theNbSamples  number of samples
  //! @param theIncludeEnds whether to include exact boundaries
  //! @param theParams     output array of parameters with span indices
  void computeUniformParams(const TColStd_Array1OfReal&        theFlatKnots,
                            int                                theDegree,
                            bool                               thePeriodic,
                            double                             theParamMin,
                            double                             theParamMax,
                            int                                theNbSamples,
                            bool                               theIncludeEnds,
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

  //! Check if UV indices are valid for parameter access.
  bool areValidIndices(int theIU, int theIV) const
  {
    return theIU >= 1 && theIU <= myUParams.Length() && theIV >= 1 && theIV <= myVParams.Length();
  }

private:
  // Surface data (stored by pointer - caller must ensure lifetime)
  int                         myDegreeU;
  int                         myDegreeV;
  const TColgp_Array2OfPnt*   myPoles;
  const TColStd_Array2OfReal* myWeights;
  const TColStd_Array1OfReal* myUFlatKnots;
  const TColStd_Array1OfReal* myVFlatKnots;
  bool                        myURational;
  bool                        myVRational;
  bool                        myUPeriodic;
  bool                        myVPeriodic;
  bool                        myIsInitialized;

  // Internal flat knots for Bezier surfaces
  TColStd_Array1OfReal myBezierUFlatKnots;
  TColStd_Array1OfReal myBezierVFlatKnots;

  // Pre-computed parameters with span indices
  NCollection_Array1<ParamWithSpan> myUParams;
  NCollection_Array1<ParamWithSpan> myVParams;
};

#endif // _BSplSLib_GridEvaluator_HeaderFile

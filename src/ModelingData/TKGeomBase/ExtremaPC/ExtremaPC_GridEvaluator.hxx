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

#ifndef _ExtremaPC_GridEvaluator_HeaderFile
#define _ExtremaPC_GridEvaluator_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_DistanceFunction.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <math_Vector.hxx>
#include <MathRoot_Multiple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Failure.hxx>

#include <algorithm>
#include <cmath>
#include <optional>

//! @brief Numerical point-curve extrema computation over a cached parameter partition.
//!
//! Each adjacent parameter pair is solved independently, making non-uniform knot- and
//! curvature-aware distributions effective. C2 continuity bounds are inserted into the
//! partition and classified separately from ordinary sampling boundaries.
class ExtremaPC_GridEvaluator
{
public:
  //! Default constructor.
  ExtremaPC_GridEvaluator() = default;

  //! Stores ordered parameter values defining the numerical search partition.
  //! @param[in] theParams ordered parameter values
  void SetParams(const math_Vector& theParams)
  {
    const size_t aNbParams = theParams.Size();
    if (aNbParams == 0)
    {
      myParams.Clear();
      return;
    }

    myParams.Resize(aNbParams);
    for (size_t anIndex = 0; anIndex < aNbParams; ++anIndex)
    {
      myParams[anIndex] = theParams.At(anIndex);
    }
  }

  //! Returns mutable reference to the result for post-processing.
  ExtremaPC::Result& Result() const { return myResult; }

  //! Performs extrema computation using the cached parameter partition.
  [[nodiscard]] const ExtremaPC::Result& Perform(const Adaptor3d_Curve&     theCurve,
                                                 const gp_Pnt&              theP,
                                                 const ExtremaPC::Domain1D& theDomain,
                                                 double                     theTol,
                                                 ExtremaPC::SearchMode      theMode) const
  {
    myResult.Clear();
    if (!theDomain.IsValid() || !ExtremaPC::IsValidTolerance(theTol)
        || !ExtremaPC::IsFinitePoint(theP))
    {
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return myResult;
    }
    if (theDomain.Min == theDomain.Max)
    {
      myResult.Status = ExtremaPC::Status::NoSolution;
      return myResult;
    }
    if (!validateParams(theDomain))
    {
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return myResult;
    }

    const std::optional<double> aConstantDistance =
      constantSquareDistance(theCurve, theP, theDomain);
    if (aConstantDistance.has_value())
    {
      myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aConstantDistance.value();
      return myResult;
    }

    myContinuityBounds.Clear();
    myPartition.Clear();
    buildPartition(theCurve, theDomain, myContinuityBounds, myPartition);

    MathRoot::MultipleConfig aConfig;
    aConfig.NbSamples     = 2;
    aConfig.XTolerance    = ExtremaPC::THE_PARAM_TOLERANCE;
    aConfig.FTolerance    = theTol;
    aConfig.NullTolerance = theTol;

    myRoots.Clear();
    for (size_t anInterval = 0; anInterval + 1 < myPartition.Size(); ++anInterval)
    {
      const double aLower  = myPartition.Value(anInterval);
      const double anUpper = myPartition.Value(anInterval + 1);
      if (anUpper - aLower <= ExtremaPC::THE_PARAM_TOLERANCE)
      {
        continue;
      }
      occ::handle<Adaptor3d_Curve> aLocalCurve;
      try
      {
        aLocalCurve = theCurve.Trim(aLower, anUpper, Precision::PConfusion());
      }
      catch (const Standard_Failure&)
      {
        // Some custom adaptors support evaluation but do not implement Trim().
      }
      const Adaptor3d_Curve&     aCurveForInterval = aLocalCurve.IsNull() ? theCurve : *aLocalCurve;
      ExtremaPC_DistanceFunction aLocalFunction(aCurveForInterval, theP);
      MathRoot::MultipleResult   anIntervalRoots;
      try
      {
        anIntervalRoots =
          MathRoot::FindAllRootsWithDerivative(aLocalFunction, aLower, anUpper, aConfig);
      }
      catch (const Standard_Failure&)
      {
        myResult.Status = ExtremaPC::Status::NumericalError;
        return myResult;
      }
      if (!anIntervalRoots.IsDone())
      {
        myResult.Status = ExtremaPC::Status::NumericalError;
        return myResult;
      }
      if (anIntervalRoots.IsAllNull)
      {
        MathRoot::MultipleConfig aRetryConfig = aConfig;
        aRetryConfig.FTolerance               = std::min(theTol, ExtremaPC::THE_DEFAULT_TOLERANCE);
        aRetryConfig.NullTolerance            = 0.0;
        try
        {
          anIntervalRoots =
            MathRoot::FindAllRootsWithDerivative(aLocalFunction, aLower, anUpper, aRetryConfig);
        }
        catch (const Standard_Failure&)
        {
          myResult.Status = ExtremaPC::Status::NumericalError;
          return myResult;
        }
        if (!anIntervalRoots.IsDone() || anIntervalRoots.IsAllNull)
        {
          myResult.Status = ExtremaPC::Status::NumericalError;
          return myResult;
        }
      }
      for (size_t aRootIndex = 0; aRootIndex < anIntervalRoots.NbRoots(); ++aRootIndex)
      {
        appendRoot(myRoots, RootCandidate{anIntervalRoots[aRootIndex], aLower, anUpper});
      }
    }
    ExtremaPC_DistanceFunction aFunction(theCurve, theP);
    const bool                 isClosed = IsClosedDomain(theCurve, theDomain);
    classifyRoots(theCurve,
                  theP,
                  theDomain,
                  aFunction,
                  myContinuityBounds,
                  myRoots,
                  theTol,
                  theMode);
    classifyJunctions(theCurve,
                      theP,
                      theDomain,
                      aFunction,
                      myContinuityBounds,
                      myPartition,
                      myRoots,
                      isClosed,
                      theTol,
                      theMode);

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC::Status::NoSolution : ExtremaPC::Status::OK;
    return myResult;
  }

  //! Builds a uniform parameter partition.
  static math_Vector BuildUniformParams(double theUMin, double theUMax, size_t theNbSamples)
  {
    Standard_RangeError_Raise_if(theNbSamples < 2, "ExtremaPC_GridEvaluator::BuildUniformParams");
    math_Vector  aParams(theNbSamples);
    const double aStep = (theUMax - theUMin) / static_cast<double>(theNbSamples - 1);
    for (size_t anIndex = 0; anIndex < theNbSamples; ++anIndex)
    {
      aParams.ChangeAt(anIndex) = theUMin + static_cast<double>(anIndex) * aStep;
    }
    aParams.ChangeAt(theNbSamples - 1) = theUMax;
    return aParams;
  }

  //! Builds a deflection-aware partition within each C2-smooth curve interval.
  static math_Vector BuildCurveAwareParams(const Adaptor3d_Curve&     theCurve,
                                           const ExtremaPC::Domain1D& theDomain)
  {
    try
    {
      NCollection_LinearVector<double> aParameters;
      insertParameter(aParameters, theDomain.Min);
      insertParameter(aParameters, theDomain.Max);

      const int                          aNbIntervals = theCurve.NbIntervals(GeomAbs_C2);
      NCollection_LocalArray<double, 32> anIntervalStorage(static_cast<size_t>(aNbIntervals + 1));
      NCollection_Array1<double>         anIntervals(anIntervalStorage[0], 1, aNbIntervals + 1);
      theCurve.Intervals(anIntervals, GeomAbs_C2);
      for (size_t anInterval = 0; anInterval + 1 < anIntervals.Size(); ++anInterval)
      {
        const double aLower  = theDomain.Clamp(anIntervals.At(anInterval));
        const double anUpper = theDomain.Clamp(anIntervals.At(anInterval + 1));
        if (anUpper - aLower <= ExtremaPC::THE_PARAM_TOLERANCE)
        {
          continue;
        }

        insertParameter(aParameters, aLower);
        insertParameter(aParameters, anUpper);
        occ::handle<Adaptor3d_Curve> aLocalCurve =
          theCurve.Trim(aLower, anUpper, Precision::PConfusion());
        if (aLocalCurve.IsNull())
        {
          continue;
        }

        const occ::handle<NCollection_HArray1<double>> aDeflectionParameters =
          Extrema_CurveTool::DeflCurvIntervals(*aLocalCurve);
        if (aDeflectionParameters.IsNull())
        {
          continue;
        }
        for (double aParameter : aDeflectionParameters->Array1())
        {
          insertParameter(aParameters, ExtremaPC::Domain1D(aLower, anUpper).Clamp(aParameter));
        }
      }

      math_Vector aResult(aParameters.Size());
      for (size_t anIndex = 0; anIndex < aParameters.Size(); ++anIndex)
      {
        aResult.ChangeAt(anIndex) = aParameters.Value(anIndex);
      }
      return aResult;
    }
    catch (const Standard_Failure&)
    {
      return BuildUniformParams(theDomain.Min,
                                theDomain.Max,
                                ExtremaPC::THE_OTHER_CURVE_NB_SAMPLES);
    }
  }

  //! Returns true when the domain endpoints represent the same curve point.
  static bool IsClosedDomain(const Adaptor3d_Curve& theCurve, const ExtremaPC::Domain1D& theDomain)
  {
    if (!theDomain.IsFinite() || theDomain.Min == theDomain.Max)
    {
      return false;
    }
    try
    {
      if (theCurve.IsPeriodic()
          && ExtremaPC::IsClosedPeriodicDomain(theDomain,
                                               theCurve.Period(),
                                               ExtremaPC::THE_PARAM_TOLERANCE))
      {
        return true;
      }
      return theCurve.Value(theDomain.Min).SquareDistance(theCurve.Value(theDomain.Max)) == 0.0;
    }
    catch (const Standard_Failure&)
    {
      return false;
    }
  }

private:
  //! Root and the non-uniform cell used to isolate it.
  struct RootCandidate
  {
    double Parameter;
    double LeftBound;
    double RightBound;
  };

  //! Validates the cached partition and its coverage of the requested domain.
  bool validateParams(const ExtremaPC::Domain1D& theDomain) const
  {
    if (myParams.Size() < 2)
    {
      return false;
    }
    if (std::abs(myParams.First() - theDomain.Min) > ExtremaPC::THE_PARAM_TOLERANCE
        || std::abs(myParams.Last() - theDomain.Max) > ExtremaPC::THE_PARAM_TOLERANCE)
    {
      return false;
    }
    for (size_t anIndex = 1; anIndex < myParams.Size(); ++anIndex)
    {
      if (!std::isfinite(myParams[anIndex - 1]) || !std::isfinite(myParams[anIndex])
          || myParams[anIndex] <= myParams[anIndex - 1])
      {
        return false;
      }
    }
    return true;
  }

  //! Returns a constant squared distance only when the curve geometry proves it exactly.
  static std::optional<double> constantSquareDistance(const Adaptor3d_Curve&     theCurve,
                                                      const gp_Pnt&              thePoint,
                                                      const ExtremaPC::Domain1D& theDomain)
  {
    try
    {
      const GeomAbs_CurveType aType = theCurve.GetType();
      if (aType == GeomAbs_Circle)
      {
        const gp_Circ aCircle = theCurve.Circle();
        const gp_Vec  aCenterToPoint(aCircle.Location(), thePoint);
        if (aCenterToPoint.Crossed(gp_Vec(aCircle.Axis().Direction())).SquareMagnitude() == 0.0)
        {
          return thePoint.SquareDistance(theCurve.Value(theDomain.Min));
        }
      }
      else if (aType == GeomAbs_Ellipse)
      {
        const gp_Elips anEllipse = theCurve.Ellipse();
        const gp_Vec   aCenterToPoint(anEllipse.Location(), thePoint);
        if (anEllipse.MajorRadius() == anEllipse.MinorRadius()
            && aCenterToPoint.Crossed(gp_Vec(anEllipse.Axis().Direction())).SquareMagnitude()
                 == 0.0)
        {
          return thePoint.SquareDistance(theCurve.Value(theDomain.Min));
        }
      }

      const occ::handle<Geom_BezierCurve> aBezier =
        aType == GeomAbs_BezierCurve ? theCurve.Bezier() : occ::handle<Geom_BezierCurve>();
      const occ::handle<Geom_BSplineCurve> aBSpline =
        aType == GeomAbs_BSplineCurve ? theCurve.BSpline() : occ::handle<Geom_BSplineCurve>();
      const size_t aNbPoles = static_cast<size_t>(
        !aBezier.IsNull() ? aBezier->NbPoles() : (!aBSpline.IsNull() ? aBSpline->NbPoles() : 0));
      if (aNbPoles > 0)
      {
        const gp_Pnt aReference = !aBezier.IsNull() ? aBezier->Pole(1) : aBSpline->Pole(1);
        for (size_t aPoleIndex = 1; aPoleIndex < aNbPoles; ++aPoleIndex)
        {
          const int    aGeomIndex = static_cast<int>(aPoleIndex + 1);
          const gp_Pnt aPole =
            !aBezier.IsNull() ? aBezier->Pole(aGeomIndex) : aBSpline->Pole(aGeomIndex);
          if (aPole.SquareDistance(aReference) != 0.0)
          {
            return std::nullopt;
          }
        }
        return thePoint.SquareDistance(aReference);
      }
      return std::nullopt;
    }
    catch (const Standard_Failure&)
    {
      return std::nullopt;
    }
  }

  //! Inserts a parameter in ascending order unless it is already present.
  static void insertParameter(NCollection_LinearVector<double>& theParameters, double theParameter)
  {
    size_t aLower  = 0;
    size_t anUpper = theParameters.Size();
    while (aLower < anUpper)
    {
      const size_t aMiddle = aLower + (anUpper - aLower) / 2;
      if (theParameters.Value(aMiddle) < theParameter)
      {
        aLower = aMiddle + 1;
      }
      else
      {
        anUpper = aMiddle;
      }
    }
    if ((aLower < theParameters.Size()
         && std::abs(theParameters.Value(aLower) - theParameter) <= ExtremaPC::THE_PARAM_TOLERANCE)
        || (aLower > 0
            && std::abs(theParameters.Value(aLower - 1) - theParameter)
                 <= ExtremaPC::THE_PARAM_TOLERANCE))
    {
      return;
    }

    if (aLower == theParameters.Size())
    {
      theParameters.Append(theParameter);
    }
    else
    {
      theParameters.InsertBefore(aLower, theParameter);
    }
  }

  //! Combines cached sampling parameters with C2 continuity boundaries.
  void buildPartition(const Adaptor3d_Curve&            theCurve,
                      const ExtremaPC::Domain1D&        theDomain,
                      NCollection_LinearVector<double>& theContinuityBounds,
                      NCollection_LinearVector<double>& thePartition) const
  {
    thePartition.Reserve(myParams.Size());
    for (const double aParameter : myParams)
    {
      thePartition.Append(aParameter);
    }

    try
    {
      const int                          aNbIntervals = theCurve.NbIntervals(GeomAbs_C2);
      NCollection_LocalArray<double, 32> anIntervalStorage(static_cast<size_t>(aNbIntervals + 1));
      NCollection_Array1<double>         anIntervals(anIntervalStorage[0], 1, aNbIntervals + 1);
      thePartition.Reserve(myParams.Size() + static_cast<size_t>(aNbIntervals + 1));
      theContinuityBounds.Reserve(static_cast<size_t>(aNbIntervals + 1));
      theCurve.Intervals(anIntervals, GeomAbs_C2);
      for (size_t anIndex = 0; anIndex < anIntervals.Size(); ++anIndex)
      {
        const double aParameter = theDomain.Clamp(anIntervals.At(anIndex));
        insertParameter(thePartition, aParameter);
        if (aParameter > theDomain.Min + ExtremaPC::THE_PARAM_TOLERANCE
            && aParameter < theDomain.Max - ExtremaPC::THE_PARAM_TOLERANCE)
        {
          insertParameter(theContinuityBounds, aParameter);
        }
      }
    }
    catch (const Standard_Failure&)
    {
      // Cached sampling remains usable for adaptors without interval APIs.
    }
  }

  //! Appends a sorted root, merging the cells of an equivalent boundary root.
  static void appendRoot(NCollection_LinearVector<RootCandidate>& theRoots,
                         const RootCandidate&                     theRoot)
  {
    if (!theRoots.IsEmpty())
    {
      RootCandidate& aLast = theRoots.ChangeValue(theRoots.Size() - 1);
      if (std::abs(aLast.Parameter - theRoot.Parameter) <= ExtremaPC::THE_PARAM_TOLERANCE)
      {
        aLast.LeftBound  = std::min(aLast.LeftBound, theRoot.LeftBound);
        aLast.RightBound = std::max(aLast.RightBound, theRoot.RightBound);
        return;
      }
      if (aLast.Parameter < theRoot.Parameter)
      {
        theRoots.Append(theRoot);
        return;
      }
    }

    // The interval traversal and MathRoot results are ordered. Keep a defensive insertion path
    // for custom or future solvers that violate that contract.
    for (size_t anIndex = 0; anIndex < theRoots.Size(); ++anIndex)
    {
      RootCandidate& anExisting = theRoots.ChangeValue(anIndex);
      if (std::abs(anExisting.Parameter - theRoot.Parameter) <= ExtremaPC::THE_PARAM_TOLERANCE)
      {
        anExisting.LeftBound  = std::min(anExisting.LeftBound, theRoot.LeftBound);
        anExisting.RightBound = std::max(anExisting.RightBound, theRoot.RightBound);
        return;
      }
      if (theRoot.Parameter < anExisting.Parameter)
      {
        theRoots.InsertBefore(anIndex, theRoot);
        return;
      }
    }
    theRoots.Append(theRoot);
  }

  //! Returns the stationarity sign at a probe, treating small values as unresolved.
  static int stationaritySign(ExtremaPC_DistanceFunction& theFunction,
                              double                      theParameter,
                              double                      theTol)
  {
    double aValue = 0.0;
    try
    {
      const double aSignTolerance = std::min(theTol, ExtremaPC::THE_DEFAULT_TOLERANCE);
      if (!theFunction.Value(theParameter, aValue) || std::abs(aValue) <= aSignTolerance)
      {
        return 0;
      }
      return aValue < 0.0 ? -1 : 1;
    }
    catch (const Standard_Failure&)
    {
      return 0;
    }
  }

  //! Adds one classified extremum if it matches the requested mode and is not duplicated.
  void addExtremum(const Adaptor3d_Curve& theCurve,
                   const gp_Pnt&          theP,
                   double                 theParameter,
                   int                    theLeftSign,
                   int                    theRightSign,
                   ExtremaPC::SearchMode  theMode) const
  {
    const bool isMinimum = theLeftSign < 0 && theRightSign > 0;
    const bool isMaximum = theLeftSign > 0 && theRightSign < 0;
    if ((!isMinimum && !isMaximum) || (theMode == ExtremaPC::SearchMode::Min && !isMinimum)
        || (theMode == ExtremaPC::SearchMode::Max && !isMaximum))
    {
      return;
    }

    for (size_t anIndex = 0; anIndex < myResult.Extrema.Size(); ++anIndex)
    {
      if (std::abs(myResult.Extrema.Value(anIndex).Parameter - theParameter)
          <= ExtremaPC::THE_PARAM_TOLERANCE)
      {
        return;
      }
    }

    const gp_Pnt              aCurvePoint = theCurve.Value(theParameter);
    ExtremaPC::ExtremumResult anExtremum;
    anExtremum.Parameter      = theParameter;
    anExtremum.Point          = aCurvePoint;
    anExtremum.SquareDistance = theP.SquareDistance(aCurvePoint);
    anExtremum.IsMinimum      = isMinimum;
    anExtremum.IsMaximum      = isMaximum;
    myResult.Extrema.Append(anExtremum);
  }

  //! Classifies smooth stationary roots within their isolation cells.
  void classifyRoots(const Adaptor3d_Curve&                         theCurve,
                     const gp_Pnt&                                  theP,
                     const ExtremaPC::Domain1D&                     theDomain,
                     ExtremaPC_DistanceFunction&                    theFunction,
                     const NCollection_LinearVector<double>&        theContinuityBounds,
                     const NCollection_LinearVector<RootCandidate>& theRoots,
                     double                                         theTol,
                     ExtremaPC::SearchMode                          theMode) const
  {
    for (size_t aRootIndex = 0; aRootIndex < theRoots.Size(); ++aRootIndex)
    {
      const RootCandidate& aRoot      = theRoots.Value(aRootIndex);
      bool                 isJunction = false;
      for (size_t aJunctionIndex = 0; aJunctionIndex < theContinuityBounds.Size(); ++aJunctionIndex)
      {
        if (std::abs(aRoot.Parameter - theContinuityBounds.Value(aJunctionIndex))
            <= ExtremaPC::THE_PARAM_TOLERANCE)
        {
          isJunction = true;
          break;
        }
      }
      if (isJunction)
      {
        continue;
      }

      const bool isAtFirst = aRoot.Parameter <= theDomain.Min + ExtremaPC::THE_PARAM_TOLERANCE;
      const bool isAtLast  = aRoot.Parameter >= theDomain.Max - ExtremaPC::THE_PARAM_TOLERANCE;
      if (isAtFirst || isAtLast)
      {
        continue;
      }

      double aLeftBound  = aRoot.LeftBound;
      double aRightBound = aRoot.RightBound;
      if (aRootIndex > 0)
      {
        const RootCandidate& aPrevious = theRoots.Value(aRootIndex - 1);
        aLeftBound = std::max(aLeftBound, (aPrevious.Parameter + aRoot.Parameter) * 0.5);
      }
      if (aRootIndex + 1 < theRoots.Size())
      {
        const RootCandidate& aNext = theRoots.Value(aRootIndex + 1);
        aRightBound = std::min(aRightBound, (aRoot.Parameter + aNext.Parameter) * 0.5);
      }
      const int aLeftSign =
        stationaritySign(theFunction, (aLeftBound + aRoot.Parameter) * 0.5, theTol);
      const int aRightSign =
        stationaritySign(theFunction, (aRoot.Parameter + aRightBound) * 0.5, theTol);
      addExtremum(theCurve, theP, aRoot.Parameter, aLeftSign, aRightSign, theMode);
    }
  }

  //! Classifies C2 continuity boundaries and the periodic seam from one-sided values.
  void classifyJunctions(const Adaptor3d_Curve&                         theCurve,
                         const gp_Pnt&                                  theP,
                         const ExtremaPC::Domain1D&                     theDomain,
                         ExtremaPC_DistanceFunction&                    theFunction,
                         const NCollection_LinearVector<double>&        theContinuityBounds,
                         const NCollection_LinearVector<double>&        thePartition,
                         const NCollection_LinearVector<RootCandidate>& theRoots,
                         bool                                           theIsClosed,
                         double                                         theTol,
                         ExtremaPC::SearchMode                          theMode) const
  {
    for (size_t aJunctionIndex = 0; aJunctionIndex < theContinuityBounds.Size(); ++aJunctionIndex)
    {
      const double aJunction   = theContinuityBounds.Value(aJunctionIndex);
      double       aLeftBound  = theDomain.Min;
      double       aRightBound = theDomain.Max;
      for (size_t aParamIndex = 1; aParamIndex < thePartition.Size(); ++aParamIndex)
      {
        if (thePartition.Value(aParamIndex) >= aJunction)
        {
          aLeftBound  = thePartition.Value(aParamIndex - 1);
          aRightBound = thePartition.Value(aParamIndex);
          if (std::abs(aRightBound - aJunction) <= ExtremaPC::THE_PARAM_TOLERANCE
              && aParamIndex + 1 < thePartition.Size())
          {
            aRightBound = thePartition.Value(aParamIndex + 1);
          }
          break;
        }
      }
      for (size_t aRootIndex = 0; aRootIndex < theRoots.Size(); ++aRootIndex)
      {
        const double aRoot = theRoots.Value(aRootIndex).Parameter;
        if (aRoot > aLeftBound + ExtremaPC::THE_PARAM_TOLERANCE
            && aRoot < aJunction - ExtremaPC::THE_PARAM_TOLERANCE)
        {
          aLeftBound = aRoot;
        }
        else if (aRoot > aJunction + ExtremaPC::THE_PARAM_TOLERANCE
                 && aRoot < aRightBound - ExtremaPC::THE_PARAM_TOLERANCE)
        {
          aRightBound = aRoot;
          break;
        }
      }

      const int aLeftSign  = stationaritySign(theFunction, (aLeftBound + aJunction) * 0.5, theTol);
      const int aRightSign = stationaritySign(theFunction, (aJunction + aRightBound) * 0.5, theTol);
      addExtremum(theCurve, theP, aJunction, aLeftSign, aRightSign, theMode);
    }

    if (theIsClosed)
    {
      double aLeftBound  = myParams[myParams.Size() - 2];
      double aRightBound = myParams[1];
      for (size_t aRootIndex = 0; aRootIndex < theRoots.Size(); ++aRootIndex)
      {
        const double aRoot = theRoots.Value(aRootIndex).Parameter;
        if (aRoot > aLeftBound + ExtremaPC::THE_PARAM_TOLERANCE
            && aRoot < theDomain.Max - ExtremaPC::THE_PARAM_TOLERANCE)
        {
          aLeftBound = aRoot;
        }
        if (aRoot > theDomain.Min + ExtremaPC::THE_PARAM_TOLERANCE
            && aRoot < aRightBound - ExtremaPC::THE_PARAM_TOLERANCE)
        {
          aRightBound = aRoot;
        }
      }
      const int aLeftSign =
        stationaritySign(theFunction, (aLeftBound + theDomain.Max) * 0.5, theTol);
      const int aRightSign =
        stationaritySign(theFunction, (theDomain.Min + aRightBound) * 0.5, theTol);
      addExtremum(theCurve, theP, theDomain.Min, aLeftSign, aRightSign, theMode);
    }
  }

private:
  NCollection_LinearVector<double>                myParams; //!< Cached parameter partition
  mutable NCollection_LinearVector<double>        myContinuityBounds; //!< Reusable C2 bounds
  mutable NCollection_LinearVector<double>        myPartition;        //!< Reusable merged partition
  mutable NCollection_LinearVector<RootCandidate> myRoots;            //!< Reusable root candidates
  mutable ExtremaPC::Result                       myResult;           //!< Reusable result
};

#endif // _ExtremaPC_GridEvaluator_HeaderFile

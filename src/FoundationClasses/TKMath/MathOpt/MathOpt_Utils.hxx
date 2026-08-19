// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathOpt_Utils_HeaderFile
#define _MathOpt_Utils_HeaderFile

#include <MathUtils_Config.hxx>
#include <MathUtils_LineSearch.hxx>

#include <algorithm>
#include <cmath>

namespace MathOpt
{
namespace Utils
{

template <typename Function>
MathUtils::Status ValueStatus(Function& theFunc, double theX, double& theValue)
{
  if (!std::isfinite(theX))
  {
    return MathUtils::Status::NumericalError;
  }
  if (!theFunc.Value(theX, theValue))
  {
    return MathUtils::Status::CallbackError;
  }
  return std::isfinite(theValue) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

inline bool IsValidConfig(const MathUtils::Config& theConfig)
{
  return theConfig.MaxIterations > 0 && std::isfinite(theConfig.Tolerance)
         && std::isfinite(theConfig.XTolerance) && std::isfinite(theConfig.FTolerance)
         && std::isfinite(theConfig.StepMin) && theConfig.Tolerance > 0.0
         && theConfig.XTolerance > 0.0 && theConfig.FTolerance > 0.0 && theConfig.StepMin > 0.0;
}

inline bool IsFinite(const math_Vector& theVector)
{
  if (theVector.Size() == 0)
  {
    return false;
  }
  for (size_t anIdx = 0; anIdx < theVector.Size(); ++anIdx)
  {
    if (!std::isfinite(theVector.At(anIdx)))
    {
      return false;
    }
  }
  return true;
}

inline bool IsFinite(const math_Matrix& theMatrix)
{
  for (size_t aRow = 0; aRow < theMatrix.RowSize(); ++aRow)
  {
    for (size_t aCol = 0; aCol < theMatrix.ColSize(); ++aCol)
    {
      if (!std::isfinite(theMatrix.At(aRow, aCol)))
      {
        return false;
      }
    }
  }
  return theMatrix.RowSize() > 0 && theMatrix.ColSize() > 0;
}

inline bool IsValidBounds(const math_Vector& theLowerBounds, const math_Vector& theUpperBounds)
{
  if (theLowerBounds.Size() == 0 || theUpperBounds.Size() != theLowerBounds.Size())
  {
    return false;
  }
  for (size_t anOffset = 0; anOffset < theLowerBounds.Size(); ++anOffset)
  {
    const double aLower  = theLowerBounds.At(anOffset);
    const double anUpper = theUpperBounds.At(anOffset);
    if (!std::isfinite(aLower) || !std::isfinite(anUpper) || aLower >= anUpper
        || !std::isfinite(anUpper - aLower))
    {
      return false;
    }
  }
  return true;
}

inline double Norm(const math_Vector& theVector)
{
  double aNorm = 0.0;
  for (size_t anIdx = 0; anIdx < theVector.Size(); ++anIdx)
  {
    aNorm = std::hypot(aNorm, theVector.At(anIdx));
  }
  return aNorm;
}

template <typename Function>
MathUtils::Status ValueStatus(Function& theFunc, const math_Vector& theX, double& theValue)
{
  if (!IsFinite(theX))
  {
    return MathUtils::Status::NumericalError;
  }
  if (!theFunc.Value(theX, theValue))
  {
    return MathUtils::Status::CallbackError;
  }
  return std::isfinite(theValue) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

template <typename Function>
bool Value(Function& theFunc, const math_Vector& theX, double& theValue)
{
  return ValueStatus(theFunc, theX, theValue) == MathUtils::Status::OK;
}

template <typename Function>
class CheckedFunction
{
public:
  explicit CheckedFunction(Function& theFunction)
      : myFunction(theFunction)
  {
  }

  bool Value(const math_Vector& theX, double& theValue)
  {
    const MathUtils::Status aStatus = Utils::ValueStatus(myFunction, theX, theValue);
    const bool              isValid = aStatus == MathUtils::Status::OK;
    HadFailure                      = HadFailure || !isValid;
    HasValidEvaluation              = HasValidEvaluation || isValid;
    if (!isValid && FailureStatus == MathUtils::Status::OK)
    {
      FailureStatus = aStatus;
    }
    return isValid;
  }

  Function& Original() { return myFunction; }

  bool              HadFailure         = false;
  bool              HasValidEvaluation = false;
  MathUtils::Status FailureStatus      = MathUtils::Status::OK;

private:
  Function& myFunction;
};

template <typename Function>
MathUtils::Status GradientStatus(Function&          theFunc,
                                 const math_Vector& theX,
                                 math_Vector&       theGradient)
{
  if (!IsFinite(theX))
  {
    return MathUtils::Status::NumericalError;
  }
  if (!theFunc.Gradient(theX, theGradient))
  {
    return MathUtils::Status::CallbackError;
  }
  return IsFinite(theGradient) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

template <typename Function>
bool Gradient(Function& theFunc, const math_Vector& theX, math_Vector& theGradient)
{
  return GradientStatus(theFunc, theX, theGradient) == MathUtils::Status::OK;
}

template <typename Function>
MathUtils::Status HessianStatus(Function& theFunc, const math_Vector& theX, math_Matrix& theHessian)
{
  if (!IsFinite(theX))
  {
    return MathUtils::Status::NumericalError;
  }
  if (!theFunc.Hessian(theX, theHessian))
  {
    return MathUtils::Status::CallbackError;
  }
  return IsFinite(theHessian) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

template <typename Function>
bool Hessian(Function& theFunc, const math_Vector& theX, math_Matrix& theHessian)
{
  return HessianStatus(theFunc, theX, theHessian) == MathUtils::Status::OK;
}

inline bool IsLineSearchEvaluationError(const MathUtils::LineSearchResult& theResult)
{
  return theResult.Status == MathUtils::Status::CallbackError
         || theResult.Status == MathUtils::Status::NumericalError;
}

inline MathUtils::Status LineSearchFailureStatus(const MathUtils::LineSearchResult& theResult)
{
  return IsLineSearchEvaluationError(theResult) ? theResult.Status
                                                : MathUtils::Status::NotConverged;
}

inline MathUtils::Status LineSearchFailureStatus(
  const MathUtils::LineSearchResult& theInitialResult,
  const MathUtils::LineSearchResult& theFallbackResult)
{
  return IsLineSearchEvaluationError(theFallbackResult) ? theFallbackResult.Status
                                                        : LineSearchFailureStatus(theInitialResult);
}

//! Adapter preventing a wrapped callback from observing coordinates outside box constraints.
template <typename Function>
class BoundedFunction
{
public:
  BoundedFunction(Function&          theFunction,
                  const math_Vector& theLowerBounds,
                  const math_Vector& theUpperBounds)
      : myFunction(theFunction),
        myLowerBounds(theLowerBounds),
        myUpperBounds(theUpperBounds)
  {
  }

  bool Value(const math_Vector& theX, double& theValue)
  {
    math_Vector aBoundedX = bounded(theX);
    return myFunction.Value(aBoundedX, theValue);
  }

private:
  math_Vector bounded(const math_Vector& theX) const
  {
    math_Vector aResult = theX;
    for (size_t anIdx = 0; anIdx < aResult.Size(); ++anIdx)
    {
      aResult.ChangeAt(anIdx) =
        std::max(myLowerBounds.At(anIdx), std::min(myUpperBounds.At(anIdx), aResult.At(anIdx)));
    }
    return aResult;
  }

private:
  Function&          myFunction;
  const math_Vector& myLowerBounds;
  const math_Vector& myUpperBounds;
};

//! Checks whether coordinate or pairwise-diagonal perturbations improve a point.
//! @param theFunc function to evaluate
//! @param theX point being checked
//! @param theFx function value at the point
//! @param theConfig solver tolerances controlling probe size and sufficient decrease
//! @param[out] theEvaluationStatus status of the last callback evaluation
//! @return true if no tested perturbation decreases the function sufficiently
template <typename Function>
bool IsStationary(Function&                theFunc,
                  const math_Vector&       theX,
                  double                   theFx,
                  const MathUtils::Config& theConfig,
                  MathUtils::Status&       theEvaluationStatus)
{
  const size_t aNbDims = theX.Size();
  math_Vector  aProbe  = theX;
  math_Vector  aSteps(aNbDims);
  const double aDecreaseTol = theConfig.FTolerance * (1.0 + std::abs(theFx));

  for (size_t anIdx = 0; anIdx < aNbDims; ++anIdx)
  {
    aSteps.ChangeAt(anIdx) =
      std::max(theConfig.XTolerance,
               2.0 * std::sqrt(theConfig.FTolerance) * (1.0 + std::abs(theX.At(anIdx))));
    for (int aSign = -1; aSign <= 1; aSign += 2)
    {
      aProbe.ChangeAt(anIdx) = theX.At(anIdx) + aSign * aSteps.At(anIdx);
      double aProbeValue     = 0.0;
      theEvaluationStatus    = ValueStatus(theFunc, aProbe, aProbeValue);
      if (theEvaluationStatus != MathUtils::Status::OK || aProbeValue < theFx - aDecreaseTol)
      {
        return false;
      }
    }
    aProbe.ChangeAt(anIdx) = theX.At(anIdx);
  }

  // Coordinate probes cannot detect negative curvature such as f(x,y) = x*y at the origin.
  for (size_t aFirstIdx = 0; aFirstIdx < aNbDims; ++aFirstIdx)
  {
    for (size_t aSecondIdx = aFirstIdx + 1; aSecondIdx < aNbDims; ++aSecondIdx)
    {
      for (int aFirstSign = -1; aFirstSign <= 1; aFirstSign += 2)
      {
        for (int aSecondSign = -1; aSecondSign <= 1; aSecondSign += 2)
        {
          aProbe.ChangeAt(aFirstIdx) =
            theX.At(aFirstIdx) + aFirstSign * aSteps.At(aFirstIdx);
          aProbe.ChangeAt(aSecondIdx) =
            theX.At(aSecondIdx) + aSecondSign * aSteps.At(aSecondIdx);
          double aProbeValue  = 0.0;
          theEvaluationStatus = ValueStatus(theFunc, aProbe, aProbeValue);
          if (theEvaluationStatus != MathUtils::Status::OK || aProbeValue < theFx - aDecreaseTol)
          {
            return false;
          }
        }
      }
      aProbe.ChangeAt(aFirstIdx)  = theX.At(aFirstIdx);
      aProbe.ChangeAt(aSecondIdx) = theX.At(aSecondIdx);
    }
  }
  return true;
}

template <typename Function>
MathUtils::LineSearchResult Backtrack(Function&          theFunc,
                                      const math_Vector& theX,
                                      const math_Vector& theDirection,
                                      const math_Vector& theGradient,
                                      double             theFx,
                                      double             theInitialStep,
                                      double             theStepMin)
{
  MathUtils::LineSearchResult aResult;
  if (!IsFinite(theX) || !IsFinite(theDirection) || !IsFinite(theGradient) || !std::isfinite(theFx)
      || !std::isfinite(theInitialStep) || theInitialStep <= 0.0)
  {
    return aResult;
  }

  double aDirectionalDerivative = 0.0;
  for (size_t anIdx = 0; anIdx < theX.Size(); ++anIdx)
  {
    aDirectionalDerivative += theGradient.At(anIdx) * theDirection.At(anIdx);
  }
  if (!std::isfinite(aDirectionalDerivative) || aDirectionalDerivative >= 0.0)
  {
    return aResult;
  }

  CheckedFunction<Function> aCheckedFunc(theFunc);
  aResult = MathUtils::ArmijoBacktrack(aCheckedFunc,
                                       theX,
                                       theDirection,
                                       theGradient,
                                       theFx,
                                       theInitialStep,
                                       1.0e-4,
                                       0.5,
                                       50);
  if (aCheckedFunc.FailureStatus != MathUtils::Status::OK)
  {
    aResult.Status = aCheckedFunc.FailureStatus;
  }
  if (aCheckedFunc.HadFailure || !aResult.IsValid || !std::isfinite(aResult.Alpha)
      || !std::isfinite(aResult.FNew) || aResult.Alpha < theStepMin
      || aResult.FNew > theFx + 1.0e-4 * aResult.Alpha * aDirectionalDerivative)
  {
    aResult.IsValid = false;
  }
  return aResult;
}

} // namespace Utils
} // namespace MathOpt

#endif // _MathOpt_Utils_HeaderFile

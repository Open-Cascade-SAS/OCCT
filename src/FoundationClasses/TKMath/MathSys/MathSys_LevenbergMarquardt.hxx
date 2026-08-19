// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathSys_LevenbergMarquardt_HeaderFile
#define _MathSys_LevenbergMarquardt_HeaderFile

#include <MathSys_LinearAlgebra.hxx>
#include <MathSys_Types.hxx>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace MathSys
{

//! Configuration for the scaled, gain-ratio Levenberg-Marquardt algorithm.
struct LMConfig : MathUtils::Config
{
  double LambdaInit     = 1.0e-3;
  double LambdaIncrease = 10.0;
  double LambdaDecrease = 0.1;
  double LambdaMax      = 1.0e16;
  double LambdaMin      = 1.0e-15;
  double RankTolerance  = Utils::THE_RANK_TOL;

  LMConfig() = default;

  explicit LMConfig(double theTolerance, uint32_t theMaxIter = 100)
      : MathUtils::Config(theTolerance, theMaxIter)
  {
  }
};

namespace Utils
{

//! Maximum damping retries performed during one LM iteration.
inline constexpr uint32_t THE_LM_DAMPING_MAX = 24;

template <typename FuncSetType>
inline bool FinishLM(FuncSetType&       theFunc,
                     const math_Vector& theSolution,
                     size_t             theNbEquations,
                     const LMConfig&    theConfig,
                     double             theInitialResidualNorm,
                     const math_Vector* theInfBound,
                     const math_Vector* theSupBound,
                     SystemResult&      theResult)
{
  math_Vector aResidual(theNbEquations, 0.0);
  math_Matrix aJacobian(theNbEquations, theSolution.Size(), 0.0);
  theResult.Solution = theSolution;
  if (!theFunc.Value(theSolution, aResidual))
  {
    theResult.Status = MathUtils::Status::CallbackError;
    return false;
  }
  if (!IsFinite(aResidual))
  {
    theResult.Status = MathUtils::Status::NumericalError;
    return false;
  }
  if (!theFunc.Derivatives(theSolution, aJacobian))
  {
    theResult.Status = MathUtils::Status::CallbackError;
    return false;
  }
  if (!IsFinite(aJacobian))
  {
    theResult.Status = MathUtils::Status::NumericalError;
    return false;
  }

  const std::optional<size_t> aRank = ScaledRank(aJacobian, theConfig.RankTolerance);
  if (!aRank.has_value())
  {
    theResult.Status = MathUtils::Status::NumericalError;
    return false;
  }
  theResult.Gradient = math_Vector(theSolution.Size(), 0.0);
  for (size_t aCol = 0; aCol < theSolution.Size(); ++aCol)
  {
    for (size_t aRow = 0; aRow < theNbEquations; ++aRow)
    {
      theResult.Gradient->ChangeAt(aCol) += aJacobian.At(aRow, aCol) * aResidual.At(aRow);
    }
  }
  theResult.ResidualNorm        = SafeNorm(aResidual);
  const double aSquaredResidual = theResult.ResidualNorm * theResult.ResidualNorm;
  if (std::isfinite(aSquaredResidual))
  {
    theResult.Value = aSquaredResidual;
  }
  if (!IsFinite(*theResult.Gradient))
  {
    theResult.Status = MathUtils::Status::NumericalError;
    return false;
  }
  double aProjectedGradientNorm = 0.0;
  for (size_t anOffset = 0; anOffset < theSolution.Size(); ++anOffset)
  {
    double aComponent = theResult.Gradient->At(anOffset);
    if (theInfBound != nullptr && theSupBound != nullptr
        && ((theSolution.At(anOffset) <= theInfBound->At(anOffset) && aComponent > 0.0)
            || (theSolution.At(anOffset) >= theSupBound->At(anOffset) && aComponent < 0.0)))
    {
      aComponent = 0.0;
    }
    aProjectedGradientNorm = std::hypot(aProjectedGradientNorm, aComponent);
  }
  const double aResidualTolerance =
    theConfig.FTolerance + theConfig.RelativeTolerance * theInitialResidualNorm;
  theResult.Jacobian     = aJacobian;
  theResult.Rank         = *aRank;
  theResult.IsRoot       = theResult.ResidualNorm <= aResidualTolerance;
  theResult.IsStationary = aProjectedGradientNorm <= theConfig.Tolerance;
  return true;
}

template <typename FuncSetType>
SystemResult LMImpl(FuncSetType&       theFunc,
                    const math_Vector& theStart,
                    const LMConfig&    theConfig,
                    const math_Vector* theInfBound,
                    const math_Vector* theSupBound)
{
  SystemResult aResult;
  const bool   hasBounds = theInfBound != nullptr && theSupBound != nullptr;
  if (theFunc.NbVariables() <= 0 || theFunc.NbEquations() <= 0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  const size_t aN = static_cast<size_t>(theFunc.NbVariables());
  const size_t aM = static_cast<size_t>(theFunc.NbEquations());
  if (theStart.Size() != aN || !IsFinite(theStart) || theConfig.MaxIterations == 0
      || !std::isfinite(theConfig.Tolerance) || !(theConfig.Tolerance > 0.0)
      || !std::isfinite(theConfig.XTolerance) || !(theConfig.XTolerance > 0.0)
      || !std::isfinite(theConfig.FTolerance) || !(theConfig.FTolerance > 0.0)
      || !std::isfinite(theConfig.RelativeTolerance) || theConfig.RelativeTolerance < 0.0
      || !std::isfinite(theConfig.StepMin) || !(theConfig.StepMin > 0.0)
      || !std::isfinite(theConfig.LambdaInit) || !(theConfig.LambdaInit > 0.0)
      || !std::isfinite(theConfig.LambdaIncrease) || !(theConfig.LambdaIncrease > 1.0)
      || !std::isfinite(theConfig.LambdaDecrease) || !(theConfig.LambdaDecrease > 0.0)
      || !(theConfig.LambdaDecrease < 1.0) || !std::isfinite(theConfig.LambdaMin)
      || !(theConfig.LambdaMin > 0.0) || !(theConfig.LambdaMin <= theConfig.LambdaInit)
      || !std::isfinite(theConfig.LambdaMax) || !(theConfig.LambdaInit <= theConfig.LambdaMax)
      || !std::isfinite(theConfig.RankTolerance)
      || !(theConfig.RankTolerance > 0.0)
      || (hasBounds
          && (theInfBound->Size() != aN || theSupBound->Size() != aN || !IsFinite(*theInfBound)
              || !IsFinite(*theSupBound))))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  math_Vector aSolution(aN);
  for (size_t anOffset = 0; anOffset < aN; ++anOffset)
  {
    aSolution.ChangeAt(anOffset) = theStart.At(anOffset);
    if (hasBounds)
    {
      const double aMin = theInfBound->At(anOffset);
      const double aMax = theSupBound->At(anOffset);
      if (aMin > aMax)
      {
        aResult.Status = MathUtils::Status::InvalidInput;
        return aResult;
      }
      aSolution.ChangeAt(anOffset) = std::clamp(aSolution.At(anOffset), aMin, aMax);
    }
  }

  math_Vector aResidual(aM, 0.0);
  math_Vector aTrialResidual(aM, 0.0);
  math_Matrix aJacobian(aM, aN, 0.0);
  if (!theFunc.Value(aSolution, aResidual))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!IsFinite(aResidual))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  double            aLambda =
    std::clamp(theConfig.LambdaInit, theConfig.LambdaMin, theConfig.LambdaMax);
  const double      aResidualNorm = SafeNorm(aResidual);
  long double       aMerit        = 0.5L * aResidualNorm * aResidualNorm;
  uint32_t          anIter = 0;
  for (; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter;
    if (!theFunc.Derivatives(aSolution, aJacobian))
    {
      aResult.Status = MathUtils::Status::CallbackError;
      break;
    }
    if (!IsFinite(aJacobian))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      break;
    }

    math_Vector aGradient(aN, 0.0);
    for (size_t aRow = 0; aRow < aM; ++aRow)
    {
      for (size_t aCol = 0; aCol < aN; ++aCol)
      {
        aGradient.ChangeAt(aCol) += aJacobian.At(aRow, aCol) * aResidual.At(aRow);
      }
    }
    if (!IsFinite(aGradient))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      break;
    }

    double aProjectedGradientNorm = 0.0;
    for (size_t anOffset = 0; anOffset < aN; ++anOffset)
    {
      double aComponent = aGradient.At(anOffset);
      if (hasBounds)
      {
        const double aMin = theInfBound->At(anOffset);
        const double aMax = theSupBound->At(anOffset);
        if ((aSolution.At(anOffset) <= aMin && aComponent > 0.0)
            || (aSolution.At(anOffset) >= aMax && aComponent < 0.0))
        {
          aComponent = 0.0;
        }
      }
      aProjectedGradientNorm = std::hypot(aProjectedGradientNorm, aComponent);
    }
    if (aProjectedGradientNorm <= theConfig.Tolerance)
    {
      aResult.Status       = MathUtils::Status::OK;
      aResult.IsStationary = true;
      break;
    }

    bool isAccepted = false;
    for (uint32_t aDampingIter = 0; aDampingIter < THE_LM_DAMPING_MAX; ++aDampingIter)
    {
      math_Matrix anAugmented(aM + aN, aN, 0.0);
      math_Vector anAugmentedRhs(aM + aN, 0.0);
      for (size_t aRow = 0; aRow < aM; ++aRow)
      {
        anAugmentedRhs.ChangeAt(aRow) = -aResidual.At(aRow);
        for (size_t aCol = 0; aCol < aN; ++aCol)
        {
          anAugmented.ChangeAt(aRow, aCol) = aJacobian.At(aRow, aCol);
        }
      }
      for (size_t aCol = 0; aCol < aN; ++aCol)
      {
        double aColumnNorm = 0.0;
        for (size_t aRow = 0; aRow < aM; ++aRow)
        {
          aColumnNorm = std::hypot(aColumnNorm, aJacobian.At(aRow, aCol));
        }
        anAugmented.ChangeAt(aM + aCol, aCol) = std::sqrt(aLambda) * std::max(aColumnNorm, 1.0);
      }

      const LinearStep aStepResult =
        SolveLinearized(anAugmented, anAugmentedRhs, theConfig.RankTolerance);
      if (!aStepResult.Solution.has_value())
      {
        if (aLambda >= theConfig.LambdaMax)
        {
          break;
        }
        aLambda = std::min(theConfig.LambdaMax, aLambda * theConfig.LambdaIncrease);
        continue;
      }
      const math_Vector& aStep = *aStepResult.Solution;
      math_Vector        aTrial(aN, 0.0);
      for (size_t anOffset = 0; anOffset < aN; ++anOffset)
      {
        aTrial.ChangeAt(anOffset) = aSolution.At(anOffset) + aStep.At(anOffset);
        if (hasBounds)
        {
          aTrial.ChangeAt(anOffset) =
            std::clamp(aTrial.At(anOffset), theInfBound->At(anOffset), theSupBound->At(anOffset));
        }
      }
      if (!IsFinite(aTrial))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        break;
      }
      if (!theFunc.Value(aTrial, aTrialResidual))
      {
        aResult.Status = MathUtils::Status::CallbackError;
        break;
      }
      if (!IsFinite(aTrialResidual))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        break;
      }

      const double      aTrialNorm  = SafeNorm(aTrialResidual);
      const long double aTrialMerit = 0.5L * aTrialNorm * aTrialNorm;
      long double       aPredicted  = 0.0L;
      for (size_t aCol = 0; aCol < aN; ++aCol)
      {
        const double aProjectedStep = aTrial.At(aCol) - aSolution.At(aCol);
        aPredicted -= static_cast<long double>(aGradient.At(aCol)) * aProjectedStep;
      }
      double aLinearNorm = 0.0;
      for (size_t aRow = 0; aRow < aM; ++aRow)
      {
        double aValue = 0.0;
        for (size_t aCol = 0; aCol < aN; ++aCol)
        {
          aValue += aJacobian.At(aRow, aCol) * (aTrial.At(aCol) - aSolution.At(aCol));
        }
        aLinearNorm = std::hypot(aLinearNorm, aValue);
      }
      aPredicted -= 0.5L * aLinearNorm * aLinearNorm;
      const long double aRho =
        (aPredicted > 0.0L) ? (aMerit - aTrialMerit) / aPredicted : -1.0L;
      if (aRho > 0.0 && aTrialMerit < aMerit)
      {
        aResult.StepNorm = 0.0;
        for (size_t aCol = 0; aCol < aN; ++aCol)
        {
          aResult.StepNorm = std::hypot(aResult.StepNorm, aTrial.At(aCol) - aSolution.At(aCol));
        }
        aSolution = aTrial;
        aResidual = aTrialResidual;
        aMerit    = aTrialMerit;
        aLambda =
          std::max(theConfig.LambdaMin, aLambda * ((aRho > 0.75) ? theConfig.LambdaDecrease : 1.0));
        isAccepted = true;
        break;
      }
      if (aLambda >= theConfig.LambdaMax)
      {
        break;
      }
      aLambda =
        std::min(theConfig.LambdaMax, aLambda * ((aRho < 0.25) ? theConfig.LambdaIncrease : 2.0));
    }

    aResult.NbIterations = anIter + 1;
    if (aResult.Status == MathUtils::Status::CallbackError
        || aResult.Status == MathUtils::Status::NumericalError)
    {
      break;
    }
    if (!isAccepted)
    {
      aResult.Status = MathUtils::Status::NotConverged;
      break;
    }
    const double aCurrentResidualNorm = SafeNorm(aResidual);
    double       aSolutionScale       = 1.0;
    for (size_t anOffset = 0; anOffset < aN; ++anOffset)
    {
      aSolutionScale = std::max(aSolutionScale, std::abs(aSolution.At(anOffset)));
    }
    const double aStepTolerance =
      std::max(theConfig.StepMin,
               theConfig.XTolerance + theConfig.RelativeTolerance * aSolutionScale);
    if (aResult.StepNorm <= aStepTolerance)
    {
      aResult.Status = (aCurrentResidualNorm <= theConfig.FTolerance)
                         ? MathUtils::Status::OK
                         : MathUtils::Status::NotConverged;
      break;
    }
  }

  if (aResult.Status == MathUtils::Status::NotConverged && anIter == theConfig.MaxIterations)
  {
    aResult.Status = MathUtils::Status::MaxIterations;
  }
  if (aResult.Status == MathUtils::Status::CallbackError
      || aResult.Status == MathUtils::Status::NumericalError)
  {
    aResult.Solution = aSolution;
    return aResult;
  }
  if (FinishLM(theFunc,
               aSolution,
               aM,
               theConfig,
               aResidualNorm,
               theInfBound,
               theSupBound,
               aResult)
       && (aResult.IsRoot || aResult.IsStationary))
  {
    aResult.Status = MathUtils::Status::OK;
  }
  else if (aResult.Status == MathUtils::Status::OK)
  {
    aResult.Status = MathUtils::Status::NotConverged;
  }
  return aResult;
}

} // namespace Utils

//! Minimize the squared residual norm using scaled Levenberg-Marquardt.
template <typename FuncSetType>
SystemResult LevenbergMarquardt(FuncSetType&       theFunc,
                                const math_Vector& theStart,
                                const LMConfig&    theConfig = LMConfig())
{
  return Utils::LMImpl(theFunc, theStart, theConfig, nullptr, nullptr);
}

//! Minimize the squared residual norm subject to box constraints.
template <typename FuncSetType>
SystemResult LevenbergMarquardtBounded(FuncSetType&       theFunc,
                                       const math_Vector& theStart,
                                       const math_Vector& theInfBound,
                                       const math_Vector& theSupBound,
                                       const LMConfig&    theConfig = LMConfig())
{
  return Utils::LMImpl(theFunc, theStart, theConfig, &theInfBound, &theSupBound);
}

} // namespace MathSys

#endif // _MathSys_LevenbergMarquardt_HeaderFile

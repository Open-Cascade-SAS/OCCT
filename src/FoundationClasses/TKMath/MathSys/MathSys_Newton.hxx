// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathSys_Newton_HeaderFile
#define _MathSys_Newton_HeaderFile

#include <MathSys_LinearAlgebra.hxx>
#include <MathSys_Types.hxx>

#include <NCollection_LinearVector.hxx>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace MathSys
{
namespace Utils
{

template <typename FuncSetType>
inline bool FinalizeSystemResult(FuncSetType&       theFunc,
                                 const math_Vector& theSolution,
                                 size_t             theNbEquations,
                                 double             theFTolerance,
                                 SystemResult&      theResult)
{
  math_Vector aResidual(theNbEquations, 0.0);
  math_Matrix aJacobian(theNbEquations, theSolution.Size(), 0.0);
  theResult.Solution = theSolution;
  if (!theFunc.Values(theSolution, aResidual, aJacobian))
  {
    theResult.Status = MathUtils::Status::CallbackError;
    return false;
  }
  if (!IsFinite(aResidual) || !IsFinite(aJacobian))
  {
    theResult.Status = MathUtils::Status::NumericalError;
    return false;
  }

  const std::optional<size_t> aRank = ScaledRank(aJacobian);
  if (!aRank.has_value())
  {
    theResult.Status = MathUtils::Status::NumericalError;
    return false;
  }
  theResult.ResidualNorm        = SafeNorm(aResidual);
  const double aSquaredResidual = theResult.ResidualNorm * theResult.ResidualNorm;
  if (std::isfinite(aSquaredResidual))
  {
    theResult.Value = aSquaredResidual;
  }
  else
  {
    theResult.Value.reset();
  }
  theResult.Jacobian = aJacobian;
  theResult.Rank     = *aRank;
  theResult.IsRoot   = theResult.ResidualNorm <= theFTolerance;
  return true;
}

template <typename FuncSetType>
SystemResult NewtonImpl(FuncSetType&       theFunc,
                        const math_Vector& theStart,
                        const math_Vector& theTolX,
                        double             theTolF,
                        uint32_t           theMaxIter,
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

  if (theStart.Size() != aN || theTolX.Size() != aN || !(theTolF > 0.0) || !std::isfinite(theTolF)
      || theMaxIter == 0 || !IsFinite(theStart) || !IsFinite(theTolX)
      || (hasBounds
          && (theInfBound->Size() != aN || theSupBound->Size() != aN || !IsFinite(*theInfBound)
              || !IsFinite(*theSupBound))))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  for (size_t anOffset = 0; anOffset < aN; ++anOffset)
  {
    if (!(theTolX.At(anOffset) > 0.0)
        || (hasBounds && theInfBound->At(anOffset) > theSupBound->At(anOffset)))
    {
      aResult.Status = MathUtils::Status::InvalidInput;
      return aResult;
    }
  }

  math_Vector aSolution(aN);
  for (size_t anOffset = 0; anOffset < aN; ++anOffset)
  {
    aSolution.ChangeAt(anOffset) = theStart.At(anOffset);
    if (hasBounds)
    {
      aSolution.ChangeAt(anOffset) =
        std::clamp(aSolution.At(anOffset), theInfBound->At(anOffset), theSupBound->At(anOffset));
    }
  }

  math_Vector aResidual(aM, 0.0);
  math_Vector aTrialResidual(aM, 0.0);
  math_Matrix aJacobian(aM, aN, 0.0);
  bool        hasTakenStep      = false;
  bool        isLastStepWithinX = false;
  for (uint32_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    aResult.NbIterations = anIter;
    if (!theFunc.Values(aSolution, aResidual, aJacobian))
    {
      aResult.Status   = MathUtils::Status::CallbackError;
      aResult.Solution = aSolution;
      return aResult;
    }
    if (!IsFinite(aResidual) || !IsFinite(aJacobian))
    {
      aResult.Status   = MathUtils::Status::NumericalError;
      aResult.Solution = aSolution;
      return aResult;
    }

    const double aResidualNorm = SafeNorm(aResidual);
    if (aResidualNorm <= theTolF && (!hasTakenStep || isLastStepWithinX))
    {
      aResult.Status = MathUtils::Status::OK;
      if (!FinalizeSystemResult(theFunc, aSolution, aM, theTolF, aResult))
      {
        return aResult;
      }
      if (!aResult.IsRoot)
      {
        aResult.Status = MathUtils::Status::NotConverged;
      }
      return aResult;
    }

    math_Vector aRightHandSide(aM, 0.0);
    math_Vector aGradient(aN, 0.0);
    for (size_t aRow = 0; aRow < aM; ++aRow)
    {
      aRightHandSide.ChangeAt(aRow) = -aResidual.At(aRow);
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

    LinearStep aLinearStep = SolveLinearized(aJacobian, aRightHandSide);
    aResult.Rank           = aLinearStep.Rank;
    if (!aLinearStep.Solution.has_value())
    {
      aResult.Status = aLinearStep.Status;
      break;
    }
    math_Vector aDirection      = *aLinearStep.Solution;
    math_Vector aSearchGradient = aGradient;

    if (hasBounds)
    {
      NCollection_LinearVector<size_t> aFreeVariables;
      for (size_t aVariable = 0; aVariable < aN; ++aVariable)
      {
        const bool isBlocked =
          (aSolution.At(aVariable) <= theInfBound->At(aVariable)
           && aGradient.At(aVariable) >= 0.0)
          || (aSolution.At(aVariable) >= theSupBound->At(aVariable)
              && aGradient.At(aVariable) <= 0.0)
          || theInfBound->At(aVariable) == theSupBound->At(aVariable);
        if (isBlocked)
        {
          aSearchGradient.ChangeAt(aVariable) = 0.0;
        }
        else
        {
          aFreeVariables.Append(aVariable);
        }
      }

      if (aFreeVariables.Size() < aN)
      {
        if (aFreeVariables.IsEmpty())
        {
          aResult.Status = MathUtils::Status::NotConverged;
          break;
        }
        math_Matrix aReducedJacobian(aM, aFreeVariables.Size(), 0.0);
        for (size_t aRow = 0; aRow < aM; ++aRow)
        {
          for (size_t aFree = 0; aFree < aFreeVariables.Size(); ++aFree)
          {
            aReducedJacobian.ChangeAt(aRow, aFree) =
              aJacobian.At(aRow, aFreeVariables.Value(aFree));
          }
        }
        const LinearStep aReducedStep = SolveLinearized(aReducedJacobian, aRightHandSide);
        aDirection.Init(0.0);
        if (aReducedStep.Solution.has_value())
        {
          for (size_t aFree = 0; aFree < aFreeVariables.Size(); ++aFree)
          {
            aDirection.ChangeAt(aFreeVariables.Value(aFree)) = aReducedStep.Solution->At(aFree);
          }
        }
        else
        {
          for (size_t aVariable = 0; aVariable < aN; ++aVariable)
          {
            aDirection.ChangeAt(aVariable) = -aSearchGradient.At(aVariable);
          }
        }
      }
    }

    bool isConverged = true;
    for (size_t anOffset = 0; anOffset < aN; ++anOffset)
    {
      isConverged = isConverged && std::abs(aDirection.At(anOffset)) <= theTolX.At(anOffset);
    }
    for (size_t anEquation = 0; anEquation < aM; ++anEquation)
    {
      isConverged = isConverged && std::abs(aResidual.At(anEquation)) <= theTolF;
    }
    if (isConverged)
    {
      isLastStepWithinX = true;
      aResult.Status    = MathUtils::Status::OK;
      break;
    }

    double aDirectionalDerivative = 0.0;
    for (size_t aCol = 0; aCol < aN; ++aCol)
    {
      aDirectionalDerivative += aSearchGradient.At(aCol) * aDirection.At(aCol);
    }
    if (!(aDirectionalDerivative < 0.0))
    {
      const double aGradientNorm = SafeNorm(aSearchGradient);
      if (!(aGradientNorm > 0.0))
      {
        if (aResidualNorm <= theTolF)
        {
          aResult.NbIterations = anIter + 1;
          aResult.StepNorm     = 0.0;
          hasTakenStep         = true;
          isLastStepWithinX    = true;
          aResult.Status       = MathUtils::Status::OK;
          break;
        }
        aResult.Status = (aResult.Rank < std::min(aM, aN)) ? MathUtils::Status::Singular
                                                           : MathUtils::Status::NotConverged;
        break;
      }
      for (size_t aCol = 0; aCol < aN; ++aCol)
      {
        aDirection.ChangeAt(aCol) = -aSearchGradient.At(aCol) / aGradientNorm;
      }
    }

    const long double aMerit     = 0.5L * aResidualNorm * aResidualNorm;
    bool              isAccepted = false;
    math_Vector       aTrial(aN, 0.0);
    for (uint32_t aLineIter = 0; aLineIter < 24; ++aLineIter)
    {
      const double anAlpha              = std::ldexp(1.0, -static_cast<int>(aLineIter));
      double       aProjectedDerivative = 0.0;
      for (size_t anOffset = 0; anOffset < aN; ++anOffset)
      {
        aTrial.ChangeAt(anOffset) = aSolution.At(anOffset) + anAlpha * aDirection.At(anOffset);
        if (hasBounds)
        {
          aTrial.ChangeAt(anOffset) =
            std::clamp(aTrial.At(anOffset), theInfBound->At(anOffset), theSupBound->At(anOffset));
        }
        aProjectedDerivative +=
          aGradient.At(anOffset) * (aTrial.At(anOffset) - aSolution.At(anOffset));
      }

      if (!(aProjectedDerivative < 0.0))
      {
        continue;
      }
      if (!theFunc.Value(aTrial, aTrialResidual))
      {
        aResult.Status   = MathUtils::Status::CallbackError;
        aResult.Solution = aSolution;
        return aResult;
      }
      if (!IsFinite(aTrialResidual))
      {
        aResult.Status   = MathUtils::Status::NumericalError;
        aResult.Solution = aSolution;
        return aResult;
      }
      const double      aTrialNorm  = SafeNorm(aTrialResidual);
      const long double aTrialMerit = 0.5L * aTrialNorm * aTrialNorm;
      if (aTrialMerit <= aMerit + 1.0e-4L * aProjectedDerivative)
      {
        aResult.StepNorm  = 0.0;
        isLastStepWithinX = true;
        for (size_t aVar = 0; aVar < aN; ++aVar)
        {
          const double aComponentStep = aTrial.At(aVar) - aSolution.At(aVar);
          aResult.StepNorm            = std::hypot(aResult.StepNorm, aComponentStep);
          isLastStepWithinX = isLastStepWithinX && std::abs(aComponentStep) <= theTolX.At(aVar);
        }
        aSolution    = aTrial;
        hasTakenStep = true;
        isAccepted   = true;
        break;
      }
    }

    aResult.NbIterations = anIter + 1;
    if (!isAccepted)
    {
      aResult.Status = MathUtils::Status::NotConverged;
      break;
    }

    if (isLastStepWithinX)
    {
      const double aTrialNorm = SafeNorm(aTrialResidual);
      aResult.Status =
        (aTrialNorm <= theTolF) ? MathUtils::Status::OK : MathUtils::Status::NotConverged;
      break;
    }
  }

  if (aResult.Status == MathUtils::Status::NotConverged && aResult.NbIterations == theMaxIter)
  {
    aResult.Status = MathUtils::Status::MaxIterations;
  }
  if (!FinalizeSystemResult(theFunc, aSolution, aM, theTolF, aResult))
  {
    return aResult;
  }
  if (aResult.IsRoot && (!hasTakenStep || isLastStepWithinX))
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

//! Solve a square, overdetermined, or underdetermined nonlinear system.
template <typename FuncSetType>
SystemResult Newton(FuncSetType&       theFunc,
                    const math_Vector& theStart,
                    const math_Vector& theTolX,
                    double             theTolF,
                    uint32_t           theMaxIter = 100)
{
  return Utils::NewtonImpl(theFunc, theStart, theTolX, theTolF, theMaxIter, nullptr, nullptr);
}

//! Solve a nonlinear system with projected box constraints.
template <typename FuncSetType>
SystemResult NewtonBounded(FuncSetType&       theFunc,
                           const math_Vector& theStart,
                           const math_Vector& theInfBound,
                           const math_Vector& theSupBound,
                           const math_Vector& theTolX,
                           double             theTolF,
                           uint32_t           theMaxIter = 100)
{
  return Utils::NewtonImpl(theFunc,
                           theStart,
                           theTolX,
                           theTolF,
                           theMaxIter,
                           &theInfBound,
                           &theSupBound);
}

//! Solve with a uniform variable tolerance.
template <typename FuncSetType>
SystemResult Newton(FuncSetType&       theFunc,
                    const math_Vector& theStart,
                    double             theTolX,
                    double             theTolF,
                    uint32_t           theMaxIter = 100)
{
  math_Vector aTolX(theStart.Size(), theTolX);
  return Newton(theFunc, theStart, aTolX, theTolF, theMaxIter);
}

} // namespace MathSys

#endif // _MathSys_Newton_HeaderFile

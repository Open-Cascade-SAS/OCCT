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

#ifndef _MathOpt_Newton_HeaderFile
#define _MathOpt_Newton_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathLin_Gauss.hxx>
#include <MathLin_Jacobi.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_LineSearch.hxx>
#include <MathUtils_Deriv.hxx>
#include <MathOpt_Utils.hxx>

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

namespace Utils
{
//! Normalize the Newton system before regularization and elimination. Scaling both
//! sides preserves the step and makes regularization independent of objective units.
inline bool RegularizeNewtonSystem(math_Matrix& theHessian,
                                   math_Vector& theRightHandSide,
                                   double       theMinimumEigenvalue)
{
  double aScale = 0.0;
  for (size_t i = 0; i < theHessian.RowSize(); ++i)
  {
    for (size_t j = 0; j < theHessian.ColSize(); ++j)
    {
      aScale = std::max(aScale, std::abs(theHessian.At(i, j)));
    }
  }
  // A locally linear objective still needs a descent step.
  if (aScale == 0.0)
  {
    for (size_t i = 0; i < theRightHandSide.Size(); ++i)
    {
      aScale = std::max(aScale, std::abs(theRightHandSide.At(i)));
    }
  }
  if (aScale == 0.0)
  {
    return false;
  }
  for (size_t i = 0; i < theHessian.RowSize(); ++i)
  {
    theRightHandSide.ChangeAt(i) /= aScale;
    for (size_t j = 0; j < theHessian.ColSize(); ++j)
    {
      theHessian.ChangeAt(i, j) /= aScale;
    }
  }
  if (!IsFinite(theRightHandSide))
  {
    return false;
  }
  // The scalar eigenvalue is the only matrix entry; no eigensolver is needed.
  if (theHessian.RowSize() == 1)
  {
    theHessian.ChangeAt(0, 0) = std::max(theHessian.At(0, 0), theMinimumEigenvalue);
    return true;
  }

  const MathUtils::EigenResult anEigen = MathLin::Jacobi(theHessian, false);
  if (!anEigen.IsDone())
  {
    return false;
  }

  double aLowestEigenvalue = anEigen.EigenValues->At(0);
  for (size_t anIndex = 1; anIndex < anEigen.EigenValues->Size(); ++anIndex)
  {
    aLowestEigenvalue = std::min(aLowestEigenvalue, anEigen.EigenValues->At(anIndex));
  }
  if (aLowestEigenvalue < theMinimumEigenvalue)
  {
    const double aShift = theMinimumEigenvalue - aLowestEigenvalue;
    for (size_t anIndex = 0; anIndex < theHessian.RowSize(); ++anIndex)
    {
      theHessian.ChangeAt(anIndex, anIndex) += aShift;
    }
  }
  return IsFinite(theHessian);
}
} // namespace Utils

//! Configuration for Newton minimization with Hessian.
//! RelativeTolerance bounds gradient reduction relative to its initial norm.
//! FTolerance accepts an absolute gradient only together with a step below XTolerance.
//! An exactly zero (projected) starting gradient is accepted without a Hessian call.
struct NewtonConfig : Config
{
  double Regularization = 1.0e-8; //!< Minimum eigenvalue after normalizing the Newton system
  bool   UseLineSearch  = true;   //!< Whether to use line search (recommended)

  //! Default constructor.
  NewtonConfig() = default;

  //! Constructor with tolerance.
  explicit NewtonConfig(double theTolerance, uint32_t theMaxIter = 100)
      : Config(theTolerance, theMaxIter)
  {
  }
};

//! Newton's method for N-dimensional minimization using Hessian.
//!
//! Fastest convergence near minimum (quadratic) but requires Hessian computation.
//! Uses line search for global convergence and Hessian regularization
//! when the Hessian is not positive definite.
//!
//! Algorithm:
//! 1. Compute gradient g and Hessian H at current point
//! 2. Normalize H and -g by a common scale, then shift H to enforce a minimum eigenvalue
//! 3. Solve H * p = -g for search direction p
//! 4. Perform line search along p
//! 5. Update x = x + alpha * p
//! 6. Repeat until convergence
//!
//! @tparam Function type with:
//!   - Value(const math_Vector&, double&) for function value
//!   - Gradient(const math_Vector&, math_Vector&) for gradient
//!   - Hessian(const math_Vector&, math_Matrix&) for Hessian
//! @param theFunc function object with value, gradient, and Hessian
//! @param theStartingPoint initial guess
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult Newton(Function&           theFunc,
                    const math_Vector&  theStartingPoint,
                    const NewtonConfig& theConfig = NewtonConfig())
{
  VectorResult aResult;

  const size_t aN = theStartingPoint.Size();

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint)
      || !std::isfinite(theConfig.Regularization) || theConfig.Regularization <= 0.0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Current point
  math_Vector aX(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aX.ChangeAt(i) = theStartingPoint.At(i);
  }

  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  // Gradient at current point
  math_Vector  aGrad(aN);
  const Status aGradientStatus = Utils::GradientStatus(theFunc, aX, aGrad);
  if (aGradientStatus != Status::OK)
  {
    aResult.Status = aGradientStatus;
    return aResult;
  }

  const double anInitialGradNorm = Utils::Norm(aGrad);
  double       aGradNorm         = anInitialGradNorm;
  if (!std::isfinite(anInitialGradNorm))
  {
    aResult.Status   = Status::NumericalError;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  if (aGradNorm == 0.0)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Working vectors and matrices
  math_Vector aDir(aN);
  math_Vector aXNew(aN);
  math_Vector aGradNew(aN);
  math_Matrix aHessian(aN, aN);
  math_Vector aNegGrad(aN);

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute Hessian
    const Status aHessianStatus = Utils::HessianStatus(theFunc, aX, aHessian);
    if (aHessianStatus != Status::OK)
    {
      aResult.Status   = aHessianStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Prepare negative gradient
    for (size_t i = 0; i < aN; ++i)
    {
      aNegGrad.ChangeAt(i) = -aGrad.At(i);
    }

    if (!Utils::RegularizeNewtonSystem(aHessian, aNegGrad, theConfig.Regularization))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    auto aLinResult = MathLin::Solve(aHessian, aNegGrad);
    if (!aLinResult.IsDone())
    {
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGrad.At(i);
      }
      goto perform_line_search;
    }

    aDir = *aLinResult.Solution;

    // Check if direction is descent
    {
      double aDirDeriv = 0.0;
      for (size_t i = 0; i < aN; ++i)
      {
        aDirDeriv += aGrad.At(i) * aDir.At(i);
      }

      if (aDirDeriv >= 0.0)
      {
        // Not a descent direction, use steepest descent
        for (size_t i = 0; i < aN; ++i)
        {
          aDir.ChangeAt(i) = -aGrad.At(i);
        }
      }
    }

  perform_line_search:
    double aFxNew = 0.0;
    if (theConfig.UseLineSearch)
    {
      // Line search
      MathUtils::LineSearchResult aLineResult =
        Utils::Backtrack(theFunc, aX, aDir, aGrad, aFx, 1.0, theConfig.StepMin);

      if (!aLineResult.IsValid)
      {
        const MathUtils::LineSearchResult anInitialLineResult = aLineResult;
        // Line search failed, try steepest descent
        for (size_t i = 0; i < aN; ++i)
        {
          aDir.ChangeAt(i) = -aGrad.At(i);
        }
        aLineResult = Utils::Backtrack(theFunc, aX, aDir, aGrad, aFx, 1.0, theConfig.StepMin);

        if (!aLineResult.IsValid)
        {
          aResult.Status   = Utils::LineSearchFailureStatus(anInitialLineResult, aLineResult);
          aResult.Solution = aX;
          aResult.Value    = aFx;
          aResult.Gradient = aGrad;
          return aResult;
        }
      }

      // Compute new point
      for (size_t i = 0; i < aN; ++i)
      {
        aXNew.ChangeAt(i) = aX.At(i) + aLineResult.Alpha * aDir.At(i);
      }
      aFxNew = aLineResult.FNew;
    }
    else
    {
      // Full Newton step (no line search)
      for (size_t i = 0; i < aN; ++i)
      {
        aXNew.ChangeAt(i) = aX.At(i) + aDir.At(i);
      }

      const Status aNewValueStatus = Utils::ValueStatus(theFunc, aXNew, aFxNew);
      if (aNewValueStatus != Status::OK)
      {
        aResult.Status   = aNewValueStatus;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew.At(i) - aX.At(i)));
    }

    // Evaluate gradient at new point
    const Status aNewGradientStatus = Utils::GradientStatus(theFunc, aXNew, aGradNew);
    if (aNewGradientStatus != Status::OK)
    {
      aResult.Status   = aNewGradientStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Check gradient convergence
    aGradNorm = Utils::Norm(aGradNew);
    if (!std::isfinite(aGradNorm))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aXNew;
      aResult.Value    = aFxNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    if (aGradNorm / anInitialGradNorm <= theConfig.RelativeTolerance
        || (aMaxDiff < theConfig.XTolerance && aGradNorm <= theConfig.FTolerance))
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aFxNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::NotConverged;
      aResult.Solution = aXNew;
      aResult.Value    = aFxNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Update for next iteration
    aX    = aXNew;
    aGrad = aGradNew;
    aFx   = aFxNew;
  }

  // Maximum iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  aResult.Gradient = aGrad;
  return aResult;
}

//! Modified Newton's method with automatic Hessian regularization.
//! Adds diagonal elements to ensure positive definiteness using
//! an adaptive regularization strategy.
//!
//! @tparam Function type with Value, Gradient, and Hessian methods
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult NewtonModified(Function&           theFunc,
                            const math_Vector&  theStartingPoint,
                            const NewtonConfig& theConfig = NewtonConfig())
{
  return Newton(theFunc, theStartingPoint, theConfig);
}

//! Newton's method with numerical Hessian.
//! Computes Hessian using finite differences when analytical Hessian
//! is not available.
//!
//! @tparam Function type with:
//!   - Value(const math_Vector&, double&) for function value
//!   - Gradient(const math_Vector&, math_Vector&) for gradient
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theHessStep step size for numerical Hessian
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult NewtonNumericalHessian(Function&           theFunc,
                                    const math_Vector&  theStartingPoint,
                                    double              theHessStep = 1.0e-6,
                                    const NewtonConfig& theConfig   = NewtonConfig())
{
  if (!std::isfinite(theHessStep) || theHessStep <= 0.0)
  {
    VectorResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Wrapper that adds numerical Hessian
  class FuncWithHessian
  {
  public:
    FuncWithHessian(Function& theF, double theStep)
        : myFunc(theF),
          myStep(theStep)
    {
    }

    bool Value(const math_Vector& theX, double& theF) { return myFunc.Value(theX, theF); }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      return myFunc.Gradient(theX, theGrad);
    }

    bool Hessian(const math_Vector& theX, math_Matrix& theHess)
    {
      math_Vector aXMod = theX;
      return MathUtils::NumericalHessian(myFunc, aXMod, theHess, myStep);
    }

  private:
    Function& myFunc;
    double    myStep;
  };

  FuncWithHessian aWrapper(theFunc, theHessStep);
  return Newton(aWrapper, theStartingPoint, theConfig);
}

//! Newton's method with fully numerical derivatives.
//! Computes both gradient and Hessian using finite differences.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method only
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theGradStep step size for numerical gradient
//! @param theHessStep step size for numerical Hessian
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult NewtonNumerical(Function&           theFunc,
                             const math_Vector&  theStartingPoint,
                             double              theGradStep = 1.0e-8,
                             double              theHessStep = 1.0e-6,
                             const NewtonConfig& theConfig   = NewtonConfig())
{
  if (!std::isfinite(theGradStep) || theGradStep <= 0.0 || !std::isfinite(theHessStep)
      || theHessStep <= 0.0)
  {
    VectorResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Wrapper that adds numerical gradient and Hessian
  class FuncWithDerivatives
  {
  public:
    FuncWithDerivatives(Function& theF, double theGStep, double theHStep)
        : myFunc(theF),
          myGradStep(theGStep),
          myHessStep(theHStep)
    {
    }

    bool Value(const math_Vector& theX, double& theF) { return myFunc.Value(theX, theF); }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      math_Vector aXMod = theX;
      return MathUtils::NumericalGradientAdaptive(myFunc, aXMod, theGrad, myGradStep);
    }

    bool Hessian(const math_Vector& theX, math_Matrix& theHess)
    {
      // Compute Hessian from finite differences of gradient
      const size_t aN = theX.Size();

      math_Vector aXMod = theX;
      math_Vector aGradPlus(aN);
      math_Vector aGradMinus(aN);

      for (size_t j = 0; j < aN; ++j)
      {
        const double aXj = aXMod.At(j);

        aXMod.ChangeAt(j) = aXj + myHessStep;
        if (!MathUtils::NumericalGradientAdaptive(myFunc, aXMod, aGradPlus, myGradStep))
        {
          aXMod.ChangeAt(j) = aXj;
          return false;
        }

        aXMod.ChangeAt(j) = aXj - myHessStep;
        if (!MathUtils::NumericalGradientAdaptive(myFunc, aXMod, aGradMinus, myGradStep))
        {
          aXMod.ChangeAt(j) = aXj;
          return false;
        }

        aXMod.ChangeAt(j) = aXj;

        for (size_t i = 0; i < aN; ++i)
        {
          theHess.ChangeAt(i, j) = (aGradPlus.At(i) - aGradMinus.At(i)) / (2.0 * myHessStep);
        }
      }

      // Symmetrize
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = i + 1; j < aN; ++j)
        {
          const double aAvg      = 0.5 * (theHess.At(i, j) + theHess.At(j, i));
          theHess.ChangeAt(i, j) = aAvg;
          theHess.ChangeAt(j, i) = aAvg;
        }
      }

      return true;
    }

  private:
    Function& myFunc;
    double    myGradStep;
    double    myHessStep;
  };

  FuncWithDerivatives aWrapper(theFunc, theGradStep, theHessStep);
  return Newton(aWrapper, theStartingPoint, theConfig);
}

//! Newton's method with bound constraints.
//!
//! Minimizes f(x) subject to theLowerBounds <= x <= theUpperBounds.
//! Uses projected gradient approach similar to BFGSBounded.
//!
//! @tparam Function type with Value, Gradient, and Hessian methods
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theLowerBounds lower bounds for each variable
//! @param theUpperBounds upper bounds for each variable
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult NewtonBounded(Function&           theFunc,
                           const math_Vector&  theStartingPoint,
                           const math_Vector&  theLowerBounds,
                           const math_Vector&  theUpperBounds,
                           const NewtonConfig& theConfig = NewtonConfig())
{
  VectorResult aResult;

  const size_t aN = theStartingPoint.Size();

  // Check dimensions
  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint)
      || !std::isfinite(theConfig.Regularization) || theConfig.Regularization <= 0.0
      || theLowerBounds.Size() != aN || !Utils::IsValidBounds(theLowerBounds, theUpperBounds))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Lambda to clamp a point to bounds
  auto aClampToBounds = [&](math_Vector& theX) {
    for (size_t i = 0; i < aN; ++i)
    {
      if (theX.At(i) < theLowerBounds.At(i))
      {
        theX.ChangeAt(i) = theLowerBounds.At(i);
      }
      if (theX.At(i) > theUpperBounds.At(i))
      {
        theX.ChangeAt(i) = theUpperBounds.At(i);
      }
    }
  };

  // Lambda to project gradient (zero components at active bounds)
  auto aProjectGradient = [&](const math_Vector& theX, math_Vector& theGrad) {
    for (size_t i = 0; i < aN; ++i)
    {
      const double aTol = MathUtils::THE_EPSILON * std::max(1.0, std::abs(theX.At(i)));

      if (theX.At(i) - theLowerBounds.At(i) < aTol && theGrad.At(i) > 0.0)
      {
        theGrad.ChangeAt(i) = 0.0;
      }
      if (theUpperBounds.At(i) - theX.At(i) < aTol && theGrad.At(i) < 0.0)
      {
        theGrad.ChangeAt(i) = 0.0;
      }
    }
  };

  auto aProjectDirection = [&](const math_Vector& theX, math_Vector& theDir) {
    for (size_t i = 0; i < aN; ++i)
    {
      const double aTol = MathUtils::THE_EPSILON * std::max(1.0, std::abs(theX.At(i)));
      if ((theX.At(i) - theLowerBounds.At(i) < aTol && theDir.At(i) < 0.0)
          || (theUpperBounds.At(i) - theX.At(i) < aTol && theDir.At(i) > 0.0))
      {
        theDir.ChangeAt(i) = 0.0;
      }
    }
  };

  // Lambda to compute max step to boundary
  auto aComputeAlphaMax = [&](const math_Vector& theX, const math_Vector& theDir) -> double {
    double aAlphaMax = 1.0;
    for (size_t i = 0; i < aN; ++i)
    {
      if (theDir.At(i) < -MathUtils::THE_EPSILON)
      {
        const double aMaxStep = (theLowerBounds.At(i) - theX.At(i)) / theDir.At(i);
        aAlphaMax             = std::min(aAlphaMax, aMaxStep);
      }
      else if (theDir.At(i) > MathUtils::THE_EPSILON)
      {
        const double aMaxStep = (theUpperBounds.At(i) - theX.At(i)) / theDir.At(i);
        aAlphaMax             = std::min(aAlphaMax, aMaxStep);
      }
    }
    return aAlphaMax;
  };

  Utils::BoundedFunction<Function> aBoundedFunc(theFunc, theLowerBounds, theUpperBounds);

  // Current point
  math_Vector aX(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aX.ChangeAt(i) = theStartingPoint.At(i);
  }
  aClampToBounds(aX);

  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  // Gradient at current point
  math_Vector  aGrad(aN);
  const Status aGradientStatus = Utils::GradientStatus(theFunc, aX, aGrad);
  if (aGradientStatus != Status::OK)
  {
    aResult.Status = aGradientStatus;
    return aResult;
  }
  aProjectGradient(aX, aGrad);

  const double anInitialGradNorm = Utils::Norm(aGrad);
  double       aGradNorm         = anInitialGradNorm;
  if (!std::isfinite(anInitialGradNorm))
  {
    aResult.Status   = Status::NumericalError;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  if (aGradNorm == 0.0)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Working vectors and matrices
  math_Vector aDir(aN);
  math_Vector aXNew(aN);
  math_Vector aGradNew(aN);
  math_Matrix aHessian(aN, aN);
  math_Vector aNegGrad(aN);

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute Hessian
    const Status aHessianStatus = Utils::HessianStatus(theFunc, aX, aHessian);
    if (aHessianStatus != Status::OK)
    {
      aResult.Status   = aHessianStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Prepare negative gradient
    for (size_t i = 0; i < aN; ++i)
    {
      aNegGrad.ChangeAt(i) = -aGrad.At(i);
    }

    if (!Utils::RegularizeNewtonSystem(aHessian, aNegGrad, theConfig.Regularization))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    auto aLinResult = MathLin::Solve(aHessian, aNegGrad);
    if (!aLinResult.IsDone())
    {
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGrad.At(i);
      }
      goto perform_bounded_line_search;
    }

    aDir = *aLinResult.Solution;
    aProjectDirection(aX, aDir);

    // Check if direction is descent
    {
      double aDirDeriv = 0.0;
      for (size_t i = 0; i < aN; ++i)
      {
        aDirDeriv += aGrad.At(i) * aDir.At(i);
      }

      if (aDirDeriv >= 0.0)
      {
        for (size_t i = 0; i < aN; ++i)
        {
          aDir.ChangeAt(i) = -aGrad.At(i);
        }
      }
    }

  perform_bounded_line_search:
    aProjectDirection(aX, aDir);
    if (theConfig.UseLineSearch)
    {
      double aAlphaMax = aComputeAlphaMax(aX, aDir);

      MathUtils::LineSearchResult aLineResult;
      if (aAlphaMax > 0.0)
      {
        aLineResult =
          Utils::Backtrack(aBoundedFunc, aX, aDir, aGrad, aFx, aAlphaMax, theConfig.StepMin);
      }

      if (!aLineResult.IsValid)
      {
        const MathUtils::LineSearchResult anInitialLineResult = aLineResult;
        // Try steepest descent
        for (size_t i = 0; i < aN; ++i)
        {
          aDir.ChangeAt(i) = -aGrad.At(i);
        }
        aAlphaMax = aComputeAlphaMax(aX, aDir);
        if (aAlphaMax > 0.0)
        {
          aLineResult =
            Utils::Backtrack(aBoundedFunc, aX, aDir, aGrad, aFx, aAlphaMax, theConfig.StepMin);
        }

        if (!aLineResult.IsValid)
        {
          aResult.Status   = Utils::LineSearchFailureStatus(anInitialLineResult, aLineResult);
          aResult.Solution = aX;
          aResult.Value    = aFx;
          aResult.Gradient = aGrad;
          return aResult;
        }
      }

      for (size_t i = 0; i < aN; ++i)
      {
        aXNew.ChangeAt(i) = aX.At(i) + aLineResult.Alpha * aDir.At(i);
      }
      aClampToBounds(aXNew);

      // Backtrack evaluated this same clamped point through aBoundedFunc.
      aFx = aLineResult.FNew;
    }
    else
    {
      for (size_t i = 0; i < aN; ++i)
      {
        aXNew.ChangeAt(i) = aX.At(i) + aDir.At(i);
      }
      aClampToBounds(aXNew);

      double       aFxNew          = 0.0;
      const Status aNewValueStatus = Utils::ValueStatus(theFunc, aXNew, aFxNew);
      if (aNewValueStatus != Status::OK)
      {
        aResult.Status   = aNewValueStatus;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }
      aFx = aFxNew;
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew.At(i) - aX.At(i)));
    }

    // Evaluate gradient at new point
    const Status aNewGradientStatus = Utils::GradientStatus(theFunc, aXNew, aGradNew);
    if (aNewGradientStatus != Status::OK)
    {
      aResult.Status   = aNewGradientStatus;
      aResult.Solution = aXNew;
      aResult.Value    = aFx;
      return aResult;
    }
    aProjectGradient(aXNew, aGradNew);

    // Check gradient convergence
    aGradNorm = Utils::Norm(aGradNew);
    if (!std::isfinite(aGradNorm))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aXNew;
      aResult.Value    = aFx;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    if (aGradNorm / anInitialGradNorm <= theConfig.RelativeTolerance
        || (aMaxDiff < theConfig.XTolerance && aGradNorm <= theConfig.FTolerance))
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aFx;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::NotConverged;
      aResult.Solution = aXNew;
      aResult.Value    = aFx;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    aX    = aXNew;
    aGrad = aGradNew;
  }

  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  aResult.Gradient = aGrad;
  return aResult;
}

} // namespace MathOpt

#endif // _MathOpt_Newton_HeaderFile

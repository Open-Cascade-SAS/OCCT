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
#include "MathOpt_Utils.hxx"

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

namespace Utils
{
inline bool RegularizeHessian(math_Matrix& theHessian, double theMinimumEigenvalue)
{
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
struct NewtonConfig : Config
{
  double Regularization = 1.0e-8; //!< Diagonal regularization for non-positive definite Hessian
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
//! 2. If H is not positive definite, regularize: H = H + lambda*I
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

  // Check if already at minimum
  double aGradNorm = Utils::Norm(aGrad);

  if (aGradNorm < theConfig.FTolerance)
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

    if (!Utils::RegularizeHessian(aHessian, theConfig.Regularization))
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

    if (aGradNorm < theConfig.FTolerance)
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
  auto ClampToBounds = [&](math_Vector& theX) {
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
  auto ProjectGradient = [&](const math_Vector& theX, math_Vector& theGrad) {
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

  auto ProjectDirection = [&](const math_Vector& theX, math_Vector& theDir) {
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
  auto ComputeAlphaMax = [&](const math_Vector& theX, const math_Vector& theDir) -> double {
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
  ClampToBounds(aX);

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
  ProjectGradient(aX, aGrad);

  // Check if already at minimum
  double aGradNorm = Utils::Norm(aGrad);

  if (aGradNorm < theConfig.FTolerance)
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

    if (!Utils::RegularizeHessian(aHessian, theConfig.Regularization))
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
    ProjectDirection(aX, aDir);

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
    ProjectDirection(aX, aDir);
    if (theConfig.UseLineSearch)
    {
      double aAlphaMax = ComputeAlphaMax(aX, aDir);

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
        aAlphaMax = ComputeAlphaMax(aX, aDir);
        if (aAlphaMax > 0.0)
        {
          aLineResult =
            Utils::Backtrack(aBoundedFunc,
                             aX,
                             aDir,
                             aGrad,
                             aFx,
                             aAlphaMax,
                             theConfig.StepMin);
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
      ClampToBounds(aXNew);

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
    else
    {
      for (size_t i = 0; i < aN; ++i)
      {
        aXNew.ChangeAt(i) = aX.At(i) + aDir.At(i);
      }
      ClampToBounds(aXNew);

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
    ProjectGradient(aXNew, aGradNew);

    // Check gradient convergence
    aGradNorm = Utils::Norm(aGradNew);

    if (aGradNorm < theConfig.FTolerance)
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

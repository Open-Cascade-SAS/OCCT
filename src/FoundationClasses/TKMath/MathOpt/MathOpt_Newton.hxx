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
#include <MathUtils_Core.hxx>
#include <MathUtils_LineSearch.hxx>
#include <MathUtils_Deriv.hxx>

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

//! Configuration for Newton minimization with Hessian.
struct NewtonConfig : Config
{
  double Regularization = 1.0e-8; //!< Diagonal regularization for non-positive definite Hessian
  bool   UseLineSearch  = true;   //!< Whether to use line search (recommended)

  //! Default constructor.
  NewtonConfig() = default;

  //! Constructor with tolerance.
  explicit NewtonConfig(double theTolerance, int theMaxIter = 100)
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

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();

  // Current point
  math_Vector aX(aLower, aUpper);
  aX = theStartingPoint;

  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Gradient at current point
  math_Vector aGrad(aLower, aUpper);
  if (!theFunc.Gradient(aX, aGrad))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Check if already at minimum
  double aGradNorm = 0.0;
  for (int i = aLower; i <= aUpper; ++i)
  {
    aGradNorm += MathUtils::Sqr(aGrad(i));
  }
  aGradNorm = std::sqrt(aGradNorm);

  if (aGradNorm < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Working vectors and matrices
  math_Vector aDir(aLower, aUpper);
  math_Vector aXNew(aLower, aUpper);
  math_Vector aGradNew(aLower, aUpper);
  math_Matrix aHessian(aLower, aUpper, aLower, aUpper);
  math_Vector aNegGrad(aLower, aUpper);

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute Hessian
    if (!theFunc.Hessian(aX, aHessian))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Prepare negative gradient
    for (int i = aLower; i <= aUpper; ++i)
    {
      aNegGrad(i) = -aGrad(i);
    }

    // Try to solve H * p = -g
    auto aLinResult = MathLin::Solve(aHessian, aNegGrad);

    if (!aLinResult.IsDone())
    {
      // Hessian is singular or not positive definite, add regularization
      double aLambda = theConfig.Regularization;
      bool   aSolved = false;

      for (int k = 0; k < 10 && !aSolved; ++k)
      {
        math_Matrix aRegHessian = aHessian;
        for (int i = aLower; i <= aUpper; ++i)
        {
          aRegHessian(i, i) += aLambda;
        }

        aLinResult = MathLin::Solve(aRegHessian, aNegGrad);
        if (aLinResult.IsDone())
        {
          aSolved = true;
        }
        else
        {
          aLambda *= 10.0;
        }
      }

      if (!aSolved)
      {
        // Fall back to steepest descent
        for (int i = aLower; i <= aUpper; ++i)
        {
          aDir(i) = -aGrad(i);
        }
        goto perform_line_search;
      }
    }

    aDir = *aLinResult.Solution;

    // Check if direction is descent
    {
      double aDirDeriv = 0.0;
      for (int i = aLower; i <= aUpper; ++i)
      {
        aDirDeriv += aGrad(i) * aDir(i);
      }

      if (aDirDeriv >= 0.0)
      {
        // Not a descent direction, use steepest descent
        for (int i = aLower; i <= aUpper; ++i)
        {
          aDir(i) = -aGrad(i);
        }
      }
    }

  perform_line_search:
    if (theConfig.UseLineSearch)
    {
      // Line search
      MathUtils::LineSearchResult aLineResult =
        MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

      if (!aLineResult.IsValid)
      {
        // Line search failed, try steepest descent
        for (int i = aLower; i <= aUpper; ++i)
        {
          aDir(i) = -aGrad(i);
        }
        aLineResult =
          MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

        if (!aLineResult.IsValid)
        {
          aResult.Status   = Status::NotConverged;
          aResult.Solution = aX;
          aResult.Value    = aFx;
          aResult.Gradient = aGrad;
          return aResult;
        }
      }

      // Compute new point
      for (int i = aLower; i <= aUpper; ++i)
      {
        aXNew(i) = aX(i) + aLineResult.Alpha * aDir(i);
      }
      aFx = aLineResult.FNew;
    }
    else
    {
      // Full Newton step (no line search)
      for (int i = aLower; i <= aUpper; ++i)
      {
        aXNew(i) = aX(i) + aDir(i);
      }

      if (!theFunc.Value(aXNew, aFx))
      {
        aResult.Status   = Status::NumericalError;
        aResult.Solution = aX;
        return aResult;
      }
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew(i) - aX(i)));
    }

    // Evaluate gradient at new point
    if (!theFunc.Gradient(aXNew, aGradNew))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aXNew;
      aResult.Value    = aFx;
      return aResult;
    }

    // Check gradient convergence
    aGradNorm = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aGradNorm += MathUtils::Sqr(aGradNew(i));
    }
    aGradNorm = std::sqrt(aGradNorm);

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
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aFx;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Update for next iteration
    aX    = aXNew;
    aGrad = aGradNew;
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
      const int aLower = theX.Lower();
      const int aUpper = theX.Upper();

      math_Vector aXMod = theX;
      math_Vector aGradPlus(aLower, aUpper);
      math_Vector aGradMinus(aLower, aUpper);

      for (int j = aLower; j <= aUpper; ++j)
      {
        const double aXj = aXMod(j);

        aXMod(j) = aXj + myHessStep;
        if (!MathUtils::NumericalGradientAdaptive(myFunc, aXMod, aGradPlus, myGradStep))
        {
          aXMod(j) = aXj;
          return false;
        }

        aXMod(j) = aXj - myHessStep;
        if (!MathUtils::NumericalGradientAdaptive(myFunc, aXMod, aGradMinus, myGradStep))
        {
          aXMod(j) = aXj;
          return false;
        }

        aXMod(j) = aXj;

        for (int i = aLower; i <= aUpper; ++i)
        {
          theHess(i, j) = (aGradPlus(i) - aGradMinus(i)) / (2.0 * myHessStep);
        }
      }

      // Symmetrize
      for (int i = aLower; i <= aUpper; ++i)
      {
        for (int j = i + 1; j <= aUpper; ++j)
        {
          double aAvg   = 0.5 * (theHess(i, j) + theHess(j, i));
          theHess(i, j) = aAvg;
          theHess(j, i) = aAvg;
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

} // namespace MathOpt

#endif // _MathOpt_Newton_HeaderFile

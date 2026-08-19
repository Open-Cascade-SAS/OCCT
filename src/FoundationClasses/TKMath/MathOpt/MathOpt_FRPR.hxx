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

#ifndef _MathOpt_FRPR_HeaderFile
#define _MathOpt_FRPR_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
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
//! Nonlinear conjugate gradient requires a stronger curvature condition than quasi-Newton methods.
inline constexpr double THE_FRPR_WOLFE_CURVATURE = 0.1;
} // namespace Utils

//! Conjugate gradient formula selection.
enum class ConjugateGradientFormula
{
  FletcherReeves,  //!< beta = g_new^T g_new / g^T g (original, guaranteed descent)
  PolakRibiere,    //!< beta = g_new^T (g_new - g) / g^T g (often faster, may need restarts)
  HestenesStiefel, //!< beta = g_new^T (g_new - g) / d^T (g_new - g)
  DaiYuan          //!< beta = g_new^T g_new / d^T (g_new - g)
};

//! Configuration for FRPR conjugate gradient method.
struct FRPRConfig : Config
{
  ConjugateGradientFormula Formula = ConjugateGradientFormula::PolakRibiere; //!< Beta formula
  uint32_t RestartInterval = 0; //!< Restart every N iterations (0 = n, where n is dimension)

  //! Default constructor.
  FRPRConfig() = default;

  //! Constructor with tolerance.
  explicit FRPRConfig(double theTolerance, uint32_t theMaxIter = 100)
      : Config(theTolerance, theMaxIter)
  {
  }
};

//! Fletcher-Reeves-Polak-Ribiere conjugate gradient method.
//!
//! Memory-efficient alternative to BFGS for large-scale optimization.
//! Uses only O(n) storage compared to O(n^2) for BFGS.
//!
//! Algorithm:
//! 1. Compute gradient g at current point
//! 2. First iteration: search direction p = -g
//! 3. Perform line search along p
//! 4. Compute new gradient g_new
//! 5. Update: beta = (g_new . g_new) / (g . g) [Fletcher-Reeves]
//!         or beta = (g_new . (g_new - g)) / (g . g) [Polak-Ribiere]
//! 6. New direction: p = -g_new + beta * p
//! 7. Restart with steepest descent if beta < 0 or periodically
//! 8. Repeat until convergence
//!
//! @tparam Function type with:
//!   - Value(const math_Vector&, double&) for function value
//!   - Gradient(const math_Vector&, math_Vector&) for gradient
//! @param theFunc function object with value and gradient
//! @param theStartingPoint initial guess
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult FRPR(Function&          theFunc,
                  const math_Vector& theStartingPoint,
                  const FRPRConfig&  theConfig = FRPRConfig())
{
  VectorResult aResult;

  const size_t aN = theStartingPoint.Size();

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Restart interval
  const size_t aRestartInterval = (theConfig.RestartInterval > 0) ? theConfig.RestartInterval : aN;

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
  double aGradNormSq = MathUtils::DotProduct(aGrad, aGrad);

  if (std::sqrt(aGradNormSq) < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Search direction (initially steepest descent)
  math_Vector aDir(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aDir.ChangeAt(i) = -aGrad.At(i);
  }

  // Working vectors
  math_Vector aXNew(aN);
  math_Vector aGradNew(aN);
  math_Vector aGradDiff(aN);

  uint32_t aRestartCount = 0;

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Line search
    MathUtils::LineSearchResult aLineResult =
      MathUtils::WolfeSearch(theFunc,
                             aX,
                             aDir,
                             aGrad,
                             aFx,
                             1.0,
                             MathUtils::THE_ARMIJO_C1,
                             Utils::THE_FRPR_WOLFE_CURVATURE);
    if (aLineResult.Alpha < theConfig.StepMin)
    {
      aLineResult.IsValid = false;
    }

    if (!aLineResult.IsValid)
    {
      const MathUtils::LineSearchResult anInitialLineResult = aLineResult;
      // Line search failed, try steepest descent
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGrad.At(i);
      }
      aLineResult = MathUtils::WolfeSearch(theFunc,
                                           aX,
                                           aDir,
                                           aGrad,
                                           aFx,
                                           1.0,
                                           MathUtils::THE_ARMIJO_C1,
                                           Utils::THE_FRPR_WOLFE_CURVATURE);
      if (aLineResult.Alpha < theConfig.StepMin)
      {
        aLineResult.IsValid = false;
      }

      if (!aLineResult.IsValid)
      {
        aResult.Status   = Utils::LineSearchFailureStatus(anInitialLineResult, aLineResult);
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }
      aRestartCount = 0; // Reset restart counter after steepest descent
    }

    // Compute new point
    for (size_t i = 0; i < aN; ++i)
    {
      aXNew.ChangeAt(i) = aX.At(i) + aLineResult.Alpha * aDir.At(i);
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
    double aGradNewNormSq = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aGradNewNormSq += MathUtils::Sqr(aGradNew.At(i));
    }

    if (std::sqrt(aGradNewNormSq) < theConfig.FTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Compute gradient difference for some formulas
    for (size_t i = 0; i < aN; ++i)
    {
      aGradDiff.ChangeAt(i) = aGradNew.At(i) - aGrad.At(i);
    }

    // Compute beta based on selected formula
    double aBeta = 0.0;
    ++aRestartCount;

    if (aRestartCount >= aRestartInterval)
    {
      // Periodic restart with steepest descent
      aBeta         = 0.0;
      aRestartCount = 0;
    }
    else
    {
      switch (theConfig.Formula)
      {
        case ConjugateGradientFormula::FletcherReeves:
          // beta = g_new^T g_new / g^T g
          if (aGradNormSq > MathUtils::THE_ZERO_TOL)
          {
            aBeta = aGradNewNormSq / aGradNormSq;
          }
          break;

        case ConjugateGradientFormula::PolakRibiere: {
          // beta = g_new^T (g_new - g) / g^T g
          double aDot = 0.0;
          for (size_t i = 0; i < aN; ++i)
          {
            aDot += aGradNew.At(i) * aGradDiff.At(i);
          }
          if (aGradNormSq > MathUtils::THE_ZERO_TOL)
          {
            aBeta = aDot / aGradNormSq;
          }
          // Restart if beta < 0 (PR+ variant)
          if (aBeta < 0.0)
          {
            aBeta         = 0.0;
            aRestartCount = 0;
          }
        }
        break;

        case ConjugateGradientFormula::HestenesStiefel: {
          // beta = g_new^T (g_new - g) / d^T (g_new - g)
          double aNum = 0.0;
          double aDen = 0.0;
          for (size_t i = 0; i < aN; ++i)
          {
            aNum += aGradNew.At(i) * aGradDiff.At(i);
            aDen += aDir.At(i) * aGradDiff.At(i);
          }
          if (std::abs(aDen) > MathUtils::THE_ZERO_TOL)
          {
            aBeta = aNum / aDen;
          }
          if (aBeta < 0.0)
          {
            aBeta         = 0.0;
            aRestartCount = 0;
          }
        }
        break;

        case ConjugateGradientFormula::DaiYuan: {
          // beta = g_new^T g_new / d^T (g_new - g)
          double aDen = 0.0;
          for (size_t i = 0; i < aN; ++i)
          {
            aDen += aDir.At(i) * aGradDiff.At(i);
          }
          if (std::abs(aDen) > MathUtils::THE_ZERO_TOL)
          {
            aBeta = aGradNewNormSq / aDen;
          }
        }
        break;
        default:
          aResult.Status   = Status::InvalidInput;
          aResult.Solution = aX;
          aResult.Value    = aFx;
          aResult.Gradient = aGrad;
          return aResult;
      }
    }

    if (!std::isfinite(aBeta))
    {
      aBeta         = 0.0;
      aRestartCount = 0;
    }

    // Update search direction: p = -g_new + beta * p
    for (size_t i = 0; i < aN; ++i)
    {
      aDir.ChangeAt(i) = -aGradNew.At(i) + aBeta * aDir.At(i);
    }

    // Check if direction is still a descent direction
    double aDirDeriv = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aDirDeriv += aGradNew.At(i) * aDir.At(i);
    }

    if (!std::isfinite(aDirDeriv) || aDirDeriv >= 0.0)
    {
      // Not a descent direction, restart with steepest descent
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGradNew.At(i);
      }
      aRestartCount = 0;
    }

    // Update for next iteration
    aX          = aXNew;
    aGrad       = aGradNew;
    aGradNormSq = aGradNewNormSq;
    aFx         = aLineResult.FNew;
  }

  // Maximum iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  aResult.Gradient = aGrad;
  return aResult;
}

//! FRPR with numerical gradient.
//! Uses central differences when analytical gradient is not available.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method only
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theGradStep step size for numerical gradient
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult FRPRNumerical(Function&          theFunc,
                           const math_Vector& theStartingPoint,
                           double             theGradStep = 1.0e-8,
                           const FRPRConfig&  theConfig   = FRPRConfig())
{
  if (!std::isfinite(theGradStep) || theGradStep <= 0.0)
  {
    VectorResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Wrapper that adds numerical gradient
  class FuncWithGradient
  {
  public:
    FuncWithGradient(Function& theF, double theStep)
        : myFunc(theF),
          myStep(theStep)
    {
    }

    bool Value(const math_Vector& theX, double& theF) { return myFunc.Value(theX, theF); }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      math_Vector aXMod = theX;
      return MathUtils::NumericalGradientAdaptive(myFunc, aXMod, theGrad, myStep);
    }

  private:
    Function& myFunc;
    double    myStep;
  };

  FuncWithGradient aWrapper(theFunc, theGradStep);
  return FRPR(aWrapper, theStartingPoint, theConfig);
}

} // namespace MathOpt

#endif // _MathOpt_FRPR_HeaderFile

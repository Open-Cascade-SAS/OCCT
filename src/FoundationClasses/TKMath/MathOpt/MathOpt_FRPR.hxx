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

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

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
  int RestartInterval = 0; //!< Restart every N iterations (0 = n, where n is dimension)

  //! Default constructor.
  FRPRConfig() = default;

  //! Constructor with tolerance.
  explicit FRPRConfig(double theTolerance, int theMaxIter = 100)
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

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();
  const int aN     = aUpper - aLower + 1;

  // Restart interval
  const int aRestartInterval = (theConfig.RestartInterval > 0) ? theConfig.RestartInterval : aN;

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
  double aGradNormSq = 0.0;
  for (int i = aLower; i <= aUpper; ++i)
  {
    aGradNormSq += MathUtils::Sqr(aGrad(i));
  }

  if (std::sqrt(aGradNormSq) < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Search direction (initially steepest descent)
  math_Vector aDir(aLower, aUpper);
  for (int i = aLower; i <= aUpper; ++i)
  {
    aDir(i) = -aGrad(i);
  }

  // Working vectors
  math_Vector aXNew(aLower, aUpper);
  math_Vector aGradNew(aLower, aUpper);
  math_Vector aGradDiff(aLower, aUpper);

  int aRestartCount = 0;

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Line search
    MathUtils::LineSearchResult aLineResult =
      MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

    if (!aLineResult.IsValid || aLineResult.Alpha < MathUtils::THE_EPSILON)
    {
      // Line search failed, try steepest descent
      for (int i = aLower; i <= aUpper; ++i)
      {
        aDir(i) = -aGrad(i);
      }
      aLineResult = MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

      if (!aLineResult.IsValid)
      {
        aResult.Status   = Status::NotConverged;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }
      aRestartCount = 0; // Reset restart counter after steepest descent
    }

    // Compute new point
    for (int i = aLower; i <= aUpper; ++i)
    {
      aXNew(i) = aX(i) + aLineResult.Alpha * aDir(i);
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
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Check gradient convergence
    double aGradNewNormSq = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aGradNewNormSq += MathUtils::Sqr(aGradNew(i));
    }

    if (std::sqrt(aGradNewNormSq) < theConfig.FTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Compute gradient difference for some formulas
    for (int i = aLower; i <= aUpper; ++i)
    {
      aGradDiff(i) = aGradNew(i) - aGrad(i);
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
          for (int i = aLower; i <= aUpper; ++i)
          {
            aDot += aGradNew(i) * aGradDiff(i);
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
          for (int i = aLower; i <= aUpper; ++i)
          {
            aNum += aGradNew(i) * aGradDiff(i);
            aDen += aDir(i) * aGradDiff(i);
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
          for (int i = aLower; i <= aUpper; ++i)
          {
            aDen += aDir(i) * aGradDiff(i);
          }
          if (std::abs(aDen) > MathUtils::THE_ZERO_TOL)
          {
            aBeta = aGradNewNormSq / aDen;
          }
        }
        break;
      }
    }

    // Update search direction: p = -g_new + beta * p
    for (int i = aLower; i <= aUpper; ++i)
    {
      aDir(i) = -aGradNew(i) + aBeta * aDir(i);
    }

    // Check if direction is still a descent direction
    double aDirDeriv = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aDirDeriv += aGradNew(i) * aDir(i);
    }

    if (aDirDeriv >= 0.0)
    {
      // Not a descent direction, restart with steepest descent
      for (int i = aLower; i <= aUpper; ++i)
      {
        aDir(i) = -aGradNew(i);
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

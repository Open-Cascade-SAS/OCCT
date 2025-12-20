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

#ifndef _MathSys_LevenbergMarquardt_HeaderFile
#define _MathSys_LevenbergMarquardt_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathLin_Gauss.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>

namespace MathSys
{
using namespace MathUtils;

//! Configuration for Levenberg-Marquardt algorithm.
//! Extends base Config with damping parameter settings.
struct LMConfig : Config
{
  double LambdaInit     = 1.0e-3;  //!< Initial damping parameter
  double LambdaIncrease = 10.0;    //!< Factor to increase lambda on rejected step
  double LambdaDecrease = 0.1;     //!< Factor to decrease lambda on accepted step
  double LambdaMax      = 1.0e10;  //!< Maximum lambda value before failing
  double LambdaMin      = 1.0e-12; //!< Minimum lambda value

  //! Default constructor.
  LMConfig() = default;

  //! Constructor with custom tolerance.
  //! @param theTolerance convergence tolerance
  //! @param theMaxIter maximum iterations
  explicit LMConfig(double theTolerance, int theMaxIter = 100)
      : Config(theTolerance, theMaxIter)
  {
  }
};

//! Levenberg-Marquardt algorithm for nonlinear least squares.
//!
//! Minimizes ||F(X)||^2 where F is a vector function of vector X.
//! Combines Gauss-Newton method (fast near minimum) with gradient
//! descent (robust far from minimum) using adaptive damping.
//!
//! Algorithm:
//! 1. Compute F(X) and Jacobian J at current point
//! 2. Solve (J^T*J + lambda*I) * dX = -J^T*F for correction dX
//! 3. If ||F(X+dX)||^2 < ||F(X)||^2: accept step, decrease lambda
//! 4. Otherwise: reject step, increase lambda
//! 5. Repeat until convergence
//!
//! @tparam FuncSetType type with NbVariables(), NbEquations(),
//!         Value(const math_Vector& X, math_Vector& F) and
//!         Derivatives(const math_Vector& X, math_Matrix& J) or
//!         Values(const math_Vector& X, math_Vector& F, math_Matrix& J)
//! @param theFunc function set providing residuals and Jacobian
//! @param theStart initial guess vector
//! @param theConfig Levenberg-Marquardt configuration
//! @return result containing solution vector if converged
template <typename FuncSetType>
VectorResult LevenbergMarquardt(FuncSetType&       theFunc,
                                const math_Vector& theStart,
                                const LMConfig&    theConfig = LMConfig())
{
  VectorResult aResult;

  const int aNbVars = theFunc.NbVariables();
  const int aNbEqs  = theFunc.NbEquations();

  // Check dimensions
  if (theStart.Length() != aNbVars)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aVarLower = theStart.Lower();
  const int aVarUpper = theStart.Upper();

  // Working vectors and matrices
  math_Vector aSol    = theStart;
  math_Vector aF(1, aNbEqs);
  math_Vector aFNew(1, aNbEqs);
  math_Vector aDeltaX(aVarLower, aVarUpper);
  math_Vector aGrad(aVarLower, aVarUpper);
  math_Matrix aJac(1, aNbEqs, aVarLower, aVarUpper);
  math_Matrix aJtJ(aVarLower, aVarUpper, aVarLower, aVarUpper);
  math_Vector aJtF(aVarLower, aVarUpper);

  double aLambda = theConfig.LambdaInit;

  // Evaluate initial residual
  if (!theFunc.Value(aSol, aF))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Compute initial ||F||^2
  double aChi2 = 0.0;
  for (int i = 1; i <= aNbEqs; ++i)
  {
    aChi2 += aF(i) * aF(i);
  }

  // Main iteration loop
  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check F convergence
    bool aFConverged = true;
    for (int i = 1; i <= aNbEqs; ++i)
    {
      if (std::abs(aF(i)) > theConfig.FTolerance)
      {
        aFConverged = false;
        break;
      }
    }

    if (aFConverged)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aSol;
      aResult.Value    = aChi2;
      return aResult;
    }

    // Compute Jacobian
    if (!theFunc.Derivatives(aSol, aJac))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    // Compute J^T * J
    for (int i = aVarLower; i <= aVarUpper; ++i)
    {
      for (int j = aVarLower; j <= aVarUpper; ++j)
      {
        double aSum = 0.0;
        for (int k = 1; k <= aNbEqs; ++k)
        {
          aSum += aJac(k, i) * aJac(k, j);
        }
        aJtJ(i, j) = aSum;
      }
    }

    // Compute J^T * F (negative gradient of chi^2)
    for (int i = aVarLower; i <= aVarUpper; ++i)
    {
      double aSum = 0.0;
      for (int k = 1; k <= aNbEqs; ++k)
      {
        aSum += aJac(k, i) * aF(k);
      }
      aJtF(i) = aSum;
      aGrad(i) = 2.0 * aSum; // Gradient of chi^2 = 2 * J^T * F
    }

    // Check gradient convergence
    double aGradNorm = 0.0;
    for (int i = aVarLower; i <= aVarUpper; ++i)
    {
      aGradNorm += aGrad(i) * aGrad(i);
    }
    aGradNorm = std::sqrt(aGradNorm);

    if (aGradNorm < theConfig.Tolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aSol;
      aResult.Value    = aChi2;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Inner loop: try to find an acceptable step
    bool aStepAccepted = false;
    for (int aLamIter = 0; aLamIter < 20 && !aStepAccepted; ++aLamIter)
    {
      // Add damping: J^T*J + lambda*I
      math_Matrix aDamped = aJtJ;
      for (int i = aVarLower; i <= aVarUpper; ++i)
      {
        aDamped(i, i) += aLambda;
      }

      // Solve (J^T*J + lambda*I) * dX = -J^T*F
      math_Vector aNegJtF(aVarLower, aVarUpper);
      for (int i = aVarLower; i <= aVarUpper; ++i)
      {
        aNegJtF(i) = -aJtF(i);
      }

      auto aLinResult = MathLin::Solve(aDamped, aNegJtF);
      if (!aLinResult.IsDone())
      {
        // Matrix is singular, increase damping
        aLambda *= theConfig.LambdaIncrease;
        if (aLambda > theConfig.LambdaMax)
        {
          aResult.Status   = Status::Singular;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
        continue;
      }

      aDeltaX = *aLinResult.Solution;

      // Compute new solution
      math_Vector aSolNew(aVarLower, aVarUpper);
      for (int i = aVarLower; i <= aVarUpper; ++i)
      {
        aSolNew(i) = aSol(i) + aDeltaX(i);
      }

      // Evaluate new residual
      if (!theFunc.Value(aSolNew, aFNew))
      {
        // Function evaluation failed, increase damping
        aLambda *= theConfig.LambdaIncrease;
        if (aLambda > theConfig.LambdaMax)
        {
          aResult.Status   = Status::NumericalError;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
        continue;
      }

      // Compute new ||F||^2
      double aChi2New = 0.0;
      for (int i = 1; i <= aNbEqs; ++i)
      {
        aChi2New += aFNew(i) * aFNew(i);
      }

      // Accept or reject step
      if (aChi2New < aChi2)
      {
        // Step accepted
        aSol   = aSolNew;
        aF     = aFNew;
        aChi2  = aChi2New;
        aLambda *= theConfig.LambdaDecrease;
        if (aLambda < theConfig.LambdaMin)
        {
          aLambda = theConfig.LambdaMin;
        }
        aStepAccepted = true;

        // Check X convergence
        bool aXConverged = true;
        for (int i = aVarLower; i <= aVarUpper; ++i)
        {
          if (std::abs(aDeltaX(i)) > theConfig.XTolerance * (1.0 + std::abs(aSol(i))))
          {
            aXConverged = false;
            break;
          }
        }

        if (aXConverged)
        {
          aResult.Status   = Status::OK;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
      }
      else
      {
        // Step rejected, increase damping
        aLambda *= theConfig.LambdaIncrease;
        if (aLambda > theConfig.LambdaMax)
        {
          aResult.Status   = Status::NotConverged;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
      }
    }

    if (!aStepAccepted)
    {
      // Failed to find acceptable step
      aResult.Status   = Status::NotConverged;
      aResult.Solution = aSol;
      aResult.Value    = aChi2;
      return aResult;
    }
  }

  // Max iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aSol;
  aResult.Value    = aChi2;
  return aResult;
}

//! Levenberg-Marquardt with bounds constraints.
//!
//! Minimizes ||F(X)||^2 subject to theInfBound <= X <= theSupBound.
//! Solution is clamped to bounds after each step.
//!
//! @param theFunc function set providing residuals and Jacobian
//! @param theStart initial guess vector
//! @param theInfBound lower bounds for solution
//! @param theSupBound upper bounds for solution
//! @param theConfig Levenberg-Marquardt configuration
//! @return result containing solution vector if converged
template <typename FuncSetType>
VectorResult LevenbergMarquardtBounded(FuncSetType&       theFunc,
                                       const math_Vector& theStart,
                                       const math_Vector& theInfBound,
                                       const math_Vector& theSupBound,
                                       const LMConfig&    theConfig = LMConfig())
{
  VectorResult aResult;

  const int aNbVars = theFunc.NbVariables();
  const int aNbEqs  = theFunc.NbEquations();

  // Check dimensions
  if (theStart.Length() != aNbVars || theInfBound.Length() != aNbVars
      || theSupBound.Length() != aNbVars)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aVarLower = theStart.Lower();
  const int aVarUpper = theStart.Upper();

  // Working vectors and matrices
  math_Vector aSol = theStart;
  math_Vector aF(1, aNbEqs);
  math_Vector aFNew(1, aNbEqs);
  math_Vector aDeltaX(aVarLower, aVarUpper);
  math_Vector aGrad(aVarLower, aVarUpper);
  math_Matrix aJac(1, aNbEqs, aVarLower, aVarUpper);
  math_Matrix aJtJ(aVarLower, aVarUpper, aVarLower, aVarUpper);
  math_Vector aJtF(aVarLower, aVarUpper);

  // Clamp initial solution to bounds
  for (int i = aVarLower; i <= aVarUpper; ++i)
  {
    aSol(i) = MathUtils::Clamp(aSol(i), theInfBound(i), theSupBound(i));
  }

  double aLambda = theConfig.LambdaInit;

  // Evaluate initial residual
  if (!theFunc.Value(aSol, aF))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Compute initial ||F||^2
  double aChi2 = 0.0;
  for (int i = 1; i <= aNbEqs; ++i)
  {
    aChi2 += aF(i) * aF(i);
  }

  // Main iteration loop
  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check F convergence
    bool aFConverged = true;
    for (int i = 1; i <= aNbEqs; ++i)
    {
      if (std::abs(aF(i)) > theConfig.FTolerance)
      {
        aFConverged = false;
        break;
      }
    }

    if (aFConverged)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aSol;
      aResult.Value    = aChi2;
      return aResult;
    }

    // Compute Jacobian
    if (!theFunc.Derivatives(aSol, aJac))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    // Compute J^T * J
    for (int i = aVarLower; i <= aVarUpper; ++i)
    {
      for (int j = aVarLower; j <= aVarUpper; ++j)
      {
        double aSum = 0.0;
        for (int k = 1; k <= aNbEqs; ++k)
        {
          aSum += aJac(k, i) * aJac(k, j);
        }
        aJtJ(i, j) = aSum;
      }
    }

    // Compute J^T * F
    for (int i = aVarLower; i <= aVarUpper; ++i)
    {
      double aSum = 0.0;
      for (int k = 1; k <= aNbEqs; ++k)
      {
        aSum += aJac(k, i) * aF(k);
      }
      aJtF(i)  = aSum;
      aGrad(i) = 2.0 * aSum;
    }

    // Check gradient convergence
    double aGradNorm = 0.0;
    for (int i = aVarLower; i <= aVarUpper; ++i)
    {
      aGradNorm += aGrad(i) * aGrad(i);
    }
    aGradNorm = std::sqrt(aGradNorm);

    if (aGradNorm < theConfig.Tolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aSol;
      aResult.Value    = aChi2;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Inner loop: try to find an acceptable step
    bool aStepAccepted = false;
    for (int aLamIter = 0; aLamIter < 20 && !aStepAccepted; ++aLamIter)
    {
      // Add damping
      math_Matrix aDamped = aJtJ;
      for (int i = aVarLower; i <= aVarUpper; ++i)
      {
        aDamped(i, i) += aLambda;
      }

      // Solve (J^T*J + lambda*I) * dX = -J^T*F
      math_Vector aNegJtF(aVarLower, aVarUpper);
      for (int i = aVarLower; i <= aVarUpper; ++i)
      {
        aNegJtF(i) = -aJtF(i);
      }

      auto aLinResult = MathLin::Solve(aDamped, aNegJtF);
      if (!aLinResult.IsDone())
      {
        aLambda *= theConfig.LambdaIncrease;
        if (aLambda > theConfig.LambdaMax)
        {
          aResult.Status   = Status::Singular;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
        continue;
      }

      aDeltaX = *aLinResult.Solution;

      // Compute new solution with bounds clamping
      math_Vector aSolNew(aVarLower, aVarUpper);
      for (int i = aVarLower; i <= aVarUpper; ++i)
      {
        aSolNew(i) = MathUtils::Clamp(aSol(i) + aDeltaX(i), theInfBound(i), theSupBound(i));
      }

      // Evaluate new residual
      if (!theFunc.Value(aSolNew, aFNew))
      {
        aLambda *= theConfig.LambdaIncrease;
        if (aLambda > theConfig.LambdaMax)
        {
          aResult.Status   = Status::NumericalError;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
        continue;
      }

      // Compute new ||F||^2
      double aChi2New = 0.0;
      for (int i = 1; i <= aNbEqs; ++i)
      {
        aChi2New += aFNew(i) * aFNew(i);
      }

      // Accept or reject step
      if (aChi2New < aChi2)
      {
        aSol  = aSolNew;
        aF    = aFNew;
        aChi2 = aChi2New;
        aLambda *= theConfig.LambdaDecrease;
        if (aLambda < theConfig.LambdaMin)
        {
          aLambda = theConfig.LambdaMin;
        }
        aStepAccepted = true;

        // Check X convergence
        bool aXConverged = true;
        for (int i = aVarLower; i <= aVarUpper; ++i)
        {
          if (std::abs(aDeltaX(i)) > theConfig.XTolerance * (1.0 + std::abs(aSol(i))))
          {
            aXConverged = false;
            break;
          }
        }

        if (aXConverged)
        {
          aResult.Status   = Status::OK;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
      }
      else
      {
        aLambda *= theConfig.LambdaIncrease;
        if (aLambda > theConfig.LambdaMax)
        {
          aResult.Status   = Status::NotConverged;
          aResult.Solution = aSol;
          aResult.Value    = aChi2;
          return aResult;
        }
      }
    }

    if (!aStepAccepted)
    {
      aResult.Status   = Status::NotConverged;
      aResult.Solution = aSol;
      aResult.Value    = aChi2;
      return aResult;
    }
  }

  // Max iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aSol;
  aResult.Value    = aChi2;
  return aResult;
}

} // namespace MathSys

#endif // _MathSys_LevenbergMarquardt_HeaderFile

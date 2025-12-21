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

#ifndef _MathSys_Newton_HeaderFile
#define _MathSys_Newton_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathLin_Gauss.hxx>
#include <math_FunctionSetWithDerivatives.hxx>

#include <cmath>

namespace MathSys
{
using namespace MathUtils;

//! Newton-Raphson method for solving systems of nonlinear equations.
//!
//! Solves F(X) = 0 where F is a vector function of vector X.
//! The method iteratively improves an initial guess by solving
//! the linear system J(X)*dX = -F(X) where J is the Jacobian matrix.
//!
//! @param theFunc function set with derivatives (Jacobian)
//! @param theStart initial guess vector
//! @param theTolX tolerance for solution change ||X(n+1) - X(n)|| < tolX
//! @param theTolF tolerance for function values ||F(X)|| < tolF
//! @param theMaxIter maximum number of iterations
//! @return result containing solution vector if converged
template <typename FuncSetType>
VectorResult Newton(FuncSetType&       theFunc,
                    const math_Vector& theStart,
                    const math_Vector& theTolX,
                    double             theTolF,
                    size_t             theMaxIter = 100)
{
  VectorResult aResult;

  const int aN = theFunc.NbVariables();
  const int aM = theFunc.NbEquations();

  // Check dimensions
  if (aN != aM || theStart.Length() != aN || theTolX.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aLower = theStart.Lower();
  const int aUpper = theStart.Upper();

  // Working vectors
  math_Vector aSol = theStart;
  math_Vector aF(aLower, aUpper);
  math_Vector aDeltaX(aLower, aUpper);
  math_Matrix aJacobian(aLower, aUpper, aLower, aUpper);

  // Newton iteration
  for (size_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    // Evaluate function and Jacobian
    if (!theFunc.Values(aSol, aF, aJacobian))
    {
      aResult.Status       = Status::NumericalError;
      aResult.NbIterations = anIter;
      return aResult;
    }

    // Solve J * dX = -F
    // Negate F for the right-hand side
    math_Vector aNegF(aLower, aUpper);
    for (int i = aLower; i <= aUpper; ++i)
    {
      aNegF(i) = -aF(i);
    }

    auto aLinResult = MathLin::Solve(aJacobian, aNegF);
    if (!aLinResult.IsDone())
    {
      aResult.Status       = Status::Singular;
      aResult.NbIterations = anIter;
      return aResult;
    }

    aDeltaX = *aLinResult.Solution;

    // Update solution
    for (int i = aLower; i <= aUpper; ++i)
    {
      aSol(i) += aDeltaX(i);
    }

    // Check convergence
    bool aXConverged = true;
    bool aFConverged = true;

    for (int i = aLower; i <= aUpper; ++i)
    {
      if (std::abs(aDeltaX(i)) > theTolX(i))
      {
        aXConverged = false;
      }
      if (std::abs(aF(i)) > theTolF)
      {
        aFConverged = false;
      }
    }

    if (aXConverged && aFConverged)
    {
      aResult.Status       = Status::OK;
      aResult.NbIterations = anIter + 1;
      aResult.Solution     = aSol;
      aResult.Jacobian     = aJacobian;
      return aResult;
    }

    aResult.NbIterations = anIter + 1;
  }

  // Max iterations reached - return last solution
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aSol;
  aResult.Jacobian = aJacobian;
  return aResult;
}

//! Newton-Raphson method with bounds constraints.
//!
//! Solves F(X) = 0 subject to InfBound <= X <= SupBound.
//! If the Newton step would take X outside bounds, the solution
//! is clamped to the boundary.
//!
//! @param theFunc function set with derivatives (Jacobian)
//! @param theStart initial guess vector
//! @param theInfBound lower bounds for solution
//! @param theSupBound upper bounds for solution
//! @param theTolX tolerance for solution change
//! @param theTolF tolerance for function values
//! @param theMaxIter maximum number of iterations
//! @return result containing solution vector if converged
template <typename FuncSetType>
VectorResult NewtonBounded(FuncSetType&       theFunc,
                           const math_Vector& theStart,
                           const math_Vector& theInfBound,
                           const math_Vector& theSupBound,
                           const math_Vector& theTolX,
                           double             theTolF,
                           size_t             theMaxIter = 100)
{
  VectorResult aResult;

  const int aN = theFunc.NbVariables();
  const int aM = theFunc.NbEquations();

  // Check dimensions
  if (aN != aM || theStart.Length() != aN || theTolX.Length() != aN || theInfBound.Length() != aN
      || theSupBound.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aLower = theStart.Lower();
  const int aUpper = theStart.Upper();

  // Working vectors
  math_Vector aSol = theStart;
  math_Vector aF(aLower, aUpper);
  math_Vector aDeltaX(aLower, aUpper);
  math_Matrix aJacobian(aLower, aUpper, aLower, aUpper);

  // Clamp initial solution to bounds
  for (int i = aLower; i <= aUpper; ++i)
  {
    if (aSol(i) < theInfBound(i))
    {
      aSol(i) = theInfBound(i);
    }
    if (aSol(i) > theSupBound(i))
    {
      aSol(i) = theSupBound(i);
    }
  }

  // Newton iteration
  for (size_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    // Evaluate function and Jacobian
    if (!theFunc.Values(aSol, aF, aJacobian))
    {
      aResult.Status       = Status::NumericalError;
      aResult.NbIterations = anIter;
      return aResult;
    }

    // Solve J * dX = -F
    math_Vector aNegF(aLower, aUpper);
    for (int i = aLower; i <= aUpper; ++i)
    {
      aNegF(i) = -aF(i);
    }

    auto aLinResult = MathLin::Solve(aJacobian, aNegF);
    if (!aLinResult.IsDone())
    {
      aResult.Status       = Status::Singular;
      aResult.NbIterations = anIter;
      return aResult;
    }

    aDeltaX = *aLinResult.Solution;

    // Update solution with bounds clamping
    for (int i = aLower; i <= aUpper; ++i)
    {
      aSol(i) += aDeltaX(i);
      if (aSol(i) < theInfBound(i))
      {
        aSol(i) = theInfBound(i);
      }
      if (aSol(i) > theSupBound(i))
      {
        aSol(i) = theSupBound(i);
      }
    }

    // Check convergence
    bool aXConverged = true;
    bool aFConverged = true;

    for (int i = aLower; i <= aUpper; ++i)
    {
      if (std::abs(aDeltaX(i)) > theTolX(i))
      {
        aXConverged = false;
      }
      if (std::abs(aF(i)) > theTolF)
      {
        aFConverged = false;
      }
    }

    if (aXConverged && aFConverged)
    {
      aResult.Status       = Status::OK;
      aResult.NbIterations = anIter + 1;
      aResult.Solution     = aSol;
      aResult.Jacobian     = aJacobian;
      return aResult;
    }

    aResult.NbIterations = anIter + 1;
  }

  // Max iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aSol;
  aResult.Jacobian = aJacobian;
  return aResult;
}

//! Simplified Newton method with uniform tolerances.
//!
//! @param theFunc function set with derivatives
//! @param theStart initial guess vector
//! @param theTolX uniform tolerance for all variables
//! @param theTolF tolerance for function values
//! @param theMaxIter maximum number of iterations
//! @return result containing solution vector if converged
template <typename FuncSetType>
VectorResult Newton(FuncSetType&       theFunc,
                    const math_Vector& theStart,
                    double             theTolX,
                    double             theTolF,
                    size_t             theMaxIter = 100)
{
  math_Vector aTolXVec(theStart.Lower(), theStart.Upper(), theTolX);
  return Newton(theFunc, theStart, aTolXVec, theTolF, theMaxIter);
}

} // namespace MathSys

#endif // _MathSys_Newton_HeaderFile

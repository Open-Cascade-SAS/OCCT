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

#ifndef _MathSys_Newton2D_HeaderFile
#define _MathSys_Newton2D_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>

#include <cmath>
#include <algorithm>

//! @file MathSys_Newton2D.hxx
//! @brief Optimized 2D Newton-Raphson solver for systems of 2 equations in 2 unknowns.
//!
//! This solver is specifically optimized for 2D problems like:
//! - Point-surface extrema (find u,v where gradient is zero)
//! - Curve intersection (find t1,t2 where curves meet)
//! - Surface intersection curves
//!
//! Optimizations compared to general Newton:
//! - No math_Vector/math_Matrix allocation overhead
//! - Cramer's rule for 2x2 system (faster than LU decomposition)
//! - Squared norm comparisons (avoid sqrt in convergence check)
//! - Step limiting to prevent wild oscillations
//! - Gradient descent fallback for singular Jacobian

namespace MathSys
{
using namespace MathUtils;

//! Result of 2D Newton iteration.
struct Newton2DResult
{
  MathUtils::Status Status = MathUtils::Status::NotConverged; //!< Computation status
  double            U      = 0.0;                             //!< Solution U coordinate
  double            V      = 0.0;                             //!< Solution V coordinate
  size_t            NbIter = 0;                               //!< Number of iterations performed
  double            FNorm  = 0.0;                             //!< Final |F| norm

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Optimized 2D Newton-Raphson solver with bounds.
//!
//! Solves the system [F1, F2] = [0, 0] using Newton-Raphson iteration
//! with Cramer's rule for the 2x2 linear system at each step.
//!
//! The function type must provide a method:
//! @code
//!   bool ValueAndJacobian(double theU, double theV,
//!                         double& theF1, double& theF2,
//!                         double& theDF1dU, double& theDF1dV,
//!                         double& theDF2dU, double& theDF2dV) const;
//! @endcode
//!
//! @tparam Function type with ValueAndJacobian method
//! @param theFunc function to solve (provides F and Jacobian)
//! @param theU0 initial U guess
//! @param theV0 initial V guess
//! @param theUMin U lower bound
//! @param theUMax U upper bound
//! @param theVMin V lower bound
//! @param theVMax V upper bound
//! @param theTol tolerance for convergence (|F| < tol)
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton2DResult containing solution if converged
template <typename Function>
Newton2DResult Newton2D(const Function& theFunc,
                        double          theU0,
                        double          theV0,
                        double          theUMin,
                        double          theUMax,
                        double          theVMin,
                        double          theVMax,
                        double          theTol,
                        size_t          theMaxIter = 20)
{
  Newton2DResult aRes;
  aRes.U = theU0;
  aRes.V = theV0;

  // Pre-compute max step limit once
  const double aMaxStep = 0.5 * std::max(theUMax - theUMin, theVMax - theVMin);
  const double aTolSq   = theTol * theTol;

  for (size_t i = 0; i < theMaxIter; ++i)
  {
    aRes.NbIter = i + 1;

    double aF1, aF2, aDF1dU, aDF1dV, aDF2dU, aDF2dV;
    if (!theFunc.ValueAndJacobian(aRes.U, aRes.V, aF1, aF2, aDF1dU, aDF1dV, aDF2dU, aDF2dV))
    {
      aRes.Status = Status::NumericalError;
      return aRes;
    }

    // Check convergence using squared norm (avoid sqrt)
    const double aFNormSq = aF1 * aF1 + aF2 * aF2;

    if (aFNormSq < aTolSq)
    {
      aRes.FNorm  = std::sqrt(aFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }

    // Solve 2x2 linear system: J * [dU, dV]^T = -[F1, F2]^T
    // J = [[DF1dU, DF1dV], [DF2dU, DF2dV]]
    const double aDet = aDF1dU * aDF2dV - aDF1dV * aDF2dU;

    double aDU, aDV;
    if (std::abs(aDet) < 1.0e-30)
    {
      // Singular Jacobian - try gradient descent step
      const double aNormSq = aDF1dU * aDF1dU + aDF1dV * aDF1dV + aDF2dU * aDF2dU + aDF2dV * aDF2dV;
      if (aNormSq < 1.0e-60)
      {
        aRes.FNorm  = std::sqrt(aFNormSq);
        aRes.Status = Status::Singular;
        return aRes;
      }
      const double aStep = std::sqrt(aFNormSq / aNormSq) * 0.1;
      // Use steepest descent direction: -J^T * F
      aDU = -aStep * (aDF1dU * aF1 + aDF2dU * aF2);
      aDV = -aStep * (aDF1dV * aF1 + aDF2dV * aF2);
    }
    else
    {
      // Cramer's rule (optimized: compute inverse determinant once)
      const double aInvDet = 1.0 / aDet;
      aDU                  = (-aF1 * aDF2dV + aF2 * aDF1dV) * aInvDet;
      aDV                  = (-aF2 * aDF1dU + aF1 * aDF2dU) * aInvDet;

      // Limit step size (optimized: avoid sqrt when possible)
      const double aStepNormSq = aDU * aDU + aDV * aDV;
      if (aStepNormSq > aMaxStep * aMaxStep)
      {
        const double aScale = aMaxStep / std::sqrt(aStepNormSq);
        aDU *= aScale;
        aDV *= aScale;
      }
    }

    // Update and clamp to bounds
    aRes.U = std::max(theUMin, std::min(theUMax, aRes.U + aDU));
    aRes.V = std::max(theVMin, std::min(theVMax, aRes.V + aDV));
  }

  // Final convergence check (only at max iterations)
  double aF1, aF2, aDF1dU, aDF1dV, aDF2dU, aDF2dV;
  if (theFunc.ValueAndJacobian(aRes.U, aRes.V, aF1, aF2, aDF1dU, aDF1dV, aDF2dU, aDF2dV))
  {
    aRes.FNorm = std::sqrt(aF1 * aF1 + aF2 * aF2);
    if (aRes.FNorm < theTol)
    {
      aRes.Status = Status::OK;
    }
    else
    {
      aRes.Status = Status::MaxIterations;
    }
  }
  else
  {
    aRes.Status = Status::NumericalError;
  }

  return aRes;
}

//! Unbounded 2D Newton-Raphson solver.
//!
//! @tparam Function type with ValueAndJacobian method
//! @param theFunc function to solve
//! @param theU0 initial U guess
//! @param theV0 initial V guess
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton2DResult containing solution if converged
template <typename Function>
Newton2DResult Newton2D(const Function& theFunc,
                        double          theU0,
                        double          theV0,
                        double          theTol,
                        size_t          theMaxIter = 20)
{
  constexpr double aInf = 1.0e100;
  return Newton2D(theFunc, theU0, theV0, -aInf, aInf, -aInf, aInf, theTol, theMaxIter);
}

//! Optimized 2D Newton solver for symmetric Jacobian with backtracking line search.
//!
//! Many 2D problems have symmetric Jacobians (e.g., gradient of scalar function).
//! This variant is optimized for the symmetric case where J12 = J21.
//! Convergence tolerances match math_FunctionSetRoot for high precision.
//!
//! Features:
//! - Backtracking Armijo line search for robust convergence
//! - Gradient descent fallback for singular/ill-conditioned Jacobian
//! - Step size limiting to prevent wild oscillations
//!
//! The function type must provide methods:
//! @code
//!   bool ValueAndJacobian(double theU, double theV,
//!                         double& theF1, double& theF2,
//!                         double& theJ11, double& theJ12, double& theJ22) const;
//!   bool Value(double theU, double theV, double& theF1, double& theF2) const;
//! @endcode
//! where J = [[J11, J12], [J12, J22]] (symmetric).
//!
//! @tparam Function type with symmetric ValueAndJacobian method
//! @param theFunc function to solve
//! @param theU0 initial U guess
//! @param theV0 initial V guess
//! @param theUMin U lower bound
//! @param theUMax U upper bound
//! @param theVMin V lower bound
//! @param theVMax V upper bound
//! @param theTol tolerance for final convergence check
//! @param theMaxIter maximum iterations
//! @return Newton2DResult containing solution if converged
template <typename Function>
Newton2DResult Newton2DSymmetric(const Function& theFunc,
                                 double          theU0,
                                 double          theV0,
                                 double          theUMin,
                                 double          theUMax,
                                 double          theVMin,
                                 double          theVMax,
                                 double          theTol,
                                 size_t          theMaxIter = THE_NEWTON_MAX_ITER)
{
  Newton2DResult aRes;
  aRes.U = theU0;
  aRes.V = theV0;

  const double aMaxStep  = 0.5 * std::max(theUMax - theUMin, theVMax - theVMin);
  const double aStepTolU = THE_NEWTON_STEP_TOL_FACTOR * (theUMax - theUMin);
  const double aStepTolV = THE_NEWTON_STEP_TOL_FACTOR * (theVMax - theVMin);
  const double aFTolSq   = THE_NEWTON_FTOL_SQ;

  // Line search parameters
  constexpr double THE_BACKTRACK_RHO = 0.5; // Step reduction factor
  constexpr int    THE_MAX_BACKTRACK = 10;  // Maximum backtracking iterations

  for (size_t i = 0; i < theMaxIter; ++i)
  {
    aRes.NbIter = i + 1;

    double aF1, aF2, aJ11, aJ12, aJ22;
    if (!theFunc.ValueAndJacobian(aRes.U, aRes.V, aF1, aF2, aJ11, aJ12, aJ22))
    {
      aRes.Status = Status::NumericalError;
      return aRes;
    }

    const double aFNormSq = aF1 * aF1 + aF2 * aF2;
    if (aFNormSq < aFTolSq)
    {
      aRes.FNorm  = std::sqrt(aFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }

    // Solve 2x2 symmetric linear system: J * [dU, dV]^T = -[F1, F2]^T
    const double aDet = aJ11 * aJ22 - aJ12 * aJ12;

    double aDU, aDV;
    bool   aNeedLineSearch = false; // Only use line search when step is unreliable

    if (std::abs(aDet) < 1.0e-30)
    {
      // Singular Jacobian - use gradient descent direction
      // For F(u,v) = 0.5*(f1^2 + f2^2), gradient is J^T * F
      const double aGradU  = aJ11 * aF1 + aJ12 * aF2;
      const double aGradV  = aJ12 * aF1 + aJ22 * aF2;
      const double aGradSq = aGradU * aGradU + aGradV * aGradV;

      if (aGradSq < 1.0e-60)
      {
        aRes.FNorm  = std::sqrt(aFNormSq);
        aRes.Status = Status::Singular;
        return aRes;
      }

      // Steepest descent: step = -alpha * grad, alpha chosen to give reasonable step
      const double aAlpha = aFNormSq / aGradSq; // Optimal for quadratic
      aDU                 = -aAlpha * aGradU;
      aDV                 = -aAlpha * aGradV;
      aNeedLineSearch     = true; // Gradient descent needs line search
    }
    else
    {
      // Cramer's rule for symmetric matrix
      const double aInvDet = 1.0 / aDet;
      aDU                  = (-aF1 * aJ22 + aF2 * aJ12) * aInvDet;
      aDV                  = (-aF2 * aJ11 + aF1 * aJ12) * aInvDet;
    }

    // Limit step size to prevent wild jumps
    const double aStepNormSq = aDU * aDU + aDV * aDV;
    if (aStepNormSq > aMaxStep * aMaxStep)
    {
      const double aScale = aMaxStep / std::sqrt(aStepNormSq);
      aDU *= aScale;
      aDV *= aScale;
      aNeedLineSearch = true; // Limited step needs line search
    }

    // Compute new position
    double aNewU = aRes.U + aDU;
    double aNewV = aRes.V + aDV;

    // Clamp to bounds
    aNewU = std::max(theUMin, std::min(theUMax, aNewU));
    aNewV = std::max(theVMin, std::min(theVMax, aNewV));

    double aNewFNormSq = 0.0;

    // Only use line search when step is unreliable (gradient descent or limited step)
    if (aNeedLineSearch)
    {
      double aNewF1, aNewF2;
      if (!theFunc.Value(aNewU, aNewV, aNewF1, aNewF2))
      {
        aRes.Status = Status::NumericalError;
        return aRes;
      }
      aNewFNormSq = aNewF1 * aNewF1 + aNewF2 * aNewF2;

      // Backtrack if step increased the function value significantly
      if (aNewFNormSq > aFNormSq * 1.5) // Allow some increase (1.5x) before backtracking
      {
        double aAlpha = THE_BACKTRACK_RHO;
        for (int k = 0; k < THE_MAX_BACKTRACK - 1; ++k)
        {
          aNewU = aRes.U + aAlpha * aDU;
          aNewV = aRes.V + aAlpha * aDV;
          aNewU = std::max(theUMin, std::min(theUMax, aNewU));
          aNewV = std::max(theVMin, std::min(theVMax, aNewV));

          if (!theFunc.Value(aNewU, aNewV, aNewF1, aNewF2))
          {
            aAlpha *= THE_BACKTRACK_RHO;
            continue;
          }

          aNewFNormSq = aNewF1 * aNewF1 + aNewF2 * aNewF2;
          if (aNewFNormSq < aFNormSq * 1.5)
          {
            break; // Accept step
          }
          aAlpha *= THE_BACKTRACK_RHO;
        }
      }
    }

    // Update position
    const double aOldU = aRes.U;
    const double aOldV = aRes.V;
    aRes.U             = aNewU;
    aRes.V             = aNewV;

    // Compute actual step taken (after clamping)
    const double aActualDU = aRes.U - aOldU;
    const double aActualDV = aRes.V - aOldV;

    // Parametric convergence: step is negligible (matching math_FunctionSetRoot)
    if (std::abs(aActualDU) < aStepTolU && std::abs(aActualDV) < aStepTolV)
    {
      // Compute final FNorm if not already computed by line search
      if (!aNeedLineSearch)
      {
        double aNewF1, aNewF2;
        if (theFunc.Value(aRes.U, aRes.V, aNewF1, aNewF2))
        {
          aNewFNormSq = aNewF1 * aNewF1 + aNewF2 * aNewF2;
        }
      }
      aRes.FNorm  = std::sqrt(aNewFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }
  }

  // Final convergence check using user tolerance
  double aF1, aF2, aJ11, aJ12, aJ22;
  if (theFunc.ValueAndJacobian(aRes.U, aRes.V, aF1, aF2, aJ11, aJ12, aJ22))
  {
    aRes.FNorm = std::sqrt(aF1 * aF1 + aF2 * aF2);
    if (aRes.FNorm < theTol)
    {
      aRes.Status = Status::OK;
    }
    else
    {
      aRes.Status = Status::MaxIterations;
    }
  }
  else
  {
    aRes.Status = Status::NumericalError;
  }

  return aRes;
}

} // namespace MathSys

#endif // _MathSys_Newton2D_HeaderFile

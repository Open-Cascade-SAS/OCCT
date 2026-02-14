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

namespace MathSys
{
using namespace MathUtils;

//! Solve 2x2 symmetric system using eigenvalue decomposition (SVD fallback).
//! More robust than Cramer's rule for ill-conditioned matrices.
//! @param theJ11 J(1,1) element
//! @param theJ12 J(1,2) = J(2,1) element
//! @param theJ22 J(2,2) element
//! @param theF1 RHS component 1
//! @param theF2 RHS component 2
//! @param theDU output: solution U component
//! @param theDV output: solution V component
//! @param theTol tolerance for eigenvalue regularization
//! @return true if solution found
inline bool SolveSymmetric2x2SVD(double theJ11, double theJ12, double theJ22,
                                  double theF1, double theF2,
                                  double& theDU, double& theDV,
                                  double theTol = 1.0e-15)
{
  // For symmetric 2x2 matrix J = [[a,b],[b,c]], eigenvalues are:
  // λ1,2 = (a+c)/2 ± sqrt(((a-c)/2)² + b²)
  const double aTrace   = theJ11 + theJ22;
  const double aDiff    = (theJ11 - theJ22) * 0.5;
  const double aDiscrim = std::sqrt(aDiff * aDiff + theJ12 * theJ12);

  double aLambda1 = aTrace * 0.5 + aDiscrim;
  double aLambda2 = aTrace * 0.5 - aDiscrim;

  // Regularize small eigenvalues (truncated SVD)
  const double aMinLambda = std::max(std::abs(aLambda1), std::abs(aLambda2)) * theTol;
  if (std::abs(aLambda1) < aMinLambda && std::abs(aLambda2) < aMinLambda)
  {
    return false; // Both eigenvalues effectively zero
  }

  // Eigenvector for lambda1: (b, lambda1 - a) normalized, or (lambda1 - c, b) normalized
  // Use the formula that gives better numerical stability
  double aV1x, aV1y;
  if (std::abs(theJ12) > std::abs(aDiff))
  {
    const double aLen1 = std::sqrt(theJ12 * theJ12 + (aLambda1 - theJ11) * (aLambda1 - theJ11));
    if (aLen1 < theTol) return false;
    aV1x = theJ12 / aLen1;
    aV1y = (aLambda1 - theJ11) / aLen1;
  }
  else
  {
    const double aLen1 = std::sqrt((aLambda1 - theJ22) * (aLambda1 - theJ22) + theJ12 * theJ12);
    if (aLen1 < theTol) return false;
    aV1x = (aLambda1 - theJ22) / aLen1;
    aV1y = theJ12 / aLen1;
  }

  // Eigenvector for lambda2 is orthogonal: (-v1y, v1x)
  double aV2x = -aV1y;
  double aV2y = aV1x;

  // Project RHS onto eigenvector basis
  double aB1 = aV1x * (-theF1) + aV1y * (-theF2);
  double aB2 = aV2x * (-theF1) + aV2y * (-theF2);

  // Solve in eigenspace with regularization
  double aX1 = (std::abs(aLambda1) > aMinLambda) ? aB1 / aLambda1 : 0.0;
  double aX2 = (std::abs(aLambda2) > aMinLambda) ? aB2 / aLambda2 : 0.0;

  // Transform back to original space
  theDU = aV1x * aX1 + aV2x * aX2;
  theDV = aV1y * aX1 + aV2y * aX2;

  return true;
}

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
  const double aMaxStep = THE_NEWTON2D_MAX_STEP_RATIO * std::max(theUMax - theUMin, theVMax - theVMin);
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
    if (std::abs(aDet) < THE_NEWTON2D_SINGULAR_DET)
    {
      // Singular Jacobian - try gradient descent step
      const double aNormSq = aDF1dU * aDF1dU + aDF1dV * aDF1dV + aDF2dU * aDF2dU + aDF2dV * aDF2dV;
      if (aNormSq < THE_NEWTON2D_CRITICAL_GRAD_SQ)
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

//! Robust 2D Newton solver for symmetric Jacobian (point-surface extrema).
//!
//! This solver is designed for finding extrema of the squared distance function
//! from a point to a surface. It uses unconstrained Newton iteration internally
//! and only applies bounds at the final result.
//!
//! Key features matching math_FunctionSetRoot behavior:
//! - Convergence based on FUNCTION VALUE (gradient norm), not step size
//! - Allows solutions slightly outside domain if gradient is small
//! - Stagnation detection to avoid infinite loops
//! - Adaptive step limiting
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
//! @param theUMin U lower bound (soft constraint)
//! @param theUMax U upper bound (soft constraint)
//! @param theVMin V lower bound (soft constraint)
//! @param theVMax V upper bound (soft constraint)
//! @param theTol tolerance for gradient convergence
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

  // Domain size for step limiting
  const double aDomainU = theUMax - theUMin;
  const double aDomainV = theVMax - theVMin;
  const double aMaxStep = THE_NEWTON2D_MAX_STEP_RATIO * std::max(aDomainU, aDomainV);

  // Convergence tolerances
  // Use user tolerance for gradient, not hardcoded value
  const double aGradTolSq = theTol * theTol;

  // Small extension beyond domain (matching OLD behavior)
  // OLD allows solutions slightly outside domain if gradient is near zero
  const double aExtU = THE_NEWTON2D_DOMAIN_EXT * aDomainU;
  const double aExtV = THE_NEWTON2D_DOMAIN_EXT * aDomainV;
  const double aExtUMin = theUMin - aExtU;
  const double aExtUMax = theUMax + aExtU;
  const double aExtVMin = theVMin - aExtV;
  const double aExtVMax = theVMax + aExtV;

  // Track function value for stagnation detection
  double aPrevFNormSq = 1.0e100;
  int    aStagnationCount = 0;

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

    // PRIMARY CONVERGENCE: gradient is small enough
    if (aFNormSq < aGradTolSq)
    {
      aRes.FNorm  = std::sqrt(aFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }

    // Stagnation detection: if function not improving, may be stuck
    if (aFNormSq >= aPrevFNormSq * THE_NEWTON2D_STAGNATION_RATIO) // Less than 0.1% improvement
    {
      aStagnationCount++;
      if (aStagnationCount >= THE_NEWTON2D_STAGNATION_COUNT)
      {
        // Stagnated - accept current position if gradient is reasonably small
        aRes.FNorm = std::sqrt(aFNormSq);
        if (aRes.FNorm < theTol * THE_NEWTON2D_STAGNATION_RELAX) // Relaxed tolerance for stagnation
        {
          aRes.Status = Status::OK;
        }
        else
        {
          aRes.Status = Status::MaxIterations;
        }
        return aRes;
      }
    }
    else
    {
      aStagnationCount = 0;
    }
    aPrevFNormSq = aFNormSq;

    // Solve 2x2 symmetric linear system: J * [dU, dV]^T = -[F1, F2]^T
    const double aDet = aJ11 * aJ22 - aJ12 * aJ12;

    double aDU, aDV;

    if (std::abs(aDet) < THE_NEWTON2D_SINGULAR_DET)
    {
      // Try SVD-based solve first (more robust for ill-conditioned matrices)
      bool aSVDSuccess = SolveSymmetric2x2SVD(aJ11, aJ12, aJ22, aF1, aF2, aDU, aDV);

      if (!aSVDSuccess)
      {
        // SVD failed - fall back to gradient descent
        // For merit function phi = 0.5*(F1^2 + F2^2), gradient is [J11*F1 + J12*F2, J12*F1 + J22*F2]
        const double aGradU  = aJ11 * aF1 + aJ12 * aF2;
        const double aGradV  = aJ12 * aF1 + aJ22 * aF2;
        const double aGradSq = aGradU * aGradU + aGradV * aGradV;

        if (aGradSq < THE_NEWTON2D_CRITICAL_GRAD_SQ)
        {
          // Gradient is essentially zero - we're at a critical point
          aRes.FNorm  = std::sqrt(aFNormSq);
          aRes.Status = (aRes.FNorm < theTol * THE_NEWTON2D_MAXITER_RELAX) ? Status::OK : Status::Singular;
          return aRes;
        }

        // Steepest descent with adaptive step size
        // Use Barzilai-Borwein-like step: alpha = |F|^2 / |grad|^2
        double aAlpha = aFNormSq / aGradSq;
        aAlpha = std::min(aAlpha, aMaxStep / std::sqrt(aGradSq)); // Limit step
        aDU = -aAlpha * aGradU;
        aDV = -aAlpha * aGradV;
      }
      else
      {
        // SVD succeeded - limit step size
        const double aStepNorm = std::sqrt(aDU * aDU + aDV * aDV);
        if (aStepNorm > aMaxStep)
        {
          const double aScale = aMaxStep / aStepNorm;
          aDU *= aScale;
          aDV *= aScale;
        }
      }
    }
    else
    {
      // Cramer's rule for symmetric matrix
      const double aInvDet = 1.0 / aDet;
      aDU = (-aF1 * aJ22 + aF2 * aJ12) * aInvDet;
      aDV = (-aF2 * aJ11 + aF1 * aJ12) * aInvDet;

      // Limit step size to prevent wild jumps
      const double aStepNorm = std::sqrt(aDU * aDU + aDV * aDV);
      if (aStepNorm > aMaxStep)
      {
        const double aScale = aMaxStep / aStepNorm;
        aDU *= aScale;
        aDV *= aScale;
      }
    }

    // Update position - allow slight overshoot beyond domain
    double aNewU = aRes.U + aDU;
    double aNewV = aRes.V + aDV;

    // Soft clamping: allow small extension beyond domain
    aNewU = std::max(aExtUMin, std::min(aExtUMax, aNewU));
    aNewV = std::max(aExtVMin, std::min(aExtVMax, aNewV));

    // Step size relative to domain - used for early termination and line search skip
    const double aStepNormSq = aDU * aDU + aDV * aDV;
    const double aDomainSizeSq = aDomainU * aDomainU + aDomainV * aDomainV;
    const double aRelStepSq = aStepNormSq / std::max(aDomainSizeSq, THE_HESSIAN_DEGENERACY_ABS);

    // Early convergence: if step is very small relative to domain, we're converged
    if (aRelStepSq < THE_NEWTON_STEP_TOL_FACTOR * THE_NEWTON_STEP_TOL_FACTOR)
    {
      aRes.FNorm  = std::sqrt(aFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }

    // Skip line search for small steps - Newton is working well
    // This avoids expensive Value() calls when not needed
    const bool aSkipLineSearch = (aRelStepSq < THE_NEWTON2D_SKIP_LINESEARCH_SQ);

    // Line search if step increases function value significantly
    double aNewF1, aNewF2;
    if (!aSkipLineSearch && theFunc.Value(aNewU, aNewV, aNewF1, aNewF2))
    {
      double aNewFNormSq = aNewF1 * aNewF1 + aNewF2 * aNewF2;

      // Compute directional derivative for Armijo condition: grad_f . d
      // For f = 0.5*(F1^2 + F2^2), grad_f = [F1, F2] (simplified)
      // d = [dU, dV], so grad_f . d = F1*dU + F2*dV
      const double aDirectionalDeriv = aF1 * aDU + aF2 * aDV;

      // Backtrack if function increased too much OR Armijo not satisfied
      // Armijo: f(x + d) <= f(x) + c1 * grad_f . d
      const bool aArmijoFailed = (aNewFNormSq > aFNormSq + THE_ARMIJO_C1 * aDirectionalDeriv);
      if (aNewFNormSq > aFNormSq * THE_NEWTON2D_BACKTRACK_TRIGGER || aArmijoFailed)
      {
        // Use quadratic interpolation for first estimate
        // Model: f(α) ≈ f(0) + f'(0)*α + c*α², where c = (f(1) - f(0) - f'(0))/1
        // Directional derivative f'(0) = grad·d where d is search direction
        // For squared norm: grad = 2*[F1, F2], d = [dU, dV]
        // f'(0) = 2*(F1*dU_from_F1 + F2*dU_from_F2) ... but we approximate
        // Using f(0), f(1) and assuming quadratic, min at α = f(0) / (2*(f(1) - f(0) + f(0)))
        // Simplified: α_opt ≈ 0.5 * f(0) / (f(1) - f(0) + f(0)) = 0.5 * f(0) / f(1)
        // But safer: α_opt = f(0) / (f(1) - f(0)) / 2, clamped to [0.1, 0.5]

        double aAlpha = 0.5;
        if (aNewFNormSq > aFNormSq)
        {
          // Quadratic interpolation: assume f(α) = a + b*α + c*α²
          // f(0) = a = aFNormSq
          // f(1) = a + b + c = aNewFNormSq
          // Minimum of quadratic at α = -b/(2c)
          // Approximating b ≈ -(f(0)), c ≈ f(1) - f(0) + f(0) = f(1)
          // gives α ≈ f(0)/(2*f(1)), clamped for safety
          double aAlphaQuad = aFNormSq / (2.0 * aNewFNormSq);
          aAlpha = std::max(0.1, std::min(0.5, aAlphaQuad));
        }

        for (int k = 0; k < THE_NEWTON2D_LINE_SEARCH_MAX; ++k)
        {
          double aTryU = aRes.U + aAlpha * aDU;
          double aTryV = aRes.V + aAlpha * aDV;
          aTryU = std::max(aExtUMin, std::min(aExtUMax, aTryU));
          aTryV = std::max(aExtVMin, std::min(aExtVMax, aTryV));

          double aTryF1, aTryF2;
          if (theFunc.Value(aTryU, aTryV, aTryF1, aTryF2))
          {
            double aTryFNormSq = aTryF1 * aTryF1 + aTryF2 * aTryF2;
            // Accept if Armijo condition satisfied: f(x + α*d) <= f(x) + c1*α*(grad·d)
            const double aArmijoThreshold = aFNormSq + THE_ARMIJO_C1 * aAlpha * aDirectionalDeriv;
            if (aTryFNormSq <= aArmijoThreshold || aTryFNormSq < aFNormSq * THE_NEWTON2D_BACKTRACK_ACCEPT)
            {
              aNewU = aTryU;
              aNewV = aTryV;
              break;
            }

            // Update quadratic estimate for next iteration
            // New model: interpolate between f(0) and f(α)
            if (aTryFNormSq > aFNormSq && k < THE_NEWTON2D_LINE_SEARCH_MAX - 1)
            {
              double aAlphaQuad = aAlpha * aFNormSq / (2.0 * aTryFNormSq);
              aAlpha = std::max(aAlpha * 0.25, std::min(aAlpha * 0.5, aAlphaQuad));
            }
            else
            {
              aAlpha *= 0.5;
            }
          }
          else
          {
            aAlpha *= 0.5;
          }
        }
      }
    }

    aRes.U = aNewU;
    aRes.V = aNewV;
  }

  // Final evaluation
  double aF1, aF2, aJ11, aJ12, aJ22;
  if (theFunc.ValueAndJacobian(aRes.U, aRes.V, aF1, aF2, aJ11, aJ12, aJ22))
  {
    aRes.FNorm = std::sqrt(aF1 * aF1 + aF2 * aF2);

    // Accept if gradient is reasonably small (relaxed for max iterations)
    if (aRes.FNorm < theTol * THE_NEWTON2D_MAXITER_RELAX)
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

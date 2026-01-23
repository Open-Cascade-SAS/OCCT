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

//! Optimized 2D Newton solver for symmetric Jacobian.
//!
//! Many 2D problems have symmetric Jacobians (e.g., gradient of scalar function).
//! This variant is optimized for the symmetric case where J12 = J21.
//!
//! The function type must provide a method:
//! @code
//!   bool ValueAndJacobian(double theU, double theV,
//!                         double& theF1, double& theF2,
//!                         double& theJ11, double& theJ12, double& theJ22) const;
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
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations (default 20)
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

    double aF1, aF2, aJ11, aJ12, aJ22;
    if (!theFunc.ValueAndJacobian(aRes.U, aRes.V, aF1, aF2, aJ11, aJ12, aJ22))
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

    // Solve 2x2 symmetric linear system: J * [dU, dV]^T = -[F1, F2]^T
    // J = [[J11, J12], [J12, J22]] (symmetric)
    const double aDet = aJ11 * aJ22 - aJ12 * aJ12;

    double aDU, aDV;
    if (std::abs(aDet) < 1.0e-30)
    {
      // Singular Jacobian - try gradient descent step
      const double aNormSq = aJ11 * aJ11 + 2.0 * aJ12 * aJ12 + aJ22 * aJ22;
      if (aNormSq < 1.0e-60)
      {
        aRes.FNorm  = std::sqrt(aFNormSq);
        aRes.Status = Status::Singular;
        return aRes;
      }
      const double aStep = std::sqrt(aFNormSq / aNormSq) * 0.1;
      aDU                = -aStep * aF1;
      aDV                = -aStep * aF2;
    }
    else
    {
      // Cramer's rule for symmetric matrix
      const double aInvDet = 1.0 / aDet;
      aDU                  = (-aF1 * aJ22 + aF2 * aJ12) * aInvDet;
      aDV                  = (-aF2 * aJ11 + aF1 * aJ12) * aInvDet;

      // Limit step size
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

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

#ifndef _MathSys_Newton4D_HeaderFile
#define _MathSys_Newton4D_HeaderFile

#include <MathSys_NewtonTypes.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <array>
#include <cmath>

//! @file MathSys_Newton4D.hxx
//! @brief Optimized 4D Newton-Raphson solver for systems of 4 equations in 4 unknowns.
//!
//! This solver is specifically optimized for 4D problems like:
//! - Surface-surface extrema (find u1,v1,u2,v2 where distance is extremal)
//! - Curve-surface intersection with tangent constraints
//! - Two-curve closest point with additional constraints
//!
//! Optimizations compared to general Newton:
//! - No math_Vector/math_Matrix allocation overhead
//! - Gaussian elimination with partial pivoting for 4x4 system
//! - Squared norm comparisons (avoid sqrt in convergence check)
//! - Step limiting to prevent wild oscillations
//! - Gradient descent fallback for singular Jacobian

namespace MathSys
{
namespace detail
{

//! Check that NewtonBoundsN<4> has valid (min <= max) ranges.
inline bool IsBoundsValid4D(const NewtonBoundsN<4>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return true;
  }

  return theBounds.Min[0] <= theBounds.Max[0] && theBounds.Min[1] <= theBounds.Max[1]
         && theBounds.Min[2] <= theBounds.Max[2] && theBounds.Min[3] <= theBounds.Max[3];
}

//! Check that NewtonOptions fields are positive and valid.
inline bool IsOptionsValid4D(const NewtonOptions& theOptions)
{
  return theOptions.ResidualTol > 0.0 && theOptions.StepTolRel > 0.0 && theOptions.MaxIterations > 0
         && theOptions.MaxStepRatio > 0.0 && theOptions.SoftBoundsExtension >= 0.0;
}

//! Return the largest domain extent across all 4 dimensions (min 1.0).
inline double MaxDomainSize4D(const NewtonBoundsN<4>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return 1.0;
  }

  const double aD0 = theBounds.Max[0] - theBounds.Min[0];
  const double aD1 = theBounds.Max[1] - theBounds.Min[1];
  const double aD2 = theBounds.Max[2] - theBounds.Min[2];
  const double aD3 = theBounds.Max[3] - theBounds.Min[3];
  return std::max(1.0, std::max(aD0, std::max(aD1, std::max(aD2, aD3))));
}

//! Clamp solution array to bounds, optionally extending by soft-bounds ratio.
inline void Clamp4D(std::array<double, 4>&  theX,
                    const NewtonBoundsN<4>& theBounds,
                    bool                    theUseSoftBounds,
                    double                  theSoftExtRatio)
{
  if (!theBounds.HasBounds)
  {
    return;
  }

  for (int i = 0; i < 4; ++i)
  {
    const double aExt =
      theUseSoftBounds ? (theBounds.Max[i] - theBounds.Min[i]) * theSoftExtRatio : 0.0;
    theX[i] = std::clamp(theX[i], theBounds.Min[i] - aExt, theBounds.Max[i] + aExt);
  }
}

//! Solve 4x4 linear system using Gaussian elimination with partial pivoting.
//! @param[in] theJ 4x4 Jacobian matrix
//! @param[in] theF 4-element right-hand side
//! @param[out] theDelta 4-element solution vector
//! @return true if system was solved successfully
inline bool Solve4x4(const double theJ[4][4], const double theF[4], double theDelta[4])
{
  // Augmented matrix [J | -F]
  double A[4][5];
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      A[i][j] = theJ[i][j];
    }
    A[i][4] = -theF[i];
  }

  // Forward elimination with partial pivoting
  for (int k = 0; k < 4; ++k)
  {
    // Find pivot
    int    aMaxRow = k;
    double aMaxVal = std::abs(A[k][k]);
    for (int i = k + 1; i < 4; ++i)
    {
      const double aVal = std::abs(A[i][k]);
      if (aVal > aMaxVal)
      {
        aMaxVal = aVal;
        aMaxRow = i;
      }
    }

    // Check for singularity
    if (aMaxVal < 1.0e-30)
    {
      return false;
    }

    // Swap rows if needed
    if (aMaxRow != k)
    {
      for (int j = k; j <= 4; ++j)
      {
        std::swap(A[k][j], A[aMaxRow][j]);
      }
    }

    // Eliminate column
    const double aInvPivot = 1.0 / A[k][k];
    for (int i = k + 1; i < 4; ++i)
    {
      const double aFactor = A[i][k] * aInvPivot;
      for (int j = k + 1; j <= 4; ++j)
      {
        A[i][j] -= aFactor * A[k][j];
      }
      A[i][k] = 0.0;
    }
  }

  // Back substitution
  for (int i = 3; i >= 0; --i)
  {
    double aSum = A[i][4];
    for (int j = i + 1; j < 4; ++j)
    {
      aSum -= A[i][j] * theDelta[j];
    }
    theDelta[i] = aSum / A[i][i];
  }

  return true;
}

} // namespace detail

//! Solve a 4x4 nonlinear system by Newton iteration with bounds.
//!
//! Solves the system [F1, F2, F3, F4] = [0, 0, 0, 0] using Newton-Raphson iteration
//! with Gaussian elimination for the 4x4 linear system at each step.
//!
//! The function type must be callable with signature:
//! @code
//!   bool operator()(double theX1, double theX2, double theX3, double theX4,
//!                   double theF[4], double theJ[4][4]) const;
//! @endcode
//! where theF is the function values and theJ is the 4x4 Jacobian matrix.
//!
//! @tparam Function callable type (functor, lambda, or function pointer)
//! @param[in] theFunc function to solve (provides F and Jacobian)
//! @param[in] theX0 initial guess {x1, x2, x3, x4}
//! @param[in] theBounds box bounds for each variable
//! @param[in] theOptions solver options (tolerances, max iterations, etc.)
//! @return NewtonResultN<4> containing solution, status, and diagnostics
template <typename Function>
NewtonResultN<4> Solve4D(const Function&              theFunc,
                         const std::array<double, 4>& theX0,
                         const NewtonBoundsN<4>&      theBounds,
                         const NewtonOptions&         theOptions = NewtonOptions())
{
  NewtonResultN<4> aRes;
  aRes.X = theX0;

  if (!detail::IsOptionsValid4D(theOptions) || !detail::IsBoundsValid4D(theBounds))
  {
    aRes.Status = NewtonStatus::InvalidInput;
    return aRes;
  }

  detail::Clamp4D(aRes.X, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);

  const double aTolSq   = theOptions.ResidualTol * theOptions.ResidualTol;
  const double aMaxStep = theOptions.MaxStepRatio * detail::MaxDomainSize4D(theBounds);

  for (size_t anIter = 0; anIter < theOptions.MaxIterations; ++anIter)
  {
    aRes.NbIter = anIter + 1;

    double aF[4];
    double aJ[4][4];
    if (!theFunc(aRes.X[0], aRes.X[1], aRes.X[2], aRes.X[3], aF, aJ))
    {
      aRes.Status = NewtonStatus::NumericalError;
      return aRes;
    }

    // Check convergence using squared norm (avoid sqrt)
    const double aFNormSq = aF[0] * aF[0] + aF[1] * aF[1] + aF[2] * aF[2] + aF[3] * aF[3];
    aRes.ResidualNorm     = std::sqrt(aFNormSq);
    if (aFNormSq <= aTolSq)
    {
      aRes.Status = NewtonStatus::Converged;
      return aRes;
    }

    // Solve 4x4 linear system: J * delta = -F
    double aDelta[4];
    if (!detail::Solve4x4(aJ, aF, aDelta))
    {
      // Singular Jacobian - try steepest descent direction: -J^T * F
      double aGrad[4] = {0.0, 0.0, 0.0, 0.0};
      for (int c = 0; c < 4; ++c)
      {
        for (int r = 0; r < 4; ++r)
        {
          aGrad[c] += aJ[r][c] * aF[r];
        }
      }

      const double aGradSq =
        aGrad[0] * aGrad[0] + aGrad[1] * aGrad[1] + aGrad[2] * aGrad[2] + aGrad[3] * aGrad[3];
      if (aGradSq < 1.0e-60)
      {
        aRes.Status = NewtonStatus::SingularJacobian;
        return aRes;
      }

      const double aAlpha = std::min(1.0, std::sqrt(aFNormSq / aGradSq) * 0.1);
      for (int i = 0; i < 4; ++i)
      {
        aDelta[i] = -aAlpha * aGrad[i];
      }
    }

    // Limit step size to prevent wild oscillations
    const double aStepNormSq =
      aDelta[0] * aDelta[0] + aDelta[1] * aDelta[1] + aDelta[2] * aDelta[2] + aDelta[3] * aDelta[3];
    const double aStepNorm = std::sqrt(aStepNormSq);
    if (aStepNorm > aMaxStep)
    {
      const double aScale = aMaxStep / aStepNorm;
      for (int i = 0; i < 4; ++i)
      {
        aDelta[i] *= aScale;
      }
    }

    // Update and clamp to bounds
    std::array<double, 4> aNewX = {aRes.X[0] + aDelta[0],
                                   aRes.X[1] + aDelta[1],
                                   aRes.X[2] + aDelta[2],
                                   aRes.X[3] + aDelta[3]};
    detail::Clamp4D(aNewX, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);

    aRes.StepNorm = std::sqrt((aNewX[0] - aRes.X[0]) * (aNewX[0] - aRes.X[0])
                              + (aNewX[1] - aRes.X[1]) * (aNewX[1] - aRes.X[1])
                              + (aNewX[2] - aRes.X[2]) * (aNewX[2] - aRes.X[2])
                              + (aNewX[3] - aRes.X[3]) * (aNewX[3] - aRes.X[3]));
    aRes.X        = aNewX;

    const double aScaleRef = std::max(
      1.0,
      std::max(std::abs(aRes.X[0]),
               std::max(std::abs(aRes.X[1]), std::max(std::abs(aRes.X[2]), std::abs(aRes.X[3])))));
    if (aRes.StepNorm <= theOptions.StepTolRel * aScaleRef)
    {
      aRes.Status = NewtonStatus::MaxIterations;
      return aRes;
    }
  }

  // Final convergence check after max iterations
  double aF[4];
  double aJ[4][4];
  if (!theFunc(aRes.X[0], aRes.X[1], aRes.X[2], aRes.X[3], aF, aJ))
  {
    aRes.Status = NewtonStatus::NumericalError;
    return aRes;
  }

  aRes.ResidualNorm = std::sqrt(aF[0] * aF[0] + aF[1] * aF[1] + aF[2] * aF[2] + aF[3] * aF[3]);
  aRes.Status       = (aRes.ResidualNorm <= theOptions.ResidualTol) ? NewtonStatus::Converged
                                                                    : NewtonStatus::MaxIterations;
  return aRes;
}

//! Optimized 4D Newton solver for surface-surface extrema.
//!
//! Specialized version for finding extrema between two surfaces S1(u1,v1) and S2(u2,v2).
//! The function values are the gradient components of the squared distance:
//! - F1 = (S1-S2) . dS1/dU1
//! - F2 = (S1-S2) . dS1/dV1
//! - F3 = (S2-S1) . dS2/dU2
//! - F4 = (S2-S1) . dS2/dV2
//!
//! The Jacobian has a special block structure with 2x2 blocks.
//!
//! @tparam SurfaceEvaluator1 type providing D2 evaluation for first surface
//! @tparam SurfaceEvaluator2 type providing D2 evaluation for second surface
//! @param[in] theSurf1 first surface evaluator
//! @param[in] theSurf2 second surface evaluator
//! @param[in] theX0 initial guess {u1, v1, u2, v2}
//! @param[in] theBounds box bounds for {u1, v1, u2, v2}
//! @param[in] theOptions solver options
//! @return NewtonResultN<4> with u1, v1, u2, v2 stored in X[0..3]
template <typename SurfaceEvaluator1, typename SurfaceEvaluator2>
NewtonResultN<4> SolveSurfaceSurfaceExtrema4D(const SurfaceEvaluator1&     theSurf1,
                                              const SurfaceEvaluator2&     theSurf2,
                                              const std::array<double, 4>& theX0,
                                              const NewtonBoundsN<4>&      theBounds,
                                              const NewtonOptions& theOptions = NewtonOptions())
{
  auto aFunc = [&theSurf1, &theSurf2](double theU1,
                                      double theV1,
                                      double theU2,
                                      double theV2,
                                      double theF[4],
                                      double theJ[4][4]) -> bool {
    gp_Pnt aPt1, aPt2;
    gp_Vec aD1U1, aD1V1, aD2UU1, aD2VV1, aD2UV1;
    gp_Vec aD1U2, aD1V2, aD2UU2, aD2VV2, aD2UV2;

    theSurf1.D2(theU1, theV1, aPt1, aD1U1, aD1V1, aD2UU1, aD2VV1, aD2UV1);
    theSurf2.D2(theU2, theV2, aPt2, aD1U2, aD1V2, aD2UU2, aD2VV2, aD2UV2);

    // D = P1 - P2
    const gp_Vec aD(aPt2, aPt1);

    // Function values: gradient of ||S1 - S2||^2
    theF[0] = aD.Dot(aD1U1);
    theF[1] = aD.Dot(aD1V1);
    theF[2] = -aD.Dot(aD1U2);
    theF[3] = -aD.Dot(aD1V2);

    // Jacobian: Hessian of ||S1 - S2||^2
    // Block [0:2, 0:2]: derivatives w.r.t. (u1, v1)
    theJ[0][0] = aD1U1.Dot(aD1U1) + aD.Dot(aD2UU1);
    theJ[0][1] = aD1V1.Dot(aD1U1) + aD.Dot(aD2UV1);
    theJ[1][0] = theJ[0][1]; // Symmetric
    theJ[1][1] = aD1V1.Dot(aD1V1) + aD.Dot(aD2VV1);

    // Block [0:2, 2:4]: cross derivatives
    theJ[0][2] = -aD1U2.Dot(aD1U1);
    theJ[0][3] = -aD1V2.Dot(aD1U1);
    theJ[1][2] = -aD1U2.Dot(aD1V1);
    theJ[1][3] = -aD1V2.Dot(aD1V1);

    // Block [2:4, 0:2]: cross derivatives (symmetric to above)
    theJ[2][0] = theJ[0][2];
    theJ[2][1] = theJ[1][2];
    theJ[3][0] = theJ[0][3];
    theJ[3][1] = theJ[1][3];

    // Block [2:4, 2:4]: derivatives w.r.t. (u2, v2)
    theJ[2][2] = aD1U2.Dot(aD1U2) - aD.Dot(aD2UU2);
    theJ[2][3] = aD1V2.Dot(aD1U2) - aD.Dot(aD2UV2);
    theJ[3][2] = theJ[2][3]; // Symmetric
    theJ[3][3] = aD1V2.Dot(aD1V2) - aD.Dot(aD2VV2);
    return true;
  };

  return Solve4D(aFunc, theX0, theBounds, theOptions);
}

} // namespace MathSys

#endif // _MathSys_Newton4D_HeaderFile

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

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>

#include <algorithm>
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
using namespace MathUtils;

//! Result of 4D Newton iteration.
struct Newton4DResult
{
  Status Status = Status::NotConverged; //!< Computation status
  double X1     = 0.0;                  //!< Solution X1 coordinate
  double X2     = 0.0;                  //!< Solution X2 coordinate
  double X3     = 0.0;                  //!< Solution X3 coordinate
  double X4     = 0.0;                  //!< Solution X4 coordinate
  size_t NbIter = 0;                    //!< Number of iterations performed
  double FNorm  = 0.0;                  //!< Final |F| norm

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

namespace detail
{

//! Solve 4x4 linear system using Gaussian elimination with partial pivoting.
//! @param theJ input 4x4 Jacobian matrix (modified during computation)
//! @param theF input 4-element right-hand side (modified: negated)
//! @param theDelta output 4-element solution vector
//! @return true if system was solved successfully
inline bool Solve4x4(double theJ[4][4], double theF[4], double theDelta[4])
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

//! Optimized 4D Newton-Raphson solver with bounds.
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
//! @param theFunc function to solve (provides F and Jacobian)
//! @param theX1_0 initial X1 guess
//! @param theX2_0 initial X2 guess
//! @param theX3_0 initial X3 guess
//! @param theX4_0 initial X4 guess
//! @param theX1Min X1 lower bound
//! @param theX1Max X1 upper bound
//! @param theX2Min X2 lower bound
//! @param theX2Max X2 upper bound
//! @param theX3Min X3 lower bound
//! @param theX3Max X3 upper bound
//! @param theX4Min X4 lower bound
//! @param theX4Max X4 upper bound
//! @param theTol tolerance for convergence (|F| < tol)
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton4DResult containing solution if converged
template <typename Function>
Newton4DResult Newton4D(const Function& theFunc,
                        double          theX1_0,
                        double          theX2_0,
                        double          theX3_0,
                        double          theX4_0,
                        double          theX1Min,
                        double          theX1Max,
                        double          theX2Min,
                        double          theX2Max,
                        double          theX3Min,
                        double          theX3Max,
                        double          theX4Min,
                        double          theX4Max,
                        double          theTol,
                        size_t          theMaxIter = 20)
{
  Newton4DResult aRes;
  aRes.X1 = theX1_0;
  aRes.X2 = theX2_0;
  aRes.X3 = theX3_0;
  aRes.X4 = theX4_0;

  // Pre-compute max step limit (50% of smallest domain range)
  const double aMaxStep =
    0.5
    * std::min(
      {theX1Max - theX1Min, theX2Max - theX2Min, theX3Max - theX3Min, theX4Max - theX4Min});
  const double aTolSq = theTol * theTol;

  for (size_t i = 0; i < theMaxIter; ++i)
  {
    aRes.NbIter = i + 1;

    double aF[4];
    double aJ[4][4];
    if (!theFunc(aRes.X1, aRes.X2, aRes.X3, aRes.X4, aF, aJ))
    {
      aRes.Status = Status::NumericalError;
      return aRes;
    }

    // Check convergence using squared norm (avoid sqrt)
    const double aFNormSq = aF[0] * aF[0] + aF[1] * aF[1] + aF[2] * aF[2] + aF[3] * aF[3];

    if (aFNormSq < aTolSq)
    {
      aRes.FNorm  = std::sqrt(aFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }

    // Solve 4x4 linear system: J * delta = -F
    double aDelta[4];
    if (!detail::Solve4x4(aJ, aF, aDelta))
    {
      // Singular Jacobian - try gradient descent step
      double aJNormSq = 0.0;
      for (int r = 0; r < 4; ++r)
      {
        for (int c = 0; c < 4; ++c)
        {
          aJNormSq += aJ[r][c] * aJ[r][c];
        }
      }

      if (aJNormSq < 1.0e-60)
      {
        aRes.FNorm  = std::sqrt(aFNormSq);
        aRes.Status = Status::Singular;
        return aRes;
      }

      // Use steepest descent direction: -J^T * F
      const double aStep = std::sqrt(aFNormSq / aJNormSq) * 0.1;
      for (int k = 0; k < 4; ++k)
      {
        aDelta[k] = 0.0;
        for (int r = 0; r < 4; ++r)
        {
          aDelta[k] -= aStep * aJ[r][k] * aF[r];
        }
      }
    }
    else
    {
      // Limit step size to prevent wild oscillations
      const double aStepNormSq = aDelta[0] * aDelta[0] + aDelta[1] * aDelta[1]
                                 + aDelta[2] * aDelta[2] + aDelta[3] * aDelta[3];
      if (aStepNormSq > aMaxStep * aMaxStep)
      {
        const double aScale = aMaxStep / std::sqrt(aStepNormSq);
        aDelta[0] *= aScale;
        aDelta[1] *= aScale;
        aDelta[2] *= aScale;
        aDelta[3] *= aScale;
      }
    }

    // Update and clamp to bounds
    aRes.X1 = std::clamp(aRes.X1 + aDelta[0], theX1Min, theX1Max);
    aRes.X2 = std::clamp(aRes.X2 + aDelta[1], theX2Min, theX2Max);
    aRes.X3 = std::clamp(aRes.X3 + aDelta[2], theX3Min, theX3Max);
    aRes.X4 = std::clamp(aRes.X4 + aDelta[3], theX4Min, theX4Max);
  }

  // Final convergence check (only at max iterations)
  double aF[4];
  double aJ[4][4];
  if (theFunc(aRes.X1, aRes.X2, aRes.X3, aRes.X4, aF, aJ))
  {
    aRes.FNorm = std::sqrt(aF[0] * aF[0] + aF[1] * aF[1] + aF[2] * aF[2] + aF[3] * aF[3]);
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

//! Unbounded 4D Newton-Raphson solver.
//!
//! @tparam Function type with ValueAndJacobian method
//! @param theFunc function to solve
//! @param theX1_0 initial X1 guess
//! @param theX2_0 initial X2 guess
//! @param theX3_0 initial X3 guess
//! @param theX4_0 initial X4 guess
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton4DResult containing solution if converged
template <typename Function>
Newton4DResult Newton4D(const Function& theFunc,
                        double          theX1_0,
                        double          theX2_0,
                        double          theX3_0,
                        double          theX4_0,
                        double          theTol,
                        size_t          theMaxIter = 20)
{
  constexpr double aInf = 1.0e100;
  return Newton4D(theFunc,
                  theX1_0,
                  theX2_0,
                  theX3_0,
                  theX4_0,
                  -aInf,
                  aInf,
                  -aInf,
                  aInf,
                  -aInf,
                  aInf,
                  -aInf,
                  aInf,
                  theTol,
                  theMaxIter);
}

//! Optimized 4D Newton solver for Surface-Surface extrema.
//!
//! Specialized version for the common case of finding extrema between two surfaces.
//! The function values are the gradient components of the squared distance:
//! - F1 = (S1-S2) · dS1/dU1
//! - F2 = (S1-S2) · dS1/dV1
//! - F3 = (S2-S1) · dS2/dU2
//! - F4 = (S2-S1) · dS2/dV2
//!
//! The Jacobian has a special structure with 2x2 blocks.
//!
//! @tparam SurfaceEvaluator type providing D2 evaluation for surfaces
//! @param theSurf1 first surface evaluator
//! @param theSurf2 second surface evaluator
//! @param theU1_0 initial U1 guess on surface 1
//! @param theV1_0 initial V1 guess on surface 1
//! @param theU2_0 initial U2 guess on surface 2
//! @param theV2_0 initial V2 guess on surface 2
//! @param theU1Min U1 lower bound
//! @param theU1Max U1 upper bound
//! @param theV1Min V1 lower bound
//! @param theV1Max V1 upper bound
//! @param theU2Min U2 lower bound
//! @param theU2Max U2 upper bound
//! @param theV2Min V2 lower bound
//! @param theV2Max V2 upper bound
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton4DResult with U1,V1,U2,V2 stored in X1,X2,X3,X4
template <typename SurfaceEvaluator1, typename SurfaceEvaluator2>
Newton4DResult Newton4DSurfaceSurface(const SurfaceEvaluator1& theSurf1,
                                      const SurfaceEvaluator2& theSurf2,
                                      double                   theU1_0,
                                      double                   theV1_0,
                                      double                   theU2_0,
                                      double                   theV2_0,
                                      double                   theU1Min,
                                      double                   theU1Max,
                                      double                   theV1Min,
                                      double                   theV1Max,
                                      double                   theU2Min,
                                      double                   theU2Max,
                                      double                   theV2Min,
                                      double                   theV2Max,
                                      double                   theTol,
                                      size_t                   theMaxIter = 20)
{
  // Lambda that computes F and J for surface-surface extrema
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

  return Newton4D(aFunc,
                  theU1_0,
                  theV1_0,
                  theU2_0,
                  theV2_0,
                  theU1Min,
                  theU1Max,
                  theV1Min,
                  theV1Max,
                  theU2Min,
                  theU2Max,
                  theV2Min,
                  theV2Max,
                  theTol,
                  theMaxIter);
}

} // namespace MathSys

#endif // _MathSys_Newton4D_HeaderFile

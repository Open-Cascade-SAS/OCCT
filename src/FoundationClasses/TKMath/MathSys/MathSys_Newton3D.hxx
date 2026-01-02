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

#ifndef _MathSys_Newton3D_HeaderFile
#define _MathSys_Newton3D_HeaderFile

#include <MathUtils_Config.hxx>
#include <MathUtils_Types.hxx>

#include <algorithm>
#include <cmath>

//! @file MathSys_Newton3D.hxx
//! @brief Optimized 3D Newton-Raphson solver for systems of 3 equations in 3 unknowns.
//!
//! This solver is specifically optimized for 3D problems like:
//! - Curve-surface extrema (find t, u, v where distance is extremal)
//! - Curve intersection with surface plus a constraint
//! - Point-surface with additional constraint
//!
//! Optimizations compared to general Newton:
//! - No math_Vector/math_Matrix allocation overhead
//! - Cramer's rule with precomputed cofactors for 3x3 system
//! - Squared norm comparisons (avoid sqrt in convergence check)
//! - Step limiting to prevent wild oscillations
//! - Gradient descent fallback for singular Jacobian

namespace MathSys
{
using namespace MathUtils;

//! Result of 3D Newton iteration.
struct Newton3DResult
{
  Status Status = Status::NotConverged; //!< Computation status
  double X1     = 0.0;                  //!< Solution X1 coordinate
  double X2     = 0.0;                  //!< Solution X2 coordinate
  double X3     = 0.0;                  //!< Solution X3 coordinate
  size_t NbIter = 0;                    //!< Number of iterations performed
  double FNorm  = 0.0;                  //!< Final |F| norm

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

namespace detail
{

//! Solve 3x3 linear system using Cramer's rule with cofactor expansion.
//! @param theJ input 3x3 Jacobian matrix
//! @param theF input 3-element right-hand side (solves J*x = -F)
//! @param theDelta output 3-element solution vector
//! @return true if system was solved successfully (non-singular)
inline bool Solve3x3(const double theJ[3][3], const double theF[3], double theDelta[3])
{
  // Compute cofactors for first row (used for determinant and inverse)
  const double aCof00 = theJ[1][1] * theJ[2][2] - theJ[1][2] * theJ[2][1];
  const double aCof01 = theJ[1][2] * theJ[2][0] - theJ[1][0] * theJ[2][2];
  const double aCof02 = theJ[1][0] * theJ[2][1] - theJ[1][1] * theJ[2][0];

  // Determinant by first row expansion
  const double aDet = theJ[0][0] * aCof00 + theJ[0][1] * aCof01 + theJ[0][2] * aCof02;

  // Check for singularity
  if (std::abs(aDet) < 1.0e-30)
  {
    return false;
  }

  const double aInvDet = 1.0 / aDet;

  // Remaining cofactors
  const double aCof10 = theJ[0][2] * theJ[2][1] - theJ[0][1] * theJ[2][2];
  const double aCof11 = theJ[0][0] * theJ[2][2] - theJ[0][2] * theJ[2][0];
  const double aCof12 = theJ[0][1] * theJ[2][0] - theJ[0][0] * theJ[2][1];

  const double aCof20 = theJ[0][1] * theJ[1][2] - theJ[0][2] * theJ[1][1];
  const double aCof21 = theJ[0][2] * theJ[1][0] - theJ[0][0] * theJ[1][2];
  const double aCof22 = theJ[0][0] * theJ[1][1] - theJ[0][1] * theJ[1][0];

  // Solve: delta = -J^(-1) * F = -adjugate(J)^T * F / det
  theDelta[0] = -(aCof00 * theF[0] + aCof10 * theF[1] + aCof20 * theF[2]) * aInvDet;
  theDelta[1] = -(aCof01 * theF[0] + aCof11 * theF[1] + aCof21 * theF[2]) * aInvDet;
  theDelta[2] = -(aCof02 * theF[0] + aCof12 * theF[1] + aCof22 * theF[2]) * aInvDet;

  return true;
}

} // namespace detail

//! Optimized 3D Newton-Raphson solver with bounds.
//!
//! Solves the system [F1, F2, F3] = [0, 0, 0] using Newton-Raphson iteration
//! with Cramer's rule for the 3x3 linear system at each step.
//!
//! The function type must be callable with signature:
//! @code
//!   bool operator()(double theX1, double theX2, double theX3,
//!                   double theF[3], double theJ[3][3]) const;
//! @endcode
//! where theF is the function values and theJ is the 3x3 Jacobian matrix.
//!
//! @tparam Function callable type (functor, lambda, or function pointer)
//! @param theFunc function to solve (provides F and Jacobian)
//! @param theX1_0 initial X1 guess
//! @param theX2_0 initial X2 guess
//! @param theX3_0 initial X3 guess
//! @param theX1Min X1 lower bound
//! @param theX1Max X1 upper bound
//! @param theX2Min X2 lower bound
//! @param theX2Max X2 upper bound
//! @param theX3Min X3 lower bound
//! @param theX3Max X3 upper bound
//! @param theTol tolerance for convergence (|F| < tol)
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton3DResult containing solution if converged
template <typename Function>
Newton3DResult Newton3D(const Function& theFunc,
                        double          theX1_0,
                        double          theX2_0,
                        double          theX3_0,
                        double          theX1Min,
                        double          theX1Max,
                        double          theX2Min,
                        double          theX2Max,
                        double          theX3Min,
                        double          theX3Max,
                        double          theTol,
                        size_t          theMaxIter = 20)
{
  Newton3DResult aRes;
  aRes.X1 = theX1_0;
  aRes.X2 = theX2_0;
  aRes.X3 = theX3_0;

  // Pre-compute max step limit (50% of smallest domain range)
  const double aMaxStep =
    0.5 * std::min({theX1Max - theX1Min, theX2Max - theX2Min, theX3Max - theX3Min});
  const double aTolSq = theTol * theTol;

  for (size_t i = 0; i < theMaxIter; ++i)
  {
    aRes.NbIter = i + 1;

    double aF[3];
    double aJ[3][3];
    if (!theFunc(aRes.X1, aRes.X2, aRes.X3, aF, aJ))
    {
      aRes.Status = Status::NumericalError;
      return aRes;
    }

    // Check convergence using squared norm (avoid sqrt)
    const double aFNormSq = aF[0] * aF[0] + aF[1] * aF[1] + aF[2] * aF[2];

    if (aFNormSq < aTolSq)
    {
      aRes.FNorm  = std::sqrt(aFNormSq);
      aRes.Status = Status::OK;
      return aRes;
    }

    // Solve 3x3 linear system: J * delta = -F
    double aDelta[3];
    if (!detail::Solve3x3(aJ, aF, aDelta))
    {
      // Singular Jacobian - try gradient descent step
      double aJNormSq = 0.0;
      for (int r = 0; r < 3; ++r)
      {
        for (int c = 0; c < 3; ++c)
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
      for (int k = 0; k < 3; ++k)
      {
        aDelta[k] = 0.0;
        for (int r = 0; r < 3; ++r)
        {
          aDelta[k] -= aStep * aJ[r][k] * aF[r];
        }
      }
    }
    else
    {
      // Limit step size to prevent wild oscillations
      const double aStepNormSq =
        aDelta[0] * aDelta[0] + aDelta[1] * aDelta[1] + aDelta[2] * aDelta[2];
      if (aStepNormSq > aMaxStep * aMaxStep)
      {
        const double aScale = aMaxStep / std::sqrt(aStepNormSq);
        aDelta[0] *= aScale;
        aDelta[1] *= aScale;
        aDelta[2] *= aScale;
      }
    }

    // Update and clamp to bounds
    aRes.X1 = std::clamp(aRes.X1 + aDelta[0], theX1Min, theX1Max);
    aRes.X2 = std::clamp(aRes.X2 + aDelta[1], theX2Min, theX2Max);
    aRes.X3 = std::clamp(aRes.X3 + aDelta[2], theX3Min, theX3Max);
  }

  // Final convergence check (only at max iterations)
  double aF[3];
  double aJ[3][3];
  if (theFunc(aRes.X1, aRes.X2, aRes.X3, aF, aJ))
  {
    aRes.FNorm = std::sqrt(aF[0] * aF[0] + aF[1] * aF[1] + aF[2] * aF[2]);
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

//! Unbounded 3D Newton-Raphson solver.
//!
//! @tparam Function callable type
//! @param theFunc function to solve
//! @param theX1_0 initial X1 guess
//! @param theX2_0 initial X2 guess
//! @param theX3_0 initial X3 guess
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton3DResult containing solution if converged
template <typename Function>
Newton3DResult Newton3D(const Function& theFunc,
                        double          theX1_0,
                        double          theX2_0,
                        double          theX3_0,
                        double          theTol,
                        size_t          theMaxIter = 20)
{
  constexpr double aInf = 1.0e100;
  return Newton3D(theFunc,
                  theX1_0,
                  theX2_0,
                  theX3_0,
                  -aInf,
                  aInf,
                  -aInf,
                  aInf,
                  -aInf,
                  aInf,
                  theTol,
                  theMaxIter);
}

//! Optimized 3D Newton solver for Curve-Surface extrema.
//!
//! Specialized version for finding extrema between a curve C(t) and surface S(u,v).
//! The function values are the gradient components of the squared distance:
//! - F1 = (C-S) . dC/dt
//! - F2 = (S-C) . dS/du
//! - F3 = (S-C) . dS/dv
//!
//! @tparam CurveEvaluator type providing D2 evaluation for curve
//! @tparam SurfaceEvaluator type providing D2 evaluation for surface
//! @param theCurve curve evaluator
//! @param theSurface surface evaluator
//! @param theT0 initial t guess on curve
//! @param theU0 initial u guess on surface
//! @param theV0 initial v guess on surface
//! @param theTMin t lower bound
//! @param theTMax t upper bound
//! @param theUMin u lower bound
//! @param theUMax u upper bound
//! @param theVMin v lower bound
//! @param theVMax v upper bound
//! @param theTol tolerance for convergence
//! @param theMaxIter maximum iterations (default 20)
//! @return Newton3DResult with t,u,v stored in X1,X2,X3
template <typename CurveEvaluator, typename SurfaceEvaluator>
Newton3DResult Newton3DCurveSurface(const CurveEvaluator&   theCurve,
                                    const SurfaceEvaluator& theSurface,
                                    double                  theT0,
                                    double                  theU0,
                                    double                  theV0,
                                    double                  theTMin,
                                    double                  theTMax,
                                    double                  theUMin,
                                    double                  theUMax,
                                    double                  theVMin,
                                    double                  theVMax,
                                    double                  theTol,
                                    size_t                  theMaxIter = 20)
{
  // Lambda that computes F and J for curve-surface extrema
  auto aFunc = [&theCurve, &theSurface](double theT,
                                        double theU,
                                        double theV,
                                        double theF[3],
                                        double theJ[3][3]) -> bool {
    gp_Pnt aPtC, aPtS;
    gp_Vec aD1C, aD2C;
    gp_Vec aD1U, aD1V, aD2UU, aD2VV, aD2UV;

    theCurve.D2(theT, aPtC, aD1C, aD2C);
    theSurface.D2(theU, theV, aPtS, aD1U, aD1V, aD2UU, aD2VV, aD2UV);

    // D = C - S (vector from surface point to curve point)
    const gp_Vec aD(aPtS, aPtC);

    // Function values: gradient of ||C - S||^2
    // F1 = 2*D.dC/dt  -> we drop factor 2 since it doesn't affect zeros
    // F2 = -2*D.dS/du
    // F3 = -2*D.dS/dv
    theF[0] = aD.Dot(aD1C);
    theF[1] = -aD.Dot(aD1U);
    theF[2] = -aD.Dot(aD1V);

    // Jacobian: Hessian of ||C - S||^2 (without factor 2)
    // J[0][0] = dF1/dt = dC/dt . dC/dt + D . d2C/dt2
    theJ[0][0] = aD1C.Dot(aD1C) + aD.Dot(aD2C);

    // J[0][1] = dF1/du = -dS/du . dC/dt
    theJ[0][1] = -aD1U.Dot(aD1C);

    // J[0][2] = dF1/dv = -dS/dv . dC/dt
    theJ[0][2] = -aD1V.Dot(aD1C);

    // J[1][0] = dF2/dt = -dC/dt . dS/du (symmetric to J[0][1])
    theJ[1][0] = theJ[0][1];

    // J[1][1] = dF2/du = dS/du . dS/du - D . d2S/du2
    theJ[1][1] = aD1U.Dot(aD1U) - aD.Dot(aD2UU);

    // J[1][2] = dF2/dv = dS/dv . dS/du - D . d2S/dudv
    theJ[1][2] = aD1V.Dot(aD1U) - aD.Dot(aD2UV);

    // J[2][0] = dF3/dt = -dC/dt . dS/dv (symmetric to J[0][2])
    theJ[2][0] = theJ[0][2];

    // J[2][1] = dF3/du = dS/du . dS/dv - D . d2S/dudv (symmetric to J[1][2])
    theJ[2][1] = theJ[1][2];

    // J[2][2] = dF3/dv = dS/dv . dS/dv - D . d2S/dv2
    theJ[2][2] = aD1V.Dot(aD1V) - aD.Dot(aD2VV);

    return true;
  };

  return Newton3D(aFunc,
                  theT0,
                  theU0,
                  theV0,
                  theTMin,
                  theTMax,
                  theUMin,
                  theUMax,
                  theVMin,
                  theVMax,
                  theTol,
                  theMaxIter);
}

} // namespace MathSys

#endif // _MathSys_Newton3D_HeaderFile

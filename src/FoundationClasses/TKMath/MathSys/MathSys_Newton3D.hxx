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

#include <MathSys_NewtonTypes.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

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
//! - Overflow-safe norm comparisons
//! - Step limiting to prevent wild oscillations
//! - Gradient descent fallback for singular Jacobian

namespace MathSys
{
namespace Utils
{

//! Check that NewtonBoundsN<3> has valid (min <= max) ranges.
inline bool IsBoundsValid3D(const NewtonBoundsN<3>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return true;
  }

  return IsFiniteArray(theBounds.Min) && IsFiniteArray(theBounds.Max)
         && theBounds.Min[0] <= theBounds.Max[0] && theBounds.Min[1] <= theBounds.Max[1]
         && theBounds.Min[2] <= theBounds.Max[2];
}

//! Check that NewtonOptions fields are positive and valid.
inline bool IsOptionsValid3D(const NewtonOptions& theOptions)
{
  return std::isfinite(theOptions.FTolerance) && theOptions.FTolerance > 0.0
         && std::isfinite(theOptions.XTolerance) && theOptions.XTolerance > 0.0
         && theOptions.MaxIterations > 0 && std::isfinite(theOptions.MaxStepRatio)
         && theOptions.MaxStepRatio > 0.0 && std::isfinite(theOptions.SoftBoundsExtension)
         && theOptions.SoftBoundsExtension >= 0.0;
}

//! Return the largest domain extent across all 3 dimensions (min 1.0).
inline double MaxDomainSize3D(const NewtonBoundsN<3>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return 1.0;
  }

  const double aDX = theBounds.Max[0] - theBounds.Min[0];
  const double aDY = theBounds.Max[1] - theBounds.Min[1];
  const double aDZ = theBounds.Max[2] - theBounds.Min[2];
  return std::max(1.0, std::max(aDX, std::max(aDY, aDZ)));
}

//! Clamp solution array to bounds, optionally extending by soft-bounds ratio.
inline void Clamp3D(std::array<double, 3>&  theX,
                    const NewtonBoundsN<3>& theBounds,
                    bool                    theUseSoftBounds,
                    double                  theSoftExtRatio)
{
  if (!theBounds.HasBounds)
  {
    return;
  }

  const double aExtX =
    theUseSoftBounds ? (theBounds.Max[0] - theBounds.Min[0]) * theSoftExtRatio : 0.0;
  const double aExtY =
    theUseSoftBounds ? (theBounds.Max[1] - theBounds.Min[1]) * theSoftExtRatio : 0.0;
  const double aExtZ =
    theUseSoftBounds ? (theBounds.Max[2] - theBounds.Min[2]) * theSoftExtRatio : 0.0;

  theX[0] = std::clamp(theX[0], theBounds.Min[0] - aExtX, theBounds.Max[0] + aExtX);
  theX[1] = std::clamp(theX[1], theBounds.Min[1] - aExtY, theBounds.Max[1] + aExtY);
  theX[2] = std::clamp(theX[2], theBounds.Min[2] - aExtZ, theBounds.Max[2] + aExtZ);
}

//! Solve 3x3 linear system J*x = -F using scaled Gaussian elimination with pivoting.
//! @param[in] theJ 3x3 Jacobian matrix
//! @param[in] theF 3-element right-hand side
//! @param[out] theDelta 3-element solution vector
//! @return true if system was solved successfully (non-singular)
inline bool Solve3x3(const double theJ[3][3], const double theF[3], double theDelta[3])
{
  double aScale = 0.0;
  for (size_t aRow = 0; aRow < 3; ++aRow)
  {
    for (size_t aCol = 0; aCol < 3; ++aCol)
    {
      aScale = std::max(aScale, std::abs(theJ[aRow][aCol]));
    }
  }
  if (!(aScale > 0.0))
  {
    return false;
  }
  double aJ[3][3];
  double aF[3];
  for (size_t aRow = 0; aRow < 3; ++aRow)
  {
    aF[aRow] = theF[aRow] / aScale;
    for (size_t aCol = 0; aCol < 3; ++aCol)
    {
      aJ[aRow][aCol] = theJ[aRow][aCol] / aScale;
    }
  }

  for (size_t aPivot = 0; aPivot < 3; ++aPivot)
  {
    size_t aPivotRow = aPivot;
    for (size_t aRow = aPivot + 1; aRow < 3; ++aRow)
    {
      if (std::abs(aJ[aRow][aPivot]) > std::abs(aJ[aPivotRow][aPivot]))
      {
        aPivotRow = aRow;
      }
    }
    if (!(std::abs(aJ[aPivotRow][aPivot]) > THE_NEWTON_PIVOT_TOL))
    {
      return false;
    }
    if (aPivotRow != aPivot)
    {
      for (size_t aCol = aPivot; aCol < 3; ++aCol)
      {
        std::swap(aJ[aPivot][aCol], aJ[aPivotRow][aCol]);
      }
      std::swap(aF[aPivot], aF[aPivotRow]);
    }
    for (size_t aRow = aPivot + 1; aRow < 3; ++aRow)
    {
      const double aFactor = aJ[aRow][aPivot] / aJ[aPivot][aPivot];
      for (size_t aCol = aPivot + 1; aCol < 3; ++aCol)
      {
        aJ[aRow][aCol] -= aFactor * aJ[aPivot][aCol];
      }
      aF[aRow] -= aFactor * aF[aPivot];
    }
  }
  for (size_t aRow = 3; aRow-- > 0;)
  {
    double aValue = -aF[aRow];
    for (size_t aCol = aRow + 1; aCol < 3; ++aCol)
    {
      aValue -= aJ[aRow][aCol] * theDelta[aCol];
    }
    theDelta[aRow] = aValue / aJ[aRow][aRow];
  }
  return std::isfinite(theDelta[0]) && std::isfinite(theDelta[1]) && std::isfinite(theDelta[2]);
}

} // namespace Utils

//! Solve a 3x3 nonlinear system by Newton iteration with bounds.
//!
//! Solves the system [F1, F2, F3] = [0, 0, 0] using Newton-Raphson iteration
//! with scaled, pivoted elimination for the 3x3 linear system at each step.
//!
//! The function type must be callable with signature:
//! @code
//!   bool operator()(double theX1, double theX2, double theX3,
//!                   double theF[3], double theJ[3][3]) const;
//! @endcode
//! where theF is the function values and theJ is the 3x3 Jacobian matrix.
//!
//! @tparam Function callable type (functor, lambda, or function pointer)
//! @param[in] theFunc function to solve (provides F and Jacobian)
//! @param[in] theX0 initial guess {x1, x2, x3}
//! @param[in] theBounds box bounds for each variable
//! @param[in] theOptions solver options (tolerances, max iterations, etc.)
//! @return NewtonResultN<3> containing solution, status, and diagnostics
template <typename Function>
NewtonResultN<3> Solve3D(const Function&              theFunc,
                         const std::array<double, 3>& theX0,
                         const NewtonBoundsN<3>&      theBounds,
                         const NewtonOptions&         theOptions = NewtonOptions())
{
  NewtonResultN<3> aRes;
  aRes.X = theX0;

  if (!Utils::IsOptionsValid3D(theOptions) || !Utils::IsBoundsValid3D(theBounds)
      || !Utils::IsFiniteArray(theX0))
  {
    aRes.Status = MathUtils::Status::InvalidInput;
    return aRes;
  }

  Utils::Clamp3D(aRes.X, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);

  const double aMaxStep = theOptions.MaxStepRatio * Utils::MaxDomainSize3D(theBounds);

  for (uint32_t anIter = 0; anIter < theOptions.MaxIterations; ++anIter)
  {
    aRes.NbIterations = anIter + 1;

    double aF[3];
    double aJ[3][3];
    if (!theFunc(aRes.X[0], aRes.X[1], aRes.X[2], aF, aJ) || !Utils::IsFiniteSystemN(aF, aJ))
    {
      aRes.Status = MathUtils::Status::NumericalError;
      return aRes;
    }

    aRes.ResidualNorm = Utils::SafeNormN(aF);
    if (aRes.ResidualNorm <= theOptions.FTolerance)
    {
      aRes.Status = MathUtils::Status::OK;
      return aRes;
    }

    // Solve 3x3 linear system: J * delta = -F
    double aDelta[3];
    if (!Utils::Solve3x3(aJ, aF, aDelta))
    {
      // Singular Jacobian - try steepest descent direction: -J^T * F
      const double aGradX       = aJ[0][0] * aF[0] + aJ[1][0] * aF[1] + aJ[2][0] * aF[2];
      const double aGradY       = aJ[0][1] * aF[0] + aJ[1][1] * aF[1] + aJ[2][1] * aF[2];
      const double aGradZ       = aJ[0][2] * aF[0] + aJ[1][2] * aF[1] + aJ[2][2] * aF[2];
      const double aGradient[3] = {aGradX, aGradY, aGradZ};
      const double aGradNorm    = Utils::SafeNormN(aGradient);
      if (aGradNorm < 1.0e-30)
      {
        aRes.Status = MathUtils::Status::Singular;
        return aRes;
      }

      const double aAlpha = std::min(1.0, aRes.ResidualNorm / aGradNorm * 0.1);
      aDelta[0]           = -aAlpha * aGradX;
      aDelta[1]           = -aAlpha * aGradY;
      aDelta[2]           = -aAlpha * aGradZ;
    }

    // Limit step size to prevent wild oscillations
    const double aStepNorm = Utils::SafeNormN(aDelta);
    if (aStepNorm > aMaxStep)
    {
      const double aScale = aMaxStep / aStepNorm;
      aDelta[0] *= aScale;
      aDelta[1] *= aScale;
      aDelta[2] *= aScale;
    }

    std::array<double, 3> aNewX = aRes.X;
    if (theOptions.EnableLineSearch)
    {
      bool isAccepted = false;
      for (size_t aLineIter = 0; aLineIter < Utils::THE_LINE_SEARCH_MAX; ++aLineIter)
      {
        const double anAlpha = std::ldexp(1.0, -static_cast<int>(aLineIter));
        for (size_t anIndex = 0; anIndex < 3; ++anIndex)
        {
          aNewX[anIndex] = aRes.X[anIndex] + anAlpha * aDelta[anIndex];
        }
        Utils::Clamp3D(aNewX,
                       theBounds,
                       theOptions.AllowSoftBounds,
                       theOptions.SoftBoundsExtension);
        const std::array<double, 3> aProjectedStep = {aNewX[0] - aRes.X[0],
                                                      aNewX[1] - aRes.X[1],
                                                      aNewX[2] - aRes.X[2]};
        const long double aDerivative = Utils::MeritDirectionalDerivative(aF, aJ, aProjectedStep);
        double            aTrialF[3];
        double            aTrialJ[3][3];
        if (theFunc(aNewX[0], aNewX[1], aNewX[2], aTrialF, aTrialJ)
            && Utils::IsFiniteSystemN(aTrialF, aTrialJ)
            && Utils::IsArmijoAccepted(aRes.ResidualNorm, Utils::SafeNormN(aTrialF), aDerivative))
        {
          isAccepted = true;
          break;
        }
      }
      if (!isAccepted)
      {
        aRes.Status = MathUtils::Status::NonDescentDirection;
        return aRes;
      }
    }
    else
    {
      aNewX = {aRes.X[0] + aDelta[0], aRes.X[1] + aDelta[1], aRes.X[2] + aDelta[2]};
      Utils::Clamp3D(aNewX, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);
    }

    const bool isStepWithinTolerance =
      Utils::IsStepWithinTolerance(aNewX, aRes.X, theOptions.XTolerance);
    aRes.StepNorm = Utils::SafeDistanceN(aNewX, aRes.X);
    aRes.X        = aNewX;

    if (isStepWithinTolerance)
    {
      double aCheckF[3];
      double aCheckJ[3][3];
      if (!theFunc(aRes.X[0], aRes.X[1], aRes.X[2], aCheckF, aCheckJ)
          || !Utils::IsFiniteSystemN(aCheckF, aCheckJ))
      {
        aRes.Status = MathUtils::Status::NumericalError;
        return aRes;
      }

      aRes.ResidualNorm = Utils::SafeNormN(aCheckF);
      aRes.Status = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                                 : MathUtils::Status::MaxIterations;
      return aRes;
    }
  }

  // Final convergence check after max iterations
  double aF[3];
  double aJ[3][3];
  if (!theFunc(aRes.X[0], aRes.X[1], aRes.X[2], aF, aJ) || !Utils::IsFiniteSystemN(aF, aJ))
  {
    aRes.Status = MathUtils::Status::NumericalError;
    return aRes;
  }

  aRes.ResidualNorm = Utils::SafeNormN(aF);
  aRes.Status = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                             : MathUtils::Status::MaxIterations;
  return aRes;
}

//! Optimized 3D Newton solver for curve-surface extrema.
//!
//! Specialized version for finding extrema between a curve C(t) and surface S(u,v).
//! The function values are the gradient components of the squared distance:
//! - F1 = (C-S) . dC/dt
//! - F2 = (S-C) . dS/du
//! - F3 = (S-C) . dS/dv
//!
//! @tparam CurveEvaluator type providing D2(t, P, D1, D2) evaluation
//! @tparam SurfaceEvaluator type providing D2(u, v, P, D1U, D1V, D2UU, D2VV, D2UV) evaluation
//! @param[in] theCurve curve evaluator
//! @param[in] theSurface surface evaluator
//! @param[in] theX0 initial guess {t, u, v}
//! @param[in] theBounds box bounds for {t, u, v}
//! @param[in] theOptions solver options
//! @return NewtonResultN<3> with t, u, v stored in X[0], X[1], X[2]
template <typename CurveEvaluator, typename SurfaceEvaluator>
NewtonResultN<3> SolveCurveSurfaceExtrema3D(const CurveEvaluator&        theCurve,
                                            const SurfaceEvaluator&      theSurface,
                                            const std::array<double, 3>& theX0,
                                            const NewtonBoundsN<3>&      theBounds,
                                            const NewtonOptions& theOptions = NewtonOptions())
{
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

    // Function values: gradient of ||C - S||^2 (factor 2 dropped)
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

    // J[1][0] = dF2/dt (symmetric to J[0][1])
    theJ[1][0] = theJ[0][1];
    // J[1][1] = dF2/du = dS/du . dS/du - D . d2S/du2
    theJ[1][1] = aD1U.Dot(aD1U) - aD.Dot(aD2UU);
    // J[1][2] = dF2/dv = dS/dv . dS/du - D . d2S/dudv
    theJ[1][2] = aD1V.Dot(aD1U) - aD.Dot(aD2UV);

    // J[2][0] = dF3/dt (symmetric to J[0][2])
    theJ[2][0] = theJ[0][2];
    // J[2][1] = dF3/du (symmetric to J[1][2])
    theJ[2][1] = theJ[1][2];
    // J[2][2] = dF3/dv = dS/dv . dS/dv - D . d2S/dv2
    theJ[2][2] = aD1V.Dot(aD1V) - aD.Dot(aD2VV);
    return true;
  };

  return Solve3D(aFunc, theX0, theBounds, theOptions);
}

} // namespace MathSys

#endif // _MathSys_Newton3D_HeaderFile

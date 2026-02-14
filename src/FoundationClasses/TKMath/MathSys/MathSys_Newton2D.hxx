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

#include <MathSys_NewtonTypes.hxx>

#include <algorithm>
#include <array>
#include <cmath>

//! @file MathSys_Newton2D.hxx
//! @brief Optimized 2D Newton-Raphson solvers with strict convergence criteria.
//!
//! Specifically optimized for 2D problems like:
//! - Point-surface extrema (find u,v where gradient is zero)
//! - Curve intersection (find t1,t2 where curves meet)
//! - Surface intersection curves

namespace MathSys
{
namespace detail
{

constexpr double THE_SINGULAR_DET_TOL = 1.0e-25;
constexpr double THE_CRITICAL_GRAD_SQ = 1.0e-60;
constexpr int    THE_LINE_SEARCH_MAX  = 8;
constexpr double THE_ARMIJO_C1        = 1.0e-4;

//! Check that NewtonOptions fields are positive and valid.
inline bool IsOptionsValid(const NewtonOptions& theOptions)
{
  return theOptions.FTolerance > 0.0 && theOptions.XTolerance > 0.0 && theOptions.MaxIterations > 0
         && theOptions.MaxStepRatio > 0.0 && theOptions.SoftBoundsExtension >= 0.0;
}

//! Check that NewtonBoundsN<2> has valid (min <= max) ranges.
inline bool IsBoundsValid2D(const NewtonBoundsN<2>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return true;
  }
  return theBounds.Min[0] <= theBounds.Max[0] && theBounds.Min[1] <= theBounds.Max[1];
}

//! Return the largest domain extent across both dimensions (min 1.0).
inline double MaxDomainSize2D(const NewtonBoundsN<2>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return 1.0;
  }

  const double aDU = theBounds.Max[0] - theBounds.Min[0];
  const double aDV = theBounds.Max[1] - theBounds.Min[1];
  return std::max(1.0, std::max(aDU, aDV));
}

//! Clamp solution array to bounds, optionally extending by soft-bounds ratio.
inline void Clamp2D(std::array<double, 2>&  theX,
                    const NewtonBoundsN<2>& theBounds,
                    bool                    theUseSoftBounds,
                    double                  theSoftExtRatio)
{
  if (!theBounds.HasBounds)
  {
    return;
  }

  const double aExtU =
    theUseSoftBounds ? (theBounds.Max[0] - theBounds.Min[0]) * theSoftExtRatio : 0.0;
  const double aExtV =
    theUseSoftBounds ? (theBounds.Max[1] - theBounds.Min[1]) * theSoftExtRatio : 0.0;

  const double aUMin = theBounds.Min[0] - aExtU;
  const double aUMax = theBounds.Max[0] + aExtU;
  const double aVMin = theBounds.Min[1] - aExtV;
  const double aVMax = theBounds.Max[1] + aExtV;

  theX[0] = std::clamp(theX[0], aUMin, aUMax);
  theX[1] = std::clamp(theX[1], aVMin, aVMax);
}

//! Solve 2x2 symmetric system using eigenvalue decomposition (SVD fallback).
//! More robust than Cramer's rule for ill-conditioned matrices.
//! @param[in] theJ11, theJ12, theJ22 symmetric Jacobian elements
//! @param[in] theF1, theF2 right-hand side
//! @param[out] theDU, theDV solution components
//! @param[in] theTol tolerance for eigenvalue regularization
//! @return true if solution found
inline bool SolveSymmetric2x2SVD(double  theJ11,
                                 double  theJ12,
                                 double  theJ22,
                                 double  theF1,
                                 double  theF2,
                                 double& theDU,
                                 double& theDV,
                                 double  theTol = 1.0e-15)
{
  const double aTrace   = theJ11 + theJ22;
  const double aDiff    = (theJ11 - theJ22) * 0.5;
  const double aDiscrim = std::sqrt(aDiff * aDiff + theJ12 * theJ12);

  const double aLambda1 = aTrace * 0.5 + aDiscrim;
  const double aLambda2 = aTrace * 0.5 - aDiscrim;

  const double aMinLambda = std::max(std::abs(aLambda1), std::abs(aLambda2)) * theTol;
  if (std::abs(aLambda1) < aMinLambda && std::abs(aLambda2) < aMinLambda)
  {
    return false;
  }

  double aV1x = 0.0;
  double aV1y = 0.0;
  if (std::abs(theJ12) > std::abs(aDiff))
  {
    const double aLen1 = std::sqrt(theJ12 * theJ12 + (aLambda1 - theJ11) * (aLambda1 - theJ11));
    if (aLen1 < theTol)
    {
      return false;
    }

    aV1x = theJ12 / aLen1;
    aV1y = (aLambda1 - theJ11) / aLen1;
  }
  else
  {
    const double aLen1 = std::sqrt((aLambda1 - theJ22) * (aLambda1 - theJ22) + theJ12 * theJ12);
    if (aLen1 < theTol)
    {
      return false;
    }

    aV1x = (aLambda1 - theJ22) / aLen1;
    aV1y = theJ12 / aLen1;
  }

  const double aV2x = -aV1y;
  const double aV2y = aV1x;

  const double aB1 = aV1x * (-theF1) + aV1y * (-theF2);
  const double aB2 = aV2x * (-theF1) + aV2y * (-theF2);

  const double aX1 = (std::abs(aLambda1) > aMinLambda) ? aB1 / aLambda1 : 0.0;
  const double aX2 = (std::abs(aLambda2) > aMinLambda) ? aB2 / aLambda2 : 0.0;

  theDU = aV1x * aX1 + aV2x * aX2;
  theDV = aV1y * aX1 + aV2y * aX2;
  return true;
}

} // namespace detail

//! Solve a general 2x2 nonlinear system by Newton iteration.
//! Function contract:
//! bool ValueAndJacobian(double u, double v,
//!                       double& f1, double& f2,
//!                       double& j11, double& j12,
//!                       double& j21, double& j22) const;
template <typename Function>
NewtonResultN<2> Solve2D(const Function&              theFunc,
                         const std::array<double, 2>& theX0,
                         const NewtonBoundsN<2>&      theBounds,
                         const NewtonOptions&         theOptions = NewtonOptions())
{
  NewtonResultN<2> aRes;
  aRes.X = theX0;

  if (!detail::IsOptionsValid(theOptions) || !detail::IsBoundsValid2D(theBounds))
  {
    aRes.Status = MathUtils::Status::InvalidInput;
    return aRes;
  }

  detail::Clamp2D(aRes.X, theBounds, false, 0.0);

  const double aTolSq   = theOptions.FTolerance * theOptions.FTolerance;
  const double aMaxStep = theOptions.MaxStepRatio * detail::MaxDomainSize2D(theBounds);

  for (int anIter = 0; anIter < theOptions.MaxIterations; ++anIter)
  {
    aRes.NbIterations = static_cast<size_t>(anIter + 1);

    double aF1, aF2, aJ11, aJ12, aJ21, aJ22;
    if (!theFunc.ValueAndJacobian(aRes.X[0], aRes.X[1], aF1, aF2, aJ11, aJ12, aJ21, aJ22))
    {
      aRes.Status = MathUtils::Status::NumericalError;
      return aRes;
    }

    const double aFNormSq = aF1 * aF1 + aF2 * aF2;
    aRes.ResidualNorm     = std::sqrt(aFNormSq);

    if (aFNormSq <= aTolSq)
    {
      aRes.Status = MathUtils::Status::OK;
      return aRes;
    }

    double aDU = 0.0;
    double aDV = 0.0;

    const double aDet = aJ11 * aJ22 - aJ12 * aJ21;
    if (std::abs(aDet) < detail::THE_SINGULAR_DET_TOL)
    {
      const double aGradU  = aJ11 * aF1 + aJ21 * aF2;
      const double aGradV  = aJ12 * aF1 + aJ22 * aF2;
      const double aGradSq = aGradU * aGradU + aGradV * aGradV;
      if (aGradSq < detail::THE_CRITICAL_GRAD_SQ)
      {
        aRes.Status = MathUtils::Status::Singular;
        return aRes;
      }

      const double aAlpha = std::min(1.0, std::sqrt(aFNormSq / aGradSq) * 0.1);
      aDU                 = -aAlpha * aGradU;
      aDV                 = -aAlpha * aGradV;
    }
    else
    {
      const double aInvDet = 1.0 / aDet;
      aDU                  = (-aF1 * aJ22 + aF2 * aJ12) * aInvDet;
      aDV                  = (-aF2 * aJ11 + aF1 * aJ21) * aInvDet;
    }

    const double aStepNormSq = aDU * aDU + aDV * aDV;
    const double aStepNorm   = std::sqrt(aStepNormSq);
    if (aStepNorm > aMaxStep)
    {
      const double aScale = aMaxStep / aStepNorm;
      aDU *= aScale;
      aDV *= aScale;
    }

    std::array<double, 2> aNewX = {aRes.X[0] + aDU, aRes.X[1] + aDV};
    detail::Clamp2D(aNewX, theBounds, false, 0.0);

    aRes.StepNorm = std::sqrt((aNewX[0] - aRes.X[0]) * (aNewX[0] - aRes.X[0])
                              + (aNewX[1] - aRes.X[1]) * (aNewX[1] - aRes.X[1]));
    aRes.X        = aNewX;

    const double aScaleRef = std::max(1.0, std::max(std::abs(aRes.X[0]), std::abs(aRes.X[1])));
    if (aRes.StepNorm <= theOptions.XTolerance * aScaleRef)
    {
      aRes.Status = MathUtils::Status::MaxIterations;
      return aRes;
    }
  }

  double aF1, aF2, aJ11, aJ12, aJ21, aJ22;
  if (!theFunc.ValueAndJacobian(aRes.X[0], aRes.X[1], aF1, aF2, aJ11, aJ12, aJ21, aJ22))
  {
    aRes.Status = MathUtils::Status::NumericalError;
    return aRes;
  }

  aRes.ResidualNorm = std::sqrt(aF1 * aF1 + aF2 * aF2);
  aRes.Status       = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                                    : MathUtils::Status::MaxIterations;
  return aRes;
}

//! Solve a 2x2 system with symmetric Jacobian by robust Newton iteration.
//! Function contract:
//! bool ValueAndJacobian(double u, double v,
//!                       double& f1, double& f2,
//!                       double& j11, double& j12, double& j22) const;
//! bool Value(double u, double v, double& f1, double& f2) const; // required if line search is
//! enabled
template <typename Function>
NewtonResultN<2> Solve2DSymmetric(const Function&              theFunc,
                                  const std::array<double, 2>& theX0,
                                  const NewtonBoundsN<2>&      theBounds,
                                  const NewtonOptions&         theOptions = NewtonOptions())
{
  NewtonResultN<2> aRes;
  aRes.X = theX0;

  if (!detail::IsOptionsValid(theOptions) || !detail::IsBoundsValid2D(theBounds))
  {
    aRes.Status = MathUtils::Status::InvalidInput;
    return aRes;
  }

  detail::Clamp2D(aRes.X, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);

  const double aTolSq   = theOptions.FTolerance * theOptions.FTolerance;
  const double aMaxStep = theOptions.MaxStepRatio * detail::MaxDomainSize2D(theBounds);

  for (int anIter = 0; anIter < theOptions.MaxIterations; ++anIter)
  {
    aRes.NbIterations = static_cast<size_t>(anIter + 1);

    double aF1, aF2, aJ11, aJ12, aJ22;
    if (!theFunc.ValueAndJacobian(aRes.X[0], aRes.X[1], aF1, aF2, aJ11, aJ12, aJ22))
    {
      aRes.Status = MathUtils::Status::NumericalError;
      return aRes;
    }

    const double aFNormSq = aF1 * aF1 + aF2 * aF2;
    aRes.ResidualNorm     = std::sqrt(aFNormSq);

    if (aFNormSq <= aTolSq)
    {
      aRes.Status = MathUtils::Status::OK;
      return aRes;
    }

    double aDU = 0.0;
    double aDV = 0.0;

    const double aDet = aJ11 * aJ22 - aJ12 * aJ12;
    if (std::abs(aDet) < detail::THE_SINGULAR_DET_TOL)
    {
      if (!detail::SolveSymmetric2x2SVD(aJ11, aJ12, aJ22, aF1, aF2, aDU, aDV))
      {
        const double aGradU  = aJ11 * aF1 + aJ12 * aF2;
        const double aGradV  = aJ12 * aF1 + aJ22 * aF2;
        const double aGradSq = aGradU * aGradU + aGradV * aGradV;
        if (aGradSq < detail::THE_CRITICAL_GRAD_SQ)
        {
          aRes.Status = MathUtils::Status::Singular;
          return aRes;
        }

        const double aAlpha = std::min(1.0, std::sqrt(aFNormSq / aGradSq) * 0.1);
        aDU                 = -aAlpha * aGradU;
        aDV                 = -aAlpha * aGradV;
      }
    }
    else
    {
      const double aInvDet = 1.0 / aDet;
      aDU                  = (-aF1 * aJ22 + aF2 * aJ12) * aInvDet;
      aDV                  = (-aF2 * aJ11 + aF1 * aJ12) * aInvDet;
    }

    const double aStepNormSq = aDU * aDU + aDV * aDV;
    const double aStepNorm   = std::sqrt(aStepNormSq);
    if (aStepNorm > aMaxStep)
    {
      const double aScale = aMaxStep / aStepNorm;
      aDU *= aScale;
      aDV *= aScale;
    }

    const double aGradPhiU = aJ11 * aF1 + aJ12 * aF2;
    const double aGradPhiV = aJ12 * aF1 + aJ22 * aF2;
    const double aDirDeriv = aGradPhiU * aDU + aGradPhiV * aDV;

    std::array<double, 2> aNewX            = aRes.X;
    double                aNewResidualNorm = -1.0;

    if (theOptions.EnableLineSearch)
    {
      if (aDirDeriv >= 0.0)
      {
        aRes.Status = MathUtils::Status::NonDescentDirection;
        return aRes;
      }

      const double aPhi0      = 0.5 * aFNormSq;
      double       aAlpha     = 1.0;
      bool         isAccepted = false;

      for (int k = 0; k < detail::THE_LINE_SEARCH_MAX; ++k)
      {
        aNewX[0] = aRes.X[0] + aAlpha * aDU;
        aNewX[1] = aRes.X[1] + aAlpha * aDV;
        detail::Clamp2D(aNewX,
                        theBounds,
                        theOptions.AllowSoftBounds,
                        theOptions.AllowSoftBounds ? theOptions.SoftBoundsExtension : 0.0);

        double aTryF1, aTryF2;
        if (!theFunc.Value(aNewX[0], aNewX[1], aTryF1, aTryF2))
        {
          aAlpha *= 0.5;
          continue;
        }

        const double aTryFNormSq  = aTryF1 * aTryF1 + aTryF2 * aTryF2;
        const double aTryPhi      = 0.5 * aTryFNormSq;
        const double aArmijoBound = aPhi0 + detail::THE_ARMIJO_C1 * aAlpha * aDirDeriv;
        if (aTryPhi <= aArmijoBound)
        {
          isAccepted       = true;
          aNewResidualNorm = std::sqrt(aTryFNormSq);
          break;
        }

        aAlpha *= 0.5;
      }

      if (!isAccepted)
      {
        aRes.Status = MathUtils::Status::NonDescentDirection;
        return aRes;
      }
    }
    else
    {
      aNewX[0] += aDU;
      aNewX[1] += aDV;
      detail::Clamp2D(aNewX,
                      theBounds,
                      theOptions.AllowSoftBounds,
                      theOptions.AllowSoftBounds ? theOptions.SoftBoundsExtension : 0.0);
    }

    aRes.StepNorm = std::sqrt((aNewX[0] - aRes.X[0]) * (aNewX[0] - aRes.X[0])
                              + (aNewX[1] - aRes.X[1]) * (aNewX[1] - aRes.X[1]));
    aRes.X        = aNewX;

    if (aNewResidualNorm >= 0.0)
    {
      aRes.ResidualNorm = aNewResidualNorm;
    }

    const double aScaleRef = std::max(1.0, std::max(std::abs(aRes.X[0]), std::abs(aRes.X[1])));
    if (aRes.StepNorm <= theOptions.XTolerance * aScaleRef)
    {
      double aCheckF1, aCheckF2;
      if (!theFunc.Value(aRes.X[0], aRes.X[1], aCheckF1, aCheckF2))
      {
        aRes.Status = MathUtils::Status::NumericalError;
        return aRes;
      }

      aRes.ResidualNorm = std::sqrt(aCheckF1 * aCheckF1 + aCheckF2 * aCheckF2);
      aRes.Status       = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                                        : MathUtils::Status::MaxIterations;
      return aRes;
    }
  }

  double aF1, aF2;
  if (!theFunc.Value(aRes.X[0], aRes.X[1], aF1, aF2))
  {
    aRes.Status = MathUtils::Status::NumericalError;
    return aRes;
  }

  aRes.ResidualNorm = std::sqrt(aF1 * aF1 + aF2 * aF2);
  aRes.Status       = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                                    : MathUtils::Status::MaxIterations;
  return aRes;
}

} // namespace MathSys

#endif // _MathSys_Newton2D_HeaderFile

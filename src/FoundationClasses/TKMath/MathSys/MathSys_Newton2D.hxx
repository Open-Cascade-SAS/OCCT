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
#include <cstdint>
#include <limits>
#include <optional>

//! @file MathSys_Newton2D.hxx
//! @brief Optimized 2D Newton-Raphson solvers with strict convergence criteria.
//!
//! Specifically optimized for 2D problems like:
//! - Point-surface extrema (find u,v where gradient is zero)
//! - Curve intersection (find t1,t2 where curves meet)
//! - Surface intersection curves

namespace MathSys
{
namespace Utils
{

constexpr double THE_CRITICAL_GRAD    = 1.0e-30;
constexpr double THE_CRITICAL_GRAD_SQ = 1.0e-60;

//! Check that NewtonOptions fields are positive and valid.
inline bool IsOptionsValid(const NewtonOptions& theOptions)
{
  return std::isfinite(theOptions.FTolerance) && theOptions.FTolerance > 0.0
         && std::isfinite(theOptions.XTolerance) && theOptions.XTolerance > 0.0
         && theOptions.MaxIterations > 0 && std::isfinite(theOptions.MaxStepRatio)
         && theOptions.MaxStepRatio > 0.0 && std::isfinite(theOptions.SoftBoundsExtension)
         && theOptions.SoftBoundsExtension >= 0.0;
}

//! Check that NewtonBoundsN<2> has valid (min <= max) ranges.
inline bool IsBoundsValid2D(const NewtonBoundsN<2>& theBounds)
{
  if (!theBounds.HasBounds)
  {
    return true;
  }
  return Utils::IsFiniteArray(theBounds.Min) && Utils::IsFiniteArray(theBounds.Max)
         && theBounds.Min[0] <= theBounds.Max[0] && theBounds.Min[1] <= theBounds.Max[1];
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
  const double aScale = std::max({std::abs(theJ11), std::abs(theJ12), std::abs(theJ22)});
  if (!(aScale > 0.0))
  {
    return false;
  }
  const double aJ11 = theJ11 / aScale;
  const double aJ12 = theJ12 / aScale;
  const double aJ22 = theJ22 / aScale;
  const double aF1  = theF1 / aScale;
  const double aF2  = theF2 / aScale;
  if (!std::isfinite(aF1) || !std::isfinite(aF2))
  {
    return false;
  }

  const double aTrace   = aJ11 + aJ22;
  const double aDiff    = (aJ11 - aJ22) * 0.5;
  const double aDiscrim = std::hypot(aDiff, aJ12);

  const double aLambda1 = aTrace * 0.5 + aDiscrim;
  const double aLambda2 = aTrace * 0.5 - aDiscrim;

  const double aMinLambda = std::max(std::abs(aLambda1), std::abs(aLambda2)) * theTol;
  if (std::abs(aLambda1) < aMinLambda && std::abs(aLambda2) < aMinLambda)
  {
    return false;
  }

  double aV1x = 0.0;
  double aV1y = 0.0;
  if (std::abs(aJ12) > std::abs(aDiff))
  {
    const double aLen1 = std::hypot(aJ12, aLambda1 - aJ11);
    if (aLen1 < theTol)
    {
      return false;
    }

    aV1x = aJ12 / aLen1;
    aV1y = (aLambda1 - aJ11) / aLen1;
  }
  else
  {
    const double aLen1 = std::hypot(aLambda1 - aJ22, aJ12);
    if (aLen1 < theTol)
    {
      return false;
    }

    aV1x = (aLambda1 - aJ22) / aLen1;
    aV1y = aJ12 / aLen1;
  }

  const double aV2x = -aV1y;
  const double aV2y = aV1x;

  const double aB1 = aV1x * (-aF1) + aV1y * (-aF2);
  const double aB2 = aV2x * (-aF1) + aV2y * (-aF2);

  const double aX1 = (std::abs(aLambda1) > aMinLambda) ? aB1 / aLambda1 : 0.0;
  const double aX2 = (std::abs(aLambda2) > aMinLambda) ? aB2 / aLambda2 : 0.0;

  theDU = aV1x * aX1 + aV2x * aX2;
  theDV = aV1y * aX1 + aV2y * aX2;
  return std::isfinite(theDU) && std::isfinite(theDV);
}

} // namespace Utils

//! Solve a general 2x2 nonlinear system by Newton iteration.
//! Function contract:
//! bool operator()(double u, double v,
//!                 double f[2], double j[2][2]) const;
template <typename Function>
NewtonResultN<2> Solve2D(const Function&              theFunc,
                         const std::array<double, 2>& theX0,
                         const NewtonBoundsN<2>&      theBounds,
                         const NewtonOptions&         theOptions = NewtonOptions())
{
  NewtonResultN<2> aRes;
  aRes.X = theX0;

  if (!Utils::IsOptionsValid(theOptions) || !Utils::IsBoundsValid2D(theBounds)
      || !Utils::IsFiniteArray(theX0))
  {
    aRes.Status = MathUtils::Status::InvalidInput;
    return aRes;
  }

  Utils::Clamp2D(aRes.X, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);

  const double aMaxStep = theOptions.MaxStepRatio * Utils::MaxDomainSize2D(theBounds);

  for (uint32_t anIter = 0; anIter < theOptions.MaxIterations; ++anIter)
  {
    aRes.NbIterations = anIter + 1;

    double aF[2];
    double aJ[2][2];
    if (!theFunc(aRes.X[0], aRes.X[1], aF, aJ) || !Utils::IsFiniteSystemN(aF, aJ))
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

    double aDU = 0.0;
    double aDV = 0.0;

    const double aJScale =
      std::max({std::abs(aJ[0][0]), std::abs(aJ[0][1]), std::abs(aJ[1][0]), std::abs(aJ[1][1])});
    const double aS00 = (aJScale > 0.0) ? aJ[0][0] / aJScale : 0.0;
    const double aS01 = (aJScale > 0.0) ? aJ[0][1] / aJScale : 0.0;
    const double aS10 = (aJScale > 0.0) ? aJ[1][0] / aJScale : 0.0;
    const double aS11 = (aJScale > 0.0) ? aJ[1][1] / aJScale : 0.0;
    const double aDet = aS00 * aS11 - aS01 * aS10;
    if (!(aJScale > 0.0) || std::abs(aDet) <= 64.0 * std::numeric_limits<double>::epsilon())
    {
      const double aGradU    = aJ[0][0] * aF[0] + aJ[1][0] * aF[1];
      const double aGradV    = aJ[0][1] * aF[0] + aJ[1][1] * aF[1];
      const double aGradNorm = std::hypot(aGradU, aGradV);
      if (aGradNorm < Utils::THE_CRITICAL_GRAD)
      {
        aRes.Status = MathUtils::Status::Singular;
        return aRes;
      }

      const double aAlpha = std::min(1.0, aRes.ResidualNorm / aGradNorm * 0.1);
      aDU                 = -aAlpha * aGradU;
      aDV                 = -aAlpha * aGradV;
    }
    else
    {
      const double aScaledF0 = aF[0] / aJScale;
      const double aScaledF1 = aF[1] / aJScale;
      const double aInvDet   = 1.0 / aDet;
      aDU                    = (-aScaledF0 * aS11 + aScaledF1 * aS01) * aInvDet;
      aDV                    = (-aScaledF1 * aS00 + aScaledF0 * aS10) * aInvDet;
    }

    const double aStepNorm = std::hypot(aDU, aDV);
    if (aStepNorm > aMaxStep)
    {
      const double aScale = aMaxStep / aStepNorm;
      aDU *= aScale;
      aDV *= aScale;
    }

    std::array<double, 2> aNewX = aRes.X;
    if (theOptions.EnableLineSearch)
    {
      bool isAccepted = false;
      for (size_t aLineIter = 0; aLineIter < Utils::THE_LINE_SEARCH_MAX; ++aLineIter)
      {
        const double anAlpha = std::ldexp(1.0, -static_cast<int>(aLineIter));
        aNewX                = {aRes.X[0] + anAlpha * aDU, aRes.X[1] + anAlpha * aDV};
        Utils::Clamp2D(aNewX,
                       theBounds,
                       theOptions.AllowSoftBounds,
                       theOptions.SoftBoundsExtension);
        const std::array<double, 2> aProjectedStep = {aNewX[0] - aRes.X[0], aNewX[1] - aRes.X[1]};
        const long double aDerivative = Utils::MeritDirectionalDerivative(aF, aJ, aProjectedStep);
        double            aTrialF[2];
        double            aTrialJ[2][2];
        if (theFunc(aNewX[0], aNewX[1], aTrialF, aTrialJ)
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
      aNewX = {aRes.X[0] + aDU, aRes.X[1] + aDV};
      Utils::Clamp2D(aNewX, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);
    }

    const bool isStepWithinTolerance =
      Utils::IsStepWithinTolerance(aNewX, aRes.X, theOptions.XTolerance);
    aRes.StepNorm = Utils::SafeDistanceN(aNewX, aRes.X);
    aRes.X        = aNewX;

    if (isStepWithinTolerance)
    {
      double aCheckF[2];
      double aCheckJ[2][2];
      if (!theFunc(aRes.X[0], aRes.X[1], aCheckF, aCheckJ)
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

  double aF[2];
  double aJ[2][2];
  if (!theFunc(aRes.X[0], aRes.X[1], aF, aJ) || !Utils::IsFiniteSystemN(aF, aJ))
  {
    aRes.Status = MathUtils::Status::NumericalError;
    return aRes;
  }

  aRes.ResidualNorm = Utils::SafeNormN(aF);
  aRes.Status = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
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

  if (!Utils::IsOptionsValid(theOptions) || !Utils::IsBoundsValid2D(theBounds)
      || !Utils::IsFiniteArray(theX0))
  {
    aRes.Status = MathUtils::Status::InvalidInput;
    return aRes;
  }

  Utils::Clamp2D(aRes.X, theBounds, theOptions.AllowSoftBounds, theOptions.SoftBoundsExtension);

  const double aMaxStep = theOptions.MaxStepRatio * Utils::MaxDomainSize2D(theBounds);

  for (uint32_t anIter = 0; anIter < theOptions.MaxIterations; ++anIter)
  {
    aRes.NbIterations = anIter + 1;

    double aF1, aF2, aJ11, aJ12, aJ22;
    if (!theFunc.ValueAndJacobian(aRes.X[0], aRes.X[1], aF1, aF2, aJ11, aJ12, aJ22)
        || !std::isfinite(aF1) || !std::isfinite(aF2) || !std::isfinite(aJ11)
        || !std::isfinite(aJ12) || !std::isfinite(aJ22))
    {
      aRes.Status = MathUtils::Status::NumericalError;
      return aRes;
    }

    aRes.ResidualNorm = std::hypot(aF1, aF2);
    if (aRes.ResidualNorm <= theOptions.FTolerance)
    {
      aRes.Status = MathUtils::Status::OK;
      return aRes;
    }

    double aDU = 0.0;
    double aDV = 0.0;

    const double aJScale = std::max({std::abs(aJ11), std::abs(aJ12), std::abs(aJ22)});
    const double aS11    = (aJScale > 0.0) ? aJ11 / aJScale : 0.0;
    const double aS12    = (aJScale > 0.0) ? aJ12 / aJScale : 0.0;
    const double aS22    = (aJScale > 0.0) ? aJ22 / aJScale : 0.0;
    const double aDet    = aS11 * aS22 - aS12 * aS12;
    if (!(aJScale > 0.0) || std::abs(aDet) <= 64.0 * std::numeric_limits<double>::epsilon())
    {
      if (!Utils::SolveSymmetric2x2SVD(aJ11, aJ12, aJ22, aF1, aF2, aDU, aDV))
      {
        const double aFScale = std::max(std::abs(aF1), std::abs(aF2));
        const double aGradU  = aS11 * (aF1 / aFScale) + aS12 * (aF2 / aFScale);
        const double aGradV  = aS12 * (aF1 / aFScale) + aS22 * (aF2 / aFScale);
        const double aGradNorm = std::hypot(aGradU, aGradV);
        if (!(aGradNorm >= Utils::THE_CRITICAL_GRAD))
        {
          aRes.Status = MathUtils::Status::Singular;
          return aRes;
        }

        const double aAlpha = (aRes.ResidualNorm >= 10.0 * aGradNorm)
                                ? 1.0
                                : 0.1 * aRes.ResidualNorm / aGradNorm;
        aDU                 = -aAlpha * aGradU / aGradNorm;
        aDV                 = -aAlpha * aGradV / aGradNorm;
      }
    }
    else
    {
      const double aScaledF1 = aF1 / aJScale;
      const double aScaledF2 = aF2 / aJScale;
      const double aInvDet   = 1.0 / aDet;
      aDU                    = (-aScaledF1 * aS22 + aScaledF2 * aS12) * aInvDet;
      aDV                    = (-aScaledF2 * aS11 + aScaledF1 * aS12) * aInvDet;
    }

    const double aStepNorm = std::hypot(aDU, aDV);
    if (aStepNorm > aMaxStep)
    {
      const double aScale = aMaxStep / aStepNorm;
      aDU *= aScale;
      aDV *= aScale;
    }

    std::array<double, 2> aNewX            = aRes.X;
    std::optional<double> aNewResidualNorm;

    if (theOptions.EnableLineSearch)
    {
      bool isAccepted = false;

      for (size_t aLineIter = 0; aLineIter < Utils::THE_LINE_SEARCH_MAX; ++aLineIter)
      {
        const double aAlpha = std::ldexp(1.0, -static_cast<int>(aLineIter));
        aNewX[0] = aRes.X[0] + aAlpha * aDU;
        aNewX[1] = aRes.X[1] + aAlpha * aDV;
        Utils::Clamp2D(aNewX,
                       theBounds,
                       theOptions.AllowSoftBounds,
                       theOptions.AllowSoftBounds ? theOptions.SoftBoundsExtension : 0.0);

        const double                 aF[2] = {aF1, aF2};
        const double                 aJ[2][2] = {{aJ11, aJ12}, {aJ12, aJ22}};
        const std::array<double, 2> aProjectedStep = {aNewX[0] - aRes.X[0],
                                                       aNewX[1] - aRes.X[1]};
        const long double aProjectedDerivative =
          Utils::MeritDirectionalDerivative(aF, aJ, aProjectedStep);
        if (!(aProjectedDerivative < 0.0L))
        {
          continue;
        }

        double aTryF1, aTryF2;
        if (!theFunc.Value(aNewX[0], aNewX[1], aTryF1, aTryF2) || !std::isfinite(aTryF1)
            || !std::isfinite(aTryF2))
        {
          continue;
        }

        const double aTryF[2] = {aTryF1, aTryF2};
        const double aTryNorm = Utils::SafeNormN(aTryF);
        if (Utils::IsArmijoAccepted(aRes.ResidualNorm, aTryNorm, aProjectedDerivative))
        {
          isAccepted       = true;
          aNewResidualNorm = aTryNorm;
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
      aNewX[0] += aDU;
      aNewX[1] += aDV;
      Utils::Clamp2D(aNewX,
                     theBounds,
                     theOptions.AllowSoftBounds,
                     theOptions.AllowSoftBounds ? theOptions.SoftBoundsExtension : 0.0);
    }

    const bool isStepWithinTolerance =
      Utils::IsStepWithinTolerance(aNewX, aRes.X, theOptions.XTolerance);
    aRes.StepNorm = Utils::SafeDistanceN(aNewX, aRes.X);
    aRes.X        = aNewX;

    if (aNewResidualNorm.has_value())
    {
      aRes.ResidualNorm = *aNewResidualNorm;
    }

    if (isStepWithinTolerance)
    {
      double aCheckF1, aCheckF2;
      if (!theFunc.Value(aRes.X[0], aRes.X[1], aCheckF1, aCheckF2) || !std::isfinite(aCheckF1)
          || !std::isfinite(aCheckF2))
      {
        aRes.Status = MathUtils::Status::NumericalError;
        return aRes;
      }

      aRes.ResidualNorm = std::hypot(aCheckF1, aCheckF2);
      aRes.Status = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                                 : MathUtils::Status::MaxIterations;
      return aRes;
    }
  }

  double aF1, aF2;
  if (!theFunc.Value(aRes.X[0], aRes.X[1], aF1, aF2) || !std::isfinite(aF1) || !std::isfinite(aF2))
  {
    aRes.Status = MathUtils::Status::NumericalError;
    return aRes;
  }

  aRes.ResidualNorm = std::hypot(aF1, aF2);
  aRes.Status = (aRes.ResidualNorm <= theOptions.FTolerance) ? MathUtils::Status::OK
                                                             : MathUtils::Status::MaxIterations;
  return aRes;
}

} // namespace MathSys

#endif // _MathSys_Newton2D_HeaderFile

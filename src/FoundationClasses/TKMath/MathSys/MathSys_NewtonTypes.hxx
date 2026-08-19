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

#ifndef _MathSys_NewtonTypes_HeaderFile
#define _MathSys_NewtonTypes_HeaderFile

#include <MathUtils_Config.hxx>
#include <MathUtils_Types.hxx>

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>

//! Shared types for specialized small-dimension Newton solvers.
namespace MathSys
{
using namespace MathUtils;

//! Result of N-dimensional Newton solver.
template <size_t N>
struct NewtonResultN
{
  MathUtils::Status     Status       = MathUtils::Status::NotConverged; //!< Final solver status
  std::array<double, N> X            = {};                              //!< Solution estimate
  uint32_t              NbIterations = 0;                               //!< Performed iterations
  double                ResidualNorm = 0.0; //!< Final residual norm ||F||
  double                StepNorm     = 0.0; //!< Final step norm ||dX||

  //! Returns true if computation converged.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Box bounds for N-dimensional solver.
template <size_t N>
struct NewtonBoundsN
{
  std::array<double, N> Min       = {};   //!< Lower bounds
  std::array<double, N> Max       = {};   //!< Upper bounds
  bool                  HasBounds = true; //!< True to apply bounds
};

//! Solver options for small-dimension Newton methods.
struct NewtonOptions : MathUtils::Config
{
  double MaxStepRatio        = 0.5;    //!< Max step as ratio of largest domain size
  bool   EnableLineSearch    = true;   //!< Enable Armijo backtracking line search
  bool   AllowSoftBounds     = false;  //!< Allow slight bounds extension
  double SoftBoundsExtension = 1.0e-4; //!< Extension ratio for soft bounds

  //! Default constructor with strict residual/step tolerances for specialized Newton.
  NewtonOptions()
  {
    Tolerance     = 1.0e-10;
    XTolerance    = 1.0e-16;
    FTolerance    = 1.0e-10;
    MaxIterations = 100;
  }
};

namespace Utils
{

inline constexpr size_t THE_LINE_SEARCH_MAX = 8;
inline constexpr double THE_ARMIJO_C1       = 1.0e-4;
//! Relative pivot threshold used by fixed-size Newton linear solves.
inline constexpr double THE_NEWTON_PIVOT_TOL = 64.0 * std::numeric_limits<double>::epsilon();

template <size_t N>
inline bool IsFiniteArray(const std::array<double, N>& theValues)
{
  for (double aValue : theValues)
  {
    if (!std::isfinite(aValue))
    {
      return false;
    }
  }
  return true;
}

template <size_t N>
inline double SafeNormN(const double (&theValues)[N])
{
  double aNorm = 0.0;
  for (double aValue : theValues)
  {
    aNorm = std::hypot(aNorm, aValue);
  }
  return aNorm;
}

template <size_t N>
inline double SafeDistanceN(const std::array<double, N>& theLeft,
                            const std::array<double, N>& theRight)
{
  double aNorm = 0.0;
  for (size_t anIndex = 0; anIndex < N; ++anIndex)
  {
    aNorm = std::hypot(aNorm, theLeft[anIndex] - theRight[anIndex]);
  }
  return aNorm;
}

template <size_t N>
inline bool IsStepWithinTolerance(const std::array<double, N>& theLeft,
                                  const std::array<double, N>& theRight,
                                  double                       theTolerance)
{
  for (size_t anIndex = 0; anIndex < N; ++anIndex)
  {
    if (std::abs(theLeft[anIndex] - theRight[anIndex]) > theTolerance)
    {
      return false;
    }
  }
  return true;
}

template <size_t N>
inline long double MeritDirectionalDerivative(const double (&theF)[N],
                                              const double (&theJ)[N][N],
                                              const std::array<double, N>& theStep)
{
  long double aDerivative = 0.0L;
  for (size_t aRow = 0; aRow < N; ++aRow)
  {
    long double aRowDerivative = 0.0L;
    for (size_t aCol = 0; aCol < N; ++aCol)
    {
      aRowDerivative += static_cast<long double>(theJ[aRow][aCol]) * theStep[aCol];
    }
    aDerivative += static_cast<long double>(theF[aRow]) * aRowDerivative;
  }
  return aDerivative;
}

//! Test the Armijo condition after scaling by the current residual norm.
inline bool IsArmijoAccepted(double      theCurrentNorm,
                             double      theTrialNorm,
                             long double theDirectionalDerivative)
{
  if (!(theCurrentNorm > 0.0) || !(theDirectionalDerivative < 0.0L))
  {
    return false;
  }

  const long double aCurrentNorm          = theCurrentNorm;
  const long double aRatio                = static_cast<long double>(theTrialNorm) / aCurrentNorm;
  const long double aNormalizedDerivative = theDirectionalDerivative / aCurrentNorm / aCurrentNorm;
  const long double aBound                = 1.0L + 2.0L * THE_ARMIJO_C1 * aNormalizedDerivative;
  return aBound >= 0.0L && aRatio <= std::sqrt(aBound);
}

template <size_t N>
inline bool IsFiniteSystemN(const double (&theF)[N], const double (&theJ)[N][N])
{
  for (size_t aRow = 0; aRow < N; ++aRow)
  {
    if (!std::isfinite(theF[aRow]))
    {
      return false;
    }
    for (size_t aCol = 0; aCol < N; ++aCol)
    {
      if (!std::isfinite(theJ[aRow][aCol]))
      {
        return false;
      }
    }
  }
  return true;
}

} // namespace Utils

} // namespace MathSys

#endif // _MathSys_NewtonTypes_HeaderFile

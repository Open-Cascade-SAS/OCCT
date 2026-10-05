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

#ifndef _MathUtils_Convergence_HeaderFile
#define _MathUtils_Convergence_HeaderFile

#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <math_Vector.hxx>

#include <cmath>

//! Modern math solver utilities.
namespace MathUtils
{

//! Check convergence based on relative change in X.
//! Uses relative tolerance scaled by current value magnitude.
//! @param theXOld previous X value
//! @param theXNew current X value
//! @param theTolerance relative tolerance
//! @return true if converged
inline bool IsXConverged(double theXOld, double theXNew, double theTolerance)
{
  return IsWithinTolerance(theXOld, theXNew, theTolerance, theTolerance);
}

//! Check convergence based on absolute function value.
//! @param theFValue function value f(x)
//! @param theTolerance absolute tolerance
//! @return true if |f(x)| < tolerance
inline bool IsFConverged(double theFValue, double theTolerance)
{
  return std::isfinite(theFValue) && std::isfinite(theTolerance) && theTolerance >= 0.0
         && std::abs(theFValue) <= theTolerance;
}

//! Combined convergence test for scalar root finders.
//! Checks both X convergence and function value convergence.
//! @param theXOld previous X value
//! @param theXNew current X value
//! @param theFValue function value at theXNew
//! @param theConfig solver configuration
//! @return true if either criterion is satisfied
inline bool IsConverged(double theXOld, double theXNew, double theFValue, const Config& theConfig)
{
  if (!theConfig.IsValid() || !std::isfinite(theFValue))
  {
    return false;
  }
  return IsWithinTolerance(theXOld, theXNew, theConfig.XTolerance, theConfig.RelativeTolerance)
         || IsFConverged(theFValue, theConfig.FTolerance);
}

//! Convergence test for minimization (checks both X and F change).
//! @param theXOld previous X value
//! @param theXNew current X value
//! @param theFOld previous function value
//! @param theFNew current function value
//! @param theConfig solver configuration
//! @return true if converged
inline bool IsMinConverged(double        theXOld,
                           double        theXNew,
                           double        theFOld,
                           double        theFNew,
                           const Config& theConfig)
{
  if (!theConfig.IsValid() || !std::isfinite(theFOld) || !std::isfinite(theFNew))
  {
    return false;
  }
  if (IsWithinTolerance(theXOld, theXNew, theConfig.XTolerance, theConfig.RelativeTolerance))
  {
    return true;
  }

  // Relative function change convergence
  return IsWithinTolerance(theFOld, theFNew, theConfig.FTolerance, theConfig.RelativeTolerance);
}

//! Convergence test for vector solvers using infinity norm.
//! @param theOld previous solution vector
//! @param theNew current solution vector
//! @param theTolerance relative tolerance
//! @return true if max|new_i - old_i| / max(1, |new_i|) < tolerance
inline bool IsVectorConverged(const math_Vector& theOld,
                              const math_Vector& theNew,
                              double             theTolerance)
{
  if (theOld.Size() != theNew.Size() || !std::isfinite(theTolerance) || theTolerance < 0.0)
  {
    return false;
  }
  for (size_t i = 0; i < theOld.Size(); ++i)
  {
    const double anOld = theOld.At(i);
    const double aNew  = theNew.At(i);
    if (!IsWithinTolerance(anOld, aNew, theTolerance, theTolerance))
    {
      return false;
    }
  }
  return true;
}

//! Convergence test using gradient norm for minimization.
//! @param theGradient gradient vector
//! @param theTolerance tolerance for gradient norm
//! @return true if ||gradient|| < tolerance
inline bool IsGradientConverged(const math_Vector& theGradient, double theTolerance)
{
  return std::isfinite(theTolerance) && theTolerance >= 0.0
         && VectorNorm(theGradient) <= theTolerance;
}

//! Compute infinity norm of a vector.
//! @param theVector input vector
//! @return max|v_i|
inline double InfinityNorm(const math_Vector& theVector)
{
  double aMax = 0.0;
  for (size_t i = 0; i < theVector.Size(); ++i)
  {
    aMax = std::max(aMax, std::abs(theVector.At(i)));
  }
  return aMax;
}

//! Compute Euclidean (L2) norm of a vector.
//! @param theVector input vector
//! @return sqrt(sum(v_i^2))
inline double EuclideanNorm(const math_Vector& theVector)
{
  return VectorNorm(theVector);
}

} // namespace MathUtils

#endif // _MathUtils_Convergence_HeaderFile

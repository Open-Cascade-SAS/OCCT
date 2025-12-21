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
  const double aDiff  = std::abs(theXNew - theXOld);
  const double aScale = std::max(1.0, std::abs(theXNew));
  return aDiff < theTolerance * aScale;
}

//! Check convergence based on absolute function value.
//! @param theFValue function value f(x)
//! @param theTolerance absolute tolerance
//! @return true if |f(x)| < tolerance
inline bool IsFConverged(double theFValue, double theTolerance)
{
  return std::abs(theFValue) < theTolerance;
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
  return IsXConverged(theXOld, theXNew, theConfig.XTolerance)
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
  // X convergence
  if (IsXConverged(theXOld, theXNew, theConfig.XTolerance))
  {
    return true;
  }

  // Relative function change convergence
  const double aFDiff  = std::abs(theFNew - theFOld);
  const double aFScale = std::max(1.0, std::abs(theFNew));
  return aFDiff < theConfig.FTolerance * aFScale;
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
  double aMaxDiff  = 0.0;
  double aMaxScale = 1.0;
  for (int i = theOld.Lower(); i <= theOld.Upper(); ++i)
  {
    aMaxDiff  = std::max(aMaxDiff, std::abs(theNew(i) - theOld(i)));
    aMaxScale = std::max(aMaxScale, std::abs(theNew(i)));
  }
  return aMaxDiff < theTolerance * aMaxScale;
}

//! Convergence test using gradient norm for minimization.
//! @param theGradient gradient vector
//! @param theTolerance tolerance for gradient norm
//! @return true if ||gradient|| < tolerance
inline bool IsGradientConverged(const math_Vector& theGradient, double theTolerance)
{
  double aNormSq = 0.0;
  for (int i = theGradient.Lower(); i <= theGradient.Upper(); ++i)
  {
    aNormSq += Sqr(theGradient(i));
  }
  return std::sqrt(aNormSq) < theTolerance;
}

//! Compute infinity norm of a vector.
//! @param theVector input vector
//! @return max|v_i|
inline double InfinityNorm(const math_Vector& theVector)
{
  double aMax = 0.0;
  for (int i = theVector.Lower(); i <= theVector.Upper(); ++i)
  {
    aMax = std::max(aMax, std::abs(theVector(i)));
  }
  return aMax;
}

//! Compute Euclidean (L2) norm of a vector.
//! @param theVector input vector
//! @return sqrt(sum(v_i^2))
inline double EuclideanNorm(const math_Vector& theVector)
{
  double aSumSq = 0.0;
  for (int i = theVector.Lower(); i <= theVector.Upper(); ++i)
  {
    aSumSq += Sqr(theVector(i));
  }
  return std::sqrt(aSumSq);
}

} // namespace MathUtils

#endif // _MathUtils_Convergence_HeaderFile

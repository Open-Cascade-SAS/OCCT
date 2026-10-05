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

#ifndef _MathUtils_Core_HeaderFile
#define _MathUtils_Core_HeaderFile

#include <math_Vector.hxx>

#include <cmath>
#include <algorithm>
#include <limits>

//! Modern math solver utilities.
namespace MathUtils
{

//! Machine epsilon for double precision.
inline constexpr double THE_EPSILON = std::numeric_limits<double>::epsilon();

//! Small value for zero comparisons (more practical than epsilon).
inline constexpr double THE_ZERO_TOL = 1.0e-15;

//! Pi constant.
inline constexpr double THE_PI = 3.14159265358979323846;

//! Two Pi constant.
inline constexpr double THE_2PI = 6.28318530717958647692;

//! Euler's number, the base of natural logarithms.
inline constexpr double THE_EULER_NUMBER = 2.718281828459045;

//! Golden ratio for optimization algorithms.
inline constexpr double THE_GOLDEN_RATIO = 1.618033988749895;

//! Inverse golden ratio (1 - 1/phi).
inline constexpr double THE_GOLDEN_SECTION = 0.381966011250105;

//! Clamp value to range [theLower, theUpper].
//! @param theValue value to clamp
//! @param theLower lower bound
//! @param theUpper upper bound
//! @return clamped value
inline constexpr double Clamp(double theValue, double theLower, double theUpper)
{
  return (theValue < theLower) ? theLower : ((theValue > theUpper) ? theUpper : theValue);
}

//! Check if value is effectively zero.
//! @param theValue value to check
//! @param theTolerance tolerance for zero comparison
//! @return true if |theValue| < theTolerance
inline bool IsZero(double theValue, double theTolerance = THE_ZERO_TOL)
{
  return std::isfinite(theValue) && std::isfinite(theTolerance) && theTolerance >= 0.0
         && std::abs(theValue) <= theTolerance;
}

//! Check if two values are approximately equal.
//! @param theA first value
//! @param theB second value
//! @param theTolerance relative tolerance
//! @return true if values are approximately equal
inline bool IsEqual(double theA, double theB, double theTolerance = THE_ZERO_TOL)
{
  if (!std::isfinite(theA) || !std::isfinite(theB) || !std::isfinite(theTolerance)
      || theTolerance < 0.0)
  {
    return false;
  }
  const double aDiff = std::abs(theA - theB);
  if (!std::isfinite(aDiff))
  {
    return false;
  }
  const double aScale = std::max({1.0, std::abs(theA), std::abs(theB)});
  return aDiff / aScale <= theTolerance;
}

//! Test a difference against coherent absolute and relative tolerances.
inline bool IsWithinTolerance(double theA,
                              double theB,
                              double theAbsTolerance,
                              double theRelTolerance)
{
  if (!std::isfinite(theA) || !std::isfinite(theB) || !std::isfinite(theAbsTolerance)
      || !std::isfinite(theRelTolerance) || theAbsTolerance < 0.0 || theRelTolerance < 0.0)
  {
    return false;
  }
  const double aDiff = std::abs(theA - theB);
  if (!std::isfinite(aDiff))
  {
    return false;
  }
  const double aScale = std::max(std::abs(theA), std::abs(theB));
  if (aScale == 0.0)
  {
    return true;
  }
  if (aDiff <= theAbsTolerance)
  {
    return true;
  }
  return (aDiff - theAbsTolerance) / aScale <= theRelTolerance;
}

//! Safe division avoiding division by zero.
//! @param theNumerator numerator
//! @param theDenominator denominator
//! @param theDefault default value if denominator is zero
//! @return theNumerator / theDenominator or theDefault
inline double SafeDiv(double theNumerator, double theDenominator, double theDefault = 0.0)
{
  return IsZero(theDenominator) ? theDefault : theNumerator / theDenominator;
}

//! Sign function.
//! @param theValue input value
//! @return -1 if negative, 0 if zero, +1 if positive
inline int Sign(double theValue)
{
  if (theValue > THE_ZERO_TOL)
    return 1;
  if (theValue < -THE_ZERO_TOL)
    return -1;
  return 0;
}

//! Sign transfer function: returns |theA| with sign of theB.
//! Equivalent to copysign but avoids edge cases with zero.
//! @param theA value whose magnitude is used
//! @param theB value whose sign is used
//! @return |theA| * sign(theB)
inline double SignTransfer(double theA, double theB)
{
  return (theB >= 0.0) ? std::abs(theA) : -std::abs(theA);
}

//! Square of a value.
//! @param theValue input value
//! @return theValue * theValue
inline constexpr double Sqr(double theValue)
{
  return theValue * theValue;
}

//! Cube of a value.
//! @param theValue input value
//! @return theValue^3
inline constexpr double Cube(double theValue)
{
  return theValue * theValue * theValue;
}

//! Cube root with proper sign handling.
//! Unlike std::cbrt, this handles negative values correctly on all platforms.
//! @param theValue input value
//! @return cube root of theValue
inline double CubeRoot(double theValue)
{
  return (theValue >= 0.0) ? std::cbrt(theValue) : -std::cbrt(-theValue);
}

//! Check if value is finite (not NaN or Inf).
//! @param theValue value to check
//! @return true if finite
inline bool IsFinite(double theValue)
{
  return std::isfinite(theValue);
}

//! Compute scaling factor for coefficient normalization.
//! Used to improve numerical stability by scaling coefficients.
//! @param theCoeffs array of coefficients
//! @param theCount number of coefficients
//! @return scaling factor (power of 2 for exact arithmetic)
inline double ComputeScaleFactor(const double* theCoeffs, size_t theCount)
{
  if (theCoeffs == nullptr || theCount == 0)
  {
    return std::numeric_limits<double>::quiet_NaN();
  }
  double aMaxAbs = 0.0;
  for (size_t i = 0; i < theCount; ++i)
  {
    if (!std::isfinite(theCoeffs[i]))
    {
      return std::numeric_limits<double>::quiet_NaN();
    }
    aMaxAbs = std::max(aMaxAbs, std::abs(theCoeffs[i]));
  }

  if (aMaxAbs == 0.0)
  {
    return 1.0;
  }
  if (aMaxAbs < THE_ZERO_TOL || aMaxAbs > 1.0e15)
  {
    // Find nearest power of 2 for exact scaling
    int anExp = 0;
    std::frexp(aMaxAbs, &anExp);
    return std::ldexp(1.0, -anExp + 1);
  }

  return 1.0;
}

//! Compute dot product of two vectors.
//! @param theA first vector
//! @param theB second vector
//! @return dot product sum(A[i] * B[i])
inline double DotProduct(const math_Vector& theA, const math_Vector& theB)
{
  if (theA.Size() != theB.Size())
  {
    return std::numeric_limits<double>::quiet_NaN();
  }
  long double aSum        = 0.0L;
  long double aCorrection = 0.0L;
  for (size_t i = 0; i < theA.Size(); ++i)
  {
    const double anA = theA.At(i);
    const double aB  = theB.At(i);
    if (!std::isfinite(anA) || !std::isfinite(aB))
    {
      return std::numeric_limits<double>::quiet_NaN();
    }
    const long double aProduct = static_cast<long double>(anA) * static_cast<long double>(aB);
    const long double aNewSum  = aSum + aProduct;
    aCorrection += std::abs(aSum) >= std::abs(aProduct) ? (aSum - aNewSum) + aProduct
                                                        : (aProduct - aNewSum) + aSum;
    aSum = aNewSum;
  }
  return static_cast<double>(aSum + aCorrection);
}

//! Compute Euclidean norm of a vector.
//! @param theVec input vector
//! @return sqrt(sum(V[i]^2))
inline double VectorNorm(const math_Vector& theVec)
{
  double aScale = 0.0;
  double aSum   = 1.0;
  for (size_t i = 0; i < theVec.Size(); ++i)
  {
    const double aValue = std::abs(theVec.At(i));
    if (!std::isfinite(aValue))
    {
      return aValue;
    }
    if (aValue == 0.0)
    {
      continue;
    }
    if (aScale < aValue)
    {
      const double aRatio = aScale / aValue;
      aSum                = 1.0 + aSum * aRatio * aRatio;
      aScale              = aValue;
    }
    else
    {
      const double aRatio = aValue / aScale;
      aSum += aRatio * aRatio;
    }
  }
  return aScale == 0.0 ? 0.0 : aScale * std::sqrt(aSum);
}

//! Compute the Euclidean norm.
inline double Norm(const math_Vector& theVec)
{
  return VectorNorm(theVec);
}

//! Compute the squared Euclidean norm. Overflow is reported as infinity.
inline double Norm2(const math_Vector& theVec)
{
  const double aNorm = VectorNorm(theVec);
  return aNorm * aNorm;
}

//! Compute infinity norm (maximum absolute value) of a vector.
//! @param theVec input vector
//! @return max(|V[i]|)
inline double VectorInfNorm(const math_Vector& theVec)
{
  double aMax = 0.0;
  for (size_t i = 0; i < theVec.Size(); ++i)
  {
    aMax = std::max(aMax, std::abs(theVec.At(i)));
  }
  return aMax;
}

} // namespace MathUtils

#endif // _MathUtils_Core_HeaderFile

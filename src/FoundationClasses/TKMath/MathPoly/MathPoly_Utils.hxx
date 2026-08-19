// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathPoly_Utils_HeaderFile
#define _MathPoly_Utils_HeaderFile

#include "MathPoly_Types.hxx"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <limits>
#include <optional>

namespace MathPoly
{
namespace Utils
{

//! Residual threshold used to recognize distinct real roots at critical points.
inline constexpr long double THE_DISTINCT_ROOT_RESIDUAL_TOLERANCE =
  2048.0L * std::numeric_limits<long double>::epsilon();

//! Fixed bisection count providing full long-double refinement for degree <= 4.
inline constexpr uint32_t THE_DISTINCT_ROOT_BISECTION_ITERATIONS = 200;

//! Residual threshold used to identify repeated roots from successive derivatives.
inline constexpr long double THE_ROOT_MULTIPLICITY_TOLERANCE =
  32768.0L * std::numeric_limits<long double>::epsilon();

using LongCoefficients = std::array<long double, THE_MAX_POLY_DEGREE + 1>;

inline bool IsFinite(const std::complex<double>& theValue)
{
  return std::isfinite(theValue.real()) && std::isfinite(theValue.imag());
}

inline std::optional<double> ToDouble(long double theValue)
{
  if (!std::isfinite(theValue) || std::abs(theValue) > std::numeric_limits<double>::max())
  {
    return std::nullopt;
  }
  return static_cast<double>(theValue);
}

inline bool Validate(const double* theCoefficients, int theDegree, double theTolerance = 1.0)
{
  if (theCoefficients == nullptr || theDegree < 0 || theDegree > THE_MAX_POLY_DEGREE
      || !std::isfinite(theTolerance) || theTolerance <= 0.0)
  {
    return false;
  }
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    if (!std::isfinite(theCoefficients[anIndex]))
    {
      return false;
    }
  }
  return true;
}

//! Scales x = 2^theVariableExponent*y and normalizes coefficients by a common power of two.
inline bool ScalePolynomial(const double*    theCoefficients,
                            int              theDegree,
                            LongCoefficients& theScaled,
                            int&             theVariableExponent)
{
  if (theCoefficients[theDegree] == 0.0)
  {
    return false;
  }

  const int aLeadingExponent = std::ilogb(std::abs(theCoefficients[theDegree]));
  long double aScaleExponent = -1000.0L;
  for (int anIndex = 0; anIndex < theDegree; ++anIndex)
  {
    if (theCoefficients[anIndex] == 0.0)
    {
      continue;
    }
    const int anExponent = std::ilogb(std::abs(theCoefficients[anIndex]));
    aScaleExponent = std::max(aScaleExponent,
                              static_cast<long double>(anExponent - aLeadingExponent)
                                / static_cast<long double>(theDegree - anIndex));
  }
  theVariableExponent = std::max(-1000, std::min(1000, static_cast<int>(std::ceil(aScaleExponent))));

  int aCommonExponent = aLeadingExponent + theDegree * theVariableExponent;
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    if (theCoefficients[anIndex] != 0.0)
    {
      aCommonExponent = std::max(aCommonExponent,
                                 std::ilogb(std::abs(theCoefficients[anIndex]))
                                   + anIndex * theVariableExponent);
    }
  }
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    theScaled[static_cast<size_t>(anIndex)] =
      std::scalbn(static_cast<long double>(theCoefficients[anIndex]),
                  anIndex * theVariableExponent - aCommonExponent);
  }
  return true;
}

inline void Evaluate(const LongCoefficients& theCoefficients,
                     int                     theDegree,
                     long double             theX,
                     long double&            theValue,
                     long double&            theDerivative,
                     long double&            theMagnitude)
{
  theValue      = theCoefficients[static_cast<size_t>(theDegree)];
  theDerivative = 0.0L;
  theMagnitude  = std::abs(theCoefficients[static_cast<size_t>(theDegree)]);
  const long double anAbsX = std::abs(theX);
  for (int anIndex = theDegree - 1; anIndex >= 0; --anIndex)
  {
    theDerivative = theDerivative * theX + theValue;
    theValue = theValue * theX + theCoefficients[static_cast<size_t>(anIndex)];
    theMagnitude = theMagnitude * anAbsX
                   + std::abs(theCoefficients[static_cast<size_t>(anIndex)]);
  }
}

inline bool HasSmallResidual(const LongCoefficients& theCoefficients,
                             int                     theDegree,
                             long double             theRoot,
                             long double             theTolerance)
{
  long double aValue = 0.0L, aDerivative = 0.0L, aMagnitude = 0.0L;
  Evaluate(theCoefficients, theDegree, theRoot, aValue, aDerivative, aMagnitude);
  return std::isfinite(aValue) && std::abs(aValue) <= theTolerance * std::max(1.0L, aMagnitude);
}

inline long double RootBound(const LongCoefficients& theCoefficients, int theDegree)
{
  const long double aLeading = std::abs(theCoefficients[static_cast<size_t>(theDegree)]);
  long double       aRatio   = 0.0L;
  for (int anIndex = 0; anIndex < theDegree; ++anIndex)
  {
    aRatio = std::max(aRatio,
                      std::abs(theCoefficients[static_cast<size_t>(anIndex)]) / aLeading);
  }
  return 1.0L + aRatio;
}

inline void AppendUnique(std::array<long double, 4>& theRoots, size_t& theCount, long double theRoot)
{
  const long double aTolerance = 128.0L * std::numeric_limits<long double>::epsilon()
                                 * std::max(1.0L, std::abs(theRoot));
  for (size_t anIndex = 0; anIndex < theCount; ++anIndex)
  {
    if (std::abs(theRoots[anIndex] - theRoot) <= aTolerance)
    {
      return;
    }
  }
  theRoots[theCount++] = theRoot;
}

inline bool SolveDistinctReal(const LongCoefficients&     theCoefficients,
                              int                         theDegree,
                              std::array<long double, 4>& theRoots,
                              size_t&                     theCount)
{
  if (theDegree == 1)
  {
    const long double aRoot = -theCoefficients[0] / theCoefficients[1];
    if (!std::isfinite(aRoot))
    {
      return false;
    }
    AppendUnique(theRoots, theCount, aRoot);
    return true;
  }

  LongCoefficients aDerivative = {};
  for (int anIndex = 1; anIndex <= theDegree; ++anIndex)
  {
    aDerivative[static_cast<size_t>(anIndex - 1)] =
      static_cast<long double>(anIndex) * theCoefficients[static_cast<size_t>(anIndex)];
  }
  std::array<long double, 4> aCritical = {};
  size_t                     aCriticalCount = 0;
  if (!SolveDistinctReal(aDerivative, theDegree - 1, aCritical, aCriticalCount))
  {
    return false;
  }
  std::sort(aCritical.begin(), aCritical.begin() + aCriticalCount);

  std::array<long double, 6> aPoints = {};
  size_t                     aPointCount = 0;
  const long double          aBound = RootBound(theCoefficients, theDegree);
  aPoints[aPointCount++] = -aBound;
  for (size_t anIndex = 0; anIndex < aCriticalCount; ++anIndex)
  {
    if (anIndex == 0 || aCritical[anIndex] != aCritical[anIndex - 1])
    {
      aPoints[aPointCount++] = aCritical[anIndex];
    }
    if (HasSmallResidual(theCoefficients,
                         theDegree,
                         aCritical[anIndex],
                         THE_DISTINCT_ROOT_RESIDUAL_TOLERANCE))
    {
      AppendUnique(theRoots, theCount, aCritical[anIndex]);
    }
  }
  aPoints[aPointCount++] = aBound;

  for (size_t anIndex = 1; anIndex < aPointCount; ++anIndex)
  {
    long double aLeft = aPoints[anIndex - 1], aRight = aPoints[anIndex];
    long double aLeftValue = 0.0L, aDummy = 0.0L, aMagnitude = 0.0L;
    long double aRightValue = 0.0L;
    Evaluate(theCoefficients, theDegree, aLeft, aLeftValue, aDummy, aMagnitude);
    Evaluate(theCoefficients, theDegree, aRight, aRightValue, aDummy, aMagnitude);
    if ((anIndex > 1
          && HasSmallResidual(theCoefficients,
                              theDegree,
                              aLeft,
                              THE_DISTINCT_ROOT_RESIDUAL_TOLERANCE))
        || (anIndex + 1 < aPointCount
            && HasSmallResidual(theCoefficients,
                                theDegree,
                                aRight,
                                THE_DISTINCT_ROOT_RESIDUAL_TOLERANCE)))
    {
      continue;
    }
    if ((aLeftValue < 0.0L) == (aRightValue < 0.0L))
    {
      continue;
    }
    for (uint32_t anIteration = 0;
         anIteration < THE_DISTINCT_ROOT_BISECTION_ITERATIONS;
         ++anIteration)
    {
      const long double aMiddle = (aLeft + aRight) / 2.0L;
      long double       aMiddleValue = 0.0L;
      Evaluate(theCoefficients, theDegree, aMiddle, aMiddleValue, aDummy, aMagnitude);
      if ((aLeftValue < 0.0L) == (aMiddleValue < 0.0L))
      {
        aLeft = aMiddle;
        aLeftValue = aMiddleValue;
      }
      else
      {
        aRight = aMiddle;
      }
    }
    AppendUnique(theRoots, theCount, (aLeft + aRight) / 2.0L);
  }
  return true;
}

inline PolyResult SolveDirect(const double* theCoefficients, int theDegree)
{
  PolyResult aResult;
  if (!Validate(theCoefficients, theDegree))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  while (theDegree > 0 && theCoefficients[theDegree] == 0.0)
  {
    --theDegree;
  }
  if (theDegree == 0)
  {
    aResult.Status = theCoefficients[0] == 0.0 ? MathUtils::Status::InfiniteSolutions
                                                : MathUtils::Status::NoSolution;
    return aResult;
  }

  if (theDegree == 1)
  {
    const std::optional<double> aRoot =
      ToDouble(-static_cast<long double>(theCoefficients[0])
               / static_cast<long double>(theCoefficients[1]));
    if (!aRoot.has_value())
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    aResult.Roots[0]          = *aRoot;
    aResult.Multiplicities[0] = 1;
    aResult.NbRoots           = 1;
    aResult.Status            = MathUtils::Status::OK;
    return aResult;
  }

  if (theDegree == 2)
  {
    const double aCoefficientScale =
      std::max({std::abs(theCoefficients[0]),
                std::abs(theCoefficients[1]),
                std::abs(theCoefficients[2])});
    const long double aA = theCoefficients[2] / aCoefficientScale;
    const long double aB = theCoefficients[1] / aCoefficientScale;
    const long double aC = theCoefficients[0] / aCoefficientScale;
    const long double aDiscriminant = aB * aB - 4.0L * aA * aC;
    const long double aDiscriminantScale = aB * aB + std::abs(4.0L * aA * aC);
    const long double aTolerance = 64.0L * std::numeric_limits<long double>::epsilon()
                                   * aDiscriminantScale;
    aResult.Status = MathUtils::Status::OK;
    if (aDiscriminant < -aTolerance)
    {
      return aResult;
    }
    if (std::abs(aDiscriminant) <= aTolerance)
    {
      const std::optional<double> aRoot = ToDouble(-aB / (2.0L * aA));
      if (!aRoot.has_value())
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      aResult.Roots[0] = *aRoot;
      aResult.Multiplicities[0] = 2;
      aResult.NbRoots = 1;
      return aResult;
    }
    const long double aSquareRoot = std::sqrt(aDiscriminant);
    const long double aQ = -0.5L * (aB + std::copysign(aSquareRoot, aB));
    const long double aRoots[2] = {aQ / aA, aC / aQ};
    for (const long double aLongRoot : aRoots)
    {
      const std::optional<double> aRoot = ToDouble(aLongRoot);
      if (!aRoot.has_value())
      {
        aResult.Status = MathUtils::Status::NumericalError;
        aResult.NbRoots = 0;
        return aResult;
      }
      aResult.Roots[aResult.NbRoots] = *aRoot;
      aResult.Multiplicities[aResult.NbRoots++] = 1;
    }
    if (aResult.Roots[0] > aResult.Roots[1])
    {
      std::swap(aResult.Roots[0], aResult.Roots[1]);
    }
    return aResult;
  }

  LongCoefficients aScaled = {};
  int              aVariableExponent = 0;
  ScalePolynomial(theCoefficients, theDegree, aScaled, aVariableExponent);
  std::array<long double, 4> aRoots = {};
  size_t                     aRootCount = 0;
  if (!SolveDistinctReal(aScaled, theDegree, aRoots, aRootCount))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  std::sort(aRoots.begin(), aRoots.begin() + aRootCount);

  for (size_t aRootIndex = 0; aRootIndex < aRootCount; ++aRootIndex)
  {
    LongCoefficients aDerivative = aScaled;
    int              aDerivativeDegree = theDegree;
    size_t           aMultiplicity = 1;
    while (aMultiplicity < static_cast<size_t>(theDegree) && aDerivativeDegree > 0)
    {
      LongCoefficients aNext = {};
      for (int anIndex = 1; anIndex <= aDerivativeDegree; ++anIndex)
      {
        aNext[static_cast<size_t>(anIndex - 1)] =
          static_cast<long double>(anIndex) * aDerivative[static_cast<size_t>(anIndex)];
      }
      --aDerivativeDegree;
      if (!HasSmallResidual(aNext,
                            aDerivativeDegree,
                            aRoots[aRootIndex],
                             THE_ROOT_MULTIPLICITY_TOLERANCE))
      {
        break;
      }
      ++aMultiplicity;
      aDerivative = aNext;
    }
    const std::optional<double> aRoot =
      ToDouble(std::scalbn(aRoots[aRootIndex], aVariableExponent));
    if (!HasSmallResidual(aScaled,
                          theDegree,
                          aRoots[aRootIndex],
                          4096.0L * std::numeric_limits<long double>::epsilon())
        || !aRoot.has_value())
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    aResult.Roots[aResult.NbRoots] = *aRoot;
    aResult.Multiplicities[aResult.NbRoots] = aMultiplicity;
    ++aResult.NbRoots;
  }
  aResult.Status = MathUtils::Status::OK;
  return aResult;
}

} // namespace Utils
} // namespace MathPoly

#endif // _MathPoly_Utils_HeaderFile

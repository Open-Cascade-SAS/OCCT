// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathPoly_Laguerre_HeaderFile
#define _MathPoly_Laguerre_HeaderFile

#include "MathPoly_Quartic.hxx"
#include <MathUtils_Core.hxx>

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

//! Number of deterministic complex starts used to recover a Laguerre root.
inline constexpr size_t THE_LAGUERRE_NB_STARTS = 16;

struct IterationResult
{
  MathUtils::Status     Status = MathUtils::Status::NotConverged;
  std::complex<double>  Root   = {};
  std::optional<double> RootError;
  uint32_t              NbIterations = 0;
};

inline void EvaluateComplex(const double*         theCoefficients,
                            int                   theDegree,
                            std::complex<double>  theX,
                            std::complex<double>& theValue,
                            std::complex<double>& theDerivative,
                            std::complex<double>& theSecondDerivative,
                            double&               theMagnitude)
{
  theValue            = theCoefficients[theDegree];
  theDerivative       = 0.0;
  theSecondDerivative = 0.0;
  theMagnitude        = std::abs(theCoefficients[theDegree]);
  const double anAbsX = std::abs(theX);
  for (int anIndex = theDegree - 1; anIndex >= 0; --anIndex)
  {
    theSecondDerivative = theSecondDerivative * theX + 2.0 * theDerivative;
    theDerivative       = theDerivative * theX + theValue;
    theValue            = theValue * theX + theCoefficients[anIndex];
    theMagnitude        = theMagnitude * anAbsX + std::abs(theCoefficients[anIndex]);
  }
}

inline bool HasSmallResidual(const double*               theCoefficients,
                             int                         theDegree,
                             const std::complex<double>& theRoot,
                             double                      theTolerance)
{
  std::complex<double> aValue, aDerivative, aSecondDerivative;
  double               aMagnitude = 0.0;
  EvaluateComplex(theCoefficients,
                  theDegree,
                  theRoot,
                  aValue,
                  aDerivative,
                  aSecondDerivative,
                  aMagnitude);
  return IsFinite(aValue) && std::abs(aValue) <= theTolerance * std::max(1.0, aMagnitude);
}

inline IterationResult LaguerreIteration(const double*        theCoefficients,
                                         int                  theDegree,
                                         std::complex<double> theStart,
                                         double               theTolerance,
                                         uint32_t             theMaxIterations)
{
  IterationResult      aResult;
  std::complex<double> aRoot = theStart;
  for (uint32_t anIteration = 0; anIteration < theMaxIterations; ++anIteration)
  {
    std::complex<double> aValue, aDerivative, aSecondDerivative;
    double               aMagnitude = 0.0;
    EvaluateComplex(theCoefficients,
                    theDegree,
                    aRoot,
                    aValue,
                    aDerivative,
                    aSecondDerivative,
                    aMagnitude);
    if (!IsFinite(aValue) || !IsFinite(aDerivative) || !IsFinite(aSecondDerivative)
        || !std::isfinite(aMagnitude))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    if (aValue == std::complex<double>(0.0, 0.0))
    {
      aResult.Status    = MathUtils::Status::OK;
      aResult.Root      = aRoot;
      aResult.RootError = std::numeric_limits<double>::epsilon() * std::max(1.0, std::abs(aRoot));
      aResult.NbIterations = anIteration;
      return aResult;
    }

    const std::complex<double> aG           = aDerivative / aValue;
    const std::complex<double> aH           = aG * aG - aSecondDerivative / aValue;
    const double               aDegree      = static_cast<double>(theDegree);
    const std::complex<double> aSquareRoot  = std::sqrt((aDegree - 1.0) * (aDegree * aH - aG * aG));
    const std::complex<double> aPlus        = aG + aSquareRoot;
    const std::complex<double> aMinus       = aG - aSquareRoot;
    const std::complex<double> aDenominator = std::abs(aPlus) >= std::abs(aMinus) ? aPlus : aMinus;
    if (!IsFinite(aDenominator) || std::abs(aDenominator) == 0.0)
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    const std::complex<double> aStep = aDegree / aDenominator;
    const std::complex<double> aNext = aRoot - aStep;
    if (!IsFinite(aStep) || !IsFinite(aNext))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    const double aRootScale = std::max(1.0, std::abs(aRoot));
    if (std::abs(aValue) <= theTolerance * std::max(1.0, aMagnitude)
        && std::abs(aStep) <= theTolerance * aRootScale)
    {
      aResult.Status       = MathUtils::Status::OK;
      aResult.Root         = aRoot;
      aResult.RootError    = std::abs(aStep) + std::numeric_limits<double>::epsilon() * aRootScale;
      aResult.NbIterations = anIteration;
      return aResult;
    }
    aRoot = aNext;
  }
  aResult.Status       = MathUtils::Status::MaxIterations;
  aResult.Root         = aRoot;
  aResult.NbIterations = theMaxIterations;
  return aResult;
}

inline IterationResult FindRoot(const double* theCoefficients,
                                int           theDegree,
                                double        theTolerance,
                                uint32_t      theMaxIterations)
{
  for (size_t aStartIndex = 0; aStartIndex < THE_LAGUERRE_NB_STARTS; ++aStartIndex)
  {
    const double aRadius = aStartIndex == 0 ? 0.0 : (0.25 + 1.75 * (aStartIndex % 4) / 3.0);
    const double anAngle = MathUtils::THE_2PI * static_cast<double>(aStartIndex)
                           / THE_LAGUERRE_NB_STARTS;
    const std::complex<double> aStart = std::polar(aRadius, anAngle);
    IterationResult            aResult =
      LaguerreIteration(theCoefficients, theDegree, aStart, theTolerance, theMaxIterations);
    if (aResult.Status == MathUtils::Status::OK
        && HasSmallResidual(theCoefficients, theDegree, aResult.Root, theTolerance))
    {
      return aResult;
    }
  }
  IterationResult aResult;
  aResult.Status = MathUtils::Status::MaxIterations;
  return aResult;
}

inline bool DeflateReal(double* theCoefficients,
                        int&    theDegree,
                        double  theRoot,
                        double  theTolerance)
{
  std::array<double, THE_MAX_POLY_DEGREE + 1> aQuotient = {};
  aQuotient[static_cast<size_t>(theDegree - 1)]         = theCoefficients[theDegree];
  for (int anIndex = theDegree - 2; anIndex >= 0; --anIndex)
  {
    aQuotient[static_cast<size_t>(anIndex)] =
      theCoefficients[anIndex + 1] + theRoot * aQuotient[static_cast<size_t>(anIndex + 1)];
  }
  const double aRemainder = theCoefficients[0] + theRoot * aQuotient[0];
  double       aMagnitude = std::abs(theCoefficients[theDegree]);
  for (int anIndex = theDegree - 1; anIndex >= 0; --anIndex)
  {
    aMagnitude = aMagnitude * std::abs(theRoot) + std::abs(theCoefficients[anIndex]);
  }
  if (!std::isfinite(aRemainder)
      || std::abs(aRemainder) > 32.0 * theTolerance * std::max(1.0, aMagnitude))
  {
    return false;
  }
  for (int anIndex = 0; anIndex < theDegree; ++anIndex)
  {
    theCoefficients[anIndex] = aQuotient[static_cast<size_t>(anIndex)];
  }
  --theDegree;
  return true;
}

inline bool DeflateComplex(double*                     theCoefficients,
                           int&                        theDegree,
                           const std::complex<double>& theRoot,
                           double                      theTolerance)
{
  const double aB = -2.0 * theRoot.real();
  const double aC = std::norm(theRoot);
  if (!std::isfinite(aB) || !std::isfinite(aC))
  {
    return false;
  }
  std::array<double, THE_MAX_POLY_DEGREE + 1> aQuotient = {};
  aQuotient[static_cast<size_t>(theDegree - 2)]         = theCoefficients[theDegree];
  if (theDegree > 2)
  {
    aQuotient[static_cast<size_t>(theDegree - 3)] =
      theCoefficients[theDegree - 1] - aB * aQuotient[static_cast<size_t>(theDegree - 2)];
  }
  for (int anIndex = theDegree - 4; anIndex >= 0; --anIndex)
  {
    aQuotient[static_cast<size_t>(anIndex)] = theCoefficients[anIndex + 2]
                                              - aB * aQuotient[static_cast<size_t>(anIndex + 1)]
                                              - aC * aQuotient[static_cast<size_t>(anIndex + 2)];
  }
  const double aRemainder1 =
    theCoefficients[1] - aB * aQuotient[0] - (theDegree > 2 ? aC * aQuotient[1] : 0.0);
  const double aRemainder0 = theCoefficients[0] - aC * aQuotient[0];
  double       aNorm       = 0.0;
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aNorm = std::max(aNorm, std::abs(theCoefficients[anIndex]));
  }
  if (!std::isfinite(aRemainder0) || !std::isfinite(aRemainder1)
      || std::max(std::abs(aRemainder0), std::abs(aRemainder1))
           > 64.0 * theTolerance * std::max(1.0, aNorm))
  {
    return false;
  }
  for (int anIndex = 0; anIndex <= theDegree - 2; ++anIndex)
  {
    theCoefficients[anIndex] = aQuotient[static_cast<size_t>(anIndex)];
  }
  theDegree -= 2;
  return true;
}

inline bool HasMultiplicity(const double* theCoefficients,
                            int           theDegree,
                            double&       theRoot,
                            size_t        theMultiplicity)
{
  std::array<long double, THE_MAX_POLY_DEGREE + 1> aDerivative = {};
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aDerivative[static_cast<size_t>(anIndex)] = theCoefficients[anIndex];
  }
  std::array<long double, THE_MAX_POLY_DEGREE + 1> aMultiplicityDerivative = aDerivative;
  int                                              aMultiplicityDegree     = theDegree;
  for (size_t anOrder = 1; anOrder < theMultiplicity; ++anOrder)
  {
    for (int anIndex = 1; anIndex <= aMultiplicityDegree; ++anIndex)
    {
      aMultiplicityDerivative[static_cast<size_t>(anIndex - 1)] =
        static_cast<long double>(anIndex) * aMultiplicityDerivative[static_cast<size_t>(anIndex)];
    }
    --aMultiplicityDegree;
  }

  long double aRoot = theRoot;
  for (int anIteration = 0; anIteration < theDegree; ++anIteration)
  {
    long double aValue = aMultiplicityDerivative[static_cast<size_t>(aMultiplicityDegree)];
    long double aDerivativeValue = 0.0L;
    for (int anIndex = aMultiplicityDegree - 1; anIndex >= 0; --anIndex)
    {
      aDerivativeValue = aDerivativeValue * aRoot + aValue;
      aValue           = aValue * aRoot + aMultiplicityDerivative[static_cast<size_t>(anIndex)];
    }
    if (!std::isfinite(aValue) || !std::isfinite(aDerivativeValue) || aDerivativeValue == 0.0L)
    {
      return false;
    }
    const long double aRefinedRoot = aRoot - aValue / aDerivativeValue;
    if (!std::isfinite(aRefinedRoot))
    {
      return false;
    }
    if (aRefinedRoot == aRoot)
    {
      break;
    }
    aRoot = aRefinedRoot;
  }

  int aDegree = theDegree;
  for (size_t anOrder = 0; anOrder < theMultiplicity; ++anOrder)
  {
    long double aValue = aDerivative[static_cast<size_t>(aDegree)];
    long double aNorm  = std::abs(aValue);
    for (int anIndex = aDegree - 1; anIndex >= 0; --anIndex)
    {
      aValue = aValue * aRoot + aDerivative[static_cast<size_t>(anIndex)];
      aNorm += std::abs(aDerivative[static_cast<size_t>(anIndex)]);
    }
    if (!std::isfinite(aValue)
        || std::abs(aValue) > std::numeric_limits<double>::epsilon() * std::max(1.0L, aNorm))
    {
      return false;
    }
    for (int anIndex = 1; anIndex <= aDegree; ++anIndex)
    {
      aDerivative[static_cast<size_t>(anIndex - 1)] =
        static_cast<long double>(anIndex) * aDerivative[static_cast<size_t>(anIndex)];
    }
    --aDegree;
  }
  theRoot = static_cast<double>(aRoot);
  return true;
}

inline double MultipleRootUncertainty(const double* theCoefficients,
                                      int           theDegree,
                                      double        theRoot,
                                      size_t        theMultiplicity)
{
  std::array<long double, THE_MAX_POLY_DEGREE + 1> aDerivative = {};
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aDerivative[static_cast<size_t>(anIndex)] = theCoefficients[anIndex];
  }

  const long double anAbsRoot  = std::abs(static_cast<long double>(theRoot));
  long double       aMagnitude = std::abs(aDerivative[static_cast<size_t>(theDegree)]);
  for (int anIndex = theDegree - 1; anIndex >= 0; --anIndex)
  {
    aMagnitude = aMagnitude * anAbsRoot + std::abs(aDerivative[static_cast<size_t>(anIndex)]);
  }

  int         aDerivativeDegree = theDegree;
  long double aFactorial        = 1.0L;
  for (size_t anOrder = 0; anOrder < theMultiplicity; ++anOrder)
  {
    for (int anIndex = 1; anIndex <= aDerivativeDegree; ++anIndex)
    {
      aDerivative[static_cast<size_t>(anIndex - 1)] =
        static_cast<long double>(anIndex) * aDerivative[static_cast<size_t>(anIndex)];
    }
    --aDerivativeDegree;
    aFactorial *= static_cast<long double>(anOrder + 1);
  }

  long double aTaylorCoefficient = aDerivative[static_cast<size_t>(aDerivativeDegree)];
  for (int anIndex = aDerivativeDegree - 1; anIndex >= 0; --anIndex)
  {
    aTaylorCoefficient = aTaylorCoefficient * theRoot + aDerivative[static_cast<size_t>(anIndex)];
  }
  aTaylorCoefficient /= aFactorial;
  if (!std::isfinite(aTaylorCoefficient) || aTaylorCoefficient == 0.0L)
  {
    return 0.0;
  }

  const long double aCoefficientRoundoff = std::numeric_limits<double>::epsilon() * aMagnitude;
  const long double anUncertainty = std::pow(aCoefficientRoundoff / std::abs(aTaylorCoefficient),
                                             1.0L / static_cast<long double>(theMultiplicity));
  return static_cast<double>(anUncertainty)
         + std::numeric_limits<double>::epsilon() * std::max(1.0, std::abs(theRoot));
}

inline void ConsolidateRealRoots(PolyResult&   theResult,
                                 double        theTolerance,
                                 const double* theOriginal,
                                 int           theDegree,
                                 int           theVariableExponent)
{
  std::sort(theResult.Roots.begin(), theResult.Roots.begin() + theResult.NbRoots);
  const double aMergeTolerance = std::pow(theTolerance, 1.0 / theDegree);
  const size_t aNbRoots        = theResult.NbRoots;
  size_t       aSource         = 0;
  size_t       aCount          = 0;
  while (aSource < aNbRoots)
  {
    size_t aClusterEnd = aSource + 1;
    while (aClusterEnd < aNbRoots
           && std::abs(std::scalbn(theResult.Roots[aClusterEnd], -theVariableExponent)
                       - std::scalbn(theResult.Roots[aClusterEnd - 1], -theVariableExponent))
                <= aMergeTolerance)
    {
      ++aClusterEnd;
    }

    size_t aMultiplicity = aClusterEnd - aSource;
    double aCandidate    = theResult.Roots[aSource];
    while (aMultiplicity > 1)
    {
      long double aSum = 0.0L;
      for (size_t anIndex = 0; anIndex < aMultiplicity; ++anIndex)
      {
        aSum += theResult.Roots[aSource + anIndex];
      }
      aCandidate              = static_cast<double>(aSum / aMultiplicity);
      double aScaledCandidate = std::scalbn(aCandidate, -theVariableExponent);
      if (HasMultiplicity(theOriginal, theDegree, aScaledCandidate, aMultiplicity))
      {
        aCandidate = std::scalbn(aScaledCandidate, theVariableExponent);
        break;
      }
      --aMultiplicity;
    }
    if (aMultiplicity == 1)
    {
      aCandidate = theResult.Roots[aSource];
    }

    theResult.Roots[aCount]          = aCandidate;
    theResult.Multiplicities[aCount] = aMultiplicity;
    ++aCount;
    aSource += aMultiplicity;
  }
  theResult.NbRoots = aCount;
}

inline bool HasComplexMultiplicity(const double*         theCoefficients,
                                   int                   theDegree,
                                   std::complex<double>& theRoot,
                                   size_t                theMultiplicity)
{
  using LongComplex                                            = std::complex<long double>;
  std::array<long double, THE_MAX_POLY_DEGREE + 1> aDerivative = {};
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aDerivative[static_cast<size_t>(anIndex)] = theCoefficients[anIndex];
  }
  std::array<long double, THE_MAX_POLY_DEGREE + 1> aMultiplicityDerivative = aDerivative;
  int                                              aMultiplicityDegree     = theDegree;
  for (size_t anOrder = 1; anOrder < theMultiplicity; ++anOrder)
  {
    for (int anIndex = 1; anIndex <= aMultiplicityDegree; ++anIndex)
    {
      aMultiplicityDerivative[static_cast<size_t>(anIndex - 1)] =
        static_cast<long double>(anIndex) * aMultiplicityDerivative[static_cast<size_t>(anIndex)];
    }
    --aMultiplicityDegree;
  }

  LongComplex aRoot(theRoot.real(), theRoot.imag());
  for (int anIteration = 0; anIteration < theDegree; ++anIteration)
  {
    LongComplex aValue = aMultiplicityDerivative[static_cast<size_t>(aMultiplicityDegree)];
    LongComplex aDerivativeValue = 0.0L;
    for (int anIndex = aMultiplicityDegree - 1; anIndex >= 0; --anIndex)
    {
      aDerivativeValue = aDerivativeValue * aRoot + aValue;
      aValue           = aValue * aRoot + aMultiplicityDerivative[static_cast<size_t>(anIndex)];
    }
    if (!std::isfinite(aValue.real()) || !std::isfinite(aValue.imag())
        || !std::isfinite(aDerivativeValue.real()) || !std::isfinite(aDerivativeValue.imag())
        || std::abs(aDerivativeValue) == 0.0L)
    {
      return false;
    }
    const LongComplex aRefinedRoot = aRoot - aValue / aDerivativeValue;
    if (!std::isfinite(aRefinedRoot.real()) || !std::isfinite(aRefinedRoot.imag()))
    {
      return false;
    }
    if (aRefinedRoot == aRoot)
    {
      break;
    }
    aRoot = aRefinedRoot;
  }

  int aDegree = theDegree;
  for (size_t anOrder = 0; anOrder < theMultiplicity; ++anOrder)
  {
    LongComplex       aValue     = aDerivative[static_cast<size_t>(aDegree)];
    long double       aMagnitude = std::abs(aValue);
    const long double anAbsRoot  = std::abs(aRoot);
    for (int anIndex = aDegree - 1; anIndex >= 0; --anIndex)
    {
      aValue     = aValue * aRoot + aDerivative[static_cast<size_t>(anIndex)];
      aMagnitude = aMagnitude * anAbsRoot + std::abs(aDerivative[static_cast<size_t>(anIndex)]);
    }
    if (!std::isfinite(aValue.real()) || !std::isfinite(aValue.imag())
        || std::abs(aValue) > std::numeric_limits<double>::epsilon() * std::max(1.0L, aMagnitude))
    {
      return false;
    }
    for (int anIndex = 1; anIndex <= aDegree; ++anIndex)
    {
      aDerivative[static_cast<size_t>(anIndex - 1)] =
        static_cast<long double>(anIndex) * aDerivative[static_cast<size_t>(anIndex)];
    }
    --aDegree;
  }
  theRoot =
    std::complex<double>(static_cast<double>(aRoot.real()), static_cast<double>(aRoot.imag()));
  return true;
}

inline void ConsolidateComplexRoots(PolyResult&   theResult,
                                    double        theTolerance,
                                    const double* theOriginal,
                                    int           theDegree,
                                    int           theVariableExponent)
{
  std::array<std::complex<double>, THE_MAX_POLY_DEGREE / 2> aPositiveRoots   = {};
  size_t                                                    aNbPositiveRoots = 0;
  for (size_t anIndex = 0; anIndex < theResult.NbComplexRoots; ++anIndex)
  {
    if (theResult.ComplexRoots[anIndex].imag() > 0.0)
    {
      aPositiveRoots[aNbPositiveRoots++] = theResult.ComplexRoots[anIndex];
    }
  }
  std::sort(aPositiveRoots.begin(),
            aPositiveRoots.begin() + aNbPositiveRoots,
            [](const std::complex<double>& theLeft, const std::complex<double>& theRight) {
              return theLeft.real() < theRight.real()
                     || (theLeft.real() == theRight.real() && theLeft.imag() < theRight.imag());
            });

  const double aMultipleRootResolution = std::pow(theTolerance, 1.0 / theDegree);
  size_t       aSource                 = 0;
  size_t       aCount                  = 0;
  while (aSource < aNbPositiveRoots)
  {
    size_t aClusterEnd = aSource + 1;
    while (aClusterEnd < aNbPositiveRoots
           && std::abs(aPositiveRoots[aClusterEnd] - aPositiveRoots[aClusterEnd - 1])
                <= aMultipleRootResolution
                     * std::max({1.0,
                                 std::abs(aPositiveRoots[aClusterEnd]),
                                 std::abs(aPositiveRoots[aClusterEnd - 1])}))
    {
      ++aClusterEnd;
    }

    size_t               aMultiplicity = aClusterEnd - aSource;
    std::complex<double> aCandidate    = aPositiveRoots[aSource];
    while (aMultiplicity > 1)
    {
      std::complex<long double> aSum = 0.0L;
      for (size_t anIndex = 0; anIndex < aMultiplicity; ++anIndex)
      {
        aSum += std::complex<long double>(aPositiveRoots[aSource + anIndex].real(),
                                          aPositiveRoots[aSource + anIndex].imag());
      }
      const std::complex<long double> anAverage = aSum / static_cast<long double>(aMultiplicity);
      aCandidate = std::complex<double>(static_cast<double>(anAverage.real()),
                                        static_cast<double>(anAverage.imag()));
      std::complex<double> aScaledCandidate = aCandidate * std::scalbn(1.0, -theVariableExponent);
      if (HasComplexMultiplicity(theOriginal, theDegree, aScaledCandidate, aMultiplicity))
      {
        aCandidate = aScaledCandidate * std::scalbn(1.0, theVariableExponent);
        break;
      }
      --aMultiplicity;
    }
    if (aMultiplicity == 1)
    {
      aCandidate = aPositiveRoots[aSource];
    }

    theResult.ComplexRoots[aCount]            = aCandidate;
    theResult.ComplexMultiplicities[aCount++] = aMultiplicity;
    theResult.ComplexRoots[aCount]            = std::conj(aCandidate);
    theResult.ComplexMultiplicities[aCount++] = aMultiplicity;
    aSource += aMultiplicity;
  }
  theResult.NbComplexRoots = aCount;
}

} // namespace Utils

//! Solves a real-coefficient polynomial using checked Laguerre iteration and deflation.
//! @param theCoefficients coefficients in ascending order
//! @param theDegree polynomial degree
//! @param theTolerance positive relative residual tolerance
//! @param theMaxIterations maximum iterations for each start
inline PolyResult Laguerre(const double* theCoefficients,
                           int           theDegree,
                           double        theTolerance     = 1.0e-12,
                           uint32_t      theMaxIterations = 100)
{
  PolyResult aResult;
  if (!Utils::Validate(theCoefficients, theDegree, theTolerance) || theDegree < 1
      || theCoefficients[theDegree] == 0.0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  Utils::LongCoefficients aLongScaled       = {};
  int                     aVariableExponent = 0;
  Utils::ScalePolynomial(theCoefficients, theDegree, aLongScaled, aVariableExponent);
  std::array<double, THE_MAX_POLY_DEGREE + 1> aWork      = {};
  std::array<double, THE_MAX_POLY_DEGREE + 1> anOriginal = {};
  for (int anIndex = 0; anIndex <= theDegree; ++anIndex)
  {
    aWork[static_cast<size_t>(anIndex)] =
      static_cast<double>(aLongScaled[static_cast<size_t>(anIndex)]);
    anOriginal[static_cast<size_t>(anIndex)] = aWork[static_cast<size_t>(anIndex)];
  }

  int aDegree = theDegree;
  while (aDegree > 0)
  {
    Utils::IterationResult anIteration =
      Utils::FindRoot(aWork.data(), aDegree, theTolerance, theMaxIterations);
    if (anIteration.Status != MathUtils::Status::OK || !Utils::IsFinite(anIteration.Root))
    {
      aResult.Status = anIteration.Status;
      return aResult;
    }
    const double anImaginaryUncertainty =
      *anIteration.RootError
      + std::numeric_limits<double>::epsilon() * std::max(1.0, std::abs(anIteration.Root));
    if (std::abs(anIteration.Root.imag()) <= anImaginaryUncertainty)
    {
      const double aRoot = anIteration.Root.real();
      if (!Utils::HasSmallResidual(aWork.data(), aDegree, aRoot, theTolerance)
          || !Utils::DeflateReal(aWork.data(), aDegree, aRoot, theTolerance))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      const std::optional<double> anUnscaled =
        Utils::ToDouble(std::scalbn(static_cast<long double>(aRoot), aVariableExponent));
      if (!anUnscaled.has_value())
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      aResult.Roots[aResult.NbRoots]          = *anUnscaled;
      aResult.Multiplicities[aResult.NbRoots] = 1;
      ++aResult.NbRoots;
    }
    else
    {
      if (aDegree < 2
          || !Utils::HasSmallResidual(aWork.data(), aDegree, anIteration.Root, theTolerance))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      double     aRepeatedRealRoot = anIteration.Root.real();
      const bool isRepeatedReal =
        Utils::HasMultiplicity(anOriginal.data(), theDegree, aRepeatedRealRoot, 2)
        && std::abs(anIteration.Root.imag())
             <= Utils::MultipleRootUncertainty(anOriginal.data(), theDegree, aRepeatedRealRoot, 2);
      if (isRepeatedReal)
      {
        if (!Utils::DeflateReal(aWork.data(), aDegree, aRepeatedRealRoot, theTolerance)
            || !Utils::DeflateReal(aWork.data(), aDegree, aRepeatedRealRoot, theTolerance))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        const double anUnscaledRealRoot = std::scalbn(aRepeatedRealRoot, aVariableExponent);
        aResult.Roots[aResult.NbRoots]  = anUnscaledRealRoot;
        aResult.Multiplicities[aResult.NbRoots++] = 1;
        aResult.Roots[aResult.NbRoots]            = anUnscaledRealRoot;
        aResult.Multiplicities[aResult.NbRoots++] = 1;
      }
      else
      {
        if (!Utils::DeflateComplex(aWork.data(), aDegree, anIteration.Root, theTolerance))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        const std::complex<double> anUnscaled =
          anIteration.Root * std::scalbn(1.0, aVariableExponent);
        if (!Utils::IsFinite(anUnscaled))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        const std::complex<double> aPositiveRoot =
          anUnscaled.imag() > 0.0 ? anUnscaled : std::conj(anUnscaled);
        aResult.ComplexRoots[aResult.NbComplexRoots]            = aPositiveRoot;
        aResult.ComplexMultiplicities[aResult.NbComplexRoots++] = 1;
        aResult.ComplexRoots[aResult.NbComplexRoots]            = std::conj(aPositiveRoot);
        aResult.ComplexMultiplicities[aResult.NbComplexRoots++] = 1;
      }
    }
  }

  Utils::ConsolidateRealRoots(aResult,
                              theTolerance,
                              anOriginal.data(),
                              theDegree,
                              aVariableExponent);
  Utils::ConsolidateComplexRoots(aResult,
                                 theTolerance,
                                 anOriginal.data(),
                                 theDegree,
                                 aVariableExponent);
  for (size_t aRootIndex = 0; aRootIndex < aResult.NbRoots; ++aRootIndex)
  {
    if (aResult.Multiplicities[aRootIndex] > 1)
    {
      continue;
    }
    long double aRoot =
      std::scalbn(static_cast<long double>(aResult.Roots[aRootIndex]), -aVariableExponent);
    for (uint32_t anIteration = 0; anIteration < 12; ++anIteration)
    {
      long double aValue = 0.0L, aDerivative = 0.0L, aMagnitude = 0.0L;
      Utils::Evaluate(aLongScaled, theDegree, aRoot, aValue, aDerivative, aMagnitude);
      if (aDerivative == 0.0L || !std::isfinite(aDerivative))
      {
        break;
      }
      const long double aStep = aValue / aDerivative;
      if (!std::isfinite(aStep))
      {
        break;
      }
      aRoot -= aStep;
      if (std::abs(aStep)
          <= std::numeric_limits<long double>::epsilon() * std::max(1.0L, std::abs(aRoot)))
      {
        break;
      }
    }
    aResult.Roots[aRootIndex] = static_cast<double>(std::scalbn(aRoot, aVariableExponent));
  }

  for (size_t aRootIndex = 0; aRootIndex < aResult.NbRoots; ++aRootIndex)
  {
    const long double aScaledRoot =
      std::scalbn(static_cast<long double>(aResult.Roots[aRootIndex]), -aVariableExponent);
    if (!Utils::HasSmallResidual(aLongScaled, theDegree, aScaledRoot, 128.0L * theTolerance))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
  }
  for (size_t aRootIndex = 0; aRootIndex < aResult.NbComplexRoots; ++aRootIndex)
  {
    const std::complex<double> aScaledRoot =
      aResult.ComplexRoots[aRootIndex] * std::scalbn(1.0, -aVariableExponent);
    if (!Utils::HasSmallResidual(anOriginal.data(), theDegree, aScaledRoot, 128.0 * theTolerance))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
  }

  aResult.Status = MathUtils::Status::OK;
  return aResult;
}

inline GeneralPolyResult LaguerreN(const double* theCoefficients,
                                   size_t        theSize,
                                   double        theTolerance     = 1.0e-12,
                                   uint32_t      theMaxIterations = 100)
{
  if (theSize < 2 || theSize > static_cast<size_t>(THE_MAX_POLY_DEGREE + 1))
  {
    PolyResult aResult;
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  return Laguerre(theCoefficients, static_cast<int>(theSize - 1), theTolerance, theMaxIterations);
}

inline GeneralPolyResult Quintic(double theA,
                                 double theB,
                                 double theC,
                                 double theD,
                                 double theE,
                                 double theF)
{
  const double aCoefficients[6] = {theF, theE, theD, theC, theB, theA};
  if (theA == 0.0)
  {
    return Quartic(theB, theC, theD, theE, theF);
  }
  return Laguerre(aCoefficients, 5);
}

inline GeneralPolyResult Sextic(double theA,
                                double theB,
                                double theC,
                                double theD,
                                double theE,
                                double theF,
                                double theG)
{
  const double aCoefficients[7] = {theG, theF, theE, theD, theC, theB, theA};
  if (theA == 0.0)
  {
    return Quintic(theB, theC, theD, theE, theF, theG);
  }
  return Laguerre(aCoefficients, 6);
}

inline GeneralPolyResult Octic(const double theCoefficients[9])
{
  return Laguerre(theCoefficients, 8);
}

} // namespace MathPoly

#endif // _MathPoly_Laguerre_HeaderFile

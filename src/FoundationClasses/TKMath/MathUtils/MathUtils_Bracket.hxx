// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathUtils_Bracket_HeaderFile
#define _MathUtils_Bracket_HeaderFile

#include <MathUtils_Core.hxx>
#include <MathUtils_Types.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

//! Correctness-first bracketing utilities.
namespace MathUtils
{

//! Result of root bracketing. Finite endpoint fields are available only when their HasF flag is
//! set.
struct BracketResult
{
  MathUtils::Status Status       = MathUtils::Status::NotConverged;
  bool              IsValid      = false;
  uint32_t          NbIterations = 0;
  double            A            = 0.0;
  double            B            = 0.0;
  double            Fa           = std::numeric_limits<double>::quiet_NaN();
  double            Fb           = std::numeric_limits<double>::quiet_NaN();
  bool              HasFA        = false;
  bool              HasFB        = false;
};

namespace Utils
{
template <typename Function>
bool EvaluateFinite(Function& theFunc, double theX, double& theValue, MathUtils::Status& theStatus)
{
  if (!theFunc.Value(theX, theValue))
  {
    theStatus = MathUtils::Status::CallbackError;
    return false;
  }
  if (!std::isfinite(theValue))
  {
    theStatus = MathUtils::Status::NumericalError;
    return false;
  }
  return true;
}

inline bool OppositeSigns(double theA, double theB)
{
  return theA == 0.0 || theB == 0.0 || std::signbit(theA) != std::signbit(theB);
}
} // namespace Utils

//! Bracket a root by expanding the endpoint with the smaller residual.
template <typename Function>
BracketResult BracketRoot(Function& theFunc, double theA, double theB, uint32_t theMaxIter = 50)
{
  BracketResult aResult;
  aResult.A = theA;
  aResult.B = theB;
  if (!std::isfinite(theA) || !std::isfinite(theB) || theA == theB || theMaxIter <= 0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  if (!Utils::EvaluateFinite(theFunc, aResult.A, aResult.Fa, aResult.Status))
  {
    return aResult;
  }
  aResult.HasFA = true;
  if (!Utils::EvaluateFinite(theFunc, aResult.B, aResult.Fb, aResult.Status))
  {
    return aResult;
  }
  aResult.HasFB = true;

  for (uint32_t anIter = 0; anIter <= theMaxIter; ++anIter)
  {
    if (Utils::OppositeSigns(aResult.Fa, aResult.Fb))
    {
      if (aResult.A > aResult.B)
      {
        std::swap(aResult.A, aResult.B);
        std::swap(aResult.Fa, aResult.Fb);
      }
      aResult.Status  = MathUtils::Status::OK;
      aResult.IsValid = true;
      return aResult;
    }
    if (anIter == theMaxIter)
    {
      break;
    }

    double* aPoint     = nullptr;
    double* aValue     = nullptr;
    double  aCandidate = 0.0;
    if (std::abs(aResult.Fa) < std::abs(aResult.Fb))
    {
      aPoint     = &aResult.A;
      aValue     = &aResult.Fa;
      aCandidate = aResult.A + THE_GOLDEN_RATIO * (aResult.A - aResult.B);
    }
    else
    {
      aPoint     = &aResult.B;
      aValue     = &aResult.Fb;
      aCandidate = aResult.B + THE_GOLDEN_RATIO * (aResult.B - aResult.A);
    }
    if (!std::isfinite(aCandidate) || aCandidate == *aPoint)
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    double aCandidateValue = 0.0;
    if (!Utils::EvaluateFinite(theFunc, aCandidate, aCandidateValue, aResult.Status))
    {
      return aResult;
    }
    *aPoint = aCandidate;
    *aValue = aCandidateValue;
    ++aResult.NbIterations;
  }
  aResult.Status = MathUtils::Status::MaxIterations;
  return aResult;
}

//! Result of minimum bracketing. On success A < B < C and Fb < Fa,Fc.
struct MinBracketResult
{
  MathUtils::Status Status       = MathUtils::Status::NotConverged;
  bool              IsValid      = false;
  uint32_t          NbIterations = 0;
  double            A            = 0.0;
  double            B            = 0.0;
  double            C            = 0.0;
  double            Fa           = std::numeric_limits<double>::quiet_NaN();
  double            Fb           = std::numeric_limits<double>::quiet_NaN();
  double            Fc           = std::numeric_limits<double>::quiet_NaN();
};

//! Options for minimum bracketing.
struct MinBracketOptions
{
  uint32_t MaxIterations = 50;
  bool   UseLimits     = false;
  double LeftLimit     = 0.0;
  double RightLimit    = 0.0;
  bool   HasFA         = false;
  bool   HasFB         = false;
  double FA            = 0.0;
  double FB            = 0.0;
};

namespace Utils
{
inline double Limited(double theValue, const MinBracketOptions& theOptions)
{
  return theOptions.UseLimits
           ? std::max(theOptions.LeftLimit, std::min(theOptions.RightLimit, theValue))
           : theValue;
}

inline bool IsValid(const MinBracketOptions& theOptions)
{
  return theOptions.MaxIterations > 0
         && (!theOptions.UseLimits
             || (std::isfinite(theOptions.LeftLimit) && std::isfinite(theOptions.RightLimit)
                 && theOptions.LeftLimit < theOptions.RightLimit))
         && (!theOptions.HasFA || std::isfinite(theOptions.FA))
         && (!theOptions.HasFB || std::isfinite(theOptions.FB));
}
} // namespace Utils

//! Bracket a strict local minimum by safeguarded golden-ratio expansion.
template <typename Function>
MinBracketResult BracketMinimum(Function&                theFunc,
                                double                   theA,
                                double                   theB,
                                const MinBracketOptions& theOptions = MinBracketOptions())
{
  MinBracketResult aResult;
  if (!Utils::IsValid(theOptions) || !std::isfinite(theA) || !std::isfinite(theB))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  aResult.A = Utils::Limited(theA, theOptions);
  aResult.B = Utils::Limited(theB, theOptions);
  if (aResult.A == aResult.B)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  const bool isUseFA = theOptions.HasFA && aResult.A == theA;
  const bool isUseFB = theOptions.HasFB && aResult.B == theB;
  if (isUseFA)
  {
    aResult.Fa = theOptions.FA;
  }
  else if (!Utils::EvaluateFinite(theFunc, aResult.A, aResult.Fa, aResult.Status))
  {
    return aResult;
  }
  if (isUseFB)
  {
    aResult.Fb = theOptions.FB;
  }
  else if (!Utils::EvaluateFinite(theFunc, aResult.B, aResult.Fb, aResult.Status))
  {
    return aResult;
  }

  if (aResult.Fb > aResult.Fa)
  {
    std::swap(aResult.A, aResult.B);
    std::swap(aResult.Fa, aResult.Fb);
  }
  aResult.C = Utils::Limited(aResult.B + THE_GOLDEN_RATIO * (aResult.B - aResult.A), theOptions);
  if (!std::isfinite(aResult.C))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  if (aResult.C == aResult.B)
  {
    aResult.Status = MathUtils::Status::NoSolution;
    return aResult;
  }
  if (!Utils::EvaluateFinite(theFunc, aResult.C, aResult.Fc, aResult.Status))
  {
    return aResult;
  }

  for (uint32_t anIter = 0; anIter <= theOptions.MaxIterations; ++anIter)
  {
    if (aResult.Fb < aResult.Fa && aResult.Fb < aResult.Fc)
    {
      if (aResult.A > aResult.C)
      {
        std::swap(aResult.A, aResult.C);
        std::swap(aResult.Fa, aResult.Fc);
      }
      if (aResult.A < aResult.B && aResult.B < aResult.C)
      {
        aResult.Status  = MathUtils::Status::OK;
        aResult.IsValid = true;
        return aResult;
      }
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    if (anIter == theOptions.MaxIterations)
    {
      break;
    }

    const double aCandidate =
      Utils::Limited(aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B), theOptions);
    if (!std::isfinite(aCandidate))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    if (aCandidate == aResult.C)
    {
      aResult.Status = MathUtils::Status::NoSolution;
      return aResult;
    }
    double aCandidateValue = 0.0;
    if (!Utils::EvaluateFinite(theFunc, aCandidate, aCandidateValue, aResult.Status))
    {
      return aResult;
    }
    aResult.A  = aResult.B;
    aResult.Fa = aResult.Fb;
    aResult.B  = aResult.C;
    aResult.Fb = aResult.Fc;
    aResult.C  = aCandidate;
    aResult.Fc = aCandidateValue;
    ++aResult.NbIterations;
  }
  aResult.Status = MathUtils::Status::MaxIterations;
  return aResult;
}

template <typename Function>
MinBracketResult BracketMinimum(Function& theFunc, double theA, double theB, uint32_t theMaxIter)
{
  MinBracketOptions anOptions;
  anOptions.MaxIterations = theMaxIter;
  return BracketMinimum(theFunc, theA, theB, anOptions);
}

} // namespace MathUtils

#endif // _MathUtils_Bracket_HeaderFile

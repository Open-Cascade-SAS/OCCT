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

#ifndef _MathUtils_Bracket_HeaderFile
#define _MathUtils_Bracket_HeaderFile

#include <MathUtils_Core.hxx>

#include <algorithm>
#include <cmath>
#include <utility>

//! Modern math solver utilities.
namespace MathUtils
{

//! Result of root bracketing operation.
struct BracketResult
{
  bool   IsValid = false; //!< True if valid bracket found
  double A       = 0.0;   //!< Lower bound
  double B       = 0.0;   //!< Upper bound
  double Fa      = 0.0;   //!< Function value at A
  double Fb      = 0.0;   //!< Function value at B
};

//! Bracket a root by expanding interval until sign change is found.
//! Starting from [theA, theB], expands outward using golden ratio.
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to bracket
//! @param theA initial lower bound
//! @param theB initial upper bound
//! @param theMaxIter maximum expansion iterations
//! @return bracketing result
template <typename Function>
BracketResult BracketRoot(Function& theFunc, double theA, double theB, int theMaxIter = 50)
{
  BracketResult aResult;
  aResult.A = theA;
  aResult.B = theB;

  if (!theFunc.Value(aResult.A, aResult.Fa))
  {
    return aResult;
  }
  if (!theFunc.Value(aResult.B, aResult.Fb))
  {
    return aResult;
  }

  for (int i = 0; i < theMaxIter; ++i)
  {
    if (aResult.Fa * aResult.Fb < 0.0)
    {
      aResult.IsValid = true;
      // Ensure A < B
      if (aResult.A > aResult.B)
      {
        std::swap(aResult.A, aResult.B);
        std::swap(aResult.Fa, aResult.Fb);
      }
      return aResult;
    }

    // Expand the interval using golden ratio
    if (std::abs(aResult.Fa) < std::abs(aResult.Fb))
    {
      aResult.A += THE_GOLDEN_RATIO * (aResult.A - aResult.B);
      if (!theFunc.Value(aResult.A, aResult.Fa))
      {
        return aResult;
      }
    }
    else
    {
      aResult.B += THE_GOLDEN_RATIO * (aResult.B - aResult.A);
      if (!theFunc.Value(aResult.B, aResult.Fb))
      {
        return aResult;
      }
    }
  }

  return aResult;
}

//! Result of minimum bracketing operation.
struct MinBracketResult
{
  bool   IsValid = false; //!< True if valid bracket found (Fb < Fa and Fb < Fc)
  double A       = 0.0;   //!< Left bound
  double B       = 0.0;   //!< Middle point (minimum location estimate)
  double C       = 0.0;   //!< Right bound
  double Fa      = 0.0;   //!< Function value at A
  double Fb      = 0.0;   //!< Function value at B
  double Fc      = 0.0;   //!< Function value at C
};

//! Options for minimum bracketing.
struct MinBracketOptions
{
  int    MaxIterations = 50;    //!< Maximum iterations
  bool   UseLimits     = false; //!< Enable hard limits for parameter
  double LeftLimit     = 0.0;   //!< Left hard limit (inclusive)
  double RightLimit    = 0.0;   //!< Right hard limit (inclusive)
  bool   HasFA         = false; //!< True if FA is precomputed
  bool   HasFB         = false; //!< True if FB is precomputed
  double FA            = 0.0;   //!< Precomputed f(A)
  double FB            = 0.0;   //!< Precomputed f(B)
};

namespace detail
{
inline double Limited(double theValue, const MinBracketOptions& theOptions)
{
  if (!theOptions.UseLimits)
  {
    return theValue;
  }
  return std::max(theOptions.LeftLimit, std::min(theOptions.RightLimit, theValue));
}

template <typename Function>
bool LimitAndMayBeSwap(Function&                theFunc,
                       const MinBracketOptions& theOptions,
                       const double             theA,
                       double&                  theB,
                       double&                  theFB,
                       double&                  theC,
                       double&                  theFC)
{
  theC = Limited(theC, theOptions);
  if (std::abs(theB - theC) < THE_ZERO_TOL)
  {
    return false;
  }
  if (!theFunc.Value(theC, theFC))
  {
    return false;
  }

  // Keep B between A and C
  if ((theA - theB) * (theB - theC) < 0.0)
  {
    std::swap(theB, theC);
    std::swap(theFB, theFC);
  }
  return true;
}
} // namespace detail

//! Bracket a minimum by finding three points a < b < c with f(b) < f(a) and f(b) < f(c).
//! Uses golden section expansion with parabolic interpolation.
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to bracket
//! @param theA initial point A
//! @param theB initial point B (should be to the right of A in descent direction)
//! @param theOptions bracketing options
//! @return bracketing result
template <typename Function>
MinBracketResult BracketMinimum(Function&                    theFunc,
                                double                       theA,
                                double                       theB,
                                const MinBracketOptions&     theOptions = MinBracketOptions())
{
  MinBracketResult aResult;
  if (theOptions.MaxIterations < 1)
  {
    return aResult;
  }
  if (theOptions.UseLimits && theOptions.LeftLimit > theOptions.RightLimit)
  {
    return aResult;
  }

  aResult.A = detail::Limited(theA, theOptions);
  aResult.B = detail::Limited(theB, theOptions);
  if (std::abs(aResult.A - aResult.B) < THE_ZERO_TOL)
  {
    return aResult;
  }

  const bool isUseFA =
    theOptions.HasFA && (!theOptions.UseLimits || std::abs(aResult.A - theA) < THE_ZERO_TOL);
  const bool isUseFB =
    theOptions.HasFB && (!theOptions.UseLimits || std::abs(aResult.B - theB) < THE_ZERO_TOL);

  if (isUseFA)
  {
    aResult.Fa = theOptions.FA;
  }
  else if (!theFunc.Value(aResult.A, aResult.Fa))
  {
    return aResult;
  }

  if (isUseFB)
  {
    aResult.Fb = theOptions.FB;
  }
  else if (!theFunc.Value(aResult.B, aResult.Fb))
  {
    return aResult;
  }

  // Ensure we go downhill from A to B
  if (aResult.Fb > aResult.Fa)
  {
    std::swap(aResult.A, aResult.B);
    std::swap(aResult.Fa, aResult.Fb);
  }

  // Initial guess for C using golden ratio
  aResult.C = aResult.B + THE_GOLDEN_RATIO * (aResult.B - aResult.A);
  if (theOptions.UseLimits)
  {
    if (!detail::LimitAndMayBeSwap(
          theFunc, theOptions, aResult.A, aResult.B, aResult.Fb, aResult.C, aResult.Fc))
    {
      return aResult;
    }
  }
  else if (!theFunc.Value(aResult.C, aResult.Fc))
  {
    return aResult;
  }

  // Keep expanding until we bracket a minimum
  for (int anIter = 0; anIter < theOptions.MaxIterations && aResult.Fb >= aResult.Fc; ++anIter)
  {
    // Parabolic extrapolation
    const double aR     = (aResult.B - aResult.A) * (aResult.Fb - aResult.Fc);
    const double aQ     = (aResult.B - aResult.C) * (aResult.Fb - aResult.Fa);
    const double aDenom = 2.0 * SignTransfer(std::max(std::abs(aQ - aR), THE_ZERO_TOL), aQ - aR);

    double aU = aResult.B - ((aResult.B - aResult.C) * aQ - (aResult.B - aResult.A) * aR) / aDenom;

    double       aULim = aResult.B + 100.0 * (aResult.C - aResult.B);
    if (theOptions.UseLimits)
    {
      aULim = detail::Limited(aULim, theOptions);
    }
    double       aFu   = 0.0;

    if ((aResult.B - aU) * (aU - aResult.C) > 0.0)
    {
      // U is between B and C
      if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }

      if (aFu < aResult.Fc)
      {
        aResult.A       = aResult.B;
        aResult.B       = aU;
        aResult.Fa      = aResult.Fb;
        aResult.Fb      = aFu;
        aResult.IsValid = true;
        return aResult;
      }
      else if (aFu > aResult.Fb)
      {
        aResult.C       = aU;
        aResult.Fc      = aFu;
        aResult.IsValid = true;
        return aResult;
      }

      // Parabolic step didn't help, use golden section
      aU = aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B);
      if (theOptions.UseLimits)
      {
        if (!detail::LimitAndMayBeSwap(theFunc, theOptions, aResult.B, aResult.C, aResult.Fc, aU, aFu))
        {
          return aResult;
        }
      }
      else if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }
    }
    else if ((aResult.C - aU) * (aU - aULim) > 0.0)
    {
      // U is between C and limit
      if (theOptions.UseLimits)
      {
        if (!detail::LimitAndMayBeSwap(theFunc, theOptions, aResult.B, aResult.C, aResult.Fc, aU, aFu))
        {
          return aResult;
        }
      }
      else if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }

      if (aFu < aResult.Fc)
      {
        aResult.B  = aResult.C;
        aResult.C  = aU;
        aU         = aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B);
        aResult.Fb = aResult.Fc;
        aResult.Fc = aFu;
        if (theOptions.UseLimits)
        {
          if (!detail::LimitAndMayBeSwap(
                theFunc, theOptions, aResult.B, aResult.C, aResult.Fc, aU, aFu))
          {
            return aResult;
          }
        }
        else if (!theFunc.Value(aU, aFu))
        {
          return aResult;
        }
      }
    }
    else if ((aU - aULim) * (aULim - aResult.C) >= 0.0)
    {
      // U is beyond limit
      aU = aULim;
      if (theOptions.UseLimits)
      {
        if (!detail::LimitAndMayBeSwap(theFunc, theOptions, aResult.B, aResult.C, aResult.Fc, aU, aFu))
        {
          return aResult;
        }
      }
      else if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }
    }
    else
    {
      // Default golden section step
      aU = aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B);
      if (theOptions.UseLimits)
      {
        if (!detail::LimitAndMayBeSwap(theFunc, theOptions, aResult.B, aResult.C, aResult.Fc, aU, aFu))
        {
          return aResult;
        }
      }
      else if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }
    }

    // Shift points
    aResult.A  = aResult.B;
    aResult.B  = aResult.C;
    aResult.C  = aU;
    aResult.Fa = aResult.Fb;
    aResult.Fb = aResult.Fc;
    aResult.Fc = aFu;
  }

  aResult.IsValid = (aResult.Fb < aResult.Fa && aResult.Fb < aResult.Fc);

  // Ensure A < B < C ordering
  if (aResult.IsValid && aResult.A > aResult.C)
  {
    std::swap(aResult.A, aResult.C);
    std::swap(aResult.Fa, aResult.Fc);
  }

  if (aResult.IsValid && !(aResult.A < aResult.B && aResult.B < aResult.C))
  {
    aResult.IsValid = false;
  }

  return aResult;
}

//! Backward-compatible convenience overload with only max-iterations argument.
template <typename Function>
MinBracketResult BracketMinimum(Function& theFunc, double theA, double theB, int theMaxIter)
{
  MinBracketOptions anOptions;
  anOptions.MaxIterations = theMaxIter;
  return BracketMinimum(theFunc, theA, theB, anOptions);
}

} // namespace MathUtils

#endif // _MathUtils_Bracket_HeaderFile

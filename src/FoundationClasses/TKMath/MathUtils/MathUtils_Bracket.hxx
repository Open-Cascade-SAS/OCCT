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

//! Bracket a minimum by finding three points a < b < c with f(b) < f(a) and f(b) < f(c).
//! Uses golden section expansion with parabolic interpolation.
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to bracket
//! @param theA initial point A
//! @param theB initial point B (should be to the right of A in descent direction)
//! @param theMaxIter maximum iterations
//! @return bracketing result
template <typename Function>
MinBracketResult BracketMinimum(Function& theFunc, double theA, double theB, int theMaxIter = 50)
{
  MinBracketResult aResult;
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

  // Ensure we go downhill from A to B
  if (aResult.Fb > aResult.Fa)
  {
    std::swap(aResult.A, aResult.B);
    std::swap(aResult.Fa, aResult.Fb);
  }

  // Initial guess for C using golden ratio
  aResult.C = aResult.B + THE_GOLDEN_RATIO * (aResult.B - aResult.A);
  if (!theFunc.Value(aResult.C, aResult.Fc))
  {
    return aResult;
  }

  // Keep expanding until we bracket a minimum
  for (int anIter = 0; anIter < theMaxIter && aResult.Fb >= aResult.Fc; ++anIter)
  {
    // Parabolic extrapolation
    const double aR = (aResult.B - aResult.A) * (aResult.Fb - aResult.Fc);
    const double aQ = (aResult.B - aResult.C) * (aResult.Fb - aResult.Fa);
    const double aDenom = 2.0 * SignTransfer(std::max(std::abs(aQ - aR), THE_ZERO_TOL), aQ - aR);

    double aU = aResult.B - ((aResult.B - aResult.C) * aQ - (aResult.B - aResult.A) * aR) / aDenom;

    const double aULim = aResult.B + 100.0 * (aResult.C - aResult.B);
    double       aFu = 0.0;

    if ((aResult.B - aU) * (aU - aResult.C) > 0.0)
    {
      // U is between B and C
      if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }

      if (aFu < aResult.Fc)
      {
        aResult.A = aResult.B;
        aResult.B = aU;
        aResult.Fa = aResult.Fb;
        aResult.Fb = aFu;
        aResult.IsValid = true;
        return aResult;
      }
      else if (aFu > aResult.Fb)
      {
        aResult.C = aU;
        aResult.Fc = aFu;
        aResult.IsValid = true;
        return aResult;
      }

      // Parabolic step didn't help, use golden section
      aU = aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B);
      if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }
    }
    else if ((aResult.C - aU) * (aU - aULim) > 0.0)
    {
      // U is between C and limit
      if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }

      if (aFu < aResult.Fc)
      {
        aResult.B = aResult.C;
        aResult.C = aU;
        aU = aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B);
        aResult.Fb = aResult.Fc;
        aResult.Fc = aFu;
        if (!theFunc.Value(aU, aFu))
        {
          return aResult;
        }
      }
    }
    else if ((aU - aULim) * (aULim - aResult.C) >= 0.0)
    {
      // U is beyond limit
      aU = aULim;
      if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }
    }
    else
    {
      // Default golden section step
      aU = aResult.C + THE_GOLDEN_RATIO * (aResult.C - aResult.B);
      if (!theFunc.Value(aU, aFu))
      {
        return aResult;
      }
    }

    // Shift points
    aResult.A = aResult.B;
    aResult.B = aResult.C;
    aResult.C = aU;
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

  return aResult;
}

} // namespace MathUtils

#endif // _MathUtils_Bracket_HeaderFile

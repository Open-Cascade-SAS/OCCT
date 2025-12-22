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

#ifndef _MathRoot_Trig_HeaderFile
#define _MathRoot_Trig_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathPoly.hxx>

#include <cmath>
#include <algorithm>

namespace MathRoot
{
using namespace MathUtils;

//! Result for trigonometric equation solver.
struct TrigResult
{
  MathUtils::Status     Status        = MathUtils::Status::NotConverged;
  std::array<double, 4> Roots         = {0.0, 0.0, 0.0, 0.0};
  int                   NbRoots       = 0;
  bool                  InfiniteRoots = false;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Solve trigonometric equation: a*cos^2(x) + 2*b*cos(x)*sin(x) + c*cos(x) + d*sin(x) + e = 0.
//!
//! Uses half-angle substitution t = tan(x/2) to convert to polynomial:
//! - cos(x) = (1-t^2)/(1+t^2)
//! - sin(x) = 2t/(1+t^2)
//!
//! Resulting polynomial is of degree 4, 3, or 2 depending on coefficients.
//! Roots are filtered to lie within [theInfBound, theSupBound].
//!
//! @param theA coefficient of cos^2(x)
//! @param theB coefficient of cos(x)*sin(x) (equation uses 2*b)
//! @param theC coefficient of cos(x)
//! @param theD coefficient of sin(x)
//! @param theE constant term
//! @param theInfBound lower bound for roots (default 0)
//! @param theSupBound upper bound for roots (default 2*PI)
//! @param theEps tolerance for coefficient comparison
//! @return TrigResult containing roots in specified interval
inline TrigResult Trigonometric(double theA,
                                double theB,
                                double theC,
                                double theD,
                                double theE,
                                double theInfBound = 0.0,
                                double theSupBound = THE_2PI,
                                double theEps      = 1.5e-12)
{
  TrigResult aResult;
  aResult.Status = MathUtils::Status::OK;

  // Compute working interval
  double aMyBorneInf, aDelta, aMod;
  if (theInfBound <= std::numeric_limits<double>::lowest() / 2.0
      && theSupBound >= std::numeric_limits<double>::max() / 2.0)
  {
    aMyBorneInf = 0.0;
    aDelta      = THE_2PI;
    aMod        = 0.0;
  }
  else if (theSupBound >= std::numeric_limits<double>::max() / 2.0)
  {
    aMyBorneInf = theInfBound;
    aDelta      = THE_2PI;
    aMod        = aMyBorneInf / THE_2PI;
  }
  else if (theInfBound <= std::numeric_limits<double>::lowest() / 2.0)
  {
    aMyBorneInf = theSupBound - THE_2PI;
    aDelta      = THE_2PI;
    aMod        = aMyBorneInf / THE_2PI;
  }
  else
  {
    aMyBorneInf = theInfBound;
    aDelta      = theSupBound - theInfBound;
    aMod        = theInfBound / THE_2PI;
    if (aDelta > THE_2PI)
    {
      aDelta = THE_2PI;
    }
  }

  std::array<double, 4> aZer       = {0.0, 0.0, 0.0, 0.0};
  size_t                aNZer      = 0;
  const double          aDelta_Eps = std::numeric_limits<double>::epsilon() * std::abs(aDelta);

  // Case: A = B = 0 (degree <= 2 in cos/sin)
  if (std::abs(theA) <= theEps && std::abs(theB) <= theEps)
  {
    if (std::abs(theC) <= theEps)
    {
      if (std::abs(theD) <= theEps)
      {
        if (std::abs(theE) <= theEps)
        {
          aResult.InfiniteRoots = true;
          return aResult;
        }
        else
        {
          aResult.NbRoots = 0;
          return aResult;
        }
      }
      else
      {
        // d*sin(x) + e = 0  =>  sin(x) = -e/d
        double aVal = -theE / theD;
        if (std::abs(aVal) > 1.0)
        {
          aResult.NbRoots = 0;
          return aResult;
        }

        aZer[0] = std::asin(aVal);
        aZer[1] = THE_PI - aZer[0];
        aNZer   = 2;

        // Adjust to positive range
        for (size_t i = 0; i < aNZer; ++i)
        {
          if (aZer[i] <= -theEps)
          {
            aZer[i] = THE_2PI - std::abs(aZer[i]);
          }
          aZer[i] += std::trunc(aMod) * THE_2PI;
          double aX = aZer[i] - aMyBorneInf;
          if (aX >= -aDelta_Eps && aX <= aDelta + aDelta_Eps)
          {
            aResult.Roots[aResult.NbRoots++] = aZer[i];
          }
        }
        return aResult;
      }
    }
    else if (std::abs(theD) <= theEps)
    {
      // c*cos(x) + e = 0  =>  cos(x) = -e/c
      double aVal = -theE / theC;
      if (std::abs(aVal) > 1.0)
      {
        aResult.NbRoots = 0;
        return aResult;
      }

      aZer[0] = std::acos(aVal);
      aZer[1] = -aZer[0];
      aNZer   = 2;

      for (size_t i = 0; i < aNZer; ++i)
      {
        if (aZer[i] <= -theEps)
        {
          aZer[i] = THE_2PI - std::abs(aZer[i]);
        }
        aZer[i] += std::trunc(aMod) * THE_2PI;
        double aX = aZer[i] - aMyBorneInf;
        if (aX >= -aDelta_Eps && aX <= aDelta + aDelta_Eps)
        {
          aResult.Roots[aResult.NbRoots++] = aZer[i];
        }
      }
      return aResult;
    }
    else
    {
      // c*cos(x) + d*sin(x) + e = 0
      // Using t = tan(x/2): (e-c) + 2dt + (e+c)t^2 = 0
      double aAA = theE - theC;
      double aBB = 2.0 * theD;
      double aCC = theE + theC;

      MathPoly::PolyResult aPoly = MathPoly::Quadratic(aCC, aBB, aAA);
      if (!aPoly.IsDone())
      {
        aResult.Status = aPoly.Status;
        return aResult;
      }
      if (aPoly.Status == MathUtils::Status::InfiniteSolutions)
      {
        aResult.InfiniteRoots = true;
        return aResult;
      }

      aNZer = aPoly.NbRoots;
      for (size_t i = 0; i < aNZer; ++i)
      {
        aZer[i] = aPoly.Roots[i];
      }
    }
  }
  else
  {
    // Special case: A = E = 0
    if (std::abs(theA) <= theEps && std::abs(theE) <= theEps)
    {
      if (std::abs(theC) <= theEps)
      {
        // 2*B*sin*cos + D*sin = 0  =>  sin(x)*(2*B*cos(x) + D) = 0
        aZer[0] = 0.0;
        aZer[1] = THE_PI;
        aNZer   = 2;

        double aVal = -theD / (theB * 2.0);
        if (std::abs(aVal) <= 1.0 + 1.0e-10)
        {
          if (aVal >= 1.0)
          {
            aZer[2] = 0.0;
            aZer[3] = 0.0;
          }
          else if (aVal <= -1.0)
          {
            aZer[2] = THE_PI;
            aZer[3] = THE_PI;
          }
          else
          {
            aZer[2] = std::acos(aVal);
            aZer[3] = THE_2PI - aZer[2];
          }
          aNZer = 4;
        }

        for (size_t i = 0; i < aNZer; ++i)
        {
          if (aZer[i] <= aMyBorneInf - theEps)
          {
            aZer[i] += THE_2PI;
          }
          aZer[i] += std::trunc(aMod) * THE_2PI;
          double aX = aZer[i] - aMyBorneInf;
          if (aX >= -1.0e-10 && aX <= aDelta + 1.0e-10)
          {
            aZer[i] = std::max(theInfBound, std::min(theSupBound, aZer[i]));
            aResult.Roots[aResult.NbRoots++] = aZer[i];
          }
        }
        return aResult;
      }
      if (std::abs(theD) <= theEps)
      {
        // 2*B*sin*cos + C*cos = 0  =>  cos(x)*(2*B*sin(x) + C) = 0
        aZer[0] = THE_PI / 2.0;
        aZer[1] = THE_PI * 3.0 / 2.0;
        aNZer   = 2;

        double aVal = -theC / (theB * 2.0);
        if (std::abs(aVal) <= 1.0 + 1.0e-10)
        {
          if (aVal >= 1.0)
          {
            aZer[2] = THE_PI / 2.0;
            aZer[3] = THE_PI / 2.0;
          }
          else if (aVal <= -1.0)
          {
            aZer[2] = THE_PI * 3.0 / 2.0;
            aZer[3] = THE_PI * 3.0 / 2.0;
          }
          else
          {
            aZer[2] = std::asin(aVal);
            aZer[3] = THE_PI - aZer[2];
          }
          aNZer = 4;
        }

        for (size_t i = 0; i < aNZer; ++i)
        {
          if (aZer[i] <= aMyBorneInf - theEps)
          {
            aZer[i] += THE_2PI;
          }
          aZer[i] += std::trunc(aMod) * THE_2PI;
          double aX = aZer[i] - aMyBorneInf;
          if (aX >= -1.0e-10 && aX <= aDelta + 1.0e-10)
          {
            aZer[i] = std::max(theInfBound, std::min(theSupBound, aZer[i]));
            aResult.Roots[aResult.NbRoots++] = aZer[i];
          }
        }
        return aResult;
      }
    }

    // General case: degree 4 polynomial
    // t = tan(x/2), then:
    // ko[0]*t^4 + ko[1]*t^3 + ko[2]*t^2 + ko[3]*t + ko[4] = 0
    double ko0 = theA - theC + theE;
    double ko1 = 2.0 * theD - 4.0 * theB;
    double ko2 = 2.0 * theE - 2.0 * theA;
    double ko3 = 4.0 * theB + 2.0 * theD;
    double ko4 = theA + theC + theE;

    MathPoly::PolyResult aPoly = MathPoly::Quartic(ko0, ko1, ko2, ko3, ko4);
    if (!aPoly.IsDone())
    {
      if (aPoly.Status == MathUtils::Status::InfiniteSolutions)
      {
        aResult.InfiniteRoots = true;
      }
      else
      {
        aResult.Status = aPoly.Status;
      }
      return aResult;
    }

    aNZer = aPoly.NbRoots;
    for (size_t i = 0; i < aNZer; ++i)
    {
      aZer[i] = aPoly.Roots[i];
    }

    // Sort roots
    std::sort(aZer.begin(), aZer.begin() + aNZer);
  }

  // Convert t values to angles and filter by bounds
  for (size_t i = 0; i < aNZer; ++i)
  {
    double aTeta = 2.0 * std::atan(aZer[i]);
    if (aZer[i] <= -theEps)
    {
      aTeta = THE_2PI - std::abs(aTeta);
    }
    aTeta += std::trunc(aMod) * THE_2PI;
    if (aTeta - aMyBorneInf < 0.0)
    {
      aTeta += THE_2PI;
    }

    double aX = aTeta - aMyBorneInf;
    if (aX >= -aDelta_Eps && aX <= aDelta + aDelta_Eps)
    {
      // Newton refinement
      auto aRefineRoot = [&](double theX) -> double {
        constexpr int    THE_MAX_ITER = 10;
        constexpr double THE_TOL      = 1.0e-15;

        for (int anIter = 0; anIter < THE_MAX_ITER; ++anIter)
        {
          double aCos = std::cos(theX);
          double aSin = std::sin(theX);
          double aF =
            theA * aCos * aCos + 2.0 * theB * aCos * aSin + theC * aCos + theD * aSin + theE;
          double aDF = -2.0 * theA * aCos * aSin + 2.0 * theB * (aCos * aCos - aSin * aSin)
                       - theC * aSin + theD * aCos;

          if (std::abs(aDF) < 1.0e-30)
          {
            break;
          }

          double aDelta = aF / aDF;
          theX -= aDelta;

          if (std::abs(aDelta) < THE_TOL)
          {
            break;
          }
        }
        return theX;
      };

      double aTetaRefined = aRefineRoot(aTeta);

      // Check if Newton didn't diverge too far
      double aDeltaNewton = std::abs(aTetaRefined - aTeta);
      double aSupmInfs100 = (theSupBound - theInfBound) * 0.01;
      if (aDeltaNewton <= aSupmInfs100)
      {
        aTeta = aTetaRefined;
      }

      // Insert in sorted order, avoiding duplicates
      bool aFound = false;
      for (int k = 0; k < aResult.NbRoots; ++k)
      {
        if (std::abs(aTeta - aResult.Roots[k]) < theEps)
        {
          aFound = true;
          break;
        }
      }

      if (!aFound && aResult.NbRoots < 4)
      {
        // Insert sorted
        int aPos = aResult.NbRoots;
        for (int k = 0; k < aResult.NbRoots; ++k)
        {
          if (aTeta < aResult.Roots[k])
          {
            aPos = k;
            break;
          }
        }
        for (int k = aResult.NbRoots; k > aPos; --k)
        {
          aResult.Roots[k] = aResult.Roots[k - 1];
        }
        aResult.Roots[aPos] = aTeta;
        aResult.NbRoots++;
      }
    }
  }

  // Special case: check if PI is a root (when A - C + E = 0)
  if (aResult.NbRoots < 4 && std::abs(theA - theC + theE) <= theEps)
  {
    double aTeta = THE_PI + std::trunc(aMod) * THE_2PI;
    double aX    = aTeta - aMyBorneInf;
    if (aX >= -aDelta_Eps && aX <= aDelta + aDelta_Eps)
    {
      bool aFound = false;
      for (int k = 0; k < aResult.NbRoots; ++k)
      {
        if (std::abs(aTeta - aResult.Roots[k]) <= theEps)
        {
          aFound = true;
          break;
        }
      }
      if (!aFound)
      {
        int aPos = aResult.NbRoots;
        for (int k = 0; k < aResult.NbRoots; ++k)
        {
          if (aTeta < aResult.Roots[k])
          {
            aPos = k;
            break;
          }
        }
        for (int k = aResult.NbRoots; k > aPos; --k)
        {
          aResult.Roots[k] = aResult.Roots[k - 1];
        }
        aResult.Roots[aPos] = aTeta;
        aResult.NbRoots++;
      }
    }
  }

  return aResult;
}

//! Solve linear trigonometric equation: d*sin(x) + e = 0.
//!
//! @param theD coefficient of sin(x)
//! @param theE constant term
//! @param theInfBound lower bound for roots
//! @param theSupBound upper bound for roots
//! @return TrigResult containing roots
inline TrigResult TrigonometricLinear(double theD,
                                      double theE,
                                      double theInfBound = 0.0,
                                      double theSupBound = THE_2PI)
{
  return Trigonometric(0.0, 0.0, 0.0, theD, theE, theInfBound, theSupBound);
}

//! Solve trigonometric equation: c*cos(x) + d*sin(x) + e = 0.
//!
//! @param theC coefficient of cos(x)
//! @param theD coefficient of sin(x)
//! @param theE constant term
//! @param theInfBound lower bound for roots
//! @param theSupBound upper bound for roots
//! @return TrigResult containing roots
inline TrigResult TrigonometricCDE(double theC,
                                   double theD,
                                   double theE,
                                   double theInfBound = 0.0,
                                   double theSupBound = THE_2PI)
{
  return Trigonometric(0.0, 0.0, theC, theD, theE, theInfBound, theSupBound);
}

} // namespace MathRoot

#endif // _MathRoot_Trig_HeaderFile

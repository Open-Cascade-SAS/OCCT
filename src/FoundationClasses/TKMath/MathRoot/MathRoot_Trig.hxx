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
#include <MathUtils_Poly.hxx>
#include <MathRoot_Utils.hxx>
#include <MathPoly_Bounded.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <array>
#include <cmath>

namespace MathRoot
{
using namespace MathUtils;

//! Result for trigonometric equation solver.
struct TrigResult
{
  MathUtils::Status     Status        = MathUtils::Status::NotConverged;
  std::array<double, 4> Roots         = {0.0, 0.0, 0.0, 0.0};
  size_t                NbRoots       = 0;
  bool                  InfiniteRoots = false;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

namespace Utils
{

//! Inserts a root in ascending order unless an equivalent root is already present.
inline void AddDistinctTrigRoot(TrigResult& theResult, double theRoot, double theTolerance)
{
  size_t aPosition = 0;
  while (aPosition < theResult.NbRoots && theResult.Roots[aPosition] < theRoot)
  {
    if (std::abs(theRoot - theResult.Roots[aPosition]) <= theTolerance)
    {
      return;
    }
    ++aPosition;
  }
  if ((aPosition < theResult.NbRoots
       && std::abs(theRoot - theResult.Roots[aPosition]) <= theTolerance)
      || theResult.NbRoots == theResult.Roots.size())
  {
    return;
  }
  for (size_t anIndex = theResult.NbRoots; anIndex > aPosition; --anIndex)
  {
    theResult.Roots[anIndex] = theResult.Roots[anIndex - 1];
  }
  theResult.Roots[aPosition] = theRoot;
  ++theResult.NbRoots;
}

//! Maps a periodic root into the interval.
inline bool MapPeriodicTrigRoot(double  theRoot,
                                double  theLower,
                                double  theUpper,
                                double  theTolerance,
                                double& theMappedRoot)
{
  // Determine the final turn before translating. Subtracting a period and
  // adding it back destroys small roots that already lie in the interval.
  double aTurn = std::floor((theLower - theRoot) / THE_2PI);
  if (std::fma(aTurn, THE_2PI, theRoot) < theLower - theTolerance)
  {
    aTurn += 1;
  }
  theRoot = std::fma(aTurn, THE_2PI, theRoot);
  if (theRoot < theLower)
  {
    theRoot = theLower;
  }
  if (theRoot > theUpper + theTolerance)
  {
    return false;
  }
  if (theRoot > theUpper)
  {
    theRoot = theUpper;
  }
  size_t aNbRepresentatives = 0;
  if (!ComputePeriodicRootCount(theRoot, theUpper, THE_2PI, theTolerance, false, aNbRepresentatives)
      || aNbRepresentatives == 0)
  {
    return false;
  }
  theMappedRoot = theRoot;
  return true;
}

//! Maps a periodic root into the interval and appends it when present.
inline void AddPeriodicTrigRoot(TrigResult& theResult,
                                double      theRoot,
                                double      theLower,
                                double      theUpper,
                                double      theTolerance)
{
  double aMappedRoot = 0.0;
  if (MapPeriodicTrigRoot(theRoot, theLower, theUpper, theTolerance, aMappedRoot))
  {
    AddDistinctTrigRoot(theResult, aMappedRoot, theTolerance);
  }
}

} // namespace Utils

//! Solve trigonometric equation: a*cos^2(x) + 2*b*cos(x)*sin(x) + c*cos(x) + d*sin(x) + e = 0.
//!
//! Uses two bounded half-angle charts (tan(x/2) and cot(x/2)) to avoid
//! unbounded polynomial roots near PI. In the tangent chart:
//! - cos(x) = (1-t^2)/(1+t^2)
//! - sin(x) = 2t/(1+t^2)
//!
//! Each chart produces a polynomial of degree at most 4.
//! Roots are filtered to lie within [theInfBound, theSupBound].
//!
//! @param theA coefficient of cos^2(x)
//! @param theB coefficient of cos(x)*sin(x) (equation uses 2*b)
//! @param theC coefficient of cos(x)
//! @param theD coefficient of sin(x)
//! @param theE constant term
//! @param theInfBound lower bound for roots (default 0)
//! @param theSupBound upper bound for roots (default 2*PI)
//! @param theEps relative tolerance for coefficient classification after common scaling
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
  const bool isLowerInfinite = Precision::IsNegativeInfinite(theInfBound);
  const bool isUpperInfinite = Precision::IsPositiveInfinite(theSupBound);
  if (!std::isfinite(theA) || !std::isfinite(theB) || !std::isfinite(theC) || !std::isfinite(theD)
      || !std::isfinite(theE) || !Utils::IsValidBounds(theInfBound, theSupBound)
      || Precision::IsPositiveInfinite(theInfBound) || Precision::IsNegativeInfinite(theSupBound)
      || !std::isfinite(theEps) || theEps <= 0.0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  aResult.Status = MathUtils::Status::OK;

  const double aCoefficientScale =
    std::max({std::abs(theA), std::abs(theB), std::abs(theC), std::abs(theD), std::abs(theE)});
  if (aCoefficientScale > 0.0)
  {
    theA /= aCoefficientScale;
    theB /= aCoefficientScale;
    theC /= aCoefficientScale;
    theD /= aCoefficientScale;
    theE /= aCoefficientScale;
  }

  // Compute working interval
  double aMyBorneInf, aDelta;
  if (isLowerInfinite && isUpperInfinite)
  {
    aMyBorneInf = 0.0;
    aDelta      = THE_2PI;
  }
  else if (isUpperInfinite)
  {
    aMyBorneInf = theInfBound;
    aDelta      = THE_2PI;
  }
  else if (isLowerInfinite)
  {
    aMyBorneInf = theSupBound - THE_2PI;
    aDelta      = THE_2PI;
  }
  else
  {
    aMyBorneInf = theInfBound;
    aDelta      = theSupBound - theInfBound;
    if (aDelta > THE_2PI)
    {
      aDelta = THE_2PI;
    }
  }

  std::array<double, 4> aZer       = {0.0, 0.0, 0.0, 0.0};
  size_t                aNZer      = 0;
  const double          aDeltaEps  = Precision::Computational() * std::abs(aDelta);
  const double          anUpper    = aMyBorneInf + aDelta;
  const double          anAngleTol = std::max(theEps, aDeltaEps);

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

        for (size_t i = 0; i < aNZer; ++i)
        {
          Utils::AddPeriodicTrigRoot(aResult, aZer[i], aMyBorneInf, anUpper, anAngleTol);
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

      double aPrincipal = std::acos(aVal);
      aZer[0]           = aPrincipal;  // acos gives [0, PI]
      aZer[1]           = -aPrincipal; // Negative angle
      aNZer             = 2;

      // For each solution, find the representative in or near the given bounds
      for (size_t i = 0; i < aNZer; ++i)
      {
        Utils::AddPeriodicTrigRoot(aResult, aZer[i], aMyBorneInf, anUpper, anAngleTol);
      }
      return aResult;
    }
    else
    {
      if (theE == 0)
      {
        // A normal to (C,D) gives the homogeneous roots directly. Forming
        // phase +/- pi/2 would cancel significant digits near zero.
        const double anAngle = std::atan2(-theC, theD);
        Utils::AddPeriodicTrigRoot(aResult, anAngle, aMyBorneInf, anUpper, anAngleTol);
        Utils::AddPeriodicTrigRoot(aResult, anAngle + THE_PI, aMyBorneInf, anUpper, anAngleTol);
        return aResult;
      }
      // c*cos(x) + d*sin(x) = r*cos(x - phase)
      const double aRadius = std::hypot(theC, theD);
      double       aRatio  = -theE / aRadius;
      if (std::abs(aRatio) > 1.0 + theEps)
      {
        aResult.NbRoots = 0;
        return aResult;
      }
      aRatio                = std::max(-1.0, std::min(1.0, aRatio));
      const double aPhase   = std::atan2(theD, theC);
      const double anOffset = std::acos(aRatio);
      aZer[0]               = aPhase + anOffset;
      aZer[1]               = aPhase - anOffset;
      for (size_t anIndex = 0; anIndex < 2; ++anIndex)
      {
        Utils::AddPeriodicTrigRoot(aResult, aZer[anIndex], aMyBorneInf, anUpper, anAngleTol);
      }
      return aResult;
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
          Utils::AddPeriodicTrigRoot(aResult, aZer[i], aMyBorneInf, anUpper, anAngleTol);
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
          Utils::AddPeriodicTrigRoot(aResult, aZer[i], aMyBorneInf, anUpper, anAngleTol);
        }
        return aResult;
      }
    }

    // Preserve roots at chart centres and shared endpoints when direct
    // coefficient combinations suffer cancellation. Compensated Horner
    // evaluates the same exact stored coefficients with a recovered rounding term.
    const double aAtZero[3] = {theE, theC, theA};
    if (MathUtils::EvalPolyCompensated(aAtZero, 2, 1.0) == 0.0)
    {
      Utils::AddPeriodicTrigRoot(aResult, 0.0, aMyBorneInf, anUpper, anAngleTol);
    }
    const double aAtPi[3] = {theE, -theC, theA};
    if (MathUtils::EvalPolyCompensated(aAtPi, 2, 1.0) == 0.0)
    {
      Utils::AddPeriodicTrigRoot(aResult, THE_PI, aMyBorneInf, anUpper, anAngleTol);
    }
    const double aAtHalfPi[2] = {theE, theD};
    if (MathUtils::EvalPolyCompensated(aAtHalfPi, 1, 1.0) == 0.0)
    {
      Utils::AddPeriodicTrigRoot(aResult, THE_PI / 2.0, aMyBorneInf, anUpper, anAngleTol);
    }
    const double aAtThreeHalfPi[2] = {theE, -theD};
    if (MathUtils::EvalPolyCompensated(aAtThreeHalfPi, 1, 1.0) == 0.0)
    {
      Utils::AddPeriodicTrigRoot(aResult, 3.0 * THE_PI / 2.0, aMyBorneInf, anUpper, anAngleTol);
    }

    // Cover the circle by two bounded half-angle charts. In the first chart
    // t=tan(x/2), |t|<=1; in the second t=cot(x/2), |t|<=1. Reversing the
    // polynomial coefficients supplies the second chart. No root tends to
    // infinity, and a small leading coefficient never discards roots near pi.
    //
    // tan chart: [-pi/2, pi/2]       cot chart: [pi/2, 3*pi/2]
    //            x=2*atan(t)                    x=pi-2*atan(t)
    //
    // Ascending coefficients of (1+t*t)^2 * F(2*atan(t)).
    std::array<double, 5> aCoefficients = {theA + theC + theE,
                                           4.0 * theB + 2.0 * theD,
                                           2.0 * (theE - theA),
                                           2.0 * theD - 4.0 * theB,
                                           theA - theC + theE};
    for (int aChart = 0; aChart < 2; ++aChart)
    {
      const MathPoly::BoundedResult aRoots = MathPoly::Bounded(aCoefficients.data(), 4, -1, 1);
      if (!aRoots.IsDone())
      {
        if (aRoots.Status == MathUtils::Status::InfiniteSolutions)
        {
          aResult.InfiniteRoots = true;
          aResult.Status        = MathUtils::Status::OK;
        }
        else
        {
          aResult.Status = aRoots.Status;
        }
        return aResult;
      }
      for (size_t i = 0; i < aRoots.NbRoots; ++i)
      {
        const double aRoot = aRoots.Roots[i];
        // The first chart owns the two shared endpoints. Avoid accepting a
        // duplicated root solely because of its independent angle rounding.
        if (aChart == 1 && (aRoot == -1 || aRoot == 1))
        {
          continue;
        }
        const double anAngle =
          aChart == 0 ? 2.0 * std::atan(aRoot) : THE_PI - 2.0 * std::atan(aRoot);
        Utils::AddPeriodicTrigRoot(aResult, anAngle, aMyBorneInf, anUpper, anAngleTol);
      }
      std::reverse(aCoefficients.begin(), aCoefficients.end());
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

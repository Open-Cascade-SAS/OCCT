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

#ifndef _MathRoot_Utils_HeaderFile
#define _MathRoot_Utils_HeaderFile

#include <MathUtils_Config.hxx>

#include <cmath>
#include <cstddef>

namespace MathRoot
{
namespace Utils
{

//! Return true when a scalar root solver configuration is usable.
inline bool IsValidConfig(const MathUtils::Config& theConfig)
{
  return theConfig.MaxIterations > 0 && std::isfinite(theConfig.XTolerance)
         && theConfig.XTolerance > 0.0 && std::isfinite(theConfig.FTolerance)
         && theConfig.FTolerance >= 0.0;
}

//! Return true when bounds form a finite, non-empty ordered interval.
inline bool IsValidBounds(double theLower, double theUpper)
{
  return std::isfinite(theLower) && std::isfinite(theUpper) && theLower < theUpper;
}

//! Return true when two non-zero values have opposite signs without multiplying them.
inline bool HaveOppositeSigns(double theFirst, double theSecond)
{
  return std::signbit(theFirst) != std::signbit(theSecond);
}

//! Compute a midpoint without overflowing when the endpoints have extreme magnitudes.
inline double Midpoint(double theFirst, double theSecond)
{
  return 0.5 * theFirst + 0.5 * theSecond;
}

//! Compute how many periodic representatives starting at theFirst lie in an upper-bounded domain.
//! @param[in] theFirst first representative, already mapped to the domain lower bound
//! @param[in] theUpper domain upper bound
//! @param[in] thePeriod positive period
//! @param[in] theTolerance tolerance applied at the upper bound
//! @param[in] theExcludeUpperTolerance true to stop before upper-tolerance; false to include
//! upper+tolerance
//! @param[out] theCount number of representable periodic roots
//! @return false if the inputs, count, or floating-point step cannot be represented safely
inline bool ComputePeriodicRootCount(double  theFirst,
                                     double  theUpper,
                                     double  thePeriod,
                                     double  theTolerance,
                                     bool    theExcludeUpperTolerance,
                                     size_t& theCount)
{
  theCount = 0;
  if (!std::isfinite(theFirst) || !std::isfinite(theUpper) || !std::isfinite(thePeriod)
      || thePeriod <= 0.0 || !std::isfinite(theTolerance) || theTolerance < 0.0)
  {
    return false;
  }

  const double aLimit =
    theExcludeUpperTolerance ? theUpper - theTolerance : theUpper + theTolerance;
  if (!std::isfinite(aLimit))
  {
    return false;
  }
  if (theExcludeUpperTolerance ? theFirst >= aLimit : theFirst > aLimit)
  {
    return true;
  }

  const long double aSpan = static_cast<long double>(aLimit) - theFirst;
  const long double aCount =
    theExcludeUpperTolerance ? std::ceil(aSpan / thePeriod) : std::floor(aSpan / thePeriod) + 1.0L;
  if (aCount < 1.0L || aCount >= static_cast<long double>(static_cast<size_t>(-1)))
  {
    return false;
  }
  theCount     = static_cast<size_t>(aCount);
  double aLast = theFirst + static_cast<double>(theCount - 1) * thePeriod;
  if (theExcludeUpperTolerance ? aLast >= aLimit : aLast > aLimit)
  {
    --theCount;
  }
  const double aNext        = theFirst + static_cast<double>(theCount) * thePeriod;
  const bool   isNextInside = theExcludeUpperTolerance ? aNext < aLimit : aNext <= aLimit;
  if (theCount < static_cast<size_t>(-1) - 1 && isNextInside)
  {
    ++theCount;
  }
  if (theCount == 0)
  {
    return true;
  }
  aLast = theFirst + static_cast<double>(theCount - 1) * thePeriod;
  if (theCount < 2 || theFirst + thePeriod <= theFirst)
  {
    return theCount < 2;
  }

  const double aPrevious = theFirst + static_cast<double>(theCount - 2) * thePeriod;
  return std::isfinite(aLast) && aLast > aPrevious;
}

} // namespace Utils
} // namespace MathRoot

#endif // _MathRoot_Utils_HeaderFile

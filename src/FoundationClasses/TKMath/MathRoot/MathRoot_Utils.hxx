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

} // namespace Utils
} // namespace MathRoot

#endif // _MathRoot_Utils_HeaderFile

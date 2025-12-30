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

#ifndef _MathUtils_Domain_HeaderFile
#define _MathUtils_Domain_HeaderFile

#include <cmath>

//! @file MathUtils_Domain.hxx
//! @brief Parameter domain types for curves and surfaces.
//!
//! Provides lightweight value types for representing 1D and 2D parameter domains
//! with utility methods for bounds checking, clamping, and domain analysis.

namespace MathUtils
{

//! @brief 1D parameter domain for curves.
//!
//! Represents a parameter range [Min, Max] with utility methods for:
//! - Bounds checking (Contains)
//! - Parameter clamping (Clamp)
//! - Domain analysis (IsLarge, IsFullPeriod)
//!
//! @note This is a lightweight value type designed for efficiency.
//!       All methods are inline and constexpr where possible.
struct Domain1D
{
  double Min = 0.0; //!< Lower bound of the domain
  double Max = 0.0; //!< Upper bound of the domain

  //! Default constructor - creates empty domain [0, 0].
  constexpr Domain1D() = default;

  //! Construct from bounds.
  //! @param theMin lower bound
  //! @param theMax upper bound
  constexpr Domain1D(double theMin, double theMax)
      : Min(theMin),
        Max(theMax)
  {
  }

  //! Returns the length of the domain.
  constexpr double Length() const { return Max - Min; }

  //! Returns the midpoint of the domain.
  constexpr double Mid() const { return (Min + Max) * 0.5; }

  //! Check if value is within domain.
  //! @param theU parameter value to check
  //! @param theTol tolerance for boundary check
  //! @return true if theU is in [Min - theTol, Max + theTol]
  bool Contains(double theU, double theTol = 0.0) const
  {
    return theU >= Min - theTol && theU <= Max + theTol;
  }

  //! Clamp value to domain bounds.
  //! @param theU parameter value to clamp
  //! @return clamped value in [Min, Max]
  double Clamp(double theU) const { return theU < Min ? Min : (theU > Max ? Max : theU); }

  //! Check if domain is "large" (effectively unbounded for optimization).
  //! Large domains allow skipping bounds checking for performance.
  //! @param theThreshold size threshold (default 1000)
  //! @return true if Length() > theThreshold
  bool IsLarge(double theThreshold = 1000.0) const { return Length() > theThreshold; }

  //! Check if domain covers a full periodic range.
  //! @param thePeriod period of the parameter (e.g., 2*PI for angles)
  //! @param theTol tolerance
  //! @return true if domain covers at least one full period
  bool IsFullPeriod(double thePeriod, double theTol = 1.0e-10) const
  {
    return Length() >= thePeriod - theTol;
  }

  //! Interpolate within domain.
  //! @param theT interpolation parameter in [0, 1]
  //! @return Min + theT * Length()
  constexpr double Lerp(double theT) const { return Min + theT * (Max - Min); }

  //! Normalize parameter to [0, 1] range.
  //! @param theU parameter value
  //! @return (theU - Min) / Length(), or 0.5 if Length() == 0
  double Normalize(double theU) const
  {
    double aLen = Length();
    return aLen > 0.0 ? (theU - Min) / aLen : 0.5;
  }

  //! Check if domain has finite bounds (not effectively infinite).
  //! @param theInfLimit threshold for "infinity" (default 1e100)
  //! @return true if both Min and Max are within finite range
  bool IsFinite(double theInfLimit = 1.0e100) const
  {
    return Min > -theInfLimit && Max < theInfLimit;
  }

  //! Check if this domain equals another within tolerance.
  //! @param theOther domain to compare with
  //! @param theTol tolerance for comparison
  //! @return true if domains are equal within tolerance
  bool IsEqual(const Domain1D& theOther, double theTol = 1.0e-10) const
  {
    return std::abs(Min - theOther.Min) <= theTol && std::abs(Max - theOther.Max) <= theTol;
  }
};

//! @brief 2D parameter domain for surfaces.
//!
//! Represents a rectangular parameter domain [UMin, UMax] x [VMin, VMax]
//! with utility methods for:
//! - Bounds checking (Contains)
//! - Parameter clamping (Clamp)
//! - Domain analysis (IsLarge, IsFullPeriod)
//! - Access to U and V subdomains
//!
//! @note This is a lightweight value type designed for efficiency.
struct Domain2D
{
  double UMin = 0.0; //!< Lower U bound
  double UMax = 0.0; //!< Upper U bound
  double VMin = 0.0; //!< Lower V bound
  double VMax = 0.0; //!< Upper V bound

  //! Default constructor - creates empty domain.
  constexpr Domain2D() = default;

  //! Construct from bounds.
  //! @param theUMin lower U bound
  //! @param theUMax upper U bound
  //! @param theVMin lower V bound
  //! @param theVMax upper V bound
  constexpr Domain2D(double theUMin, double theUMax, double theVMin, double theVMax)
      : UMin(theUMin),
        UMax(theUMax),
        VMin(theVMin),
        VMax(theVMax)
  {
  }

  //! Construct from two 1D domains.
  //! @param theUDomain U parameter domain
  //! @param theVDomain V parameter domain
  constexpr Domain2D(const Domain1D& theUDomain, const Domain1D& theVDomain)
      : UMin(theUDomain.Min),
        UMax(theUDomain.Max),
        VMin(theVDomain.Min),
        VMax(theVDomain.Max)
  {
  }

  //! Returns the U subdomain.
  constexpr Domain1D U() const { return {UMin, UMax}; }

  //! Returns the V subdomain.
  constexpr Domain1D V() const { return {VMin, VMax}; }

  //! Returns U length.
  constexpr double ULength() const { return UMax - UMin; }

  //! Returns V length.
  constexpr double VLength() const { return VMax - VMin; }

  //! Returns U midpoint.
  constexpr double UMid() const { return (UMin + UMax) * 0.5; }

  //! Returns V midpoint.
  constexpr double VMid() const { return (VMin + VMax) * 0.5; }

  //! Check if UV point is within domain.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @param theTol tolerance for boundary check
  //! @return true if (theU, theV) is in domain
  bool Contains(double theU, double theV, double theTol = 0.0) const
  {
    return theU >= UMin - theTol && theU <= UMax + theTol && theV >= VMin - theTol
           && theV <= VMax + theTol;
  }

  //! Clamp UV to domain bounds.
  //! @param theU U parameter (modified in place)
  //! @param theV V parameter (modified in place)
  void Clamp(double& theU, double& theV) const
  {
    theU = theU < UMin ? UMin : (theU > UMax ? UMax : theU);
    theV = theV < VMin ? VMin : (theV > VMax ? VMax : theV);
  }

  //! Check if both U and V domains are "large".
  //! @param theThreshold size threshold (default 1000)
  //! @return true if both U and V lengths exceed threshold
  bool IsLarge(double theThreshold = 1000.0) const
  {
    return ULength() > theThreshold && VLength() > theThreshold;
  }

  //! Check if U domain covers a full period.
  //! @param thePeriod period of U parameter
  //! @param theTol tolerance
  bool IsUFullPeriod(double thePeriod, double theTol = 1.0e-10) const
  {
    return ULength() >= thePeriod - theTol;
  }

  //! Check if V domain covers a full period.
  //! @param thePeriod period of V parameter
  //! @param theTol tolerance
  bool IsVFullPeriod(double thePeriod, double theTol = 1.0e-10) const
  {
    return VLength() >= thePeriod - theTol;
  }

  //! Check if domain has finite bounds (not effectively infinite).
  //! @param theInfLimit threshold for "infinity" (default 1e100)
  bool IsFinite(double theInfLimit = 1.0e100) const
  {
    return UMin > -theInfLimit && UMax < theInfLimit && VMin > -theInfLimit && VMax < theInfLimit;
  }
};

} // namespace MathUtils

#endif // _MathUtils_Domain_HeaderFile

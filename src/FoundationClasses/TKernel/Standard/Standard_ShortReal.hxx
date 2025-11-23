// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Standard_ShortReal_HeaderFile
#define _Standard_ShortReal_HeaderFile

#include <algorithm>
#include <cmath>
#include <climits>
#include <float.h>

#include <Standard_TypeDef.hxx>

//! Returns the minimum positive float value.
[[nodiscard]] constexpr float ShortRealSmall()
{
  return FLT_MIN;
}

//! Returns the absolute value of a float @p Value.
//! Equivalent to std::abs.
Standard_DEPRECATED("This function duplicates std::abs and will be removed in future releases. "
                    "Use std::abs instead.")
inline float Abs(const float theValue)
{
  return std::abs(theValue);
}

//! Returns the number of digits of precision in a float.
[[nodiscard]] constexpr int ShortRealDigits()
{
  return FLT_DIG;
}

//! Returns the minimum positive float such that 1.0f + ShortRealEpsilon() != 1.0f.
[[nodiscard]] constexpr float ShortRealEpsilon()
{
  return FLT_EPSILON;
}

//! Returns the minimum negative value of a float.
[[nodiscard]] constexpr float ShortRealFirst()
{
  return -FLT_MAX;
}

//! Returns the minimum value of exponent(base 10) of a float.
[[nodiscard]] constexpr int ShortRealFirst10Exp()
{
  return FLT_MIN_10_EXP;
}

//! Returns the maximum value of a float.
[[nodiscard]] constexpr float ShortRealLast()
{
  return FLT_MAX;
}

//! Returns the maximum value of exponent(base 10) of a float.
[[nodiscard]] constexpr int ShortRealLast10Exp()
{
  return FLT_MAX_10_EXP;
}

//! Returns the mantissa (number of bits in the significand) of a float.
[[nodiscard]] constexpr int ShortRealMantissa()
{
  return FLT_MANT_DIG;
}

//! Returns the radix (base) of a float.
[[nodiscard]] constexpr int ShortRealRadix()
{
  return FLT_RADIX;
}

//! Returns the size in bits of a float.
[[nodiscard]] constexpr int ShortRealSize()
{
  return CHAR_BIT * sizeof(float);
}

//! Returns the maximum value of two floats.
//! Equivalent to std::max.
Standard_DEPRECATED("This function duplicates std::max and will be removed in future releases. "
                    "Use std::max instead.")
constexpr float Max(const float theValue1, const float theValue2)
{
  return (std::max)(theValue1, theValue2);
}

//! Returns the minimum value of two floats.
//! Equivalent to std::min.
Standard_DEPRECATED("This function duplicates std::min and will be removed in future releases. "
                    "Use std::min instead.")
constexpr float Min(const float theValue1, const float theValue2)
{
  return (std::min)(theValue1, theValue2);
}

#endif // _Standard_ShortReal_HeaderFile

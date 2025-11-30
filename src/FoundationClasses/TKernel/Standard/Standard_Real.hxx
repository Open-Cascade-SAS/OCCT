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

#ifndef _Standard_Real_HeaderFile
#define _Standard_Real_HeaderFile

#include <algorithm>
#include <cmath>
#include <climits>
#include <float.h>

#ifdef _MSC_VER
  #ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
  #endif
  #include <math.h>
#endif

#include <Standard_TypeDef.hxx>

//! Returns the value of the arc cosine of a @p theValue.
Standard_DEPRECATED("This function duplicates std::acos and will be removed in future releases. "
                    "Use std::acos instead.")
inline double ACos(const double theValue)
{
  return std::acos(theValue);
}

//! Returns the approximate value of the arc cosine @p theValue.
//! The max error is about 1 degree near Value=0.
//! NOTE: Avoid using this function in new code, it presumably slower then std::acos.
Standard_DEPRECATED("Deprecated, use std::acos instead")
Standard_EXPORT double ACosApprox(const double theValue);

//! Returns the value of the arc sine of a @p theValue.
Standard_DEPRECATED("This function duplicates std::asin and will be removed in future releases. "
                    "Use std::asin instead.")
inline double ASin(const double theValue)
{
  return std::asin(theValue);
}

//! Computes the arc tangent of @p theX divided by @p theY using the signs of both
//! arguments to determine the quadrant of the return value.
Standard_DEPRECATED("This function duplicates std::atan2 and will be removed in future releases. "
                    "Use std::atan2 instead.")
inline double ATan2(const double theX, const double theY)
{
  return std::atan2(theX, theY);
}

//! Returns the value of the hyperbolic arc tangent of @p theValue.
Standard_DEPRECATED("This function duplicates std::atanh and will be removed in future releases. "
                    "Use std::atanh instead.")
inline double ATanh(const double theValue)
{
  return std::atanh(theValue);
}

//! Returns the value of the hyperbolic arc cosine of @p theValue.
Standard_DEPRECATED("This function duplicates std::acosh and will be removed in future releases. "
                    "Use std::acosh instead.")
inline double ACosh(const double theValue)
{
  return std::acosh(theValue);
}

//! Returns the hyperbolic cosine of a double @p theValue.
Standard_DEPRECATED("This function duplicates std::cosh and will be removed in future releases. "
                    "Use std::cosh instead.")
inline double Cosh(const double theValue)
{
  return std::cosh(theValue);
}

//! Returns the hyperbolic sine of a double @p theValue.
Standard_DEPRECATED("This function duplicates std::sinh and will be removed in future releases. "
                    "Use std::sinh instead.")
inline double Sinh(const double theValue)
{
  return std::sinh(theValue);
}

//! Computes the natural (base-e) logarithm of number @p theValue.
Standard_DEPRECATED("This function duplicates std::log and will be removed in future releases. "
                    "Use std::log instead.")
inline double Log(const double theValue)
{
  return std::log(theValue);
}

//! Returns the square root of a double @p theValue.
Standard_DEPRECATED("This function duplicates std::sqrt and will be removed in future releases. "
                    "Use std::sqrt instead.")
inline double Sqrt(const double theValue)
{
  return std::sqrt(theValue);
}

//! Returns the next representable value of a double @p theValue
//! in the direction of @p theDirection. Equivalent to std::nextafter.
Standard_DEPRECATED(
  "This function duplicates std::nextafter and will be removed in future releases. "
  "Use std::nextafter instead.")
inline double NextAfter(const double theValue, const double theDirection)
{
  return std::nextafter(theValue, theDirection);
}

//! Composes a floating point value with the magnitude of @p theMagnitude
//! and the sign of @p theSign. Equivalent to std::copysign.
Standard_DEPRECATED(
  "This function duplicates std::copysign and will be removed in future releases. "
  "Use std::copysign instead.")
inline double Sign(const double theMagnitude, const double theSign)
{
  return std::copysign(theMagnitude, theSign);
}

//! Returns the minimum positive double value greater than zero.
[[nodiscard]] constexpr double RealSmall()
{
  return DBL_MIN;
}

//! Returns the absolute value of a double @p Value.
//! Equivalent to std::abs.
Standard_DEPRECATED("This function duplicates std::abs and will be removed in future releases. "
                    "Use std::abs instead.")
inline double Abs(const double theValue)
{
  return std::abs(theValue);
}

//! Returns Standard_True if two doubles are equal within the precision
//! defined by RealSmall().
[[nodiscard]] inline bool IsEqual(const double theValue1, const double theValue2)
{
  return std::abs(theValue1 - theValue2) < RealSmall();
}

//! Returns the number of digits of precision in a double.
[[nodiscard]] constexpr int RealDigits()
{
  return DBL_DIG;
}

//! Returns the minimum positive double such that
//! 1.0 + RealEpsilon() != 1.0.
[[nodiscard]] constexpr double RealEpsilon()
{
  return DBL_EPSILON;
}

//! Returns the minimum value of a double.
[[nodiscard]] constexpr double RealFirst()
{
  return -DBL_MAX;
}

//! Returns the minimum value of exponent(base 10) of a double.
[[nodiscard]] constexpr int RealFirst10Exp()
{
  return DBL_MIN_10_EXP;
}

//! Returns the maximum value of a double.
[[nodiscard]] constexpr double RealLast()
{
  return DBL_MAX;
}

//! Returns the maximum value of exponent(base 10) of a double.
[[nodiscard]] constexpr int RealLast10Exp()
{
  return DBL_MAX_10_EXP;
}

//! Returns the size in bits of the mantissa part of a double.
[[nodiscard]] constexpr int RealMantissa()
{
  return DBL_MANT_DIG;
}

//! Returns the radix of a double.
[[nodiscard]] constexpr int RealRadix()
{
  return FLT_RADIX;
}

//! Returns the size in bits of a double.
[[nodiscard]] constexpr int RealSize()
{
  return CHAR_BIT * sizeof(double);
}

//! Converts a int @p theValue to a double.
[[nodiscard]] constexpr double IntToReal(const int theValue)
{
  return theValue;
}

//! Returns the value of the arc tangent of a double @p theValue.
Standard_DEPRECATED("This function duplicates std::atan and will be removed in future releases. "
                    "Use std::atan instead.")
inline double ATan(const double theValue)
{
  return std::atan(theValue);
}

//! Returns the next integer greater than or equal to a double @p theValue.
Standard_DEPRECATED("This function duplicates std::ceil and will be removed in future releases. "
                    "Use std::ceil instead.")
inline double Ceiling(const double theValue)
{
  return std::ceil(theValue);
}

//! Returns the cosine of a double @p theValue.
//! Equivalent to std::cos.
Standard_DEPRECATED("This function duplicates std::cos and will be removed in future releases. "
                    "Use std::cos instead.")
inline double Cos(const double theValue)
{
  return std::cos(theValue);
}

//! The function returns absolute value of difference between @p theValue and other nearest value of
//! double type. Nearest value is chosen in direction of infinity the same sign as @p theValue.
//! If @p theValue is 0 then returns minimal positive value of double type.
[[nodiscard]] inline double Epsilon(const double theValue)
{
  return theValue >= 0.0 ? (std::nextafter(theValue, RealLast()) - theValue)
                         : (theValue - std::nextafter(theValue, RealFirst()));
}

//! Returns the exponential of a double @p theValue.
//! Equivalent to std::exp.
Standard_DEPRECATED("This function duplicates std::exp and will be removed in future releases. "
                    "Use std::exp instead.")
inline double Exp(const double theValue)
{
  return std::exp(theValue);
}

//! Returns the nearest integer less than or equal to a double @p theValue.
//! Equivalent to std::floor.
Standard_DEPRECATED("This function duplicates std::floor and will be removed in future releases. "
                    "Use std::floor instead.")
inline double Floor(const double theValue)
{
  return std::floor(theValue);
}

//! Returns the integer part of a double @p theValue.
//! Equivalent to std::trunc.
Standard_DEPRECATED("This function duplicates std::trunc and will be removed in future releases. "
                    "Use std::trunc instead.")
inline double IntegerPart(const double theValue)
{
  return std::trunc(theValue);
}

//! Returns the logarithm to base 10 of a double @p theValue.
//! Equivalent to std::log10.
Standard_DEPRECATED("This function duplicates std::log10 and will be removed in future releases. "
                    "Use std::log10 instead.")
inline double Log10(const double theValue)
{
  return std::log10(theValue);
}

//! Returns the maximum value of two doubles.
//! Equivalent to std::max.
Standard_DEPRECATED("This function duplicates std::max and will be removed in future releases. "
                    "Use std::max instead.")
constexpr double Max(const double theValue1, const double theValue2)
{
  return (std::max)(theValue1, theValue2);
}

//! Returns the minimum value of two doubles.
//! Equivalent to std::min.
Standard_DEPRECATED("This function duplicates std::min and will be removed in future releases. "
                    "Use std::min instead.")
constexpr double Min(const double theValue1, const double theValue2)
{
  return (std::min)(theValue1, theValue2);
}

//! Returns a double @p theValue raised to the power of @p thePower.
Standard_DEPRECATED("This function duplicates std::pow and will be removed in future releases. "
                    "Use std::pow instead.")
inline double Pow(const double theValue, const double thePower)
{
  return std::pow(theValue, thePower);
}

//! Returns the fractional part of a double @p theValue.
//! Always non-negative.
[[nodiscard]] inline double RealPart(const double theValue)
{
  return std::abs(std::trunc(theValue) - theValue);
}

//! Converts a double @p theValue to the nearest valid int.
//! If input value is out of valid range for int, minimal or maximal possible int is returned.
[[nodiscard]] constexpr int RealToInt(const double theValue)
{
  // Note that on WNT under MS VC++ 8.0 conversion of double value less
  // than INT_MIN or greater than INT_MAX to integer will cause signal
  // "Floating point multiple trap" (OCC17861)
  return theValue < static_cast<double>(INT_MIN)
           ? static_cast<int>(INT_MIN)
           : (theValue > static_cast<double>(INT_MAX) ? static_cast<int>(INT_MAX)
                                                      : static_cast<int>(theValue));
}

//! Converts a double @p theValue to the nearest valid float.
//! If input value is out of valid range for float, minimal or maximal
//! possible float is returned.
[[nodiscard]] constexpr float RealToShortReal(const double theValue)
{
  return theValue < -FLT_MAX  ? -FLT_MAX
         : theValue > FLT_MAX ? FLT_MAX
                              : static_cast<float>(theValue);
}

//! Returns the nearest integer of a double @p theValue.
//! Equivalent to std::round.
Standard_DEPRECATED("This function duplicates std::round and will be removed in future releases. "
                    "Use std::round instead.")
inline double Round(const double theValue)
{
  return std::round(theValue);
}

//! Returns the sine of a double @p theValue.
//! Equivalent to std::sin.
Standard_DEPRECATED("This function duplicates std::sin and will be removed in future releases. "
                    "Use std::sin instead.")
inline double Sin(const double theValue)
{
  return std::sin(theValue);
}

//! Returns the hyperbolic arc sine of a double @p theValue.
//! Equivalent to std::asinh.
Standard_DEPRECATED("This function duplicates std::asinh and will be removed in future releases. "
                    "Use std::asinh instead.")
inline double ASinh(const double theValue)
{
  return std::asinh(theValue);
}

//! Returns the square of a double @p theValue.
[[nodiscard]] constexpr double Square(const double theValue)
{
  return theValue * theValue;
}

//! Returns the tangent of a double @p theValue.
//! Equivalent to std::tan.
Standard_DEPRECATED("This function duplicates std::tan and will be removed in future releases. "
                    "Use std::tan instead.")
inline double Tan(const double theValue)
{
  return std::tan(theValue);
}

//! Returns the hyperbolic tangent of a double @p theValue.
//! Equivalent to std::tanh.
Standard_DEPRECATED("This function duplicates std::tanh and will be removed in future releases. "
                    "Use std::tanh instead.")
inline double Tanh(const double theValue)
{
  return std::tanh(theValue);
}

#endif // _Standard_Real_HeaderFile

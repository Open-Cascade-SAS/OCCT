// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _Standard_Integer_HeaderFile
#define _Standard_Integer_HeaderFile

#include <Standard_Std.hxx>
#include <Standard_TypeDef.hxx>

#include <algorithm>
#include <climits>
#include <cmath>

//! Returns the absolute value of a int @p Value.
//! Equivalent to std::abs.
[[nodiscard]] constexpr int Abs(const int theValue)
{
  return theValue < 0 ? -theValue : theValue;
}

//! Returns true if @p theValue is even.
[[nodiscard]] constexpr bool IsEven(const int theValue)
{
  return theValue % 2 == 0;
}

//! Returns true if @p theValue is odd.
[[nodiscard]] constexpr bool IsOdd(const int theValue)
{
  return theValue % 2 == 1;
}

//! Returns the maximum value of two integers.
//! Equivalent to std::max.
Standard_DEPRECATED("This function duplicates std::max and will be removed in future releases. "
                    "Use std::max instead.")
constexpr int Max(const int theValue1, const int theValue2)
{
  return (std::max)(theValue1, theValue2);
}

//! Returns the minimum value of two integers.
//! Equivalent to std::min.
Standard_DEPRECATED("This function duplicates std::min and will be removed in future releases. "
                    "Use std::min instead.")
constexpr int Min(const int theValue1, const int theValue2)
{
  return (std::min)(theValue1, theValue2);
}

//! Returns the modulus of @p theValue by @p theDivisor.
[[nodiscard]] constexpr int Modulus(const int theValue, const int theDivisor)
{
  return theValue % theDivisor;
}

//! Returns the square of a int @p theValue.
//! Note that behavior is undefined in case of overflow.
[[nodiscard]] constexpr int Square(const int theValue)
{
  return theValue * theValue;
}

//! Returns the minimum value of an integer.
[[nodiscard]] constexpr int IntegerFirst()
{
  return INT_MIN;
}

//! Returns the maximum value of an integer.
[[nodiscard]] constexpr int IntegerLast()
{
  return INT_MAX;
}

//! Returns the size in bits of an integer.
[[nodiscard]] constexpr int IntegerSize()
{
  return CHAR_BIT * sizeof(int);
}

#endif // _Standard_Integer_HeaderFile

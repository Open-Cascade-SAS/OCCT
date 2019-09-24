// Created on: 2006-08-22
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef _Standard_Size_HeaderFile
#define _Standard_Size_HeaderFile

#include <Standard_Integer.hxx>

// msv 26.05.2009: add HashCode and IsEqual functions

//! Computes a hash code for the given value of the Standard_Size type, in the range [1, theUpperBound]
//! @tparam TheSize the type of the given value (it is Standard_Size,
//! and must not be the same as "unsigned int", because the overload of the HashCode function
//! for "unsigned int" type is already presented in Standard_Integer.hxx)
//! @param theValue the value of the Standard_Size type which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
template <typename TheSize>
typename opencascade::std::enable_if<!opencascade::std::is_same<Standard_Size, unsigned int>::value
                                       && opencascade::std::is_same<TheSize, Standard_Size>::value,
                                     Standard_Integer>::type
HashCode (const TheSize theValue, const Standard_Integer theUpperBound)
{
  Standard_Size aKey = ~theValue + (theValue << 18);
  aKey ^= (aKey >> 31);
  aKey *= 21;
  aKey ^= (aKey >> 11);
  aKey += (aKey << 6);
  aKey ^= (aKey >> 22);
  return IntegerHashCode(aKey, IntegerLast(), theUpperBound);
}

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two values are equal
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual(const Standard_Size One,
                                const Standard_Size Two)
{
  return One == Two;
}

#endif

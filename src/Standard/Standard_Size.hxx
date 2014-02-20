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

// msv 26.05.2009: add HashCode and IsEqual functions

// ------------------------------------------------------------------
// Hascode : Computes a hashcoding value for a given value
// ------------------------------------------------------------------
inline Standard_Integer HashCode(const Standard_Size Val,
                                 const Standard_Integer Upper)
{
  Standard_Size aKey = ~Val + (Val << 18);
  aKey ^= (aKey >> 31);
  aKey *= 21;
  aKey ^= (aKey >> 11);
  aKey += (aKey << 6);
  aKey ^= (aKey >> 22);
  return (Standard_Integer(aKey & 0x7fffffff) % Upper) + 1;
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

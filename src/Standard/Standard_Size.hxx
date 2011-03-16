// File:      Standard_Size.hxx
// Created:   22.08.06 19:14:29
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


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

// Created on: 2006-08-22
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

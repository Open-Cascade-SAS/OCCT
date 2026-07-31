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

#ifndef _Standard_Character_HeaderFile
#define _Standard_Character_HeaderFile

#include <Standard_TypeDef.hxx>

#include <cctype>

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two characters have the same value
// ------------------------------------------------------------------
constexpr bool IsEqual(const char One, const char Two)
{
  return One == Two;
}

// ===============================================
// Character classification functions
//
// NOTE: Character classification routines in C standard library
// (isdigit(), isalpha() etc.) have integer argument instead of char.
// Therefore if character from extended Ascii part of char table
// (i.e. above 128) is passed into such functions it is converted
// to int giving negative value (if characters are signed that
// is default for most compilers).
// It can be dangerous since implementation of these C functions
// may use argument as index in the array without any checks
// (for instance, in Microsoft VC++ -- see MSDN)
// To avoid this, we cast char to unsigned char when passing to these functions.
// ===============================================

// ==================================================================
// IsAlphabetic : Returns Standard_True if a character is alphabetic
// ==================================================================
inline bool IsAlphabetic(const char me)
{
  return std::isalpha((unsigned char)me) != 0;
}

// ==================================================================
// IsDigit : Returns Standard_True if a character is a digit
// ==================================================================
inline bool IsDigit(const char me)
{
  return std::isdigit((unsigned char)me) != 0;
}

// ==================================================================
// IsXDigit : Returns Standard_True if a character is a digit
// ==================================================================
inline bool IsXDigit(const char me)
{
  return std::isxdigit((unsigned char)me) != 0;
}

// ==================================================================
// IsAlphanumeric : Returns Standard_True if a character is alphanumeric
// ==================================================================
inline bool IsAlphanumeric(const char me)
{
  return (IsAlphabetic(me) || IsDigit(me));
}

// ==================================================================
// IsControl : Returns Standard_True if a character  is a control character
// ==================================================================
inline bool IsControl(const char me)
{
  return std::iscntrl((unsigned char)me) != 0;
}

// ==================================================================
// IsGraphic : Returns Standard_True if a character is graphic
// ==================================================================
inline bool IsGraphic(const char me)
{
  return std::isgraph((unsigned char)me) != 0;
}

// ==================================================================
// IsLowerCase : Returns Standard_True if a character is lowercase
// ==================================================================
inline bool IsLowerCase(const char me)
{
  return std::islower((unsigned char)me) != 0;
}

// ==================================================================
// IsPrintable : Returns Standard_True if a character is printable
// ==================================================================
inline bool IsPrintable(const char me)
{
  return std::isprint((unsigned char)me) != 0;
}

// ==================================================================
// IsPunctuation : Returns Standard_True if a character is a graphic and
//                 not a alphanumeric character
// ==================================================================
inline bool IsPunctuation(const char me)
{
  return (IsGraphic(me) && !IsAlphanumeric(me));
}

// ==================================================================
// IsSpace : Returns Standard_True if a character is a space
// ==================================================================
inline bool IsSpace(const char me)
{
  return std::isspace((unsigned char)me) != 0;
}

// ==================================================================
// IsUppercase : Returns Standard_True if a character is uppercase
// ==================================================================
inline bool IsUpperCase(const char me)
{
  return std::isupper((unsigned char)me) != 0;
}

// ==================================================================
// LowerCase : Returns a lowercase character
// ==================================================================
inline char LowerCase(const char me)
{
  return (char)(unsigned char)std::tolower((unsigned char)me);
}

// ==================================================================
// UpperCase : Returns a uppercase character
// ==================================================================
inline char UpperCase(const char me)
{
  return (char)(unsigned char)std::toupper((unsigned char)me);
}

#endif
